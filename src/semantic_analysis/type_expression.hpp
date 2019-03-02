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
#include <utility>
#include <vector>

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

  class array_type : public expression {
  public:
    array_type(std::shared_ptr<expression> element_type_,
               size_t element_number_)
        : element_type(std::move(element_type_)),
          element_number(element_number_) {}
    ~array_type() override = default;

    bool equivalent_with(const expression &rhs) const override;

  private:
    std::shared_ptr<expression> element_type;
    size_t element_number;
  };

  class record_type : public expression {
  public:
    record_type(std::vector<std::pair<std::string, std::shared_ptr<expression>>>
                    field_types_)
        : field_types(std::move(field_types_)) {}
    ~record_type() override = default;

    bool equivalent_with(const expression &rhs) const override;

  private:
    std::vector<std::pair<std::string, std::shared_ptr<expression>>>
        field_types;
  };

  class function_type : public expression {
  public:
    function_type(std::shared_ptr<expression> from_type_,
                  std::shared_ptr<expression> to_type_)
        : from_type(std::move(from_type_)), to_type(to_type_) {}
    ~function_type() override = default;

    bool equivalent_with(const expression &rhs) const override;

  private:
    std::shared_ptr<expression> from_type;
    std::shared_ptr<expression> to_type;
  };

  class Cartesian_product_type : public expression {
  public:
    Cartesian_product_type(std::shared_ptr<expression> first_type_,
                           std::shared_ptr<expression> second_type_)
        : first_type(std::move(first_type_)), second_type(second_type_) {}
    ~Cartesian_product_type() override = default;

    bool equivalent_with(const expression &rhs) const override;

  private:
    std::shared_ptr<expression> first_type;
    std::shared_ptr<expression> second_type;
  };

} // namespace cyy::compiler::type_expression
