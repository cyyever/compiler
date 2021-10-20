
#pragma once

#include <cstddef>
#include <unordered_map>
#include <cyy/algorithm/hash.hpp>

namespace cyy::compiler{
  class value_number_method {
    public:
      using value_number_type = size_t;
      using signature_type=std::vector<value_number_type>;
    value_number_type get_value_number(const signature_type& signature)
    {
      auto [it, has_insertion] = value_numbers.try_emplace(signature, 0);
      if (has_insertion) {
        it->second = next_value_number;
        next_value_number++;
      }
      return it->second;
    }

    private:
    std::unordered_map<signature_type, value_number_type>
        value_numbers;
    value_number_type next_value_number{0};
  };
}
