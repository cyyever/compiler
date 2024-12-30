/*!
 * \file declaration_sdd.cpp
 *
 * \brief
 */

#include "declaration_sdd.hpp"

#include <cyy/algorithm/alphabet/common_tokens.hpp>

#include "grammar.hpp"

namespace cyy::compiler::example_grammar {
  using cyy::algorithm::common_token;
  declaration_SDD::declaration_SDD() : grammar(get_declaration_grammar()) {

    sdd = std::make_unique<L_attributed_SDD>(*grammar);

    for (auto const &[head, bodies] : grammar->get_productions()) {
      for (auto const &body : bodies) {
        if (head == "S") {
          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{
                  .result_attribute = "$1.symbol_table",
                  .arguments = {},
                  .action = [](const auto &) {
                    return std::make_any<std::shared_ptr<symbol_table>>(
                        std::make_shared<symbol_table>());
                  }});

          sdd->add_synthesized_attribute(
              {head, body},
              SDD::semantic_rule{.result_attribute = "$0.symbol_table",
                                 .arguments = {"$1.symbol_table"},
                                 .action = SDD::semantic_rule::copy_action});
          continue;
        }
        if (head == "declarations" && !body.empty()) {
          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{.result_attribute = "$1.symbol_table",
                                 .arguments = {"$0.symbol_table"},
                                 .action = SDD::semantic_rule::copy_action});

          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{
                  .result_attribute = "$4.symbol_table",
                  .arguments = {"$0.symbol_table", "$1.type", "$2"},
                  .action =
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
                SDD::semantic_rule{.result_attribute = "$0.type",
                                   .arguments = {"$2.type"},
                                   .action = SDD::semantic_rule::copy_action});

            sdd->add_inherited_attribute(
                {head, body},
                SDD::semantic_rule{.result_attribute = "$2.inh_type",
                                   .arguments = {"$1.type"},
                                   .action = SDD::semantic_rule::copy_action});
            continue;
          } else if (body[0] == static_cast<CFG::terminal_type>(
                                    common_token::record)) { // record
            sdd->add_inherited_attribute(
                {head, body},
                SDD::semantic_rule{
                    .result_attribute = "$3.symbol_table",
                    .arguments = {},
                    .action = [](const auto &) -> std::optional<std::any> {
                      return std::make_any<std::shared_ptr<symbol_table>>(
                          std::make_shared<symbol_table>());
                    }});

            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    .result_attribute = "$0.type",
                    .arguments = {"$3.symbol_table"},
                    .action =
                        [](const auto &arguments) -> std::optional<std::any> {
                      auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                          *(arguments.at(0)));

                      return std::make_any<
                          std::shared_ptr<type_expression::expression>>(
                          std::make_shared<type_expression::record_type>(
                              table));
                    }});
            continue;
          } else if (body[0] == static_cast<CFG::terminal_type>(
                                    common_token::CLASS)) { // class
            sdd->add_inherited_attribute(
                {head, body},
                SDD::semantic_rule{
                    .result_attribute = "$5.symbol_table",
                    .arguments = {},
                    .action = [](const auto &) -> std::optional<std::any> {
                      return std::make_any<std::shared_ptr<symbol_table>>(
                          std::make_shared<symbol_table>());
                    }});

            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    .result_attribute = "$0.type",
                    .arguments = {"$0.symbol_table", "$2", "$3.class_name",
                                  "$5.symbol_table"},
                    .action =
                        [](const auto &arguments) -> std::optional<std::any> {
                      auto table = std::any_cast<std::shared_ptr<symbol_table>>(
                          *arguments.at(3));

                      auto const &class_name =
                          std::any_cast<token>(*arguments.at(1)).lexeme;
                      auto const &parent_class_name =
                          std::any_cast<std::string>(*(arguments.at(2)));
                      std::shared_ptr<type_expression::class_type> parent_class;
                      std::shared_ptr<symbol_table> parent_class_symbol_table;
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
                        parent_class = std::dynamic_pointer_cast<
                            type_expression::class_type>(
                            std::dynamic_pointer_cast<
                                type_expression::type_name>(
                                parent_class_opt->type)
                                ->get_type());
                        parent_class_symbol_table =
                            parent_class->get_symbol_table();
                        size_t total_width = 0;
                        for (const auto &e :
                             parent_class_symbol_table->get_symbol_view()) {
                          total_width += e.type->get_width();
                        }
                        table->add_relative_address_offset(total_width);
                        table->set_prev_table(parent_class_symbol_table);
                      }
                      auto class_type =
                          std::make_shared<type_expression::type_name>(
                              class_name,
                              std::make_shared<type_expression::class_type>(
                                  parent_class, table));

                      return std::make_any<
                          std::shared_ptr<type_expression::expression>>(
                          class_type);
                    }});
            continue;
          }
        }
        if (head == "parent_class") {
          if (body.empty()) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    .result_attribute = "$0.class_name",
                    .arguments = {},
                    .action = [](const auto &) -> std::optional<std::any> {
                      return std::make_any<std::string>("");
                    }});
            continue;
          }
          sdd->add_synthesized_attribute(
              {head, body},
              SDD::semantic_rule{
                  .result_attribute = "$0.class_name",
                  .arguments = {"$2"},
                  .action = [](const auto &arguments) {
                    return std::make_any<std::string>(
                        std::any_cast<token>(*(arguments.at(0))).lexeme);
                  }});
          continue;
        }
        if (head == "type") {
          if (body[0] == static_cast<CFG::terminal_type>(common_token::INT)) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    .result_attribute = "$0.type",
                    .arguments = {},
                    .action = [](const auto &) -> std::optional<std::any> {
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
                    .result_attribute = "$0.type",
                    .arguments = {},
                    .action = [](const auto &) -> std::optional<std::any> {
                      return std::make_any<
                          std::shared_ptr<type_expression::expression>>(
                          std::make_shared<type_expression::basic_type>(
                              type_expression::basic_type::type_enum::FLOAT));
                    }});
            continue;
          }
        }
        if (head == "array_part") {
          if (body.empty()) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{.result_attribute = "$0.type",
                                   .arguments = {"$0.inh_type"},
                                   .action = SDD::semantic_rule::copy_action});
            continue;
          }

          sdd->add_inherited_attribute(
              {head, body},
              SDD::semantic_rule{.result_attribute = "$4.inh_type",
                                 .arguments = {"$0.inh_type"},
                                 .action = SDD::semantic_rule::copy_action});

          sdd->add_synthesized_attribute(
              {head, body},
              SDD::semantic_rule{
                  .result_attribute = "$0.type",
                  .arguments = {"$2", "$4.type"},
                  .action =
                      [](const auto &arguments) -> std::optional<std::any> {
                    size_t element_number = std::stoll(
                        std::any_cast<token>(*(arguments.at(0))).lexeme);

                    return std::make_any<
                        std::shared_ptr<type_expression::expression>>(
                        std::make_shared<type_expression::array_type>(
                            std::any_cast<
                                std::shared_ptr<type_expression::expression>>(
                                *(arguments.at(1))),
                            element_number));
                  }});
        }
      }
    }
  }

  std::shared_ptr<symbol_table> declaration_SDD::run(token_span span) {
    auto attributes_opt = sdd->run(span, {"S.symbol_table"});
    if (!attributes_opt) {

      return {};
    }
    return std::any_cast<std::shared_ptr<symbol_table>>(
        (*attributes_opt)["S.symbol_table"]);
  }
} // namespace cyy::compiler::example_grammar
