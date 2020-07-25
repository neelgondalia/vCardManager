/**
 * @file HelperModule.h
 * @author Neel Gondalia (0940429)
 * @date September 2018
 * @brief File containing the function headers of a VCard Parser Helper Module
 */

#ifndef _HELPERMODULE_H
#define _HELPERMODULE_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "LinkedListAPI.h"
#include "VCardParser.h"

#define MAX_INTEGER 30000
#define MAX_STATIC_ROWS 20
#define MAX_STATIC_COLS_PARAMS 20
#define MAX_STATIC_COLS_PROPVALUES 999
#define MAX_STATIC_COLS_PARAMVALUES 100
#define MAX_CONTENT_LINE_SIZE 998
#define DEBUGERROR false
#define RED "\033[31m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define UGREEN "\033[4;32m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

typedef struct propertyDetails
{
    /*
     *Name of the property
     */
    char name[20];

    /*
     * If the property has a single limit to how many property values it can have, then its true. Otherwise, it is false.
     */
    bool strictNumValues;

    /*
     * If strictNumValues is True, then this member is that single limit.
     * If strictNumValues is False, then this member tells you what the max limit of the number of values is. If the limit is infinite it is capped off by MAX_INTEGER.
     */
    int propertyValuesRequired;

    /*
     * Minimum instances of a property
     */
    int minInstancesInCard;

    /*
     * Maximum instances of a property
     */
    int maxInstancesInCard;

    /*
     * The amount of times the propety is found in a particular card
     */
    int inCardCounter;
} propDetails;

/** Check if an allocated string ends with a given suffix
 * @pre strToBeChecked and suffix is not NULL
 * @param strToBeChecked is the string that is to be analyzed for its suffix
 * @param suffix is the suffix to check in the strToBeChecked
 * @return true if suffix is the suffix of strToBeChecked . Otherwise, returns false
 **/
bool hModule__stringEndsWith(char *strToBeChecked, const char *suffix);

/** Check if an allocated string starts with a given prefix
 * @pre strToBeChecked and prefix is not NULL
 * @param strToBeChecked is the string that is to be analyzed for its prefix
 * @param prefix is the prefix to check in the strToBeChecked
 * @return true if prefix is the prefix of strToBeChecked . Otherwise, returns false
 **/
bool hModule__stringStartsWith(char *strToBeChecked, const char *prefix);

/** Parses a file char by char and copies the data into a string that gets analyzed further by getting passed to hModule_fileToVCard function.
 * This function also takes care of any unfolding needed.
 * @pre vcardfile pointer must not be NULL and must already be opened. Also toBeCreated Card object must not be NULL
 * @param vcardfile is the file to be parsed
 * @param toBeCreated is the card object that is to be populated given no errors occur
 * @return VCardErrorCode returns OK if successful parsing and creating of the object. Otherwise returns an appropriate error code.
 **/
VCardErrorCode hModule__parseFile(FILE *vcardfile, Card *toBeCreated);

/** Breaks up the file data into a single content line and passes the content line to hModule_analyzePropertyLine function.
 * @pre Card toBeCreated and contentLines must not be NULL and needs to have memory allocated to it.
 * @param contentLines contains the whole file data
 * @param toBeCreated is the card object that is to be populated given no errors occur
 * @return  VCardErrorCode returns OK if successful analysis of all the content lines. Otherwise returns an appropriate error code.
 **/
VCardErrorCode hModule__fileToVCard(char *contentLines, Card *toBeCreated);

/** Analyzes a content line with the help of split and fillCard functions. The split function splits the content line into group, prop
 * name, parameters, and property values. These split strings gets passed to fill card function, where the card gets populated given no errors occur.
 * @pre lineToParse and toBeCreated are not NULL
 * @param lineToParse contains a single content line
 * @param toBeCreated is the card object that is to be populated given no errors occur
 * @param lineNumber represents the line number of the content line
 * @return  VCardErrorCode returns OK if successful analysis of a single content line. Otherwise returns an appropriate error code.
 **/
VCardErrorCode hModule__analyzePropertyLine(char *lineToParse, Card *toBeCreated, int lineNumber);

/** Helper function of analyzePropertyLine function with the role of splitting a content line into a group, property name, property values and property paramters.
 * @pre lineToParse must not be NULL and have memory allocated to it
 * @param lineToParse contains a single content line to be split
 * @param line_property contains the memory address of the pointer where property name is to be stored
 * @param property_parameters contains the memory address of the pointer where property parameters is to be stored
 * @param property_value contains the memory address of the pointer where the property value is to be stored
 * @param property_group contains the memory address of the pointer where the property group is to be stored
 * @param lineNumber is the line in the file that is being analyzed
 * @return VCardErrorCode returns OK if successful split of a single content line. Otherwise returns an appropriate error code.
 **/
VCardErrorCode hModule__splitStringIntoPropertyParamsAndValues(char *lineToParse, char **line_property, char **property_parameters, char **property_value, char **property_group, int lineNumber);

