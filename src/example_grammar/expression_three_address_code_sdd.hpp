/*!
 * \file expression_three_address_code_sdd.hpp
 *
 * \brief
 */
#pragma once
#include <cyy/computation/context_free_lang/slr_grammar.hpp>

#include "intermediate_representation/three_address_code.hpp"
#include "semantic_analysis/s_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"
#include "token.hpp"

namespace cyy::compiler::example_grammar {

  class expression_three_address_code_SDD {
  public:
    expression_three_address_code_SDD();
    bool run(token_span span, std::shared_ptr<symbol_table> table_ = {});

  public:
    std::vector<std::shared_ptr<IR::three_address_code::instruction>>
        instruction_sequence;

  private:
    template <typename operator_type>
    auto generate_binary_assignment_rule(operator_type op) {

      return SDD::semantic_rule{
          "$0.addr",
          {"$1.addr", "$3.addr"},
          [this, op](const auto &arguments) -> std::optional<std::any> {
            auto left = std::any_cast<IR::three_address_code::address_ptr>(
                *arguments[0]);
            auto right = std::any_cast<IR::three_address_code::address_ptr>(
                *arguments[1]);
            auto result_name = std::make_shared<IR::three_address_code::name>(
                table->create_temporary_symbol(
                    std::format("tmp_{}", tmp_name_index++)));
            if constexpr (std::is_same_v<operator_type,
                                         binary_arithmetic_operator>) {
              auto instruction = std::make_shared<
                  IR::three_address_code::
                      binary_arithmetic_assignment_instruction>();
              instruction->op = op;
              instruction->result = result_name;
              instruction->left = left;
              instruction->right = right;
              instruction_sequence.emplace_back(instruction);
            }
            return std::dynamic_pointer_cast<IR::three_address_code::address>(
                result_name);
          }};
    }
    template <typename operator_type>
    auto generate_unary_assignment_rule(operator_type op) {

      return SDD::semantic_rule{
          "$0.addr",
          {"$2.addr"},
          [this, op](const auto &arguments) -> std::optional<std::any> {
            auto result_name = std::make_shared<IR::three_address_code::name>(
                table->create_temporary_symbol(
                    std::format("tmp_{}", tmp_name_index++)));
            auto operand = std::any_cast<IR::three_address_code::address_ptr>(
                *arguments[0]);
            if constexpr (std::is_same_v<operator_type,
                                         unary_arithmetic_operator>) {
              auto instruction = std::make_shared<
                  IR::three_address_code::
                      unary_arithmetic_assignment_instruction>();
              instruction->op = op;
              instruction->result = result_name;
              instruction->operand = operand;
              instruction_sequence.emplace_back(instruction);
            }
            return std::dynamic_pointer_cast<IR::three_address_code::address>(
                result_name);
          }};
    }

  private:
    std::unique_ptr<S_attributed_SDD> sdd;
    std::shared_ptr<symbol_table> table;
    std::shared_ptr<SLR_grammar> grammar;
    size_t tmp_name_index{1};
  };

} // namespace cyy::compiler::example_grammar
