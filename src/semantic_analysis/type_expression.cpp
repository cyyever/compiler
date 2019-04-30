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

  bool expression::equivalent_with(const expression &rhs) const {
    auto type_name_ptr = dynamic_cast<const type_name *>(&rhs);
    if (type_name_ptr && type_name_ptr->_equivalent_with(*this)) {
      return true;
    }
    return this->_equivalent_with(rhs);
  }

  bool basic_type::_equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const basic_type *>(&rhs);
    return ptr && ptr->type == type;
  }

  const std::shared_ptr<expression> &type_name::get_expression() const {
    return named_type;
  }

  bool type_name::_equivalent_with(const expression &rhs) const {
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

  bool array_type::_equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const array_type *>(&rhs);
    return ptr && ptr->element_number == element_number &&
           element_type->equivalent_with(*(ptr->element_type));
  }

  bool class_type::_equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const class_type *>(&rhs);
    if (!ptr) {
      return false;
    }
    if (parent_class_type) {
      if (!ptr->parent_class_type) {
        return false;
      }
      if (!parent_class_type->equivalent_with(*(ptr->parent_class_type))) {
        return false;
      }
    } else {
      if (ptr->parent_class_type) {
        return false;
      }
    }
    if (ptr->field_types.size() != field_types.size()) {
      return false;
    }
    for (size_t i = 0; i < field_types.size(); i++) {
      if (field_types[i].first != ptr->field_types[i].first) {
        return false;
      }
      if (!field_types[i].second->equivalent_with(
              *(ptr->field_types[i].second))) {
        return false;
      }
    }
    return true;
  }

  bool class_type::is_class_type(const expression &type_expr) {
    auto type_name_ptr = dynamic_cast<const type_name *>(&type_expr);
    if (type_name_ptr) {
      return is_class_type(*type_name_ptr);
    }
    return dynamic_cast<const class_type *>(&type_expr) != nullptr;
  }
  bool record_type::_equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const record_type *>(&rhs);
    if (!ptr) {
      return false;
    }
    if (ptr->field_types.size() != field_types.size()) {
      return false;
    }
    for (size_t i = 0; i < field_types.size(); i++) {
      if (field_types[i].first != ptr->field_types[i].first) {
        return false;
      }
      if (!field_types[i].second->equivalent_with(
              *(ptr->field_types[i].second))) {
        return false;
      }
    }
    return true;
  }

  bool function_type::_equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const function_type *>(&rhs);
    return ptr && from_type->equivalent_with(*(ptr->from_type)) &&
           to_type->equivalent_with(*(ptr->to_type));
  }

  bool Cartesian_product_type::_equivalent_with(const expression &rhs) const {
    auto ptr = dynamic_cast<const Cartesian_product_type *>(&rhs);
    return ptr && first_type->equivalent_with(*(ptr->first_type)) &&
           second_type->equivalent_with(*(ptr->second_type));
  }
} // namespace cyy::compiler::type_expression
