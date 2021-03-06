/*!
 * \file syntax_tree_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#include <doctest/doctest.h>

#include "../../src/semantic_analysis/s_attributed_sdd.hpp"
#include "../../src/semantic_analysis/syntax_tree.hpp"
#include <cyy/computation/context_free_lang/slr_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("syntax tree") {

  std::vector<CFG_production> production_vector;

  std::vector<SDD::semantic_rule> rules;

  production_vector.emplace_back("E", CFG_production::body_type{"E", '+', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto E_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[0]));
        auto T_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[1]));
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::binary_expression_node>(
                syntax_tree::binary_operator::addtion, E_val, T_val));
      }});

  production_vector.emplace_back("E", CFG_production::body_type{"E", '-', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto E_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[0]));
        auto T_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[1]));
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
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

  production_vector.emplace_back("T", CFG_production::body_type{'(', "E", ')'});

  rules.emplace_back(
      SDD::semantic_rule{"$0.node",
                         {"$2.node"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

  auto number_token = static_cast<CFG::terminal_type>(common_token::number);
  production_vector.emplace_back("T", CFG_production::body_type{number_token});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$1"}, [](const auto &arguments) -> std::optional<std::any> {
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::symbol_node>(
                std::any_cast<token>(*arguments[0]).lexeme)
                ->common_subexpression_elimination_by_DAG());
      }});

  auto id_token = static_cast<CFG::terminal_type>(common_token::id);
  production_vector.emplace_back("T", CFG_production::body_type{id_token});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$1"}, [](const auto &arguments) -> std::optional<std::any> {
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::symbol_node>(
                std::any_cast<token>(*arguments[0]).lexeme)
                ->common_subexpression_elimination_by_DAG());
      }});

  REQUIRE(production_vector.size() == rules.size());
  CFG::production_set_type productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  SLR_grammar grammar("common_tokens", "E", productions);
  S_attributed_SDD sdd(grammar);

  for (size_t i = 0; i < production_vector.size(); i++) {
    sdd.add_synthesized_attribute(production_vector[i], std::move(rules[i]));
  }

  std::vector<token> tokens;
  tokens.push_back(token{id_token, "a", {}});
  tokens.push_back(token{'-', "-", {}});
  tokens.push_back(token{number_token, "4", {}});
  tokens.push_back(token{'+', "+", {}});
  tokens.push_back(token{id_token, "c", {}});

  auto attriubtes = sdd.run(tokens, {"E.node"});
  REQUIRE(attriubtes);
  REQUIRE(std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
      attriubtes.value()["E.node"]));
}

TEST_CASE("common_subexpression_elimination_by_DAG") {

  std::vector<CFG_production> production_vector;

  std::vector<SDD::semantic_rule> rules;

  production_vector.emplace_back("E", CFG_production::body_type{"E", '+', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto E_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[0]));
        auto T_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[1]));
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::binary_expression_node>(
                syntax_tree::binary_operator::addtion, E_val, T_val)
                ->common_subexpression_elimination_by_DAG());
      }});

  production_vector.emplace_back("E", CFG_production::body_type{"E", '-', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.node",
      {"$1.node", "$3.node"},
      [](const auto &arguments) -> std::optional<std::any> {
        auto E_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[0]));
        auto T_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[1]));
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::binary_expression_node>(
                syntax_tree::binary_operator::subtraction, E_val, T_val)
                ->common_subexpression_elimination_by_DAG());
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
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[0]));
        auto F_val =
            std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
                *(arguments[1]));
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
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

  auto number_token = static_cast<CFG::terminal_type>(common_token::number);
  production_vector.emplace_back("F", CFG_production::body_type{number_token});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$1"}, [](const auto &arguments) -> std::optional<std::any> {
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::symbol_node>(
                std::any_cast<token>(*arguments[0]).lexeme));
      }});

  auto id_token = static_cast<CFG::terminal_type>(common_token::id);
  production_vector.emplace_back("F", CFG_production::body_type{id_token});
  rules.emplace_back(SDD::semantic_rule{
      "$0.node", {"$1"}, [](const auto &arguments) -> std::optional<std::any> {
        return std::make_any<std::shared_ptr<syntax_tree::expression_node>>(
            std::make_shared<syntax_tree::symbol_node>(
                std::any_cast<token>(*arguments[0]).lexeme));
      }});

  REQUIRE(production_vector.size() == rules.size());
  CFG::production_set_type productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  SLR_grammar grammar("common_tokens", "E", productions);
  S_attributed_SDD sdd(grammar);

  for (size_t i = 0; i < production_vector.size(); i++) {
    sdd.add_synthesized_attribute(production_vector[i], std::move(rules[i]));
  }

  std::vector<token> tokens;
  tokens.push_back(token{id_token, "a", {}});
  tokens.push_back(token{'+', "+", {}});
  tokens.push_back(token{id_token, "a", {}});
  tokens.push_back(token{'*', "*", {}});
  tokens.push_back(token{'(', "(", {}});
  tokens.push_back(token{id_token, "b", {}});
  tokens.push_back(token{'-', "-", {}});
  tokens.push_back(token{id_token, "c", {}});
  tokens.push_back(token{')', ")", {}});
  tokens.push_back(token{'+', "+", {}});
  tokens.push_back(token{'(', "(", {}});
  tokens.push_back(token{id_token, "b", {}});
  tokens.push_back(token{'-', "-", {}});
  tokens.push_back(token{id_token, "c", {}});
  tokens.push_back(token{')', ")", {}});
  tokens.push_back(token{'*', "*", {}});
  tokens.push_back(token{id_token, "d", {}});

  auto attriubtes = sdd.run(tokens, {"E.node"});
  REQUIRE(attriubtes);
  REQUIRE(std::any_cast<std::shared_ptr<syntax_tree::expression_node>>(
      attriubtes.value()["E.node"]));
}
