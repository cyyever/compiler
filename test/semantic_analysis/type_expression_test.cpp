/*!
 * \file type_expression_test.cpp
 *
 * \brief 测试type expressions
 * \author cyy
 * \date 2019-03-03
 */
#include <cyy/computation/lang/common_tokens.hpp>
#include <doctest/doctest.h>

#include "example_grammar/declaration_sdd.hpp"
#include "example_grammar/lexical_analyzer.hpp"
#include "exception.hpp"
#include "type_expression.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("types and storage layout") {

  auto analyzer = example_grammar::get_lexical_analyzer();
  example_grammar::declaration_SDD sdd;

  SUBCASE("types and widths") {
    analyzer->set_source_code("int[2][3]a;");
    auto tokens = analyzer->scan_all();

    auto table = sdd.run(tokens);
    REQUIRE(table);
    auto e = table->get_symbol("a");

    REQUIRE(e);
    REQUIRE(e->type);
    REQUIRE(e->type->get_width() == 24);
    REQUIRE(e->type->equivalent_with(type_expression::array_type(
        std::make_shared<type_expression::array_type>(
            std::make_shared<type_expression::basic_type>(
                type_expression::basic_type::type_enum::INT),
            3),
        2)));
  }

  SUBCASE("relative addresses") {
    std::vector<token> tokens;
    tokens.emplace_back(static_cast<symbol_type>(common_token::FLOAT), "float");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "x");
    tokens.emplace_back(';', ";");

    auto table = sdd.run(tokens);
    REQUIRE(table);
    auto e = table->get_symbol("x");
    REQUIRE(e);

    REQUIRE(e->type->equivalent_with(type_expression::basic_type(
        type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(e->relative_address == 0);
  }

  SUBCASE("records") {
    std::vector<token> tokens;
    tokens.emplace_back(static_cast<symbol_type>(common_token::record),
                        "record");
    tokens.emplace_back('{', "{");
    tokens.emplace_back(static_cast<symbol_type>(common_token::INT), "int");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "tag");
    tokens.emplace_back(';', ";");
    tokens.emplace_back(static_cast<symbol_type>(common_token::FLOAT), "float");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "x");
    tokens.emplace_back(';', ";");

    tokens.emplace_back(static_cast<symbol_type>(common_token::FLOAT), "float");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "y");
    tokens.emplace_back(';', ";");
    tokens.emplace_back('}', "}");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "q");
    tokens.emplace_back(';', ";");

    auto table = sdd.run(tokens);
    REQUIRE(table);
    auto e = table->get_symbol("q");
    REQUIRE(e);

    auto associated_symbol_table =
        std::dynamic_pointer_cast<type_expression::record_type>(e->type)
            ->get_symbol_table();
    REQUIRE(associated_symbol_table);
    REQUIRE(associated_symbol_table->get_symbol("tag")->type->equivalent_with(
        type_expression::basic_type(
            type_expression::basic_type::type_enum::INT)));
    REQUIRE(associated_symbol_table->get_symbol("tag")->relative_address == 0);
    REQUIRE(associated_symbol_table->get_symbol("x"));
    REQUIRE(associated_symbol_table->get_symbol("x")->type->equivalent_with(
        type_expression::basic_type(
            type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(associated_symbol_table->get_symbol("x")->relative_address == 4);
    REQUIRE(associated_symbol_table->get_symbol("y"));
    REQUIRE(associated_symbol_table->get_symbol("y")->type->equivalent_with(
        type_expression::basic_type(
            type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(associated_symbol_table->get_symbol("y")->relative_address == 12);

    std::vector<
        std::pair<std::string, std::shared_ptr<type_expression::expression>>>
        field_types;
    field_types.emplace_back(
        "tag", std::shared_ptr<type_expression::expression>(
                   std::make_shared<type_expression::basic_type>(
                       type_expression::basic_type::type_enum::INT)));
    field_types.emplace_back(
        "x", std::shared_ptr<type_expression::expression>(
                 std::make_shared<type_expression::basic_type>(
                     type_expression::basic_type::type_enum::FLOAT)));

    field_types.emplace_back(
        "y", std::shared_ptr<type_expression::expression>(
                 std::make_shared<type_expression::basic_type>(
                     type_expression::basic_type::type_enum::FLOAT)));

    REQUIRE(
        e->type->equivalent_with(type_expression::record_type(field_types)));
  }

  SUBCASE("classes") {

    std::vector<token> tokens;
    tokens.emplace_back(static_cast<symbol_type>(common_token::CLASS), "class");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "A");
    tokens.emplace_back('{', "{");
    tokens.emplace_back(static_cast<symbol_type>(common_token::INT), "int");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "x");
    tokens.emplace_back(';', ";");
    tokens.emplace_back('}', "}");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "q");
    tokens.emplace_back(';', ";");

    auto table = sdd.run(tokens);
    REQUIRE(table);
    auto e = table->get_symbol("q");
    REQUIRE(e);

    auto associated_symbol_table =
        std::dynamic_pointer_cast<type_expression::class_type>(

            std::dynamic_pointer_cast<type_expression::type_name>(e->type)
                ->get_type()

                )
            ->get_symbol_table();
    REQUIRE(associated_symbol_table);
    REQUIRE(associated_symbol_table->get_symbol("x"));
    REQUIRE(associated_symbol_table->get_symbol("x")->type->equivalent_with(
        type_expression::basic_type(
            type_expression::basic_type::type_enum::INT)));
    REQUIRE(associated_symbol_table->get_symbol("x")->relative_address == 0);

    std::vector<
        std::pair<std::string, std::shared_ptr<type_expression::expression>>>
        field_types;
    field_types.emplace_back(
        "x", std::shared_ptr<type_expression::expression>(
                 std::make_shared<type_expression::basic_type>(
                     type_expression::basic_type::type_enum::INT)));
    REQUIRE(e->type->equivalent_with(
        type_expression::class_type(nullptr, field_types)));

    tokens.emplace_back(static_cast<symbol_type>(common_token::CLASS), "class");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "B");
    tokens.emplace_back(':', ":");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "A");
    tokens.emplace_back('{', "{");
    tokens.emplace_back(static_cast<symbol_type>(common_token::INT), "int");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "y");
    tokens.emplace_back(';', ";");
    tokens.emplace_back('}', "}");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "p");
    tokens.emplace_back(';', ";");

    table = sdd.run(tokens);
    REQUIRE(table);

    e = table->get_symbol("p");
    REQUIRE(e);
    associated_symbol_table =
        std::dynamic_pointer_cast<type_expression::class_type>(

            std::dynamic_pointer_cast<type_expression::type_name>(e->type)
                ->get_type()

                )
            ->get_symbol_table();
    REQUIRE(associated_symbol_table);
    REQUIRE(associated_symbol_table->get_symbol("x"));
    REQUIRE(associated_symbol_table->get_symbol("x")->type->equivalent_with(
        type_expression::basic_type(
            type_expression::basic_type::type_enum::INT)));
    REQUIRE(associated_symbol_table->get_symbol("x")->relative_address == 0);

    REQUIRE(associated_symbol_table->get_symbol("y"));
    REQUIRE(associated_symbol_table->get_symbol("y")->type->equivalent_with(
        type_expression::basic_type(
            type_expression::basic_type::type_enum::INT)));
    REQUIRE(associated_symbol_table->get_symbol("y")->relative_address == 4);
  }
}
