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

#include "../../src/semantic_analysis/l_attributed_sdd.hpp"
#include "../../src/semantic_analysis/type_expression.hpp"
#include <cyy/computation/lang/common_tokens.hpp>

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("type expression") {

  /*
  std::vector<CFG_production> production_vector;


  production_vector.emplace_back("D", CFG_production::body_type{"T",
  static_cast<CFG::terminal_type>(static_cast<CFG::terminal_type>(common_token::id)),';',"D"});
  production_vector.emplace_back("D",
  CFG_production::body_type{ALPHABET::get("common_tokens")->get_epsilon()});
  production_vector.emplace_back("T", CFG_production::body_type{"B","C"});
  production_vector.emplace_back("T",
  CFG_production::body_type{static_cast<CFG::terminal_type>(common_token::record),'{',"D",'}'});
  production_vector.emplace_back("B",
  CFG_production::body_type{static_cast<CFG::terminal_type>(common_token::INT)});
  production_vector.emplace_back("B",
  CFG_production::body_type{static_cast<CFG::terminal_type>(common_token::FLOAT)});
  production_vector.emplace_back("C",
  CFG_production::body_type{ALPHABET::get("common_tokens")->get_epsilon()});
  production_vector.emplace_back("C",
  CFG_production::body_type{'[',static_cast<CFG::terminal_type>(common_token::number),']',"C"});

  std::map<CFG::nonterminal_type, std::vector<CFG_production::body_type>>
      productions;
  for (auto const &production : production_vector) {
    productions[production.get_head()].emplace_back(production.get_body());
  }

  SUBCASE("types and widths") {

    LL_grammar grammar("common_tokens", "T", productions);
    L_attributed_SDD sdd(grammar);

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{
          "$0.type",
          {"$2.type"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(0).get();
          }});

  sdd.add_synthesized_attribute(
      production_vector[2],
      SDD::semantic_rule{
          "$0.width",
          {"$2.width"},
          [](const std::vector<std::reference_wrapper<const std::any>>
                 &arguments) -> std::optional<std::any> {
            return arguments.at(0).get();
          }});

    sdd.add_inherited_attribute(
        production_vector[2],
        SDD::semantic_rule{
          "$2.inh_type",
          {"$1.type"},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return arguments.at(0).get();
        }});

    sdd.add_inherited_attribute(
        production_vector[2],
        SDD::semantic_rule{
          "$2.inh_width",
          {"$1.width"},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return arguments.at(0).get();
        }});

    sdd.add_synthesized_attribute(
        production_vector[4],
        SDD::semantic_rule{
          "$0.type",
          {},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return std::make_any<std::shared_ptr<
  cyy::compiler::type_expression::expression>>(std::make_shared<
  cyy::compiler::type_expression::basic_type>(
  cyy::compiler::type_expression::basic_type::type_enum::INT));
        }});

    sdd.add_synthesized_attribute(
        production_vector[4],
        SDD::semantic_rule{
          "$0.width",
          {},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return std::make_any<size_t>(4);
        }});

    sdd.add_synthesized_attribute(
        production_vector[5],
        SDD::semantic_rule{
          "$0.type",
          {},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return std::make_any<std::shared_ptr<
  cyy::compiler::type_expression::expression>>(std::make_shared<
  cyy::compiler::type_expression::basic_type>(
  cyy::compiler::type_expression::basic_type::type_enum::FLOAT));
        }});

    sdd.add_synthesized_attribute(
        production_vector[5],
        SDD::semantic_rule{
          "$0.width",
          {},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return std::make_any<size_t>(8);
        }});


    sdd.add_synthesized_attribute(
        production_vector[6],
        SDD::semantic_rule{
          "$0.type",
          {"$0.inh_type"},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return arguments.at(0).get();
        }});
    sdd.add_synthesized_attribute(
        production_vector[6],
        SDD::semantic_rule{
          "$0.width",
          {"$0.inh_width"},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {
        return arguments.at(0).get();
        }});

    sdd.add_synthesized_attribute(
        production_vector[7],
        SDD::semantic_rule{
          "$0.type",
          {"$2","$4.type"},
        [](const std::vector<std::reference_wrapper<const std::any>>
            &arguments) -> std::optional<std::any> {


        size_t element_number=0;

        for (auto c:std::any_cast<token>(arguments.at(0).get()).lexeme[0]) {
        element_number=element_number*10+c-'0';
        }

        return std::make_any<
  cyy::compiler::type_expression::array_type>(element_number,
  std::any_cast<std::shared_ptr<cyy::compiler::type_expression::expression>>(arguments.at(1).get()));
        }});
  }

*/
}
