#include <gtest/gtest.h>

// on windows we can check if a debugger is present
// we use this to choose whether to catch exceptions or not
// we dont if the debugger is present, because then we can get 
// it to break where the exception is thrown

#ifdef _MSC_VER
#include <Windows.h>
inline void configure_exceptions(){
	if (!IsDebuggerPresent())
		::testing::GTEST_FLAG(catch_exceptions)=true;
}
#else
inline void configure_exceptions(){}
#endif

int main(int argc, char **argv) {

	// determine if we should catch exceptions or not
	configure_exceptions();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
