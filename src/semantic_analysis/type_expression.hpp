/*!
 * \file type_expression.hpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-24
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "../exception.hpp"

namespace cyy::compiler::type_expression {
  class expression {
  public:
    expression() = default;
    virtual ~expression() = default;
    virtual bool equivalent_with(const expression &rhs) const = 0;
  };
  class basic_type : public expression {
  public:
    enum class type_enum {
      BOOL,
      CHAR,
      INTEGEA,
      FLOAT,
      VOID,
    };

    basic_type(type_enum type_) : type(type_) {}
    ~basic_type() override = default;

    bool equivalent_with(const expression &rhs) const override;

  private:
    type_enum type;
  };

  class type_name : public expression {
  public:
    type_name(std::string name_, std::shared_ptr<expression> expr)
        : name(std::move(name_)) {
      if (!name_and_expressions.try_emplace(name, expr).second) {
        throw exception::type_name_confliction(name);
      }
    }
    ~type_name() override = default;

    std::shared_ptr<expression> &get_expression() const;
    bool equivalent_with(const expression &rhs) const override;

    static void make_stand_for_self();

  private:
    std::string name;
    static inline bool stand_for_self{false};
    static inline std::unordered_map<std::string, std::shared_ptr<expression>>
        name_and_expressions;
  };
} // namespace cyy::compiler::type_expression
