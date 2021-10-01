/*!
 * \file type_expression.cpp
 *
 * \brief
 * \author cyy
 * \date 2019-02-24
 */

#include "type_expression.hpp"

#include <cassert>

#include "symbol_table/symbol_table.hpp"

namespace cyy::compiler::type_expression {

  bool expression::equivalent_with(const expression &rhs) const {
    return get_signature() == rhs.get_signature();
  }

  value_number_method::signature_type basic_type::get_signature() const {
    return {0, static_cast<value_number_method::value_number_type>(type)};
  }

  const std::shared_ptr<expression> &type_name::get_type() const {
    return named_type;
  }
  value_number_method::signature_type type_name::get_signature() const {
    return get_type()->get_signature();
  }

  bool type_name::is_type_name(const expression &type_expr) {
    return dynamic_cast<const type_name *>(&type_expr) != nullptr;
  }

  void type_name::make_stand_for_self() { stand_for_self = true; }

  value_number_method::signature_type array_type::get_signature() const {
    auto signature = element_type->get_signature();
    signature.insert(signature.begin(), 1);
    signature.push_back(element_number);
    return signature;
  }
  value_number_method::signature_type record_type::get_signature() const {
    value_number_method::signature_type signature;
    signature.push_back(2);
    for (auto const &[_, field_type] : field_types) {
      auto field_signature = field_type->get_signature();
      signature.insert(signature.end(), field_signature.begin(),
                       field_signature.end());
    }
    return signature;
  }

  value_number_method::signature_type class_type::get_signature() const {
    value_number_method::signature_type signature;
    if (parent_class) {
      signature = parent_class->get_signature();
    } else {
      signature.push_back(4);
    }
    for (auto const &[_, field_type] : field_types) {
      auto field_signature = field_type->get_signature();
      signature.insert(signature.end(), field_signature.begin(),
                       field_signature.end());
    }
    return signature;
  }

  bool class_type::is_class_type(const expression &type_expr) {
    auto type_name_ptr = dynamic_cast<const type_name *>(&type_expr);
    if (type_name_ptr) {
      return is_class_type(*type_name_ptr->get_type());
    }
    return dynamic_cast<const class_type *>(&type_expr) != nullptr;
  }

  record_type::record_type(
      std::vector<std::pair<std::string, std::shared_ptr<expression>>>
          field_types_)
      : field_types(std::move(field_types_)) {}

  record_type::record_type(
      std::shared_ptr<symbol_table> associated_symbol_table_)
      : associated_symbol_table{associated_symbol_table_} {

    auto sorted_entries = associated_symbol_table->get_ordered_symbol_list();
    for (auto const &entry : sorted_entries) {
      field_types.emplace_back(entry.lexeme, entry.type);
    }
  }

  value_number_method::signature_type function_type::get_signature() const {
    value_number_method::signature_type signature;
    signature.push_back(5);
    auto from_signature = from_type->get_signature();
    signature.insert(signature.end(), from_signature.begin(),
                     from_signature.end());
    auto to_signature = to_type->get_signature();
    signature.insert(signature.end(), to_signature.begin(), to_signature.end());
    return signature;
  }

  value_number_method::signature_type
  Cartesian_product_type::get_signature() const {
    value_number_method::signature_type signature;
    signature.push_back(6);
    auto first_signature = first_type->get_signature();
    signature.insert(signature.end(), first_signature.begin(),
                     first_signature.end());
    auto second_signature = second_type->get_signature();
    signature.insert(signature.end(), second_signature.begin(),
                     second_signature.end());
    return signature;
  }

} // namespace cyy::compiler::type_expression
