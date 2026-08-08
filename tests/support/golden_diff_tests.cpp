#include "support/golden_diff.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("golden_diff.reports_first_changed_character", "[golden]") {
    const auto mismatch =
        cjm::test::find_golden_mismatch("alpha\nbravo\n", "alpha\nbraxo\n");

    REQUIRE(mismatch.kind == cjm::test::GoldenMismatchKind::DifferentCharacter);
    REQUIRE(mismatch.line == 2);
    REQUIRE(mismatch.column == 4);
    REQUIRE(mismatch.expected_line == "bravo");
    REQUIRE(mismatch.actual_line == "braxo");

    const auto message =
        cjm::test::format_golden_mismatch("alpha\nbravo\n", "alpha\nbraxo\n");
    REQUIRE(message.find("line 2, column 4") != std::string::npos);
    REQUIRE(message.find("different_character") != std::string::npos);
}

TEST_CASE("golden_diff.reports_missing_trailing_content", "[golden]") {
    const auto mismatch =
        cjm::test::find_golden_mismatch("alpha\nbravo\n", "alpha\n");

    REQUIRE(mismatch.kind ==
            cjm::test::GoldenMismatchKind::MissingTrailingContent);
    REQUIRE(mismatch.line == 2);
    REQUIRE(mismatch.column == 1);
    REQUIRE(mismatch.expected_line == "bravo");
    REQUIRE(mismatch.actual_line == "");

    const auto message =
        cjm::test::format_golden_mismatch("alpha\nbravo\n", "alpha\n");

    REQUIRE(message.find("line 2, column 1") != std::string::npos);
    REQUIRE(message.find("missing_trailing_content") != std::string::npos);
}

TEST_CASE("golden_diff.reports_extra_trailing_content", "[golden]") {
    const auto mismatch =
        cjm::test::find_golden_mismatch("alpha\n", "alpha\nbravo\n");

    REQUIRE(mismatch.kind ==
            cjm::test::GoldenMismatchKind::ExtraTrailingContent);
    REQUIRE(mismatch.line == 2);
    REQUIRE(mismatch.column == 1);
    REQUIRE(mismatch.expected_line == "");
    REQUIRE(mismatch.actual_line == "bravo");

    const auto message =
        cjm::test::format_golden_mismatch("alpha\n", "alpha\nbravo\n");

    REQUIRE(message.find("line 2, column 1") != std::string::npos);
    REQUIRE(message.find("extra_trailing_content") != std::string::npos);
}
