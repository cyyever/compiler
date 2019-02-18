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

#include <cyy/computation/contex_free_lang/ll_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>

#include "../../src/semantic_analysis/l_attributed_sdd.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("run") {

  std::vector<CFG_production> production_vector;

  production_vector.emplace_back("T", CFG_production::body_type{"F", "T'"});
  production_vector.emplace_back("T'",
                                 CFG_production::body_type{'*', "F", "T'"});
  production_vector.emplace_back(
      "T'",
      CFG_production::body_type{ALPHABET::get("common_tokens")->get_epsilon()});
  auto digit_token = static_cast<CFG::terminal_type>(common_token::digit);
  production_vector.emplace_back("F", CFG_production::body_type{digit_token});

  std::map<CFG::nonterminal_type, std::vector<CFG_production::body_type>>
      productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  LL_grammar grammar("common_tokens", "T", productions);
  L_attributed_SDD sdd(grammar);

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{
          "$2.inh",
          {"$1.val"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(0).get();
          }});

  sdd.add_synthesized_attribute(
      production_vector[0],
      SDD::semantic_rule{
          "$0.val",
          {"$2.syn"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(0).get();
          }});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{
          "$2.inh2",
          {"$1.val", "$2.syn"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(1).get();
          }});

  sdd.add_inherited_attribute(
      production_vector[1],
      SDD::semantic_rule{
          "$3.inh",
          {"$0.inh", "$2.val"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            auto T_inh = std::any_cast<int>(arguments.at(0).get());
            auto F_val = std::any_cast<int>(arguments.at(1).get());
            return std::make_any<int>(T_inh * F_val);
          }});

  sdd.add_synthesized_attribute(
      production_vector[1],
      SDD::semantic_rule{
          "$0.syn",
          {"$3.syn"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(0).get();
          }});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{
          "$0.syn",
          {"$0.inh"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(0).get();
          }});

  sdd.add_synthesized_attribute(
      production_vector[3],
      SDD::semantic_rule{
          "$0.val",
          {"$1"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return std::make_any<int>(
                static_cast<char>(
                    std::any_cast<token>(arguments.at(0)).lexeme[0]) -
                '0');
          }});

  std::vector<token> tokens;
  tokens.push_back(token{digit_token, U"3", {}});
  tokens.push_back(token{'*', U"*", {}});
  tokens.push_back(token{digit_token, U"5", {}});

  auto attriubtes = sdd.run(tokens);
  REQUIRE(std::any_cast<int>(attriubtes["T.val"]) == 15);
}
