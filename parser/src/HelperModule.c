/**
 * @file HelperModule.c
 * @author Neel Gondalia (0940429)
 * @date September 2018
 * @brief File containing the function definitions of a VCard Parser Helper Module
 */

#include "HelperModule.h"

bool hModule__stringEndsWith(char *strToBeChecked, const char *suffix)
{
    if (strToBeChecked == NULL || suffix == NULL)
        return false;

    const int stringSize = strlen(strToBeChecked);
    const int suffixSize = strlen(suffix);

    if (suffixSize > stringSize)
        return false;

    int compareResults = strcmp(strToBeChecked + stringSize - suffixSize, suffix);
    if (compareResults != 0)
        return false;

    return true;
}

bool hModule__stringStartsWith(char *strToBeChecked, const char *prefix)
{
    if (strToBeChecked == NULL || prefix == NULL)
        return false;

    const int stringSize = strlen(strToBeChecked);
    const int prefixSize = strlen(prefix);

    if (prefixSize > stringSize)
        return false;

    int compareResults = strncmp(strToBeChecked, prefix, prefixSize);
    if (compareResults != 0)
        return false;

    return true;
}

VCardErrorCode hModule__parseFile(FILE *vcardfile, Card *toBeCreated)
{
    VCardErrorCode err = OK;

    if (vcardfile == NULL || toBeCreated == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Filename or card is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    fseek(vcardfile, 0, SEEK_END);
    size_t fileSize = ftell(vcardfile);
    fseek(vcardfile, 0, SEEK_SET);

    int charIter = 0;
    int _character = 0;
    char *temp = calloc(fileSize + 1, sizeof(char));
    if (temp == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        err = OTHER_ERROR;
        return err;
    }

    while ((_character = fgetc(vcardfile)) != EOF)
    {
        temp[charIter] = (char)_character;

        //Unfolding
        if (charIter > 1 && (temp[charIter] == ' ' || temp[charIter] == '\t') && temp[charIter - 1] == '\n' && temp[charIter - 2] == '\r')
        {
            charIter -= 2;
            continue;
        }
        charIter++;
    }
    temp[charIter] = '\0';

    //copying and freeing temp
    int sizeOfUnfoldedFile = strlen(temp);
    char *UnfoldedContentLines = calloc(sizeOfUnfoldedFile + 1, sizeof(char));
    if (UnfoldedContentLines == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        err = OTHER_ERROR;
        free(temp);
        return err;
    }
    strncpy(UnfoldedContentLines, temp, sizeOfUnfoldedFile);

    //passing a copy of the pointer so the original reference is not lost
    char *contentLines_pointercpy = UnfoldedContentLines;
    err = hModule__fileToVCard(contentLines_pointercpy, toBeCreated);

    //freeing all the file data stored
    contentLines_pointercpy = NULL;
    free(temp);
    free(UnfoldedContentLines);
    return err;
}

VCardErrorCode hModule__fileToVCard(char *contentLines, Card *toBeCreated)
{
    VCardErrorCode err = OK;

    if (contentLines == NULL || toBeCreated == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "contentLines or card object is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int contentLinesSize = strlen(contentLines);
    int iter = 0, lineNumber = 1;

    while (iter < contentLinesSize)
    {
        int i = 0;
        char *lineToParse = calloc(1000, sizeof(char));
        if (lineToParse == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }

        while (i < MAX_CONTENT_LINE_SIZE && iter < contentLinesSize && contentLines[iter] != '\r' && contentLines[iter] != '\n')
        {
            lineToParse[i] = contentLines[iter];
            iter++;
            i++;
        }

        while (i < MAX_CONTENT_LINE_SIZE && iter < contentLinesSize && (contentLines[iter] == '\r' || contentLines[iter] == '\n'))
        {
            lineToParse[i] = contentLines[iter];
            iter++;
            i++;
        }

        if (i == MAX_CONTENT_LINE_SIZE)
        {
            char tempErrorMssg[200] = {'\0'};
            sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file exceeds 998 characters.", lineNumber);

            hModule__printError(__FILE__, __LINE__, tempErrorMssg);
            free(lineToParse);
            err = INV_PROP;
            return err;
        }

        //Accounts for only one content line in the file which will result in an error so it should check for begin card instead of end card
        if (iter == contentLinesSize && lineNumber == 1)
        {
            err = hModule__analyzePropertyLine(lineToParse, toBeCreated, lineNumber);
            if (err != OK)
            {
                free(lineToParse);
                return err;
            }
        }
        else if (iter >= contentLinesSize && lineNumber > 1)
        {
            err = hModule__analyzePropertyLine(lineToParse, toBeCreated, -1); //-1 represents the last line
            if (err != OK)
            {
                free(lineToParse);
                return err;
            }

            //CASE: no fn property found before END:VCARD
            if (toBeCreated->fn == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "FN property not found in the card.");

                free(lineToParse);
                err = INV_CARD;
                return err;
            }
        }
        else
        {
            err = hModule__analyzePropertyLine(lineToParse, toBeCreated, lineNumber);
            if (err != OK)
            {
                free(lineToParse);
                return err;
            }
        }

        lineNumber++;
        free(lineToParse);
    }

    if (lineNumber < 4)
    {
        hModule__printError(__FILE__, __LINE__, "Invalid card, not enough content lines in the card.");
        err = INV_CARD;
        return err;
    }
    return err;
}

VCardErrorCode hModule__analyzePropertyLine(char *lineToParse, Card *toBeCreated, int lineNumber)
{
    VCardErrorCode err = OK;

    if (lineToParse == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Content Line to be parsed is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    if (lineNumber == -1)
    {
        hModule__upperString(lineToParse);
        if (!hModule__stringStartsWith(lineToParse, "END:VCARD\r") && !hModule__stringStartsWith(lineToParse, "END:VCARD\n"))
        {
            hModule__printError(__FILE__, __LINE__, "END:VCARD tag is not found in the card.");
            err = INV_CARD;
            return err;
        }
        if (!hModule__stringEndsWith(lineToParse, "\r\n") || hModule__stringEndsWith(lineToParse, "\r\r\n") || hModule__stringEndsWith(lineToParse, "\n\r\n") || strlen(lineToParse) > MAX_CONTENT_LINE_SIZE)
        {
            hModule__printError(__FILE__, __LINE__, "Invalid property, line ending is incorrect or content line exceeds 998 characters.");
            err = INV_PROP;
            return err;
        }
        return err;
    }
    //CASE: FIRST LINE SHOULD START WITH BEGIN:VCARD
    else if (lineNumber == 1)
    {
        hModule__upperString(lineToParse);
        if (!hModule__stringStartsWith(lineToParse, "BEGIN:VCARD\r") && !hModule__stringStartsWith(lineToParse, "BEGIN:VCARD\n"))
        {
            hModule__printError(__FILE__, __LINE__, "BEGIN:VCARD tag is not found in the card.");
            err = INV_CARD;
            return err;
        }
        if (!hModule__stringEndsWith(lineToParse, "\r\n") || hModule__stringEndsWith(lineToParse, "\r\r\n") || hModule__stringEndsWith(lineToParse, "\n\r\n") || strlen(lineToParse) > MAX_CONTENT_LINE_SIZE)
        {
            hModule__printError(__FILE__, __LINE__, "Invalid property, line ending is incorrect or content line exceeds 998 characters.");
            err = INV_PROP;
            return err;
        }
        return err;
    }
    //CASE: SECOND LINE SHOULD CONTAIN VERSION
    else if (lineNumber == 2)
    {
        hModule__upperString(lineToParse);
        if (!hModule__stringStartsWith(lineToParse, "VERSION:4.0\r") && !hModule__stringStartsWith(lineToParse, "VERSION:4.0\n"))
        {
            hModule__printError(__FILE__, __LINE__, "VERSION:4.0 tag is not found in the card.");
            err = INV_CARD;
            return err;
        }
        if (!hModule__stringEndsWith(lineToParse, "\r\n") || hModule__stringEndsWith(lineToParse, "\r\r\n") || hModule__stringEndsWith(lineToParse, "\n\r\n") || strlen(lineToParse) > MAX_CONTENT_LINE_SIZE)
        {
            hModule__printError(__FILE__, __LINE__, "Invalid property, line ending is incorrect or content line exceeds 998 characters.");
            err = INV_PROP;
            return err;
        }
        return err;
    }

    /*   NOT NEEDED IN A2
  //MULTIPLE BEGIN,and VERSION tag
    if (lineNumber > 2)
    {
        char *tempParseLine = calloc(strlen(lineToParse) + 1, sizeof(char));
        strncpy(tempParseLine, lineToParse, strlen(lineToParse));
        hModule__upperString(tempParseLine);

        if (hModule__stringStartsWith(tempParseLine, "VERSION:4.0\r") || hModule__stringStartsWith(tempParseLine, "VERSION:4.0\n"))
        {
            hModule__printError(__FILE__, __LINE__, "Another VERSION:4.0 tag is found in the card.");
            free(tempParseLine);
            err = INV_CARD;
            return err;
        }
        if (hModule__stringStartsWith(tempParseLine, "BEGIN:VCARD\r") || hModule__stringStartsWith(tempParseLine, "BEGIN:VCARD\n"))
        {
            hModule__printError(__FILE__, __LINE__, "Another BEGIN:VCARD tag is found in the card.");
            free(tempParseLine);
            err = INV_CARD;
            return err;
        }
        free(tempParseLine);
    }

    //Multiple END TAGS and/or end tag is not last
    if (lineNumber != -1)
    {
        char *tempParseLine = calloc(strlen(lineToParse) + 1, sizeof(char));
        strncpy(tempParseLine, lineToParse, strlen(lineToParse));
        hModule__upperString(tempParseLine);

        if (hModule__stringStartsWith(tempParseLine, "END:VCARD\r") || hModule__stringStartsWith(tempParseLine, "END:VCARD\n"))
        {
            hModule__printError(__FILE__, __LINE__, "multiple END:VCARD tags are found in the card or END:CARD is not at the end of file.");
            free(tempParseLine);
            err = INV_CARD;
            return err;
        }

        free(tempParseLine);
    } */

    if (!hModule__stringEndsWith(lineToParse, "\r\n") || hModule__stringEndsWith(lineToParse, "\r\r\n") || hModule__stringEndsWith(lineToParse, "\n\r\n") || strlen(lineToParse) > MAX_CONTENT_LINE_SIZE)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on content line %d of the file does not end with the correct line ending or exceeds the max length of a content line.", lineNumber);

        hModule__printError(__FILE__, __LINE__, tempErrorMssg);
        err = INV_PROP;
        return err;
    }

    int tlen = strlen(lineToParse);
    lineToParse[tlen - 1] = '\0';
    lineToParse[tlen - 2] = '\0';

    char *lineWithoutCRLF = calloc(tlen + 1, sizeof(char));
    if (lineWithoutCRLF == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        err = OTHER_ERROR;
        return err;
    }
    strcpy(lineWithoutCRLF, lineToParse);

    char *property_group = NULL;
    char *line_property = NULL;
    char *property_parameters = NULL;
    char *property_value = NULL;

    err = hModule__splitStringIntoPropertyParamsAndValues(lineWithoutCRLF, &line_property, &property_parameters, &property_value, &property_group, lineNumber);
    if (err != OK)
    {
        free(lineWithoutCRLF);
        free(property_group);
        free(line_property);
        free(property_parameters);
        free(property_value);
        return err;
    }

    err = hModule__fillCard(toBeCreated, line_property, property_parameters, property_value, property_group, lineNumber);

    free(lineWithoutCRLF);
    free(property_group);
    free(line_property);
    free(property_parameters);
    free(property_value);
    return err;
}

VCardErrorCode hModule__splitStringIntoPropertyParamsAndValues(char *lineToParse, char **line_property, char **property_parameters, char **property_value, char **property_group, int lineNumber)
{
    VCardErrorCode err = OK;

    if (lineToParse == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Content Line is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int lineLen = strlen(lineToParse);
    int i = 0;

    char *temp_preValue = calloc(lineLen + 1, sizeof(char));
    if (temp_preValue == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        err = OTHER_ERROR;
        return err;
    }
    char *temp_value = calloc(lineLen + 1, sizeof(char));
    if (temp_value == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        free(temp_preValue);
        err = OTHER_ERROR;
        return err;
    }

    //Splitting line to be parsed by colon to get value
    while (i < lineLen && lineToParse[i] != ':')
    {
        temp_preValue[i] = lineToParse[i];
        i++;
    }

    //CASE: NO COLON FOUND THEREFORE NO VALUE
    if (i == lineLen)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file does not contain a colon.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        free(temp_preValue);
        free(temp_value);
        err = INV_PROP;
        return err;
    }

    //CASE: COLON FOUND BUT NO VALUE
    if ((i + 1) == lineLen)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file does not have a property value.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        free(temp_preValue);
        free(temp_value);
        err = INV_PROP;
        return err;
    }

    //CASE: COLON FIRST CHAR THEREFORE NO PROPERTY NAME
    if (i == 0)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file does not have a property name.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        free(temp_preValue);
        free(temp_value);
        err = INV_PROP;
        return err;
    }

    i++;
    int j = 0;
    while (i < lineLen)
    {
        temp_value[j] = lineToParse[i];
        i++;
        j++;
    }

    //Splitting pre value string to get property and group and storing the rest as parameters
    i = 0;
    int preValueLen = strlen(temp_preValue);
    char *temp_preparameters = calloc(preValueLen + 1, sizeof(char));
    char *temp_parameters = calloc(preValueLen + 1, sizeof(char));
    char *temp_propName = calloc(preValueLen + 1, sizeof(char));
    char *temp_group = calloc(preValueLen + 1, sizeof(char));

    if (temp_preparameters == NULL || temp_parameters == NULL || temp_propName == NULL || temp_group == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        free(temp_preValue);
        free(temp_value);
        free(temp_preparameters);
        free(temp_propName);
        free(temp_parameters);
        free(temp_group);
        err = OTHER_ERROR;
        return err;
    }

    while (i < preValueLen && temp_preValue[i] != ';')
    {
        temp_preparameters[i] = temp_preValue[i];
        i++;
    }

    //CASE: NO PARAMTERS FOUND
    if (i == preValueLen)
    {
        strcpy(temp_parameters, "");
    }
    else
    {
        i++;
        int j = 0;

        //CASE: PROPNAME;:
        if (i == preValueLen)
        {
            char tempErrorMssg[200] = {'\0'};
            sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file suggests that there are parameters, but none found.", lineNumber);
            hModule__printError(__FILE__, __LINE__, tempErrorMssg);

            free(temp_preValue);
            free(temp_value);
            free(temp_preparameters);
            free(temp_propName);
            free(temp_parameters);
            free(temp_group);
            err = INV_PROP;
            return err;
        }
        while (i < preValueLen)
        {
            temp_parameters[j] = temp_preValue[i];
            j++;
            i++;
        }
    }

    int preParamLen = strlen(temp_preparameters);
    int g = 0;
    while (g < preParamLen && temp_preparameters[g] != '.')
    {
        temp_group[g] = temp_preparameters[g];
        g++;
    }

    if (g == preParamLen)
    {
        strcpy(temp_propName, temp_group);
        strcpy(temp_group, "");
    }
    else
    {
        g++;
        int k = 0;
        while (g < preParamLen)
        {
            temp_propName[k] = temp_preparameters[g];
            g++;
            k++;
        }
    }

    int groupLength = strlen(temp_group);
    int propertyLength = strlen(temp_propName);
    int valueLength = strlen(temp_value);
    int parametersLength = strlen(temp_parameters);

    if (propertyLength <= 0)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file does not contain a property name.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        free(temp_preValue);
        free(temp_value);
        free(temp_preparameters);
        free(temp_propName);
        free(temp_parameters);
        free(temp_group);
        err = INV_PROP;
        return err;
    }
    if (valueLength <= 0)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file does not contain a property value.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        free(temp_preValue);
        free(temp_value);
        free(temp_preparameters);
        free(temp_propName);
        free(temp_parameters);
        free(temp_group);
        err = INV_PROP;
        return err;
    }

    if (temp_group && strcmp(temp_group, "") != 0 && !hModule__isAlphaNumericOrDash(temp_group))
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid Property, Content line located on line %d of the file has an invalid group name.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        free(temp_preValue);
        free(temp_value);
        free(temp_preparameters);
        free(temp_propName);
        free(temp_parameters);
        free(temp_group);
        err = INV_PROP;
        return err;
    }

    *property_group = calloc(groupLength + 1, sizeof(char));
    *line_property = calloc(propertyLength + 1, sizeof(char));
    *property_value = calloc(valueLength + 1, sizeof(char));
    *property_parameters = calloc(parametersLength + 1, sizeof(char));

    if (*property_group == NULL || *line_property == NULL || *property_value == NULL || *property_parameters == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        free(temp_preValue);
        free(temp_value);
        free(temp_preparameters);
        free(temp_propName);
        free(temp_parameters);
        free(temp_group);
        err = OTHER_ERROR;
        return err;
    }

    strcpy(*property_group, temp_group);
    strcpy(*line_property, temp_propName);
    strcpy(*property_value, temp_value);
    strcpy(*property_parameters, temp_parameters);

    free(temp_preValue);
    free(temp_value);
    free(temp_preparameters);
    free(temp_propName);
    free(temp_parameters);
    free(temp_group);
    return err;
}

