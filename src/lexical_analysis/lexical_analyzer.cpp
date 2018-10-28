/*!
 * \file lexical_analyzer.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma warning(push)
#if __has_include(<CppCoreCheck\Warnings.h>)
#include <CppCoreCheck\Warnings.h>
#pragma warning(disable : ALL_CPPCORECHECK_WARNINGS)
#endif
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#pragma warning(pop)
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
  rest_input.clear();

  std::shared_ptr<ALPHABET> alphabet =
      ::cyy::computation::ALPHABET::get(alphabet_name);


  NFA nfa({0}, alphabet_name, 0, {}, {});

  uint64_t start_state = 1;
  for (auto const &p : patterns) {
    auto sub_nfa = regex(alphabet_name, p.second).to_NFA(start_state);
    REQUIRE(sub_nfa.get_final_states().size() == 1);
    auto final_state = *(sub_nfa.get_final_states().begin());
    nfa.add_sub_NFA(sub_nfa, true);
    pattern_final_states[final_state] = p.first;
    start_state = final_state + 1;
  }
  REQUIRE(nfa.get_final_states().size() == patterns.size());
  nfa_opt=std::move(nfa);
}

std::variant< lexical_analyzer::token,int> lexical_analyzer::scan() {
  make_NFA();
  symbol_string lexeme;
  size_t max_lexeme_size = 0;
  std::set<uint64_t> prev_final_set;

  auto cur_set= nfa_opt->get_start_epsilon_closure();

  auto line_no = cur_line;
  auto column_no = cur_column;

  while (true) {
    symbol_type c = 0;
    if(!rest_input.empty()) {
      c=rest_input.front();
      rest_input.erase(rest_input.begin());
    }
    else {
    input_stream.get(c);

    if (!input_stream) {
      break;
    }
    }

    cur_set = nfa_opt->move(cur_set, c);


    std::cout<<"c is"<<(char)c<<std::endl;
    lexeme.push_back(c);

    if (c == '\n') {
      cur_line++;
      cur_column = 1;
    } else {
      cur_column++;
    }

    if (nfa_opt->contain_final_state(cur_set)) {
      prev_final_set = cur_set;
      max_lexeme_size = lexeme.size();
    }
  }

  std::cout<<"max_lexeme_size="<<max_lexeme_size<<std::endl;
  if (max_lexeme_size>0) {
    if(max_lexeme_size<lexeme.size()) {
      rest_input.insert(rest_input.end(),lexeme.begin()+max_lexeme_size,lexeme.end());
    }
    lexeme.resize(max_lexeme_size);

    token t;
    for (auto const &[final_state, token_name] : pattern_final_states) {
      if (prev_final_set.count(final_state)) {
        t.name = token_name;
        t.attribute.line_no = line_no;
        t.attribute.column_no = column_no;
        t.attribute.lexeme = std::move(lexeme);
        return {t};
      }
    }
    assert(0);
  }

  if (!input_stream && !input_stream.eof()) {
    return {-2};
  }

  if (!lexeme.empty()) {
    return {1};
  }
  return {-1};
}

} // namespace cyy::compiler