/** Fills the card object given the group, property name, property params, and property values. This function uses splitPropertyValues to
 * split property values so they can stored in a list. This function also uses splitParamAndParamValues to split property params string into
 * indivual param and param value, which are then stored in a list. 
 * @pre _lproperty, _lparams, _lvalues, _lgroup and toBeCreated Card must not be NULL
 * @param _lproperty is the property name of the content line
 * @param _lgroup is the group of the property of the content line
 * @parma _params is the string that contains parameters and parameter values of a single content line
 * @param _lvalues is the string that contains all the property values of a single content line
 * @param toBeCreated is the card object that is to be populated given no errors occur
 * @param lineNumber is the line in the file that is being analyzed
 * @return VCardErrorCode returns OK if the content line is successfully loaded to the card object. Otherwise returns an appropriate error code.
 **/
VCardErrorCode hModule__fillCard(Card *toBeCreated, char *_lproperty, char *_lparams, char *_lvalues, char *_lgroup, int lineNumber);

/** Checks if a given string is an allowed property
 * @pre _lprop must not be NULL
 * @param _lprop string to be checked if it is a valid property
 * @return VCardErrorCode OK if it is a valid property otherwise INV_PROP
 **/
VCardErrorCode hModule__isPropValid(char *_lprop);

/** Checks if a given string is an allowed parameter
 * @pre _lparam must not be NULL
 * @param _lparam string to be checked if it is a valid paramter
 * @return VCardErrorCode OK if it is a valid parameter otherwise INV_PROP
 **/
VCardErrorCode hModule__isParamValid(char *_lparam);

/** Sorts a string into a parameter names and its parameter values. It also error checks parameter syntax.
 * @pre _lparam must not be NULL 
 * @param  _lparam string that contains all the parameter names and parameter values
 * @param params where all the param names will be stored
 * @param paramsValuePair where all the param values will be stored
 * @param lineNumber is the line in the file that is being analyzed
 * @return VCardErrorCode OK if it is a parameters and parameters values are presented in a proper syntax otherwise INV_PROP or OTHER_ERROR
 **/
VCardErrorCode hModule__sortParamAndParamValues(char *_lparams, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES], int lineNumber);

/** Splits Property values and stores each value individually
 * @pre _lvalues must not be NULL 
 * @param  _lvalues string that contains all the parameter names and parameter values
 * @param propValues where all the property values will be stored
 * @param lineNumber is the line in the file that is being analyzed
 * @return VCardErrorCode OK if all the values are split properly otherwise INV_PROP or OTHER_ERROR 
 **/
VCardErrorCode hModule__splitPropertyValues(char *_lvalues, char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES], int lineNumber);

/** Populates a dateTime struct given the parameter names and values and property values.
 * @pre dtStruct must not be NULL and should have memory allocated to it
 * @param dtStruct is the dateTime struct to be loaded
 * @param params holds the the parameter names of the dateTime property
 * @param paramsValuePair are the parameter values of the params
 * @param propValues holds the property values of the dateTime property
 * @param lineNumber is the line in the file that is being analyzed
 * @return VCardErrorCode returns OK if the dateTime struct is successfully created 
 **/
VCardErrorCode hModule__populateDateTimeStruct(DateTime *dtStruct, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES], char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES], int lineNumber);

/** Populates a property struct given group, name, parameters and values
 * @pre newProp must not be NULL and be allocated. _lproperty and property values must not be empty or NULL
 * @param newProp is the allocated property that needs to be filled with the data provided
 * @param _lgroup is the string that represents the group of a property
 * @param _lproperty is the string that represents the property name. Should not be null or empty
 * @param params is where all the parameters are stored
 * @param paramsValuePair is where all the parameter values are stored
 * @param propValues is where all the property values are stored. One value must exist.
 * @param lineNumber is the line in the file that is being analyzed
 * @return VCardErrorCode returns OK if populating property is successful otherwise returns INV_PROP or OTHER_ERROR
 **/
VCardErrorCode hModule__populateProperty(Property *newProp, char *_lgroup, char *_lproperty, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES], char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES], int lineNumber);

/** Adds parameter and parameter values to the given list that stores objects of type parameter. This function adds the parameters of a single content line.
 * @pre list must not be NULL and be initialized
 * @param list is the list where the parameter structs are added
 * @param params are the parameter names that are to be added to the parameter struct which is then added to the list
 * @param paramsValuePair are the parameter values that are to be added to the parameter struct which is then added to the list
 * @return VCardErrorCode returns OK if all the parameters are successfully loaded to the list
 **/
VCardErrorCode hModule__addParamsToList(List *list, char params[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMS], char paramsValuePair[MAX_STATIC_ROWS][MAX_STATIC_COLS_PARAMVALUES]);

/** Adds property values to the given list that stores char pointers. This function adds the property values of a single content line or property.
 * @pre list must not be NULL and be initialized
 * @param list is the list where the char pointers representing property values are added
 * @param propValues are the property values that are added to the list
 * @return VCardErrorCode returns OK if all the parameters are successfully loaded to the list
 **/
VCardErrorCode hModule__addPropValuesToList(List *list, char propValues[MAX_STATIC_ROWS][MAX_STATIC_COLS_PROPVALUES]);

