//
// Created by imjalpreet on 21/9/15.
//

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"

typedef struct data {
    char columnName[20];
    char size[20];
    struct node *head;
}data;

typedef struct node {
    char value[500];
    struct node *row, *column, *first;
}node;

struct node* SortedMerge(struct node* a, struct node* b, int columnNumber[], char order[], int numberOfColumnsSort);
void FrontBackSplit(struct node* source, struct node** frontRef, struct node** backRef);

/* sorts the linked list by changing next pointers (not data) */
void MergeSort(struct node** headRef, int columnNumber[], char order[], int numberOfColumnsSort)
{
    struct node* head = *headRef;
    struct node* a;
    struct node* b;

    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->column == NULL))
    {
        return;
    }

    /* Split head into 'a' and 'b' sublists */
    FrontBackSplit(head, &a, &b);

    /* Recursively sort the sublists */
    MergeSort(&a, columnNumber, order, numberOfColumnsSort);
    MergeSort(&b, columnNumber, order, numberOfColumnsSort);

    /* answer = merge the two sorted lists together */
    *headRef = SortedMerge(a, b, columnNumber, order, numberOfColumnsSort);
}

struct node* SortedMerge(struct node* a, struct node* b, int columnNumber[], char order[], int numberOfColumnsSort)
{
    struct node* result = NULL;

    /* Base cases */
    if (a == NULL)
        return(b);
    else if (b==NULL)
        return(a);
    int i;
    /* Pick either a or b, and recur */
    for (i = 0; i<numberOfColumnsSort; ++i) {
        int cNumber = columnNumber[i], flag = 0;
        node *temp1, *temp2;
        temp1 = a;
        temp2 = b;
        while (cNumber--) {
            temp1 = temp1->row;
            temp2 = temp2->row;
        }
        /*if(i == 1)
            printf("**%d**\n", columnNumber[i]);*/
        if ((strcmp(temp1->value, temp2->value) < 0 && order[0] == 'a') ||
            (strcmp(temp1->value, temp2->value) > 0 && order[0] == 'd')) {
            flag = 1;
            result = a;
            result->column = SortedMerge(a->column, b, columnNumber, order, numberOfColumnsSort);
        }
        else if((strcmp(temp1->value, temp2->value) == 0)) {
            continue;
        }
        else {
            flag = 1;
            result = b;
            result->column = SortedMerge(a, b->column, columnNumber, order, numberOfColumnsSort);
        }
        if(flag == 1)
            break;
    }
    return(result);
}

/* Split the nodes of the given list into front and back halves,
     and return the two lists using the reference parameters.
     If the length is odd, the extra node should go in the front list.
     Uses the fast/slow pointer strategy.  */
void FrontBackSplit(struct node* source,
                    struct node** frontRef, struct node** backRef)
{
    struct node* fast;
    struct node* slow;
    if (source==NULL || source->column==NULL)
    {
        /* length < 2 cases */
        *frontRef = source;
        *backRef = NULL;
    }
    else
    {
        slow = source;
        fast = source->column;

        /* Advance 'fast' two nodes, and advance 'slow' one node */
        while (fast != NULL)
        {
            fast = fast->column;
            if (fast != NULL)
            {
                slow = slow->column;
                fast = fast->column;
            }
        }

        /* 'slow' is before the midpoint in the list, so split it in two
          at that point. */
        *frontRef = source;
        *backRef = slow->column;
        slow->column = NULL;
    }
}

