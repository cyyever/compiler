/*!
 * \file lexical_analyzer_fuzzing.cpp
 *
 * \brief 测试lexical_analyzer
 * \author cyy
 * \date 2018-10-28
 */

#include "../../src/lexical_analysis/lexical_analyzer.hpp"
#include <cyy/computation/lang/common_tokens.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  using namespace cyy::computation;
  std::string source_code;

  for (size_t i = 0; i < Size; i++) {
    source_code.push_back(Data[i] % 95 + 32);
  }
  try {

    static cyy::compiler::lexical_analyzer analyzer("common_tokens");
    static bool inited = false;

    if (!inited) {
      analyzer.append_pattern(static_cast<symbol_type>(common_token::id),
                              U"[a-zA-Z_][a-zA-Z_0-9]*");
      analyzer.append_pattern(static_cast<symbol_type>(common_token::digit),
                              U"[0-9]+");
      analyzer.append_pattern(
          static_cast<symbol_type>(common_token::whitespace),
          U"[ \\v\\f\\t\\n\\r\\t]*");
      analyzer.append_pattern('+', U"\\+");
      analyzer.append_pattern('*', U"\\*");
      analyzer.append_pattern('=', U"=");
      inited = true;
    }
    analyzer.set_source_code(std::istringstream(source_code));
    analyzer.scan();
  } catch (const std::invalid_argument &) {
  }
  return 0; // Non-zero return values are reserved for future use.
}
