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

#include "algorithm/value_number_method.hpp"
#include "exception.hpp"

namespace cyy::compiler {
  class symbol_table;
}
namespace cyy::compiler::type_expression {
  class expression {
  public:
    expression() = default;
    virtual ~expression() = default;
    virtual value_number_method::signature_type get_signature() const = 0;
    bool equivalent_with(const expression &rhs) const;
    virtual size_t get_width() const { return 0; }

  protected:
    static inline value_number_method method;
  };

  class basic_type : public expression {
  public:
    enum class type_enum {
      BOOL,
      CHAR,
      INT,
      FLOAT,
    };
    explicit basic_type(type_enum type_) : type(type_) {}
    ~basic_type() override = default;

    void set_width(size_t width_) { width = width_; }
    value_number_method::signature_type get_signature() const override;
    size_t get_width() const override {
      if (width != 0) {
        return width;
      }
      switch (type) {
        case type_enum::BOOL:
          return 1;
        case type_enum::CHAR:
          return 1;
        case type_enum::INT:
          return 4;
        case type_enum::FLOAT:
          return 8;
      }
      return 0;
    }

  private:
    type_enum type;
    size_t width = 0;
  };

  class type_name : public expression {
  public:
    type_name(std::string name_, const std::shared_ptr<expression> &named_type_,
              bool stand_for_self_ = false)
        : name(std::move(name_)), stand_for_self{stand_for_self_} {

      auto type_name_ptr = std::dynamic_pointer_cast<type_name>(named_type_);
      if (type_name_ptr) {
        named_type = type_name_ptr->get_type();
      } else {
        named_type = named_type_;
      }
    }
    ~type_name() override = default;

    const std::string &get_name() const { return name; }
    const std::shared_ptr<expression> &get_type() const;
    value_number_method::signature_type get_signature() const override;

    static bool is_type_name(const expression &type_expr);
    size_t get_width() const override { return named_type->get_width(); }

  private:
    std::string name;
    std::shared_ptr<expression> named_type;
    bool stand_for_self{false};
  };

  struct array_type : public expression {
    array_type(std::shared_ptr<expression> element_type_,
               size_t element_number_)
        : element_type(std::move(element_type_)),
          element_number(element_number_) {}
    ~array_type() override = default;

    value_number_method::signature_type get_signature() const override;

    size_t get_width() const override {
      return element_number * element_type->get_width();
    }

    std::shared_ptr<expression> element_type;
    size_t element_number;
  };

  class record_type : public expression {
  public:
    explicit record_type(
        std::vector<std::pair<std::string, std::shared_ptr<expression>>>
            field_types_);
    explicit record_type(
        std::shared_ptr<symbol_table> associated_symbol_table_);
    ~record_type() override = default;

    value_number_method::signature_type get_signature() const override;

    size_t get_width() const override {
      if (total_width != 0) {
        return total_width;
      }
      for (auto const &[_, field_type] : field_types) {
        total_width += field_type->get_width();
      }
      return total_width;
    }
    std::shared_ptr<symbol_table> get_symbol_table() const {
      return associated_symbol_table;
    }

  protected:
    std::vector<std::pair<std::string, std::shared_ptr<expression>>>
        field_types;
    std::shared_ptr<symbol_table> associated_symbol_table;

  private:
    mutable size_t total_width{0};
  };

  class class_type : public record_type {
  public:
    class_type(std::shared_ptr<expression> parent_class_,
               std::vector<std::pair<std::string, std::shared_ptr<expression>>>
                   field_types_)
        : record_type(std::move(field_types_)),
          parent_class(std::move(parent_class_))

    {
      if (parent_class && !is_class_type(*parent_class)) {
        throw exception::not_class_type("parent class");
      }
    }
    class_type(std::shared_ptr<expression> parent_class_,
               std::shared_ptr<symbol_table> associated_symbol_table_)
        : record_type(associated_symbol_table_),
          parent_class(std::move(parent_class_))

    {
      if (parent_class && !is_class_type(*parent_class)) {
        throw exception::not_class_type("parent class");
      }
    }
    ~class_type() override = default;
    value_number_method::signature_type get_signature() const override;

    static bool is_class_type(const expression &type_expr);
    size_t get_width() const override {
      size_t width = record_type::get_width();
      if (parent_class) {
        width += parent_class->get_width();
      }
      return width;
    }

  private:
    std::shared_ptr<expression> parent_class;
  };

  class function_type : public expression {
  public:
    function_type(std::shared_ptr<expression> from_type_,
                  std::shared_ptr<expression> to_type_)
        : from_type(std::move(std::move(from_type_))),
          to_type(std::move(std::move(to_type_))) {}
    ~function_type() override = default;

    value_number_method::signature_type get_signature() const override;

  private:
    std::shared_ptr<expression> from_type;
    std::shared_ptr<expression> to_type;
  };

  class Cartesian_product_type : public expression {
  public:
    Cartesian_product_type(std::shared_ptr<expression> first_type_,
                           std::shared_ptr<expression> second_type_)
        : first_type(std::move(std::move(first_type_))),
          second_type(std::move(std::move(second_type_))) {}
    ~Cartesian_product_type() override = default;

    value_number_method::signature_type get_signature() const override;

    size_t get_width() const override {
      return first_type->get_width() + second_type->get_width();
    }

  private:
    std::shared_ptr<expression> first_type;
    std::shared_ptr<expression> second_type;
  };

} // namespace cyy::compiler::type_expression
