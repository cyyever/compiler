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

  cyy::computation::NFA lexical_analyzer::make_NFA() {
    /* if (nfa_opt) { */
    /*   return *nfa; */
    /* } */

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
    return nfa;
  }
  bool lexical_analyzer::make_DFA() {
    auto nfa = make_NFA();
    auto [dfa, state_mapping] = nfa.to_DFA_with_mapping();
    auto old_pattern_final_states = std::move(pattern_final_states);
    for (auto final_state : dfa.get_final_states()) {
      auto const &original_final_states = state_mapping.right.at(final_state);
      assert(!original_final_states.empty());
      size_t kw_cnt = 0;
      if (original_final_states.size() > 1) {
        for (auto original_final_state : original_final_states) {
          auto pattern = old_pattern_final_states[original_final_state];
          if (keywords.contains(pattern)) {
            kw_cnt++;
            pattern_final_states[final_state] = pattern;
          }
        }
        if (kw_cnt != 1) {
          std::cerr << "can't resolve conflicts for patterns" << std::endl;
          return false;
        }
      } else {
        pattern_final_states[final_state] =
            old_pattern_final_states[*original_final_states.begin()];
      }
    }
    dfa_opt = std::move(dfa);
    return true;
  }

  std::optional<token> lexical_analyzer::scan() {
    if (!make_DFA()) {
      return {};
    }

    auto cur_state = dfa_opt->get_start_state();
    decltype(cur_state) final_state;
    auto cur_view = last_view;
    auto cur_attribute = last_attribute;
    token cur_token;
    cur_token.attribute = cur_attribute;
    bool next_is_newline = false;
    while (!cur_view.empty()) {
      auto c = cur_view.front();
      auto cur_state_opt =
          dfa_opt->go(cur_state, static_cast<cyy::computation::symbol_type>(c));
      if (!cur_state_opt.has_value()) {
        break;
      }
      cur_view.remove_prefix(1);
      if (next_is_newline) {
        cur_attribute.line_no++;
        cur_attribute.column_no = 1;
      } else {
        cur_attribute.column_no++;
      }
      next_is_newline = (c == '\n');
      /* if(cur_state_opt.boost::bimaps::relation::hash_value()) */

      /* if (cur_state.empty()) { */
      /*   break; */
      /* } */

      if (dfa_opt->is_final_state(cur_state)) {
        final_state = cur_state;
        /* final_state = dfa_opt->final_state_intersection(cur_state); */
        /* if (!final_state.empty()) { */
        cur_token.lexeme.append(last_view.data(),
                                last_view.size() - cur_view.size());
        last_attribute = cur_attribute;
        last_view = cur_view;
        /* } */
      }
    }

    if (!cur_token.lexeme.empty()) {
      // resolve conflicts
      cur_token.name = pattern_final_states[final_state];
      if (ignored_patterns.contains(cur_token.name)) {
        return scan();
      }
      return cur_token;
      /* } */
      /* break; */
      /* std::cerr << "can't resolve conflicts for lexeme:" << cur_token.lexeme
       */
      /*   << std::endl; */
    }
    return {};
  }
  std::pair<std::vector<token>, bool> lexical_analyzer::scan_all() {
    std::vector<token> tokens;
    while (true) {
      auto token_opt = scan();
      if (token_opt.has_value()) {
        tokens.emplace_back(std::move(*token_opt));
        continue;
      }
      break;
    }
    return {tokens, true};
  }

} // namespace cyy::compiler
