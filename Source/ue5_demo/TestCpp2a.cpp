// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCpp2a.h"
#include <Logging/LogVerbosity.h>
#include <Logging/LogCategory.h>
DEFINE_LOG_CATEGORY_STATIC(TESTCPP2a,Warning,All)

#include <format>
using namespace std;
TestCpp2a::TestCpp2a()
{
}

TestCpp2a::~TestCpp2a()
{
}

void TestCpp2a::Test()
{
	 auto str = std::format("HelloWorld {}"sv,20);
	 auto fstr = FString(str.c_str());
	 UE_LOG(TESTCPP2a,Warning,TEXT("%s"),*fstr);
}
