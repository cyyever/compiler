/*!
 * \file lexical_analyzer.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <cyy/computation/lang/lang.hpp>
#include <cyy/computation/regular_lang/nfa.hpp>

namespace cyy::compiler {
using namespace cyy::computation;

class lexical_analyzer {
public:
  struct token_attribute {
    size_t line_no{1};
    size_t column_no{1};
  };

  struct token {
    symbol_type name;
    symbol_string lexeme;
    token_attribute attribute;
  };

public:
  explicit lexical_analyzer(const std::string &alphabet_name_)
      : alphabet_name(alphabet_name_) {}

  void append_pattern(const symbol_type &token_name, symbol_string pattern) {
    patterns.emplace_back(token_name, std::move(pattern));
    dfa_opt.reset();
    reset_input();
  }

  bool set_source_code(symbol_istream &&is) {
    source_code = symbol_string(std::istreambuf_iterator<symbol_type>(is),
                                 std::istreambuf_iterator<symbol_type>{});
    if (is.bad() || is.fail()) {
      std::cerr << "read symbol stream failed";
      return false;
    }
    last_view = source_code;
    reset_input();
    return true;
  }

  void reset_input() {
    last_view = source_code;
    last_attribute = {};
  }

  //! \brief scan the input stream,return first token
  //! \return when successed,return token
  //	when no token in remain input,return 1
  std::variant<token, int> scan();

private:
  void make_NFA();

private:
  std::string alphabet_name;
  std::vector<std::pair<symbol_type, symbol_string>> patterns;
  token_attribute last_attribute;
  symbol_string source_code;
  symbol_string_view last_view;
  std::optional<DFA> dfa_opt;
  std::map<uint64_t, symbol_type> pattern_final_states;
};
} // namespace cyy::compiler
