#include <cctype>
#include <map>
#include <set>
#include "frontends/cxx/semantic/analysis.hpp"
#include "core/ir/model.hpp"

namespace cjm::semantic {

namespace {

std::string trim(const std::string& text) {
    std::size_t first = 0;
    while (first < text.size() &&
           std::isspace(static_cast<unsigned char>(text[first]))) {
        ++first;
    }

    std::size_t last = text.size();
    while (last > first &&
           std::isspace(static_cast<unsigned char>(text[last - 1]))) {
        --last;
    }

    return text.substr(first, last - first);
}

std::vector<std::string> split_options(const std::string& text) {
    std::vector<std::string> result;
    std::size_t start = 0;

    while (start <= text.size()) {
        const auto comma = text.find(",", start);
        if (comma == std::string::npos) {
            result.push_back(trim(text.substr(start)));
            break;
        }

        result.push_back(trim(text.substr(start, comma - start)));
        start = comma + 1;
    }
    return result;
}

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.rfind(prefix, 0) == 0;
}

bool ends_with(const std::string& text, const std::string& suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(), suffix.size(), suffix) ==
               0;
}

Diagnostic make_diagnostic(const SourceLocation& comment_location,
                           const std::string& message) {
    Diagnostic diagnostic;
    diagnostic.location = comment_location;
    diagnostic.message = message;
    return diagnostic;
}

/*
 * Build a qualified candidate name from the first namespace_count namespace
 * components and an unqualified type name.
 *
 * This supports parent namespace lookup. For a field declared in
 * company::model::detail, semantic analysis should try candidates such as:
 *
 *   company::model::detail::Address
 *   company::model::Address
 *   company::Address
 *
 * This helper only constructs one candidate name. It does not decide whether
 * the candidate exists and does not perform type classification.
 */

std::string
join_qualified_name_prefix(const std::vector<std::string>& namespace_path,
                           std::size_t namespace_count,
                           const std::string& name) {
    std::string result;
    for (auto i = 0; i < namespace_count; ++i) {
        const auto& namespace_name = namespace_path[i];
        if (!result.empty()) {
            result += "::";
        }
        result += namespace_name;
    }
    if (!result.empty()) {
        result += "::";
    }
    result += name;
    return result;
}

std::string join_qualified_name(const std::vector<std::string>& namespace_path,
                                const std::string& name) {
    std::string result;
    for (const auto& namespace_name : namespace_path) {
        if (!result.empty()) {
            result += "::";
        }
        result += namespace_name;
    }
    if (!result.empty()) {
        result += "::";
    }
    result += name;
    return result;
}

std::string strip_global_prefix(const std::string& spelling) {
    if (starts_with(spelling, "::")) {
        return spelling.substr(2);
    }
    return spelling;
}

std::string strip_cv_ref(const std::string& spelling) {
    std::string result = trim(spelling);
    if (starts_with(result, "const ")) {
        result = trim(result.substr(6));
    }
    if (ends_with(result, "&")) {
        result = trim(result.substr(0, result.size() - 1));
    }
    return result;
}

bool parse_template_argument(const std::string& spelling,
                             const std::string& prefix, std::string& argument) {
    if (!starts_with(spelling, prefix + "<") || !ends_with(spelling, ">")) {
        return false;
    }

    argument = trim(spelling.substr(prefix.size() + 1,
                                    spelling.size() - prefix.size() - 2));
    return !argument.empty();
}

bool parse_two_template_arguments(const std::string& spelling,
                                  const std::string& prefix, std::string& first,
                                  std::string& second) {
    if (!starts_with(spelling, prefix + "<") || !ends_with(spelling, ">")) {
        return false;
    }

    auto body = trim(spelling.substr(prefix.size() + 1,
                                     spelling.size() - prefix.size() - 2));
    if (body.empty()) {
        return false;
    }

    const auto comma = body.find(",");
    if (comma == std::string::npos) {
        return false;
    }

    first = trim(body.substr(0, comma));
    second = trim(body.substr(comma + 1));
    return !first.empty() && !second.empty();
}

