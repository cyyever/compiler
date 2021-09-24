/*!
 * \file three_address_code_sdd.cpp
 *
 * \brief
 */
#include "three_address_code_sdd.hpp"

#include <cassert>

#include <cyy/computation/context_free_lang/common_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>
#include <fmt/format.h>

#include "operator.hpp"
#include "semantic_analysis/l_attributed_sdd.hpp"

namespace cyy::compiler {
  three_address_code_SDD::three_address_code_SDD() {

    auto production_set = cyy::computation::get_expression_productions();
    production_set["S"].emplace(CFG_production::body_type{
        static_cast<CFG::terminal_type>(common_token::id), '=', "E"});

    grammar =
        std::make_unique<SLR_grammar>("common_tokens", "S", production_set);
    sdd = std::make_unique<S_attributed_SDD>(*grammar);
    auto id = static_cast<CFG::terminal_type>(common_token::id);

    for (auto const &body : grammar->get_bodies("F")) {
      if (body == CFG_production::body_type{id}) {
        sdd->add_synthesized_attribute(
            {"F", body},
            SDD::semantic_rule{
                "$0.addr",
                {"$1"},
                [this](const auto &arguments) -> std::optional<std::any> {
                  assert(table);
                  auto name = std::make_shared<IR::three_address_code::name>(
                      table->create_and_get_symbol(
                          std::any_cast<token>(*arguments[0]).lexeme));
                  return std::dynamic_pointer_cast<
                      IR::three_address_code::address>(name);
                }});
        continue;
      }
      if (body[0] == '(') {
        sdd->add_synthesized_attribute(
            {"F", body}, SDD::semantic_rule{"$0.addr",
                                            {"$2.addr"},
                                            SDD::semantic_rule::copy_action});
        continue;
      }
    }

    for (auto const &body : grammar->get_bodies("T")) {
      if (body[0] == "F") {
        sdd->add_synthesized_attribute(
            {"T", body}, SDD::semantic_rule{"$0.addr",
                                            {"$1.addr"},
                                            SDD::semantic_rule::copy_action});
        continue;
      }
      if (body[0] == '-') {
        sdd->add_synthesized_attribute(
            {"T", body},
            SDD::semantic_rule{
                "$0.addr",
                {"$2.addr"},
                [this](const auto &arguments) -> std::optional<std::any> {
                  auto result_name =
                      std::make_shared<IR::three_address_code::name>(
                          table->create_temporary_symbol(
                              fmt::format("tmp_{}", tmp_name_index++)));
                  auto operand =
                      std::any_cast<IR::three_address_code::address_ptr>(
                          *arguments[0]);
                  auto instruction = std::make_shared<
                      IR::three_address_code::
                          unary_arithmetic_assignment_instruction>();
                  instruction->op =
                      cyy::compiler::unary_arithmetic_operator::minus;
                  instruction->result = result_name;
                  instruction->operand = operand;
                  instruction_sequence.emplace_back(instruction);
                  return std::dynamic_pointer_cast<
                      IR::three_address_code::address>(result_name);
                }});
        continue;
      }
      if (body[1] == '*') {
        sdd->add_synthesized_attribute(
            {"T", body},
            SDD::semantic_rule{
                "$0.addr",
                {"$1.addr", "$3.addr"},
                [this](const auto &arguments) -> std::optional<std::any> {
                  return generate_binary_assignment_instruction<
                      binary_arithmetic_operator>(
                      binary_arithmetic_operator::multiplication, arguments);
                }});
        continue;
      }
    }
    for (auto const &body : grammar->get_bodies("E")) {
      if (body[0] == "T") {
        sdd->add_synthesized_attribute(
            {"E", body}, SDD::semantic_rule{"$0.addr",
                                            {"$1.addr"},
                                            SDD::semantic_rule::copy_action});
        continue;
      }
      if (body[1] == '+') {
        sdd->add_synthesized_attribute(
            {"E", body},
            SDD::semantic_rule{
                "$0.addr",
                {"$1.addr", "$3.addr"},
                [this](const auto &arguments) -> std::optional<std::any> {
                  return generate_binary_assignment_instruction<
                      binary_arithmetic_operator>(
                      binary_arithmetic_operator::plus, arguments);
                }});
        continue;
      }
      if (body[1] == '-') {
        sdd->add_synthesized_attribute(
            {"E", body},
            SDD::semantic_rule{
                "$0.addr",
                {"$1.addr", "$3.addr"},
                [this](const auto &arguments) -> std::optional<std::any> {
                  return generate_binary_assignment_instruction<
                      binary_arithmetic_operator>(
                      binary_arithmetic_operator::minus, arguments);
                }});
        continue;
      }
    }
    for (auto const &body : grammar->get_bodies("S")) {
      sdd->add_synthesized_attribute(
          {"S", body},
          SDD::semantic_rule{
              "$0.addr",
              {"$1", "$3.addr"},
              [this](const auto &arguments) -> std::optional<std::any> {
                auto name = std::make_shared<IR::three_address_code::name>(
                    table->create_and_get_symbol(
                        std::any_cast<token>(*arguments[0]).lexeme));
                auto address =
                    std::any_cast<IR::three_address_code::address_ptr>(
                        *arguments[1]);

                instruction_sequence.emplace_back(
                    std::make_shared<IR::three_address_code::copy_instruction>(
                        name, address));
                return std::dynamic_pointer_cast<
                    IR::three_address_code::address>(name);
              }});
    }
  }

  bool three_address_code_SDD::run(token_span span,
                                   std::shared_ptr<symbol_table> table_) {
    table = table_;
    if (!table) {
      table = std::make_shared<symbol_table>();
    }
    instruction_sequence.clear();
    tmp_name_index = 0;
    return sdd->run(span, {"S.addr"}).has_value();
  }
} // namespace cyy::compiler
