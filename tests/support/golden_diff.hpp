#pragma once

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace cjm::test {

enum class GoldenMismatchKind {
    DifferentCharacter,
    MissingTrailingContent,
    ExtraTrailingContent,
};

struct GoldenMismatch {
    GoldenMismatchKind kind;
    std::size_t line;
    std::size_t column;
    std::string expected_line;
    std::string actual_line;
};

namespace detail {

inline std::size_t first_mismatch_offset(std::string_view expected,
                                         std::string_view actual) {
    const std::size_t shared_size = std::min(expected.size(), actual.size());

    for (std::size_t offset = 0; offset < shared_size; ++offset) {
        if (expected[offset] != actual[offset]) {
            return offset;
        }
    }
    return shared_size;
}

inline std::pair<std::size_t, std::size_t> line_column_at(std::string_view text,
                                                          std::size_t offset) {
    std::size_t line = 1;
    std::size_t column = 1;

    const std::size_t bounded_offset = std::min(offset, text.size());

    for (std::size_t index = 0; index < bounded_offset; ++index) {
        if (text[index] == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
    }
    return {line, column};
}

inline std::string line_at(std::string_view text, std::size_t offset) {
    if (text.empty()) {
        return {};
    }

    const std::size_t bounded_offset = std::min(offset, text.size());

    std::size_t line_start = text.rfind('\n', bounded_offset);
    if (line_start == std::string_view::npos) {
        line_start = 0;
    } else {
        ++line_start;
    }

    std::size_t line_end = text.find('\n', bounded_offset);
    if (line_end == std::string_view::npos) {
        line_end = text.size();
    }

    return std::string{text.substr(line_start, line_end - line_start)};
}

inline GoldenMismatchKind mismatch_kind(std::string_view expected,
                                        std::string_view actual,
                                        std::size_t offset) {
    if (offset == actual.size() && expected.size() > actual.size()) {
        return GoldenMismatchKind::MissingTrailingContent;
    }

    if (offset == expected.size() && actual.size() > expected.size()) {
        return GoldenMismatchKind::ExtraTrailingContent;
    }

    return GoldenMismatchKind::DifferentCharacter;
}

inline std::string mismatch_kind_name(GoldenMismatchKind kind) {
    switch (kind) {
    case GoldenMismatchKind::DifferentCharacter:
        return "different_character";
    case GoldenMismatchKind::MissingTrailingContent:
        return "missing_trailing_content";
    case GoldenMismatchKind::ExtraTrailingContent:
        return "extra_trailing_content";
    }
    return "unknown";
}

inline std::string caret_line(std::size_t column) {
    if (column == 0) {
        return "^";
    }
    return std::string(column - 1, ' ') + "^";
}

} // namespace detail
inline GoldenMismatch find_golden_mismatch(std::string_view expected,
                                           std::string_view actual) {
    const std::size_t offset = detail::first_mismatch_offset(expected, actual);
    const auto [line, column] = detail::line_column_at(expected, offset);

    return GoldenMismatch{
        detail::mismatch_kind(expected, actual, offset),
        line,
        column,
        detail::line_at(expected, offset),
        detail::line_at(actual, offset),
    };
}

inline std::string format_golden_mismatch(std::string_view expected,
                                          std::string_view actual) {
    const GoldenMismatch mismatch = find_golden_mismatch(expected, actual);

    std::ostringstream out;
    out << "golden mismatch at line " << mismatch.line << ", column "
        << mismatch.column << '\n';
    out << "kind: " << detail::mismatch_kind_name(mismatch.kind) << '\n';
    out << "expected: " << mismatch.expected_line << '\n';
    out << "actual:   " << mismatch.actual_line << '\n';
    out << "          " << detail::caret_line(mismatch.column) << '\n';
    return out.str();
}

} // namespace cjm::test