metadata::SourceLocation
to_metadata_location(const parser::SourceLocation& location) {
    metadata::SourceLocation result;
    result.file = location.file;
    result.line = static_cast<std::uint32_t>(location.line);
    result.column = static_cast<std::uint32_t>(location.column);
    return result;
}

struct TypeSymbols {
    std::set<std::string> generated_types;
    std::set<std::string> enums;
    std::map<std::string, std::string> aliases;
};

struct FieldAnalysisResult {
    metadata::FieldModel field;
    bool include = false; // Add field to type.fields when true
    bool success = true;
};

metadata::FieldType make_type(metadata::FieldTypeKind kind,
                              const std::string& spelling,
                              const std::string& qualified_name) {
    metadata::FieldType type;
    type.kind = kind;
    type.spelling = spelling;
    type.qualified_name = qualified_name;
    return type;
}

SourceLocation to_semantic_location(const parser::SourceLocation& location) {
    SourceLocation result;
    result.file = location.file;
    result.line = location.line;
    result.column = location.column;
    return result;
}

Diagnostic make_diagnostic(const parser::SourceLocation& location,
                           const std::string& message) {
    Diagnostic diagnostic;
    diagnostic.location = to_semantic_location(location);
    diagnostic.message = message;
    return diagnostic;
}

/**
 * Look up once alias target for a type spelling in the current semantic scope.
 *
 * Inputs:
 *   symbols        - Alias table collected from parser syntax.
 *   namespace_path - Namespace containing the field declaration
 *   spelling       - Type spelling to lookup after cv/ref
 * normalization.
 *
 * Output:
 *   Returns the alias target spelling if the spelling names an alias.
 *   Returns an empty string if no alias is found.
 *
 * This function performs lookup once. It does not recursively resolve aliases
 * and does not classify whether the final type is JSON-mappable.
 */

std::string lookup_alias_once(const TypeSymbols& symbols,
                              const std::vector<std::string>& namespace_path,
                              const std::string& spelling) {
    const auto normalized = strip_global_prefix(strip_cv_ref(spelling));
    auto scoped_spelling = join_qualified_name(namespace_path, normalized);
    const auto scoped = symbols.aliases.find(scoped_spelling);
    if (scoped != symbols.aliases.end()) {
        return scoped->second;
    }

    const auto direct = symbols.aliases.find(normalized);
    if (direct != symbols.aliases.end()) {
        return direct->second;
    }

    return {};
}

/**
 * Resolve alias chains such as Id -> UserId -> int.
 *
 * Inputs:
 *  symbols         - Alias table collected from parser syntax.
 *  namespace_path  - Namespace containing the field declaration.
 *  spelling        - Original field type spelling.
 *  diagnostic      - Receives a diagnostic if an alias cycle is detected.
 *  success         - Set to false if alias resolution fails.
 *
 * Output:
 *  Returns the final non-alias spelling when resolution succeeds.
 *
 *  This function resolves names only. It does not decide whether the resolved
 *  type is supported by JSON mapping.
 */
std::string resolve_alias(const TypeSymbols& symbols,
                          const std::vector<std::string>& namespace_path,
                          const std::string& spelling,
                          std::vector<Diagnostic>& diagnostics, bool& success) {
    std::set<std::string> seen;
    std::string current = strip_global_prefix(strip_cv_ref(spelling));

    while (true) {
        if (seen.count(current) != 0) {
            success = false;
            Diagnostic diagnostic;
            diagnostic.message = "cyclic type alias detected: " + current;
            diagnostics.push_back(diagnostic);

            return current;
        }
        seen.insert(current);

        const auto next = lookup_alias_once(symbols, namespace_path, current);
        if (next.empty()) {
            return current;
        }
        current = strip_global_prefix(strip_cv_ref(next));
    }
}

