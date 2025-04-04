/*!
 * \file lexical_analyzer.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once


#include <boost/bimap.hpp>
#include <cyy/algorithm/alphabet/alphabet.hpp>
#include <cyy/computation/regular_lang/nfa.hpp>

#include "token.hpp"

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
    void add_keyword(const symbol_type &token_name, symbol_string pattern) {
      add_pattern(token_name, std::move(pattern));
      keywords.insert(token_name);
    }
    void add_ignored_pattern(const symbol_type &token_name,
                             symbol_string pattern) {
      add_pattern(token_name, std::move(pattern));
      ignored_patterns.insert(token_name);
    }

    void set_source_code(std::string code) {
      source_code = std::move(code);
      last_view = source_code;
      reset_input();
      return;
    }

    bool set_source_code(const std::istringstream &is) {
      if (is.bad() || is.fail()) {
        std::cerr << "read symbol stream failed";
        return false;
      }
      set_source_code(is.str());
      return true;
    }

    void reset_input() noexcept {
      last_view = source_code;
      last_attribute = {};
    }

    //! \brief scan the input stream, return the first token
    std::optional<token> scan();
    std::pair<std::vector<token>, bool> scan_all();

  private:
    void make_NFA();

  private:
    ALPHABET_ptr alphabet;
    std::unordered_map<symbol_type, symbol_string> patterns;
    std::unordered_set<symbol_type> keywords;
    std::unordered_set<symbol_type> ignored_patterns;
    token_attribute last_attribute;
    std::string source_code;
    std::string_view last_view;
    std::optional<NFA> nfa_opt;
    std::unordered_map<NFA::state_type, symbol_type> pattern_final_states;
  };

} // namespace cyy::compiler
