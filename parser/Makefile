 ##############################################
 ## 	Name: Neel Gondalia
 ## 	Student ID: 0940429
 ##############################################
 
#compiler variables
CC          =	gcc
CFLAGS      =	-Wall -std=c11 -g

# include directory
Include_Dir = include

#Linked list variables
LinkedList_CFiles = src/LinkedListAPI.c
LinkedList_Object = bin/LinkedListAPI.o
LinkedList_Library = bin/libllist.a

#Parser variables
Parser_CFiles = src/VCardParser.c
Parser_Object = bin/VCardParser.o
Parser_Library = bin/libcparse.a

#helper Module variables
hModule_CFiles = src/HelperModule.c
hModule_Object = bin/HelperModule.o

#All libraries and objects
ALL_Libraries = $(LinkedList_Library) $(Parser_Library)
ALL_Objects = $(LinkedList_Object) $(Parser_Object) $(hModule_Object)

all:
	make list
	make parser
	
parserLib: $(LinkedList_CFiles) $(hModule_CFiles) $(Parser_CFiles) 
	$(CC) $(CFLAGS) -I$(Include_Dir) -fpic -c $(LinkedList_CFiles) -o $(LinkedList_Object)
	$(CC) $(CFLAGS) -I$(Include_Dir) -fpic -c $(hModule_CFiles) -o $(hModule_Object)
	$(CC) $(CFLAGS) -I$(Include_Dir) -fpic -c $(Parser_CFiles) -o $(Parser_Object)
	$(CC) -shared -o ../vCardParser.so $(Parser_Object) $(LinkedList_Object) $(hModule_Object)

	
	
list: $(LinkedList_CFiles)
	$(CC) $(CFLAGS) -I$(Include_Dir) -c $(LinkedList_CFiles) -o $(LinkedList_Object)
	ar cr $(LinkedList_Library) $(LinkedList_Object)

parser: list $(hModule_CFiles) $(Parser_CFiles)
	$(CC) $(CFLAGS) -I$(Include_Dir) -c $(hModule_CFiles) -o $(hModule_Object)
	$(CC) $(CFLAGS) -I$(Include_Dir) -c $(Parser_CFiles) -o $(Parser_Object)
	ar cr $(Parser_Library) $(LinkedList_Object) $(hModule_Object) $(Parser_Object)

clean:
	rm -f $(ALL_Libraries) $(ALL_Objects)

####################################################################################################################################################
#########################################################   TO BE REMOVED BEFORE SUBMISSION   ######################################################
####################################################################################################################################################

#testSuite variables
TestSuite_CFiles = testsuite/testsuite.c testsuite/CardTestUtilities.c
TestSuite_Executable = bin/testSuite
TestSuite_testFile = testsuite/files/testCard.vcf

#Testsuite include directory
TestSuite_Include_Dir = testsuite/include

#documentationLoc
Documentation_Dir = bin/html


test: $(TestSuite_CFiles)
	make all
	$(CC) $(CFLAGS) -I$(Include_Dir) -I$(TestSuite_Include_Dir) $(TestSuite_CFiles) $(ALL_Libraries) -o $(TestSuite_Executable)

valgrind: 
	make tclean
	make test
	valgrind ./$(TestSuite_Executable)

valgrindFull: 
	make tclean
	make test
	valgrind --leak-check=full --track-origins=yes ./$(TestSuite_Executable)

tclean:
	rm -f $(ALL_Libraries) $(ALL_Objects) $(TestSuite_Executable)
	rm -rf $(Documentation_Dir)
	
doxy: Doxyfile
	doxygen
####################################################################################################################################################
####################################################################################################################################################
####################################################################################################################################################
