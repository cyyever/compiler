/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <set>

#include <cyy/computation/lang/lang.hpp>

#include "../semantic_analysis/type_expression.hpp"
#include "../token/token.hpp"

namespace cyy::compiler {
  using namespace cyy::computation;

  class symbol_table final {
  public:
    struct entry {
      std::string lexeme;
      std::shared_ptr<type_expression::expression> type;
      size_t relative_address{};

      bool operator==(const entry &rhs) const { return lexeme == rhs.lexeme; }
    };

    struct entry_hash {
      std::size_t operator()(const entry &e) const noexcept {
        return ::std::hash<std::string>()(e.lexeme);
      }
    };

  public:
    symbol_table() = default;
    symbol_table(std::shared_ptr<symbol_table> prev_table_)
        : prev_table(prev_table_) {}
    ~symbol_table() = default;

    void add_entry(entry e) {
      auto [it, has_emplaced] = entries.emplace(std::move(e));
      if (!has_emplaced) {
        throw exception::existed_symbol_table_entry(it->lexeme);
      }
    }

  private:
    std::unordered_set<entry, entry_hash> entries;
    std::shared_ptr<symbol_table> prev_table;
  };
} // namespace cyy::compiler
