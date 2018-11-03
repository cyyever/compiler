/*!
 * \file lexical_analyzer.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include <cassert>
#include <cyy/computation/regular_lang/regex.hpp>
#include <cyy/computation/regular_lang/dfa.hpp>

#include "lexical_analyzer.hpp"

namespace cyy::compiler {

void lexical_analyzer::make_NFA() {
  if (dfa_opt) {
    return;
  }

  if (patterns.empty()) {
    throw std::runtime_error("no pattern");
  }

  pattern_final_states.clear();

  std::shared_ptr<ALPHABET> alphabet =
      ::cyy::computation::ALPHABET::get(alphabet_name);

  NFA nfa({0}, alphabet_name, 0, {}, {});

  uint64_t start_state = 1;
  for (auto const &p : patterns) {
    auto sub_nfa = regex(alphabet_name, p.second).to_NFA(start_state);
    assert(sub_nfa.get_final_states().size() == 1);
    auto final_state = *(sub_nfa.get_final_states().begin());
    nfa.add_sub_NFA(sub_nfa, true);
    pattern_final_states[final_state] = p.first;
    start_state = final_state + 1;
  }
  assert(nfa.get_final_states().size() == patterns.size());

  auto [dfa,state_mapping]=nfa.to_DFA_with_mapping();
 
  decltype(pattern_final_states) pattern_DFA_final_states;

  for(auto const &[dfa_state,nfa_states]:state_mapping) {
    if(!dfa.is_final_state(dfa_state)) {
      continue;
    }

    for(auto nfa_state:nfa_states) {
      //use first pattern
      if(pattern_final_states.count(nfa_state)) {
	pattern_DFA_final_states[dfa_state]=pattern_final_states[nfa_state];
	break;
      }
    }
  }
  pattern_final_states=std::move(pattern_DFA_final_states);

  dfa_opt = std::move(dfa);
}

std::variant<token, int> lexical_analyzer::scan() {
  make_NFA();

  auto cur_state = dfa_opt->get_start_state();
  auto cur_view = last_view;
  auto cur_attribute = last_attribute;

  token cur_token;
  cur_token.attribute = cur_attribute;
  while (!cur_view.empty()) {
    symbol_type c = cur_view.front();
    cur_view.remove_prefix(1);
    auto cur_state_opt = dfa_opt->move(cur_state, c);

    if (c == '\n') {
      cur_attribute.line_no++;
      cur_attribute.column_no = 1;
    } else {
      cur_attribute.column_no++;
    }

    if(!cur_state_opt) {
      break;
    }
    cur_state=cur_state_opt.value();

    if (dfa_opt->is_final_state(cur_state)) {
      last_attribute = cur_attribute;
      cur_token.lexeme.append(last_view.data(),
                              last_view.size() - cur_view.size());
      last_view = cur_view;
      cur_token.name = pattern_final_states[cur_state];
    }
  }

  if (!cur_token.lexeme.empty()) {
    return {cur_token};
  }
  return {1};
}

} // namespace cyy::compiler