int main(int argc, char *argv[]) {
    char inputFile[100], outputFile[100], order[5];
    char MemoryAllowed[10];
    FILE *metaData;
    char testColumn[20], temp[200], testColumnSize[20];
    int flag, recordSize=0, sublists=0, numberOfRecords = 0;
    if(argc < 5) {
        printf("Argument List incomplete!\n");
        return 0;
    }
    strcpy(inputFile, argv[1]);
    strcpy(outputFile, argv[2]);
    strcpy(MemoryAllowed, argv[3]);
    strcpy(order, argv[4]);
    int numberOfColumnsSort = argc - 5, i = 0, j, k;
    char columns[numberOfColumnsSort][5];
    int tempCol = numberOfColumnsSort;
    while(tempCol--) {
        strcpy(columns[i], argv[5+i]);
        i++;
    }
    int columnNumber[numberOfColumnsSort], len;
    for (j = 0; j < numberOfColumnsSort; ++j) {
        len = strlen(columns[j]);
        columnNumber[j] = 0;
        for (i = 1; i < len; ++i) {
            columnNumber[j] += (columns[j][i] - '0') * pow(10, len - 1 - i);
        }
    }
    metaData = fopen("metadata.txt", "r");
    if(metaData == NULL)
    {
        printf("error in opening file : metadata.txt\n");
        return -1;
    }
    while(fscanf(metaData, "%s", temp) > 0){
        i++;
    }
    fclose(metaData);
    metaData = fopen("metadata.txt", "r");
    data *DATA[i];
    k = 0;
    while(fscanf(metaData, "%s", temp) > 0) {
        i=0;
        int len = strlen(temp);
        flag = 0;
        while(len--) {
            if(temp[i] == ','){
                i++;
                j = 0;
                flag = 1;
                continue;
            }
            if(!flag) {
                testColumn[i] = temp[i];
            }
            else {
                testColumnSize[j] = temp[i];
                j++;
            }
            i++;
        }
        testColumnSize[j] = '\0';
        data *tempNode = NULL;
        tempNode = (data *)malloc(sizeof(data));
        strcpy(tempNode->columnName, testColumn);
        strcpy(tempNode->size, testColumnSize);
        tempNode->head = NULL;
        DATA[k] = tempNode;
        k++;
    }
    fclose(metaData);
    int maxMemoryAllowed = 0;
    for (i = 0; i < k; ++i) {
        len = strlen(DATA[i]->size);
        for (j = 0; j < len; ++j) {
            recordSize = recordSize + (DATA[i]->size[j] - '0')*pow(10,len-1-j);
        }
    }
    len = strlen(MemoryAllowed);
    for (i = 0; i < len; i++)
        maxMemoryAllowed += (MemoryAllowed[i] - '0')*pow(10, len-1-i);
    int MaxMem = (int)((0.02)*maxMemoryAllowed*1024*1024)/(recordSize);
    FILE *input;
    input = fopen(inputFile, "r");
    if(input == NULL)
    {
        printf("error in opening file : input.txt\n");
        return -1;
    }
    FILE *tempFile;
    tempFile = fopen("temp.txt", "a");
    i = 0;
    j = 0;
    node *firstNode;
    node *lastNode[k];
    while(1) {
        if(i==k) {
            i=0;
            j++;
        }
        int length = 0;
        int returnValue;
        char x[200];
        if(i == 0) {
            char c = fgetc(input);
            length = 0;
            while(c == '\r' || c == '\n')
                c = fgetc(input);
            if(c == EOF)
                break;
            while (length != 10) {
                temp[length++] = c;
                c = fgetc(input);
            }
            temp[length] = '\0';
        }
        else {
            returnValue = fscanf(input, "%s", temp);
            if(returnValue < 0)
                break;
        }

        length = strlen(temp);
        int size = atoi(DATA[i]->size);
        while(length != size) {
            char c = fgetc(input);
            temp[length++] = c;
        }
        temp[length] = '\0';
        if(j >= MaxMem){
            numberOfRecords += j;
            //printf("**%d**\n", j);
            sublists++;
            int l,m;
            MergeSort(&(DATA[0]->head), columnNumber, order, numberOfColumnsSort);
            node *Temp;
            Temp = DATA[0]->head;
            while(Temp){
                for (l = 0; l < k; ++l) {
                    node *Temp2;
                    Temp2 = Temp;
                    for (m = 0; m < l; m++)
                        Temp2 = Temp2->row;
                    if (l != k-1)
                        fprintf(tempFile, "%s  ", Temp2->value);
                    else
                        fprintf(tempFile, "%s\n", Temp2->value);
                }
                Temp=Temp->column;
            }
            j = 0;
            for (l = 0; l < k; ++l) {
                node *temp2, *temp1;
                temp1 = DATA[l]->head->column;
                while (temp1 != NULL){
                    temp2 = temp1;
                    temp1 = temp1->column;
                    free(temp2);
                }
                DATA[l]->head = NULL;
            }
        }
        node *tempData, *tempData2;
        node *tempNode;
        if (DATA[i]->head != NULL)
            tempData = DATA[i]->head;
        else
            tempData = NULL;
        if (tempData == NULL) {
            tempNode = (node *)malloc(sizeof(node));
            strcpy(tempNode->value, temp);
            tempNode->column = NULL;
            tempNode->row = NULL;
            tempData = tempNode;
            DATA[i]->head = tempData;
            if(i!=0) {
                DATA[i]->head->first = firstNode;
                DATA[i - 1]->head->row = DATA[i]->head;
            }
            else {
                DATA[i]->head->first = DATA[i]->head;
                firstNode = DATA[i]->head;
            }
            lastNode[i] = DATA[i]->head;
        }
        else {
            tempNode = (node *) malloc(sizeof(node));
            strcpy(tempNode->value, temp);
            tempNode->column = NULL;
            tempNode->row = NULL;
            lastNode[i]->column = tempNode;
            lastNode[i] = tempNode;
            if(i!=0){
                lastNode[i-1]->row = tempNode;
            }
        }
        i++;
    }
    fclose(input);
    numberOfRecords += j;
    MergeSort(&(DATA[0]->head), columnNumber, order, numberOfColumnsSort);
    node *Temp;
    Temp = DATA[0]->head;
    while(Temp){
        for (i = 0; i < k; ++i) {
            node *Temp2;
            Temp2 = Temp;
            for (j = 0; j < i; j++)
                Temp2 = Temp2->row;
            if (i != k-1)
                fprintf(tempFile, "%s  ", Temp2->value);
            else
                fprintf(tempFile, "%s\n", Temp2->value);
        }
        Temp=Temp->column;
    }
    for (i = 0; i < k; ++i) {
        node *temp2, *temp1;
        temp1 = DATA[i]->head->column;
        while (temp1 != NULL){
            temp2 = temp1;
            temp1 = temp1->column;
            free(temp2);
        }
        DATA[i]->head = NULL;
    }
    sublists++;
    //printf("Sublists: %d\n", sublists);
    fclose(tempFile);
    printf("PHASE-1 OVER\n");
    FILE *tempPointer[sublists];
    int maxRecords = MaxMem, bytePosition;
    for (i = 0; i < sublists; ++i) {
        strcpy(temp, "");
        tempPointer[i] = fopen("temp.txt", "r");
        bytePosition = (recordSize+(2*(k-1))+1)*maxRecords*i;
        fseek(tempPointer[i], bytePosition, SEEK_SET);
    }
    int checkSublists[sublists], verifySublists[sublists];
    for (i = 0; i < sublists; ++i) {
        checkSublists[i] = 0;
        if (i != sublists-1)
            verifySublists[i] = MaxMem;
        else {
            verifySublists[i] = numberOfRecords - (sublists-1) * MaxMem;
            if (verifySublists[i] == 0) {
                verifySublists[i] = MaxMem;
            }
        }
    }
    int firstIteration = 0, whichSublist = -1, minPosition = -1;
    FILE *output;
    output = fopen(outputFile, "a+");
    while(1) {
        node *Temp, *minTemp;
        if(firstIteration == 0) {
            for (j = 0; j < sublists; ++j) {
                i = 0;
                while(1) {
                    if (i == k) {
                        i = 0;
                        break;
                    }
                    int length = 0;
                    int returnValue;
                    char x[200];
                    if(i == 0) {
                        char c = fgetc(tempPointer[j]);
                        length = 0;
                        while(c == '\r' || c == '\n')
                            c = fgetc(tempPointer[j]);
                        int size = atoi(DATA[0]->size);
                        while (length != size) {
                            temp[length++] = c;
                            c = fgetc(tempPointer[j]);
                        }
                        temp[length] = '\0';
                    }
                    else {
                        returnValue = fscanf(tempPointer[j], "%s", temp);
                        if(returnValue < 0)
                            break;
                    }
                    length = strlen(temp);
                    int size = atoi(DATA[i]->size);
                    while (length != size) {
                        char c = fgetc(tempPointer[j]);
                        temp[length++] = c;
                    }
                    temp[length] = '\0';
                    node *tempData, *tempData2;
                    node *tempNode;
                    if (DATA[i]->head != NULL)
                        tempData = DATA[i]->head;
                    else
                        tempData = NULL;
                    if (tempData == NULL) {
                        tempNode = (node *)malloc(sizeof(node));
                        strcpy(tempNode->value, temp);
                        tempNode->column = NULL;
                        tempNode->row = NULL;
                        tempData = tempNode;
                        DATA[i]->head = tempData;
                        if(i!=0) {
                            DATA[i]->head->first = firstNode;
                            DATA[i - 1]->head->row = DATA[i]->head;
                        }
                        else {
                            DATA[i]->head->first = DATA[i]->head;
                            firstNode = DATA[i]->head;
                        }
                        lastNode[i] = DATA[i]->head;
                    }
                    else {
                        tempNode = (node *) malloc(sizeof(node));
                        strcpy(tempNode->value, temp);
                        tempNode->column = NULL;
                        tempNode->row = NULL;
                        lastNode[i]->column = tempNode;
                        lastNode[i] = tempNode;
                        if(i!=0){
                            lastNode[i-1]->row = tempNode;
                        }
                    }
                    i++;
                }
            }
            firstIteration = 1;
        }
        else if(minPosition >= 0 && ((minPosition == 0 && checkSublists[minPosition] != MaxMem) || (minPosition != 0 && checkSublists[minPosition] != (verifySublists[minPosition])))){
            firstIteration = 2;
            bytePosition = (recordSize+(2*(k-1))+1)*maxRecords*minPosition + (recordSize+(2*(k-1))+1)*checkSublists[minPosition];
            fseek(tempPointer[minPosition], bytePosition, SEEK_SET);
            i = 0;
            while(1) {
                if (i == k) {
                    i = 0;
                    break;
                }
                int length = 0;
                int returnValue;
                char x[200];
                if(i == 0) {
                    char c = fgetc(tempPointer[minPosition]);
                    length = 0;
                    while(c == '\r' || c == '\n')
                        c = fgetc(tempPointer[minPosition]);
                    int size = atoi(DATA[0]->size);
                    while (length != size) {
                        temp[length++] = c;
                        c = fgetc(tempPointer[minPosition]);
                    }

                    temp[length] = '\0';
                }
                else {
                    returnValue = fscanf(tempPointer[minPosition], "%s", temp);
                    if(returnValue < 0)
                        break;
                }
                length = strlen(temp);
                int size = atoi(DATA[i]->size);
                while (length != size) {
                    char c = fgetc(tempPointer[minPosition]);
                    temp[length++] = c;
                }
                temp[length] = '\0';
                int mPos = i;
                node *tempNode = minTemp;
                /*tempNode = DATA[i]->head;
                while(mPos--) {
                    tempNode = tempNode->column;
                }*/

		while(mPos--)
			tempNode = tempNode->row;
                strcpy(tempNode->value, temp);
                i++;
            }
        }
        Temp = DATA[0]->head;
        whichSublist = -1;
        minPosition = -1;
        int size = atoi(DATA[columnNumber[0]]->size);
        char tempString[size];
        strcpy(tempString, "-1");
        while(Temp) {
            whichSublist++;
            if(whichSublist == 0) {
                node *temp1;
                temp1 = Temp;
                int cNumber = columnNumber[0];
                while(cNumber--){
                    temp1 = temp1->row;
                }
                if (strlen(temp1->value) == size) {
                    strcpy(tempString, temp1->value);
                    minPosition = whichSublist;
                }
                minTemp = Temp;
            }
            else {
                node *temp1, *temp2;
                temp1 = Temp;
                temp2 = minTemp;
                int cNumber = columnNumber[0];
                while(cNumber--){
                    temp1 = temp1->row;
                    temp2 = temp2->row;
                }
                if(strlen(temp1->value) == size && strcmp(temp2->value, "-1")) {
                    if((strcmp(temp2->value, temp1->value) > 0 && order[0] == 'a') || (strcmp(temp2->value, temp1->value) < 0 && order[0] == 'd')) {
                        minPosition = whichSublist;
                        strcpy(tempString, temp1->value);
                        minTemp = Temp;
                    }
                    else if (strcmp(temp2->value, temp1->value) == 0) {
                        for (i = 1; i < numberOfColumnsSort; ++i) {
                            temp1 = Temp;
                            temp2 = minTemp;
                            cNumber = columnNumber[i];
                            while (cNumber--) {
                                temp1 = temp1->row;
                                temp2 = temp2->row;
                            }
                            if ((strcmp(temp2->value, temp1->value) > 0 && order[0] == 'a') ||
                                (strcmp(temp2->value, temp1->value) < 0 && order[0] == 'd')) {
                                minPosition = whichSublist;
                                minTemp = Temp;
                                break;
                            }
                        }
                    }
                }
                else if(strlen(temp1->value) == size && (strcmp(temp2->value, "-1") == 0)) {
                    minPosition = whichSublist;
                    strcpy(tempString, temp1->value);
                    minTemp = Temp;
                }
            }
            Temp = Temp->column;
        }
        if (minPosition == -1)
            break;
        checkSublists[minPosition]++;
        int m;
        for (j = 0; j < k; ++j) {
            node *Temp2;
            Temp2 = minTemp;
            for (m = 0; m < j; m++)
                Temp2 = Temp2->row;
            if (j != k-1) {
                fprintf(output, "%s  ", Temp2->value);
                strcpy(Temp2->value, "-1");
            }
            else {
                fprintf(output, "%s\r\n", Temp2->value);
                strcpy(Temp2->value, "-1");
            }
        }
    }
    /*for (i = 0; i < sublists; ++i) {
        printf("Sublist: %d Count: %d\n", i, checkSublists[i]);
    }*/
    return 0;
}
