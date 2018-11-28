//
//  AnalyzerGenerator.cpp
//  RulesTranslator
//
//  Created by 潇湘夜雨 on 2018/11/20.
//  Copyright © 2018 ssyram. All rights reserved.
//

#include "AnalyzerGenerator.h"
#include "../util/TranslateException.h"
#include <fstream>

namespace rules_translator {

	class AnalyzerGnerator_Impl {

		constexpr static const char* content_head =
			"#ifndef RULE_H\n"
			"#define RULE_H\n"
			"#include \"lexer.h\"\n"
			"#include \"../util/util.h\"\n"
			"#include <functional>\n"
			"#include <stack>\n"
			"#include <optional>\n"
			"namespace Mini_C::LR1\n"
			"{\n"
			"	using token_type = lexer::Token;\n"
			"\n"
			"	// using token_t = const std::variant<type, identifier, numeric_t, string_literal_t>;\n"
			"	lexer::type getType(const token_type& token)\n"
			"	{\n"
			"		return std::visit(Mini_C::util::overloaded{\n"
			"				[](const lexer::type& type) { return type; },\n"
			"				[](const lexer::identifier) { return lexer::type::IDENTIFIER; },\n"
			"				[](const lexer::numeric_t) { return lexer::type::NUMBER_CONSTANT; },\n"
			"				[](const lexer::string_literal_t) { return lexer::type::STR_LITERAL; },\n"
			"			}, token._token);\n"
			"	}\n"
			"\n"
			;

		constexpr static const char *content_tail =
			"	class SyntacticAnalyzer {\n"
			"	    using symbol_type = long long; // minus is terminate while the opposite is nonterminate\n"
			"	    using condition_of_analysis = size_t;\n"
			"	    struct analysis_symbol {\n"
			"	        condition_of_analysis condition;\n"
			"	        object_type object;\n"
			"	        analysis_symbol() = default;\n"
			"	        analysis_symbol(condition_of_analysis c, object_type &o): condition(c), object(o) {}\n"
			"	        analysis_symbol(condition_of_analysis c, object_type &&o): condition(c), object(o) {}\n"
			"	    };\n"
			"\n"
			"	    std::stack<analysis_symbol> astack; // stands for analysis stack\n"
			"\n"
			"	    template <bool finish>\n"
			"	    void reduce(ll na, const token_type &t) {\n"
			"	        size_t length = production_elementAmount_table[na];\n"
			"	        std::vector<object_type> content(length);\n"
			"	        for (size_t i = 0; i < length; ++i) {\n"
			"	            content[length - 1 - i] = std::move(astack.top().object);\n"
			"	            astack.pop();\n"
			"	        }\n"
			"	        size_t condition = goto_table[astack.top().condition][production_left_table[na]];\n"
			"	        if (!condition)\n"
			"	            throw std::make_pair(t, \"condition error\");\n"
			"	        const auto &pf = pf_map.find(na);\n"
			"	        if (pf == pf_map.end())\n"
			"	            astack.emplace(condition, default_object_type());\n"
			"	        else\n"
			"	            astack.emplace(condition, pf->second(content));\n"
			"	        analyze<finish>(t);\n"
			"	    } // end function void reduce();\n"
			"\n"
			"\n"
			"	public:\n"
			"\n"
			"	    SyntacticAnalyzer() { astack.emplace(1, default_object_type()); }\n"
			"\n"
			"	    template <bool finish = false>\n"
			"	    void analyze(const token_type &t) {\n"
			"	        ll symtype;\n"
			"	        if constexpr (finish)\n"
			"	            symtype = eof; // this should be determined by eof type\n"
			"	        else symtype = (ll)get_type(t);\n"
			"	        ll nextAction = action_table[astack.top().condition][symtype];\n"
			"	        if (nextAction > 0)\n"
			"	            // shift\n"
			"	            astack.emplace((condition_of_analysis)nextAction, t);\n"
			"	        else if (nextAction < 0)\n"
			"	            reduce<finish>(-nextAction, t);\n"
			"	        else {\n"
			"	            if constexpr (finish) throw std::make_pair(t, \"Final Position Error\");\n"
			"	            else throw std::make_pair(t, \"abc\");\n"
			"	        }\n"
			"	    } // end function void analyze();\n"
			"\n"
			"	}; // end class SyntacticAnalyzer;\n"
			"	\n"
			"	template <\n"
			"	typename T,\n"
			"	typename = std::enable_if_t<std::is_same_v<token_type,\n"
			"	std::remove_cv_t<std::remove_reference_t<decltype(std::declval<T>().operator[](0u))>>>\n"
			"	&& std::is_same_v<std::size_t, decltype(std::declval<T>().size())>>\n"
			"	> std::optional<std::pair<token_type, std::string>> analyze(const T& token_stream) {\n"
			"	    if (!token_stream.size())\n"
			"	        return std::nullopt;\n"
			"	    SyntacticAnalyzer analyzer;\n"
			"	    try {\n"
			"	        for (size_t i = 0; i < token_stream.size(); ++i)\n"
			"	            analyzer.analyze(token_stream[i]);\n"
			"	        analyzer.analyze<true>(token_stream[token_stream.size() - 1]);\n"
			"	    }\n"
			"	    catch (std::pair<token_type, std::string> &e) {\n"
			"	        return e;\n"
			"	    }\n"
			"	    return std::nullopt;\n"
			"	} // end function std::optional<std::pair<token_type, std::string>> analyze();\n"
			"\n"
			"} // end namespace Mini_C::LR1;\n"
			"\n"
			"#endif // !RULE_H\n"
			;

		FileInteractor &fi;

	public:
		AnalyzerGnerator_Impl(FileInteractor &fi) : fi(fi) {}
		void generate_head() const {
			fi.writeln(content_head);
		}
		void generate_tail() {
			fi.writeln(content_tail);
		}
	};

	AnalyzerGenerator::~AnalyzerGenerator() {
		delete impl;
	}
	AnalyzerGenerator::AnalyzerGenerator(FileInteractor &fi) {
		impl = new AnalyzerGnerator_Impl(fi);
	}
	void AnalyzerGenerator::generate_head() const {
		impl->generate_head();
	}
	void AnalyzerGenerator::generate_tail() const {
		impl->generate_tail();
	}
}
