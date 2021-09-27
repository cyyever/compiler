/*!
 * \file declaration_sdd.cpp
 *
 * \brief
 */

#include "declaration_sdd.hpp"

#include <cyy/computation/lang/common_tokens.hpp>

#include "grammar.hpp"

namespace cyy::compiler::example_grammar {
  declaration_SDD::declaration_SDD() : grammar(get_declaration_grammar()) {

    auto id = static_cast<CFG::terminal_type>(common_token::id);
    sdd = std::make_unique<L_attributed_SDD>(*grammar);

    for (auto const &[head, bodies] : grammar->get_productions()) {
      for (auto const &body : bodies) {
        if (head == "S") {
          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{
                  "$1.symbol_table", {}, [](const auto &) {
                    return std::make_any<std::shared_ptr<symbol_table>>(
                        std::make_shared<symbol_table>());
                  }});

          sdd->add_synthesized_attribute(
              {head, body},
              SDD::semantic_rule{"$0.symbol_table",
                                 {"$1.symbol_table"},
                                 SDD::semantic_rule::copy_action});
          continue;
        }
        if (head == "declarations" && !body.empty()) {
          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{"$1.symbol_table",
                                 {"$0.symbol_table"},
                                 SDD::semantic_rule::copy_action});

          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{
                  "$4.symbol_table",
                  {"$0.symbol_table", "$1.type", "$2"},
                  [](const auto &arguments) -> std::optional<std::any> {
                    auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                        *(arguments.at(0)));
                    symbol_table::symbol_entry e;
                    e.lexeme = std::any_cast<token>(*(arguments.at(2))).lexeme;
                    e.type = std::any_cast<
                        std::shared_ptr<type_expression::expression>>(
                        *(arguments.at(1)));
                    e.relative_address = table->get_next_relative_address();
                    if (!table->add_symbol(e)) {
                      throw std::runtime_error("add symbol failed");
                    }

                    auto type_name_ptr =
                        std::dynamic_pointer_cast<type_expression::type_name>(
                            e.type);
                    if (type_name_ptr) {
                      symbol_table::type_entry t;
                      t.type = type_name_ptr;
                      table->add_type(t);
                    }
                    return std::make_any<std::shared_ptr<symbol_table>>(table);
                  }});
          continue;
        }
        if (head == "full_type") {
          if (body[0] == "type") { // type  array_part

            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$0.type", {"$2.type"}, SDD::semantic_rule::copy_action});

            sdd->add_inherited_attribute(
                {head, body},
                SDD::semantic_rule{"$2.inh_type",
                                   {"$1.type"},
                                   SDD::semantic_rule::copy_action});
            continue;
          } else if (body[0] == static_cast<CFG::terminal_type>(
                                    common_token::record)) { // record
            sdd->add_inherited_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$3.symbol_table",
                    {},
                    [](const auto &) -> std::optional<std::any> {
                      return std::make_any<std::shared_ptr<symbol_table>>(
                          std::make_shared<symbol_table>());
                    }});

            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$0.type",
                    {"$3.symbol_table"},
                    [](const auto &arguments) -> std::optional<std::any> {
                      auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                          *(arguments.at(0)));

                      return std::make_any<
                          std::shared_ptr<type_expression::expression>>(
                          std::make_shared<type_expression::record_type>(
                              table));
                    }});
            continue;
          }
        }
        if (head == "type") {
          if (body[0] == static_cast<CFG::terminal_type>(common_token::INT)) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$0.type", {}, [](const auto &) -> std::optional<std::any> {
                      return std::make_any<
                          std::shared_ptr<type_expression::expression>>(
                          std::make_shared<type_expression::basic_type>(
                              type_expression::basic_type::type_enum::INT));
                    }});
            continue;
          }
          if (body[0] == static_cast<CFG::terminal_type>(common_token::FLOAT)) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$0.type", {}, [](const auto &) -> std::optional<std::any> {
                      return std::make_any<
                          std::shared_ptr<type_expression::expression>>(
                          std::make_shared<type_expression::basic_type>(
                              type_expression::basic_type::type_enum::FLOAT));
                    }});
            continue;
          }
        }
      }
    }
  }

  std::shared_ptr<symbol_table> declaration_SDD::run(token_span span) {
    sdd->run(span, {});
    return {};
  }
} // namespace cyy::compiler::example_grammar
