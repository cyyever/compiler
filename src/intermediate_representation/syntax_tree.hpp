/*!
 * \file syntax_tree.hpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-21
 */

#pragma once

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
  class expression_node;
  using expression_node_ptr = std::shared_ptr<expression_node>;

  class expression_node : public node {
  public:
    using value_number_type = uint64_t;
    expression_node() = default;
    ~expression_node() override = default;

    expression_node_ptr common_subexpression_elimination_by_DAG() {
      auto value_number = get_value_number();
      auto it = DAG_nodes.find(value_number);
      if (it != DAG_nodes.end()) {
        return it->second;
      }
      return DAG_nodes.try_emplace(value_number, make_node()).first->second;
    }

    virtual size_t get_value_number() = 0;
    virtual expression_node_ptr make_node() = 0;

  protected:
    static size_t alloc_value_number() { return next_value_number++; }

  protected:
    static inline std::unordered_map<value_number_type, expression_node_ptr>
        DAG_nodes;

  private:
    static inline value_number_type next_value_number{0};
  };

  class symbol_node : public expression_node {
  public:
    explicit symbol_node(std::shared_ptr<symbol_table_entry> entry_)
        : entry{std::move(std::move(entry_))} {}

    size_t get_value_number() override {
      auto [it, has] =
          value_numbers.try_emplace(reinterpret_cast<size_t>(entry.get()), 0);
      if (!has) {
        it->second = alloc_value_number();
      }
      return it->second;
    }
    expression_node_ptr make_node() override {
      return std::make_shared<symbol_node>(entry);
    }

  private:
    std::shared_ptr<symbol_table_entry> entry;
    static inline std::unordered_map<size_t, value_number_type> value_numbers;
  };

  class binary_expression_node : public expression_node {
  public:
    binary_expression_node(binary_operator op_,
                           std::shared_ptr<expression_node> left_,
                           std::shared_ptr<expression_node> right_)
        : op{op_}, left{std::move(std::move(left_))}, right{std::move(
                                                          std::move(right_))} {}

    expression_node_ptr make_node() override {
      return std::make_shared<binary_expression_node>(
          op, left,right);
    }

    size_t get_value_number() override {
      auto [it, has] = value_numbers.try_emplace(
          std::make_tuple(op, left->get_value_number(),
                          right->get_value_number()),
          0);
      if (!has) {
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
} // namespace cyy::compiler::syntax_tree
