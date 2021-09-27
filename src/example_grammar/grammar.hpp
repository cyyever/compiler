/*!
 * \file grammar.hpp
 *
 * \brief
 */

#pragma once
#include <cyy/computation/context_free_lang/ll_grammar.hpp>
#include <cyy/computation/context_free_lang/slr_grammar.hpp>
namespace cyy::compiler::example_grammar {
  std::shared_ptr<cyy::computation::LL_grammar> get_declaration_grammar();
  std::shared_ptr<cyy::computation::SLR_grammar> get_expression_grammar();
} // namespace cyy::compiler::example_grammar
