//
//  WholeTest.cpp
//  RulesTranslator
//
//  Created by 潇湘夜雨 on 2018/11/20.
//  Copyright © 2018 ssyram. All rights reserved.
//

#include "WholeTest.h"
#include "../core/Preprocessor.h"
#include "../core/TableGenerator.h"
#include "../core/AnalyzerGenerator.h"

namespace rules_translator::test
{
	void test()
	{
		FileInteractor fi(
			"D:/Git_repositories/Mini_C/doc/rule_origin.tsl",
			"D:/Git_repositories/Mini_C/src/lr1.hpp");
		AnalyzerGenerator analyzer(fi);
		analyzer.generate_head();

		Preprocessor pss(fi);
		RulesInfo* info = pss.generateInfo(); // info has been deleted in ~Preprocessor();
		TableGenerator generator(fi, info);
		generator.generate();

		analyzer.generate_tail();
	}
}