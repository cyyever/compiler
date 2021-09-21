/*!
 * \file s_attributed_sdd_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#include <cyy/computation/lang/common_tokens.hpp>

#include "semantic_analysis/l_attributed_sdd.hpp"
#include "three_address_code_sdd.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

namespace cyy::compiler {
three_address_code_SDD::three_address_code_SDD() {

  std::vector<CFG_production> production_vector;

  production_vector.emplace_back(
      "S", CFG_production::body_type{
               static_cast<CFG::terminal_type>(common_token::id), '=', "E"});
  production_vector.emplace_back("E", CFG_production::body_type{"E", '+', "E"});
  production_vector.emplace_back("E", CFG_production::body_type{'-', "E"});
  production_vector.emplace_back("E", CFG_production::body_type{'(', "E", ')'});
  production_vector.emplace_back(
      "E", CFG_production::body_type{
               static_cast<CFG::terminal_type>(common_token::id)});

  CFG::production_set_type productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace(production.get_body());
  }

  LL_grammar grammar("common_tokens", "S", productions);
  sdd = std::make_unique<L_attributed_SDD>(grammar);


  sdd->add_inherited_attribute(
      production_vector[0],
      SDD::semantic_rule{"$0.code",
                         {"$1", "$3.code", "$3.addr"},
                         [this](const auto &arguments) -> std::optional<std::any> {
                         auto name=IR::three_address_code::name(table->create_and_get_symbol( std::any_cast<token>(*arguments[0]).lexeme));
                //IR::three_address_code::ass
                         /* code.emplace_back(); */
                 //        return name.lexeme+"="+
                           return *(arguments[0]);
                         }});
}
}
#if 0
  sdd.add_inherited_attribute(
      production_vector[1],
      SDD::semantic_rule{"$3.inh",
                         {"$0.inh", "$2.val"},
                         [](const auto &arguments) -> std::optional<std::any> {
                           auto T_inh = std::any_cast<int>(*(arguments[0]));
                           auto F_val = std::any_cast<int>(*(arguments[1]));
                           return std::make_any<int>(T_inh * F_val);
                         }});

  sdd.add_synthesized_attribute(
      production_vector[1],
      SDD::semantic_rule{
          "$0.syn", {"$3.syn"}, SDD::semantic_rule::copy_action});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{
          "$0.syn", {"$0.inh"}, SDD::semantic_rule::copy_action});

  sdd.add_synthesized_attribute(
      production_vector[3],
      SDD::semantic_rule{
          "$0.val",
          {"$1"},
          [](const auto &arguments) -> std::optional<std::any> {
            return std::make_any<int>(
                static_cast<char>(
                    std::any_cast<token>(*arguments[0]).lexeme[0]) -
                '0');
          }});



    }
}
TEST_CASE("run") {


  std::vector<token> tokens;
  tokens.emplace_back(digit_token, "3");
  tokens.emplace_back('*', "*");
  tokens.emplace_back(digit_token, "5");

  auto attriubtes = sdd.run(tokens, {"T.val"});
  REQUIRE(attriubtes);
  REQUIRE(std::any_cast<int>(attriubtes.value()["T.val"]) == 15);
}
#endif
