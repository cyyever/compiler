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
    enum class binary_opterator {
      addtion,
      multiplication,
    };

    class node {
    public:
      node() = default;
      virtual ~node() = default;
    };

    class symbol_node : public node {
    public:
      symbol_node(symbol_string lexeme_) : lexeme{std::move(lexeme_)} {}
    private:
      symbol_string lexeme;
    };

    class binary_expression_node : public node {
    private:
      binary_opterator op;
      std::shared_ptr<node> left;
      std::shared_ptr<node> right;
    };
  };
} // namespace cyy::compiler
