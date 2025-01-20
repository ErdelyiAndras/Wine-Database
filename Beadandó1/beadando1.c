#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

typedef struct _wineRecord {
    char* wineryName;
    char* grapeType;
    int wineAmount;
    int sugarContent;
} WineRecord;

// read string to buffer, replace newline character with null terminator, handle buffer overflow
void getString(char* const string, const int size) {
    char* result = fgets(string, size, stdin);
    if (result == NULL) {
        exit(1);
    }
    if (string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
    else {
        while (fgetc(stdin) != '\n');
    }
}

// read int to num
void getNum(int* const num) {
    char buffer[1024] = "\0";
    getString(buffer, sizeof(buffer));
    *num = atoi(buffer);
}

// print a winerecord to stdout
void printWineRecord(const WineRecord* const wineRecord) {
    printf("Winery: %s, Grapetype: %s, Amount: %d litre(s), Sugarcontent: %d\n", wineRecord -> wineryName, wineRecord -> grapeType, wineRecord -> wineAmount, wineRecord -> sugarContent);
}

// initialize wine record (has to be freed with freeWineRecordFields)
WineRecord initWineRecord(const char* const wineryName, const char* const grapeType, const int wineAmount, const int sugarContent) {
    WineRecord wineRecord;
    wineRecord.wineryName = strdup(wineryName);
    wineRecord.grapeType = strdup(grapeType);
    wineRecord.wineAmount = wineAmount;
    wineRecord.sugarContent = sugarContent;
    return wineRecord;
}

// frees wineRecordFields
void freeWineRecordFields(const WineRecord* const wineRecord) {
    free(wineRecord -> wineryName);
    free(wineRecord -> grapeType);
}

// reads a winerecord from stdin
WineRecord getWineRecord() {
    char wineryName[1024] = "\0";
    char grapeType[1024] = "\0";
    int wineAmount = 0;
    int sugarContent = 0;

    printf("Name of the vinery: ");
    getString(wineryName, sizeof(wineryName));
    printf("Type of grape: ");
    getString(grapeType, sizeof(grapeType));
    printf("Amount in litres: ");
    getNum(&wineAmount);
    if (wineAmount < 0) {
        wineAmount = 0;
    }
    printf("Sugarcontent: ");
    getNum(&sugarContent);
    if (sugarContent < 0) {
        sugarContent = 0;
    }

    return initWineRecord(wineryName, grapeType, wineAmount, sugarContent);
}

// writes a winerecord to file
int writeWineRecordToFile(const WineRecord* const wineRecord, const int file) {
    int wineryNameLength = strlen(wineRecord -> wineryName) + 1;
    if (write(file, &wineryNameLength, sizeof(wineryNameLength)) != sizeof(wineryNameLength)) {
        // perror("Couldn't write the length of the name of the vinery to file");
        return 0;
    }

    if (write(file, wineRecord -> wineryName, wineryNameLength) != wineryNameLength) {
        // perror("Couldn't write the name of the vinery to file");
        return 0;
    }

    int grapeTypeLength = strlen(wineRecord -> grapeType) + 1;
    if (write(file, &grapeTypeLength, sizeof(grapeTypeLength)) != sizeof(grapeTypeLength)) {
        // perror("Couldn't write the length of the type of the grape to file");
        return 0;
    }

    if (write(file, wineRecord -> grapeType, grapeTypeLength) != grapeTypeLength) {
        // perror("Couldn't write the type of the grape to file");
        return 0;
    }

    if (write(file, &wineRecord -> wineAmount, sizeof(wineRecord -> wineAmount)) != sizeof(wineRecord -> wineAmount)) {
        // perror("Couldn't write the amount to file");
        return 0;
    }

    if (write(file, &wineRecord -> sugarContent, sizeof(wineRecord -> sugarContent)) != sizeof(wineRecord -> sugarContent)) {
        // perror("Couldn't write the sugarcontent to file");
        return 0;
    }
    
    return 1;
}

// reads a winerecord to wineRecord parameter
int readWineRecordFromFile(WineRecord* const wineRecord, const int file) {
    int wineryNameLength;
    if (read(file, &wineryNameLength, sizeof(wineryNameLength)) != sizeof(wineryNameLength)) {
        // perror("Couldn't read the length of the name of the vinery from file");
        return 0;
    }

    char* wineryName = (char*)malloc(wineryNameLength);
    if (wineryName == NULL) {
        // perror("Couldn't allocate memory for the name of the vinery");
        return 0;
    }

    if (read(file, wineryName, wineryNameLength) != wineryNameLength) {
        // perror("Couldn't read the name of the vinery from file");
        free(wineryName);
        return 0;
    }

    int grapeTypeLength;
    if (read(file, &grapeTypeLength, sizeof(grapeTypeLength)) != sizeof(grapeTypeLength)) {
        // perror("Couldn't read the length of the type of the grape from file");
        free(wineryName);
        return 0;
    }

    char* grapeType = (char*)malloc(grapeTypeLength);
    if (grapeType == NULL) {
        // perror("Couldn't allocate memory for the type of the grape");
        free(wineryName);
        return 0;
    }

    if (read(file, grapeType, grapeTypeLength) != grapeTypeLength) {
        // perror("Couldn't read the type of the grape from file");
        free(wineryName);
        free(grapeType);
        return 0;
    }

    int wineAmount;
    if (read(file, &wineAmount, sizeof(wineAmount)) != sizeof(wineAmount)) {
        free(wineryName);
        free(grapeType);
        // perror("Couldn't read the amount from file");
        return 0;
    }

    int sugarContent;
    if (read(file, &sugarContent, sizeof(sugarContent)) != sizeof(sugarContent)) {
        free(wineryName);
        free(grapeType);
        // perror("Couldn't read the sugarcontent from file");
        return 0;
    }

    *wineRecord = initWineRecord(wineryName, grapeType, wineAmount, sugarContent);

    free(wineryName);
    free(grapeType);

    return 1;
}

// counts the number of winerecords in a file 
int countWineRecords(const int file) {
    int count = 0;
    WineRecord wineRecord;
    while (readWineRecordFromFile(&wineRecord, file)) {
        count++;
        freeWineRecordFields(&wineRecord);
    }

    lseek(file, 0, SEEK_SET);
    
    return count;
}

// reads winerecords into a dynamic array
WineRecord* createWineRecordArray(const int numberOfWineRecords, const int file) {
    WineRecord* wineRecords = (WineRecord*)malloc(numberOfWineRecords * sizeof(WineRecord));
    if (wineRecords == NULL) {
        // perror("Couldn't allocate memory for the WineRecord array");
        close(file);
        return NULL;
    }

    int i = 0;
    while (readWineRecordFromFile(&wineRecords[i], file)) {
        i++;
    }
    lseek(file, 0, SEEK_SET);
    return wineRecords;
}

// displays option in menu
void displayOptions() {
    printf("1 - Add New Record\n");
    printf("2 - Modify Record\n");
    printf("3 - Delete Record\n");
    printf("4 - Print Records\n");
    printf("5 - Exit\n\n");
    printf("Choice >> ");
}

// adds new winerecord to file
void addWineRecord(const char* const filename) {
    int file = open(filename, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    if (file < 0) {
        printf("Couldn't open the file\n");
        return;
    }
    WineRecord wineRecord = getWineRecord();

    if (writeWineRecordToFile(&wineRecord, file)) {
        printf("\nSuccessfully added new record\n");
    } else {
        printf("\nSomething went wrong\n");
    }

    freeWineRecordFields(&wineRecord);
    close(file);
}

// modifies an existing record in file
void modifyWineRecord(const char* const filename) {
    int file = open(filename, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    if (file < 0) {
        printf("Couldn't open the file\n");
        return;
    }

    int numberOfWineRecords = countWineRecords(file);
    if (numberOfWineRecords < 1) {
        printf("File is empty\n");
        close(file);
        return;
    }

    printf("Record's number to modify: ");
    int recordNumber;
    getNum(&recordNumber);

    if (recordNumber < 1 || recordNumber > numberOfWineRecords) {
        printf("Invalid record number\n");
        printf("Record's number has to be a number between 1 and %d\n", numberOfWineRecords);
        close(file);
        return;
    }

    WineRecord* wineRecords = createWineRecordArray(numberOfWineRecords, file);
    if (wineRecords == NULL) {
        close(file);
        return;
    }

    close(file);

    printf("Record to be modified: ");
    printWineRecord(&wineRecords[recordNumber - 1]);

    printf("New record:\n");
    WineRecord modifiedWineRecord = getWineRecord();


    file = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (file < 0) {
        printf("Couldn't open the file\n");
        for (int i = 0; i < numberOfWineRecords; i++) {
            freeWineRecordFields(&wineRecords[i]);
        }
        free(wineRecords);
        return;
    }

    for (int i = 0; i < numberOfWineRecords; i++) {
        if (i + 1 != recordNumber) {
            writeWineRecordToFile(&wineRecords[i], file);
        }
        else {
            writeWineRecordToFile(&modifiedWineRecord, file);
        }
        freeWineRecordFields(&wineRecords[i]);
    }
    freeWineRecordFields(&modifiedWineRecord);
    free(wineRecords);

    close(file);

    printf("\nSuccessfully modified record\n");
}

// deletes a record from file
void deleteWineRecord(const char* const filename) {
    int file = open(filename, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    if (file < 0) {
        printf("Couldn't open the file\n");
        return;
    }

    int numberOfWineRecords = countWineRecords(file);
    if (numberOfWineRecords < 1) {
        printf("File is empty\n");
        close(file);
        return;
    }

    printf("Record's number to delete: ");
    int recordNumber;
    getNum(&recordNumber);

    if (recordNumber < 1 || recordNumber > numberOfWineRecords) {
        printf("Invalid record number\n");
        printf("Record's number has to be a number between 1 and %d\n", numberOfWineRecords);
        close(file);
        return;
    }

    WineRecord* wineRecords = createWineRecordArray(numberOfWineRecords, file);
    if (wineRecords == NULL) {
        close(file);
        return;
    }

    close(file);


    file = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (file < 0) {
        printf("Couldn't open the file\n");
        for (int i = 0; i < numberOfWineRecords; i++) {
            freeWineRecordFields(&wineRecords[i]);
        }
        free(wineRecords);
        return;
    }

    for (int i = 0; i < numberOfWineRecords; i++) {
        if (i + 1 != recordNumber) {
            writeWineRecordToFile(&wineRecords[i], file);
        }
        freeWineRecordFields(&wineRecords[i]);
    }
    free(wineRecords);

    close(file);

    printf("\nSuccessfully deleted record\n");
}

// prints all winerecords from file to stdout
void printWineRecords(const char* const filename) {
    int file = open(filename, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    if (file < 0) {
        printf("Couldn't open the file\n");
        return;
    }
    WineRecord wineRecord;
    int i = 1;
    while (readWineRecordFromFile(&wineRecord, file)) {
        printf("%d. ", i++);
        printWineRecord(&wineRecord);
        freeWineRecordFields(&wineRecord);
    }
    if (i == 1) {
        printf("File is empty\n");
    }
    close(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];
    // char* filename = "./wine_records_bin.txt";
    int option;
    do {
        displayOptions();
        getNum(&option);
        printf("\nChoosen menu: ");
        switch (option) {
            case 1: {
                printf("Add New Record\n\n");
                addWineRecord(filename);
                break;
            }
            case 2: {
                printf("Modify Record\n\n");
                modifyWineRecord(filename);
                break;
            }
            case 3: {
                printf("Delete Record\n\n");
                deleteWineRecord(filename);
                break;
            }
            case 4: {
                printf("Print Records\n\n");
                printWineRecords(filename);
                break;
            }
            case 5: {
                printf("Exit\n");
                break;
            }
            default: {
                printf("Doesn't exist\n");
                break;
            }
        }
        printf("\n");
    } while (option != 5);

    return 0;
}