VCardErrorCode hModule__fillCard(Card *toBeCreated, char *_lproperty, char *_lparams, char *_lvalues, char *_lgroup, int lineNumber)
{
    VCardErrorCode err = OK;
    if (toBeCreated == NULL || _lproperty == NULL || _lvalues == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Property name, property values, or the card is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES] = {'\0'};
    char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS] = {'\0'};
    char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES] = {'\0'};

    err = hModule__splitPropertyValues(_lvalues, propValues, lineNumber);
    if (err != OK)
        return err;

    err = hModule__sortParamAndParamValues(_lparams, params, paramsValuePair, lineNumber);
    if (err != OK)
        return err;

    //copy property and upper case it
    int propLen = strlen(_lproperty);
    char temp_property[200] = {'\0'};
    strncpy(temp_property, _lproperty, propLen);
    hModule__upperString(temp_property);

    //NOT NEEDED FOR A1
    //err = hModule__isPropValid(temp_property);
    //if (err != NULL) return err;

    //CASE: FN
    if (strcmp(temp_property, "FN") == 0)
    {
        //CASE: first fn already parsed and this fn will be added to list of optional properties
        if (toBeCreated->fn != NULL)
        {
            if (toBeCreated->optionalProperties == NULL)
                toBeCreated->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

            Property *another_fn_property = calloc(1, sizeof(Property));
            if (another_fn_property == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
                err = OTHER_ERROR;
                return err;
            }

            err = hModule__populateProperty(another_fn_property, _lgroup, _lproperty, params, paramsValuePair, propValues, lineNumber);
            if (err != OK)
            {
                deleteProperty(another_fn_property);
                return err;
            }
            insertBack(toBeCreated->optionalProperties, (void *)another_fn_property);
        }
        else
        {
            toBeCreated->fn = calloc(1, sizeof(Property));
            if (toBeCreated->fn == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
                err = OTHER_ERROR;
                return err;
            }
            err = hModule__populateProperty(toBeCreated->fn, _lgroup, _lproperty, params, paramsValuePair, propValues, lineNumber);
            if (err != OK)
                return err;
        }
    }
    //Case: birthday
    else if (strcmp(temp_property, "BDAY") == 0)
    {
        if (toBeCreated->birthday != NULL)
        {
            if (toBeCreated->optionalProperties == NULL)
                toBeCreated->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

            Property *another_property = calloc(1, sizeof(Property));
            if (another_property == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
                err = OTHER_ERROR;
                return err;
            }

            err = hModule__populateProperty(another_property, _lgroup, _lproperty, params, paramsValuePair, propValues, lineNumber);
            if (err != OK)
            {
                deleteProperty(another_property);
                return err;
            }
            insertBack(toBeCreated->optionalProperties, (void *)another_property);
        }
        else
        {
            toBeCreated->birthday = calloc(1 + (strlen(propValues[0]) + 1), sizeof(DateTime));
            if (toBeCreated->birthday == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
                err = OTHER_ERROR;
                return err;
            }
            err = hModule__populateDateTimeStruct(toBeCreated->birthday, params, paramsValuePair, propValues, lineNumber);
            if (err != OK)
                return err;
        }
    }
    //Case: anniversary
    else if (strcmp(temp_property, "ANNIVERSARY") == 0)
    {
        if (toBeCreated->anniversary != NULL)
        {
            if (toBeCreated->optionalProperties == NULL)
                toBeCreated->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

            Property *another_property = calloc(1, sizeof(Property));
            if (another_property == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
                err = OTHER_ERROR;
                return err;
            }

            err = hModule__populateProperty(another_property, _lgroup, _lproperty, params, paramsValuePair, propValues, lineNumber);
            if (err != OK)
            {
                deleteProperty(another_property);
                return err;
            }
            insertBack(toBeCreated->optionalProperties, (void *)another_property);
        }
        else
        {

            toBeCreated->anniversary = calloc(1 + (strlen(propValues[0]) + 1), sizeof(DateTime));
            if (toBeCreated->anniversary == NULL)
            {
                hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
                err = OTHER_ERROR;
                return err;
            }
            err = hModule__populateDateTimeStruct(toBeCreated->anniversary, params, paramsValuePair, propValues, lineNumber);
            if (err != OK)
                return err;
        }
    }
    else
    {
        if (toBeCreated->optionalProperties == NULL)
            toBeCreated->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

        Property *another_property = calloc(1, sizeof(Property));
        if (another_property == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }

        err = hModule__populateProperty(another_property, _lgroup, _lproperty, params, paramsValuePair, propValues, lineNumber);
        if (err != OK)
        {
            deleteProperty(another_property);
            return err;
        }
        insertBack(toBeCreated->optionalProperties, (void *)another_property);
    }
    return err;
}

void hModule__upperString(char *str)
{
    if (str == NULL)
        return;

    int i = 0;

    for (i = 0; i < strlen(str); i++)
    {
        str[i] = toupper(str[i]);
    }
}

void hModule__lowerString(char *str)
{
    if (str == NULL)
        return;

    int i = 0;

    for (i = 0; i < strlen(str); i++)
    {
        str[i] = tolower(str[i]);
    }
}

