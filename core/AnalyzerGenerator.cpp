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
			"\n\n"
			"// #define OUTPUT_DFA // define in test_xxx.cpp before include the file\n"
			"// #define out // define in test_xxx.cpp before include the file\n"
			"#ifdef OUTPUT_DFA\n"
			"#include \"../test/test.h\"\n"
			"			using namespace Mini_C::TEST;\n"
			"#endif // OUTPUT_DFA\n"
			"\n\n"
			"namespace Mini_C::LR1\n"
			"{\n"
			"	using namespace util;\n"
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
			"	        const size_t length = production_elementAmount_table[na];\n"
			"	        std::vector<object_type> content(length);\n"
			"	        for (size_t i = 0; i < length; ++i) {\n"
			"	            content[length - 1 - i] = std::move(astack.top().object);\n"
			"	            astack.pop();\n"
			"	        }\n"
			"	        const size_t condition = goto_table[astack.top().condition][production_left_table[na]];\n"
			"#ifdef OUTPUT_DFA\n"
			"			out << \"GOTO: <\" << productionID2name[na]\n"
			"				<< \"> [\" << astack.top().condition << \"->\" << condition << \"]\" << std::endl;\n"
			"#endif // OUTPUT_DFA\n"
			"	        if (!condition)\n"
			"	            throw std::make_pair(t, \"condition error\");\n"
			"	        const auto pf = pf_map.find(na);\n"
			"	        if (pf == pf_map.end())\n"
			"	            astack.emplace(condition, default_object_type());\n"
			"	        else\n"
			"	            astack.emplace(condition, pf->second(content.data()));\n"
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
			"			// shift\n"
			"			if (nextAction > 0)\n"
			"			{\n"
			"#ifdef OUTPUT_DFA\n"
			"				out << \"SHIFT: [\" << astack.top().condition << \"->\" << nextAction << \"], \\t\";\n"
			"				output_token_t(t._token, out);\n"
			"#endif // OUTPUT_DFA\n"
			"				astack.emplace((condition_of_analysis)nextAction, t);\n"
			"			}\n"
			"			else if (nextAction < 0)\n"
			"			{\n"
			"#ifdef OUTPUT_DFA\n"
			"				out << \"Reduce: [\" << astack.top().condition << \"], id: \" << -nextAction << \" <\" << productionID2name[-nextAction] << \">\" << std::endl;\n"
			"#endif // OUTPUT_DFA\n"
			"				reduce<finish>(-nextAction, t);\n"
			"			}\n"
			"	        else {\n"
			"	            if constexpr (finish)\n"
			"					throw std::pair<token_type, std::string>(t, \"Final Position Error\");\n"
			"	            else throw std::pair<token_type, std::string>(t, \"Syntax Error!!!\");\n"
			"	        }\n"
			"	    } // end function void analyze();\n"
			"\n"
			"	}; // end class SyntacticAnalyzer;\n"
			"	\n"
			"	template <\n"
			"			typename T,\n"
			"			typename = std::enable_if_t<std::is_same_v<token_type,\n"
			"			std::remove_cv_t<std::remove_reference_t<decltype(std::declval<T>().operator[](0u))>>>\n"
			"			&& std::is_same_v<std::size_t, decltype(std::declval<T>().size())>>\n"
			"			> std::vector<std::pair<token_type, std::string>> analyze(const T& token_stream)\n"
			"		{\n"
			"			std::vector<std::pair<token_type, std::string>> error_result;\n"
			"			if (!token_stream.size()) return error_result;\n"
			"			SyntacticAnalyzer analyzer;\n"
			"			const std::size_t size = token_stream.size();\n"
			"			bool error_sequence = false; // denote whether there is error immediately before\n"
			"			for (size_t i = 0; i < size; i++)\n"
			"			{\n"
			"				bool error = false;\n"
			"				try {\n"
			"					out << \"\\n----------------------------------------------------------\\n\\n\";\n"
			"					analyzer.analyze(token_stream[i]);\n"
			"				}\n"
			"				catch (const std::pair<token_type, std::string>& e) {\n"
			"					error = true;\n"
			"					if (!error_sequence) error_result.push_back(e);\n"
			"					error_sequence = true;\n"
			"				}\n"
			"				if (!error) error_sequence = false;\n"
			"			}\n"
			"			try {\n"
			"				out << \"\\n----------------------------------------------------------\\n\\n\";\n"
			"				token_type __eof__ = token_type{\n"
			"					std::tuple<lexer::token_t, lexer::pos_t>\n"
			"					(lexer::type::__EOF__, token_stream[size - 1]._pos + 1),\n"
			"					token_stream[size - 1]._line\n"
			"				};\n"
			"				analyzer.analyze<true>(__eof__);\n"
			"			}\n"
			"			catch (const std::pair<token_type, std::string>& e) {\n"
			"				error_result.push_back(e);\n"
			"			}\n"
			"			return error_result;\n"
			"		} // end function std::vector<std::pair<token_type, std::string>> analyze();\n"
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
