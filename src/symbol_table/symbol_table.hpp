/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <functional>
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
    explicit symbol_table(std::shared_ptr<symbol_table> prev_table_)
        : prev_table(prev_table_) {}
    ~symbol_table() = default;

    bool add_entry(entry e) { return entries.emplace(e.lexeme, e).second; }
    std::optional<entry> get_entry(const std::string &lexeme) {
      auto it = entries.find(lexeme);
      if (it != entries.end()) {
        return it->second;
      }
      if (prev_table) {
        return prev_table->get_entry(lexeme);
      }
      return {};
    }

    bool add_type_name(std::shared_ptr<type_expression::type_name> expr) {
      return type_names.emplace(expr->get_name(), expr).second;
    }

    std::optional<std::shared_ptr<type_expression::type_name>>
    get_type(const std::string &type_name) {
      auto it = type_names.find(type_name);
      if (it != type_names.end()) {
        return it->second;
      }
      if (prev_table) {
        return prev_table->get_type(type_name);
      }
      return {};
    }

    void
    foreach_entry(const std::function<void(const entry &)> &callback) const {
      for (auto const &[_, e] : entries) {
        callback(e);
      }
    }

  private:
    std::unordered_map<std::string, entry> entries;
    std::unordered_map<std::string, std::shared_ptr<type_expression::type_name>>
        type_names;
    std::shared_ptr<symbol_table> prev_table;
  };
} // namespace cyy::compiler
