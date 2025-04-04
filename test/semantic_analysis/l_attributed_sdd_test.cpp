/*!
 * \file s_attributed_sdd_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#include <doctest/doctest.h>

#include <cyy/algorithm/alphabet/common_tokens.hpp>
#include <cyy/computation/context_free_lang/ll_grammar.hpp>
#include "../../src/semantic_analysis/l_attributed_sdd.hpp"

using namespace cyy::compiler;

TEST_CASE("run") {

  std::vector<CFG_production> production_vector;

  production_vector.emplace_back("T", CFG_production::body_type{"F", "T'"});
  production_vector.emplace_back("T'",
                                 CFG_production::body_type{'*', "F", "T'"});
  production_vector.emplace_back("T'", CFG_production::body_type{});
  auto digit_token =
      static_cast<CFG::terminal_type>(cyy::algorithm::common_token::digit);
  production_vector.emplace_back("F", CFG_production::body_type{digit_token});

  CFG::production_set_type productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace(production.get_body());
  }

  LL_grammar grammar("common_tokens", "T", productions);
  L_attributed_SDD sdd(grammar);

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{.result_attribute = "$2.inh",
                         .arguments = {"$1.val"},
                         .action = SDD::semantic_rule::copy_action});

  sdd.add_synthesized_attribute(
      production_vector[0],
      SDD::semantic_rule{.result_attribute = "$0.val",
                         .arguments = {"$2.syn"},
                         .action = SDD::semantic_rule::copy_action});

  sdd.add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{
          .result_attribute = "$2.inh2",
          .arguments = {"$2.syn"},
          .action = [](const auto &arguments) -> std::optional<std::any> {
            REQUIRE(std::any_cast<int>(*(arguments[0])) == 15);
            return *(arguments[0]);
          }});

  sdd.add_inherited_attribute(
      production_vector[1],
      SDD::semantic_rule{
          .result_attribute = "$3.inh",
          .arguments = {"$0.inh", "$2.val"},
          .action = [](const auto &arguments) -> std::optional<std::any> {
            auto T_inh = std::any_cast<int>(*(arguments[0]));
            auto F_val = std::any_cast<int>(*(arguments[1]));
            return std::make_any<int>(T_inh * F_val);
          }});

  sdd.add_synthesized_attribute(
      production_vector[1],
      SDD::semantic_rule{.result_attribute = "$0.syn",
                         .arguments = {"$3.syn"},
                         .action = SDD::semantic_rule::copy_action});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{.result_attribute = "$0.syn",
                         .arguments = {"$0.inh"},
                         .action = SDD::semantic_rule::copy_action});

  sdd.add_synthesized_attribute(
      production_vector[3],
      SDD::semantic_rule{
          .result_attribute = "$0.val",
          .arguments = {"$1"},
          .action = [](const auto &arguments) -> std::optional<std::any> {
            return std::make_any<int>(
                static_cast<char>(
                    std::any_cast<token>(*arguments[0]).lexeme[0]) -
                '0');
          }});

  std::vector<token> tokens;
  tokens.emplace_back(digit_token, "3");
  tokens.emplace_back('*', "*");
  tokens.emplace_back(digit_token, "5");

  auto attriubtes = sdd.run(tokens, {"T.val"});
  REQUIRE(attriubtes);
  REQUIRE(std::any_cast<int>(attriubtes.value()["T.val"]) == 15);
}
