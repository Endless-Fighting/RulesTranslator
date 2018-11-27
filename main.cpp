//
//  main.cpp
//  RulesTranslator
//
//  Created by 潇湘夜雨 on 2018/11/16.
//  Copyright © 2018 ssyram. All rights reserved.
//
#include <iostream>
#include <string>
#include "test/WholeTest.h"

void run()
{
	try {
		rules_translator::test::test();
		std::cout << "------------------- done -------------------" << std::endl;
	}
	catch (std::string& ex) { std::cout << ex << "!!!!!!" << std::endl; }
	catch (...) { std::cout << "Unknown Exception" << std::endl; }
}

int main(int argc, const char * argv[])
{
	::run();
	std::getchar();
	return 0;
}