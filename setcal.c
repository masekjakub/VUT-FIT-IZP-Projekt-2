/****************************************************************************/
/*                              IZP Projekt 2                               */
/*                                                                          */
/*      Kuchař Vojtěch      xkucha30                    Název: setcal       */
/*      Mašek Jakub         xmasek19                                        */
/*      Polomski Filip      xpolom00                                        */
/*      Zelenák Martin      xzelen27                                        */
/*                                                                          */
/*                  Datum: 3.12.2021        Verze: 1.12                     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define countOfCommands 19
typedef struct
{
    char type; //U - universum, S - set, R -relation C - command
    int size;  //Number of chars in content.
    char *content;
} set_t;

//Prototypes of functions
void dealloc(set_t **rows, int size);
char **splitSetContentToArray(char **arrayOfElements, char content[], int *size);
char **splitRelContentToArray(char **arrayOfElements, char content[], int *size);
int myMinus(set_t *rows, int indexOfCommand, char **rowArray1, char **rowArray2, int rowArray1Size, int rowArray2Size);
void deallocArrayOfStrings(char **array, int arraySize);

//Printing of one line
void printRow(set_t row)
{
    if (row.size == 1)
    { //Printing of empty universum, set or relation.
        printf("%c\n", row.type);
    }
    else
    {
        printf("%c %s\n", row.type, row.content);
    }
}

//Loading and partially (only U, S, R) printing data.
int readFromFile(set_t **rows, int *sizeOfArrayP, char *filename, int *indexOfFirstCommand)
{
    *rows = malloc(sizeof(set_t));
    if ((*rows) == NULL)
    {
        fprintf(stderr, "ERROR: Allocation error!\n");
        return 1;
    }
    void *tmpPtr = NULL;
    int sizeOfArray = *sizeOfArrayP;
    int universumFound = 0;
    int setRelFound = 0;
    int commandFound = 0;
    int position = 0;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: File not found or failed to open!\n");
        return 1;
    }
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        if (sizeOfArray == 1000)
        { //Checks if 1000 lines were loaded.
            fprintf(stderr, "ERROR: Entered more than 1000 lines!\n");
            fclose(file);
            dealloc(rows, sizeOfArray);
            return 1;
        }
        if (c == '\n')
        { //Checks if there (in txt file) is an empty line present anywhere except the last line.
            fprintf(stderr, "ERROR: Empty line can only be as last line!\n");
            fclose(file);
            dealloc(rows, sizeOfArray);
            return 1;
        }

        if (c != 'U' && c != 'S' && c != 'R' && c != 'C')
        { //Checks for invalid chars as type of line.
            fprintf(stderr, "ERROR: Invalid type of line!\n");
            fclose(file);
            dealloc(rows, sizeOfArray);
            return 1;
        }

        sizeOfArray++;
        tmpPtr = realloc(*rows, sizeOfArray * sizeof(set_t));
        if (tmpPtr == NULL)
        { //Checks if array of structures reallocation was successful.
            fprintf(stderr, "ERROR: Allocation error!\n");
            fclose(file);
            dealloc(rows, sizeOfArray);
            return 1;
        }
        *rows = tmpPtr;

        (*rows)[sizeOfArray - 1].type = c; //Saves the first char of line as its type (U, S, R, C)
        (*rows)[sizeOfArray - 1].size = 0;
        (*rows)[sizeOfArray - 1].content = malloc(sizeof(char));
        if ((*rows)[sizeOfArray - 1].content == NULL)
        {
            fprintf(stderr, "ERROR: rows[%d].content NOT allocated!\n", sizeOfArray - 1);
            return 1;
        }

        //Check second char (valid type of line is 1 char)
        c = fgetc(file);
        if (c != ' ' && c != '\n')
        {
            fprintf(stderr, "ERROR: Invalid type of line!\n");
            fclose(file);
            dealloc(rows, sizeOfArray);
            return 1;
        }
        if (c == '\n')
        { //Checks if third char of line is end of line => is empty set or relation
            (*rows)[sizeOfArray - 1].size++;
            tmpPtr = realloc((*rows)[sizeOfArray - 1].content, (*rows)[sizeOfArray - 1].size * sizeof(char));
            if (tmpPtr == NULL)
            {
                fprintf(stderr, "ERROR: rows[%d].content NOT reallocated!\n", sizeOfArray - 1);
                return 1;
            }
            (*rows)[sizeOfArray - 1].content = tmpPtr;
            (*rows)[sizeOfArray - 1].content[0] = '\0';
        }

        //Loading the rest of line (content after type).
        position = 0;
        while (c != '\n' && c != EOF)
        {
            c = fgetc(file);
            //Checking valid line types and content chars.
            switch ((*rows)[sizeOfArray - 1].type)
            {
            case 'U':
            case 'S':
                if (!(c >= 'A' && c <= 'Z') && !(c >= 'a' && c <= 'z') && c != ' ' && c != '\n')
                {
                    fprintf(stderr, "ERROR: Invalid char '%c' found at line %d!\n", c, sizeOfArray); //Error if character is invalid.
                    fclose(file);
                    dealloc(rows, sizeOfArray);
                    return 1;
                }
                break;
            case 'R':
                if (!(c >= 'A' && c <= 'Z') && !(c >= 'a' && c <= 'z') && c != ' ' && c != '(' && c != ')' && c != '\n')
                {
                    fprintf(stderr, "ERROR: Invalid char '%c' found at line %d!\n", c, sizeOfArray); //Error if character is invalid.
                    fclose(file);
                    dealloc(rows, sizeOfArray);
                    return 1;
                }
                break;
            case 'C':
                if (!(c >= 'A' && c <= 'Z') && !(c >= 'a' && c <= 'z') && !(c >= '0' && c <= '9') && c != ' ' && c != '\n')
                {
                    fprintf(stderr, "ERROR: Invalid char '%c' found at line %d!\n", c, sizeOfArray); //Error if character is invalid.
                    fclose(file);
                    dealloc(rows, sizeOfArray);
                    return 1;
                }
                break;
            default:
                fprintf(stderr, "ERROR: Invalid line type '%c'!\n", (*rows)[sizeOfArray - 1].type); //Error if invalid type of line.
                fclose(file);
                dealloc(rows, sizeOfArray);
                return 1;
            }
            (*rows)[sizeOfArray - 1].size++;
            tmpPtr = realloc((*rows)[sizeOfArray - 1].content, (*rows)[sizeOfArray - 1].size * sizeof(char));
            if (tmpPtr == NULL)
            { //Checks if reallocations were successful.
                fprintf(stderr, "ERROR: rows[%d].content NOT reallocated!\n", sizeOfArray - 1);
                fclose(file);
                dealloc(rows, sizeOfArray);
                return 1;
            }
            (*rows)[sizeOfArray - 1].content = tmpPtr;
            (*rows)[sizeOfArray - 1].content[position] = c;

            //Replaces \n for \0 (end of line for end of string).
            if (c == '\n')
            {
                (*rows)[sizeOfArray - 1].content[position] = '\0';
            }
            position++;
        }

        //Input check, partial print and finding firstIndexOfCommand.
        if ((*rows)[sizeOfArray - 1].type == 'U')
        {
            if (!universumFound)
            {
                universumFound = 1;
            }
            else
            {
                fprintf(stderr, "ERROR: More than 1 universum found!\n");
                fclose(file);
                dealloc(rows, sizeOfArray);
                return 1;
            }
        }
        else if (!universumFound) //Checks whether or not the universum is in the first row.
        {
            fprintf(stderr, "ERROR: Universum must be first!\n");
            fclose(file);
            dealloc(rows, sizeOfArray);
            return 1;
        }
        if ((*rows)[sizeOfArray - 1].type == 'S' || (*rows)[sizeOfArray - 1].type == 'R') //Checks if any set or relatoin has been found
        {
            setRelFound = 1;
        }
        if ((*rows)[sizeOfArray - 1].type == 'C')
        {
            if (!commandFound)
            {
                commandFound = 1;
                *indexOfFirstCommand = sizeOfArray - 1;
            }
        }
        else //Error if anything else then a command appears in txt file after the first C
        {
            if (commandFound)
            {
                fprintf(stderr, "ERROR: Only commands can be entered after the first command!\n");
                fclose(file);
                dealloc(rows, sizeOfArray);
                return 1;
            }
            printRow((*rows)[sizeOfArray - 1]);
        }
    }
    fclose(file);

    if (!setRelFound)
    {
        fprintf(stderr, "ERROR: You must enter atleast 1 set or relation!\n");
        dealloc(rows, sizeOfArray);
        return 1;
    }
    if (!commandFound)
    {
        fprintf(stderr, "ERROR: You must enter atleast 1 command!\n");
        dealloc(rows, sizeOfArray);
        return 1;
    }

    *sizeOfArrayP = sizeOfArray;
    return 0;
}

//Fuction for deallocating loaded data.
void dealloc(set_t **rows, int sizeOfArray)
{
    for (int i = 0; i < sizeOfArray; i++)
    {
        free((*rows)[i].content);
    }
    free(*rows);
}

//Function: Rewrites command line by the line of set/relation.
//Output: 0 if success, 4 if allocation error.
int saveResult(set_t *rows, int rowIndex, char type, char **newContent, int newContentSize)
{
    rows[rowIndex].type = type;   //Rewrites row type  from 'C' to 'S' or 'R'.
    free(rows[rowIndex].content); //Frees original content of the row.
    void *tmpPtr = NULL;
    if (newContentSize > 0)
    {
        rows[rowIndex].size = strlen(newContent[0]) + 1; //Saves new size of content
        rows[rowIndex].content = malloc(rows[rowIndex].size * sizeof(char));
        if (rows[rowIndex].content == NULL)
        {
            return 4;
        }
        strcpy(rows[rowIndex].content, newContent[0]); //Saves new content
        for (int i = 1; i < newContentSize; i++)
        {
            rows[rowIndex].size += strlen(newContent[i]) + 1;
            tmpPtr = realloc(rows[rowIndex].content, rows[rowIndex].size);
            if (tmpPtr == NULL)
            {
                return 4;
            }
            rows[rowIndex].content = tmpPtr;
            strcat(rows[rowIndex].content, " ");
            strcat(rows[rowIndex].content, newContent[i]);
        }
    }
    else
    {
        rows[rowIndex].content = malloc(sizeof(char));
        if (rows[rowIndex].content == NULL)
        {
            return 4;
        }
        rows[rowIndex].content[0] = '\0';
    }
    return 0;
}

//Function: Checks validity of Universum.
//Output: 0 if not valid, 1 if valid.
int checkUniversum(set_t *rows, char **commandList)
{
    char **universum = NULL;
    int numberOfElements;
    universum = splitSetContentToArray(universum, rows[0].content, &numberOfElements);
    if (universum == NULL)
    {
        fprintf(stderr, "ERROR: Allocation error\n");
        return 0;
    }

    //Checks if element is shorter than 30 characters.
    for (int i = 0; i < numberOfElements; i++)
    {
        if (strlen(universum[i]) > 30)
        {
            fprintf(stderr, "ERROR: Universum has an element longer than 30 chars \"%s\"!\n", universum[i]);
            deallocArrayOfStrings(universum, numberOfElements);
            return 0;
        }
    }

    //Checks if element is not forbidden string.
    for (int i = 0; i < numberOfElements; i++)
    {
        for (int j = 0; j < countOfCommands; j++)
        {
            if (strcmp(universum[i], commandList[j]) == 0)
            {
                fprintf(stderr, "ERROR: Universum has a forbidden element \"%s\"!\n", universum[i]);
                deallocArrayOfStrings(universum, numberOfElements);
                return 0;
            }
        }
        if (strcmp(universum[i], "true") == 0)
        {
            fprintf(stderr, "ERROR: Universum has a forbidden element \"%s\"!\n", universum[i]);
            deallocArrayOfStrings(universum, numberOfElements);
            return 0;
        }
        if (strcmp(universum[i], "false") == 0)
        {
            fprintf(stderr, "ERROR: Universum has a forbidden element \"%s\"!\n", universum[i]);
            deallocArrayOfStrings(universum, numberOfElements);
            return 0;
        }
    }

    //Check repetitions of elements.
    int hasMultiple = 0;
    for (int i = 0; i < numberOfElements; i++)
    {
        for (int j = 0; j < numberOfElements; j++)
        {
            if (i != j && strcmp(universum[i], universum[j]) == 0)
            {
                hasMultiple = 1;
                break;
            }
        }
        if (hasMultiple)
        {
            fprintf(stderr, "ERROR: Universum has a repeating element \"%s\"!\n", universum[i]);
            deallocArrayOfStrings(universum, numberOfElements);
            return 0;
        }
    }

    deallocArrayOfStrings(universum, numberOfElements);
    return 1;
}

//Function: Checks the length and repetition of elements. Also checks if the set/relation elements appear in universum.
//Output: 1 if valid, 0 if invalid.
int checkSetAndRelations(set_t *rows, int firstIndexOfCommand, int sizeOfArray, char **commandList)
{
    char **universum = NULL;
    int universumSize;
    universum = splitSetContentToArray(universum, rows[0].content, &universumSize);
    if (universum == NULL)
    {
        fprintf(stderr, "ERROR: Allocation error!\n");
        return 0;
    }
    char **rowArray = NULL;
    int rowArraySize = 0;
    int lastRowIndex = 0;
    if (firstIndexOfCommand != 0)
    {
        lastRowIndex = firstIndexOfCommand;
    }
    else
    {
        lastRowIndex = sizeOfArray;
    }

    //Loading all lines of sets and relations.
    for (int i = 1; i < lastRowIndex; i++)
    {
        rowArray = NULL;
        if (rows[i].type == 'S')
        {
            rowArray = splitSetContentToArray(rowArray, rows[i].content, &rowArraySize);
        }
        else
        {
            rowArray = splitRelContentToArray(rowArray, rows[i].content, &rowArraySize);
        }

        if (rowArray == NULL)
        {
            fprintf(stderr, "ERROR: Allocation error!\n");
            deallocArrayOfStrings(universum, universumSize);
            return 0;
        }

        //Checks the length of element.
        for (int j = 0; j < rowArraySize; j++)
        {
            if (strlen(rowArray[j]) > 30)
            {
                fprintf(stderr, "ERROR: Element \"%s\" is longer than 30 chars at line %d!\n", rowArray[j], i + 1);
                deallocArrayOfStrings(rowArray, rowArraySize);
                deallocArrayOfStrings(universum, universumSize);
                return 0;
            }
        }

        //Checks if element is forbidden.
        for (int i = 0; i < rowArraySize; i++)
        {
            for (int j = 0; j < countOfCommands; j++)
            {
                if (strcmp(rowArray[i], commandList[j]) == 0)
                {
                    fprintf(stderr, "ERROR: Universum has a forbidden element \"%s\"!\n", rowArray[i]);
                    deallocArrayOfStrings(rowArray, rowArraySize);
                    deallocArrayOfStrings(universum, universumSize);
                    return 0;
                }
            }
            if (strcmp(rowArray[i], "true") == 0)
            {
                fprintf(stderr, "ERROR: Universum has a forbidden element \"%s\"!\n", rowArray[i]);
                deallocArrayOfStrings(rowArray, rowArraySize);
                deallocArrayOfStrings(universum, universumSize);
                return 0;
            }
            if (strcmp(rowArray[i], "false") == 0)
            {
                fprintf(stderr, "ERROR: Universum has a forbidden element \"%s\"!\n", rowArray[i]);
                deallocArrayOfStrings(rowArray, rowArraySize);
                deallocArrayOfStrings(universum, universumSize);
                return 0;
            }
        }

        //Checks repetitions.
        if (rows[i].type == 'S')
        {
            int isTwice = 0;
            for (int j = 0; j < rowArraySize; j++)
            {
                isTwice = 0;
                for (int k = 0; k < rowArraySize; k++)
                {
                    if (j != k && strcmp(rowArray[j], rowArray[k]) == 0)
                    {
                        isTwice = 1;
                        break;
                    }
                }
                if (isTwice)
                {
                    fprintf(stderr, "ERROR: Element \"%s\" is used atleast twice in set at line %d!\n", rowArray[j], i + 1);
                    deallocArrayOfStrings(rowArray, rowArraySize);
                    deallocArrayOfStrings(universum, universumSize);
                    return 0;
                }
            }
        }
        else //Checks repetitions of string pairs in relation.
        {
            int isTwice = 0;
            for (int j = 0; j < rowArraySize; j += 2)
            {
                isTwice = 0;
                for (int k = 0; k < rowArraySize; k += 2)
                {
                    if (j != k && strcmp(rowArray[j], rowArray[k]) == 0 && strcmp(rowArray[j + 1], rowArray[k + 1]) == 0)
                    {
                        isTwice = 1;
                        break;
                    }
                }
                if (isTwice)
                {
                    fprintf(stderr, "ERROR: Relation \"(%s %s)\" is used atleast twice in relations at line %d!\n", rowArray[j], rowArray[j + 1], i + 1);
                    deallocArrayOfStrings(rowArray, rowArraySize);
                    deallocArrayOfStrings(universum, universumSize);
                    return 0;
                }
            }
        }

        //If set is not empty.
        if (*rowArray[0] != '\0')
        {
            //Checks that elements from all the lines appear in the universum as well.
            for (int j = 0; j < rowArraySize; j++)
            {
                int isEqual = 0;
                for (int k = 0; k < universumSize; k++)
                {
                    if (strcmp(rowArray[j], universum[k]) == 0)
                    {
                        isEqual = 1;
                        break;
                    }
                }
                if (isEqual == 0)
                {
                    fprintf(stderr, "ERROR: Element %s at row %d is not in Universum\n", rowArray[j], i + 1);
                    deallocArrayOfStrings(rowArray, rowArraySize);
                    deallocArrayOfStrings(universum, universumSize);
                    return 0;
                }
            }
        }
        deallocArrayOfStrings(rowArray, rowArraySize);
    }
    deallocArrayOfStrings(universum, universumSize);
    return 1;
}

//Function: Checks if command is valid (based on used lines - sets/relations).
//Output: 0 if invalid, 1 if valid
int isOperationValidWithTypeOfLine(int idOfCommand, char type, int argumentCount, int idOfArgument)
{
    switch (idOfCommand)
    {
    case 0 ... 2:
        if (argumentCount == 1 && (type == 'U' || type == 'S'))
            return 1;
        break;

    case 3 ... 8:
        if (argumentCount == 2 && (type == 'U' || type == 'S'))
            return 1;
        break;

    case 9 ... 15:
        if (argumentCount == 1 && type == 'R')
            return 1;
        break;

    case 16 ... 18:
        if (argumentCount == 3)
        {
            if (idOfArgument == 0 && type == 'R')
                return 1;
            if (idOfArgument > 0 && (type == 'U' || type == 'S'))
                return 1;
        }
        break;
    }
    return 0;
}

//Function 0: empty A - Prints true or false depending on emptiness of set.
//Output: Prints result on stdout, returns 0 if run of function was successful.
int myEmpty(set_t *rows, int row, int row2)
{
    if (rows[row].type != 'S' && rows[row].type != 'U')
    {
        return 2;
    }

    if (row < 0 || row2 != -1)
    {
        return 3;
    }

    if (rows[row].size == 1)
    {
        printf("true\n");
    }
    else
    {
        printf("false\n");
    }
    return 0;
}

//Function 1: card A - Prints number of elements in set.
//Output: Prints result on stdout, returns 0 if run was successful.
int myCard(char **firstRowArray, int rowArraySize)
{
    if (*firstRowArray[0] == '\0')
    {
        rowArraySize = 0;
    }
    printf("%d\n", rowArraySize);
    return 0;
}

//Function 2: complement A - Prints complement of set.
//Output: Prints result on stdout, returns 0 if run was successful.
int myComplement(set_t *rows, int indexOfCommand, char **rowArray, int rowArraySize)
{
    char **universum = NULL;
    int universumSize = 0;
    universum = splitSetContentToArray(universum, rows[0].content, &universumSize);
    if (universum == NULL)
    {
        deallocArrayOfStrings(rowArray, rowArraySize);
        return 4;
    }

    int error = 0;
    error = myMinus(rows, indexOfCommand, universum, rowArray, universumSize, rowArraySize);
    deallocArrayOfStrings(universum, universumSize);
    return error;
}

//Funkction 3: union A B - Prints union of set A and B.
//Output: Prints union of sets A and B and writes it to set_t *rows.
int myUnion(set_t *rows, int indexOfCommand, char **rowArray1, char **rowArray2, int rowArray1Size, int rowArray2Size)
{
    char **elementsOfUnion = malloc(sizeof(char *));
    if (elementsOfUnion == NULL)
    {
        return 4;
    }
    int numberOfElements = 0;
    void *tmpPtr = NULL;

    for (int i = 0; i < rowArray1Size; i++) //Loads first line into union array.
    {
        tmpPtr = realloc(elementsOfUnion, (numberOfElements + 1) * sizeof(char *));
        if (tmpPtr == NULL)
        {
            return 4;
        }
        elementsOfUnion = tmpPtr;
        if (*rowArray1[i] != '\0')
        {
            elementsOfUnion[i] = rowArray1[i];
            numberOfElements++;
        }
    }

    if (numberOfElements == 0)
    {
        elementsOfUnion[0] = malloc(sizeof(char));
        if (elementsOfUnion[0] == NULL)
        {
            free(elementsOfUnion);
            return 4;
        }
        *elementsOfUnion[0] = '\0';
    }

    int isInUnion = 0;
    for (int i = 0; i < rowArray2Size; i++) //Compares union-elements with secondrow-elements and adds new ones.
    {
        isInUnion = 0;
        for (int j = 0; j < numberOfElements; j++)
        {
            if (strcmp(rowArray2[i], elementsOfUnion[j]) == 0)
            {
                isInUnion = 1;
                break;
            }
        }
        if (!isInUnion && *rowArray2[i] != '\0') //Adds new union.
        {
            numberOfElements++;
            tmpPtr = realloc(elementsOfUnion, numberOfElements * sizeof(char *));
            if (tmpPtr == NULL)
            {
                return 4;
            }
            elementsOfUnion = tmpPtr;
            elementsOfUnion[numberOfElements - 1] = rowArray2[i];
        }
    }

    int error = 0;
    error = saveResult(rows, indexOfCommand, 'S', elementsOfUnion, numberOfElements);
    if (error == 0)
    {
        printRow(rows[indexOfCommand]);
    }
    free(elementsOfUnion);
    return error;
}

//Function 4: intersect A B - Prints intersect of sets A and B.
//Output: Prints result on stdout, returns 0 if run was successful.
int myIntersect(set_t *rows, int indexOfCommand, char **rowArray1, char **rowArray2, int rowArraySize1, int rowArraySize2)
{
    char **resultArray = NULL;
    int resultArraySize = 0;
    void *tmpPtr = NULL;
    resultArray = malloc(sizeof(char *));
    if (resultArray == NULL)
    {
        return 4;
    }
    int sameElementFound = 0;
    for (int i = 0; i < rowArraySize1; i++)
    {
        sameElementFound = 0;
        for (int j = 0; j < rowArraySize2; j++)
        {
            if (!strcmp(rowArray1[i], rowArray2[j]))
            {
                sameElementFound = 1;
                break;
            }
        }
        if (sameElementFound) //If element of both sets appears, its printed on stdout.
        {
            resultArraySize++;
            tmpPtr = realloc(resultArray, resultArraySize * sizeof(char *));
            if (tmpPtr == NULL)
            {
                return 4;
            }
            resultArray = tmpPtr;
            resultArray[resultArraySize - 1] = rowArray1[i];
        }
    }
    int error = 0;
    error = saveResult(rows, indexOfCommand, 'S', resultArray, resultArraySize);
    if (error == 0)
    {
        printRow(rows[indexOfCommand]);
    }
    free(resultArray);
    return error;
}

//Function 5: minus A B - Prints difference of A and B sets.
//Output: Prints result on stdout, returns 0 if run was successful, 4 if allocation error.
int myMinus(set_t *rows, int indexOfCommand, char **rowArray1, char **rowArray2, int rowArray1Size, int rowArray2Size)
{
    char **resultArray = NULL;
    resultArray = malloc(sizeof(char *));
    if (resultArray == NULL)
    {
        return 4;
    }
    resultArray[0] = malloc(sizeof(char));
    if (resultArray[0] == NULL)
    {
        free(resultArray);
        return 4;
    }
    *resultArray[0] = '\0';
    void *tmpPtr = NULL;
    int resultArraySize = 0;
    int sameElementFound = 0;
    for (int i = 0; i < rowArray1Size; i++)
    {
        sameElementFound = 0;
        for (int j = 0; j < rowArray2Size; j++)
        {
            if (!strcmp(rowArray1[i], rowArray2[j]))
            {
                sameElementFound = 1;
                break;
            }
        }
        if (!sameElementFound)
        {
            resultArraySize++;
            tmpPtr = realloc(resultArray, resultArraySize * sizeof(char *));
            if (tmpPtr == NULL)
            {
                free(resultArray);
                return 4;
            }
            resultArray = tmpPtr;

            if ((resultArraySize - 1) == 0) //Checks if new element is rewriting allocated space (\0).
            {
                free(resultArray[0]);
            }
            resultArray[resultArraySize - 1] = rowArray1[i];
        }
    }

    int error = 0;
    error = saveResult(rows, indexOfCommand, 'S', resultArray, resultArraySize);
    if (error == 0)
    {
        printRow(rows[indexOfCommand]);
    }
    free(resultArray);
    return error;
}

//Function 6: subseteq A B - Prints true or false depending on whether or not is set A subset of set B.
//Output: Prints result on stdout, returns 0 if run was successful.
int mySubseteq(char **rowArray1, char **rowArray2, int rowArraySize1, int rowArraySize2)
{
    int isInSetB = 0;
    if (*rowArray1[0] != '\0')
    {
        for (int i = 0; i < rowArraySize1; i++)
        {
            isInSetB = 0;
            for (int j = 0; j < rowArraySize2; j++)
            {
                if (strcmp(rowArray1[i], rowArray2[j]) == 0)
                {
                    isInSetB = 1;
                    break;
                }
            }
            if (!isInSetB)
            {
                printf("false\n");
                return 0;
            }
        }
    }
    printf("true\n");
    return 0;
}

//Function 7: subseteq A B - Prints true or false depending on whether or not is set A true subset of set B.
//Output: Prints result on stdout, returns 0 if run was successful.
int mySubset(char **rowArray1, char **rowArray2, int rowArraySize1, int rowArraySize2)
{
    int numberOfElementsFound = 0;
    int isInSetB = 0;

    if (*rowArray1[0] == '\0')
    {
        rowArraySize1 = 0;
    }

    if (*rowArray2[0] == '\0')
    {
        rowArraySize2 = 0;
    }

    for (int i = 0; i < rowArraySize1; i++)
    {
        isInSetB = 0;
        for (int j = 0; j < rowArraySize2; j++)
        {
            if (strcmp(rowArray1[i], rowArray2[j]) == 0)
            {
                numberOfElementsFound++;
                isInSetB = 1;
                break;
            }
        }
        if (!isInSetB)
        {
            printf("false\n");
            return 0;
        }
    }
    //If A = B => A is not true subset.
    if (numberOfElementsFound == rowArraySize2)
    {
        printf("false\n");
        return 0;
    }
    printf("true\n");
    return 0;
}

//Function 8: equals A B - Prints true or false, depending on whether or not set A is equal to set B.
//Output: Prints result on stdout, returns 0 if run was successful.
int myEquals(char **rowArray1, char **rowArray2, int rowArraySize1, int rowArraySize2)
{
    int elementFound = 0;

    if (*rowArray1[0] == '\0')
    {
        rowArraySize1 = 0;
    }

    if (*rowArray2[0] == '\0')
    {
        rowArraySize2 = 0;
    }

    if (rowArraySize1 == rowArraySize2)
    {
        for (int i = 0; i < rowArraySize1; i++)
        {
            for (int j = 0; j < rowArraySize2; j++)
            {
                if (!strcmp(rowArray1[i], rowArray2[j]))
                {
                    elementFound = 1;
                    break;
                }
            }
            if (elementFound == 0)
            {
                printf("false\n");
                return 0;
            }
            elementFound = 0;
        }
        printf("true\n");
        return 0;
    }
    printf("false\n");
    return 0;
}

//Function 9: reflexive R - Prints true or false, depending on whether or not relation R is reflexive.
//Output: Prints result on stdout, returns 0 if run was successful.
int myReflexive(set_t *rows, char **rowArray, int rowArraySize)
{
    //Empty universum
    if (rows[0].content[0] == '\0')
    {
        //Empty relation
        if (*rowArray[0] == '\0')
        {
            printf("true\n");
            return 0;
        }
        printf("false\n");
        return 0;
    }
    //Empty relation
    if (*rowArray[0] == '\0')
    {
        printf("false\n");
        return 0;
    }

    char **reflexiveElements = malloc(sizeof(char *)); //example: (foo,foo) -> foo,(boo,boo) -> boo)
    if (reflexiveElements == NULL)
    {
        return 4;
    }

    void *tmpPtr = NULL;
    int refIndex = 0;
    for (int i = 0; i < rowArraySize; i += 2) //Take first element of each relation
    {
        if (strcmp(rowArray[i], rowArray[i + 1]) == 0)
        {
            tmpPtr = realloc(reflexiveElements, (refIndex + 2) * sizeof(char *));
            if (tmpPtr == NULL)
            {
                free(reflexiveElements);
                return 4;
            }
            reflexiveElements = tmpPtr;
            reflexiveElements[refIndex] = rowArray[i];
            refIndex++;
        }
    }

    int isRef = 0;
    for (int i = 0; i < rowArraySize; i++)
    {
        isRef = 0;
        for (int j = 0; j < refIndex; j++)
        {
            if (strcmp(rowArray[i], reflexiveElements[j]) == 0)
            {
                isRef = 1;
                break;
            }
        }
        if (isRef == 0)
        {
            free(reflexiveElements);
            printf("false\n");
            return 0;
        }
    }
    free(reflexiveElements);
    printf("true\n");
    return 0;
}

//Function 10: symmetric A - Prints true or false, depending on whether or not relation R is symetric.
//Output: Prints result on stdout, returns 0 if run was successful.
int mySymmetric(char **rowArray, int rowArraySize)
{
    //Empty relation is always symetric.
    if (*rowArray[0] == '\0')
    {
        printf("true\n");
        return 0;
    }

    int isSymmetric = 0;

    for (int i = 0; i < rowArraySize; i += 2) //Take first element of each relation
    {
        isSymmetric = 0;
        for (int j = 0; j < rowArraySize; j += 2) //Take first element of each relation
        {
            //If pair has an opposite pair -> symetric pair.
            if (strcmp(rowArray[i], rowArray[j + 1]) == 0 && strcmp(rowArray[i + 1], rowArray[j]) == 0)
            {
                isSymmetric = 1;
                break;
            }
        }
        if (isSymmetric == 0)
        {
            printf("false\n");
            return 0;
        }
    }
    printf("true\n");
    return 0;
}

//Function 10: antisymmetric A - Prints true or false, depending on whether or not relation R is antisymetric.
//Output: Prints result on stdout, returns 0 if run was successful.
int myAntisymmetric(set_t *rows, char **rowArray, int rowArraySize)
{
    //Empty universum
    if (rows[0].content[0] == '\0')
    {
        //Empty relation
        if (*rowArray[0] == '\0')
        {
            printf("true\n");
            return 0;
        }
        printf("false\n");
        return 0;
    }
    //Empty relation
    if (*rowArray[0] == '\0')
    {
        printf("true\n");
        return 0;
    }

    for (int i = 0; i < rowArraySize; i += 2) //Take first element of each relation
    {
        for (int j = 0; j < rowArraySize; j += 2) //Take first element of each relation
        {
            //If pair has not an opposite pair -> antisymetric pair.
            if (i != j && strcmp(rowArray[i], rowArray[j + 1]) == 0 && strcmp(rowArray[i + 1], rowArray[j]) == 0)
            {
                printf("false\n");
                return 0;
            }
        }
    }
    printf("true\n");
    return 0;
}

//Function 12: transitive A - Prints true or false, depending on whether or not relation A is transitive.
//Output: Prints result on stdout, returns 0 if run was successful.
int myTransitive(char **rowArray, int rowArraySize)
{
    char **mustContain = NULL;
    int mustContainSize = 0;
    void *tmpPtr = NULL;
    mustContain = malloc(sizeof(char *));
    if (mustContain == NULL)
    {
        return 4;
    }

    for (int i = 1; i < rowArraySize; i += 2) //Take first element of each relation
    {
        for (int j = 0; j < rowArraySize; j += 2) //Take second element of each relation
        {
            if (j != i - 1 && strcmp(rowArray[i], rowArray[j]) == 0) //Search for combination (X Y) (Y Z) and saves (X Z)
            {
                mustContainSize += 2;
                tmpPtr = realloc(mustContain, mustContainSize * sizeof(char *));
                if (tmpPtr == NULL)
                {
                    free(mustContain);
                    return 4;
                }
                mustContain = tmpPtr;
                mustContain[mustContainSize - 2] = rowArray[i - 1]; //First element of pair by i
                mustContain[mustContainSize - 1] = rowArray[j + 1]; //Second element of pair by j
            }
        }
    }

    //Searching for pairs, needed for making a set transitive.
    int isTransitive = 0;
    for (int i = 0; i < mustContainSize; i += 2)
    {
        isTransitive = 0;
        for (int j = 0; j < rowArraySize; j += 2)
        {
            if (i != j && strcmp(mustContain[i], rowArray[j]) == 0 && strcmp(mustContain[i + 1], rowArray[j + 1]) == 0)
            {
                isTransitive = 1;
                break;
            }
        }
        if (isTransitive == 0)
        {
            printf("false\n");
            free(mustContain);
            return 0;
        }
    }
    printf("true\n");
    free(mustContain);
    return 0;
}

//Funkce 13: function A - Prints true or false, depending on whether or not relation A is function.
//Output: Prints result on stdout, returns 0 if run was successful or returns 1 if true or 0 if false. NEBO jen vrací 1(true) nebo 0(false)
//Input: printResult - != 0 -> result printed and returns error, if error is 0 result is not printed and returns 1 if true or 0 if false.
int myIsFunction(char **rowArray, int rowArraySize, int printResult)
{
    int hasOnlyOneY = 1;
    for (int i = 0; i < rowArraySize - 1; i += 2) //Take first element of each relation
    {
        hasOnlyOneY = 1;
        for (int j = 0; j < rowArraySize - 1; j += 2) //Take first element of each relation
        {
            //Searching for combinations (X Y) (X Z) -> relation is on the function
            if (strcmp(rowArray[i], rowArray[j]) == 0 && strcmp(rowArray[i + 1], rowArray[j + 1]) != 0)
            {
                hasOnlyOneY = 0;
                break;
            }
        }
        if (hasOnlyOneY == 0)
        {
            if (printResult)
            {
                printf("false\n");
            }
            return 0;
        }
    }
    if (printResult)
    {
        printf("true\n");
        return 0;
    }
    return 1;
}

//Function 14, 15
//Input: startingRelIndex(0 - definition field, takes odd elements, 1 - range of values, takes even elements)
int myDomainAndCodomain(set_t *rows, int indexOfCommand, char **relArray, int relArraySize, int startingRelIndex)
{
    char **domainArray = malloc(sizeof(char *));
    if (domainArray == NULL)
    {
        return 4;
    }
    int indexRelArray = startingRelIndex;
    int domainIndex = 0;
    void *tmpPtr = NULL;

    if (*relArray[0] == '\0') //Checks if relation is empty
    {
        domainArray[0] = malloc(sizeof(char));
        if (domainArray == NULL)
        {
            return 4;
        }
        *domainArray[0] = '\0';
        domainIndex++;
    }
    else
    {
        for (int i = 0; i < relArraySize; i += 2)
        {
            int elementCounted = 0;
            for (int j = 0; j < domainIndex; j++)
            {
                if (strcmp(relArray[indexRelArray], domainArray[j]) == 0)
                {
                    elementCounted = 1;
                    break;
                }
            }
            if (!elementCounted)
            {
                tmpPtr = realloc(domainArray, (domainIndex + 1) * sizeof(char *));
                if (tmpPtr == NULL)
                {
                    free(domainArray);
                    return 4;
                }
                domainArray = tmpPtr;
                domainArray[domainIndex] = relArray[indexRelArray];
                domainIndex++;
            }
            indexRelArray += 2;
        }
    }
    int error = 0;
    error = saveResult(rows, indexOfCommand, 'S', domainArray, domainIndex);
    if (error == 0)
    {
        printRow(rows[indexOfCommand]);
    }
    free(domainArray);
    return error;
}

//Function 16: injective R A B - Prints true or false, depending on whether or not relation A is injektive image.
//Output: Prints result on stdout, returns 0 if run was successful
int myInjective(int indexOfCommand, char **relArray, char **setArray1, char **setArray2, int relArraySize, int setArray1Size, int setArray2Size)
{
    //Checks if relation is also a function.
    if (myIsFunction(relArray, relArraySize, 0) == 0)
    {
        fprintf(stderr, "ERROR: Relation is not a function used in command at line %d!\n", indexOfCommand + 1);
        return 1;
    }

    //Reads elements in relations.
    int isInSet = 0;
    for (int i = 0; i < relArraySize; i += 2) //Take first element of each relation
    {
        isInSet = 0;
        for (int j = 0; j < setArray1Size; j++)
        {
            if (strcmp(relArray[i], setArray1[j]) == 0) //Function is looking for the element from X of (X Y) in set A.
            {
                isInSet = 1;
                break;
            }
        }
        if (isInSet == 0)
        {
            fprintf(stderr, "ERROR: Element %s is not in set A!\n", relArray[i]);
            return 1;
        }
        isInSet = 0;
        for (int j = 0; j < setArray2Size; j++)
        {
            if (strcmp(relArray[i + 1], setArray2[j]) == 0) //Function is looking for the element from Y of (X Y) in set B.
            {
                isInSet = 1;
                break;
            }
        }
        if (isInSet == 0)
        {
            fprintf(stderr, "ERROR: Element %s is not in set B!\n", relArray[i]);
            return 1;
        }
    }

    //Checks if relation is injective.
    for (int i = 0; i < relArraySize; i += 2) //Take first element of each relation
    {
        for (int j = 0; j < relArraySize; j += 2)
        { //Checks if image has one and only one model.
            if (strcmp(relArray[i + 1], relArray[j + 1]) == 0 && strcmp(relArray[i], relArray[j]) != 0)
            {
                printf("false\n");
                return 0;
            }
        }
    }
    printf("true\n");

    return 0;
}

//Function 17: surjective R A B - Prints true or false, depending on whether or not relation A is surjektive image.
//Output: Prints result on stdout, returns 0 if run was successful.
int mySurjective(int indexOfCommand, char **relArray, char **setArray1, char **setArray2, int relArraySize, int setArray1Size, int setArray2Size)
{
    if (myIsFunction(relArray, relArraySize, 0) == 0)
    {
        fprintf(stderr, "ERROR: Relation is not a function used in command at line %d!\n", indexOfCommand + 1);
        return 1;
    }

    int isInSet = 0;
    for (int i = 0; i < relArraySize; i += 2) //Reads by pairs, that are in relation.
    {
        isInSet = 0;
        for (int j = 0; j < setArray1Size; j++)
        {
            if (strcmp(relArray[i], setArray1[j]) == 0)
            {
                isInSet = 1;
                break;
            }
        }
        if (isInSet == 0)
        {
            fprintf(stderr, "ERROR: Element %s is not in set A!\n", relArray[i]);
            return 1;
        }
        isInSet = 0;
        for (int j = 0; j < setArray2Size; j++)
        {
            if (strcmp(relArray[i + 1], setArray2[j]) == 0)
            {
                isInSet = 1;
                break;
            }
        }
        if (isInSet == 0)
        {
            fprintf(stderr, "ERROR: Element %s is not in set B!\n", relArray[i]);
            return 1;
        }
    }

    //Checks if relation is surjective.
    int isInRelation = 0;
    for (int i = 0; i < setArray2Size; i++) //Checks if every Y from B is an image (X Y).
    {
        isInRelation = 0;
        for (int j = 1; j < relArraySize; j += 2) //Reads elements Y in relation.
        {
            if (strcmp(setArray2[i], relArray[j]) == 0)
            {
                isInRelation = 1;
                break;
            }
        }
        if (isInRelation == 0)
        {
            printf("false\n");
            return 0;
        }
    }
    printf("true\n");

    return 0;
}

//Function 18: bijective R A B - Prints true or false, depending on whether or not relation A is bijektive image.
//Output: Prints result on stdout, returns 0 if run was successful.
int myBijective(int indexOfCommand, char **relArray, char **setArray1, char **setArray2, int relArraySize, int setArray1Size, int setArray2Size)
{
    //Checks if relation is also a function.
    if (myIsFunction(relArray, relArraySize, 0) == 0)
    {
        fprintf(stderr, "ERROR: Relation is not a function used in command at line %d!\n", indexOfCommand + 1);
        return 1;
    }

    //Reads elements in relation.
    int isInSet = 0;
    for (int i = 0; i < relArraySize; i += 2) //Reads by the pairs, that are in relation.
    {
        isInSet = 0;
        for (int j = 0; j < setArray1Size; j++) //Function is looking for the element from X of (X Y) in set A.
        {
            if (strcmp(relArray[i], setArray1[j]) == 0)
            {
                isInSet = 1;
                break;
            }
        }
        if (isInSet == 0)
        {
            fprintf(stderr, "ERROR: Element %s is not in set A!\n", relArray[i]);
            return 1;
        }
        isInSet = 0;
        for (int j = 0; j < setArray2Size; j++) //Function is looking for the element from Y of (X Y) in set B.
        {
            if (strcmp(relArray[i + 1], setArray2[j]) == 0)
            {
                isInSet = 1;
                break;
            }
        }
        if (isInSet == 0)
        {
            fprintf(stderr, "ERROR: Element %s is not in set B!\n", relArray[i]);
            return 1;
        }
    }

    //Check if relation is surjective (bijective = surjective + injective).
    int isInRelation = 0;
    for (int i = 0; i < setArray2Size; i++) //Checks if every Y from B is image (X Y).
    {
        isInRelation = 0;
        for (int j = 1; j < relArraySize; j += 2) //Reads elements Y in relation.
        {
            if (strcmp(setArray2[i], relArray[j]) == 0)
            {
                isInRelation = 1;
                break;
            }
        }
        if (isInRelation == 0)
        {
            printf("false\n");
            return 0;
        }
    }

    //Check bijektivity (bijektive = surjektive + injektive)
    for (int i = 0; i < relArraySize; i += 2)
    {
        for (int j = 0; j < relArraySize; j += 2) //Checks that one image has only one model.
        {
            if (strcmp(relArray[i + 1], relArray[j + 1]) == 0 && strcmp(relArray[i], relArray[j]) != 0)
            {
                printf("false\n");
                return 0;
            }
        }
    }
    printf("true\n");

    return 0;
}

//This function splits set contents to array
char **splitSetContentToArray(char **arrayOfElements, char content[], int *size)
{
    arrayOfElements = malloc(sizeof(char *));
    if (arrayOfElements == NULL)
    {
        return NULL;
    }
    int arrayIndex = 0;   //Index of element in array.
    int contentIndex = 0; //Index of character in content.
    void *tmpPtr = NULL;

    char c = content[contentIndex];
    if (c == '\0')
    {
        arrayOfElements[arrayIndex] = malloc(1);
        if (arrayOfElements == NULL)
        {
            return NULL;
        }
        arrayOfElements[arrayIndex][0] = '\0';
        arrayIndex++;
    }

    while (c != '\0')
    {
        tmpPtr = realloc(arrayOfElements, (arrayIndex + 1) * sizeof(char *));
        if (tmpPtr == NULL)
        {
            deallocArrayOfStrings(arrayOfElements, arrayIndex);
            return NULL;
        }
        arrayOfElements = tmpPtr;

        char *element = (char *)malloc(1);
        if (element == NULL)
        {
            return NULL;
        }

        int elementIndex = 0; //Index of currently loaded character.
        c = content[contentIndex];
        while (c != ' ' && c != '\0') //Until end of element not found
        {
            tmpPtr = realloc(element, elementIndex + 2);
            if (tmpPtr == NULL)
            {
                free(element);
                deallocArrayOfStrings(arrayOfElements, arrayIndex + 1);
                return NULL;
            }
            element = tmpPtr;

            element[elementIndex] = c;
            element[elementIndex + 1] = '\0';
            contentIndex++;
            elementIndex++;
            c = content[contentIndex];

            if (c == ' ' || c == '\0') //If end of one element found.
            {
                arrayOfElements[arrayIndex] = malloc(strlen(element) + 1);
                if (arrayOfElements[arrayIndex] == NULL)
                {
                    free(element);
                    deallocArrayOfStrings(arrayOfElements, arrayIndex + 1);
                    return NULL;
                }

                strcpy(arrayOfElements[arrayIndex], element);
                arrayIndex++;
                contentIndex++;
            }
        }
        free(element);
    }

    *size = arrayIndex;
    return arrayOfElements;
}

//Function: From the content string is made array of elements ze stringu content vytvoří pole prvků relace
//Output: Returns address of array, saves its size.
char **splitRelContentToArray(char **arrayOfElements, char content[], int *size)
{
    arrayOfElements = malloc(sizeof(char *));
    if (arrayOfElements == NULL)
    {
        return NULL;
    }

    int arrayIndex = 0;   //Index of element in array
    int contentIndex = 0; //Index of character in content.
    int elementsInRelCount = 0;
    void *tmpPtr = NULL;

    char c = content[contentIndex];
    if (c == '\0')
    {
        arrayOfElements[arrayIndex] = malloc(sizeof(char));
        if (arrayOfElements == NULL)
        {
            return NULL;
        }
        arrayOfElements[arrayIndex][0] = '\0';
        arrayIndex++;
    }

    while (c != '\0')
    {
        tmpPtr = realloc(arrayOfElements, (arrayIndex + 2) * sizeof(char *));
        if (tmpPtr == NULL)
        {
            deallocArrayOfStrings(arrayOfElements, arrayIndex + 1);
            return NULL;
        }
        arrayOfElements = tmpPtr;

        char *element = (char *)malloc(sizeof(char));
        if (element == NULL)
        {
            return NULL;
        }

        c = content[contentIndex];
        if (c == '(' || c == ' ')
        {
            contentIndex++;
            c = content[contentIndex];
        }

        int elementIndex = 0; //Index of currently loaded character.
        elementsInRelCount++;
        while (c != ')' && c != '\0' && c != ' ' && c != '(') //While character is piece of element.
        {
            tmpPtr = realloc(element, (elementIndex + 2) * sizeof(char));
            if (tmpPtr == NULL)
            {
                free(element);
                deallocArrayOfStrings(arrayOfElements, arrayIndex + 1);
                return NULL;
            }
            element = tmpPtr;

            element[elementIndex] = c;
            element[elementIndex + 1] = '\0';
            contentIndex++;
            elementIndex++;
            c = content[contentIndex];

            if (c == ')' || c == '\0' || c == ' ') //Until character is end of one element.
            {
                arrayOfElements[arrayIndex] = malloc(strlen(element) + 1);
                if (arrayOfElements[arrayIndex] == NULL)
                {
                    free(element);
                    deallocArrayOfStrings(arrayOfElements, arrayIndex + 1);
                    return NULL;
                }

                strcpy(arrayOfElements[arrayIndex], element);
                arrayIndex++;
                contentIndex++;
                if (elementsInRelCount > 1)
                {
                    elementsInRelCount = 0;
                    c = content[contentIndex];
                    contentIndex++;
                }
            }
        }
        free(element);
    }

    *size = arrayIndex;
    return arrayOfElements;
}

void deallocArrayOfStrings(char **array, int arraySize)
{
    for (int i = 0; i < arraySize; i++)
    {
        free(array[i]);
    }
    free(array);
}

int main(int argc, char *argv[])
{
    if (argc == 2) //Proper number of arguments
    {
        set_t *rows;
        int sizeOfArray = 0;
        int indexOfFirstCommand = 0;
        int error = 0;
        char *commandList[] = {"empty", "card", "complement", "union", "intersect", "minus", "subseteq", "subset", "equals", "reflexive", "symmetric",
                               "antisymmetric", "transitive", "function", "domain", "codomain", "injective", "surjective", "bijective"};

        error = readFromFile(&rows, &sizeOfArray, argv[1], &indexOfFirstCommand);
        if (error)
        {
            return 1;
        }

        //Checks universum.
        error = checkUniversum(rows, commandList);
        if (error != 1)
        {
            dealloc(&rows, sizeOfArray);
            return 1;
        }

        //Checks relations and sets.
        error = checkSetAndRelations(rows, indexOfFirstCommand, sizeOfArray, commandList);
        if (error == 0)
        {
            dealloc(&rows, sizeOfArray);
            return 1;
        }

        if (indexOfFirstCommand != 0)
            for (int rowIndex = indexOfFirstCommand; rowIndex < sizeOfArray; rowIndex++)
            {
                char *command = strtok(rows[rowIndex].content, " ");
                char *value1Pointer = strtok(NULL, " "); //Getting pointer of first argument of called funciton.
                if (value1Pointer == NULL)
                {
                    fprintf(stderr, "ERROR: Invalid command values at line %d!\n", rowIndex + 1);
                    dealloc(&rows, sizeOfArray);
                    return 1;
                }
                char *value2Pointer = strtok(NULL, " "); //Getting pointer of first argument of called funciton.
                char *value3Pointer = strtok(NULL, " ");
                char *value4Pointer = strtok(NULL, " ");

                int rowToCheck1 = atoi(value1Pointer) - 1; //Rewriting on index of row.
                int rowToCheck2 = -1;

                int argumentCount = 1;
                if (value2Pointer != NULL) //Checks there is a second argument of command
                {
                    rowToCheck2 = atoi(value2Pointer) - 1;
                    argumentCount++;
                }
                int rowToCheck3 = -1;

                if (value3Pointer != NULL) //Checks there is a third argument of command
                {
                    rowToCheck3 = atoi(value3Pointer) - 1;
                    argumentCount++;
                }

                if (value4Pointer != NULL) //Checks there is a third argument of command
                {
                    fprintf(stderr, "ERROR: Command at line %d has too many arguments!\n", rowToCheck2 + 1);
                    dealloc(&rows, sizeOfArray);
                    return 1;
                }
                //Searching for index of wanted function from list of function.
                int commandId = EOF;
                for (int i = 0; i < countOfCommands; i++)
                {
                    if (strcmp(command, commandList[i]) == 0)
                    {
                        commandId = i;
                        break;
                    }
                }
                if (commandId == -1)
                {
                    fprintf(stderr, "ERROR: Invalid command at line %d!\n", rowIndex + 1); //Error if command is not valid.
                    dealloc(&rows, sizeOfArray);
                    return 1;
                }
                if (!isOperationValidWithTypeOfLine(commandId, rows[rowToCheck1].type, argumentCount, 0)) //Error if Type of line number is not correct.
                {
                    fprintf(stderr, "ERROR: Command \"%s\". Wrong type of used line number %d for this operation!\n", commandList[commandId], rowToCheck1 + 1);
                    dealloc(&rows, sizeOfArray);
                    return 1;
                }
                if (rowToCheck2 != -1)
                {
                    if (!isOperationValidWithTypeOfLine(commandId, rows[rowToCheck2].type, argumentCount, 1)) //Error if Type of line number is not correct.
                    {
                        fprintf(stderr, "ERROR: Command \"%s\". Wrong type of line number %d for this operation!\n", commandList[commandId], rowToCheck2 + 1);
                        dealloc(&rows, sizeOfArray);
                        return 1;
                    }
                }
                if (rowToCheck3 != -1)
                {
                    if (!isOperationValidWithTypeOfLine(commandId, rows[rowToCheck3].type, argumentCount, 2)) //Error if Type of line number is not correct.
                    {
                        fprintf(stderr, "ERROR: Command \"%s\". Wrong type of line number %d for this operation!\n", commandList[commandId], rowToCheck3 + 1);
                        dealloc(&rows, sizeOfArray);
                        return 1;
                    }
                }
                //Saving content to first/secondRowArray. One index used for one element. Usage: for example in myMinus function.
                int firstRowArraySize = 0, secondRowArraySize = 0, thirdRowArraySize = 0;
                char **secondRowArray = NULL, **firstRowArray = NULL, **thirdRowArray = NULL;

                switch (rows[rowToCheck1].type)
                {
                case 'S':
                case 'U':
                    firstRowArray = splitSetContentToArray(firstRowArray, rows[rowToCheck1].content, &firstRowArraySize); //Spliting content to row by calling spliting fuction.
                    break;
                case 'R':
                    firstRowArray = splitRelContentToArray(firstRowArray, rows[rowToCheck1].content, &firstRowArraySize); //Spliting content to row by calling spliting fuction.
                    break;
                }

                if (firstRowArray == NULL)
                {
                    fprintf(stderr, "ERROR: Allocation error!\n"); //Error if (re)allocation failed.
                    return 1;
                }

                if (rowToCheck2 > -1)
                {
                    switch (rows[rowToCheck2].type)
                    {
                    case 'S':
                    case 'U':
                        secondRowArray = splitSetContentToArray(secondRowArray, rows[rowToCheck2].content, &secondRowArraySize); //Spliting content to row by calling spliting fuction.
                        break;
                    case 'R':
                        secondRowArray = splitRelContentToArray(secondRowArray, rows[rowToCheck2].content, &secondRowArraySize); //Spliting content to row by calling spliting fuction.
                        break;
                    }
                    if (secondRowArray == NULL)
                    {
                        fprintf(stderr, "ERROR: Allocation error!\n"); //Error if (re)allocation failed.
                        return 1;
                    }
                }
                if (rowToCheck3 > -1)
                {
                    switch (rows[rowToCheck3].type)
                    {
                    case 'S':
                    case 'U':
                        thirdRowArray = splitSetContentToArray(thirdRowArray, rows[rowToCheck3].content, &thirdRowArraySize); //Spliting content to row by calling spliting fuction.
                        break;
                    case 'R':
                        thirdRowArray = splitRelContentToArray(thirdRowArray, rows[rowToCheck3].content, &thirdRowArraySize); //Spliting content to row by calling spliting fuction.
                        break;
                    }
                    if (thirdRowArray == NULL)
                    {
                        fprintf(stderr, "ERROR: Allocation error!\n"); //Error if (re)allocation failed.
                        return 1;
                    }
                }

                switch (commandId) //Calling functions, depending on case number.
                {
                case 0:
                    error = myEmpty(rows, rowToCheck1, rowToCheck2);
                    break;
                case 1:
                    error = myCard(firstRowArray, firstRowArraySize);
                    break;
                case 2:
                    error = myComplement(rows, rowIndex, firstRowArray, firstRowArraySize);
                    break;
                case 3:
                    error = myUnion(rows, rowIndex, firstRowArray, secondRowArray, firstRowArraySize, secondRowArraySize);
                    break;
                case 4:
                    error = myIntersect(rows, rowIndex, firstRowArray, secondRowArray, firstRowArraySize, secondRowArraySize);
                    break;
                case 5:
                    error = myMinus(rows, rowIndex, firstRowArray, secondRowArray, firstRowArraySize, secondRowArraySize);
                    break;
                case 6:
                    error = mySubseteq(firstRowArray, secondRowArray, firstRowArraySize, secondRowArraySize);
                    break;
                case 7:
                    error = mySubset(firstRowArray, secondRowArray, firstRowArraySize, secondRowArraySize);
                    break;
                case 8:
                    error = myEquals(firstRowArray, secondRowArray, firstRowArraySize, secondRowArraySize);
                    break;
                case 9:
                    error = myReflexive(rows, firstRowArray, firstRowArraySize);
                    break;
                case 10:
                    error = mySymmetric(firstRowArray, firstRowArraySize);
                    break;
                case 11:
                    error = myAntisymmetric(rows, firstRowArray, firstRowArraySize);
                    break;
                case 12:
                    error = myTransitive(firstRowArray, firstRowArraySize);
                    break;
                case 13:
                    error = myIsFunction(firstRowArray, firstRowArraySize, 1);
                    break;
                case 14:
                    error = myDomainAndCodomain(rows, rowIndex, firstRowArray, firstRowArraySize, 0);
                    break;
                case 15:
                    error = myDomainAndCodomain(rows, rowIndex, firstRowArray, firstRowArraySize, 1);
                    break;
                case 16:
                    error = myInjective(rowIndex, firstRowArray, secondRowArray, thirdRowArray, firstRowArraySize, secondRowArraySize, thirdRowArraySize);
                    break;
                case 17:
                    error = mySurjective(rowIndex, firstRowArray, secondRowArray, thirdRowArray, firstRowArraySize, secondRowArraySize, thirdRowArraySize);
                    break;
                case 18:
                    error = myBijective(rowIndex, firstRowArray, secondRowArray, thirdRowArray, firstRowArraySize, secondRowArraySize, thirdRowArraySize);
                    break;
                default:
                    fprintf(stderr, "ERROR: Invalid command \"%s\"!\n", command); // Error if command is not valid.
                    error = 1;
                    break;
                }

                deallocArrayOfStrings(firstRowArray, firstRowArraySize); //Deallocating first of array.

                if (secondRowArray != NULL)
                {
                    deallocArrayOfStrings(secondRowArray, secondRowArraySize); //Deallocating second of array.
                }

                if (thirdRowArray != NULL)
                {
                    deallocArrayOfStrings(thirdRowArray, thirdRowArraySize); //Deallocating third of array.
                }

                if (error)
                {
                    switch (error)
                    {
                    case 2:
                        fprintf(stderr, "ERROR: Invalid type of line for \"%s\" command!\n", command); //Error if lines are not valid.
                        break;
                    case 3:
                        fprintf(stderr, "ERROR: Invalid rows for \"%s\" command!\n", command); //Error if rows are not valid.
                        break;
                    case 4:
                        fprintf(stderr, "ERROR: Allocation error in \"%s\"command!\n", command); // Error of (re)allocation.
                        break;
                    default:
                        break;
                    }
                    dealloc(&rows, sizeOfArray); //Deallocing of row.
                    return 1;
                }
            }
        dealloc(&rows, sizeOfArray);
    }
    else
    {
        fprintf(stderr, "ERROR: Invalid argument! Try: setcalc txtfilename.txt\n"); //Error if arguments are not valid.
        return 1;
    }
    return 0;
}
