/*!
 * \file syntax_tree.hpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-21
 */

#pragma once

#include "operator.hpp"
#include "algorithm/value_number_method.hpp"
/* #include <cyy/computation/lang/symbol.hpp> */
#include <symbol_table/symbol_table.hpp>
#include <unordered_map>
#include <utility>

namespace cyy::compiler::syntax_tree {
  using namespace cyy::computation;

  class node {
  public:
    node() = default;
    virtual ~node() = default;
  };
  class expression_node;
  using expression_node_ptr = std::shared_ptr<expression_node>;

  class expression_node : public node {
  public:
    using value_number_type = value_number_method::value_number_type;
    using signature_type=value_number_method::signature_type;
    expression_node() = default;
    ~expression_node() override = default;

    expression_node_ptr common_subexpression_elimination_by_DAG() {
      auto value_number = get_value_number();
      auto it = DAG_nodes.find(value_number);
      if (it != DAG_nodes.end()) {
        return it->second;
      }
      return DAG_nodes.try_emplace(value_number, make_DAG_node()).first->second;
    }

      value_number_type get_value_number() {
      return method.get_value_number(get_signature());
    }

  private:
    virtual signature_type get_signature() = 0;

    virtual expression_node_ptr make_DAG_node() = 0;

    static inline std::unordered_map<value_number_type, expression_node_ptr>
        DAG_nodes;
    static inline value_number_method method;
  };

  struct constant_node : public expression_node {
  public:
    explicit constant_node(std::string lexeme_)
        : lexeme{std::move(lexeme_)} {}

      signature_type get_signature() override {
        signature_type signature;
        signature.push_back(static_cast<value_number_type>(lexeme_type::constant));
        for(auto c :lexeme) {
          signature.push_back(c);
        }
        return signature;
    }
    expression_node_ptr make_DAG_node() override {
      return std::make_shared<constant_node>(*this);
    }

    std::string lexeme;
  };

  class symbol_node : public expression_node {
  public:
    explicit symbol_node(std::shared_ptr<symbol_table::symbol_entry> entry_)
        : entry{std::move(entry_)} {}

      signature_type get_signature() override {
      return { static_cast<value_number_type>( lexeme_type::symbol),reinterpret_cast<value_number_type>(entry.get())};
    }
    expression_node_ptr make_DAG_node() override {
      return std::make_shared<symbol_node>(*this);
    }

  private:
    std::shared_ptr<symbol_table::symbol_entry> entry;
  };

  template <typename op_type>
  class _binary_expression_node : public expression_node {
  public:
    _binary_expression_node(op_type op_,
                           std::shared_ptr<expression_node> left_,
                           std::shared_ptr<expression_node> right_)
        : op{op_}, left{std::move(left_)}, right{ std::move(right_)} {}

    expression_node_ptr make_DAG_node() override {
      return std::make_shared<_binary_expression_node<op_type>>(
          op, left->common_subexpression_elimination_by_DAG(),
          right->common_subexpression_elimination_by_DAG());
    }
      signature_type get_signature() override {
      return {static_cast<size_t>(op),
                                 left->get_value_number(),
                                 right->get_value_number()};
    }

  public:
    op_type op;
    std::shared_ptr<expression_node> left;
    std::shared_ptr<expression_node> right;
  };
  using binary_arithmetic_node=_binary_expression_node<binary_arithmetic_operator>;
  using binary_logical_node=_binary_expression_node<binary_logical_operator>;
} // namespace cyy::compiler::syntax_tree