std::string resolve_named_type(const std::set<std::string>& names,
                               const std::vector<std::string>& namespace_path,
                               const std::string& spelling) {

    const auto normalized = strip_global_prefix(strip_cv_ref(spelling));

    for (auto namespace_count = namespace_path.size(); namespace_count > 0;
         --namespace_count) {
        const auto scoped = join_qualified_name_prefix(
            namespace_path, namespace_count, normalized);
        if (names.count(scoped) != 0) {
            return scoped;
        }
    }

    if (names.count(normalized) != 0) {
        return normalized;
    }
    return {};
}
// analyze_field_type dispatches map templates here; this helper recursively
// classifies map key/value types through analyze_field_type.
metadata::FieldType analyze_map_field_type(
    const TypeSymbols& symbols, const std::vector<std::string>& namespace_path,
    const parser::FieldSyntax& field, const std::string& original_spelling,
    const std::string& qualified_name, const std::string& key_arg,
    const std::string& value_arg, std::vector<Diagnostic>& diagnostics,
    bool& success);

metadata::FieldType
analyze_field_type(const TypeSymbols& symbols,
                   const std::vector<std::string>& namespace_path,
                   const parser::FieldSyntax& field,
                   std::vector<Diagnostic>& diagnostics, bool& success) {

    const auto original_spelling = strip_cv_ref(field.type_spelling);
    const auto spelling = resolve_alias(
        symbols, namespace_path, original_spelling, diagnostics, success);
    if (!success) {
        return make_type(metadata::FieldTypeKind::UserDefined,
                         original_spelling, "");
    }

    static const std::set<std::string> signed_integers = {
        "char",          "signed char", "short",        "short int",
        "int",           "long",        "long int",     "long long",
        "long long int", "std::int8_t", "std::int16_t", "std::int32_t",
        "std::int64_t"};
    static const std::set<std::string> unsigned_integers = {
        "unsigned char",     "unsigned short",     "unsigned short int",
        "unsigned int",      "unsigned",           "unsigned long",
        "unsigned long int", "unsigned long long", "unsigned long long int",
        "std::uint8_t",      "std::uint16_t",      "std::uint32_t",
        "std::uint64_t",     "std::size_t"};
    static const std::set<std::string> floating_points = {"float", "double",
                                                          "long double"};

    if (spelling == "bool") {
        return make_type(metadata::FieldTypeKind::Bool, original_spelling,
                         "bool");
    }

    if (signed_integers.count(spelling) != 0) {
        return make_type(metadata::FieldTypeKind::SignedInteger,
                         original_spelling, spelling);
    }

    if (unsigned_integers.count(spelling) != 0) {
        return make_type(metadata::FieldTypeKind::UnsignedInteger,
                         original_spelling, spelling);
    }

    if (floating_points.count(spelling) != 0) {
        return make_type(metadata::FieldTypeKind::FloatingPoint,
                         original_spelling, spelling);
    }

    if (spelling == "std::string") {
        return make_type(metadata::FieldTypeKind::String, original_spelling,
                         "std::string");
    }

    std::string argument;
    if (parse_template_argument(spelling, "std::vector", argument)) {
        auto type =
            make_type(metadata::FieldTypeKind::Vector, original_spelling, "");
        parser::FieldSyntax nested = field;
        nested.type_spelling = argument;
        type.arguments.push_back(analyze_field_type(
            symbols, namespace_path, nested, diagnostics, success));
        return type;
    }

    if (parse_template_argument(spelling, "std::optional", argument)) {
        auto type =
            make_type(metadata::FieldTypeKind::Optional, original_spelling, "");
        parser::FieldSyntax nested = field;
        nested.type_spelling = argument;
        type.arguments.push_back(analyze_field_type(
            symbols, namespace_path, nested, diagnostics, success));
        return type;
    }

    std::string key_arg, value_arg;
    if (parse_two_template_arguments(spelling, "std::map", key_arg,
                                     value_arg)) {
        return analyze_map_field_type(symbols, namespace_path, field,
                                      original_spelling, "std::map", key_arg,
                                      value_arg, diagnostics, success);
    }
    if (parse_two_template_arguments(spelling, "std::unordered_map", key_arg,
                                     value_arg)) {
        return analyze_map_field_type(symbols, namespace_path, field,
                                      original_spelling, "std::unordered_map",
                                      key_arg, value_arg, diagnostics, success);
    }

    const auto enum_name =
        resolve_named_type(symbols.enums, namespace_path, spelling);
    if (!enum_name.empty()) {
        return make_type(metadata::FieldTypeKind::Enum, original_spelling,
                         enum_name);
    }

    const auto user_type =
        resolve_named_type(symbols.generated_types, namespace_path, spelling);
    if (!user_type.empty()) {
        return make_type(metadata::FieldTypeKind::UserDefined,
                         original_spelling, user_type);
    }

    success = false;
    diagnostics.push_back(make_diagnostic(
        field.location,
        "unsupported field type for JSON mapping: " + field.type_spelling));
    return make_type(metadata::FieldTypeKind::UserDefined, original_spelling,
                     "");
}

