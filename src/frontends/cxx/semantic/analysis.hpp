#pragma once

#include "core/ir/model.hpp"
#include "frontends/cxx/parser/syntax.hpp"

#include <string>
#include <vector>

namespace cjm::semantic {

struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
};

struct Diagnostic {
    SourceLocation location;
    std::string message;
};

struct AnalysisResult {
    metadata::ProjectModel project;
    std::vector<Diagnostic> diagnostics;
    bool success = false;
};

struct JsonFieldMetadataResult {
    std::string json_name;
    Diagnostic diagnostic;
    bool omit_empty = false;
    bool ignored = false;
    bool success = false;
    bool found = false;
};

JsonFieldMetadataResult
parse_json_field_metadata(const std::string& comment,
                          const SourceLocation& location);

AnalysisResult analyze_source_file(const parser::SourceFileSyntax& file);
AnalysisResult
analyze_source_files(const std::vector<parser::SourceFileSyntax>& files);
} // namespace cjm::semantic
