/*!
 * \file syntax_tree_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#include <doctest/doctest.h>

#include "intermediate_representation/syntax_tree.hpp"
#include "semantic_analysis/s_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"
#include <cyy/computation/context_free_lang/slr_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>

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
            std::make_shared<syntax_tree::binary_expression_node>(
                syntax_tree::binary_operator::addition, E_val, T_val));
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
            std::make_shared<syntax_tree::binary_expression_node>(
                syntax_tree::binary_operator::subtraction, E_val, T_val));
      }});

  production_vector.emplace_back("E", CFG_production::body_type{"T"});
  rules.emplace_back(
      SDD::semantic_rule{"$0.node",
                         {"$1.node"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

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
            std::make_shared<syntax_tree::binary_expression_node>(
                syntax_tree::binary_operator::multiplication, T_val, F_val));
      }});

  production_vector.emplace_back("T", CFG_production::body_type{"F"});
  rules.emplace_back(
      SDD::semantic_rule{"$0.node",
                         {"$1.node"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

  production_vector.emplace_back("F", CFG_production::body_type{'(', "E", ')'});

  rules.emplace_back(
      SDD::semantic_rule{"$0.node",
                         {"$2.node"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

  auto id_token = static_cast<CFG::terminal_type>(common_token::id);
  production_vector.emplace_back("F", CFG_production::body_type{id_token});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1"},
      [&table](const auto &arguments) -> std::optional<std::any> {
        symbol_table_entry entry;
        entry.lexeme = std::any_cast<token>(*arguments[0]).lexeme;
        table.add_entry(entry);
        return std::make_any<syntax_tree::expression_node_ptr>(
            std::make_shared<syntax_tree::symbol_node>(
                table.get_entry(entry.lexeme)));
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

  std::vector<token> tokens;
  tokens.emplace_back(id_token, "a");
  tokens.emplace_back('+', "+");
  tokens.emplace_back(id_token, "a");
  tokens.emplace_back('*', "*");
  tokens.emplace_back('(', "(");
  tokens.emplace_back(id_token, "b");
  tokens.emplace_back('-', "-");
  tokens.emplace_back(id_token, "c");
  tokens.emplace_back(')', ")");
  tokens.emplace_back('+', "+");
  tokens.emplace_back('(', "(");
  tokens.emplace_back(id_token, "b");
  tokens.emplace_back('-', "-");
  tokens.emplace_back(id_token, "c");
  tokens.emplace_back(')', ")");
  tokens.emplace_back('*', "*");
  tokens.emplace_back(id_token, "d");

  auto attriubtes = sdd.run(tokens, {"E.node"});
  REQUIRE(attriubtes);
  auto expression_node_ptr = std::any_cast<syntax_tree::expression_node_ptr>(
      attriubtes.value()["E.node"]);
  REQUIRE(expression_node_ptr);
  expression_node_ptr =
      expression_node_ptr->common_subexpression_elimination_by_DAG();
  auto root_node_ptr =
      std::dynamic_pointer_cast<syntax_tree::binary_expression_node>(
          expression_node_ptr);
  REQUIRE_EQ(root_node_ptr->op, syntax_tree::binary_operator::addition);
  auto left_child =
      std::dynamic_pointer_cast<syntax_tree::binary_expression_node>(
          root_node_ptr->left);
  auto right_child =
      std::dynamic_pointer_cast<syntax_tree::binary_expression_node>(
          root_node_ptr->right);
  REQUIRE_EQ(left_child->op, syntax_tree::binary_operator::addition);
  REQUIRE_EQ(right_child->op, syntax_tree::binary_operator::multiplication);
  REQUIRE_EQ(std::dynamic_pointer_cast<syntax_tree::binary_expression_node>(
                 left_child->right)
                 ->right.get(),
             right_child->left.get());
}
