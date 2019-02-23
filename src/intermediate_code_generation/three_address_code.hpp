/*!
 * \file syntax_tree.hpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-21
 */

#pragma once

#include <cyy/computation/lang/lang.hpp>
#include <memory>

namespace cyy::compiler {
  using namespace cyy::computation;
  class three_address_code {
  public:
    class address {
    public:
      address(symbol_string lexeme_) : lexeme{std::move(lexeme_)} {}
      virtual ~address() = default;

    protected:
      symbol_string lexeme;
    };

    class name : public address {
    public:
      using address::address;
    };

    class constant : public address {
      using address::address;
    };

    enum class binary_arithmetic_operator {
      addtion,
      subtraction,
      multiplication,
    };

    enum class unary_arithmetic_operator {
      minus,
    };

    enum class unary_logical_operator {
      negation,
    };

    enum class binary_logical_operator {
      AND,
      OR,
    };

    enum class relational_operator {
      equal,
      unequal,
      greater,
      greater_or_equal,
      less,
      less_or_equal,
    };

    class instruction {
    public:
      instruction() = default;
      virtual ~instruction() = default;
      void set_label(std::string_view label_) { label = label_; }

    protected:
      std::string label;
    };

    template <typename operator_type>
    class binary_assignment_instruction : public instruction {
      static_assert(std::is_same_v<operator_type, binary_arithmetic_operator> ||
                    std::is_same_v<operator_type, binary_logical_operator>);

    public:
      binary_assignment_instruction(operator_type op_, name result_,
                                    address left_, address right_)
          : op(op_), result(std::move(result_)), left(std::move(left_)),
            right(std::move(right_)) {}
      ~binary_assignment_instruction() override = default;

    private:
      operator_type op;
      name result;
      address left;
      address right;
    };

    template <typename operator_type>
    class unary_assignment_instruction : public instruction {
      static_assert(std::is_same_v<operator_type, unary_arithmetic_operator> ||
                    std::is_same_v<operator_type, unary_logical_operator>);

    public:
      unary_assignment_instruction(operator_type op_, name result_,
                                   address operand_)
          : op(op_), result(std::move(result_)), operand(std::move(operand_)) {}
      ~unary_assignment_instruction() override = default;

    private:
      operator_type op;
      name result;
      address operand;
    };

    class copy_instruction : public instruction {
    public:
      copy_instruction(name result_, address operand_)
          : result(std::move(result_)), operand(std::move(operand_)) {}
      ~copy_instruction() override = default;

    private:
      name result;
      address operand;
    };

    class unconditional_jump_instruction : public instruction {
    public:
      unconditional_jump_instruction(std::string_view target_label_)
          : target_label{target_label_} {}
      ~unconditional_jump_instruction() override = default;

    private:
      std::string target_label;
    };

    template <bool negative_if>
    class conditional_jump_instruction : public instruction {
    public:
      conditional_jump_instruction(address operand_,
                                   std::string_view target_label_)
          : operand{std::move(operand_)}, target_label{target_label_} {}
      ~conditional_jump_instruction() override = default;

    protected:
      address operand;
      std::string target_label;
    };

    class relational_operation_and_conditional_jump_instruction
        : public instruction {
    public:
      relational_operation_and_conditional_jump_instruction(
          relational_operator op_, address left_, address right_,
          std::string_view target_label_)
          : op(op_), left(std::move(left_)),
            right(std::move(right_)), target_label{target_label_} {}
      ~relational_operation_and_conditional_jump_instruction() override =
          default;

    protected:
      relational_operator op;
      address left;
      address right;
      std::string target_label;
    };
    class param_instruction : public instruction {
    public:
      param_instruction(address param_) : param(std::move(param_)) {}
      ~param_instruction() override = default;

    private:
      address param;
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
                                size_t param_number_, name result_)
          : procedure_call_instruction(function_label_, param_number_),
            result(std::move(result_)) {}
      ~function_call_instruction() override = default;

    private:
      address result;
    };

    class return_instruction : public instruction {
    public:
      return_instruction() = default;
      ~return_instruction() override = default;
    };

    class return_with_value_instruction : public return_instruction {
    public:
      return_with_value_instruction(address value_)
          : value(std::move(value_)) {}
      ~return_with_value_instruction() override = default;

    private:
      address value;
    };

    class indexed_copy_instruction : public instruction {
    public:
      indexed_copy_instruction(name result_, name operand_, size_t index_)
          : result(std::move(result_)), operand(std::move(operand_)),
            index(index_) {}
      ~indexed_copy_instruction() override = default;

    private:
      name result;
      name operand;
      size_t index;
    };

    class result_indexed_copy_instruction : public instruction {
    public:
      result_indexed_copy_instruction(name result_, size_t index_,
                                      address operand_)
          : result(std::move(result_)), index(index_),
            operand(std::move(operand_)) {}
      ~result_indexed_copy_instruction() override = default;

    private:
      name result;
      size_t index;
      address operand;
    };

    class address_assignment_instruction : public instruction {
    public:
      address_assignment_instruction(name result_, name operand_)
          : result(std::move(result_)), operand(std::move(operand_)) {}
      ~address_assignment_instruction() override = default;

    private:
      name result;
      name operand;
    };

    class pointer_assignment_instruction : public instruction {
    public:
      pointer_assignment_instruction(name result_, name operand_)
          : result(std::move(result_)), operand(std::move(operand_)) {}
      ~pointer_assignment_instruction() override = default;

    private:
      name result;
      name operand;
    };

    class result_pointer_assignment_instruction : public instruction {
    public:
      result_pointer_assignment_instruction(name result_, name operand_)
          : result(std::move(result_)), operand(std::move(operand_)) {}
      ~result_pointer_assignment_instruction() override = default;

    private:
      name result;
      name operand;
    };
  };
} // namespace cyy::compiler
