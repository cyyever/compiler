/*!
 * \file s_attributed_sdd_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#include <cyy/computation/context_free_lang/slr_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>
#include <doctest/doctest.h>

#include "../../src/semantic_analysis/s_attributed_sdd.hpp"
using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("run") {

  std::vector<CFG_production> production_vector;

  std::vector<SDD::semantic_rule> rules;

  production_vector.emplace_back("L", CFG_production::body_type{"E"});

  rules.emplace_back(
      SDD::semantic_rule{"$0.val",
                         {"$1.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

  production_vector.emplace_back("E", CFG_production::body_type{"E", '+', "T"});

  rules.emplace_back(
      SDD::semantic_rule{"$0.val",
                         {"$1.val", "$3.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           auto E_val = std::any_cast<int>(*(arguments[0]));
                           auto T_val = std::any_cast<int>(*(arguments[1]));
                           return std::make_any<int>(E_val + T_val);
                         }});

  production_vector.emplace_back("E", CFG_production::body_type{"T"});
  rules.emplace_back(
      SDD::semantic_rule{"$0.val",
                         {"$1.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});
  production_vector.emplace_back("T", CFG_production::body_type{"T", '*', "F"});

  rules.emplace_back(
      SDD::semantic_rule{"$0.val",
                         {"$1.val", "$3.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           auto T_val = std::any_cast<int>(*(arguments[0]));
                           auto F_val = std::any_cast<int>(*(arguments[1]));
                           return std::make_any<int>(T_val * F_val);
                         }});

  production_vector.emplace_back("T", CFG_production::body_type{"F"});
  rules.emplace_back(
      SDD::semantic_rule{"$0.val",
                         {"$1.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

  production_vector.emplace_back("F", CFG_production::body_type{'(', "E", ')'});

  rules.emplace_back(
      SDD::semantic_rule{"$0.val",
                         {"$2.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           return *(arguments[0]);
                         }});

  auto digit_token = static_cast<CFG::terminal_type>(common_token::digit);
  production_vector.emplace_back("F", CFG_production::body_type{digit_token});

  rules.emplace_back(SDD::semantic_rule{
      "$0.val", {"$1"}, [](const auto &arguments) -> std::optional<std::any> {
        return std::make_any<int>(
            static_cast<char>(std::any_cast<token>(*(arguments[0])).lexeme[0]) -
            '0');
      }});

  REQUIRE(production_vector.size() == rules.size());
  CFG::production_set_type productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace(production.get_body());
  }

  SLR_grammar grammar("common_tokens", "L", productions);
  S_attributed_SDD sdd(grammar);

  for (size_t i = 0; i < production_vector.size(); i++) {
    sdd.add_synthesized_attribute(production_vector[i], std::move(rules[i]));
  }

  sdd.add_synthesized_attribute(
      production_vector[0],
      SDD::semantic_rule{"$0.val_inc",
                         {"$0.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           auto L_val = std::any_cast<int>(*(arguments[0]));
                           return std::make_any<int>(L_val + 1);
                         }});

  sdd.add_synthesized_attribute(
      production_vector[0],
      SDD::semantic_rule{{},
                         {"$0.val_inc"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           auto L_val_inc = std::any_cast<int>(*(arguments[0]));

                           std::cout << "in semantic procedure val_inc is "
                                     << L_val_inc << std::endl;
                           return {};
                         }});

  std::vector<token> tokens;
  tokens.emplace_back('(', "(");
  tokens.emplace_back(digit_token, "1");
  tokens.emplace_back('+', "+");
  tokens.emplace_back(digit_token, "2");
  tokens.emplace_back(')', ")");
  tokens.emplace_back('*', "*");
  tokens.emplace_back(digit_token, "3");

  auto attriubtes = sdd.run(tokens, {"L.val", "L.val_inc"});
  REQUIRE(attriubtes);
  REQUIRE(std::any_cast<int>(attriubtes.value()["L.val"]) == 9);
  REQUIRE(std::any_cast<int>(attriubtes.value()["L.val_inc"]) == 10);
}
