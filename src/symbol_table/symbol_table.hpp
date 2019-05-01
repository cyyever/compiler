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

#include <cyy/computation/lang/lang.hpp>

#include "../semantic_analysis/type_expression.hpp"
#include "../token/token.hpp"

namespace cyy::compiler {

  class symbol_table;
  struct symbol_table_entry {
    std::string lexeme;
    std::shared_ptr<type_expression::expression> type;
    size_t relative_address{};
    std::shared_ptr<symbol_table> associated_symbol_table;

    bool operator==(const symbol_table_entry &rhs) const {
      return lexeme == rhs.lexeme;
    }
  };
} // namespace cyy::compiler

namespace std {
  template <> struct hash<cyy::compiler::symbol_table_entry> {
    size_t operator()(const cyy::compiler::symbol_table_entry &e) const
        noexcept {
      return ::std::hash<std::string>()(e.lexeme);
    }
  };

  template <> struct less<cyy::compiler::symbol_table_entry> {
    bool operator()(const cyy::compiler::symbol_table_entry &lhs,
                    const cyy::compiler::symbol_table_entry &rhs) const
        noexcept {
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

    bool has_entry(const std::string &lexeme) const;
    bool add_entry(const symbol_table_entry &e);
    std::optional<symbol_table_entry>
    get_entry(const std::string &lexeme) const;

    bool add_type_name(std::shared_ptr<type_expression::type_name> expr);

    std::optional<std::shared_ptr<type_expression::type_name>>
    get_type(const std::string &type_name);

    void foreach_entry(
        const std::function<void(const symbol_table_entry &)> &callback) const {
      for (auto const &e : entries) {
        callback(e);
      }
    }

  private:
    std::unordered_set<symbol_table_entry> entries;
    std::unordered_map<std::string, std::shared_ptr<type_expression::type_name>>
        type_names;
    std::shared_ptr<symbol_table> prev_table;
  };
} // namespace cyy::compiler
