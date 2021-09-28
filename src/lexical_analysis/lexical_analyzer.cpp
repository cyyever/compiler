/*!
 * \file lexical_analyzer.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "lexical_analyzer.hpp"

#include <cassert>

#include <cyy/computation/regular_lang/regex.hpp>

namespace cyy::compiler {

  void lexical_analyzer::make_NFA() {
    if (nfa_opt) {
      return;
    }

    if (patterns.empty()) {
      throw std::runtime_error("no pattern");
    }

    pattern_final_states.clear();

    NFA nfa({0}, alphabet, 0, {}, {});

    NFA::state_type sub_start_state = 1;
    for (auto const &[token_name, pattern] : patterns) {
      auto sub_nfa = regex(alphabet, pattern).to_NFA(sub_start_state);
      assert(sub_nfa.get_final_states().size() == 1);
      auto final_state = *(sub_nfa.get_final_states().begin());
      nfa.add_sub_NFA(std::move(sub_nfa));
      nfa.add_epsilon_transition(nfa.get_start_state(), {sub_start_state});
      pattern_final_states[final_state] = token_name;
      sub_start_state = final_state + 1;
    }
    assert(nfa.get_final_states().size() == patterns.size());
    nfa_opt = std::move(nfa);
  }

  std::optional<token> lexical_analyzer::scan() {
    make_NFA();

    auto cur_state_set = nfa_opt->get_start_set();
    decltype(cur_state_set) final_state_set;
    auto cur_view = last_view;
    auto cur_attribute = last_attribute;

    token cur_token;
    cur_token.attribute = cur_attribute;
    bool next_is_newline = false;
    while (!cur_view.empty()) {
      auto c = cur_view.front();
      cur_view.remove_prefix(1);
      cur_state_set = nfa_opt->go(
          cur_state_set, static_cast<cyy::computation::symbol_type>(c));
      if (next_is_newline) {
        cur_attribute.line_no++;
        cur_attribute.column_no = 1;
      } else {
        cur_attribute.column_no++;
      }
      next_is_newline = (c == '\n');

      if (cur_state_set.empty()) {
        break;
      }

      if (nfa_opt->contain_final_state(cur_state_set)) {
        final_state_set = nfa_opt->final_state_intersection(cur_state_set);
        if (!final_state_set.empty()) {
          last_attribute = cur_attribute;
          cur_token.lexeme.append(last_view.data(),
                                  last_view.size() - cur_view.size());
          last_view = cur_view;
        }
      }
    }

    if (!cur_token.lexeme.empty()) {
      if (final_state_set.size() == 1) {
        cur_token.name = pattern_final_states[*final_state_set.begin()];
        return cur_token;
      }
      // resolve conflicts
      for (auto s : final_state_set) {
        auto name = pattern_final_states[s];
        if (keywords.contains(name)) {
          cur_token.name = name;
          return cur_token;
        }
      }
      std::cerr << "can't resolve conflicts for patterns" << std::endl;
    }
    return {};
  }
  std::vector<token> lexical_analyzer::scan_all() {
    std::vector<token> tokens;
    while (true) {
      auto token_opt = scan();
      if (token_opt.has_value()) {
        tokens.emplace_back(std::move(*token_opt));
        continue;
      }
      break;
    }
    return tokens;
  }

} // namespace cyy::compiler
