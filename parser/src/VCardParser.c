/**
 * @file VCardParser.c
 * @author Neel Gondalia (0940429)
 * @date September 2018
 * @brief File containing the function definitions of VCard Parser API
 */

#include "LinkedListAPI.h"
#include "VCardParser.h"
#include "HelperModule.h"

/** Parses the given file, and if the parsing is done without any errors, it allocated a new card object at the provided address.
 * Otherwise, deallocates any memory allocated and sets the newCardObject to NULL and returns the appropriate error code.
 * @pre filename is allocated
 * @param fileName is the file that is to be parsed
 * @param newCardObject hold the address of the pointer where the successfully parsed card is supposed to stored
 * @return on success: OK on failure: the appropriate error code
 **/
VCardErrorCode createCard(char *fileName, Card **newCardObject)
{
    //no memory address specified
    if (newCardObject == NULL)
        return OTHER_ERROR;

    FILE *vcardfile = NULL;
    VCardErrorCode err = OK;

    if (fileName == NULL || (!(hModule__stringEndsWith(fileName, ".vcf")) && !(hModule__stringEndsWith(fileName, ".vcard"))) || (vcardfile = fopen(fileName, "r")) == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Invalid file extension or unable to open file.");
        *newCardObject = NULL;
        err = INV_FILE;
        return err;
    }

    /*Temporary card is created, allocated and initialized. This card will hold all the parsed data. If the parsing is successful, then 
     * the card will be assigned to the card that was passed in.
     */
    Card *toBeCreated = calloc(1, sizeof(Card));
    if (toBeCreated == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        err = OTHER_ERROR;
        return err;
    }

    toBeCreated->fn = NULL;
    toBeCreated->birthday = NULL;
    toBeCreated->anniversary = NULL;
    toBeCreated->optionalProperties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    err = hModule__parseFile(vcardfile, toBeCreated);
    if (err != OK)
    {
        deleteCard(toBeCreated);
        *newCardObject = NULL;
    }
    else
    {
        *newCardObject = toBeCreated;
    }

    fclose(vcardfile);
    return err;
}

/** Deallocates all the memory associated with the subcomponents of a card object and deallocates the card object itself.
 * @pre card object must exist and have memory allocated to it
 * @param obj is the pointer to the card object that is to be deleted
 **/
void deleteCard(Card *obj)
{
    if (obj == NULL)
        return;

    deleteProperty((void *)obj->fn);
    deleteDate((void *)obj->birthday);
    deleteDate((void *)obj->anniversary);
    freeList(obj->optionalProperties);
    free(obj);
    return;
}

/** Allocates a string and copies all the card data that is to be printed to it. The allocated string is then returned.
 * @pre card object must exist and have memory allocated to it
 * @param obj is the pointer to the card object that is to be printed
 * @return returns a pointer to the string that holds all the card data that is to be printed
 **/
char *printCard(const Card *obj)
{
    if (obj == NULL)
        return NULL;

    char *propFNPrint = NULL, *bdayPrint = NULL, *annivPrint = NULL, *optionalProps = NULL, *toPrint = NULL;
    int propLen = 0, bdayLen = 0, annivLen = 0, optionalLen = 0;

    //static array of all print headers
    char header[400] = CYAN "\n\n###############################################################################################\n######################################   PRINT CARD   #########################################\n###############################################################################################\n" RESET;
    char footer[400] = CYAN "\n\n###############################################################################################\n######################################   PRINT CARD   #########################################\n###############################################################################################\n\n\n" RESET;
    char bdayHeader[100] = CYAN "\n###############  BDAY  ####################" RESET;
    char annivHeader[100] = CYAN "\n###############  ANNI  ####################" RESET;
    int headerLen = strlen(header);
    int footerLen = strlen(footer);
    int bdayHeaderLen = strlen(bdayHeader);
    int annivHeaderLen = strlen(annivHeader);

    //getting a string of all prints related to the card
    propFNPrint = printProperty((void *)obj->fn);
    bdayPrint = printDate((void *)obj->birthday);
    annivPrint = printDate((void *)obj->anniversary);
    optionalProps = toString(obj->optionalProperties);

    //needed for allocating the right amount of memory. Based on what is available in the card
    if (propFNPrint)
        propLen = strlen(propFNPrint);
    if (bdayPrint)
        bdayLen = strlen(bdayPrint) + bdayHeaderLen;
    if (annivPrint)
        annivLen = strlen(annivPrint) + annivHeaderLen;
    if (optionalProps)
        optionalLen = strlen(optionalProps);

    //total memory needed to be allocated to print the card
    int totalLen = headerLen + footerLen + propLen + bdayLen + annivLen + optionalLen + 1;
    toPrint = calloc(totalLen, sizeof(char));
    if (toPrint == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory Allocation Error.");
        free(propFNPrint);
        free(bdayPrint);
        free(annivPrint);
        free(optionalProps);
        return NULL;
    }

    //concating the final string that is to be returned
    strncpy(toPrint, header, headerLen);
    if (bdayPrint)
    {
        strncat(toPrint, bdayHeader, bdayHeaderLen);
        strncat(toPrint, bdayPrint, bdayLen);
    }
    if (annivPrint)
    {
        strncat(toPrint, annivHeader, annivHeaderLen);
        strncat(toPrint, annivPrint, annivLen);
    }
    if (propFNPrint)
        strncat(toPrint, propFNPrint, propLen);
    if (optionalProps)
        strncat(toPrint, optionalProps, optionalLen);
    strncat(toPrint, footer, footerLen);

    //deallocating the strings that were allocated in the printProperty, printdate and tostring functions
    free(propFNPrint);
    free(bdayPrint);
    free(annivPrint);
    free(optionalProps);
    return toPrint;
}

