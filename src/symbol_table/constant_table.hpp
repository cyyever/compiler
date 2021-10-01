/*!
 * \file constant_table.hpp
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

namespace cyy::compiler {
  class constant_table {
  public:
    struct entry {
      std::string lexeme;
    };
    using entry_ptr = std::shared_ptr<entry>;

  public:
    constant_table() = default;
    ~constant_table() = default;

    bool add_constant(std::string lexeme);
    entry_ptr get_constant(const std::string &lexeme) const;
    void clear() { constants.clear(); }

  private:
    std::unordered_map<std::string, entry_ptr> constants;
  };
} // namespace cyy::compiler
