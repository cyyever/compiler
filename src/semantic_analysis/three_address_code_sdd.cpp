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
    auto id = static_cast<CFG::terminal_type>(common_token::id);
    production_set["S"].emplace(CFG_production::body_type{id, '=', "E"});

    grammar =
        std::make_unique<SLR_grammar>("common_tokens", "S", production_set);
    sdd = std::make_unique<S_attributed_SDD>(*grammar);

    for (auto const &[head, bodies] : grammar->get_productions()) {
      for (auto const &body : bodies) {
        if (head == "S") {
          sdd->add_synthesized_attribute(
              {"S", body},
              SDD::semantic_rule{
                  {}, {"$1", "$3.addr"}, [this](const auto &arguments) {
                    auto name = std::make_shared<IR::three_address_code::name>(
                        table->create_and_get_symbol(
                            std::any_cast<token>(*arguments[0]).lexeme));
                    auto address =
                        std::any_cast<IR::three_address_code::address_ptr>(
                            *arguments[1]);

                    instruction_sequence.emplace_back(
                        std::make_shared<
                            IR::three_address_code::copy_instruction>(name,
                                                                      address));
                    return std::optional<std::any>{};
                  }});
          break;
        }
        if (body.size() == 1) {
          if (body[0] == id) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$0.addr",
                    {"$1"},
                    [this](const auto &arguments) -> std::optional<std::any> {
                      assert(table);
                      auto name =
                          std::make_shared<IR::three_address_code::name>(
                              table->create_and_get_symbol(
                                  std::any_cast<token>(*arguments[0]).lexeme));
                      return std::dynamic_pointer_cast<
                          IR::three_address_code::address>(name);
                    }});
            continue;
          }

          if (body[0].is_nonterminal()) {
            sdd->add_synthesized_attribute(
                {head, body},
                SDD::semantic_rule{
                    "$0.addr", {"$1.addr"}, SDD::semantic_rule::copy_action});
            continue;
          }
        }
        if (body[0] == '(') {
          sdd->add_synthesized_attribute(
              {head, body},
              SDD::semantic_rule{
                  "$0.addr", {"$2.addr"}, SDD::semantic_rule::copy_action});
          continue;
        }
        if (body.size() == 3) {
          if (body[1].is_terminal()) {

            std::map<char, binary_arithmetic_operator> binary_arithmetic_ops;
            binary_arithmetic_ops['+'] = binary_arithmetic_operator::plus;
            binary_arithmetic_ops['-'] = binary_arithmetic_operator::minus;
            binary_arithmetic_ops['*'] =
                binary_arithmetic_operator::multiplication;
            binary_arithmetic_ops['/'] = binary_arithmetic_operator::division;
            auto it = binary_arithmetic_ops.find(body[1].get_terminal());
            if (it != binary_arithmetic_ops.end()) {
              sdd->add_synthesized_attribute(
                  {head, body}, generate_binary_assignment_rule(it->second));
              continue;
            }
          }
        }
        if (head == "L" && body.size() == 4) {
          std::map<char, binary_logical_operator> binary_logical_ops;
          binary_logical_ops['|'] = binary_logical_operator::OR;
          binary_logical_ops['&'] = binary_logical_operator::AND;

          auto it = binary_logical_ops.find(body[1].get_terminal());
          if (it != binary_logical_ops.end()) {
            sdd->add_synthesized_attribute(
                {head, body}, generate_binary_assignment_rule(it->second));
            continue;
          }
        }

        if (body.size() == 2) {
          if (body[0].is_terminal()) {
            std::map<char, unary_arithmetic_operator> unary_arithmetic_ops;
            unary_arithmetic_ops['-'] = unary_arithmetic_operator::minus;
            unary_arithmetic_ops['+'] = unary_arithmetic_operator::plus;
            std::map<char, unary_logical_operator> unary_logical_ops;
            unary_logical_ops['!'] = unary_logical_operator::negation;

            auto it = unary_arithmetic_ops.find(body[0].get_terminal());
            if (it != unary_arithmetic_ops.end()) {
              sdd->add_synthesized_attribute(
                  {head, body}, generate_unary_assignment_rule(it->second));
              continue;
            }

            auto it2 = unary_logical_ops.find(body[0].get_terminal());
            if (it2 != unary_logical_ops.end()) {
              sdd->add_synthesized_attribute(
                  {head, body}, generate_unary_assignment_rule(it2->second));
              continue;
            }
          }
        }
      }
    }
  }

  bool three_address_code_SDD::run(token_span span,
                                   std::shared_ptr<symbol_table> table_) {
    table = table_;
    if (!table) {
      table = std::make_shared<symbol_table>();
    }
    instruction_sequence.clear();
    tmp_name_index = 1;
    return sdd->run(span, {}).has_value();
  }
} // namespace cyy::compiler
