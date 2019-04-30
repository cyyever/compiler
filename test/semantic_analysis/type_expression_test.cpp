/*!
 * \file type_expression_test.cpp
 *
 * \brief 测试type expressions
 * \author cyy
 * \date 2019-03-03
 */
#if __has_include(<CppCoreCheck\Warnings.h>)
#include <CppCoreCheck\Warnings.h>
#pragma warning(disable : ALL_CPPCORECHECK_WARNINGS)
#endif
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include <cyy/computation/lang/common_tokens.hpp>

#include "../../src/semantic_analysis/l_attributed_sdd.hpp"
#include "../../src/semantic_analysis/type_expression.hpp"
#include "../../src/symbol_table/symbol_table.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("types and storage layout") {

  std::vector<CFG_production> production_vector;

  production_vector.emplace_back(
      "D",
      CFG_production::body_type{
          "T", static_cast<CFG::terminal_type>(common_token::id), ';', "D"});
  production_vector.emplace_back(
      "D",
      CFG_production::body_type{ALPHABET::get("common_tokens")->get_epsilon()});
  production_vector.emplace_back("T", CFG_production::body_type{"B", "C"});
  production_vector.emplace_back(
      "T", CFG_production::body_type{
               static_cast<CFG::terminal_type>(common_token::record), '{', "D",
               '}'});
  production_vector.emplace_back(
      "B", CFG_production::body_type{
               static_cast<CFG::terminal_type>(common_token::INT)});
  production_vector.emplace_back(
      "B", CFG_production::body_type{
               static_cast<CFG::terminal_type>(common_token::FLOAT)});
  production_vector.emplace_back(
      "C",
      CFG_production::body_type{ALPHABET::get("common_tokens")->get_epsilon()});
  production_vector.emplace_back(
      "C", CFG_production::body_type{
               '[', static_cast<CFG::terminal_type>(common_token::number), ']',
               "C"});

  production_vector.emplace_back(
      "T",
      CFG_production::body_type{
          static_cast<CFG::terminal_type>(common_token::CLASS),
          static_cast<CFG::terminal_type>(common_token::id), '{', "D", '}'});
  production_vector.emplace_back("P", CFG_production::body_type{"D"});

  std::map<CFG::nonterminal_type, std::vector<CFG_production::body_type>>
      productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  LL_grammar grammar("common_tokens", "P", productions);
  L_attributed_SDD sdd(grammar);

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{"$0.type",
                         {"$2.type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{"$0.width",
                         {"$2.width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_inherited_attribute(
      production_vector[2],
      SDD::semantic_rule{"$2.inh_type",
                         {"$1.type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_inherited_attribute(
      production_vector[2],
      SDD::semantic_rule{"$2.inh_width",
                         {"$1.width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_synthesized_attribute(
      production_vector[4],
      SDD::semantic_rule{
          "$0.type", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<
                std::shared_ptr<cyy::compiler::type_expression::expression>>(
                std::make_shared<cyy::compiler::type_expression::basic_type>(
                    cyy::compiler::type_expression::basic_type::type_enum::
                        INT));
          }});

  sdd.add_synthesized_attribute(
      production_vector[4],
      SDD::semantic_rule{
          "$0.width", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<size_t>(4);
          }});

  sdd.add_synthesized_attribute(
      production_vector[5],
      SDD::semantic_rule{
          "$0.type", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<
                std::shared_ptr<cyy::compiler::type_expression::expression>>(
                std::make_shared<cyy::compiler::type_expression::basic_type>(
                    cyy::compiler::type_expression::basic_type::type_enum::
                        FLOAT));
          }});

  sdd.add_synthesized_attribute(
      production_vector[5],
      SDD::semantic_rule{
          "$0.width", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<size_t>(8);
          }});

  sdd.add_synthesized_attribute(
      production_vector[6],
      SDD::semantic_rule{"$0.type",
                         {"$0.inh_type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});
  sdd.add_synthesized_attribute(
      production_vector[6],
      SDD::semantic_rule{"$0.width",
                         {"$0.inh_width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_inherited_attribute(
      production_vector[7],
      SDD::semantic_rule{"$4.inh_type",
                         {"$0.inh_type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_inherited_attribute(
      production_vector[7],
      SDD::semantic_rule{"$4.inh_width",
                         {"$0.inh_width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});
  sdd.add_synthesized_attribute(
      production_vector[7],
      SDD::semantic_rule{
          "$0.type",
          {"$2", "$4.type"},
          [](const auto &arguments) -> std::optional<std::any> {
            size_t element_number = 0;

            for (auto c : std::any_cast<token>(arguments.at(0).get()).lexeme) {
              element_number = element_number * 10 + c - '0';
            }

            return std::make_any<
                std::shared_ptr<cyy::compiler::type_expression::expression>>(
                std::make_shared<cyy::compiler::type_expression::array_type>(
                    std::any_cast<std::shared_ptr<
                        cyy::compiler::type_expression::expression>>(
                        arguments.at(1).get()),
                    element_number));
          }});

  sdd.add_synthesized_attribute(
      production_vector[7],
      SDD::semantic_rule{
          "$0.width",
          {"$2", "$4.width"},
          [](const auto &arguments) -> std::optional<std::any> {
            size_t element_number = 0;

            for (auto c : std::any_cast<token>(arguments.at(0).get()).lexeme) {
              element_number = element_number * 10 + c - '0';
            }

            return std::make_any<size_t>(
                element_number * std::any_cast<size_t>(arguments.at(1).get()));
          }});

  SUBCASE("types and widths") {
    std::vector<token> tokens;
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::INT), "int", {}});
    tokens.push_back(token{'[', "[", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::number), "2", {}});
    tokens.push_back(token{']', "]", {}});
    tokens.push_back(token{'[', "[", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::number), "3", {}});
    tokens.push_back(token{']', "]", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "a", {}});
    tokens.push_back(token{';', ";", {}});

    auto attributes = sdd.run(tokens, {"T.width", "T.type"});
    REQUIRE(attributes);

    REQUIRE(std::any_cast<size_t>(attributes.value()["T.width"]) == 24);
    REQUIRE(std::any_cast<
                std::shared_ptr<cyy::compiler::type_expression::expression>>(
                attributes.value()["T.type"])
                ->equivalent_with(

                    cyy::compiler::type_expression::array_type(
                        std::make_shared<
                            cyy::compiler::type_expression::array_type>(
                            std::make_shared<
                                cyy::compiler::type_expression::basic_type>(
                                cyy::compiler::type_expression::basic_type::
                                    type_enum::INT),
                            3),
                        2)));
  }

  sdd.add_inherited_attribute(
      production_vector[9],
      SDD::semantic_rule{
          "$1.offset", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<size_t>(0);
          }});

  sdd.add_inherited_attribute(
      production_vector[9],
      SDD::semantic_rule{
          "$1.symbol_table", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<std::shared_ptr<symbol_table>>(
                std::make_shared<symbol_table>());
          }});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{"$1.symbol_table",
                         {"$0.symbol_table"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return arguments.at(0).get();
                         }});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{
          "$4.symbol_table",
          {"$0.symbol_table", "$0.offset", "$1.type", "$2"},
          [](const auto &arguments) -> std::optional<std::any> {
            auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                arguments.at(0).get());
            symbol_table::entry e;
            e.lexeme = std::any_cast<token>(arguments.at(3).get()).lexeme;
            e.type =
                std::any_cast<std::shared_ptr<type_expression::expression>>(
                    arguments.at(2).get());
            e.relative_address = std::any_cast<size_t>(arguments.at(1).get());
            table->add_entry(e);
            return std::make_any<std::shared_ptr<symbol_table>>(table);
          }});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{"$4.offset",
                         {"$0.offset", "$1.width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return std::make_any<size_t>(
                               std::any_cast<size_t>(arguments.at(0).get()) +
                               std::any_cast<size_t>(arguments.at(1).get()));
                         }});

  SUBCASE("relative addresses") {
    std::vector<token> tokens;
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::FLOAT), "float", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "x", {}});
    tokens.push_back(token{';', ";", {}});

    auto attributes = sdd.run(tokens, {"D.symbol_table"});
    REQUIRE(attributes);
    auto const &table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["D.symbol_table"]);
    REQUIRE(table->get_entry("x")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(table->get_entry("x")->relative_address == 0);
  }

  SUBCASE("records") {
    sdd.add_inherited_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$3.symbol_table",
            {"$0.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                  arguments.at(0).get());
              return std::make_any<std::shared_ptr<symbol_table>>(table);
            }});

    sdd.add_inherited_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$3.offset", {}, [](const auto &) -> std::optional<std::any> {
              return std::make_any<size_t>(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$0.width",
            {"$3.offset"},
            [](const auto &arguments) -> std::optional<std::any> {
              return arguments.at(0).get();
            }});

    sdd.add_synthesized_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$0.type",
            {"$3.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              std::vector<symbol_table::entry> sorted_entries;

              std::any_cast<std::shared_ptr<symbol_table>>(
                  arguments.at(0).get())
                  ->foreach_entry([&sorted_entries](auto const &e) {
                    sorted_entries.push_back(e);
                  });

              std::sort(sorted_entries.begin(), sorted_entries.end(),
                        [](const auto &a, const auto &b) {
                          return a.relative_address < b.relative_address;
                        });

              std::vector<std::pair<
                  std::string,
                  std::shared_ptr<cyy::compiler::type_expression::expression>>>
                  field_types;
              for (auto const &entry : sorted_entries) {
                field_types.emplace_back(entry.lexeme, entry.type);
              }

              return std::make_any<
                  std::shared_ptr<cyy::compiler::type_expression::expression>>(
                  std::make_shared<cyy::compiler::type_expression::record_type>(
                      field_types));
            }});

    std::vector<token> tokens;
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::record), "record", {}});
    tokens.push_back(token{'{', "{", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::INT), "int", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "tag", {}});
    tokens.push_back(token{';', ";", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::FLOAT), "float", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "x", {}});
    tokens.push_back(token{';', ";", {}});

    tokens.push_back(
        token{static_cast<symbol_type>(common_token::FLOAT), "float", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "y", {}});
    tokens.push_back(token{';', ";", {}});
    tokens.push_back(token{'}', "}", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "q", {}});
    tokens.push_back(token{';', ";", {}});

    auto attributes = sdd.run(tokens, {"T.type", "D.symbol_table"});
    REQUIRE(attributes);

    auto const &table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["D.symbol_table"]);
    REQUIRE(table->get_entry("tag")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(table->get_entry("tag")->relative_address == 0);
    REQUIRE(table->get_entry("x")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(table->get_entry("x")->relative_address == 4);
    REQUIRE(table->get_entry("y")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(table->get_entry("y")->relative_address == 12);
    auto const &record_type = std::any_cast<
        std::shared_ptr<cyy::compiler::type_expression::expression>>(
        attributes.value()["T.type"]);

    std::vector<
        std::pair<std::string,
                  std::shared_ptr<cyy::compiler::type_expression::expression>>>
        field_types;
    field_types.emplace_back(
        "tag",
        std::shared_ptr<cyy::compiler::type_expression::expression>(
            std::make_shared<cyy::compiler::type_expression::basic_type>(
                cyy::compiler::type_expression::basic_type::type_enum::INT)));
    field_types.emplace_back(
        "x",
        std::shared_ptr<cyy::compiler::type_expression::expression>(
            std::make_shared<cyy::compiler::type_expression::basic_type>(
                cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));

    field_types.emplace_back(
        "y",
        std::shared_ptr<cyy::compiler::type_expression::expression>(
            std::make_shared<cyy::compiler::type_expression::basic_type>(
                cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));

    REQUIRE(record_type->equivalent_with(
        cyy::compiler::type_expression::record_type(field_types)));
  }

  SUBCASE("classes") {
    sdd.add_inherited_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$4.symbol_table",
            {"$0.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                  arguments.at(0).get());
              return std::make_any<std::shared_ptr<symbol_table>>(table);
            }});

    sdd.add_inherited_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$4.offset", {}, [](const auto &) -> std::optional<std::any> {
              return std::make_any<size_t>(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$0.width",
            {"$4.offset"},
            [](const auto &arguments) -> std::optional<std::any> {
              return arguments.at(0).get();
            }});

    sdd.add_synthesized_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$0.type",
            {"$2", "$4.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              std::vector<symbol_table::entry> sorted_entries;

              std::any_cast<std::shared_ptr<symbol_table>>(
                  arguments.at(1).get())
                  ->foreach_entry([&sorted_entries](auto const &e) {
                    sorted_entries.push_back(e);
                  });

              std::sort(sorted_entries.begin(), sorted_entries.end(),
                        [](const auto &a, const auto &b) {
                          return a.relative_address < b.relative_address;
                        });

              std::vector<std::pair<
                  std::string,
                  std::shared_ptr<cyy::compiler::type_expression::expression>>>
                  field_types;
              for (auto const &entry : sorted_entries) {
                field_types.emplace_back(entry.lexeme, entry.type);
              }

              auto const &class_name =
                  std::any_cast<token>(arguments.at(0).get()).lexeme;
              auto class_type =
                  std::make_shared<cyy::compiler::type_expression::class_type>(
                      nullptr, field_types);
              return std::make_any<
                  std::shared_ptr<cyy::compiler::type_expression::expression>>(
                  std::make_shared<cyy::compiler::type_expression::type_name>(
                      class_name, class_type));
            }});

    std::vector<token> tokens;
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::CLASS), "class", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "A", {}});
    tokens.push_back(token{'{', "{", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::INT), "int", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "tag", {}});
    tokens.push_back(token{';', ";", {}});
    tokens.push_back(token{'}', "}", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "q", {}});
    tokens.push_back(token{';', ";", {}});

    auto attributes = sdd.run(tokens, {"T.type", "D.symbol_table"});
    REQUIRE(attributes);

    auto const &table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["D.symbol_table"]);
    REQUIRE(table->get_entry("tag")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(table->get_entry("tag")->relative_address == 0);
    auto const &record_type = std::any_cast<
        std::shared_ptr<cyy::compiler::type_expression::expression>>(
        attributes.value()["T.type"]);

    std::vector<
        std::pair<std::string,
                  std::shared_ptr<cyy::compiler::type_expression::expression>>>
        field_types;
    field_types.emplace_back(
        "tag",
        std::shared_ptr<cyy::compiler::type_expression::expression>(
            std::make_shared<cyy::compiler::type_expression::basic_type>(
                cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(record_type->equivalent_with(
        cyy::compiler::type_expression::class_type(nullptr, field_types)));
  }
}
