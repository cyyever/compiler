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
  class syntax_tree {
  public:
    enum class binary_operator : uint64_t {
      addtion,
      subtraction,
      multiplication,
    };

    class node {
    public:
      node() = default;
      virtual ~node() = default;
    };

    class expression_node : public node {
    public:
      using value_number_type = uint64_t;
      expression_node() = default;
      virtual ~expression_node() = default;

      virtual std::shared_ptr<expression_node>
      common_subexpression_elimination_by_DAG() = 0;

      virtual size_t get_value_number() = 0;

    protected:
      static size_t alloc_value_number() { return next_value_number++; }

    protected:
      static inline std::unordered_map<value_number_type,
                                       std::shared_ptr<expression_node>>
          DAG_nodes;

    private:
      static inline value_number_type next_value_number{0};
    };

    class symbol_node : public expression_node {
    public:
      symbol_node(symbol_string lexeme_) : lexeme{std::move(lexeme_)} {}

      std::shared_ptr<expression_node>
      common_subexpression_elimination_by_DAG() override {
        auto value_number = get_value_number();
        auto it = DAG_nodes.find(value_number);
        if (it != DAG_nodes.end()) {
          return it->second;
        }
        return DAG_nodes
            .try_emplace(value_number, std::make_shared<symbol_node>(lexeme))
            .first->second;
      }

      size_t get_value_number() override {
        auto [it, has] = value_numbers.try_emplace(lexeme, 0);
        if (has) {
          it->second = alloc_value_number();
        }
        return it->second;
      }

    private:
      symbol_string lexeme;
      static inline std::unordered_map<symbol_string, value_number_type>
          value_numbers;
    };

    class binary_expression_node : public expression_node {
    public:
      binary_expression_node(binary_operator op_,
                             std::shared_ptr<expression_node> left_,
                             std::shared_ptr<expression_node> right_)
          : op{op_}, left{left_}, right{right_} {}

      std::shared_ptr<expression_node>
      common_subexpression_elimination_by_DAG() override {
        auto value_number = get_value_number();
        auto it = DAG_nodes.find(value_number);
        if (it != DAG_nodes.end()) {
          return it->second;
        }
        return DAG_nodes
            .try_emplace(value_number,
                         std::make_shared<binary_expression_node>(
                             op,
                             left->common_subexpression_elimination_by_DAG(),
                             right->common_subexpression_elimination_by_DAG()))
            .first->second;
      }

      size_t get_value_number() override {
        auto [it, has] = value_numbers.try_emplace(
            std::make_tuple(op, left->get_value_number(),
                            right->get_value_number()),
            0);
        if (has) {
          it->second = alloc_value_number();
        }
        return it->second;
      }

    private:
      binary_operator op;
      std::shared_ptr<expression_node> left;
      std::shared_ptr<expression_node> right;
      static inline std::map<
          std::tuple<binary_operator, value_number_type, value_number_type>,
          value_number_type>
          value_numbers;
    };
  };
} // namespace cyy::compiler
