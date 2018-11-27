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
			"C:/Users/lenovo/Source/Repos/___RulsTSL/x64/Release/rule_V1.tsl",
			"C:/Users/lenovo/Source/Repos/___RulsTSL/x64/Release/rule_V1.cpp");
		Preprocessor pss(fi);
		RulesInfo *info = pss.generateInfo();
		TableGenerator generator(fi, info);
		generator.generate();
		AnalyzerGenerator analyzer(fi);
		analyzer.generate();
	}
}