VCardErrorCode hModule__isPropValid(char *_lprop)
{
    VCardErrorCode err = OK;

    if (_lprop == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Property name to be validated is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int numProps = 35;
    //TODO: find more info on iana-token and x-name
    char _AllowedProperties[35][20] = {"SOURCE", "KIND", "FN", "N", "NICKNAME", "PHOTO", "BDAY", "ANNIVERSARY", "GENDER", "ADR", "TEL", "EMAIL", "IMPP", "LANG", "TZ", "GEO", "TITLE", "ROLE", "LOGO", "ORG", "MEMBER", "RELATED", "CATEGORIES", "NOTE", "PRODID", "REV", "SOUND", "UID", "CLIENTPIDMAP", "URL", "KEY", "FBURL", "CALADRURI", "XML", "CALURI"};

    int i = 0;
    for (i = 0; i < numProps; i++)
    {
        if (strcmp(_lprop, _AllowedProperties[i]) == 0)
        {
            return err;
        }
    }
    err = INV_PROP;
    return err;
}

VCardErrorCode __isParamValid(char *_lparam)
{
    VCardErrorCode err = OK;

    if (_lparam == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Parameter name to be validated is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int numParams = 12;
    //TODO: find more info on iana-token and x-name
    char _AllowedParams[12][20] = {"LABEL", "LANGUAGE", "VALUE", "PREF", "ALTID", "PID", "TYPE", "MEDIATYPE", "CALSCALE", "SORT-AS", "GEO", "TZ"};

    int i = 0;
    for (i = 0; i < numParams; i++)
    {
        if (strcmp(_lparam, _AllowedParams[i]) == 0)
        {
            return err;
        }
    }
    err = INV_PROP;
    return err;
}

VCardErrorCode hModule__sortParamAndParamValues(char *_lparams, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES], int lineNumber)
{
    VCardErrorCode err = OK;

    if (_lparams == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "parameters string to be broken down is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int i = 0, j = 0, paramIter = 0;
    int paramLen = strlen(_lparams);

    for (i = 0; i < paramLen; i++)
    {
        bool hitEqualSign = false;
        j = 0;
        while (i < paramLen && _lparams[i] != ';')
        {
            if (_lparams[i] == '=')
            {
                hitEqualSign = true;
                i++;

                /*CASE: i is less than the length of paramters string and next char after = is ;, which means
                      there is no value for that particular parameter. Also if i is equal to length of parameters
                      string, it means that there is no value for the current paramter*/
                if ((i < paramLen && _lparams[i] == ';') || i == paramLen)
                {
                    char tempErrorMssg[200] = {'\0'};
                    sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file does not have a parameter value.", lineNumber);
                    hModule__printError(__FILE__, __LINE__, tempErrorMssg);

                    err = INV_PROP;
                    return err;
                }
                j = 0;
            }
            if (hitEqualSign)
            {
                paramsValuePair[paramIter][j] = _lparams[i];
                j++;
                i++;
            }
            else
            {
                params[paramIter][j] = _lparams[i];
                j++;
                i++;
            }
        }

        //Case: No parameter value was found since no equal sign was hit for the current paramter
        if (!hitEqualSign)
        {
            char tempErrorMssg[200] = {'\0'};
            sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file does not have a parameter value.", lineNumber);
            hModule__printError(__FILE__, __LINE__, tempErrorMssg);

            err = INV_PROP;
            return err;
        }
        paramIter++;
        if (paramIter >= MAX_STATIC_ROWS)
        {
            hModule__printError(__FILE__, __LINE__, "parameters count exceeded 20, which is the max allowed.");
            i = paramLen;
        }
    }
    return err;
}

VCardErrorCode hModule__splitPropertyValues(char *_lvalues, char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES], int lineNumber)
{
    VCardErrorCode err = OK;

    if (_lvalues == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "property values string to be broken down is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int valLen = strlen(_lvalues);
    int i = 0, j = 0, valNum = 0;

    for (i = 0; i < valLen; i++)
    {
        char *temp = calloc(valLen + 10, sizeof(char));
        if (temp == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }

        bool actualValueFound = false;
        j = 0;

        while (i < valLen && _lvalues[i] != ';')
        {
            //PROPERTY VALUES ESCAPE HANDLING FOR SEMICOLON, COMMA AND BACKSLASH
            // NOT NEEDED FOR A2: if (_lvalues[i] == '\\' && (i + 1) <= valLen && (_lvalues[i + 1] == ';' || _lvalues[i + 1] == ',' || _lvalues[i + 1] == '\\'))
            if (_lvalues[i] == '\\' && (i + 1) <= valLen && (_lvalues[i + 1] == ';'))
            {
                i++;
                temp[j] = _lvalues[i];
                i++;
                j++;
            }
            /* NOT NEEDED FOR A2
            else if (_lvalues[i] == '\\' && (i + 1) <= valLen && (_lvalues[i + 1] == 'n' || _lvalues[i + 1] == 'N'))
            {
                i++;
                temp[j] = '\n';
                j++;
                i++;
            }
            */
            else
            {
                actualValueFound = true;
                temp[j] = _lvalues[i];
                j++;
                i++;
            }
        }

        if (!actualValueFound)
            strcpy(propValues[valNum], "");
        else
            strcpy(propValues[valNum], temp);

        valNum++;
        free(temp);

        if ((valNum + 1) >= MAX_STATIC_ROWS)
        {
            hModule__printError(__FILE__, __LINE__, "Amount of property values are more than the amount allowed.");
            i = valLen;
        }
    }
    strcpy(propValues[valNum], "EOPV!!");
    return err;
}

VCardErrorCode hModule__populateProperty(Property *newProp, char *_lgroup, char *_lproperty, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES], char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES], int lineNumber)
{
    VCardErrorCode err = OK;

    if (_lproperty == NULL || newProp == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Property name or property to be populated is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    if (strcmp(_lproperty, "") == 0)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file has no property name.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        err = INV_PROP;
        return err;
    }

    int propLen = strlen(_lproperty);
    newProp->name = calloc(propLen + 1, sizeof(char));
    if (newProp->name == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        err = OTHER_ERROR;
        return err;
    }
    strcpy(newProp->name, _lproperty);

    //place group in property struct
    if (_lgroup != NULL)
    {
        int groupLen = strlen(_lgroup);
        newProp->group = calloc(groupLen + 1, sizeof(char));
        if (newProp->group == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }
        strcpy(newProp->group, _lgroup);
    }
    else
    {
        newProp->group = calloc(1, sizeof(char));
        if (newProp->group == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }
        strcpy(newProp->group, "");
    }

    newProp->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    err = hModule__addParamsToList(newProp->parameters, params, paramsValuePair);
    if (err != OK)
        return err;

    newProp->values = initializeList(&printValue, &deleteValue, &compareValues);
    err = hModule__addPropValuesToList(newProp->values, propValues);
    return err;
}

VCardErrorCode hModule__addParamsToList(List *list, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES])
{
    VCardErrorCode err = OK;

    if (list == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Property's parameters list is not initialized and is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int fnParamIter = 0;
    while (fnParamIter < MAX_STATIC_ROWS && strcmp(params[fnParamIter], "\0") != 0)
    {
        /* PARAM NAME VALIDATION NOT NEEDED FOR A1
        char *temp = calloc(strlen(params[fnParamIter]) + 1, sizeof(char));
        strcpy(temp, params[fnParamIter]);
        hModule__upperString(temp);
        err = __isParamValid(temp);
        if (err != OK)
        {
            free(temp);
            return err;
        }
        */

        Parameter *tempParameter = calloc(1 + (strlen(paramsValuePair[fnParamIter]) + 1), sizeof(Parameter));
        if (tempParameter == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }
        strncpy(tempParameter->name, params[fnParamIter], 200);
        strcpy(tempParameter->value, paramsValuePair[fnParamIter]);
        insertBack(list, (void *)tempParameter);
        fnParamIter++;

        // PARAM NAME VALIDATION NOT NEEDED FOR A1
        // free(temp);
    }
    return err;
}

VCardErrorCode hModule__addPropValuesToList(List *list, char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES])
{
    VCardErrorCode err = OK;

    if (list == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Property's values list is not initialized and is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    int fnValuesIter = 0;
    while (fnValuesIter < MAX_STATIC_ROWS && strcmp(propValues[fnValuesIter], "EOPV!!") != 0)
    {
        char *value = calloc(strlen(propValues[fnValuesIter]) + 1, sizeof(char));
        if (value == NULL)
        {
            hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
            err = OTHER_ERROR;
            return err;
        }
        strcpy(value, propValues[fnValuesIter]);
        insertBack(list, (void *)value);
        fnValuesIter++;
    }

    //CASE: Property needs to have one value
    if (getLength(list) <= 0)
        err = INV_PROP;
    return err;
}

VCardErrorCode hModule__populateDateTimeStruct(DateTime *dtStruct, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES], char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES], int lineNumber)
{
    VCardErrorCode err = OK;
    if (dtStruct == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "DateTime struct to be populated is NULL.");
        err = OTHER_ERROR;
        return err;
    }

    dtStruct->isText = false;
    dtStruct->UTC = false;

    //getting if value type is istext
    int i = 0;
    for (i = 0; i < MAX_STATIC_ROWS; i++)
    {
        char prm[MAX_STATIC_COLS_PARAMS] = {'\0'};
        strcpy(prm, params[i]);
        hModule__upperString(prm);
        if (strcmp(prm, "VALUE") == 0)
        {
            char prmVal[MAX_STATIC_COLS_PARAMVALUES] = {'\0'};
            strcpy(prmVal, paramsValuePair[i]);
            hModule__lowerString(prmVal);
            if (strcmp(prmVal, "text") == 0 || strcmp(prmVal, "\"text\"") == 0)
            {
                dtStruct->isText = true;
            }
        }
    }

    if (!propValues[0])
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file does not have a property value.", lineNumber);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);

        err = INV_PROP;
        return err;
    }

    //getting if time is UTC
    if (!dtStruct->isText && hModule__stringEndsWith(propValues[0], "Z"))
    {
        dtStruct->UTC = true;
    }

    //isText case
    if (dtStruct->isText)
    {
        strcpy(dtStruct->text, propValues[0]);
        strcpy(dtStruct->date, "");
        strcpy(dtStruct->time, "");
    }
    else
    {
        strcpy(dtStruct->text, "");

        int j = 0;
        int propValLen = strlen(propValues[0]);

        char tempTime[MAX_STATIC_COLS_PROPVALUES] = {'\0'};
        char tempDate[MAX_STATIC_COLS_PROPVALUES] = {'\0'};

        while (j < propValLen && propValues[0][j] != 'T')
        {
            tempDate[j] = propValues[0][j];
            j++;
        }

        //CASE: NO TIME FOUND
        if (j == propValLen)
        {
            /* NOT NEED FOR A2
            //INVALID DATE
            if (!hModule__isDateValid(tempDate))
            {
                char tempErrorMssg[200] = {'\0'};
                sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file has an invalid date format.", lineNumber);
                hModule__printError(__FILE__, __LINE__, tempErrorMssg);

                err = INV_PROP;
                return err;
            } */

            //VALID DATE
            strncpy(dtStruct->date, tempDate, 8);
            strcpy(dtStruct->time, "");
        }
        //CASE: TIME FOUND
        else
        {
            j++;
            int k = 0;

            //prefix dashes
            while (j < propValLen && propValues[0][j] == '-')
            {
                tempTime[k] = '-';
                j++;
                k++;
            }

            //next dash or Z means end of HHMMSS format. dash means utc offset while Z means UTC designator
            while (j < propValLen && propValues[0][j] != '-' && propValues[0][j] != 'Z')
            {
                tempTime[k] = propValues[0][j];
                j++;
                k++;
            }

            //CASE: NO DATE
            if (strlen(tempDate) == 0)
            {
                strcpy(dtStruct->date, "");
            }

            /* NOT NEEDED FOR A2 //CASE: INVALID DATE
            else if (!hModule__isDateValid(tempDate))
            {
                char tempErrorMssg[200] = {'\0'};
                sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file has an invalid date format.", lineNumber);
                hModule__printError(__FILE__, __LINE__, tempErrorMssg);

                err = INV_PROP;
                return err;
            }   */

            //CASE: VALID DATE
            else
            {
                strncpy(dtStruct->date, tempDate, 8);
            }

            /* NOT NEEDED FOR A2 //CASE: INVALID TIME !!!! NOT NEEDED FOR CREATE CARD FUNCTIONS
            if (!hModule__isTimeValid(tempTime))
            {
                char tempErrorMssg[200] = {'\0'};
                sprintf(tempErrorMssg, "Invalid property, Content line located on line %d of the file has an invalid time format.", lineNumber);
                hModule__printError(__FILE__, __LINE__, tempErrorMssg);

                err = INV_PROP;
                return err;
            } */
            //CASE: VALID TIME
            strncpy(dtStruct->time, tempTime, 6);
        }
    }
    return err;
}

bool hModule__isDateValid(char *tempDate)
{
    int dateLen = strlen(tempDate);
    if (!hModule__isNumericOrDash(tempDate) || dateLen < 4 || dateLen > 8)
        return false;

    //NO YEAR
    //3 CASES: --MM OR --MMDD OR ---DD
    if (hModule__stringStartsWith(tempDate, "--"))
    {
        //No Month
        //CASE ---DD
        if (tempDate[2] == '-')
        {
            if (dateLen != 5 || !isdigit(tempDate[3]) || !isdigit(tempDate[4]))
                return false;
            char datestr[2];
            datestr[0] = tempDate[3];
            datestr[1] = tempDate[4];
            datestr[2] = '\0';
            int date = atoi(datestr);
            if (date < 1 || date > 31)
                return false;
        }
        //Month
        //CASE --MM or --MMDD
        else
        {
            //checking month validity
            if (!isdigit(tempDate[2]) || !isdigit(tempDate[3]))
                return false;
            char monthstr[2];
            monthstr[0] = tempDate[2];
            monthstr[1] = tempDate[3];
            monthstr[2] = '\0';
            int month = atoi(monthstr);
            if (month < 1 || month > 12)
                return false;

            //date is optional so checking if its there first
            //case --MMDD
            if (dateLen != 4)
            {
                if (dateLen != 6)
                    return false;
                if (!isdigit(tempDate[4]) || !isdigit(tempDate[5]))
                    return false;
                char datestr[2];
                datestr[0] = tempDate[4];
                datestr[1] = tempDate[5];
                datestr[2] = '\0';
                int date = atoi(datestr);
                if (date < 1 || date > 31)
                    return false;
            }
        }
    }
    //YEAR
    //3 CASES: YYYY OR YYYY-MM OR YYYYMMDD
    else
    {
        //CHECK FOR 4 DIGITS
        if (!isdigit(tempDate[0]) || !isdigit(tempDate[1]) || !isdigit(tempDate[2]) || !isdigit(tempDate[3]))
            return false;

        //No date
        //CASE: YYYY-MM
        if (dateLen > 4 && tempDate[4] == '-')
        {
            if (dateLen != 7 || !isdigit(tempDate[5]) || !isdigit(tempDate[6]))
                return false;
            char monthstr[2];
            monthstr[0] = tempDate[5];
            monthstr[1] = tempDate[6];
            monthstr[2] = '\0';
            int month = atoi(monthstr);
            if (month < 1 || month > 12)
                return false;
        }
        //CASE: YYYY OR YYYYMMDD
        else
        {
            //CASE: YYYY
            if (dateLen == 4)
                return true;
            //CASE: YYYYMMDD
            if (dateLen == 8 && isdigit(tempDate[4]) && isdigit(tempDate[5]) && isdigit(tempDate[6]) && isdigit(tempDate[7]))
            {
                char monthstr[2];
                monthstr[0] = tempDate[4];
                monthstr[1] = tempDate[5];
                monthstr[2] = '\0';
                int month = atoi(monthstr);
                if (month < 1 || month > 12)
                    return false;

                char datestr[2];
                datestr[0] = tempDate[6];
                datestr[1] = tempDate[7];
                datestr[2] = '\0';
                int date = atoi(datestr);
                if (date < 1 || date > 31)
                    return false;
            }
            //CASE: INVALID
            else
            {
                return false;
            }
        }
    }
    return true;
}

