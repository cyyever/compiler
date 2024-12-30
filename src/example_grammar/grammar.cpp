#include "grammar.hpp"

#include <cyy/algorithm/alphabet/common_tokens.hpp>
#include <cyy/computation/context_free_lang/common_grammar.hpp>

namespace cyy::compiler::example_grammar {
  using cyy::algorithm::common_token;
  std::shared_ptr<cyy::computation::LL_grammar> get_declaration_grammar() {
    using namespace cyy::computation;
    static std::shared_ptr<LL_grammar> grammar;
    if (grammar) {
      return grammar;
    }
    auto id = static_cast<CFG::terminal_type>(common_token::id);

    std::vector<CFG_production> production_vector;

    production_vector.emplace_back(
        "declarations",
        CFG_production::body_type{"full_type", id, ';', "declarations"});
    production_vector.emplace_back("declarations", CFG_production::body_type{});
    production_vector.emplace_back(
        "full_type", CFG_production::body_type{
                         static_cast<CFG::terminal_type>(common_token::record),
                         '{', "declarations", '}'});

    production_vector.emplace_back(
        "full_type", CFG_production::body_type{
                         static_cast<CFG::terminal_type>(common_token::CLASS),
                         id, "parent_class", '{', "declarations", '}'});
    production_vector.emplace_back(
        "parent_class",
        CFG_production::body_type{static_cast<CFG::terminal_type>(':'), id});

    production_vector.emplace_back("parent_class", CFG_production::body_type{});
    production_vector.emplace_back(
        "full_type", CFG_production::body_type{"type", "array_part"});
    production_vector.emplace_back(
        "type", CFG_production::body_type{
                    static_cast<CFG::terminal_type>(common_token::INT)});
    production_vector.emplace_back(
        "type", CFG_production::body_type{
                    static_cast<CFG::terminal_type>(common_token::FLOAT)});
    production_vector.emplace_back("array_part", CFG_production::body_type{});
    production_vector.emplace_back(
        "array_part",
        CFG_production::body_type{
            '[', static_cast<CFG::terminal_type>(common_token::number), ']',
            "array_part"});

    production_vector.emplace_back("S",
                                   CFG_production::body_type{"declarations"});

    CFG::production_set_type productions;
    for (auto const &production : production_vector) {
      productions[production.get_head()].emplace(production.get_body());
    }
    grammar = std::make_shared<LL_grammar>("common_tokens", "S", productions);

    return grammar;
  }
  std::shared_ptr<cyy::computation::SLR_grammar> get_expression_grammar() {
    using namespace cyy::computation;
    static std::shared_ptr<SLR_grammar> grammar;
    if (grammar) {
      return grammar;
    }
    auto production_set = cyy::computation::get_expression_productions();
    auto id = static_cast<CFG::terminal_type>(common_token::id);
    production_set["statement"].emplace(
        CFG_production::body_type{id, '=', "E", ';'});
    production_set["statement"].emplace(
        CFG_production::body_type{"array", '=', "E", ';'});
    production_set["statement"].emplace(CFG_production::body_type{"E", ';'});

    production_set["statements"].emplace(
        CFG_production::body_type{"statement", "statements"});
    production_set["statements"].emplace();
    grammar = std::make_shared<SLR_grammar>("common_tokens", "statements",
                                            production_set);

    return grammar;
  }
} // namespace cyy::compiler::example_grammar
