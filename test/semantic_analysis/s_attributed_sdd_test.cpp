/*!
 * \file s_attributed_sdd_test.cpp
 *
 * \brief 测试S_attributed_SDD
 * \author cyy
 * \date 2018-10-29
 */
#if __has_include(<CppCoreCheck\Warnings.h>)
#include <CppCoreCheck\Warnings.h>
#pragma warning(disable : ALL_CPPCORECHECK_WARNINGS)
#endif
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <cyy/computation/lang/common_tokens.hpp>
#include "../../src/semantic_analysis/s_attributed_sdd.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("run") {
	 S_attributed_SDD sdd(nullptr);
}
