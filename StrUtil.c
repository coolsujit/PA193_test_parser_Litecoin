#include "CuTest.h"
#include<string.h>
    
char* StrToUpper(char* str) {
	//str = strupr(str);
	int i=0;
	while(str[i])
	   {
	      str[i]=toupper(str[i]);
	      i++;
	   }
    return str;
}

void TestStrToUpper(CuTest *tc) {
    char* input = strdup("hello world");
    char* actual = StrToUpper(input);
    char* expected = "HELLO WORLD";
    CuAssertStrEquals(tc, expected, actual);
}

CuSuite* StrUtilGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestStrToUpper);
    return suite;
}
