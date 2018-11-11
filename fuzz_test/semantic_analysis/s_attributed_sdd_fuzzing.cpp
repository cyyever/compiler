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

using namespace cyy::computation;
using namespace cyy::compiler;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  std::unique_ptr<S_attributed_SDD> sdd_ptr;
  if (!sdd_ptr) {
    std::vector<std::pair<CFG::nonterminal_type, CFG::production_body_type>>
        production_vector;

    std::vector<SDD::semantic_rule> rules;

    production_vector.emplace_back("L", CFG::production_body_type{"E"});

    rules.emplace_back(SDD::semantic_rule{
        "L.val",
        {"E.val"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> { return arguments.at(0).get(); }});

    production_vector.emplace_back("E",
                                   CFG::production_body_type{"E", '+', "T"});

    rules.emplace_back(SDD::semantic_rule{
        "E.val",
        {"E.val", "T.val"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> {
          auto E_val = std::any_cast<int>(arguments.at(0).get());
          auto T_val = std::any_cast<int>(arguments.at(1).get());
          return std::make_any<int>(E_val + T_val);
        }});

    production_vector.emplace_back("E", CFG::production_body_type{"T"});
    rules.emplace_back(SDD::semantic_rule{
        "E.val",
        {"T.val"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> { return arguments.at(0).get(); }});
    production_vector.emplace_back("T",
                                   CFG::production_body_type{"T", '*', "F"});

    rules.emplace_back(SDD::semantic_rule{
        "T.val",
        {"T.val", "F.val"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> {
          auto T_val = std::any_cast<int>(arguments.at(0).get());
          auto F_val = std::any_cast<int>(arguments.at(1).get());
          return std::make_any<int>(T_val * F_val);
        }});

    production_vector.emplace_back("T", CFG::production_body_type{"F"});
    rules.emplace_back(SDD::semantic_rule{
        "T.val",
        {"F.val"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> { return arguments.at(0).get(); }});

    production_vector.emplace_back("F",
                                   CFG::production_body_type{'(', "E", ')'});

    rules.emplace_back(SDD::semantic_rule{
        "F.val",
        {"E.val"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> { return arguments.at(0).get(); }});

    auto digit_token = static_cast<CFG::terminal_type>(common_token::digit);
    production_vector.emplace_back("F", CFG::production_body_type{digit_token});

    rules.emplace_back(SDD::semantic_rule{
        "F.val",
        {"$0"},
        [](const std::vector<std::reference_wrapper<const std::any>> &arguments)
            -> std::optional<std::any> {
          return std::make_any<int>(
              static_cast<char>(
                  std::any_cast<token>(arguments.at(0)).lexeme[0]) -
              '0');
        }});

    std::map<CFG::nonterminal_type, std::vector<CFG::production_body_type>>
        productions;
    for (auto const &[head, body] : production_vector) {
      productions[head].emplace_back(body);
    }

    SLR_grammar grammar("common_tokens", "L", productions);
    sdd_ptr.reset(new S_attributed_SDD(grammar));

    for (size_t i = 0; i < production_vector.size(); i++) {
      sdd_ptr->add_synthesized_attribute(production_vector[i],
                                         std::move(rules[i]));
    }
  }

  try {
    std::vector<token> tokens;
    for (size_t i = 0; i < Size; i++) {
      symbol_type symbol = Data[i] % 95 + 32;
      tokens.push_back(token{symbol, symbol_string{symbol}, {}});
    }

    sdd_ptr->run(tokens);
  } catch (const std::exception &) {
  }
  return 0; // Non-zero return values are reserved for future use.
}