/** Allocates a string and copies the error message that is to be printed to it. The allocated string is then returned.
 * @param err is the error code that occured and needs to be displayed
 * @return returns a pointer to the string that holds the error message that is to be printed
 **/
char *printError(VCardErrorCode err)
{
    char *print = NULL;
    print = calloc(100, sizeof(char));
    if (print == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        return NULL;
    }

    if (err == OK)
    {
        char temp[100] = "NO ERRORS FOUND.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else if (err == INV_CARD)
    {
        char temp[100] = "*** INVALID CARD.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else if (err == INV_FILE)
    {
        char temp[100] = "*** INVALID FILE.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else if (err == INV_PROP)
    {
        char temp[100] = "*** INVALID PROPERTY FOUND.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else if (err == WRITE_ERROR)
    {
        char temp[100] = "*** A WRITE TO THE FILE IS NOT VALID.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else if (err == OTHER_ERROR)
    {
        char temp[100] = "*** NKNOWN ERROR OCCURED.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else if (err == INV_DT)
    {
        char temp[100] = "*** INVALID DATETIME PROPERTY FOUND.";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }
    else
    {
        char temp[100] = "INVALID ERROR CODE";
        int tempLen = strlen(temp);
        strncpy(print, temp, tempLen);
    }

    return print;
}

/** Function to writing a Card object into a file in iCard format.
 *@pre Card object exists, and is not NULL.
        fileName is not NULL, has the correct extension
 *@post Card has not been modified in any way, and a file representing the
        Card contents in vCard format has been created
 *@return the error code indicating success or the error encountered when traversing the Card
 *@param obj - a pointer to a Card struct
		 fileName - the name of the output file
 **/
VCardErrorCode writeCard(const char *fileName, const Card *obj)
{
    if (obj == NULL || fileName == NULL)
        return WRITE_ERROR;

    FILE *vcardfile = NULL;
    VCardErrorCode err = OK;

    char *tempFileName = calloc(strlen(fileName) + 1, sizeof(char));
    strcpy(tempFileName, "");
    strncat(tempFileName, fileName, strlen(fileName));

    if ((!(hModule__stringEndsWith(tempFileName, ".vcf")) && !(hModule__stringEndsWith(tempFileName, ".vcard"))) || (vcardfile = fopen(fileName, "w")) == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Invalid file extension or unable to open file for writing.");
        free(tempFileName);
        err = WRITE_ERROR;
        return err;
    }
    free(tempFileName);

    fputs("BEGIN:VCARD\r\n", vcardfile);
    fputs("VERSION:4.0\r\n", vcardfile);

    char *fnString = NULL;
    err = hModuleV2_getPropertyLineFromCard(obj->fn, &fnString);
    if (err != OK)
    {
        fclose(vcardfile);
        return err;
    }
    if (fnString)
        fputs(fnString, vcardfile);
    free(fnString);

    char *bdayString = NULL;
    err = hModuleV2_getdateTimePropertyLineFromCard(obj->birthday, &bdayString, 0);
    if (err != OK)
    {
        fclose(vcardfile);
        return err;
    }
    if (bdayString)
        fputs(bdayString, vcardfile);
    free(bdayString);

    char *annivString = NULL;
    err = hModuleV2_getdateTimePropertyLineFromCard(obj->anniversary, &annivString, 1);
    if (err != OK)
    {
        fclose(vcardfile);
        return err;
    }
    if (annivString)
        fputs(annivString, vcardfile);
    free(annivString);

    if (obj->optionalProperties)
    {
        int i = 0;
        Node *current = obj->optionalProperties->head;
        for (i = 0; i < obj->optionalProperties->length; i++)
        {
            Property *temp = (Property *)current->data;

            char *propString = NULL;
            err = hModuleV2_getPropertyLineFromCard(temp, &propString);
            if (err != OK)
            {
                fclose(vcardfile);
                return err;
            }
            if (propString)
                fputs(propString, vcardfile);
            free(propString);

            current = current->next;
        }
    }

    fputs("END:VCARD\r\n", vcardfile);
    fclose(vcardfile);
    return err;
}

/** Function to validate a card object
 * @pre Card object exists and is not NULL.
 * @post Card object has not been modified in any way,
 * @param obj - a pointer to a Card struct
 * @return the error code indicating success or the error encountered while validating a Card object.
 */
VCardErrorCode validateCard(const Card *obj)
{
    VCardErrorCode err = OK;
    err = hModuleV2__validateTheFullCard(obj);
    return err;
}

/** Function for converting a list of strings into a JSON string
 *@pre List exists, is not null, and is valid
 *@post List has not been modified in any way, and a JSON string has been created
 *@return newly allocated JSON string.  May be NULL.
 *@param strList - a pointer to a List of Values
 **/
char *strListToJSON(const List *strList)
{
    char *allocatedStr = NULL;

    if (strList == NULL)
    {
        return NULL;
    }

    hModuleV2__convertStrListToJSON(strList, &allocatedStr);
    return allocatedStr;
}

/** Function for creating an List of strings from an JSON string
 *@pre String is not null, and is valid
 *@post String has not been modified in any way, and a List has been created
 *@return a newly allocated List.  May be NULL.
 *@param str - a pointer to a JSON string
 **/
List *JSONtoStrList(const char *str)
{
    if (str == NULL)
        return NULL;

    List *strList = NULL;
    hModuleV2__convertJSONToStrList(&strList, str);
    return strList;
}

/** Function for converting a Property struct into a JSON string
 *@pre Property exists, is not null, and is valid
 *@post Property has not been modified in any way, and a JSON string has been created
 *@return newly allocated JSON string.  May be NULL.
 *@param prop - a pointer to a Property struct
 **/
char *propToJSON(const Property *prop)
{
    char *temp = NULL;

    if (prop == NULL)
    {
        temp = calloc(1, sizeof(char));
        strcpy(temp, "");
        return temp;
    }

    hModuleV2__convertPropToJSON(prop, &temp);
    return temp;
}

/** Function for creating a Property struct from an JSON string
 *@pre String is not null, and is valid
 *@post String has not been modified in any way, and a Property struct has been created
 *@return a newly allocated Property.  May be NULL.
 *@param str - a pointer to a JSON string
 **/
Property *JSONtoProp(const char *str)
{
    if (str == NULL)
        return NULL;

    Property *toBeReturned = NULL;
    hModuleV2__convertJSONtoProp(&toBeReturned, str);
    return toBeReturned;
}

/** Function for converting a DateTime struct into a JSON string
 *@pre DateTime exists, is not null, and is valid
 *@post DateTime has not been modified in any way, and a JSON string has been created
 *@return newly allocated JSON string.  May be NULL.
 *@param prop - a pointer to a DateTime struct
 **/
char *dtToJSON(const DateTime *prop)
{
    char *temp = NULL;
    if (prop == NULL)
    {
        temp = calloc(1, sizeof(char));
        strcpy(temp, "");
        return temp;
    }

    hModuleV2__convertDtToJSON(prop, &temp);
    return temp;
}

/** Function for creating a DateTime struct from an JSON string
 *@pre String is not null, and is valid
 *@post String has not been modified in any way, and a DateTime struct has been created
 *@return a newly allocated DateTime.  May be NULL.
 *@param str - a pointer to a JSON string
 **/
DateTime *JSONtoDT(const char *str)
{
    if (str == NULL)
        return NULL;

    DateTime *toBeReturned = NULL;
    hModuleV2__convertJSONtoDT(&toBeReturned, str);
    return toBeReturned;
}

/** Function for creating a Card struct from an JSON string
 *@pre String is not null, and is valid
 *@post String has not been modified in any way, and a Card struct has been created
 *@return a newly allocated Card.  May be NULL.
 *@param str - a pointer to a JSON string
 **/
Card *JSONtoCard(const char *str)
{
    if (str == NULL)
        return NULL;

    Card *toBeReturned = NULL;
    hModuleV2__convertJSONtoCard(&toBeReturned, str);
    return toBeReturned;
}

/** Function for adding an optional Property to a Card object
 *@pre both arguments are not NULL and valid
 *@post Property has not been modified in any way, and its address had been added to 
 *      Card's optionalProperties list
 *@return void
 *@param card - a pointer to a Card struct
 *@param toBeAdded - a pointer to an Property struct
**/
void addProperty(Card *card, const Property *toBeAdded)
{
    if (card == NULL || toBeAdded == NULL)
        return;

    if (card->optionalProperties == NULL)
        return;

    insertBack(card->optionalProperties, (void *)toBeAdded);
}

/** Deallocates all the subcomponents of a property and the property itself.
 * @pre toBeDeleted Property needs to have memory allocated
 * @param toBeDeleted is the property that is to be deallocated
 **/
void deleteProperty(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
        return;

    Property *propToBeDeleted = (Property *)toBeDeleted;

    free(propToBeDeleted->name);
    free(propToBeDeleted->group);
    freeList(propToBeDeleted->parameters);
    freeList(propToBeDeleted->values);
    free(propToBeDeleted);
    return;
}

/** Compares two properties to see if they are equal based on the name and group of the properties
 * @pre first and second properties must be allocated
 * @param first property to be compared
 * @param second property to be compared
 * @return if equal: returns 0 else if first > second returns 1 else returns -1
 **/
int compareProperties(const void *first, const void *second)
{
    if (first == NULL && second == NULL)
        return 0;
    if (first == NULL && second != NULL)
        return -1;
    if (first != NULL && second == NULL)
        return 1;

    Property *a = (Property *)first;
    Property *b = (Property *)second;

    char *a1 = printProperty((void *)a);
    char *b1 = printProperty((void *)b);

    int returnVal = strcmp(a1, b1);
    free(a1);
    free(b1);
    return returnVal;
}

/** Allocates a string and copies the property data that is to be printed to the allocated string
 * @pre toBePrinted property must be allocated
 * @param toBePrinted is the property that is to be printed 
 * @return returns a pointer to the string that holds all the property data that is to be printed
 **/
char *printProperty(void *toBePrinted)
{
    if (toBePrinted == NULL)
        return NULL;

    Property *propToPrint = (Property *)toBePrinted;
    char *toPrint = NULL;

    //Header and Tags
    char header[150] = CYAN "\n\n###########################################\n             Property Struct\n###########################################" RESET "\n";
    char groupTag[20] = ">>> Group: ";
    char nTag[50] = UGREEN ">>> Name: ";
    char paramTag[100] = RESET CYAN "\n***************************\n    PARAMETERS\n***************************" RESET;
    char pValTag[100] = CYAN "\n***************************\n    PROP VALUES\n***************************" RESET;

    //lengths of header and tags
    int headerLen = strlen(header);
    int groupTagLen = strlen(groupTag);
    int nTagLen = strlen(nTag);
    int paramTagLen = strlen(paramTag);
    int pValTagLen = strlen(pValTag);

    int groupLen = 0, nameLen = 0, propParamsLen = 0, propValuesLen = 0;
    char *propParams = NULL, *propValues = NULL;

    //getting strings and length of string on what is allocated in the property struct
    propParams = toString(propToPrint->parameters);
    if (propParams)
        propParamsLen = strlen(propParams);
    propValues = toString(propToPrint->values);
    if (propValues)
        propValuesLen = strlen(propValues);
    groupLen = strlen(propToPrint->group);
    nameLen = strlen(propToPrint->name);

    //allocating a string of total length needed to print the property struct
    int totalLen = headerLen + groupTagLen + nTagLen + groupLen + paramTagLen + pValTagLen + nameLen + propParamsLen + propValuesLen + 10;
    toPrint = calloc(totalLen, sizeof(char));
    if (toPrint == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        free(propParams);
        free(propValues);
        return NULL;
    }

    //concatenates the proper string
    strncpy(toPrint, header, headerLen);
    strncat(toPrint, groupTag, groupTagLen);
    strncat(toPrint, propToPrint->group, groupLen);
    strcat(toPrint, "\n");
    strncat(toPrint, nTag, nTagLen);
    strncat(toPrint, propToPrint->name, nameLen);
    strcat(toPrint, "\n");

    strncat(toPrint, paramTag, paramTagLen);
    if (propParams)
        strncat(toPrint, propParams, propParamsLen);
    strcat(toPrint, "\n");

    strncat(toPrint, pValTag, pValTagLen);
    if (propValues)
        strncat(toPrint, propValues, propValuesLen);

    free(propParams);
    free(propValues);
    return toPrint;
}

/** Deallocates all the memory associated with the parameter and the parameter itself
 * @pre paramter toBeDeleted is not NULL and has memory allocated to it
 * @param toBeDeleted is the parameter that is to be deleted
 **/
void deleteParameter(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
        return;

    Parameter *paramToBeDeleted = (Parameter *)toBeDeleted;
    free(paramToBeDeleted);
    return;
}

/** Compares two parameters to see if they are equal based on the name and value of the parameters
 * @pre first and second parameter must be allocated
 * @param first parameter to be compared
 * @param second parameter to be compared
 * @return if equal: returns 0 else if first > second returns 1 else returns -1
 **/
int compareParameters(const void *first, const void *second)
{
    if (first == NULL && second == NULL)
        return 0;
    if (first == NULL && second != NULL)
        return -1;
    if (first != NULL && second == NULL)
        return 1;

    Parameter *a = (Parameter *)first;
    Parameter *b = (Parameter *)second;

    char *a1 = printParameter((void *)a);
    char *b1 = printParameter((void *)b);

    int returnVal = strcmp(a1, b1);
    free(a1);
    free(b1);
    return returnVal;
}

/** Allocates a string and copies the parameter data that is to be printed to the allocated string
 * @pre toBePrinted paramter must be allocated
 * @param toBePrinted is the parameter that is to be printed 
 * @return returns a pointer to the string that holds all the parameter data that is to be printed
 **/
char *printParameter(void *toBePrinted)
{
    if (toBePrinted == NULL)
        return NULL;

    Parameter *toPrint = (Parameter *)toBePrinted;
    char nameHeader[50] = "* Parameter Name: ";
    char valueHeader[50] = "      Parameter Value: ";
    int nameHeaderLen = strlen(nameHeader);
    int valueHeaderLen = strlen(valueHeader);
    int nameLen = strlen(toPrint->name);
    int valueLen = strlen(toPrint->value);
    int totalLen = nameLen + valueLen + nameHeaderLen + valueHeaderLen + 10;

    char *temp = calloc(totalLen + 1, sizeof(char));
    if (temp == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        return NULL;
    }

    strncpy(temp, nameHeader, nameHeaderLen);
    strncat(temp, toPrint->name, nameLen);
    strncat(temp, valueHeader, valueHeaderLen);
    strncat(temp, toPrint->value, valueLen);
    return temp;
}

/** Deallocates all the memory associated with the property value
 * @pre paramter toBeDeleted is not NULL and has memory allocated to it
 * @param toBeDeleted is the property value that is to be deleted
 **/
void deleteValue(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
        return;

    char *valueToBeDeleted = (char *)toBeDeleted;
    free(valueToBeDeleted);
    return;
}

/** Compares two property values to see if they are equal
 * @pre first and second property values must be allocated
 * @param first value to be compared
 * @param second value to be compared
 * @return if equal: returns 0 else if first > second returns 1 else returns -1
 **/
int compareValues(const void *first, const void *second)
{
    if (first == NULL && second == NULL)
        return 0;
    if (first == NULL && second != NULL)
        return -1;
    if (first != NULL && second == NULL)
        return 1;

    char *a = (char *)first;
    char *b = (char *)second;

    char *a1 = printValue(a);
    char *b1 = printValue(b);
    int returnVal = strcmp(a1, b1);
    free(a1);
    free(b1);
    return returnVal;
}

/** Allocates a string and copies the propert value data that is to be printed to the allocated string
 * @pre toBePrinted property value must be allocated
 * @param toBePrinted is the property value that is to be printed 
 * @return returns a pointer to the string that holds all the property value data that is to be printed
 **/
char *printValue(void *toBePrinted)
{
    if (toBePrinted == NULL)
        return NULL;

    char *temp = (char *)toBePrinted;
    char valueHeader[50] = "* Property Value: ";

    int valueLen = strlen(temp);
    int valueHeaderLen = strlen(valueHeader);
    int totalLen = valueLen + valueHeaderLen + 10;

    char *toPrint = calloc(totalLen + 1, sizeof(char));
    if (toPrint == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        return NULL;
    }

    strcpy(toPrint, valueHeader);
    strcat(toPrint, temp);
    return toPrint;
}

/** Deallocates all the memory associated with the date and its subcomponents
 * @pre dateTime toBeDeleted is not NULL and has memory allocated to it
 * @param toBeDeleted is the dateTime that is to be deleted
 **/
void deleteDate(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
        return;

    DateTime *dateToBeDeleted = (DateTime *)toBeDeleted;
    free(dateToBeDeleted);
    return;
}

/** Compares two dateTime structures to see if they are equal based on the date, time and text elements
 * @pre first and second dateTime structures must be allocated
 * @param first dateTime to be compared
 * @param second dateTime to be compared
 * @return if equal: returns 0 else if first > second returns 1 else returns -1
 **/
int compareDates(const void *first, const void *second)
{
    if (first == NULL && second == NULL)
        return 0;
    if (first == NULL && second != NULL)
        return -1;
    if (first != NULL && second == NULL)
        return 1;

    DateTime *a = (DateTime *)first;
    DateTime *b = (DateTime *)second;

    char *a1 = printDate(a);
    char *b1 = printDate(b);

    int returnVal = strcmp(a1, b1);
    free(a1);
    free(b1);
    return returnVal;
}

/** Allocates a string and copies the dateTime data that is to be printed to the allocated string
 * @pre toBePrinted dateTime data must be allocated
 * @param toBePrinted is the dateTime data that is to be printed 
 * @return returns a pointer to the string that holds all the dateTime data that is to be printed
 **/
char *printDate(void *toBePrinted)
{
    if (toBePrinted == NULL)
        return NULL;

    DateTime *dateToPrint = (DateTime *)toBePrinted;
    char *toPrint = NULL;

    //headers
    char header[150] = "\n" CYAN "###########################################\n             DateTime Struct\n###########################################" RESET "\n";
    char dateHeader[50] = "* Date: ";
    char timeHeader[50] = "* Time: ";
    char textHeader[50] = "* Text: ";
    int headerLen = strlen(header);
    int dateHeaderLen = strlen(dateHeader);
    int timeHeaderLen = strlen(timeHeader);
    int textHeaderLen = strlen(textHeader);
    int LFlen = strlen("\n") * 3;
    int UTCLen = strlen("* NOT UTC\n");
    int isTextLen = strlen("* Is NOT text\n");

    //dateTime Values
    int dateLen = strlen(dateToPrint->date);
    int timeLen = strlen(dateToPrint->time);
    int textLen = 0;
    if (dateToPrint->text)
        textLen = strlen(dateToPrint->text);

    int totalLen = dateLen + timeLen + textLen + headerLen + dateHeaderLen + timeHeaderLen + textHeaderLen + LFlen + UTCLen + isTextLen + 10;
    toPrint = calloc(totalLen + 1, sizeof(char));
    if (toPrint == NULL)
    {
        hModule__printError(__FILE__, __LINE__, "Memory allocation error.");
        return NULL;
    }

    //Copies
    strncpy(toPrint, header, headerLen);

    if (dateToPrint->UTC)
        strcat(toPrint, "* UTC\n");
    else
        strcat(toPrint, "* NOT UTC\n");

    if (dateToPrint->isText)
        strcat(toPrint, "* Is text\n");
    else
        strcat(toPrint, "* Is NOT text\n");

    strncat(toPrint, dateHeader, dateHeaderLen);
    strncat(toPrint, dateToPrint->date, dateLen);
    strcat(toPrint, "\n");
    strncat(toPrint, timeHeader, timeHeaderLen);
    strncat(toPrint, dateToPrint->time, timeLen);
    strcat(toPrint, "\n");
    strncat(toPrint, textHeader, textHeaderLen);
    if (dateToPrint->text)
        strncat(toPrint, dateToPrint->text, textLen);
    strcat(toPrint, "\n");
    return toPrint;
}