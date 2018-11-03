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
  nfa_opt = std::move(nfa);
}

std::variant<lexical_analyzer::token, int> lexical_analyzer::scan() {
  make_NFA();

  auto cur_set = nfa_opt->get_start_epsilon_closure();
  auto cur_view = last_view;
  auto cur_attribute = last_attribute;

  token cur_token;
  cur_token.attribute = cur_attribute;
  while (!cur_view.empty()) {
    symbol_type c = cur_view.front();
    cur_view.remove_prefix(1);
    cur_set = nfa_opt->move(cur_set, c);

    if (c == '\n') {
      cur_attribute.line_no++;
      cur_attribute.column_no = 1;
    } else {
      cur_attribute.column_no++;
    }

    if (nfa_opt->contain_final_state(cur_set)) {
      last_attribute = cur_attribute;
      cur_token.lexeme.append(last_view.data(),
                              last_view.size() - cur_view.size());
      last_view = cur_view;
      for (auto const &[final_state, token_name] : pattern_final_states) {
        if (cur_set.count(final_state)) {
          cur_token.name = token_name;
          break;
        }
      }
    }
    if (cur_set.empty()) {
      break;
    }
  }

  if (!cur_token.lexeme.empty()) {
    return {cur_token};
  }
  return {1};
}

} // namespace cyy::compiler