// Build a Map FieldType from already-split key/value template arguments.
// This performs semantic classification only; it does not generate code.
metadata::FieldType analyze_map_field_type(
    const TypeSymbols& symbols, const std::vector<std::string>& namespace_path,
    const parser::FieldSyntax& field, const std::string& original_spelling,
    const std::string& qualified_name, const std::string& key_arg,
    const std::string& value_arg, std::vector<Diagnostic>& diagnostics,
    bool& success) {
    // 1. Classify the key and value types through the normal field-type path.
    auto type = make_type(metadata::FieldTypeKind::Map, original_spelling,
                          qualified_name);

    // 2. JSON object keys are string-only in v0.3
    parser::FieldSyntax key_field = field;
    key_field.type_spelling = key_arg;
    auto key_type = analyze_field_type(symbols, namespace_path, key_field,
                                       diagnostics, success);
    parser::FieldSyntax value_field = field;
    value_field.type_spelling = value_arg;
    auto value_type = analyze_field_type(symbols, namespace_path, value_field,
                                         diagnostics, success);

    if (key_type.kind != metadata::FieldTypeKind::String) {
        success = false;
        diagnostics.push_back(make_diagnostic(
            field.location,
            "unsupported map key type for JSON mapping: " + key_arg));
    }

    // 3. Store key/value as Map arguments for backend consumption.
    type.arguments.push_back(key_type);
    type.arguments.push_back(value_type);
    return type;
}

} // namespace

JsonFieldMetadataResult
parse_json_field_metadata(const std::string& comment,
                          const SourceLocation& location) {
    JsonFieldMetadataResult result;

    const std::string prefix = "json:";
    if (comment.rfind(prefix, 0) != 0) {
        result.found = false;
        return result;
    }

    result.found = true;
    const auto value = comment.substr(prefix.size());
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            R"(invalid CJM json metadata, expected json:"name")";
        return result;
    }

    const auto body = value.substr(1, value.size() - 2);
    const auto options = split_options(body);

    if (options.empty() || options[0].empty()) {
        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            "invalid CJM json metadata: empty JSON field name";
        return result;
    }

    result.json_name = options[0];
    if (result.json_name == "-") {
        result.ignored = true;
    }

    for (std::size_t i = 1; i < options.size(); ++i) {
        if (options[i] == "omitempty") {
            result.omit_empty = true;
            continue;
        }

        if (options[i].empty()) {
            continue;
        }

        result.success = false;
        result.diagnostic.location = location;
        result.diagnostic.message =
            "unsupported CJM json metadata option: " + options[i];
        return result;
    }
    result.success = true;
    return result;
}

/**
 * Check whether a resolved field type references a generated user-defined type.
 *
 * Inputs:
 *   field_type          - Semantic Metadata IR type for one field.
 *   qualified_type_name - Fully qualified generated type name to look for.
 *
 * Output:
 *   Returns true when field_type directly or recursively references
 * qualified_type_name.
 *
 * This helper only inspects resolved Metadata IR. It does not perform C++ name
 * lookup, does not resolve aliases, and does not validate whether the field
 * type is supported.
 */
