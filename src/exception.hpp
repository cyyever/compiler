/*!
 * \file exception.hpp
 *
 * \author cyy
 * \date 2018-03-03
 */

#pragma once

#include <stdexcept>

namespace cyy::compiler::exception {

class unexisted_production : public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

} // namespace cyy::compiler::exception
