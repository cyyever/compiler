/*!
 * \file syntax_tree.hpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-21
 */

#pragma once

#include <memory>

#include <fmt/format.h>

#include "operator.hpp"
#include "symbol_table/symbol_table.hpp"

namespace cyy::compiler::IR::three_address_code {
  struct address {
    explicit address(std::string lexeme_) : lexeme{std::move(lexeme_)} {}
    std::string lexeme;
  };

  struct name : public address {
    explicit name(symbol_table::symbol_entry_ptr entry_)
        : address(entry_->lexeme), entry{entry_} {}
    symbol_table::symbol_entry_ptr entry;
  };

  struct constant : public address {
    using address::address;
  };
  using name_ptr = std::shared_ptr<name>;
  using address_ptr = std::shared_ptr<address>;

  class instruction {
  public:
    instruction() = default;
    virtual ~instruction() = default;
    void set_label(std::string_view label_) { label = label_; }
    virtual std::string to_string() const { return ""; }

  protected:
    std::string label;
  };

  template <typename operator_type>
  struct binary_assignment_instruction : public instruction {
    static_assert(std::is_same_v<operator_type, binary_arithmetic_operator> ||
                  std::is_same_v<operator_type, binary_logical_operator>);

    std::string to_string() const override {
      if constexpr (std::is_same_v<operator_type, binary_arithmetic_operator>) {
        if (op == binary_arithmetic_operator::plus) {
          return fmt::format("{} = {} + {}", result->lexeme, left->lexeme,
                             right->lexeme);
        }
      }
      return "";
    }
    operator_type op;
    name_ptr result;
    address_ptr left;
    address_ptr right;
  };
  using binary_logical_assignment_instruction =
      binary_assignment_instruction<binary_logical_operator>;
  using binary_arithmetic_assignment_instruction =
      binary_assignment_instruction<binary_arithmetic_operator>;

  template <typename operator_type>
  struct unary_assignment_instruction : public instruction {
    static_assert(std::is_same_v<operator_type, unary_arithmetic_operator> ||
                  std::is_same_v<operator_type, unary_logical_operator>);

    std::string to_string() const override {
      if constexpr (std::is_same_v<operator_type, unary_arithmetic_operator>) {
        if (op == unary_arithmetic_operator::minus) {
          return fmt::format("{} = minus {}", result->lexeme, operand->lexeme);
        }
      }
      return "";
    }

    operator_type op;
    name_ptr result;
    address_ptr operand;
  };
  using unary_arithmetic_assignment_instruction =
      unary_assignment_instruction<unary_arithmetic_operator>;
  using unary_arithmetic_assignment_instruction =
      unary_assignment_instruction<unary_arithmetic_operator>;

  struct copy_instruction : public instruction {
    copy_instruction(name_ptr result_, address_ptr operand_)
        : result(std::move(result_)), operand(std::move(operand_)) {}
    ~copy_instruction() override = default;

    std::string to_string() const override {
      return result->lexeme + " = " + operand->lexeme;
    }
    name_ptr result;
    address_ptr operand;
  };

  struct unconditional_jump_instruction : public instruction {
    std::string target_label;
  };

  template <bool negative_if>
  struct conditional_jump_instruction : public instruction {
    address_ptr operand;
    std::string target_label;
  };

  struct relational_operation_and_conditional_jump_instruction
      : public instruction {
    relational_operator op;
    address_ptr left;
    address_ptr right;
    std::string target_label;
  };
  struct param_instruction : public instruction {
    address_ptr param;
  };

  class procedure_call_instruction : public instruction {
  public:
    procedure_call_instruction(std::string_view procedure_label_,
                               size_t param_number_)
        : procedure_label(procedure_label_), param_number(param_number_) {}
    ~procedure_call_instruction() override = default;

  protected:
    std::string procedure_label;
    size_t param_number;
  };

  class function_call_instruction : public procedure_call_instruction {
  public:
    function_call_instruction(std::string_view function_label_,
                              size_t param_number_, name_ptr result_)
        : procedure_call_instruction(function_label_, param_number_),
          result(std::move(result_)) {}
    ~function_call_instruction() override = default;

  private:
    address_ptr result;
  };

  class return_instruction : public instruction {
  public:
    return_instruction() = default;
    ~return_instruction() override = default;
  };

  class return_with_value_instruction : public return_instruction {
  public:
    explicit return_with_value_instruction(address_ptr value_)
        : value(std::move(value_)) {}
    ~return_with_value_instruction() override = default;

  private:
    address_ptr value;
  };

  struct array_copy_instruction : public instruction {
    name_ptr result;
    name_ptr array;
    address_ptr index;
  };

  struct copy_to_array_instruction : public instruction {
    name_ptr result_array;
    address_ptr index;
    name_ptr operand;
  };

  class address_assignment_instruction : public instruction {
  public:
    address_assignment_instruction(name_ptr result_, name_ptr operand_)
        : result(std::move(result_)), operand(std::move(operand_)) {}
    ~address_assignment_instruction() override = default;

  private:
    name_ptr result;
    name_ptr operand;
  };

  class pointer_assignment_instruction : public instruction {
  public:
    pointer_assignment_instruction(name_ptr result_, name_ptr operand_)
        : result(std::move(result_)), operand(std::move(operand_)) {}
    ~pointer_assignment_instruction() override = default;

  private:
    name_ptr result;
    name_ptr operand;
  };

  class result_pointer_assignment_instruction : public instruction {
  public:
    result_pointer_assignment_instruction(name_ptr result_, name_ptr operand_)
        : result(std::move(result_)), operand(std::move(operand_)) {}
    ~result_pointer_assignment_instruction() override = default;

  private:
    name_ptr result;
    name_ptr operand;
  };
} // namespace cyy::compiler::IR::three_address_code
