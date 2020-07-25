/*
* 0940429
* Neel Gondalia
* CIS 2750- A1
* October 1, 2018
* testsuite.c
*/

#include "testsuite.h"
#include "CardTestUtilities.h"
#include "VCardParser.h"

int main(int argc, char *argv[])
{
	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "INVALID FILE TESTS RUNNING...\n\n" RESET);
	__invalidFileTests();
	printf(YELLOW "INVALID FILE TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "INVALID CARD TESTS RUNNING...\n\n" RESET);
	__invalidCardTests();
	printf(YELLOW "INVALID CARD TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "INVALID PROP TESTS RUNNING...\n\n" RESET);
	__invalidPropTests();
	printf(YELLOW "INVALID PROP TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "ORDER OF ERRORS TESTS RUNNING...\n\n" RESET);
	__orderOfErrorsTests();
	printf(YELLOW "ORDER OF ERRORS TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "TOP LEVEL FUNCTIONS- PARAMETER HANDLING TESTS RUNNING...\n\n" RESET);
	__TLParamHandlingTests();
	printf(YELLOW "TOP LEVEL FUNCTIONS- PARAMETER HANDLING TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "WRITE CARD TO FILE TESTS RUNNING...\n\n" RESET);
	__writeTests();
	printf(YELLOW "WRITE CARD TO FILE TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "VALIDATECARD FUNCTION TESTS RUNNING...\n\n" RESET);
	__validateCardTests();
	printf(YELLOW "VALIDATECARD FUNCTION TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "STRLIST JSON CONVERSION TESTS RUNNING...\n\n" RESET);
	__strListJSONConversionTests();
	printf(YELLOW "STRLIST JSON CONVERSION TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "PROPERTY JSON CONVERSION TESTS RUNNING...\n\n" RESET);
	__propertyJSONConversionTests();
	printf(YELLOW "PROPERTY JSON CONVERSION TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "DATETIME JSON CONVERSION TESTS RUNNING...\n\n" RESET);
	__dtJSONConversionTests();
	printf(YELLOW "DATETIME JSON CONVERSION TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "CARD JSON CONVERSION TESTS RUNNING...\n\n" RESET);
	__cardJSONConversionTests();
	printf(YELLOW "CARD JSON CONVERSION TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "ADD PROPERTY TESTS RUNNING...\n\n" RESET);
	__addPropTests();
	printf(YELLOW "ADD PROPERTY TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);

	printf(YELLOW "**************************************************************************************************\n" RESET);
	printf(YELLOW "CUSTOM TESTS RUNNING...\n\n" RESET);
	__customTests();
	printf(YELLOW "CUSTOM TESTS COMPLETED...\n" RESET);
	printf(YELLOW "**************************************************************************************************\n" RESET);
	return 0;
}

void __invalidFileTests()
{
	VCardErrorCode err = OK;
	Card *tempCard;
	int i = 1;

	TS_header("invalid file: NULL CASE", i);
	err = createCard(NULL, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("invalid file: NULL CASE", i);
	i++;

	TS_header("invalid file: EMPTY FILENAME PARAMETER CASE", i);
	err = createCard("", &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("invalid file: EMPTY FILENAME PARAMETER CASE", i);
	i++;

	TS_header("invalid file: FILE DNE CASE", i);
	err = createCard(FILECASE3, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("invalid file: FILE DNE CASE", i);
	i++;

	TS_header("invalid file: INVALID EXTENSION CASE", i);
	err = createCard(FILECASE4, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("invalid file: INVALID EXTENSION CASE", i);
	i++;

	TS_header("invalid file: EXTENSION IN CAPS .VCF CASE", i);
	err = createCard(FILECASE5, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("invalid file: EXTENSION IN CAPS .VCF CASE", i);
	i++;

	TS_header("invalid file: EXTENSION IN CAPS .VCARD CASE", i);
	err = createCard(FILECASE6, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("invalid file: EXTENSION IN CAPS .VCARD CASE", i);
	i++;
}

void __invalidCardTests()
{
	VCardErrorCode err = OK;
	Card *tempCard;
	int i = 1;

	TS_header("invalid card: MISSING BEGIN TAG CASE", i);
	err = createCard(IC_FILECASE1, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("invalid card: MISSING BEGIN TAG CASE", i);
	i++;

	TS_header("invalid card: MISSING END TAG CASE", i);
	err = createCard(IC_FILECASE2, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("invalid card: MISSING END TAG CASE", i);
	i++;

	TS_header("invalid card: MISSING VERSION TAG CASE", i);
	err = createCard(IC_FILECASE3, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("invalid card: MISSING VERSION TAG CASE", i);
	i++;

	TS_header("invalid card: MISSING FN TAG CASE", i);
	err = createCard(IC_FILECASE4, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("invalid card: MISSING FN TAG CASE", i);
	i++;

	TS_header("invalid card: MISSING MULTIPLE REQUIRED TAGS CASE", i);
	err = createCard(IC_FILECASE5, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("invalid card: MISSING MULTIPLE REQUIRED TAGS CASE", i);
	i++;

	TS_header("invalid card: VERSION IS NOT 4.0 CASE", i);
	err = createCard(IC_FILECASE6, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("invalid card: VERSION IS NOT 4.0 CASE", i);
	i++;
}

void __invalidPropTests()
{
	VCardErrorCode err = OK;
	Card *tempCard;
	int i = 1;

	TS_header("invalid prop: CONTENT LINES END WITH NEWLINE ONLY CASE", i);
	err = createCard(IP_FILECASE1, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: CONTENT LINES END WITH NEWLINE ONLY CASE", i);
	i++;

	TS_header("invalid prop: CONTENT LINES END WITH CR ONLY CASE", i);
	err = createCard(IP_FILECASE2, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: CONTENT LINES END WITH CR ONLY CASE", i);
	i++;

	TS_header("invalid prop: NO COLON IN CONTENT LINE CASE", i);
	err = createCard(IP_FILECASE3, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: NO COLON IN CONTENT LINE CASE", i);
	i++;

	TS_header("invalid prop: NO VALUE FOR PROPERTY CASE", i);
	err = createCard(IP_FILECASE4, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: NO VALUE FOR PROPERTY CASE", i);
	i++;

	TS_header("invalid prop: PROP PARAM WITH NO VALUE (with =) CASE", i);
	err = createCard(IP_FILECASE5, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: PROP PARAM WITH NO VALUE (with =) CASE", i);
	i++;

	TS_header("invalid prop: PROP PARAM WITH NO VALUE (without =) CASE", i);
	err = createCard(IP_FILECASE6, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: PROP PARAM WITH NO VALUE (without =) CASE", i);
	i++;

	TS_header("invalid prop: PROP WITH NO NAME (with param) CASE", i);
	err = createCard(IP_FILECASE7, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: PROP WITH NO NAME (with param) CASE", i);
	i++;

	TS_header("invalid prop: PROP WITH NO NAME (without param) CASE", i);
	err = createCard(IP_FILECASE8, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: PROP WITH NO NAME (without param) CASE", i);
	i++;

	TS_header("invalid prop: INVALID LINE ENDING CASE 3", i);
	err = createCard(IP_FILECASE9, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: INVALID LINE ENDINGS CASE 3", i);
	i++;

	TS_header("invalid prop: INVALID LINE ENDING CASE 4", i);
	err = createCard(IP_FILECASE10, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("invalid prop: INVALID LINE ENDINGS CASE 4", i);
	i++;
}

void __orderOfErrorsTests()
{
	VCardErrorCode err = OK;
	Card *tempCard;
	int i = 1;

	TS_header("INV FILE BEFORE INV CARD CASE", i);
	err = createCard(OOE_FILECASE1, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("INV FILE BEFORE INV CARD CASE", i);
	i++;

	TS_header("INV CARD BEFORE INV PROP CASE: errors: version is not 4.0 and it ends in CR and gender missing property value.", i);
	err = createCard(OOE_FILECASE2, &tempCard);
	assert(err == INV_CARD && tempCard == NULL);
	TS_footer("INV CARD BEFORE INV PROP CASE", i);
	i++;

	TS_header("INV PROP BEFORE INV CARD CASE: errors: parameter value missing and end tag missing.", i);
	err = createCard(OOE_FILECASE3, &tempCard);
	assert(err == INV_PROP && tempCard == NULL);
	TS_footer("INV PROP BEFORE INV CARD CASE", i);
	i++;
}

void __TLParamHandlingTests()
{
	int i = 1;
	VCardErrorCode err = OK;
	Card *tempCard;
	char *print;

	TS_header("createCard: NULL file", i);
	err = createCard(NULL, &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("createCard: NULL file", i);
	i++;

	TS_header("createCard: NULL address of a card pointer", i);
	err = createCard(OOE_FILECASE3, NULL);
	assert(err == OTHER_ERROR && tempCard == NULL);
	TS_footer("createCard: NULL address of a card pointer", i);
	i++;

	TS_header("createCard: NULL file and NULL address of a card pointer", i);
	err = createCard(NULL, NULL);
	assert(err == OTHER_ERROR && tempCard == NULL);
	TS_footer("createCard: NULL file NULL address of a card pointer", i);
	i++;

	TS_header("createCard: empty string passed in for file name parameter", i);
	err = createCard("", &tempCard);
	assert(err == INV_FILE && tempCard == NULL);
	TS_footer("createCard: NULL address of a card pointer", i);
	i++;

	TS_header("deleteCard: NULL card object", i);
	deleteCard(NULL);
	TS_footer("createCard: NULL card onject", i);
	i++;

	TS_header("deleteCard: uninitialized card object", i);
	deleteCard(tempCard);
	TS_footer("createCard: uninitialized card object", i);
	i++;

	TS_header("printCard: NULL card object", i);
	print = printCard(NULL);
	assert(print == NULL);
	TS_footer("printCard: NULL card object", i);
	i++;

	TS_header("printCard: uninitialized card object", i);
	print = printCard(tempCard);
	assert(print == NULL);
	TS_footer("printCard: uninitialized card object", i);
	i++;
}

void __customTests()
{
	if (CT_toBeTested)
	{
		VCardErrorCode err = OK;
		Card *tempCard;
		int i = 1;
		char *print = NULL;

		TS_header("Create card and Validate Card using a custom file 1", i);

		err = createCard(CT_FILECASE1, &tempCard);
		assert(err == CT_expectedErr);
		if (CT_CC_print && err == OK)
		{
			print = printCard(tempCard);
			printf("%s", print);
			free(print);
		}

		err = validateCard(tempCard);
		assert(err == CT_VALIDATE_FUNCTION_expectedErr);
		if (CT_VC_print && err == OK)
		{
			print = printCard(tempCard);
			printf("%s", print);
			free(print);
		}

		if (err == OK)
		{
			deleteCard(tempCard);
		}
		TS_footer("Create card and Validate Card using a custom file 1", i);
		i++;
	}
}

void __writeTests()
{
	VCardErrorCode err = OK;
	Card *tempCard;
	Card *tempCard2;
	int i = 1;

	TS_header("Create Card for File", i);
	err = createCard(WT_FILETOPARSE, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_footer("Create Card for File", i);
	i++;

	TS_header("Write Card to File", i);
	err = writeCard(WT_FILETOCREATE, tempCard);
	assert(err == OK);
	TS_footer("Write Card to File", i);
	i++;

	TS_header("Create Card for File that a card was written to", i);
	err = createCard(WT_FILETOCREATE, &tempCard2);
	assert(err == OK && tempCard2 != NULL);
	TS_footer("Create Card for File that a card was written to", i);
	i++;

	TS_header("Compare cards from parsed file and written file", i);
	bool areEqual = _tObjEqual(tempCard, tempCard2);
	assert(areEqual == true);
	TS_footer("Compare cards from parsed file and written file", i);

	TS_header("Delete card parsed from file", i);
	deleteCard(tempCard);
	TS_footer("Delete Card parsed from File", i);
	i++;

	TS_header("Delete card parsed from written file", i);
	deleteCard(tempCard2);
	TS_footer("Delete Card parsed from written File", i);
	i++;
}

void __validateCardTests()
{
	VCardErrorCode err = OK;
	Card *tempCard;
	int i = 1;

	err = createCard(VCT_FILECASE1, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with INVALID PROPERTY NAME", i);
	err = validateCard(tempCard);
	assert(err == INV_PROP);
	TS_footer("Validate Card with INVALID PROPERTY NAME", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE2, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with MULTIPLE VERSION TAGS", i);
	err = validateCard(tempCard);
	assert(err == INV_CARD);
	TS_footer("Validate Card with MULTIPLE VERSION TAGS", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE3, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with A PROP THAT HAS A INVALID CARDINALITY", i);
	err = validateCard(tempCard);
	assert(err == INV_PROP);
	TS_footer("Validate Card with A PROP THAT HAS A INVALID CARDINALITY", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE4, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with A PROP THAT HAS A INVALID NUM OF VALUES", i);
	err = validateCard(tempCard);
	assert(err == INV_PROP);
	TS_footer("Validate Card with A PROP THAT HAS A INVALID NUM OF VALUES", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE5, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with NO FN PROPERTY", i);
	deleteProperty(tempCard->fn);
	tempCard->fn = NULL;
	err = validateCard(tempCard);
	assert(err == INV_CARD);
	TS_footer("Validate Card with NO FN PROPERTY", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE6, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with A DIFF PROPERTY IN FN MEMBER", i);
	deleteProperty(tempCard->fn);
	tempCard->fn = NULL;
	Property *a = (Property *)tempCard->optionalProperties->head->data;
	tempCard->fn = a;
	err = validateCard(tempCard);
	assert(err == INV_PROP);
	tempCard->fn = NULL;
	TS_footer("Validate Card with A DIFF PROPERTY IN FN MEMBER", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE7, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with MULTIPLE BDAY", i);
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with MULTIPLE BDAY", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE8, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with PROPERTY VALUE AS NULL", i);
	Property *t1 = (Property *)tempCard->optionalProperties->head->data;
	char *b = (char *)t1->values->head->data;
	deleteValue((void *)b);
	t1->values->head->data = NULL;
	err = validateCard(tempCard);
	assert(err == INV_PROP);
	TS_footer("Validate Card with PROPERTY VALUE AS NULL", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE9, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with PARAMETER NAME AND/OR VALUE CONTAINS EMPTY STRING", i);
	Property *t2 = (Property *)tempCard->optionalProperties->head->data;
	Parameter *one = (Parameter *)t2->parameters->head->data;
	Parameter *two = (Parameter *)t2->parameters->tail->data;
	strcpy(one->name, "");
	strcpy(two->value, "");
	t2->parameters->head->data = (void *)one;
	t2->parameters->tail->data = (void *)two;
	err = validateCard(tempCard);
	assert(err == INV_PROP);
	TS_footer("Validate Card with PARAMETER NAME AND/OR VALUE CONTAINS EMPTY STRING", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE10, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with BDAY IS TYPE TEXT BUT DATE AND TIME MEMBERS ARE NOT EMPTY", i);
	strcpy(tempCard->birthday->date, "oh no!");
	strcpy(tempCard->birthday->time, "oh no!");
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with BDAY IS TYPE TEXT BUT DATE AND TIME MEMBERS ARE NOT EMPTY", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE11, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with BDAY IS TYPE DATETIME BUT TEXT IS NOT EMPTY", i);
	strcpy(tempCard->birthday->text, "oh no!");
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with BDAY IS TYPE DATETIME BUT TEXT IS NOT EMPTY", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE12, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with BDAY IS TYPE TEXT BUT IT IS UTC", i);
	tempCard->birthday->UTC = true;
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with BDAY IS TYPE TEXT BUT IT IS UTC", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE13, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with INVALID DATE FORMAT FOR BDAY -> INVALID MONTH", i);
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with INVALID DATE FORMAT FOR BDAY -> INVALID MONTH", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE14, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with INVALID TIME FORMAT FOR BDAY -> INVALID SECONDS", i);
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with INVALID TIME FORMAT FOR BDAY -> INVALID SECONDS", i);
	deleteCard(tempCard);
	i++;

	err = createCard(VCT_FILECASE15, &tempCard);
	assert(err == OK && tempCard != NULL);
	TS_header("Validate Card with INVALID DATE AND TIME FORMAT FOR BDAY -> INVALID FORMATS", i);
	err = validateCard(tempCard);
	assert(err == INV_DT);
	TS_footer("Validate Card with INVALID DATE AND TIME FORMAT FOR BDAY -> INVALID FORMATS", i);
	deleteCard(tempCard);
	i++;
}

void __strListJSONConversionTests()
{
	int i = 1;
	char *returnStr;
	List *tempList;
	List *returnList;

	tempList = NULL;
	TS_header("Convert list to JSON string - NULL list", i);
	returnStr = strListToJSON(tempList);
	assert(returnStr == NULL);
	TS_footer("Convert list to JSON string - NULL list", i);

	TS_header("Convert JSON string to list - NULL string", i);
	returnList = JSONtoStrList(NULL);
	assert(returnList == NULL);
	TS_footer("Convert JSON string to list - NULL string", i);

	TS_header("Convert JSON string to list - Empty JSON string", i);
	returnList = JSONtoStrList("[]");
	assert(returnList != NULL && returnList->length == 0);
	TS_footer("Convert JSON string to list - Empty JSON string", i);

	freeList(tempList);
	freeList(returnList);
	free(returnStr);
	i++;

	tempList = initializeList(&printValue, &deleteValue, &compareValues);
	TS_header("Convert list to JSON string - empty list", i);
	returnStr = strListToJSON(tempList);
	assert(returnStr != NULL && strcmp(returnStr, "[]") == 0);
	TS_footer("Convert list to JSON string - empty list", i);

	TS_header("Convert JSON string to list - []", i);
	returnList = JSONtoStrList(returnStr);
	assert(returnList != NULL && returnList->length == 0);
	TS_footer("Convert JSON string to list - []", i);

	freeList(tempList);
	freeList(returnList);
	free(returnStr);
	i++;

	char *val1 = calloc(strlen("Val1") + 1, sizeof(char));
	strcpy(val1, "Val1");
	char *val2 = calloc(strlen("Val2") + 1, sizeof(char));
	strcpy(val2, "Val2");
	char *val3 = calloc(strlen("Val3") + 1, sizeof(char));
	strcpy(val3, "Val3");

	tempList = initializeList(&printValue, &deleteValue, &compareValues);
	insertBack(tempList, (void *)val1);
	insertBack(tempList, (void *)val2);
	insertBack(tempList, (void *)val3);
	TS_header("Convert list to JSON string - populated list", i);
	returnStr = strListToJSON(tempList);
	assert(returnStr != NULL && strcmp(returnStr, "[\"Val1\",\"Val2\",\"Val3\"]") == 0);
	TS_footer("Convert list to JSON string - populated list", i);

	TS_header("Convert JSON string to list - populated values", i);
	returnList = JSONtoStrList(returnStr);
	assert(returnList != NULL && returnList->length == 3);
	TS_footer("Convert JSON string to list - populated values", i);

	freeList(returnList);
	freeList(tempList);
	free(returnStr);
	i++;
}

void __propertyJSONConversionTests()
{
	int i = 1;
	Property *a, *b, *c;
	char *data = NULL, *data2 = NULL;

	TS_header("Convert JSON to Property - NULL JSON STRING", i);
	a = JSONtoProp(NULL);
	assert(a == NULL);
	TS_footer("Convert JSON to Property - NULL JSON STRING", i);
	if (a)
		deleteProperty(a);
	i++;

	TS_header("Convert JSON to Property - EMPTY JSON STRING", i);
	a = JSONtoProp("");
	assert(a == NULL);
	TS_footer("Convert JSON to Property - EMPTY JSON STRING", i);
	if (a)
		deleteProperty(a);
	i++;

	TS_header("Convert JSON to Property - INVALID JSON STRING (wrong group tag)", i);
	a = JSONtoProp("{\"grouptagwrong\":\"prop group\",\"name\":\"prop name\",\"values\":[\"val1\",\"val2\"]}");
	assert(a == NULL);
	TS_footer("Convert JSON to Property - INVALID JSON STRING (wrong group tag)", i);
	if (a)
		deleteProperty(a);
	i++;

	TS_header("Convert JSON to Property - INVALID JSON STRING (space between colon and start of value)", i);
	a = JSONtoProp("{\"group\":\"prop group\",\"name\": \"prop name\",\"values\":[\"val1\",\"val2\"]}");
	assert(a == NULL);
	TS_footer("Convert JSON to Property - INVALID JSON STRING (space between colon and start of value)", i);
	if (a)
		deleteProperty(a);
	i++;

	TS_header("Convert JSON to Property - INVALID JSON STRING (too many tags)", i);
	a = JSONtoProp("{\"group\":\"prop group\",\"name\":\"prop name\",\"values\":[\"val1\",\"val2\"], \"group\":\"prop group\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to Property - INVALID JSON STRING (too many tags)", i);
	if (a)
		deleteProperty(a);
	i++;

	TS_header("Convert JSON to Property - INVALID JSON STRING (wrong order of tags)", i);
	a = JSONtoProp("{\"name\":\"prop name\",\"group\":\"prop group\",\"values\":[\"val1\",\"val2\"]}");
	assert(a == NULL);
	TS_footer("Convert JSON to Property - INVALID JSON STRING (wrong order of tags)", i);
	if (a)
		deleteProperty(a);
	i++;

	TS_header("Convert JSON to Property - VALID JSON STRING", i);
	a = JSONtoProp("{\"group\":\"\",\"name\":\"FN\",\"values\":[\"Simon Perreault\"]}");
	assert(a != NULL && strcmp(a->group, "") == 0 && strcmp(a->name, "FN") == 0 && a->parameters != NULL && a->parameters->length == 0);
	data = (char *)a->values->head->data;
	assert(strcmp(data, "Simon Perreault") == 0);
	TS_footer("Convert JSON to Property - VALID JSON STRING", i);
	i++;

	TS_header("Convert JSON to Property - VALID JSON STRING", i);
	b = JSONtoProp("{\"group\":\"Group1\",\"name\":\"N\",\"values\":[\"Simon Perreault\",\"Tito\"]}");
	assert(a != NULL && strcmp(b->group, "Group1") == 0 && strcmp(b->name, "N") == 0 && b->parameters != NULL && b->parameters->length == 0);
	data = (char *)b->values->head->data;
	data2 = (char *)b->values->tail->data;
	assert(strcmp(data, "Simon Perreault") == 0);
	assert(strcmp(data2, "Tito") == 0);
	TS_footer("Convert JSON to Property - VALID JSON STRING", i);
	i++;

	char *returnStr;

	TS_header("Convert Property to JSON - NULL PROPERTY", i);
	returnStr = propToJSON(NULL);
	assert(returnStr != NULL && strcmp(returnStr, "") == 0);
	free(returnStr);
	TS_footer("Convert Property to JSON - NULL PROPERTY", i);

	TS_header("Convert Property to JSON - INVALID PROPERTY (member NULL)", i);
	c = calloc(1, sizeof(Property));
	c->group = calloc(100, sizeof(char));
	c->name = calloc(100, sizeof(char));
	c->parameters = NULL;
	c->values = initializeList(printValue, deleteValue, &compareValues);
	returnStr = propToJSON(c);
	assert(returnStr != NULL && strcmp(returnStr, "") == 0);
	free(returnStr);
	TS_footer("Convert Property to JSON - NULL PROPERTY (member NULL)", i);

	TS_header("Convert Property to JSON - VALID PROPERTY", i);
	returnStr = propToJSON(a);
	assert(returnStr != NULL && strcmp(returnStr, "{\"group\":\"\",\"name\":\"FN\",\"values\":[\"Simon Perreault\"]}") == 0);
	free(returnStr);
	TS_footer("Convert Property to JSON - VALID PROPERTY", i);

	TS_header("Convert Property to JSON - VALID PROPERTY", i);
	returnStr = propToJSON(b);
	assert(returnStr != NULL && strcmp(returnStr, "{\"group\":\"Group1\",\"name\":\"N\",\"values\":[\"Simon Perreault\",\"Tito\"]}") == 0);
	free(returnStr);
	TS_footer("Convert Property to JSON - VALID PROPERTY", i);

	if (a)
		deleteProperty(a);
	if (b)
		deleteProperty(b);
	if (c)
		deleteProperty(c);
}

void __dtJSONConversionTests()
{
	int i = 1;
	DateTime *a, *b;

	TS_header("Convert JSON to DateTime - NULL JSON STRING", i);
	a = JSONtoDT(NULL);
	assert(a == NULL);
	TS_footer("Convert JSON to DateTime - NULL JSON STRING", i);
	if (a)
		deleteDate(a);
	i++;

	TS_header("Convert JSON to DateTime - EMPTY JSON STRING", i);
	a = JSONtoDT("");
	assert(a == NULL);
	TS_footer("Convert JSON to DateTime - EMPTY JSON STRING", i);
	if (a)
		deleteDate(a);
	i++;

	TS_header("Convert JSON to DateTime - INVALID JSON STRING (wrong date tag)", i);
	a = JSONtoDT("{\"isText\":\"true\",\"DATE\":\"\",\"time\":\"\",\"text\":\"circa 1960\",\"isUTC\":\"false\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to DateTime - INVALID JSON STRING (wrong date tag)", i);
	if (a)
		deleteDate(a);
	i++;

	TS_header("Convert JSON to DateTime - INVALID JSON STRING (space between colon and value)", i);
	a = JSONtoDT("{\"isText\":\"true\",\"date\":\"\",\"time\": \"\",\"text\":\"circa 1960\",\"isUTC\":\"false\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to DateTime - INVALID JSON STRING (space between colon and value)", i);
	if (a)
		deleteDate(a);
	i++;

	TS_header("Convert JSON to DateTime - INVALID JSON STRING (too many tags)", i);
	a = JSONtoDT("{\"isText\":\"true\",\"date\":\"\",\"time\":\"\",\"text\":\"circa 1960\",\"isUTC\":\"\",\"isUTC\":\"false\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to DateTime - INVALID JSON STRING (too many tags)", i);
	if (a)
		deleteDate(a);
	i++;

	TS_header("Convert JSON to DateTime - INVALID JSON STRING (wrong order of tags)", i);
	a = JSONtoDT("{\"date\":\"\",\"isText\":\"true\",\"time\":\"\",\"text\":\"circa 1960\",\"isUTC\":\"false\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to DateTime - INVALID JSON STRING (wrong order of tags)", i);
	if (a)
		deleteDate(a);
	i++;

	TS_header("Convert JSON to DateTime - VALID JSON STRING", i);
	a = JSONtoDT("{\"isText\":\"true\",\"date\":\"\",\"time\":\"\",\"text\":\"circa 1960\",\"isUTC\":\"false\"}");
	assert(a != NULL);
	assert(strcmp(a->date, "") == 0 && a->isText == true && strcmp(a->time, "") == 0 && strcmp(a->text, "circa 1960") == 0 && a->UTC == false);
	TS_footer("Convert JSON to DateTime - VALID JSON STRING", i);
	i++;

	TS_header("Convert JSON to DateTime - VALID JSON STRING", i);
	b = JSONtoDT("{\"isText\":false,\"date\":\"19540203\",\"time\":\"123012\",\"text\":\"\",\"isUTC\":true}");
	assert(b != NULL);
	assert(strcmp(b->date, "19540203") == 0 && b->isText == false && strcmp(b->time, "123012") == 0 && strcmp(b->text, "") == 0 && b->UTC == true);
	TS_footer("Convert JSON to DateTime - VALID JSON STRING", i);

	char *returnStr;

	TS_header("Convert DateTime to JSON - NULL DATETIME", i);
	returnStr = dtToJSON(NULL);
	assert(returnStr != NULL && strcmp(returnStr, "") == 0);
	free(returnStr);
	TS_footer("Convert DateTime to JSON - NULL DATETIME", i);
	i++;

	TS_header("Convert DateTime to JSON - VALID DATETIME", i);
	returnStr = dtToJSON(a);
	assert(returnStr != NULL && strcmp(returnStr, "{\"isText\":true,\"date\":\"\",\"time\":\"\",\"text\":\"circa 1960\",\"isUTC\":false}") == 0);
	free(returnStr);
	TS_footer("Convert DateTime to JSON - VALID DATETIME", i);
	i++;

	TS_header("Convert DateTime to JSON - VALID DATETIME", i);
	returnStr = dtToJSON(b);
	assert(returnStr != NULL && strcmp(returnStr, "{\"isText\":false,\"date\":\"19540203\",\"time\":\"123012\",\"text\":\"\",\"isUTC\":true}") == 0);
	free(returnStr);
	TS_footer("Convert DateTime to JSON - VALID DATETIME", i);
	i++;

	if (a)
		deleteDate(a);
	if (b)
		deleteDate(b);
}

void __cardJSONConversionTests()
{
	int i = 1;
	Card *a;

	TS_header("Convert JSON to Card - NULL JSON STRING", i);
	a = JSONtoCard(NULL);
	assert(a == NULL);
	TS_footer("Convert JSON to Card - NULL JSON STRING", i);
	if (a)
		deleteCard(a);
	i++;

	TS_header("Convert JSON to Card - EMPTY JSON STRING", i);
	a = JSONtoCard("");
	assert(a == NULL);
	TS_footer("Convert JSON to Card - EMPTY JSON STRING", i);
	if (a)
		deleteCard(a);
	i++;

	TS_header("Convert JSON to Card - INVALID JSON STRING (wrong FN tag)", i);
	a = JSONtoCard("{\"fn\":\"value\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to Card - INVALID JSON STRING (wrong FN tag)", i);
	if (a)
		deleteCard(a);
	i++;

	TS_header("Convert JSON to Card - INVALID JSON STRING (space between colon and value)", i);
	a = JSONtoCard("{\"fn\": \"value\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to Card - INVALID JSON STRING (space between colon and value)", i);
	if (a)
		deleteCard(a);
	i++;

	TS_header("Convert JSON to Card - INVALID JSON STRING (too many tags)", i);
	a = JSONtoCard("{\"fn\":\"value\",\"fn2\":\"value\"}");
	assert(a == NULL);
	TS_footer("Convert JSON to Card - INVALID JSON STRING (too many tags)", i);
	if (a)
		deleteCard(a);
	i++;

	TS_header("Convert JSON to Card - VALID JSON STRING", i);

	Property *fnProp = calloc(1, sizeof(Property));
	fnProp->group = calloc(strlen("") + 1, sizeof(char));
	strcpy(fnProp->group, "");
	fnProp->name = calloc(strlen("FN") + 1, sizeof(char));
	strcpy(fnProp->name, "FN");
	fnProp->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
	fnProp->values = initializeList(&printValue, &deleteValue, &compareValues);
	char *toADD = calloc(strlen("value2") + 1, sizeof(char));
	strncpy(toADD, "value2", strlen("value2"));
	insertBack(fnProp->values, (void *)toADD);

	a = JSONtoCard("{\"FN\":\"value2\"}");
	assert(a != NULL);
	assert(compareProperties((void *)a->fn, (void *)fnProp) == 0);
	TS_footer("Convert JSON to Card - VALID JSON STRING", i);

	if (a)
		deleteCard(a);
	if (fnProp)
		deleteProperty(fnProp);
	i++;
}

void __addPropTests()
{
	Card *a = calloc(1, sizeof(Card));
	a->birthday = NULL;
	a->anniversary = NULL;
	a->fn = NULL;
	a->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

	Property *fnProp = calloc(1, sizeof(Property));
	fnProp->group = calloc(strlen("") + 1, sizeof(char));
	strcpy(fnProp->group, "");
	fnProp->name = calloc(strlen("FN") + 1, sizeof(char));
	strcpy(fnProp->name, "FN");
	fnProp->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
	fnProp->values = initializeList(&printValue, &deleteValue, &compareValues);
	char *toADD = calloc(strlen("value2") + 1, sizeof(char));
	strncpy(toADD, "value2", strlen("value2"));
	insertBack(fnProp->values, (void *)toADD);

	int i = 1;

	TS_header("Add Property - NULL PARAMETERS", i);
	addProperty(a, NULL);
	addProperty(NULL, fnProp);
	assert(a->optionalProperties->length == 0);
	TS_footer("Add Property - NULL PARAMETERS", i);
	i++;

	TS_header("Add Property - VALID ADD", i);
	addProperty(a, fnProp);
	Property *toCmp = (Property *)a->optionalProperties->head->data;
	assert(a->optionalProperties->length == 1 && compareProperties((void *)toCmp, (void *)fnProp) == 0);
	TS_footer("Add Property - VALID ADD", i);

	deleteCard(a);
}

void TS_header(char *testName, int testNumber)
{
	printf(RESET "CASE %d: %s\n" RESET, testNumber, testName);
	return;
}

void TS_footer(char *testName, int testNumber)
{
	printf(GREEN "CASE %d: %s PASSED\n\n" RESET, testNumber, testName);
	return;
}