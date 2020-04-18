/*!
 * \file lexical_analyzer.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include <cyy/computation/lang/symbol.hpp>
#include <cyy/computation/regular_lang/dfa.hpp>

#include "../token/token.hpp"

namespace cyy::compiler {
  using namespace cyy::computation;

  class lexical_analyzer {
  public:
    explicit lexical_analyzer(const std::string &alphabet_name_)
        : alphabet_name(alphabet_name_) {}

    void append_pattern(const symbol_type &token_name, symbol_string pattern) {
      patterns.emplace_back(token_name, std::move(pattern));
      dfa_opt.reset();
      reset_input();
    }

    bool set_source_code(std::istringstream &&is) {
      source_code =
          is.str(); // symbol_string(std::istreambuf_iterator<symbol_type>(is),
                    //            std::istreambuf_iterator<symbol_type>{});
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
    std::string alphabet_name;
    std::vector<std::pair<symbol_type, symbol_string>> patterns;
    token_attribute last_attribute;
    // symbol_
    std::string source_code;
    // symbol_
    std::string_view last_view;
    std::optional<DFA> dfa_opt;
    std::map<uint64_t, symbol_type> pattern_final_states;
  };

} // namespace cyy::compiler
