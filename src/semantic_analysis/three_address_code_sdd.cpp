/*!
 * \file three_address_code_sdd.cpp
 *
 * \brief
 */
#include <cyy/computation/lang/common_tokens.hpp>
#include <cyy/computation/context_free_lang/common_grammar.hpp>

#include "operator.hpp"
#include "semantic_analysis/l_attributed_sdd.hpp"
#include "three_address_code_sdd.hpp"
#include <fmt/format.h>

namespace cyy::compiler {
  three_address_code_SDD::three_address_code_SDD() {

    auto production_set=cyy::computation::get_expression_productions();
       production_set[ "S"].emplace(CFG_production::body_type{
                 static_cast<CFG::terminal_type>(common_token::id), '=', "E"});
    std::vector<CFG_production> production_vector;

    production_vector.emplace_back(
        "S", CFG_production::body_type{
                 static_cast<CFG::terminal_type>(common_token::id), '=', "E"});
    production_vector.emplace_back("E",
                                   CFG_production::body_type{"F", '+', "E"});
    production_vector.emplace_back("F", CFG_production::body_type{'-', "F"});
    production_vector.emplace_back("F",
                                   CFG_production::body_type{'(', "E", ')'});
    production_vector.emplace_back(
        "F", CFG_production::body_type{
                 static_cast<CFG::terminal_type>(common_token::id)});

    CFG::production_set_type productions;
    for (auto const &production : production_vector) {
      productions[production.get_head()].emplace(production.get_body());
    }
    // grammar = std::make_unique<SLR_grammar>("common_tokens", "S",
    // productions);
    grammar = std::make_unique<SLR_grammar>("common_tokens", "F", productions);
    sdd = std::make_unique<S_attributed_SDD>(*grammar);
    size_t tmp_name_index = 0;

#if 0
    sdd->add_synthesized_attribute(
        production_vector[0],
        SDD::semantic_rule{
            "$0.addr",
            {"$1", "$3.addr"},
            [this](const auto &arguments) -> std::optional<std::any> {
              auto name = std::make_shared<IR::three_address_code::name>(
                  table->create_and_get_symbol(
                      std::any_cast<token>(*arguments[0]).lexeme));
              auto address = std::any_cast<IR::three_address_code::address_ptr>(
                  *arguments[1]);

              instruction_sequence.emplace_back(
                  std::make_shared<IR::three_address_code::copy_instruction>(
                      name, address));
              return std::dynamic_pointer_cast<IR::three_address_code::address>(
                  name);
            }});

    sdd->add_synthesized_attribute(
        production_vector[1],
        SDD::semantic_rule{
            "$0.addr",
            {"$1.addr", "$3.addr"},
            [this, &tmp_name_index](
                const auto &arguments) -> std::optional<std::any> {
              auto left = std::any_cast<IR::three_address_code::address_ptr>(
                  *arguments[0]);
              auto right = std::any_cast<IR::three_address_code::address_ptr>(
                  *arguments[1]);
              auto result_name = std::make_shared<IR::three_address_code::name>(
                  table->create_temporary_symbol(
                      fmt::format("tmp_{}", tmp_name_index++)));
              auto instruction = std::make_shared<
                  IR::three_address_code::
                      binary_arithmetic_assignment_instruction>();
              instruction->op =
                  cyy::compiler::binary_arithmetic_operator::addition;
              instruction->result = result_name;
              instruction->left = left;
              instruction->right = right;
              instruction_sequence.emplace_back(instruction);
              return std::dynamic_pointer_cast<IR::three_address_code::address>(
                  result_name);
            }});

    sdd->add_synthesized_attribute(
        production_vector[2],
        SDD::semantic_rule{
            "$0.addr",
            {"$2.addr"},
            [this, &tmp_name_index](
                const auto &arguments) -> std::optional<std::any> {
              auto result_name = std::make_shared<IR::three_address_code::name>(
                  table->create_temporary_symbol(
                      fmt::format("tmp_{}", tmp_name_index++)));
              auto operand = std::any_cast<IR::three_address_code::address_ptr>(
                  *arguments[0]);
              auto instruction = std::make_shared<
                  IR::three_address_code::
                      unary_arithmetic_assignment_instruction>();
              instruction->op = cyy::compiler::unary_arithmetic_operator::minus;
              instruction->result = result_name;
              instruction->operand = operand;
              instruction_sequence.emplace_back(instruction);
              return std::dynamic_pointer_cast<IR::three_address_code::address>(
                  result_name);
            }});
    sdd->add_synthesized_attribute(
        production_vector[3],
        SDD::semantic_rule{
            "$0.addr", {"$2.addr"}, SDD::semantic_rule::copy_action});
#endif
    sdd->add_synthesized_attribute(
        production_vector[4],
        SDD::semantic_rule{
            "$0.addr",
            {"$1"},
            [this](const auto &arguments) -> std::optional<std::any> {
              auto name = std::make_shared<IR::three_address_code::name>(
                  table->create_and_get_symbol(
                      std::any_cast<token>(*arguments[0]).lexeme));
              return std::dynamic_pointer_cast<IR::three_address_code::address>(
                  name);
            }});
  }
  bool three_address_code_SDD::run(token_span span,
                                   std::shared_ptr<symbol_table> table_) {
    table = table_;
    instruction_sequence.clear();
    return sdd->run(span, {"S.addr"}).has_value();
  }
} // namespace cyy::compiler
