/*!
 * \file syntax_tree.hpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-21
 */

#pragma once

#include "operator.hpp"
#include <boost/container_hash/hash.hpp>
#include <cyy/computation/lang/symbol.hpp>
#include <map>
#include <memory>
#include <symbol_table/symbol_table.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

namespace std {
  template <> struct hash<std::vector<size_t>> {
    size_t operator()(const std::vector<size_t> &x) const noexcept {
      std::size_t seed = 0;
      for (auto n : x) {
        boost::hash_combine(seed, n);
      }
      return seed;
    }
  };
} // namespace std

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
    using value_number_type = size_t;
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
      auto [it, has_insertion] = value_numbers.try_emplace(get_signature(), 0);
      if (has_insertion) {
        it->second = alloc_value_number();
      }
      return it->second;
    }

  private:
    virtual std::vector<size_t> get_signature() = 0;

    virtual expression_node_ptr make_DAG_node() = 0;
    static size_t alloc_value_number() { return next_value_number++; }

    static inline std::unordered_map<value_number_type, expression_node_ptr>
        DAG_nodes;

    static inline std::map<std::vector<size_t>, value_number_type>
        value_numbers;
    static inline value_number_type next_value_number{0};
  };

  class symbol_node : public expression_node {
  public:
    explicit symbol_node(std::shared_ptr<symbol_table_entry> entry_)
        : entry{std::move(std::move(entry_))} {}

    std::vector<size_t> get_signature() override {
      return {reinterpret_cast<size_t>(entry.get())};
    }
    expression_node_ptr make_DAG_node() override {
      return std::make_shared<symbol_node>(entry);
    }

  private:
    std::shared_ptr<symbol_table_entry> entry;
  };

  template <typename op_type>
  class _binary_expression_node : public expression_node {
  public:
    _binary_expression_node(op_type op_,
                           std::shared_ptr<expression_node> left_,
                           std::shared_ptr<expression_node> right_)
        : op{op_}, left{std::move(std::move(left_))}, right{std::move(
                                                          std::move(right_))} {}

    expression_node_ptr make_DAG_node() override {
      return std::make_shared<_binary_expression_node<op_type>>(
          op, left->common_subexpression_elimination_by_DAG(),
          right->common_subexpression_elimination_by_DAG());
    }
    std::vector<size_t> get_signature() override {
      return std::vector<size_t>{static_cast<size_t>(op),
                                 left->get_value_number(),
                                 right->get_value_number()};
    }

  public:
    op_type op;
    std::shared_ptr<expression_node> left;
    std::shared_ptr<expression_node> right;
  };
  using binary_arithmetic_node=_binary_expression_node<binary_arithmetic_operator>;
} // namespace cyy::compiler::syntax_tree
