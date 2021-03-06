/*!
 * \file symbol_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <functional>
#include <optional>
#include <set>
#include <unordered_set>

#include <cyy/computation/lang/symbol.hpp>

#include "../semantic_analysis/type_expression.hpp"
#include "../token/token.hpp"

namespace cyy::compiler {

  class symbol_table;
  struct symbol_table_entry {
    std::string lexeme;
    std::shared_ptr<type_expression::expression> type;
    size_t relative_address{};
    size_t width{};
    std::shared_ptr<symbol_table> associated_symbol_table;

    bool operator==(const symbol_table_entry &rhs) const {
      return lexeme == rhs.lexeme;
    }
  };
} // namespace cyy::compiler

namespace std {
  template <> struct hash<cyy::compiler::symbol_table_entry> {
    size_t
    operator()(const cyy::compiler::symbol_table_entry &e) const noexcept {
      return ::std::hash<std::string>()(e.lexeme);
    }
  };

  template <> struct less<cyy::compiler::symbol_table_entry> {
    bool
    operator()(const cyy::compiler::symbol_table_entry &lhs,
               const cyy::compiler::symbol_table_entry &rhs) const noexcept {
      return lhs.lexeme < rhs.lexeme;
    }
  };

} // namespace std
namespace cyy::compiler {
  using namespace cyy::computation;
  class symbol_table {
  public:
    symbol_table() = default;
    virtual ~symbol_table() = default;

    void set_prev_table(std::shared_ptr<symbol_table> prev_table_) {
      prev_table = prev_table_;
    }
    bool has_entry(const std::string &lexeme) const;
    bool add_entry(const symbol_table_entry &e);
    std::optional<symbol_table_entry>
    get_entry(const std::string &lexeme) const;

    bool add_type(std::shared_ptr<type_expression::type_name> expr,
                  std::shared_ptr<symbol_table> associated_symbol_table);

    std::optional<std::pair<std::shared_ptr<type_expression::type_name>,
                            std::shared_ptr<symbol_table>>>
    get_type(const std::string &type_name) const;

    void foreach_entry(
        const std::function<void(const symbol_table_entry &)> &callback) const {
      for (auto const &e : entries) {
        callback(e);
      }
    }

    size_t get_total_width() const {
      size_t total_width = 0;
      for (auto const &e : entries) {
        total_width += e.width;
      }
      return total_width;
    }

    void add_relative_address_offset(size_t offset) {
      /*
      decltype(entries) new_entries;
      while (!entries.empty()) {
        auto node = entries.extract(entries.begin());
      }
      */
      for (auto &e : entries) {
        const_cast<symbol_table_entry &>(e).relative_address += offset;
      }
    }

  private:
    std::unordered_set<symbol_table_entry> entries;
    std::unordered_map<std::string,
                       std::pair<std::shared_ptr<type_expression::type_name>,
                                 std::shared_ptr<symbol_table>>>
        types;
    std::shared_ptr<symbol_table> prev_table;
  };
} // namespace cyy::compiler
