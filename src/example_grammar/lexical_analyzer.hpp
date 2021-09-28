/*!
 * \file declaration_sdd.hpp
 *
 * \brief
 */
#pragma once

#include "lexical_analysis/lexical_analyzer.hpp"

namespace cyy::compiler::example_grammar {
  std::shared_ptr<cyy::compiler::lexical_analyzer>
  get_lexical_analyzer(bool ignore_whitespace = true);

} // namespace cyy::compiler::example_grammar
