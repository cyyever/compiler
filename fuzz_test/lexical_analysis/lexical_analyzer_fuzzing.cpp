/*!
 * \file lexical_analyzer_fuzzing.cpp
 *
 * \brief 测试lexical_analyzer
 * \author cyy
 * \date 2018-10-28
 */

#include "example_grammar/lexical_analyzer.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  std::string source_code;

  for (size_t i = 0; i < Size; i++) {
    source_code.push_back(Data[i] % 95 + 32);
  }
  try {
    auto analyzer = cyy::compiler::example_grammar::get_lexical_analyzer(false);

    analyzer->set_source_code(std::istringstream(source_code));
    analyzer->scan();
  } catch (const std::invalid_argument &) {
  }
  return 0; // Non-zero return values are reserved for future use.
}