bool field_type_depends_on(const metadata::FieldType& field_type,
                           const std::string& qualified_type_name) {
    // A direct user-defined field depends on the generated type it resolved to
    if (field_type.kind == metadata::FieldTypeKind::UserDefined) {
        return field_type.qualified_name == qualified_type_name;
    }

    // Container-like field types may hide dependencies in their arguments.
    for (const auto& argument : field_type.arguments) {
        if (field_type_depends_on(argument, qualified_type_name)) {
            return true;
        }
    }

    // Scalar, string, enum, and unsupported fallback types do not add a
    // generated-type dependency here.
    return false;
}

/**
 * Check whether one generated type depends on another generated type.
 *
 * Inputs:
 *   type        - Candidate type whose fields may reference another type.
 *   dependency  - Generated type that must appear before type if referenced.
 *
 * Output:
 *   Returns true if any field in type directly or recursively references
 * dependency through its resolved metadata IR field type.
 *
 * This helper only compares already-resolved Metadata IR names. It does not
 * perform C++ lookup and does not decide final output order.
 */
bool type_depends_on(const metadata::TypeModel& type,
                     const metadata::TypeModel& dependency) {
    for (const auto& field : type.fields) {
        if (field_type_depends_on(field.type, dependency.qualified_name)) {
            return true;
        }
    }
    return false;
}

struct TypeOrderingResult {
    std::vector<metadata::TypeModel> types;
    bool success = true;
};

/*
 * Repeatedly move ready types into ordered: a type is ready when every
 * generated type it depends on has already been ordered.
 *
 * e.g. {User, Address,Country}
 * struct User { Address address, Country country};
 * struct Address { Country country };
 * Here User depends on Address and Country.
 *
 * First iteration: ordered = {Country}
 * Second iteration: ordered = {Country, Address}
 * Final iteration: ordered = {Country, Address, User}
 */
TypeOrderingResult
order_types_by_dependency(const std::vector<metadata::TypeModel>& types) {
    TypeOrderingResult result;
    std::set<std::string> ordered_names;

    while (result.types.size() < types.size()) {
        bool progress = false;

        for (const auto& type : types) {
            if (ordered_names.count(type.qualified_name) != 0) {
                continue;
            }

            bool blocked = false;

            for (const auto& possible_dependency : types) {
                if (possible_dependency.qualified_name == type.qualified_name) {
                    continue;
                }

                if (type_depends_on(type, possible_dependency) &&
                    ordered_names.count(possible_dependency.qualified_name) ==
                        0) {
                    blocked = true;
                }
            }
            if (!blocked) {
                result.types.push_back(type);
                ordered_names.insert(type.qualified_name);
                progress = true;
            }
        }
        if (!progress) {
            result.success = false;
            return result;
        }
    }
    return result;
}

// Collect names needed by semantic type resolution before analyzing fields.
TypeSymbols collect_type_symbols(const parser::SourceFileSyntax& file) {
    // collect symbols
    TypeSymbols symbols;
    for (const auto& declaration : file.declarations) {
        symbols.generated_types.insert(
            join_qualified_name(declaration.namespace_path, declaration.name));
    }
    for (const auto& enum_syntax : file.enums) {
        symbols.enums.insert(
            join_qualified_name(enum_syntax.namespace_path, enum_syntax.name));
    }
    for (const auto& alias : file.type_aliases) {
        const auto qualified_name =
            join_qualified_name(alias.namespace_path, alias.name);
        symbols.aliases[qualified_name] = alias.target_type_spelling;
    }
    return symbols;
}

// Create the TypeModel identity before fields are analyzed.
metadata::TypeModel
make_type_model_shell(const parser::DeclarationSyntax& declaration) {
    metadata::TypeModel type;
    type.name = declaration.name;
    type.namespace_path = declaration.namespace_path;
    type.qualified_name = join_qualified_name(type.namespace_path, type.name);
    type.source_location = to_metadata_location(declaration.location);
    return type;
}

// Record a JSON name within one generated type, reporting duplicates.
bool record_json_name(const std::string& json_name,
                      const SourceLocation& location,
                      std::set<std::string>& json_names,
                      std::vector<Diagnostic>& diagnostics) {
    if (json_names.count(json_name) != 0) {
        diagnostics.push_back(make_diagnostic(
            location, "duplicate JSON field name: " + json_name));
        return false;
    }

    json_names.insert(json_name);
    return true;
}

