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
  class  three_address_code {
  public:
    class address {
    public:
      address() = default;
      virtual ~address() = default;
    };

    class name : public  address {
    public:
      name(symbol_string lexeme_) : lexeme{std::move(lexeme_)} {}

    private:
      symbol_string lexeme;
    };

    class constant: public  address {
    public:
      name(symbol_string lexeme_) : lexeme{std::move(lexeme_)} {}

    private:
      symbol_string lexeme;
    };

    enum class binary_arithmetic_operator {
      addtion,
      subtraction,
      multiplication,
    };

    enum class binary_logical_operator :{
      AND,
      OR,
    };

    class instruction {
    public:
      instruction() = default;
      virtual ~instruction() = default;
    };

    template<typename operator_type>
    class assignment_instruction:public instruction {
    public:
      assignment_instruction(operator_type op_,name result_,address left_,address right_):op(op_),result(std::move(result_)),
      left(std::move(left_)),right(std::move(right_)) { }
      virtual ~assignment_instruction() = default;
    private:
      operator_type op;
      name result;
      address left;
      address right;
    };

  };
} // namespace cyy::compiler
