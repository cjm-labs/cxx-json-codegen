#include <cassert>
#include <string>

#include "frontends/cxx/tree_sitter/adapter.hpp"

int main() {
    {
        const std::string source = R"cpp(
namespace company::model {
enum class Status {
    Ok,
};

using Sequence = std::uint64_t;

struct User {
    Sequence id; // json:"id"
    std::string name; // json:"name"
};
}
)cpp";

        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "user.hpp", source);

        assert(result.success);
        assert(result.file.path == "user.hpp");

        assert(result.file.declarations.size() == 1);

        const auto& user = result.file.declarations[0];
        assert(user.name == "User");
        assert(user.namespace_path.size() == 2);
        assert(user.namespace_path[0] == "company");
        assert(user.namespace_path[1] == "model");
        assert(user.location.file == "user.hpp");
        assert(user.location.line == 9);
        assert(user.location.column == 1);

        assert(user.fields.size() == 2);

        const auto& name = user.fields[0];
        assert(name.name == "id");
        assert(name.type_spelling == "Sequence");
        assert(name.location.file == "user.hpp");
        assert(name.location.line == 10);
        assert(name.location.column == 5);
        assert(name.comments.size() == 1);
        assert(name.comments[0].text == "// json:\"id\"");
        assert(name.comments[0].location.file == "user.hpp");
        assert(name.comments[0].location.line == 10);
        assert(name.comments[0].location.column == 18);

        assert(result.file.enums.size() == 1);
        auto status = result.file.enums[0];
        assert(status.name == "Status");
        assert(status.namespace_path.size() == 2);
        assert(status.namespace_path[0] == "company");
        assert(status.namespace_path[1] == "model");
        assert(status.location.file == "user.hpp");
        assert(status.location.line == 3);
        assert(status.location.column == 1);

        assert(result.file.type_aliases.size() == 1);
        auto sequence = result.file.type_aliases[0];
        assert(sequence.name == "Sequence");
        assert(sequence.target_type_spelling == "std::uint64_t");
        assert(sequence.namespace_path.size() == 2);
        assert(sequence.namespace_path[0] == "company");
        assert(sequence.namespace_path[1] == "model");

        assert(result.diagnostics.empty());
    }
    {
        const std::string source = R"cpp(
#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace company::model {
enum class Status {
    Ok,
    Failed,
};

using Sequence = std::uint64_t;

struct Detail {
    std::string source; // json:"source"
    std::uint32_t shard; // json:"shard"
};

struct Event {
    Sequence sequence; // json:"sequence"
    std::int64_t timestamp_ns; // json:"timestamp_ns"
    Status status; // json:"status"
    Detail detail; // json:"detail"
    std::vector<std::string> tags; // json:"tags"
    std::map<std::string, std::vector<std::uint64_t>> buckets; // json:"buckets"
    std::optional<Sequence> retry_after; // json:"retry_after,omitempty"
    std::optional<std::unordered_map<std::string, std::string>> attributes; // json:"attributes,omitempty"
};
} // namespace company::model
)cpp";

        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "practical.hpp", source);

        assert(result.success);
        assert(result.file.path == "practical.hpp");
        assert(result.diagnostics.empty());

        assert(result.file.enums.size() == 1);
        const auto& status = result.file.enums[0];
        assert(status.name == "Status");
        assert(status.namespace_path.size() == 2);
        assert(status.namespace_path[0] == "company");
        assert(status.namespace_path[1] == "model");

        assert(result.file.type_aliases.size() == 1);
        const auto& sequence = result.file.type_aliases[0];
        assert(sequence.name == "Sequence");
        assert(sequence.target_type_spelling == "std::uint64_t");
        assert(sequence.namespace_path.size() == 2);
        assert(sequence.namespace_path[0] == "company");
        assert(sequence.namespace_path[1] == "model");

        assert(result.file.declarations.size() == 2);

        const auto& detail = result.file.declarations[0];
        assert(detail.name == "Detail");
        assert(detail.namespace_path.size() == 2);
        assert(detail.fields.size() == 2);

        assert(detail.fields[0].name == "source");
        assert(detail.fields[0].type_spelling == "std::string");
        assert(detail.fields[0].comments.size() == 1);
        assert(detail.fields[0].comments[0].text == "// json:\"source\"");

        assert(detail.fields[1].name == "shard");
        assert(detail.fields[1].type_spelling == "std::uint32_t");
        assert(detail.fields[1].comments.size() == 1);
        assert(detail.fields[1].comments[0].text == "// json:\"shard\"");

        const auto& event = result.file.declarations[1];
        assert(event.name == "Event");
        assert(event.namespace_path.size() == 2);
        assert(event.namespace_path[0] == "company");
        assert(event.namespace_path[1] == "model");
        assert(event.fields.size() == 8);

        assert(event.fields[0].name == "sequence");
        assert(event.fields[0].type_spelling == "Sequence");
        assert(event.fields[0].comments[0].text == "// json:\"sequence\"");

        assert(event.fields[1].name == "timestamp_ns");
        assert(event.fields[1].type_spelling == "std::int64_t");
        assert(event.fields[1].comments[0].text == "// json:\"timestamp_ns\"");

        assert(event.fields[2].name == "status");
        assert(event.fields[2].type_spelling == "Status");
        assert(event.fields[2].comments[0].text == "// json:\"status\"");

        assert(event.fields[3].name == "detail");
        assert(event.fields[3].type_spelling == "Detail");
        assert(event.fields[3].comments[0].text == "// json:\"detail\"");

        assert(event.fields[4].name == "tags");
        assert(event.fields[4].type_spelling == "std::vector<std::string>");
        assert(event.fields[4].comments[0].text == "// json:\"tags\"");

        assert(event.fields[5].name == "buckets");
        assert(event.fields[5].type_spelling ==
               "std::map<std::string, std::vector<std::uint64_t>>");
        assert(event.fields[5].comments[0].text == "// json:\"buckets\"");

        assert(event.fields[6].name == "retry_after");
        assert(event.fields[6].type_spelling == "std::optional<Sequence>");
        assert(event.fields[6].comments[0].text ==
               "// json:\"retry_after,omitempty\"");

        assert(event.fields[7].name == "attributes");
        assert(event.fields[7].type_spelling ==
               "std::optional<std::unordered_map<std::string, std::string>>");
        assert(event.fields[7].comments[0].text ==
               "// json:\"attributes,omitempty\"");
    }

    {
        const std::string source = R"cpp(
struct User {
)cpp";
        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "broken.hpp", source);
        assert(!result.success);
        assert(!result.diagnostics.empty());
        assert(result.diagnostics[0].message.find("syntax errors") !=
               std::string::npos);
        assert(result.diagnostics[0].location.file == "broken.hpp");
        assert(result.diagnostics[0].location.line >= 2);
    }
    {
        const std::string source = R"cpp(
struct User {
    static int count; // json:"count"
};
)cpp";
        const auto result =
            cjm::frontends::cxx ::tree_sitter::parse_source_text("static.hpp",
                                                                 source);
        assert(!result.success);
        assert(!result.diagnostics.empty());
        assert(result.diagnostics[0].message.find("static data members") !=
               std::string::npos);
        assert(result.diagnostics[0].location.file == "static.hpp");
    }
    {
        const std::string source = R"cpp(
struct User {
    int x, y; // json:"x"
};
)cpp";

        const auto result = cjm::frontends::cxx::tree_sitter::parse_source_text(
            "multiple.hpp", source);

        assert(!result.success);
        assert(!result.diagnostics.empty());
        assert(result.diagnostics[0].message.find(
                   "multiple field declarators") != std::string::npos);
        assert(result.diagnostics[0].location.file == "multiple.hpp");
    }
    return 0;
}
