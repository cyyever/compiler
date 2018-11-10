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
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include <cyy/computation/lang/common_tokens.hpp>
#include <cyy/computation/contex_free_lang/slr_grammar.hpp>
#include "../../src/semantic_analysis/s_attributed_sdd.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("run") {



    std::vector<std::pair<CFG::nonterminal_type, CFG::production_body_type>> production_vector;

    std::vector<SDD::semantic_rule> rules;

    production_vector.emplace_back("L",CFG::production_body_type{"E"});

		    
    rules.emplace_back(	SDD::semantic_rule{

		    "L.val",
		    {"E.val"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    result=arguments.at(0).get();
		    std::cout<<std::any_cast<int>(result)<<std::endl;
		    }
		    }) ;

    production_vector.emplace_back("E",CFG::production_body_type {"E",'+', "T"});

    rules.emplace_back(	SDD::semantic_rule{
		    "E.val",
		    {"E.val","T.val"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    std::cout<<"aaaaaa "<<arguments[0].get().type().name()<<std::endl;
		    auto E_val=std::any_cast<int>(arguments.at(0).get());
		    auto T_val=std::any_cast<int>(arguments.at(1).get());
		    result=E_val+T_val;
		    }
		    });

    production_vector.emplace_back("E",CFG::production_body_type {"T"});
    rules.emplace_back(	SDD::semantic_rule{
		    "E.val",
		    {"T.val"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    result=arguments.at(0).get();
		    }
		    });
    production_vector.emplace_back("T",CFG::production_body_type 
        { "T",'*',"F"});

    rules.emplace_back(	SDD::semantic_rule{
		    "T.val",
		    {"T.val","F.val"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    auto T_val=std::any_cast<int>(arguments.at(0).get());
		    auto F_val=std::any_cast<int>(arguments.at(1).get());
		    result=T_val*F_val;
		    }
		    });

    production_vector.emplace_back("T",CFG::production_body_type 
        { "F"});
    rules.emplace_back(	SDD::semantic_rule{
		    "T.val",
		    {"F.val"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    result=arguments.at(0).get();
		    }
		    });

    production_vector.emplace_back("F",CFG::production_body_type 
        {'(', "E", ')'});

    rules.emplace_back(	SDD::semantic_rule{
		    "F.val",
		    {"E.val"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    result=arguments.at(0).get();
		    }
		    });

    auto digit = static_cast<CFG::terminal_type>(common_token::digit);
    production_vector.emplace_back("F",CFG::production_body_type 
        {digit});

    rules.emplace_back(	SDD::semantic_rule{
		    "F.val",
		    {"$1"},
		    []( std::any &result, const std::vector<std::reference_wrapper<const std::any>> &arguments)  {
		    std::cout<<"lexeme is "<< static_cast<char>(std::any_cast<token>(arguments.at(0)).lexeme[0])<<std::endl;
		    int digit=static_cast<char>(std::any_cast<token>(arguments.at(0)).lexeme[0])-'0';
		    std::cout<<"digit is "<<digit <<std::endl;
		    result=digit;


		    std::cout<<result.type().name()<<std::endl;
		    }
		    });


    REQUIRE(production_vector.size()==rules.size());
    std::map<CFG::nonterminal_type, std::vector<CFG::production_body_type>> productions;
    for(auto const &[head,body]:production_vector) {
	    productions[head].emplace_back(body);
    }

    SLR_grammar grammar("common_tokens", "L", productions);
    S_attributed_SDD sdd(grammar);

    for(size_t i=0;i<production_vector.size();i++) {
	    sdd.add_synthesized_attribute(
			    production_vector[i],
			    std::move( rules[i])
			    );

    }

    std::vector<token> tokens;
    tokens.push_back(token{'(',U"(",{}});
    tokens.push_back(token{digit,U"1",{}});
    tokens.push_back(token{'+',U"+",{}});
    tokens.push_back(token{digit,U"2",{}});
    tokens.push_back(token{')',U")",{}});
 //   tokens.push_back(token{'*',U"*",{}});
  //  tokens.push_back(token{digit,U"3",{}});

    sdd.run(tokens);

}
