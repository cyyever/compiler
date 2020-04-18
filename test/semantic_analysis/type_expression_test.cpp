/*!
 * \file type_expression_test.cpp
 *
 * \brief 测试type expressions
 * \author cyy
 * \date 2019-03-03
 */
#include <doctest/doctest.h>

#include <cyy/computation/lang/common_tokens.hpp>

#include "../../src/exception.hpp"
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
  production_vector.emplace_back("D", CFG_production::body_type{});
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
  production_vector.emplace_back("C", CFG_production::body_type{});
  production_vector.emplace_back(
      "C", CFG_production::body_type{
               '[', static_cast<CFG::terminal_type>(common_token::number), ']',
               "C"});

  production_vector.emplace_back(
      "T", CFG_production::body_type{
               static_cast<CFG::terminal_type>(common_token::CLASS),
               static_cast<CFG::terminal_type>(common_token::id),
               "parent_class", '{', "D", '}'});

  production_vector.emplace_back(
      "parent_class", CFG_production::body_type{
                          static_cast<CFG::terminal_type>(':'),
                          static_cast<CFG::terminal_type>(common_token::id)});

  production_vector.emplace_back("parent_class", CFG_production::body_type{});
  production_vector.emplace_back("P", CFG_production::body_type{"D"});

  std::map<CFG::nonterminal_type, std::vector<CFG_production::body_type>>
      productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  LL_grammar grammar("common_tokens", "P", productions);
  L_attributed_SDD sdd(grammar);
  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{"$1.symbol_table",
                         {"$0.symbol_table"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{"$0.type",
                         {"$2.type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{
          "$0.associated_symbol_table",
          {},
          [](const auto &) -> std::optional<std::any> {
            return std::make_any<std::shared_ptr<cyy::compiler::symbol_table>>(
                nullptr);
          }});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{"$0.width",
                         {"$2.width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_inherited_attribute(
      production_vector[2],
      SDD::semantic_rule{"$2.inh_type",
                         {"$1.type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_inherited_attribute(
      production_vector[2],
      SDD::semantic_rule{"$2.inh_width",
                         {"$1.width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
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
                           return *arguments.at(0);
                         }});
  sdd.add_synthesized_attribute(
      production_vector[6],
      SDD::semantic_rule{"$0.width",
                         {"$0.inh_width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_inherited_attribute(
      production_vector[7],
      SDD::semantic_rule{"$4.inh_type",
                         {"$0.inh_type"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_inherited_attribute(
      production_vector[7],
      SDD::semantic_rule{"$4.inh_width",
                         {"$0.inh_width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});
  sdd.add_synthesized_attribute(
      production_vector[7],
      SDD::semantic_rule{
          "$0.type",
          {"$2", "$4.type"},
          [](const auto &arguments) -> std::optional<std::any> {
            size_t element_number = 0;

            for (auto c : std::any_cast<token>(*(arguments.at(0))).lexeme) {
              element_number =
                  element_number * 10 + static_cast<size_t>(c - '0');
            }

            return std::make_any<
                std::shared_ptr<cyy::compiler::type_expression::expression>>(
                std::make_shared<cyy::compiler::type_expression::array_type>(
                    std::any_cast<std::shared_ptr<
                        cyy::compiler::type_expression::expression>>(
                        *(arguments.at(1))),
                    element_number));
          }});

  sdd.add_synthesized_attribute(
      production_vector[7],
      SDD::semantic_rule{
          "$0.width",
          {"$2", "$4.width"},
          [](const auto &arguments) -> std::optional<std::any> {
            size_t element_number = 0;

            for (auto c : std::any_cast<token>(*(arguments.at(0))).lexeme) {
              element_number =
                  element_number * 10 + static_cast<size_t>(c - '0');
            }

            return std::make_any<size_t>(
                element_number * std::any_cast<size_t>(*(arguments.at(1))));
          }});

  sdd.add_inherited_attribute(
      production_vector[11],
      SDD::semantic_rule{
          "$1.offset", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<size_t>(0);
          }});

  sdd.add_inherited_attribute(
      production_vector[11],
      SDD::semantic_rule{
          "$1.symbol_table", {}, [](const auto &) -> std::optional<std::any> {
            return std::make_any<std::shared_ptr<symbol_table>>(
                std::make_shared<symbol_table>());
          }});

  sdd.add_synthesized_attribute(
      production_vector[11],
      SDD::semantic_rule{"$0.symbol_table",
                         {"$1.symbol_table"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *arguments.at(0);
                         }});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{
          "$4.symbol_table",
          {"$0.symbol_table", "$0.offset", "$1.type", "$1.width",
           "$1.associated_symbol_table", "$2"},
          [](const auto &arguments) -> std::optional<std::any> {
            symbol_table_entry e;
            e.lexeme = std::any_cast<token>(*(arguments.at(5))).lexeme;
            e.type =
                std::any_cast<std::shared_ptr<type_expression::expression>>(
                    *(arguments.at(2)));
            e.relative_address = std::any_cast<size_t>(*(arguments.at(1)));
            e.width = std::any_cast<size_t>(*(arguments.at(3)));
            e.associated_symbol_table =
                std::any_cast<std::shared_ptr<symbol_table>>(
                    *(arguments.at(4)));
            auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                *(arguments.at(0)));
            if (!table->add_entry(e)) {
              throw std::runtime_error("add entry failed");
            }

            auto type_name_ptr =
                std::dynamic_pointer_cast<type_expression::type_name>(e.type);
            if (type_name_ptr) {
              table->add_type(type_name_ptr, e.associated_symbol_table);
            }
            return std::make_any<std::shared_ptr<symbol_table>>(table);
          }});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{"$4.offset",
                         {"$0.offset", "$1.width"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return std::make_any<size_t>(
                               std::any_cast<size_t>(*(arguments.at(0))) +
                               std::any_cast<size_t>(*(arguments.at(1))));
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

  SUBCASE("relative addresses") {
    std::vector<token> tokens;
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::FLOAT), "float", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "x", {}});
    tokens.push_back(token{';', ";", {}});

    auto attributes = sdd.run(tokens, {"P.symbol_table"});
    REQUIRE(attributes);
    auto const &table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["P.symbol_table"]);
    REQUIRE(table->get_entry("x")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(table->get_entry("x")->relative_address == 0);
  }

  SUBCASE("records") {
    sdd.add_inherited_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$3.symbol_table", {}, [](const auto &) -> std::optional<std::any> {
              return std::make_any<std::shared_ptr<symbol_table>>(
                  std::make_shared<symbol_table>());
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
              return *arguments.at(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$0.associated_symbol_table",
            {"$3.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              return *arguments.at(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$0.type",
            {"$3.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              std::vector<symbol_table_entry> sorted_entries;

              std::any_cast<std::shared_ptr<symbol_table>>(*(arguments.at(0)))
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

    auto attributes = sdd.run(tokens, {"P.symbol_table"});
    REQUIRE(attributes);

    auto const table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["P.symbol_table"]);

    auto e = table->get_entry("q");
    REQUIRE(e.has_value());

    REQUIRE(e->associated_symbol_table);
    REQUIRE(e->associated_symbol_table->get_entry("tag")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(e->associated_symbol_table->get_entry("tag")->relative_address ==
            0);
    REQUIRE(e->associated_symbol_table->get_entry("x"));
    REQUIRE(e->associated_symbol_table->get_entry("x")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(e->associated_symbol_table->get_entry("x")->relative_address == 4);
    REQUIRE(e->associated_symbol_table->get_entry("y"));
    REQUIRE(e->associated_symbol_table->get_entry("y")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::FLOAT)));
    REQUIRE(e->associated_symbol_table->get_entry("y")->relative_address == 12);

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

    REQUIRE(e->type->equivalent_with(
        cyy::compiler::type_expression::record_type(field_types)));
  }

  SUBCASE("classes") {
    sdd.add_inherited_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$5.symbol_table", {}, [](const auto &) -> std::optional<std::any> {
              return std::make_any<std::shared_ptr<symbol_table>>(
                  std::make_shared<symbol_table>());
            }});

    sdd.add_inherited_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$5.offset", {}, [](const auto &) -> std::optional<std::any> {
              return std::make_any<size_t>(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$0.width",
            {"$5.offset"},
            [](const auto &arguments) -> std::optional<std::any> {
              return *arguments.at(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$0.associated_symbol_table",
            {"$5.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              return *arguments.at(0);
            }});

    sdd.add_synthesized_attribute(
        production_vector[8],
        SDD::semantic_rule{
            "$0.type",
            {"$0.symbol_table", "$2", "$3.class_name", "$5.symbol_table"},
            [](const auto &arguments) -> std::optional<std::any> {
              std::vector<symbol_table_entry> sorted_entries;

              auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                  *arguments.at(3));
              table->foreach_entry([&sorted_entries](auto const &e) {
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
                  std::any_cast<token>(*arguments.at(1)).lexeme;
              auto const &parent_class_name =
                  std::any_cast<std::string>(*(arguments.at(2)));
              std::shared_ptr<cyy::compiler::type_expression::expression>
                  parent_class;
              std::shared_ptr<cyy::compiler::symbol_table>
                  parent_class_symbol_table;
              if (!parent_class_name.empty()) {
                auto const &scope_symbol_table =
                    std::any_cast<std::shared_ptr<symbol_table>>(
                        *arguments.at(0));
                auto parent_class_opt =
                    scope_symbol_table->get_type(parent_class_name);
                if (!parent_class_opt) {
                  throw cyy::compiler::exception::no_parent_class(
                      parent_class_name);
                }
                std::tie(parent_class, parent_class_symbol_table) =
                    *parent_class_opt;
                table->add_relative_address_offset(
                    parent_class_symbol_table->get_total_width());
                table->set_prev_table(parent_class_symbol_table);
              }
              auto class_type = std::make_shared<
                  cyy::compiler::type_expression::type_name>(
                  class_name,
                  std::make_shared<cyy::compiler::type_expression::class_type>(
                      parent_class, field_types));

              return std::make_any<
                  std::shared_ptr<cyy::compiler::type_expression::expression>>(
                  class_type);
            }});

    sdd.add_synthesized_attribute(
        production_vector[9],
        SDD::semantic_rule{
            "$0.class_name",
            {"$2"},
            [](const auto &arguments) -> std::optional<std::any> {
              return std::make_any<std::string>(
                  std::any_cast<token>(*(arguments.at(0))).lexeme);
            }});
    sdd.add_synthesized_attribute(
        production_vector[10],
        SDD::semantic_rule{
            "$0.class_name", {}, [](const auto &) -> std::optional<std::any> {
              return std::make_any<std::string>("");
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
        token{static_cast<symbol_type>(common_token::id), "x", {}});
    tokens.push_back(token{';', ";", {}});
    tokens.push_back(token{'}', "}", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "q", {}});
    tokens.push_back(token{';', ";", {}});

    auto attributes = sdd.run(tokens, {"P.symbol_table"});
    REQUIRE(attributes);

    auto table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["P.symbol_table"]);

    auto e = table->get_entry("q");
    REQUIRE(e.has_value());
    REQUIRE(e->associated_symbol_table);
    REQUIRE(e->associated_symbol_table->get_entry("x"));
    REQUIRE(e->associated_symbol_table->get_entry("x")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(e->associated_symbol_table->get_entry("x")->relative_address == 0);

    std::vector<
        std::pair<std::string,
                  std::shared_ptr<cyy::compiler::type_expression::expression>>>
        field_types;
    field_types.emplace_back(
        "x",
        std::shared_ptr<cyy::compiler::type_expression::expression>(
            std::make_shared<cyy::compiler::type_expression::basic_type>(
                cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(e->type->equivalent_with(
        cyy::compiler::type_expression::class_type(nullptr, field_types)));

    tokens.push_back(
        token{static_cast<symbol_type>(common_token::CLASS), "class", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "B", {}});
    tokens.push_back(token{':', ":", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "A", {}});
    tokens.push_back(token{'{', "{", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::INT), "int", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "y", {}});
    tokens.push_back(token{';', ";", {}});
    tokens.push_back(token{'}', "}", {}});
    tokens.push_back(
        token{static_cast<symbol_type>(common_token::id), "p", {}});
    tokens.push_back(token{';', ";", {}});

    attributes = sdd.run(tokens, {"P.symbol_table"});
    REQUIRE(attributes);

    table = std::any_cast<std::shared_ptr<symbol_table>>(
        attributes.value()["P.symbol_table"]);

    e = table->get_entry("p");
    REQUIRE(e.has_value());
    REQUIRE(e->associated_symbol_table);
    REQUIRE(e->associated_symbol_table->get_entry("x"));
    REQUIRE(e->associated_symbol_table->get_entry("x")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(e->associated_symbol_table->get_entry("x")->relative_address == 0);

    REQUIRE(e->associated_symbol_table->get_entry("y"));
    REQUIRE(e->associated_symbol_table->get_entry("y")->type->equivalent_with(
        cyy::compiler::type_expression::basic_type(
            cyy::compiler::type_expression::basic_type::type_enum::INT)));
    REQUIRE(e->associated_symbol_table->get_entry("y")->relative_address == 4);
  }
}