FieldAnalysisResult analyze_field(
    const TypeSymbols& symbols, const std::vector<std::string>& namespace_path,
    const parser::FieldSyntax& field_syntax, std::set<std::string>& json_names,
    std::vector<Diagnostic>& diagnostics) {
    FieldAnalysisResult result;

    // 1. Find the CJM JSON metadata comment that selects this field.
    for (const auto& comment : field_syntax.comments) {
        const auto comment_location = to_semantic_location(comment.location);
        const auto metadata_result =
            parse_json_field_metadata(comment.text, comment_location);
        if (!metadata_result.found) {
            continue;
        }
        if (!metadata_result.success) {
            result.success = false;
            diagnostics.push_back(metadata_result.diagnostic);
            continue;
        }
        if (metadata_result.ignored) {
            return result;
        }

        if (!record_json_name(metadata_result.json_name, comment_location,
                              json_names, diagnostics)) {
            result.success = false;
            return result;
        }

        // Build the Metadata IR field after metadata and type validation.
        metadata::FieldModel field;
        field.name = field_syntax.name;

        bool type_success = true;
        field.type = analyze_field_type(symbols, namespace_path, field_syntax,
                                        diagnostics, type_success);

        field.json.name = metadata_result.json_name;
        field.json.omit_empty = metadata_result.omit_empty;

        field.source_location = to_metadata_location(field_syntax.location);

        if (!type_success) {
            result.success = false;
        }
        result.field = field;
        result.include = true;
        return result;
    }
    return result;
}

AnalysisResult analyze_source_file(const parser::SourceFileSyntax& file) {
    AnalysisResult result;
    result.success = true;
    // 1. Collect symbols before analyzing fields so type lookup can resolve
    // forward references within the source file.
    const auto symbols = collect_type_symbols(file);
    // 2. Convert parser declarations into Metadata IR types.
    for (const auto& declaration : file.declarations) {
        auto type = make_type_model_shell(declaration);
        std::set<std::string> json_names;
        for (const auto& field_syntax : declaration.fields) {
            auto field_result =
                analyze_field(symbols, declaration.namespace_path, field_syntax,
                              json_names, result.diagnostics);
            if (!field_result.success) {
                result.success = false;
                continue;
            }
            if (field_result.include) {
                type.fields.push_back(field_result.field);
            }
        }
        if (!type.fields.empty()) {
            result.project.types.push_back(type);
        }
    }

    // 3. Do not expose partial Metadata IR when semantic analysis failed.
    if (!result.success) {
        result.project.types.clear();
        return result;
    }
    // 4. Stabilize output order so dependencies appear before their users.
    auto ordering_result = order_types_by_dependency(result.project.types);
    if (!ordering_result.success) {
        Diagnostic diagnostic;
        diagnostic.message = "cyclic generated type dependency detected";
        result.diagnostics.push_back(diagnostic);
        result.success = false;
        result.project.types.clear();
        return result;
    }
    result.project.types = ordering_result.types;

    return result;
}

// Analyze multiple parser outputs as one semantic unit so symbols can resolve
// across explicit input files.
AnalysisResult
analyze_source_files(const std::vector<parser::SourceFileSyntax>& files) {
    AnalysisResult result;
    result.success = true;

    if (files.empty()) {
        return result;
    }

    parser::SourceFileSyntax merged;
    for (const auto& file : files) {
        merged.declarations.insert(merged.declarations.end(),
                                   file.declarations.begin(),
                                   file.declarations.end());
        merged.enums.insert(merged.enums.end(), file.enums.begin(),
                            file.enums.end());
        merged.type_aliases.insert(merged.type_aliases.end(),
                                   file.type_aliases.begin(),
                                   file.type_aliases.end());
        merged.comments.insert(merged.comments.end(), file.comments.begin(),
                               file.comments.end());
    }

    return analyze_source_file(merged);
}

} // namespace cjm::semantic
