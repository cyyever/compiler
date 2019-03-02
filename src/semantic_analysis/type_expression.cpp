/*!
 * \file type_expression.cpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-24
 */

#include "type_expression.hpp"
#include <cassert>

namespace cyy::compiler::type_expression {

  bool basic_type::equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const basic_type *>(&rhs);
    return ptr && ptr->type == type;
  }

  std::shared_ptr<expression> &type_name::get_expression() const {
    auto it = name_and_expressions.find(name);
    assert(it != name_and_expressions.end());
    return it->second;
  }

  bool type_name::equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const type_name *>(&rhs);
    if (stand_for_self) {
      return ptr && ptr->name == name;
    }

    if (ptr) {
      return get_expression()->equivalent_with(*ptr->get_expression());
    }
    return get_expression()->equivalent_with(rhs);
  }

  void type_name::make_stand_for_self() { stand_for_self = true; }

  bool array_type::equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const array_type *>(&rhs);
    return ptr && ptr->element_number == element_number &&
           element_type->equivalent_with(*(ptr->element_type));
  }

  bool function_type::equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const function_type *>(&rhs);
    return ptr && from_type->equivalent_with(*(ptr->from_type)) &&
           to_type->equivalent_with(*(ptr->to_type));
  }

  bool Cartesian_product_type::equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const Cartesian_product_type *>(&rhs);
    return ptr && first_type->equivalent_with(*(ptr->first_type)) &&
           second_type->equivalent_with(*(ptr->second_type));
  }
} // namespace cyy::compiler::type_expression