bool hModule__isTimeValid(char *tempTime)
{
    //6 CASES: HH OR HHMM OR HHMMSS OR -MM OR -MMSS OR --SS
    int timeLen = strlen(tempTime);
    if (!hModule__isNumericOrDash(tempTime) || timeLen < 2 || timeLen > 6)
        return false;

    //NO HOUR
    //CASES: -MM -MMSS --SS
    if (tempTime[0] == '-')
    {
        //NO MINUTE --SS case
        if (tempTime[1] == '-')
        {
            if (timeLen != 4 || !isdigit(tempTime[2]) || !isdigit(tempTime[3]))
                return false;
            char secondstr[2];
            secondstr[0] = tempTime[2];
            secondstr[1] = tempTime[3];
            secondstr[2] = '\0';
            int second = atoi(secondstr);
            if (second < 0 || second > 59)
                return false;
        }
        //MINUTE case -MM or -MMSS
        else
        {
            if (timeLen < 3)
                return false;

            //case -MM
            if (timeLen == 3 && isdigit(tempTime[1]) && isdigit(tempTime[2]))
            {
                char minutestr[2];
                minutestr[0] = tempTime[1];
                minutestr[1] = tempTime[2];
                minutestr[2] = '\0';
                int minute = atoi(minutestr);
                if (minute < 0 || minute > 59)
                    return false;
            }
            //case -MMSS
            else if (timeLen == 5 && isdigit(tempTime[1]) && isdigit(tempTime[2]) && isdigit(tempTime[3]) && isdigit(tempTime[4]))
            {
                char minutestr[2];
                minutestr[0] = tempTime[1];
                minutestr[1] = tempTime[2];
                minutestr[2] = '\0';
                int minute = atoi(minutestr);
                if (minute < 0 || minute > 59)
                    return false;

                char secondstr[2];
                secondstr[0] = tempTime[3];
                secondstr[1] = tempTime[4];
                secondstr[2] = '\0';
                int second = atoi(secondstr);
                if (second < 0 || second > 59)
                    return false;
            }
            else
            {
                return false;
            }
        }
    }
    //HOUR
    //CASES: HH OR HHMM OR HHMMSS
    else
    {
        char hourstr[2];
        hourstr[0] = tempTime[0];
        hourstr[1] = tempTime[1];
        hourstr[2] = '\0';
        int hour = atoi(hourstr);
        if (hour < 0 || hour > 23)
            return false;

        if (timeLen > 2)
        {
            if (timeLen == 4 && isdigit(tempTime[2]) && isdigit(tempTime[3]))
            {
                char minutestr[2];
                minutestr[0] = tempTime[2];
                minutestr[1] = tempTime[3];
                minutestr[2] = '\0';
                int minute = atoi(minutestr);
                if (minute < 0 || minute > 59)
                    return false;
            }
            else if (timeLen == 6 && isdigit(tempTime[2]) && isdigit(tempTime[3]) && isdigit(tempTime[4]) && isdigit(tempTime[5]))
            {
                char minutestr[2];
                minutestr[0] = tempTime[2];
                minutestr[1] = tempTime[3];
                minutestr[2] = '\0';
                int minute = atoi(minutestr);
                if (minute < 0 || minute > 59)
                    return false;

                char secondstr[2];
                secondstr[0] = tempTime[4];
                secondstr[1] = tempTime[5];
                secondstr[2] = '\0';
                int second = atoi(secondstr);
                if (second < 0 || second > 59)
                    return false;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

void hModule__printError(char *fileName, int lineNum, char *message)
{
    if (fileName != NULL && message != NULL && DEBUGERROR)
        printf(RED "[%s: %d] : %s" RESET "\n", fileName, lineNum, message);
}

bool hModule__isAlphaNumericOrDash(char *group)
{
    if (group == NULL)
        return false;

    int i = 0;
    int groupLen = strlen(group);

    for (i = 0; i < groupLen; i++)
    {
        if (!isalpha(group[i]) && !isdigit(group[i]) && group[i] != '-')
        {
            return false;
        }
    }
    return true;
}

bool hModule__isNumericOrDash(char *datetimestr)
{
    if (datetimestr == NULL)
        return false;

    int i = 0;
    int groupLen = strlen(datetimestr);

    for (i = 0; i < groupLen; i++)
    {
        if (!isdigit(datetimestr[i]) && datetimestr[i] != '-')
        {
            return false;
        }
    }
    return true;
}

VCardErrorCode hModuleV2_getPropertyLineFromCard(Property *prop, char **str)
{
    VCardErrorCode err = OK;
    *str = NULL;

    if (prop == NULL)
        return OK;

    char *tempStr = calloc(2000, sizeof(char));
    if (!tempStr)
        return OTHER_ERROR;
    strcpy(tempStr, "");

    //GROUP
    if (prop->group && strcmp(prop->group, "") != 0)
    {
        strncat(tempStr, prop->group, strlen(prop->group));
        strcat(tempStr, ".");
    }

    //NAME
    if (prop->name)
        strncat(tempStr, prop->name, strlen(prop->name));

    //PARAMETERS
    if (prop->parameters && prop->parameters->length > 0)
    {
        strcat(tempStr, ";");
        int i = 0;
        Node *current = prop->parameters->head;
        for (i = 0; i < prop->parameters->length; i++)
        {
            Parameter *temp = (Parameter *)current->data;
            strncat(tempStr, temp->name, strlen(temp->name));
            strcat(tempStr, "=");
            strncat(tempStr, temp->value, strlen(temp->value));
            if ((i + 1) != prop->parameters->length)
                strcat(tempStr, ";");
            current = current->next;
        }
    }

    //PROPERTY VALUES
    if (prop->values && prop->values->length > 0)
    {
        strcat(tempStr, ":");
        int i = 0;
        Node *current = prop->values->head;
        for (i = 0; i < prop->values->length; i++)
        {
            char *temp = (char *)current->data;

            char *postEsc = NULL;
            hModuleV2__addEscapeChar(temp, &postEsc);
            if (postEsc)
                strncat(tempStr, postEsc, strlen(postEsc));
            free(postEsc);

            if ((i + 1) != prop->values->length)
                strcat(tempStr, ";");
            current = current->next;
        }
    }

    //LINE ENDING
    strcat(tempStr, "\r\n");

    //ONLY POSSIBLE ERROR WHILE WRITING SINCE CARD GETS VALIDATED BEFORE
    int tempStrLen = strlen(tempStr);
    if (tempStrLen > 998)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "WRITE ERROR: Content Line exceeding 998 characters found while trying to write to the file. Property of the content line is %s.", prop->name);
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);
        free(tempStr);
        return WRITE_ERROR;
    }

    *str = calloc(tempStrLen + 1, sizeof(char));
    strncpy(*str, tempStr, tempStrLen);
    free(tempStr);
    return err;
}

VCardErrorCode hModuleV2_getdateTimePropertyLineFromCard(DateTime *dt, char **str, int bdayOrAnniv)
{
    VCardErrorCode err = OK;
    *str = NULL;

    if (dt == NULL)
        return OK;

    char *tempStr = calloc(2000, sizeof(char));
    if (!tempStr)
        return OTHER_ERROR;
    strcpy(tempStr, "");

    if (bdayOrAnniv == 0)
        strncat(tempStr, "BDAY", strlen("BDAY"));
    else if (bdayOrAnniv == 1)
        strncat(tempStr, "ANNIVERSARY", strlen("ANNIVERSARY"));
    else
    {
        free(tempStr);
        return WRITE_ERROR;
    }

    if (dt->isText)
    {
        strncat(tempStr, ";VALUE=text:", strlen(";VALUE=text:"));
        char *temp = NULL;
        hModuleV2__addEscapeChar(dt->text, &temp);
        if (temp)
            strncat(tempStr, temp, strlen(temp));
        free(temp);
    }
    else
    {
        strcat(tempStr, ":");
        if (strcmp(dt->date, "") != 0)
            strncat(tempStr, dt->date, strlen(dt->date));
        if (strcmp(dt->time, "") != 0)
        {
            strcat(tempStr, "T");
            strncat(tempStr, dt->time, strlen(dt->time));
        }
        if (dt->UTC)
            strcat(tempStr, "Z");
    }

    strcat(tempStr, "\r\n");
    int tempStrLen = strlen(tempStr);
    if (tempStrLen > 998)
    {
        char tempErrorMssg[200] = {'\0'};
        sprintf(tempErrorMssg, "WRITE ERROR: Content Line for BDAY OR ANNIV, exceeding 998 characters found while trying to write to the file.");
        hModule__printError(__FILE__, __LINE__, tempErrorMssg);
        free(tempStr);
        return WRITE_ERROR;
    }

    *str = calloc(tempStrLen + 1, sizeof(char));
    strncpy(*str, tempStr, tempStrLen);
    free(tempStr);
    return err;
}

void hModuleV2__addEscapeChar(char *str, char **postEscStr)
{
    *postEscStr = NULL;
    if (str == NULL)
        return;

    char *temp = calloc(2000, sizeof(char));
    if (!temp)
        return;

    int len = strlen(str);
    int i = 0, j = 0;

    for (i = 0; i < len; i++)
    {

        //NOT NEEDED FOR A2: if (str[i] == '\\' || str[i] == ',' || str[i] == ';')
        if (str[i] == ';')
        {
            temp[j] = '\\';
            j++;
        }

        /* NOT NEEDED FOR A2
        if (str[i] == '\n')
        {
            temp[j] = '\\';
            j++;
            temp[j] = 'n';
            j++;
            continue;
        }
        */

        temp[j] = str[i];
        j++;
    }

    *postEscStr = calloc(strlen(temp) + 1, sizeof(char));
    strcpy(*postEscStr, "");
    strncat(*postEscStr, temp, strlen(temp));
    free(temp);
    return;
}

void hModuleV2__deletePropDetails(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
        return;

    propDetails *temp = (propDetails *)toBeDeleted;
    free(temp);
    return;
}

int hModuleV2__comparePropDetails(const void *first, const void *second)
{
    if (first == NULL && second == NULL)
        return 0;
    if (first == NULL && second != NULL)
        return -1;
    if (first != NULL && second == NULL)
        return 1;

    propDetails *a = (propDetails *)first;
    propDetails *b = (propDetails *)second;

    int aNameLen = strlen(a->name);
    int bNameLen = strlen(b->name);
    if (aNameLen == bNameLen && strncmp(a->name, b->name, aNameLen) == 0)
        return 0;
    if (bNameLen > aNameLen)
        return -1;
    return 1;
}

char *hModuleV2__printPropDetails(void *toBePrinted)
{
    if (toBePrinted == NULL)
        return NULL;

    propDetails *a = (propDetails *)toBePrinted;
    int nameLen = strlen(a->name);

    char *temp = calloc(nameLen + 1, sizeof(char));
    if (!temp)
        return NULL;
    strncpy(temp, a->name, nameLen);
    return temp;
}

VCardErrorCode hModuleV2__initializeAllPropertiesDetails(List *list)
{
    if (list == NULL)
        return OTHER_ERROR;

    //CARDINALITY OF * --->  MININSTANCES 0 MAXINSTANCES MAXINTEGER
    //CARDINALITY OF *1 --->  MININSTANCES 0 MAXINSTANCES 1
    //CARDINALITY OF 1* --->  MININSTANCES 1 MAXINSTANCES MAXINTEGER
    //CARDINALITY OF 1 --->  MININSTANCES 1 MAXINSTANCES 1

    //GENERAL PROPERTIES
    propDetails *one = calloc(1, sizeof(propDetails));
    if (!one)
        return OTHER_ERROR;
    strcpy(one->name, "BEGIN");
    one->strictNumValues = true;
    one->propertyValuesRequired = 1;
    one->minInstancesInCard = 1;
    one->maxInstancesInCard = 1;
    one->inCardCounter = 1;
    insertBack(list, (void *)one);

    propDetails *two = calloc(1, sizeof(propDetails));
    if (!two)
        return OTHER_ERROR;
    strcpy(two->name, "END");
    two->strictNumValues = true;
    two->propertyValuesRequired = 1;
    two->minInstancesInCard = 1;
    two->maxInstancesInCard = 1;
    two->inCardCounter = 1;
    insertBack(list, (void *)two);

    propDetails *three = calloc(1, sizeof(propDetails));
    if (!three)
        return OTHER_ERROR;
    strcpy(three->name, "SOURCE");
    three->strictNumValues = false;
    three->propertyValuesRequired = MAX_INTEGER;
    three->minInstancesInCard = 0;
    three->maxInstancesInCard = MAX_INTEGER;
    three->inCardCounter = 0;
    insertBack(list, (void *)three);

    propDetails *four = calloc(1, sizeof(propDetails));
    if (!four)
        return OTHER_ERROR;
    strcpy(four->name, "KIND");
    four->strictNumValues = true;
    four->propertyValuesRequired = 1;
    four->minInstancesInCard = 0;
    four->maxInstancesInCard = 1;
    four->inCardCounter = 0;
    insertBack(list, (void *)four);

    propDetails *five = calloc(1, sizeof(propDetails));
    if (!five)
        return OTHER_ERROR;
    strcpy(five->name, "XML");
    five->strictNumValues = true;
    five->propertyValuesRequired = 1;
    five->minInstancesInCard = 0;
    five->maxInstancesInCard = MAX_INTEGER;
    five->inCardCounter = 0;
    insertBack(list, (void *)five);

    //IDENTIFICATION PROPERTIES
    propDetails *ione = calloc(1, sizeof(propDetails));
    if (!ione)
        return OTHER_ERROR;
    strcpy(ione->name, "FN");
    ione->strictNumValues = true;
    ione->propertyValuesRequired = 1;
    ione->minInstancesInCard = 1;
    ione->maxInstancesInCard = MAX_INTEGER;
    ione->inCardCounter = 0;
    insertBack(list, (void *)ione);

    propDetails *itwo = calloc(1, sizeof(propDetails));
    if (!itwo)
        return OTHER_ERROR;
    strcpy(itwo->name, "N");
    itwo->strictNumValues = true;
    itwo->propertyValuesRequired = 5;
    itwo->minInstancesInCard = 0;
    itwo->maxInstancesInCard = 1;
    itwo->inCardCounter = 0;
    insertBack(list, (void *)itwo);

    propDetails *ithree = calloc(1, sizeof(propDetails));
    if (!ithree)
        return OTHER_ERROR;
    strcpy(ithree->name, "NICKNAME");
    ithree->strictNumValues = true;
    ithree->propertyValuesRequired = 1;
    ithree->minInstancesInCard = 0;
    ithree->maxInstancesInCard = MAX_INTEGER;
    ithree->inCardCounter = 0;
    insertBack(list, (void *)ithree);

    propDetails *ifour = calloc(1, sizeof(propDetails));
    if (!ifour)
        return OTHER_ERROR;
    strcpy(ifour->name, "PHOTO");
    ifour->strictNumValues = false;
    ifour->propertyValuesRequired = MAX_INTEGER;
    ifour->minInstancesInCard = 0;
    ifour->maxInstancesInCard = MAX_INTEGER;
    ifour->inCardCounter = 0;
    insertBack(list, (void *)ifour);

    propDetails *ifive = calloc(1, sizeof(propDetails));
    if (!ifive)
        return OTHER_ERROR;
    strcpy(ifive->name, "BDAY");
    ifive->strictNumValues = true;
    ifive->propertyValuesRequired = 1;
    ifive->minInstancesInCard = 0;
    ifive->maxInstancesInCard = 1;
    ifive->inCardCounter = 0;
    insertBack(list, (void *)ifive);

    propDetails *isix = calloc(1, sizeof(propDetails));
    if (!isix)
        return OTHER_ERROR;
    strcpy(isix->name, "ANNIVERSARY");
    isix->strictNumValues = true;
    isix->propertyValuesRequired = 1;
    isix->minInstancesInCard = 0;
    isix->maxInstancesInCard = 1;
    isix->inCardCounter = 0;
    insertBack(list, (void *)isix);

    propDetails *iseven = calloc(1, sizeof(propDetails));
    if (!iseven)
        return OTHER_ERROR;
    strcpy(iseven->name, "GENDER");
    iseven->strictNumValues = false;
    iseven->propertyValuesRequired = 2;
    iseven->minInstancesInCard = 0;
    iseven->maxInstancesInCard = 1;
    iseven->inCardCounter = 0;
    insertBack(list, (void *)iseven);

    //Delivery Addressing Properties
    propDetails *daone = calloc(1, sizeof(propDetails));
    if (!daone)
        return OTHER_ERROR;
    strcpy(daone->name, "ADR");
    daone->strictNumValues = true;
    daone->propertyValuesRequired = 7;
    daone->minInstancesInCard = 0;
    daone->maxInstancesInCard = MAX_INTEGER;
    daone->inCardCounter = 0;
    insertBack(list, (void *)daone);

    //Communications Properties
    propDetails *cone = calloc(1, sizeof(propDetails));
    if (!cone)
        return OTHER_ERROR;
    strcpy(cone->name, "TEL");
    cone->strictNumValues = false;
    cone->propertyValuesRequired = 28;
    cone->minInstancesInCard = 0;
    cone->maxInstancesInCard = MAX_INTEGER;
    cone->inCardCounter = 0;
    insertBack(list, (void *)cone);

    propDetails *ctwo = calloc(1, sizeof(propDetails));
    if (!ctwo)
        return OTHER_ERROR;
    strcpy(ctwo->name, "EMAIL");
    ctwo->strictNumValues = true;
    ctwo->propertyValuesRequired = 1;
    ctwo->minInstancesInCard = 0;
    ctwo->maxInstancesInCard = MAX_INTEGER;
    ctwo->inCardCounter = 0;
    insertBack(list, (void *)ctwo);

    propDetails *cthree = calloc(1, sizeof(propDetails));
    if (!cthree)
        return OTHER_ERROR;
    strcpy(cthree->name, "IMPP");
    cthree->strictNumValues = false;
    cthree->propertyValuesRequired = MAX_INTEGER;
    cthree->minInstancesInCard = 0;
    cthree->maxInstancesInCard = MAX_INTEGER;
    cthree->inCardCounter = 0;
    insertBack(list, (void *)cthree);

    propDetails *cfour = calloc(1, sizeof(propDetails));
    if (!cfour)
        return OTHER_ERROR;
    strcpy(cfour->name, "LANG");
    cfour->strictNumValues = true;
    cfour->propertyValuesRequired = 1;
    cfour->minInstancesInCard = 0;
    cfour->maxInstancesInCard = MAX_INTEGER;
    cfour->inCardCounter = 0;
    insertBack(list, (void *)cfour);

    //Geographical Properties
    propDetails *gone = calloc(1, sizeof(propDetails));
    if (!gone)
        return OTHER_ERROR;
    strcpy(gone->name, "TZ");
    gone->strictNumValues = true;
    gone->propertyValuesRequired = 1;
    gone->minInstancesInCard = 0;
    gone->maxInstancesInCard = MAX_INTEGER;
    gone->inCardCounter = 0;
    insertBack(list, (void *)gone);

    propDetails *gtwo = calloc(1, sizeof(propDetails));
    if (!gtwo)
        return OTHER_ERROR;
    strcpy(gtwo->name, "GEO");
    gtwo->strictNumValues = false;
    gtwo->propertyValuesRequired = MAX_INTEGER;
    gtwo->minInstancesInCard = 0;
    gtwo->maxInstancesInCard = MAX_INTEGER;
    gtwo->inCardCounter = 0;
    insertBack(list, (void *)gtwo);

    //Organizational Properties
    propDetails *oone = calloc(1, sizeof(propDetails));
    if (!oone)
        return OTHER_ERROR;
    strcpy(oone->name, "TITLE");
    oone->strictNumValues = true;
    oone->propertyValuesRequired = 1;
    oone->minInstancesInCard = 0;
    oone->maxInstancesInCard = MAX_INTEGER;
    oone->inCardCounter = 0;
    insertBack(list, (void *)oone);

    propDetails *otwo = calloc(1, sizeof(propDetails));
    if (!otwo)
        return OTHER_ERROR;
    strcpy(otwo->name, "ROLE");
    otwo->strictNumValues = true;
    otwo->propertyValuesRequired = 1;
    otwo->minInstancesInCard = 0;
    otwo->maxInstancesInCard = MAX_INTEGER;
    otwo->inCardCounter = 0;
    insertBack(list, (void *)otwo);

    propDetails *othree = calloc(1, sizeof(propDetails));
    if (!othree)
        return OTHER_ERROR;
    strcpy(othree->name, "LOGO");
    othree->strictNumValues = false;
    othree->propertyValuesRequired = MAX_INTEGER;
    othree->minInstancesInCard = 0;
    othree->maxInstancesInCard = MAX_INTEGER;
    othree->inCardCounter = 0;
    insertBack(list, (void *)othree);

    propDetails *ofour = calloc(1, sizeof(propDetails));
    if (!ofour)
        return OTHER_ERROR;
    strcpy(ofour->name, "ORG");
    ofour->strictNumValues = false;
    ofour->propertyValuesRequired = MAX_INTEGER;
    ofour->minInstancesInCard = 0;
    ofour->maxInstancesInCard = MAX_INTEGER;
    ofour->inCardCounter = 0;
    insertBack(list, (void *)ofour);

    propDetails *ofive = calloc(1, sizeof(propDetails));
    if (!ofive)
        return OTHER_ERROR;
    strcpy(ofive->name, "MEMBER");
    ofive->strictNumValues = false;
    ofive->propertyValuesRequired = MAX_INTEGER;
    ofive->minInstancesInCard = 0;
    ofive->maxInstancesInCard = MAX_INTEGER;
    ofive->inCardCounter = 0;
    insertBack(list, (void *)ofive);

    propDetails *osix = calloc(1, sizeof(propDetails));
    if (!osix)
        return OTHER_ERROR;
    strcpy(osix->name, "RELATED");
    osix->strictNumValues = false;
    osix->propertyValuesRequired = MAX_INTEGER;
    osix->minInstancesInCard = 0;
    osix->maxInstancesInCard = MAX_INTEGER;
    osix->inCardCounter = 0;
    insertBack(list, (void *)osix);

    //EXPLANATORY PROPERTIES
    propDetails *eone = calloc(1, sizeof(propDetails));
    if (!eone)
        return OTHER_ERROR;
    strcpy(eone->name, "CATEGORIES");
    eone->strictNumValues = true;
    eone->propertyValuesRequired = 1;
    eone->minInstancesInCard = 0;
    eone->maxInstancesInCard = MAX_INTEGER;
    eone->inCardCounter = 0;
    insertBack(list, (void *)eone);

    propDetails *etwo = calloc(1, sizeof(propDetails));
    if (!etwo)
        return OTHER_ERROR;
    strcpy(etwo->name, "NOTE");
    etwo->strictNumValues = true;
    etwo->propertyValuesRequired = 1;
    etwo->minInstancesInCard = 0;
    etwo->maxInstancesInCard = MAX_INTEGER;
    etwo->inCardCounter = 0;
    insertBack(list, (void *)etwo);

    propDetails *ethree = calloc(1, sizeof(propDetails));
    if (!ethree)
        return OTHER_ERROR;
    strcpy(ethree->name, "PRODID");
    ethree->strictNumValues = true;
    ethree->propertyValuesRequired = 1;
    ethree->minInstancesInCard = 0;
    ethree->maxInstancesInCard = 1;
    ethree->inCardCounter = 0;
    insertBack(list, (void *)ethree);

    propDetails *efour = calloc(1, sizeof(propDetails));
    if (!efour)
        return OTHER_ERROR;
    strcpy(efour->name, "REV");
    efour->strictNumValues = true;
    efour->propertyValuesRequired = 1;
    efour->minInstancesInCard = 0;
    efour->maxInstancesInCard = 1;
    efour->inCardCounter = 0;
    insertBack(list, (void *)efour);

    propDetails *efive = calloc(1, sizeof(propDetails));
    if (!efive)
        return OTHER_ERROR;
    strcpy(efive->name, "SOUND");
    efive->strictNumValues = false;
    efive->propertyValuesRequired = MAX_INTEGER;
    efive->minInstancesInCard = 0;
    efive->maxInstancesInCard = MAX_INTEGER;
    efive->inCardCounter = 0;
    insertBack(list, (void *)efive);

    propDetails *esix = calloc(1, sizeof(propDetails));
    if (!esix)
        return OTHER_ERROR;
    strcpy(esix->name, "UID");
    esix->strictNumValues = false;
    esix->propertyValuesRequired = MAX_INTEGER;
    esix->minInstancesInCard = 0;
    esix->maxInstancesInCard = 1;
    esix->inCardCounter = 0;
    insertBack(list, (void *)esix);

    propDetails *eseven = calloc(1, sizeof(propDetails));
    if (!eseven)
        return OTHER_ERROR;
    strcpy(eseven->name, "CLIENTPIDMAP");
    eseven->strictNumValues = true;
    eseven->propertyValuesRequired = 2;
    eseven->minInstancesInCard = 0;
    eseven->maxInstancesInCard = MAX_INTEGER;
    eseven->inCardCounter = 0;
    insertBack(list, (void *)eseven);

    propDetails *eeight = calloc(1, sizeof(propDetails));
    if (!eeight)
        return OTHER_ERROR;
    strcpy(eeight->name, "URL");
    eeight->strictNumValues = false;
    eeight->propertyValuesRequired = MAX_INTEGER;
    eeight->minInstancesInCard = 0;
    eeight->maxInstancesInCard = MAX_INTEGER;
    eeight->inCardCounter = 0;
    insertBack(list, (void *)eeight);

    propDetails *enine = calloc(1, sizeof(propDetails));
    if (!enine)
        return OTHER_ERROR;
    strcpy(enine->name, "VERSION");
    enine->strictNumValues = true;
    enine->propertyValuesRequired = 1;
    enine->minInstancesInCard = 1;
    enine->maxInstancesInCard = 1;
    enine->inCardCounter = 1;
    insertBack(list, (void *)enine);

    //Security Properties
    propDetails *sone = calloc(1, sizeof(propDetails));
    if (!sone)
        return OTHER_ERROR;
    strcpy(sone->name, "KEY");
    sone->strictNumValues = false;
    sone->propertyValuesRequired = MAX_INTEGER;
    sone->minInstancesInCard = 0;
    sone->maxInstancesInCard = MAX_INTEGER;
    sone->inCardCounter = 0;
    insertBack(list, (void *)sone);

    //Calendar Properties
    propDetails *calone = calloc(1, sizeof(propDetails));
    if (!calone)
        return OTHER_ERROR;
    strcpy(calone->name, "FBURL");
    calone->strictNumValues = false;
    calone->propertyValuesRequired = MAX_INTEGER;
    calone->minInstancesInCard = 0;
    calone->maxInstancesInCard = MAX_INTEGER;
    calone->inCardCounter = 0;
    insertBack(list, (void *)calone);

    propDetails *caltwo = calloc(1, sizeof(propDetails));
    if (!caltwo)
        return OTHER_ERROR;
    strcpy(caltwo->name, "CALADRURI");
    caltwo->strictNumValues = false;
    caltwo->propertyValuesRequired = MAX_INTEGER;
    caltwo->minInstancesInCard = 0;
    caltwo->maxInstancesInCard = MAX_INTEGER;
    caltwo->inCardCounter = 0;
    insertBack(list, (void *)caltwo);

    propDetails *calthree = calloc(1, sizeof(propDetails));
    if (!calthree)
        return OTHER_ERROR;
    strcpy(calthree->name, "CALURI");
    calthree->strictNumValues = false;
    calthree->propertyValuesRequired = MAX_INTEGER;
    calthree->minInstancesInCard = 0;
    calthree->maxInstancesInCard = MAX_INTEGER;
    calthree->inCardCounter = 0;
    insertBack(list, (void *)calthree);

    return OK;
}

VCardErrorCode hModuleV2__isPropStructValid(Property *p)
{
    if (p == NULL)
        return INV_PROP;

    if (p->name == NULL || strcmp(p->name, "") == 0)
        return INV_PROP;

    if (p->group == NULL || p->parameters == NULL || p->values == NULL || p->values->length < 1)
        return INV_PROP;

    //Checking for NULL property values
    int valiter = 0;
    int numVals = p->values->length;
    Node *currentValNode = p->values->head;
    for (valiter = 0; valiter < numVals; valiter++)
    {
        char *temp = (char *)currentValNode->data;
        if (temp == NULL)
            return INV_PROP;
        currentValNode = currentValNode->next;
    }
    return OK;
}

VCardErrorCode hModuleV2__isDateTimeStructValid(DateTime *dt)
{
    if (dt == NULL)
        return INV_DT;

    if (dt->isText)
    {
        if (dt->UTC)
            return INV_DT;
        if (strcmp(dt->date, "") != 0)
            return INV_DT;
        if (strcmp(dt->time, "") != 0)
            return INV_DT;
        if (strcmp(dt->text, "") == 0)
            return INV_DT;
    }
    else
    {
        if (strcmp(dt->text, "") != 0)
            return INV_DT;
        if (strcmp(dt->date, "") == 0 && strcmp(dt->time, "") == 0)
            return INV_DT;
        if (strcmp(dt->date, "") != 0 && !hModule__isDateValid(dt->date))
            return INV_DT;
        if (strcmp(dt->time, "") != 0 && !hModule__isTimeValid(dt->time))
            return INV_DT;
    }
    return OK;
}

VCardErrorCode hModuleV2__getPropDetailsFromList(List *list, propDetails **p, char *propName)
{
    if (list == NULL || propName == NULL || list->length == 0)
        return OTHER_ERROR;

    int propNameLen = strlen(propName);
    char *temp = calloc(propNameLen + 1, sizeof(char));
    if (!temp)
        return OTHER_ERROR;
    strncpy(temp, propName, propNameLen);
    hModule__upperString(temp);

    int i = 0;
    int listLen = list->length;
    Node *current = list->head;

    for (i = 0; i < listLen; i++)
    {
        propDetails *toReturn = (propDetails *)current->data;
        int toReturnNameLen = strlen(toReturn->name);

        if (propNameLen == toReturnNameLen && strncmp(temp, toReturn->name, toReturnNameLen) == 0)
        {
            *p = toReturn;
            free(temp);
            return OK;
        }

        current = current->next;
    }

    *p = NULL;
    free(temp);
    return INV_PROP;
}

VCardErrorCode hModuleV2__validatePropertyValuesAndUpdateCounter(List *list, propDetails *p, Property *prop)
{
    if (list == NULL || p == NULL || prop == NULL)
        return OTHER_ERROR;

    //validating property values
    if (p->strictNumValues && prop->values->length != p->propertyValuesRequired)
        return INV_PROP;
    else if (!p->strictNumValues && (prop->values->length < 1 || prop->values->length > p->propertyValuesRequired))
        return INV_PROP;

    //validating property parameters
    if (prop->parameters->length > 0)
    {
        int paramListLen = prop->parameters->length;
        int i = 0;

        Node *current = prop->parameters->head;
        for (i = 0; i < paramListLen; i++)
        {
            Parameter *temp = (Parameter *)current->data;
            if (strcmp(temp->name, "") == 0 || strcmp(temp->value, "") == 0)
                return INV_PROP;

            current = current->next;
        }
    }

    //updating the counter
    propDetails *toBeAdded = calloc(1, sizeof(propDetails));
    strcpy(toBeAdded->name, p->name);
    toBeAdded->strictNumValues = p->strictNumValues;
    toBeAdded->propertyValuesRequired = p->propertyValuesRequired;
    toBeAdded->minInstancesInCard = p->minInstancesInCard;
    toBeAdded->maxInstancesInCard = p->maxInstancesInCard;
    toBeAdded->inCardCounter = p->inCardCounter + 1;

    void *dataToBeDeleted = NULL;

    dataToBeDeleted = deleteDataFromList(list, (void *)p);
    list->deleteData(dataToBeDeleted);
    insertBack(list, (void *)toBeAdded);
    return OK;
}

VCardErrorCode hModuleV2__validateTheFullCard(const Card *obj)
{
    VCardErrorCode err = OK;
    if (obj == NULL || obj->fn == NULL || obj->optionalProperties == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Card struct was NULL or One of its members was NULL.");
        return INV_CARD;
    }

    List *allProperties = initializeList(&hModuleV2__printPropDetails, &hModuleV2__deletePropDetails, &hModuleV2__comparePropDetails);
    err = hModuleV2__initializeAllPropertiesDetails(allProperties);
    if (err != OK)
    {
        hModule__printError(__FILE__, __LINE__, "Failed to initialize all the property details.");
        freeList(allProperties);
        return err;
    }

    Property *toBeExamined;
    propDetails *toBeExaminedDetails;

    //*************************************************
    //          FN MEMBER VALIDATION
    //*************************************************
    toBeExamined = obj->fn;
    err = hModuleV2__isPropStructValid(toBeExamined);
    if (err != OK)
    {
        hModule__printError(__FILE__, __LINE__, "FN Property struct is invalid.");
        freeList(allProperties);
        return err;
    }
    char tempFN[200] = {'\0'};
    strncpy(tempFN, toBeExamined->name, strlen(toBeExamined->name));
    hModule__upperString(tempFN);
    if (strcmp(tempFN, "FN") != 0)
    {
        hModule__printError(__FILE__, __LINE__, "FN member of the card does not contain the fn property.");
        freeList(allProperties);
        return INV_PROP;
    }
    err = hModuleV2__getPropDetailsFromList(allProperties, &toBeExaminedDetails, toBeExamined->name);
    if (err != OK)
    {
        if (err == INV_PROP)
            hModule__printError(__FILE__, __LINE__, "FN Property struct has an invalid name.");
        freeList(allProperties);
        return err;
    }
    err = hModuleV2__validatePropertyValuesAndUpdateCounter(allProperties, toBeExaminedDetails, toBeExamined);
    if (err != OK)
    {
        hModule__printError(__FILE__, __LINE__, "FN Property struct has an invalid amount of values or invalid parameters.");
        freeList(allProperties);
        return err;
    }

    //*************************************************
    //          BDAY MEMBER VALIDATION
    //*************************************************
    if (obj->birthday != NULL)
    {
        err = hModuleV2__isDateTimeStructValid(obj->birthday);
        if (err != OK)
        {
            hModule__printError(__FILE__, __LINE__, "BDAY DATETIME STRUCT IS INVALID.");
            freeList(allProperties);
            return err;
        }

        err = hModuleV2__getPropDetailsFromList(allProperties, &toBeExaminedDetails, "BDAY");
        if (err != OK)
        {
            hModule__printError(__FILE__, __LINE__, "BDAY PROPERTY NOT FOUND.");
            freeList(allProperties);
            return err;
        }
        //updating the counter
        propDetails *toBeAdded = calloc(1, sizeof(propDetails));
        strcpy(toBeAdded->name, toBeExaminedDetails->name);
        toBeAdded->strictNumValues = toBeExaminedDetails->strictNumValues;
        toBeAdded->propertyValuesRequired = toBeExaminedDetails->propertyValuesRequired;
        toBeAdded->minInstancesInCard = toBeExaminedDetails->minInstancesInCard;
        toBeAdded->maxInstancesInCard = toBeExaminedDetails->maxInstancesInCard;
        toBeAdded->inCardCounter = toBeExaminedDetails->inCardCounter + 1;

        void *dataToBeDeleted = NULL;
        dataToBeDeleted = deleteDataFromList(allProperties, (void *)toBeExaminedDetails);
        allProperties->deleteData(dataToBeDeleted);
        insertBack(allProperties, (void *)toBeAdded);
    }

    //*************************************************
    //       ANNIVERSARY MEMBER VALIDATION
    //*************************************************
    if (obj->anniversary != NULL)
    {
        err = hModuleV2__isDateTimeStructValid(obj->anniversary);
        if (err != OK)
        {
            hModule__printError(__FILE__, __LINE__, "ANNIVERSARY DATETIME STRUCT IS INVALID.");
            freeList(allProperties);
            return err;
        }

        err = hModuleV2__getPropDetailsFromList(allProperties, &toBeExaminedDetails, "ANNIVERSARY");
        if (err != OK)
        {
            hModule__printError(__FILE__, __LINE__, "ANNIVERSARY PROPERTY NOT FOUND.");
            freeList(allProperties);
            return err;
        }
        //updating the counter
        propDetails *toBeAdded = calloc(1, sizeof(propDetails));
        strcpy(toBeAdded->name, toBeExaminedDetails->name);
        toBeAdded->strictNumValues = toBeExaminedDetails->strictNumValues;
        toBeAdded->propertyValuesRequired = toBeExaminedDetails->propertyValuesRequired;
        toBeAdded->minInstancesInCard = toBeExaminedDetails->minInstancesInCard;
        toBeAdded->maxInstancesInCard = toBeExaminedDetails->maxInstancesInCard;
        toBeAdded->inCardCounter = toBeExaminedDetails->inCardCounter + 1;

        void *dataToBeDeleted = NULL;
        dataToBeDeleted = deleteDataFromList(allProperties, (void *)toBeExaminedDetails);
        allProperties->deleteData(dataToBeDeleted);
        insertBack(allProperties, (void *)toBeAdded);
    }

    //*************************************************
    //      OPTIONAL PROPERTIES VALIDATION
    //*************************************************
    int optPropListLen = obj->optionalProperties->length;
    int optPropIter = 0;

    Node *current = obj->optionalProperties->head;
    for (optPropIter = 0; optPropIter < optPropListLen; optPropIter++)
    {
        toBeExamined = (Property *)current->data;
        err = hModuleV2__isPropStructValid(toBeExamined);
        if (err != OK)
        {
            hModule__printError(__FILE__, __LINE__, "INVALID PROPERTY STRUCTURE.");
            freeList(allProperties);
            return err;
        }

        char tempProp[200] = {'\0'};
        strncpy(tempProp, toBeExamined->name, strlen(toBeExamined->name));
        hModule__upperString(tempProp);
        if (strcmp(tempProp, "BEGIN") == 0 || strcmp(tempProp, "END") == 0 || strcmp(tempProp, "VERSION") == 0)
        {
            hModule__printError(__FILE__, __LINE__, "BEGIN END OR VERSION TAG FOUND IN OPTIONAL PROPERTIES.");
            freeList(allProperties);
            return INV_CARD;
        }
        if (strcmp(tempProp, "BDAY") == 0 || strcmp(tempProp, "ANNIVERSARY") == 0)
        {
            hModule__printError(__FILE__, __LINE__, "BDAY OR ANNIVERSARY FOUND IN OPTIONAL PROPERTIES.");
            freeList(allProperties);
            return INV_DT;
        }

        err = hModuleV2__getPropDetailsFromList(allProperties, &toBeExaminedDetails, toBeExamined->name);
        if (err != OK)
        {
            if (err == INV_PROP)
            {
                char tempErrorMssg[200] = {'\0'};
                sprintf(tempErrorMssg, "Property struct in optional properties has an invalid property name, %s.", tempProp);
                hModule__printError(__FILE__, __LINE__, tempErrorMssg);
            }
            freeList(allProperties);
            return err;
        }

        err = hModuleV2__validatePropertyValuesAndUpdateCounter(allProperties, toBeExaminedDetails, toBeExamined);
        if (err != OK)
        {
            char tempErrorMssg[200] = {'\0'};
            sprintf(tempErrorMssg, "Property struct in optional properties has an invalid amount of property values or an invalid paramter struct, %s.", tempProp);
            hModule__printError(__FILE__, __LINE__, tempErrorMssg);
            freeList(allProperties);
            return err;
        }
        current = current->next;
    }

    //cardinality Checker
    err = hModuleV2__cardinalityCheckerForAllProperties(allProperties);

    freeList(allProperties);
    return err;
}

VCardErrorCode hModuleV2__cardinalityCheckerForAllProperties(List *allprop)
{
    if (allprop == NULL)
        return OTHER_ERROR;

    int allPropListLen = allprop->length;
    int i = 0;

    Node *current = allprop->head;
    for (i = 0; i < allPropListLen; i++)
    {
        propDetails *temp = (propDetails *)current->data;
        if (!temp)
            return OTHER_ERROR;

        int min = temp->minInstancesInCard;
        int max = temp->maxInstancesInCard;
        int cardInstances = temp->inCardCounter;

        if (cardInstances < min || cardInstances > max)
        {
            char tempErrorMssg[200] = {'\0'};
            sprintf(tempErrorMssg, "CARDINALITY FAILED FOR PROPERTY, %s.", temp->name);
            hModule__printError(__FILE__, __LINE__, tempErrorMssg);
            return INV_PROP;
        }
        current = current->next;
    }
    return OK;
}

void hModuleV2__convertStrListToJSON(const List *list, char **allocatedJSONstr)
{
    *allocatedJSONstr = NULL;
    char *temp = calloc(1000, sizeof(char));

    if (list == NULL || !temp)
    {
        return;
    }

    strcpy(temp, "");
    strcat(temp, "[");

    int count = 0;
    int listLen = list->length;
    Node *current = list->head;

    for (count = 0; count < listLen; count++)
    {
        char *value = (char *)current->data;

        strcat(temp, "\"");
        strncat(temp, value, strlen(value));
        strcat(temp, "\"");

        if ((count + 1) < listLen)
            strcat(temp, ",");

        current = current->next;
    }

    strcat(temp, "]");

    *allocatedJSONstr = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(*allocatedJSONstr, temp, strlen(temp));
    free(temp);
    return;
}

void hModuleV2__convertPropToJSON(const Property *prop, char **str)
{
    char *temp = calloc(2000, sizeof(char));

    if (!prop || !prop->group || !prop->name || !prop->values || !prop->parameters || !temp)
    {
        *str = calloc(1, sizeof(char));
        strcpy(*str, "");
        free(temp);
        return;
    }

    strcpy(temp, "");

    char groupTag[50] = "\"group\":\"";
    char nameTag[50] = "\"name\":\"";
    char valuesTag[50] = "\"values\":";

    //opening brace
    strcat(temp, "{");

    //group
    strncat(temp, groupTag, strlen(groupTag));
    strncat(temp, prop->group, strlen(prop->group));
    strcat(temp, "\",");

    //name
    strncat(temp, nameTag, strlen(nameTag));
    strncat(temp, prop->name, strlen(prop->name));
    strcat(temp, "\",");

    //values
    strncat(temp, valuesTag, strlen(valuesTag));
    char *propVals = strListToJSON(prop->values);
    strncat(temp, propVals, strlen(propVals));
    free(propVals);

    //closing brace
    strcat(temp, "}");

    *str = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(*str, temp, strlen(temp));
    free(temp);
}

void hModuleV2__convertDtToJSON(const DateTime *prop, char **str)
{
    char *temp = calloc(2000, sizeof(char));

    if (!prop || !temp)
    {
        *str = calloc(1, sizeof(char));
        strcpy(*str, "");
        free(temp);
        return;
    }

    strcpy(temp, "");

    char isTextTag[50] = "\"isText\":";
    char dateTag[50] = "\"date\":\"";
    char timeTag[50] = "\"time\":\"";
    char textTag[50] = "\"text\":\"";
    char isUTCTag[50] = "\"isUTC\":";

    strcat(temp, "{");

    //isText
    strncat(temp, isTextTag, strlen(isTextTag));
    if (prop->isText)
        strncat(temp, "true", strlen("true"));
    else
        strncat(temp, "false", strlen("false"));
    strcat(temp, ",");

    strncat(temp, dateTag, strlen(dateTag));
    strncat(temp, prop->date, strlen(prop->date));
    strcat(temp, "\",");

    strncat(temp, timeTag, strlen(timeTag));
    strncat(temp, prop->time, strlen(prop->time));
    strcat(temp, "\",");

    strncat(temp, textTag, strlen(textTag));
    if (prop->text)
        strncat(temp, prop->text, strlen(prop->text));
    strcat(temp, "\",");

    strncat(temp, isUTCTag, strlen(isUTCTag));
    if (prop->UTC)
        strncat(temp, "true", strlen("true"));
    else
        strncat(temp, "false", strlen("false"));

    strcat(temp, "}");

    *str = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(*str, temp, strlen(temp));
    free(temp);
}

void hModuleV2__convertJSONToStrList(List **list, const char *str)
{
    *list = NULL;

    if (str == NULL || strlen(str) == 0)
        return;

    if (str[0] != '[' || str[strlen(str) - 1] != ']')
        return;

    int i = 1;
    *list = initializeList(&printValue, &deleteValue, &compareValues);

    while (i < strlen(str) && str[i] != ']')
    {
        char temp[1000] = {'\0'};
        char tempFinal[1000] = {'\0'};
        int j = 0;
        while (i < (strlen(str) - 1) && str[i] != ',')
        {
            temp[j] = str[i];
            j++;
            i++;
        }

        if (strlen(temp) == 0 || temp[0] != '\"' || temp[strlen(temp) - 1] != '\"')
        {
            freeList(*list);
            *list = NULL;
            return;
        }

        int one = 0, two = 0;
        for (one = 0; one < strlen(temp); one++)
        {
            if (one == 0 || one == (strlen(temp) - 1))
                continue;

            tempFinal[two] = temp[one];
            two++;
        }

        char *toBeAdded = calloc(strlen(tempFinal) + 1, sizeof(char));
        strncpy(toBeAdded, tempFinal, strlen(tempFinal));
        insertBack(*list, (void *)toBeAdded);
        i++;
    }
}

void hModuleV2__convertJSONtoProp(Property **propToBeCreated, const char *str)
{
    *propToBeCreated = NULL;

    if (str == NULL || strlen(str) == 0)
        return;

    if (str[0] != '{' || str[strlen(str) - 1] != '}')
        return;

    int stringLen = strlen(str);
    char *temp = calloc(stringLen + 1, sizeof(char));

    int i = 0, j = 0;
    for (i = 0; i < stringLen; i++)
    {
        //stripping {}
        if (i == 0 || i == (stringLen - 1))
            continue;

        temp[j] = str[i];
        j++;
    }

    char *groupTok = NULL;
    char *nameTok = NULL;
    char *valuesTok = NULL;
    char *nullChecker = NULL;

    groupTok = strtok(temp, ",");
    if (groupTok)
        nameTok = strtok(NULL, ",");
    if (nameTok)
        valuesTok = strtok(NULL, "\0");
    if (valuesTok)
        nullChecker = strtok(NULL, ",");

    if (!nameTok || !groupTok || !valuesTok || nullChecker)
    {
        free(temp);
        return;
    }

    char grouptag[100] = {'\0'};
    char groupVal[1000] = {'\0'};
    char nametag[100] = {'\0'};
    char nameVal[1000] = {'\0'};
    char valuestag[100] = {'\0'};
    char valuesVal[1000] = {'\0'};

    i = 0;
    while (i < strlen(groupTok))
    {
        while (i < strlen(groupTok) && groupTok[i] != ':')
        {
            grouptag[i] = groupTok[i];
            i++;
        }

        //no value
        if (i == strlen(groupTok) || i + 1 == strlen(groupTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(groupTok))
        {
            groupVal[j] = groupTok[i];
            j++;
            i++;
        }
    }

    i = 0;
    while (i < strlen(nameTok))
    {
        while (i < strlen(nameTok) && nameTok[i] != ':')
        {
            nametag[i] = nameTok[i];
            i++;
        }

        //no value
        if (i == strlen(nameTok) || i + 1 == strlen(nameTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(nameTok))
        {
            nameVal[j] = nameTok[i];
            j++;
            i++;
        }
    }

    i = 0;
    while (i < strlen(valuesTok))
    {
        while (i < strlen(valuesTok) && valuesTok[i] != ':')
        {
            valuestag[i] = valuesTok[i];
            i++;
        }

        //no value
        if (i == strlen(valuesTok) || i + 1 == strlen(valuesTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(valuesTok))
        {
            valuesVal[j] = valuesTok[i];
            j++;
            i++;
        }
    }

    if (strcmp(grouptag, "\"group\"") != 0 || strcmp(nametag, "\"name\"") != 0 || strcmp(valuestag, "\"values\"") != 0)
    {
        free(temp);
        return;
    }

    if (strlen(nameVal) < 2 || strlen(groupVal) < 2 || nameVal[0] != '\"' || nameVal[strlen(nameVal) - 1] != '\"' || groupVal[0] != '\"' || groupVal[strlen(groupVal) - 1] != '\"')
    {
        free(temp);
        return;
    }

    char finalName[1000] = {'\0'};
    char finalGroup[1000] = {'\0'};

    i = 0;
    j = 0;
    for (i = 0; i < strlen(nameVal); i++)
    {
        //stripping {}
        if (i == 0 || i == (strlen(nameVal) - 1))
            continue;

        finalName[j] = nameVal[i];
        j++;
    }

    i = 0;
    j = 0;
    for (i = 0; i < strlen(groupVal); i++)
    {
        //stripping {}
        if (i == 0 || i == (strlen(groupVal) - 1))
            continue;

        finalGroup[j] = groupVal[i];
        j++;
    }

    Property *a = calloc(1, sizeof(Property));

    a->name = calloc(strlen(finalName) + 1, sizeof(char));
    strncpy(a->name, finalName, strlen(finalName));

    a->group = calloc(strlen(finalGroup) + 1, sizeof(char));
    strncpy(a->group, finalGroup, strlen(finalGroup));

    a->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    a->values = JSONtoStrList(valuesVal);
    if (!a->values)
    {
        free(temp);
        deleteProperty(a);
        return;
    }

    *propToBeCreated = a;
    free(temp);
    return;
}

void hModuleV2__convertJSONtoDT(DateTime **dt, const char *str)
{
    *dt = NULL;

    if (str == NULL || strlen(str) == 0)
        return;

    if (str[0] != '{' || str[strlen(str) - 1] != '}')
        return;

    int stringLen = strlen(str);
    char *temp = calloc(stringLen + 1, sizeof(char));

    int i = 0, j = 0;
    for (i = 0; i < stringLen; i++)
    {
        //stripping {}
        if (i == 0 || i == (stringLen - 1))
            continue;

        temp[j] = str[i];
        j++;
    }

    char *isUTCTok = NULL;
    char *dateTok = NULL;
    char *timeTok = NULL;
    char *textTok = NULL;
    char *isTextTok = NULL;
    char *nullChecker = NULL;

    isTextTok = strtok(temp, ",");
    if (isTextTok)
        dateTok = strtok(NULL, ",");
    if (dateTok)
        timeTok = strtok(NULL, ",");
    if (timeTok)
        textTok = strtok(NULL, ",");
    if (textTok)
        isUTCTok = strtok(NULL, ",");
    if (isUTCTok)
        nullChecker = strtok(NULL, "");

    if (!isTextTok || !dateTok || !timeTok || !textTok || !isUTCTok || nullChecker)
    {
        free(temp);
        return;
    }

    char isTextTag[100] = {'\0'};
    char isTextVal[1000] = {'\0'};
    char dateTag[100] = {'\0'};
    char dateVal[1000] = {'\0'};
    char timeTag[100] = {'\0'};
    char timeVal[1000] = {'\0'};
    char textTag[100] = {'\0'};
    char textVal[1000] = {'\0'};
    char isUTCTag[100] = {'\0'};
    char isUTCVal[1000] = {'\0'};

    i = 0;
    while (i < strlen(isTextTok))
    {
        while (i < strlen(isTextTok) && isTextTok[i] != ':')
        {
            isTextTag[i] = isTextTok[i];
            i++;
        }

        //no value
        if (i == strlen(isTextTok) || i + 1 == strlen(isTextTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(isTextTok))
        {
            isTextVal[j] = isTextTok[i];
            i++;
            j++;
        }
    }

    i = 0;
    while (i < strlen(dateTok))
    {
        while (i < strlen(dateTok) && dateTok[i] != ':')
        {
            dateTag[i] = dateTok[i];
            i++;
        }

        //no value
        if (i == strlen(dateTok) || i + 1 == strlen(dateTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(dateTok))
        {
            dateVal[j] = dateTok[i];
            i++;
            j++;
        }
    }

    i = 0;
    while (i < strlen(timeTok))
    {
        while (i < strlen(timeTok) && timeTok[i] != ':')
        {
            timeTag[i] = timeTok[i];
            i++;
        }

        //no value
        if (i == strlen(timeTok) || i + 1 == strlen(timeTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(timeTok))
        {
            timeVal[j] = timeTok[i];
            i++;
            j++;
        }
    }

    i = 0;
    while (i < strlen(textTok))
    {
        while (i < strlen(textTok) && textTok[i] != ':')
        {
            textTag[i] = textTok[i];
            i++;
        }

        //no value
        if (i == strlen(textTok) || i + 1 == strlen(textTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(textTok))
        {
            textVal[j] = textTok[i];
            i++;
            j++;
        }
    }

    i = 0;
    while (i < strlen(isUTCTok))
    {
        while (i < strlen(isUTCTok) && isUTCTok[i] != ':')
        {
            isUTCTag[i] = isUTCTok[i];
            i++;
        }

        //no value
        if (i == strlen(isUTCTok) || i + 1 == strlen(isUTCTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(isUTCTok))
        {
            isUTCVal[j] = isUTCTok[i];
            i++;
            j++;
        }
    }

    if (strcmp(isTextTag, "\"isText\"") != 0 || strcmp(dateTag, "\"date\"") != 0 || strcmp(timeTag, "\"time\"") != 0 || strcmp(textTag, "\"text\"") != 0 || strcmp(isUTCTag, "\"isUTC\"") != 0)
    {
        free(temp);
        return;
    }

    if (strlen(isTextVal) < 2 || strlen(dateVal) < 2 || strlen(timeVal) < 2 || strlen(textVal) < 2 || strlen(isUTCVal) < 2)
    {
        free(temp);
        return;
    }

    if (dateVal[0] != '\"' || timeVal[0] != '\"' || textVal[0] != '\"')
    {
        free(temp);
        return;
    }

    if (dateVal[strlen(dateVal) - 1] != '\"' || timeVal[strlen(timeVal) - 1] != '\"' || textVal[strlen(textVal) - 1] != '\"')
    {
        free(temp);
        return;
    }

    char finalIsText[1000] = {'\0'};
    char finaldate[1000] = {'\0'};
    char finaltime[1000] = {'\0'};
    char finaltext[1000] = {'\0'};
    char finalIsUTC[1000] = {'\0'};

    i = 0;
    j = 0;
    for (i = 0; i < strlen(isTextVal); i++)
    {
        finalIsText[j] = isTextVal[i];
        j++;
    }

    i = 0;
    j = 0;
    for (i = 0; i < strlen(dateVal); i++)
    {
        //stripping {}
        if (i == 0 || i == (strlen(dateVal) - 1))
            continue;

        finaldate[j] = dateVal[i];
        j++;
    }

    i = 0;
    j = 0;
    for (i = 0; i < strlen(timeVal); i++)
    {
        //stripping {}
        if (i == 0 || i == (strlen(timeVal) - 1))
            continue;

        finaltime[j] = timeVal[i];
        j++;
    }

    i = 0;
    j = 0;
    for (i = 0; i < strlen(textVal); i++)
    {
        //stripping {}
        if (i == 0 || i == (strlen(textVal) - 1))
            continue;

        finaltext[j] = textVal[i];
        j++;
    }

    i = 0;
    j = 0;
    for (i = 0; i < strlen(isUTCVal); i++)
    {
        finalIsUTC[j] = isUTCVal[i];
        j++;
    }

    DateTime *a = calloc(1 + (strlen(finaltext) + 1), sizeof(DateTime));

    if (strcmp(finalIsText, "true") == 0 || strcmp(finalIsText, "\"true\"") == 0)
        a->isText = true;
    else
        a->isText = false;

    if (strcmp(finalIsUTC, "true") == 0 || strcmp(finalIsUTC, "\"true\"") == 0)
        a->UTC = true;
    else
        a->UTC = false;

    strncpy(a->date, finaldate, 8);
    strncpy(a->time, finaltime, 6);
    strncpy(a->text, finaltext, strlen(finaltext));

    *dt = a;
    free(temp);
    return;
}

void hModuleV2__convertJSONtoCard(Card **obj, const char *str)
{
    *obj = NULL;

    if (str == NULL || strlen(str) == 0)
        return;

    if (str[0] != '{' || str[strlen(str) - 1] != '}')
        return;

    int stringLen = strlen(str);
    char *temp = calloc(stringLen + 1, sizeof(char));

    int i = 0, j = 0;
    for (i = 0; i < stringLen; i++)
    {
        //stripping {}
        if (i == 0 || i == (stringLen - 1))
            continue;

        temp[j] = str[i];
        j++;
    }

    char *fnTok = NULL;
    char *nullchecker = NULL;

    fnTok = strtok(temp, ",");
    if (fnTok)
        nullchecker = strtok(NULL, ",");

    if (!fnTok || nullchecker)
    {
        free(temp);
        return;
    }

    char fnTag[100] = {'\0'};
    char fnVal[1000] = {'\0'};

    i = 0;
    while (i < strlen(fnTok))
    {
        while (i < strlen(fnTok) && fnTok[i] != ':')
        {
            fnTag[i] = fnTok[i];
            i++;
        }

        //no value
        if (i == strlen(fnTok) || i + 1 == strlen(fnTok))
        {
            free(temp);
            return;
        }

        i++;
        int j = 0;
        while (i < strlen(fnTok))
        {
            fnVal[j] = fnTok[i];
            i++;
            j++;
        }
    }

    if (strcmp(fnTag, "\"FN\"") != 0 || strlen(fnVal) < 2 || fnVal[0] != '\"' || fnVal[strlen(fnVal) - 1] != '\"')
    {
        free(temp);
        return;
    }

    char finalFN[1000] = {'\0'};

    i = 0;
    j = 0;
    for (i = 0; i < strlen(fnVal); i++)
    {
        //stripping {}
        if (i == 0 || i == (strlen(fnVal) - 1))
            continue;

        finalFN[j] = fnVal[i];
        j++;
    }

    Property *fnProp = calloc(1, sizeof(Property));
    fnProp->group = calloc(strlen("") + 1, sizeof(char));
    strcpy(fnProp->group, "");
    fnProp->name = calloc(strlen("FN") + 1, sizeof(char));
    strcpy(fnProp->name, "FN");
    fnProp->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    fnProp->values = initializeList(&printValue, &deleteValue, &compareValues);
    char *toADD = calloc(strlen(finalFN) + 1, sizeof(char));
    strncpy(toADD, finalFN, strlen(finalFN));
    insertBack(fnProp->values, (void *)toADD);

    Card *a = calloc(1, sizeof(Card));
    a->anniversary = NULL;
    a->birthday = NULL;
    a->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    a->fn = fnProp;

    *obj = a;
    free(temp);
    return;
}

// NodeJS Helper Functions

char *createFileLogJSONString(char *fileName)
{
    Card *obj = NULL;
    VCardErrorCode err = OK;
    err = createCard(fileName, &obj);
    if (obj)
        err = validateCard(obj);

    char *JSONString = NULL;

    if (err != OK)
    {
        char errorTag[50] = "\"error\":\"";
        char filenameTag[50] = "\"filename\":\"";

        char *temp = calloc(2000, sizeof(char));
        strcpy(temp, "");

        strcat(temp, "{");

        strncat(temp, filenameTag, strlen(filenameTag));
        strncat(temp, fileName, strlen(fileName));
        strcat(temp, "\",");

        strncat(temp, errorTag, strlen(errorTag));
        char *errString = printError(err);
        strncat(temp, errString, strlen(errString));
        strcat(temp, "\"}");

        JSONString = calloc(strlen(temp) + 1, sizeof(char));
        strcpy(JSONString, temp);

        free(temp);
        free(errString);
        return JSONString;
    }

    int bdannivCounter = 0;
    if (obj->birthday)
        bdannivCounter++;
    if (obj->anniversary)
        bdannivCounter++;

    char *temp = calloc(2000, sizeof(char));
    strcpy(temp, "");

    char filenameTag[50] = "\"filename\":\"";
    char nameTag[50] = "\"name\":\"";
    char apTag[50] = "\"additionalProperties\":\"";

    //opening brace
    strcat(temp, "{");

    strncat(temp, filenameTag, strlen(filenameTag));
    strncat(temp, fileName, strlen(fileName));
    strcat(temp, "\",");

    strncat(temp, nameTag, strlen(nameTag));
    strncat(temp, (char *)obj->fn->values->head->data, strlen((char *)obj->fn->values->head->data));
    strcat(temp, "\",");

    char tempNum[100];
    snprintf(tempNum, sizeof(tempNum), "%d", obj->optionalProperties->length + bdannivCounter);

    strncat(temp, apTag, strlen(apTag));
    strncat(temp, tempNum, strlen(tempNum));
    strcat(temp, "\"");

    //closing brace
    strcat(temp, "}");

    JSONString = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(JSONString, temp, strlen(temp));

    free(temp);
    deleteCard(obj);
    return JSONString;
}

char *createCardViewJSONString(char *fileName)
{
    Card *obj = NULL;
    VCardErrorCode err = OK;
    err = createCard(fileName, &obj);
    if (obj)
        err = validateCard(obj);

    char *JSONString = NULL;

    if (err != OK)
    {
        char errorTag[50] = "\"error\":\"";
        char *temp = calloc(2000, sizeof(char));
        strcpy(temp, "");

        strcat(temp, "{");
        strncat(temp, errorTag, strlen(errorTag));
        char *errString = printError(err);
        strncat(temp, errString, strlen(errString));
        strcat(temp, "\"}");

        JSONString = calloc(strlen(temp) + 1, sizeof(char));
        strcpy(JSONString, temp);

        free(temp);
        free(errString);
        return JSONString;
    }

    char *temp = calloc(100000, sizeof(char));
    strcpy(temp, "");

    strcat(temp, "[");

    char *fnTemp = propToJSON(obj->fn);
    strncat(temp, fnTemp, strlen(fnTemp));
    free(fnTemp);

    int i = 0;
    Node *current = obj->optionalProperties->head;
    for (i = 0; i < obj->optionalProperties->length; i++)
    {
        Property *currentProp = (Property *)current->data;

        strcat(temp, ",");

        char *opTemp = propToJSON(currentProp);
        strncat(temp, opTemp, strlen(opTemp));
        free(opTemp);

        current = current->next;
    }
    strcat(temp, "]");

    JSONString = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(JSONString, temp, strlen(temp));

    free(temp);
    deleteCard(obj);
    return JSONString;
}