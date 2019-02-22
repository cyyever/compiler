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
    class negative_conditional_jump_instruction
        : public conditional_jump_instruction {
    public:
      using conditional_jump_instruction::conditional_jump_instruction;
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
  };
} // namespace cyy::compiler
