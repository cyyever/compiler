/*!
 * \file lexical_analyzer.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include <cassert>
#include <cyy/computation/regular_lang/regex.hpp>

#include "lexical_analyzer.hpp"

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
        auto final_states = nfa_opt->final_state_intersection(cur_state_set);
        if (final_states.size() == 1) {
          last_attribute = cur_attribute;
          cur_token.lexeme.append(last_view.data(),
                                  last_view.size() - cur_view.size());
          last_view = cur_view;
          cur_token.name = pattern_final_states[*final_states.begin()];
        }
      }
    }

    if (!cur_token.lexeme.empty()) {
      return cur_token;
    }
    return {};
  }

} // namespace cyy::compiler
