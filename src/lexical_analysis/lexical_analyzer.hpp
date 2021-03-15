/*!
 * \file lexical_analyzer.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <iostream>
#include <lang/alphabet.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <cyy/computation/regular_lang/nfa.hpp>

#include "../token/token.hpp"

namespace cyy::compiler {
  using namespace cyy::computation;

  class lexical_analyzer {
  public:
    explicit lexical_analyzer(const ALPHABET_ptr &alphabet_)
        : alphabet(alphabet_) {}

    void add_pattern(const symbol_type &token_name, symbol_string pattern) {
      patterns.emplace(token_name, std::move(pattern));
      nfa_opt.reset();
      reset_input();
    }

    bool set_source_code(std::istringstream &&is) {
      source_code = is.str();
      if (is.bad() || is.fail()) {
        std::cerr << "read symbol stream failed";
        return false;
      }
      last_view = source_code;
      reset_input();
      return true;
    }

    void reset_input() noexcept {
      last_view = source_code;
      last_attribute = {};
    }

    //! \brief scan the input stream,return first token
    std::optional<token> scan();

  private:
    void make_NFA();

  private:
    ALPHABET_ptr alphabet;
    std::unordered_map<symbol_type, symbol_string> patterns;
    token_attribute last_attribute;
    std::string source_code;
    std::string_view last_view;
    std::optional<NFA> nfa_opt;
    std::unordered_map<NFA::state_type, symbol_type> pattern_final_states;
  };

} // namespace cyy::compiler
