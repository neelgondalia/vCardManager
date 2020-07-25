#include <stdio.h>
#include <string.h>
#include "LinkedListAPI.h"
#include "VCardParser.h"

//color macros
#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

//Invalid File cases
#define FILECASE3 "testsuite/files/invalidFiles/case3.vcf"
#define FILECASE4 "testsuite/files/invalidFiles/case4.txt"
#define FILECASE5 "testsuite/files/invalidFiles/case5.VCF"
#define FILECASE6 "testsuite/files/invalidFiles/case6.VCARD"

//Invalid card cases
#define IC_FILECASE1 "testsuite/files/invalidCards/case1.vcf"
#define IC_FILECASE2 "testsuite/files/invalidCards/case2.vcard"
#define IC_FILECASE3 "testsuite/files/invalidCards/case3.vcf"
#define IC_FILECASE4 "testsuite/files/invalidCards/case4.vcard"
#define IC_FILECASE5 "testsuite/files/invalidCards/case5.vcf"
#define IC_FILECASE6 "testsuite/files/invalidCards/case6.vcf"

//Invalid prop cases
#define IP_FILECASE1 "testsuite/files/invalidProps/case1.vcf"
#define IP_FILECASE2 "testsuite/files/invalidProps/case2.vcf"
#define IP_FILECASE3 "testsuite/files/invalidProps/case3.vcf"
#define IP_FILECASE4 "testsuite/files/invalidProps/case4.vcf"
#define IP_FILECASE5 "testsuite/files/invalidProps/case5.vcf"
#define IP_FILECASE6 "testsuite/files/invalidProps/case6.vcf"
#define IP_FILECASE7 "testsuite/files/invalidProps/case7.vcf"
#define IP_FILECASE8 "testsuite/files/invalidProps/case8.vcf"
#define IP_FILECASE9 "testsuite/files/invalidProps/case9.vcf"
#define IP_FILECASE10 "testsuite/files/invalidProps/case10.vcf"

//Order of errors cases
#define OOE_FILECASE1 "testsuite/files/OOE/case1.txt"
#define OOE_FILECASE2 "testsuite/files/OOE/case2.vcf"
#define OOE_FILECASE3 "testsuite/files/OOE/case3.vcf"

//write card testing macros
#define WT_FILETOPARSE "testsuite/files/valid/testCard.vcf"
#define WT_FILETOCREATE "testsuite/files/writeCardFiles/testCard.vcf"

//validate card test files
#define VCT_FILECASE1 "testsuite/files/validateCardFiles/case1.vcf"
#define VCT_FILECASE2 "testsuite/files/validateCardFiles/case2.vcf"
#define VCT_FILECASE3 "testsuite/files/validateCardFiles/case3.vcf"
#define VCT_FILECASE4 "testsuite/files/validateCardFiles/case4.vcf"
#define VCT_FILECASE5 "testsuite/files/validateCardFiles/case5.vcf"
#define VCT_FILECASE6 "testsuite/files/validateCardFiles/case6.vcf"
#define VCT_FILECASE7 "testsuite/files/validateCardFiles/case7.vcf"
#define VCT_FILECASE8 "testsuite/files/validateCardFiles/case8.vcf"
#define VCT_FILECASE9 "testsuite/files/validateCardFiles/case9.vcf"
#define VCT_FILECASE10 "testsuite/files/validateCardFiles/case10.vcf"
#define VCT_FILECASE11 "testsuite/files/validateCardFiles/case11.vcf"
#define VCT_FILECASE12 "testsuite/files/validateCardFiles/case12.vcf"
#define VCT_FILECASE13 "testsuite/files/validateCardFiles/case13.vcf"
#define VCT_FILECASE14 "testsuite/files/validateCardFiles/case14.vcf"
#define VCT_FILECASE15 "testsuite/files/validateCardFiles/case15.vcf"

//custom tests
#define CT_toBeTested true
#define CT_CC_print false
#define CT_VC_print false
#define CT_expectedErr OK
#define CT_VALIDATE_FUNCTION_expectedErr OK
#define CT_FILECASE1 "testsuite/files/customTestFiles/testCard.vcf"

void __invalidFileTests();
void __invalidCardTests();
void __invalidPropTests();
void __orderOfErrorsTests();
void __TLParamHandlingTests();
void __customTests();
void __writeTests();
void __validateCardTests();
void __strListJSONConversionTests();
void __propertyJSONConversionTests();
void __dtJSONConversionTests();
void __cardJSONConversionTests();
void __addPropTests();

void TS_header(char *testName, int testNumber);
void TS_footer(char *testName, int testNumber);
