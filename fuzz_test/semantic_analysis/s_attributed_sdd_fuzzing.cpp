/*!
 * \file s_attributed_sdd_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */

#include "../../src/semantic_analysis/s_attributed_sdd.hpp"
#include <cyy/computation/contex_free_lang/slr_grammar.hpp>
#include <cyy/computation/lang/common_tokens.hpp>
#include <mutex>

using namespace cyy::computation;
using namespace cyy::compiler;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  static std::unique_ptr<S_attributed_SDD> sdd_ptr;
  static std::mutex sdd_mu;

  {
    std::lock_guard guard(sdd_mu);
    if (!sdd_ptr) {
      std::vector<CFG_production> production_vector;

      std::vector<SDD::semantic_rule> rules;

      production_vector.emplace_back("L", CFG_production::body_type{"E"});

      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$1.val"},
          [](const auto &arguments) -> std::optional<std::any> {
            return *arguments[0];
          }});

      production_vector.emplace_back("E",
                                     CFG_production::body_type{"E", '+', "T"});

      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$1.val", "$3.val"},
          [](const auto &arguments) -> std::optional<std::any> {
            auto E_val = std::any_cast<int>(*arguments[0]);
            auto T_val = std::any_cast<int>(*arguments[1]);
            return std::make_any<int>(E_val + T_val);
          }});

      production_vector.emplace_back("E", CFG_production::body_type{"T"});
      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$1.val"},
          [](const auto &arguments) -> std::optional<std::any> {
            return *arguments[0];
          }});
      production_vector.emplace_back("T",
                                     CFG_production::body_type{"T", '*', "F"});

      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$1.val", "$3.val"},
          [](const auto &arguments) -> std::optional<std::any> {
            auto T_val = std::any_cast<int>(*arguments[0]);
            auto F_val = std::any_cast<int>(*arguments[1]);
            return std::make_any<int>(T_val * F_val);
          }});

      production_vector.emplace_back("T", CFG_production::body_type{"F"});
      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$1.val"},
          [](const auto &arguments) -> std::optional<std::any> {
            return *arguments[0];
          }});

      production_vector.emplace_back("F",
                                     CFG_production::body_type{'(', "E", ')'});

      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$2.val"},
          [](const auto &arguments) -> std::optional<std::any> {
            return *arguments[0];
          }});

      auto digit_token = static_cast<CFG::terminal_type>(common_token::digit);
      production_vector.emplace_back("F",
                                     CFG_production::body_type{digit_token});

      rules.emplace_back(SDD::semantic_rule{
          "$0.val",
          {"$1"},
          [](const auto &arguments) -> std::optional<std::any> {
            return std::make_any<int>(
                static_cast<char>(
                    std::any_cast<token>(*arguments[0]).lexeme[0]) -
                '0');
          }});

      std::map<CFG::nonterminal_type, std::vector<CFG_production::body_type>>
          productions;
      for (auto const &p : production_vector) {
        productions[p.get_head()].emplace_back(p.get_body());
      }

      static SLR_grammar grammar("common_tokens", "L", productions);
      sdd_ptr.reset(new S_attributed_SDD(grammar));

      for (size_t i = 0; i < production_vector.size(); i++) {
        sdd_ptr->add_synthesized_attribute(production_vector[i],
                                           std::move(rules[i]));
      }
    }
  }

  try {
    std::vector<token> tokens;
    for (size_t i = 0; i < Size; i++) {
      symbol_type symbol = Data[i] % 95 + 32;
      tokens.push_back(
          token{symbol, std::string{static_cast<char>(symbol)}, {}});
    }

    sdd_ptr->run(tokens, {"L.val"});
  } catch (const std::exception &) {
  }
  return 0; // Non-zero return values are reserved for future use.
}
