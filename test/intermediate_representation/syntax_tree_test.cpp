/*!
 * \file syntax_tree_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#include <cyy/algorithm/alphabet/common_tokens.hpp>
#include <cyy/computation/context_free_lang/slr_grammar.hpp>
#include <doctest/doctest.h>

#include "example_grammar/lexical_analyzer.hpp"
#include "intermediate_representation/syntax_tree.hpp"
#include "operator.hpp"
#include "semantic_analysis/s_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("common_subexpression_elimination_by_DAG") {

  std::vector<CFG_production> production_vector;

  std::vector<SDD::semantic_rule> rules;
  symbol_table table;

  production_vector.emplace_back("E", CFG_production::body_type{"E", '+', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto E_val =
            std::any_cast<syntax_tree::expression_node_ptr>(*(arguments[0]));
        auto T_val =
            std::any_cast<syntax_tree::expression_node_ptr>(*(arguments[1]));
        return std::make_any<syntax_tree::expression_node_ptr>(
            std::make_shared<syntax_tree::binary_arithmetic_node>(
                binary_arithmetic_operator::plus, E_val, T_val));
      }});

  production_vector.emplace_back("E", CFG_production::body_type{"E", '-', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto E_val =
            std::any_cast<syntax_tree::expression_node_ptr>(*(arguments[0]));
        auto T_val =
            std::any_cast<syntax_tree::expression_node_ptr>(*(arguments[1]));
        return std::make_any<syntax_tree::expression_node_ptr>(
            std::make_shared<syntax_tree::binary_arithmetic_node>(
                binary_arithmetic_operator::minus, E_val, T_val));
      }});

  production_vector.emplace_back("E", CFG_production::body_type{"T"});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$1.node"}, SDD::semantic_rule::copy_action});

  production_vector.emplace_back("T", CFG_production::body_type{"T", '*', "F"});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto T_val =
            std::any_cast<syntax_tree::expression_node_ptr>(*(arguments[0]));
        auto F_val =
            std::any_cast<syntax_tree::expression_node_ptr>(*(arguments[1]));
        return std::make_any<syntax_tree::expression_node_ptr>(
            std::make_shared<syntax_tree::binary_arithmetic_node>(
                binary_arithmetic_operator::multiplication, T_val, F_val));
      }});

  production_vector.emplace_back("T", CFG_production::body_type{"F"});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$1.node"}, SDD::semantic_rule::copy_action});

  production_vector.emplace_back("F", CFG_production::body_type{'(', "E", ')'});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$2.node"}, SDD::semantic_rule::copy_action});

  auto id_token = static_cast<CFG::terminal_type>(common_token::id);
  production_vector.emplace_back("F", CFG_production::body_type{id_token});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1"},
      [&table](const auto &arguments) -> std::optional<std::any> {
        symbol_table::symbol_entry entry;
        entry.lexeme = std::any_cast<token>(*arguments[0]).lexeme;
        table.add_symbol(entry);
        return std::make_any<syntax_tree::expression_node_ptr>(
            std::make_shared<syntax_tree::symbol_node>(
                table.get_symbol(entry.lexeme)));
      }});

  REQUIRE(production_vector.size() == rules.size());
  CFG::production_set_type productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace(production.get_body());
  }

  SLR_grammar grammar("common_tokens", "E", productions);
  S_attributed_SDD sdd(grammar);

  for (size_t i = 0; i < production_vector.size(); i++) {
    sdd.add_synthesized_attribute(production_vector[i], std::move(rules[i]));
  }

  auto analyzer = example_grammar::get_lexical_analyzer();
  analyzer->set_source_code("a+a*(b-c)+(b-c)*d");

  auto [tokens, res] = analyzer->scan_all();
  REQUIRE(res);

  auto attriubtes = sdd.run(tokens, {"E.node"});
  REQUIRE(attriubtes);
  auto expression_node_ptr = std::any_cast<syntax_tree::expression_node_ptr>(
      attriubtes.value()["E.node"]);
  REQUIRE(expression_node_ptr);
  expression_node_ptr =
      expression_node_ptr->common_subexpression_elimination_by_DAG();
  auto root_node_ptr =
      std::dynamic_pointer_cast<syntax_tree::binary_arithmetic_node>(
          expression_node_ptr);
  REQUIRE_EQ(root_node_ptr->op, binary_arithmetic_operator::plus);
  auto left_child =
      std::dynamic_pointer_cast<syntax_tree::binary_arithmetic_node>(
          root_node_ptr->left);
  auto right_child =
      std::dynamic_pointer_cast<syntax_tree::binary_arithmetic_node>(
          root_node_ptr->right);
  REQUIRE_EQ(left_child->op, binary_arithmetic_operator::plus);
  REQUIRE_EQ(right_child->op, binary_arithmetic_operator::multiplication);
  REQUIRE_EQ(std::dynamic_pointer_cast<syntax_tree::binary_arithmetic_node>(
                 left_child->right)
                 ->right.get(),
             right_child->left.get());
}
