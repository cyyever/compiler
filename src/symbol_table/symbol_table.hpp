/*!
 * \file symbol_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string_view>
#include <unordered_set>
#include <utility>

#include <cyy/computation/lang/symbol.hpp>

#include "semantic_analysis/type_expression.hpp"
#include "token.hpp"

namespace cyy::compiler {
  using namespace cyy::computation;
  class symbol_table {
  public:
    struct entry {};
    struct symbol_entry : public entry {
      std::shared_ptr<type_expression::expression> type;
      std::string lexeme;
      size_t relative_address{};
      auto operator<=>(const symbol_entry &rhs) const {
        return relative_address <=> rhs.relative_address;
      }
    };
    using symbol_entry_ptr = std::shared_ptr<symbol_entry>;
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
    symbol_entry_ptr get_symbol(const std::string &lexeme) const;
    symbol_entry_ptr create_and_get_symbol(const std::string &lexeme);
    symbol_entry_ptr create_temporary_symbol(const std::string &lexeme);
    bool has_symbol(const std::string &lexeme) const;
    auto get_symbol_view() const {
      return symbols | std::views::values |
             std::views::transform([](const auto &ptr) { return *ptr; });
    }
    auto get_ordered_symbol_list() const {
      auto view = get_symbol_view();
      std::vector<symbol_entry> symbol_list(view.begin(), view.end());
      std::ranges::sort(symbol_list, [](auto const &a, auto const &b) {
        return a.relative_address < b.relative_address;
      });
      return symbol_list;
    }

    void add_relative_address_offset(size_t offset) {
      for (auto &[_, e] : symbols) {
        e->relative_address += offset;
      }
    }
    size_t get_next_relative_address() const { return next_relative_address; }
    void clear() {
      symbols.clear();
      types.clear();
      next_relative_address = 0;
    }

  private:
    std::unordered_map<std::string, symbol_entry_ptr> symbols;
    std::unordered_map<std::string, std::shared_ptr<type_entry>> types;
    std::shared_ptr<symbol_table> prev_table;
    size_t next_relative_address = 0;
  };
} // namespace cyy::compiler