/** converts the string to all uppercase
 * @pre str must not be NULL and should have memory allocated to it
 * @param str to be converted to uppercase
 **/
void hModule__upperString(char *str);

/** converts the string to all lowercase
 * @pre str must not be NULL and should have memory allocated to it
 * @param str to be converted to lowercase
 **/
void hModule__lowerString(char *str);

/** Prints the error location, line, and message
 * @pre fileName and message must not be NULL and should have memory allocated to it
 * @param lineNum is the line number where the error occured
 * @param fileName is the file in which the error occured
 * @param message is the error message
 */
void hModule__printError(char *fileName, int lineNum, char *message);

/** Checks if the string is alpha numeric or dash
 * @pre group must not be NULL and should have memory allocated to it 
 * @param group name to be anlyzed
 * @return true if group is alpha numeric or dash and false otherwise
 */
bool hModule__isAlphaNumericOrDash(char *group);

/** Checks if the date and or time string is numeric or dash
 * @pre datetimestr must not be NULL and should have memory allocated to it 
 * @param datetimestr is the date and or time string to be anlyzed
 * @return true if datetimestr is numeric or dash and false otherwise
 */
bool hModule__isNumericOrDash(char *datetimestr);

/** Validates Time
 * @pre tempTime is not NULL and has memory allocated to it
 * @param tempTime is the string that represents a time
 * @return true if valid time and false if invalid time
 */
bool hModule__isTimeValid(char *tempTime);

/** Validates Date
 * @pre tempDate is not NULL and has memory allocated to it
 * @param tempDate is the string that represents a date
 * @return true if valid date and false if invalid date
 */
bool hModule__isDateValid(char *tempDate);

/*
 *HELPER FUNCTIONS FOR WRITE CARD. Which also calls validate card
 */

/** Allocates a string that contains a content line to be written to the file
 * @pre prop must not be NULL and must have memory allocated to it
 * @param str address of the pointer where the allocated string is to be stored
 * @param prop is the property to be converted to a content line
 * @return appropriate error code
 */
VCardErrorCode hModuleV2_getPropertyLineFromCard(Property *prop, char **str);

/** Allocates a string that contains a content line to be written to the file. This is for datetime structs only.
 * @pre dt must not be NULL
 * @param str address of the pointer where the allocated string is to be stored
 * @param dt is the datetime to be converted to a content line
 * @param bdayOrAnniv 0 represents bday and 1 represents anniversary
 * @return appropriate error code
 */
VCardErrorCode hModuleV2_getdateTimePropertyLineFromCard(DateTime *dt, char **str, int bdayOrAnniv);

/** Allocates a string and copies a string that contains escape characters to it.
 * @pre str must not be NULL
 * @param str is the string that needs escape characters added to it
 * @param postEscStr is the string that is the postescape string
 * @post postEscStr is not NULL and has memory allocated to it
 */
void hModuleV2__addEscapeChar(char *str, char **postEscStr);

/*
 *HELPER FUNCTIONS FOR WRITE CARD
 */

/** List helper function to delete propDetails Struct
 * @param toBeDeleted is the struct to be deleted
 */
void hModuleV2__deletePropDetails(void *toBeDeleted);

/** List helper function to compare propDetails Structs
 * @param first propDetails struct to be compared
 * @param second propDetails struct to be compared
 */
int hModuleV2__comparePropDetails(const void *first, const void *second);

/** List helper function to print propDetails Struct
 * @param toBePrinted is the propDetails struct to be printed
 * @return string to be printed
 */
char *hModuleV2__printPropDetails(void *toBePrinted);

/** initializes all properties details
 * @param list is the list to be filled
 * @return appropriate error code
 */
VCardErrorCode hModuleV2__initializeAllPropertiesDetails(List *list);

VCardErrorCode hModuleV2__isPropStructValid(Property *p);

VCardErrorCode hModuleV2__isDateTimeStructValid(DateTime *dt);

VCardErrorCode hModuleV2__getPropDetailsFromList(List *list, propDetails **p, char *propName);

VCardErrorCode hModuleV2__validatePropertyValuesAndUpdateCounter(List *list, propDetails *p, Property *prop);

VCardErrorCode hModuleV2__validateTheFullCard(const Card *obj);

VCardErrorCode hModuleV2__cardinalityCheckerForAllProperties(List *allprop);

//JSON CONVERSION
void hModuleV2__convertJSONToStrList(List **list, const char *str);
void hModuleV2__convertJSONtoProp(Property **prop, const char *str);
void hModuleV2__convertJSONtoDT(DateTime **dt, const char *str);
void hModuleV2__convertJSONtoCard(Card **obj, const char *str);

//TO JSON CONVERSION
void hModuleV2__convertStrListToJSON(const List *list, char **allocatedJSONstr);
void hModuleV2__convertPropToJSON(const Property *prop, char **str);
void hModuleV2__convertDtToJSON(const DateTime *prop, char **str);

char *createFileLogJSONString(char *fileName);
char *createCardViewJSONString(char *fileName);
#endif