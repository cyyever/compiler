/*!
 * \file s_attributed_sdd_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#if __has_include(<CppCoreCheck\Warnings.h>)
#include <CppCoreCheck\Warnings.h>
#pragma warning(disable : ALL_CPPCORECHECK_WARNINGS)
#endif
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include "../../src/semantic_analysis/s_attributed_sdd.hpp"
#include <cyy/computation/contex_free_lang/slr_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("run") {

  std::vector<CFG_production> production_vector;

  std::vector<SDD::semantic_rule> rules;

  production_vector.emplace_back("L", CFG_production::body_type{"E"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$1.val"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> { return arguments.at(0).get(); }});

  production_vector.emplace_back("E", CFG_production::body_type{"E", '+', "T"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$1.val", "$3.val"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> {
        auto E_val = std::any_cast<int>(arguments.at(0).get());
        auto T_val = std::any_cast<int>(arguments.at(1).get());
        return std::make_any<int>(E_val + T_val);
      }});

  production_vector.emplace_back("E", CFG_production::body_type{"T"});
  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$1.val"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> { return arguments.at(0).get(); }});
  production_vector.emplace_back("T", CFG_production::body_type{"T", '*', "F"});

  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$1.val", "$3.val"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> {
        auto T_val = std::any_cast<int>(arguments.at(0).get());
        auto F_val = std::any_cast<int>(arguments.at(1).get());
        return std::make_any<int>(T_val * F_val);
      }});

  production_vector.emplace_back("T", CFG_production::body_type{"F"});
  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$1.val"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> { return arguments.at(0).get(); }});

  production_vector.emplace_back("F", CFG_production::body_type{'(', "E", ')'});

  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$2.val"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> { return arguments.at(0).get(); }});

  auto digit_token = static_cast<CFG::terminal_type>(common_token::digit);
  production_vector.emplace_back("F", CFG_production::body_type{digit_token});

  rules.emplace_back(SDD::semantic_rule{
      "$0.val",
      {"$1"},
      [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
          -> std::optional<std::any> {
        return std::make_any<int>(
            static_cast<char>(std::any_cast<token>(arguments.at(0)).lexeme[0]) -
            '0');
      }});

  REQUIRE(production_vector.size() == rules.size());
  std::map<CFG::nonterminal_type, std::vector<CFG_production::body_type>>
      productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  SLR_grammar grammar("common_tokens", "L", productions);
  S_attributed_SDD sdd(grammar);

  for (size_t i = 0; i < production_vector.size(); i++) {
    sdd.add_synthesized_attribute(production_vector[i], std::move(rules[i]));
  }

  sdd.add_synthesized_attribute(
      production_vector[0],
      SDD::semantic_rule{
          "$0.val_inc",
          {"$0.val"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            auto L_val = std::any_cast<int>(arguments.at(0).get());
            return std::make_any<int>(L_val + 1);
          }});

  sdd.add_synthesized_attribute(
      production_vector[0],
      SDD::semantic_rule{
          {},
          {"$0.val_inc"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            auto L_val_inc = std::any_cast<int>(arguments.at(0).get());

            std::cout << "in semantic procedure val_inc is " << L_val_inc
                      << std::endl;
            return {};
          }});

  std::vector<token> tokens;
  tokens.push_back(token{'(', U"(", {}});
  tokens.push_back(token{digit_token, U"1", {}});
  tokens.push_back(token{'+', U"+", {}});
  tokens.push_back(token{digit_token, U"2", {}});
  tokens.push_back(token{')', U")", {}});
  tokens.push_back(token{'*', U"*", {}});
  tokens.push_back(token{digit_token, U"3", {}});

  auto attriubtes = sdd.run(tokens);
  REQUIRE(attriubtes);
  REQUIRE(std::any_cast<int>(attriubtes.value()["L.val"]) == 9);
  REQUIRE(std::any_cast<int>(attriubtes.value()["L.val_inc"]) == 10);
}
