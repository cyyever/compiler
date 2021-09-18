/*!
 * \file symbol_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <unordered_set>
#include <utility>

#include <cyy/computation/lang/symbol.hpp>

#include "semantic_analysis/type_expression.hpp"
#include "token.hpp"

namespace cyy::compiler {
  using namespace cyy::computation;
  class symbol_table {
  public:
    struct entry {
      std::shared_ptr<symbol_table> associated_symbol_table;
    };
    struct symbol_entry : public entry {
      std::shared_ptr<type_expression::expression> type;
      std::string lexeme;
      size_t relative_address{};
      size_t width{};
      auto operator<=>(const symbol_entry &rhs) const {
        return relative_address <=> rhs.relative_address;
      }
    };
    struct type_entry : public entry {
      std::shared_ptr<type_expression::type_name> type;
    };

  public:
    symbol_table() = default;
    ~symbol_table() = default;

    void set_prev_table(std::shared_ptr<symbol_table> prev_table_) {
      prev_table = std::move(prev_table_);
    }

    bool add_type(type_entry entry);
    std::shared_ptr<type_entry> get_type(const std::string &type_name) const;
    bool add_symbol(symbol_entry e);
    std::shared_ptr<symbol_entry> get_symbol(const std::string &lexeme) const;
    bool has_symbol(const std::string &lexeme) const;
    auto get_symbol_view() const { return symbols | std::views::values; }
    void foreach_symbol(
        const std::function<void(const symbol_entry &)> &callback) const {
      for (const auto &[_, e] : symbols) {
        callback(*e);
      }
    }

    size_t get_total_width() const {
      size_t total_width = 0;
      for (const auto &[_, e] : symbols) {
        total_width += e->width;
      }
      return total_width;
    }

    void add_relative_address_offset(size_t offset) {
      for (auto &[_, e] : symbols) {
        e->relative_address += offset;
      }
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<symbol_entry>> symbols;
    std::unordered_map<std::string, std::shared_ptr<type_entry>> types;
    std::shared_ptr<symbol_table> prev_table;
  };
} // namespace cyy::compiler
