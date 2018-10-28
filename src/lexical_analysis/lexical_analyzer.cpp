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
  std::set<uint64_t> prev_final_set;

  auto cur_set = nfa_opt->get_start_epsilon_closure();

  token cur_token;

  symbol_string rest_input = std::move(last_token.lexeme);
  std::reverse(rest_input.begin(), rest_input.end());
  while (true) {
    symbol_type c = 0;
    if (!rest_input.empty()) {
      c = rest_input.back();
      rest_input.pop_back();
    } else {
      input_stream.get(c);

      if (!input_stream) {
        break;
      }
    }

    cur_set = nfa_opt->move(cur_set, c);

    std::cout << "c is" << (char)c << std::endl;
    last_token.lexeme.push_back(c);

    if (c == '\n') {
      last_token.attribute.line_no++;
      last_token.attribute.column_no = 1;
    } else {
      last_token.attribute.column_no++;
    }

    if (nfa_opt->contain_final_state(cur_set)) {
      prev_final_set = cur_set;
      cur_token.attribute = last_token.attribute;
      cur_token.lexeme += last_token.lexeme;
      last_token.lexeme.clear();
    }
  }

  if (!cur_token.lexeme.empty()) {
    for (auto const &[final_state, token_name] : pattern_final_states) {
      if (prev_final_set.count(final_state)) {
        cur_token.name = token_name;
        return {cur_token};
      }
    }
    assert(0);
  }

  if (input_stream.eof()) {
    return {1};
  }

  return {-1};
}

} // namespace cyy::compiler
