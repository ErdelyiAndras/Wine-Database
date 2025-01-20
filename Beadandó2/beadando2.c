#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <wait.h>

typedef struct _wineRecord {
    char* wineryName;
    char* grapeType;
    int wineAmount;
    int sugarContent;
} WineRecord;

typedef struct _grapeInfo {
    char* grapeType;
    int sugarContent;
} GrapeInfo;

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
    printf("5 - Check Records\n");
    printf("6 - Exit\n\n");
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
        printf("Record's number has to be between 1 and %d\n", numberOfWineRecords);
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

void checkReadyHandler() {
    printf("Ready to check\n\n");
}

int findSugarContent(const WineRecord* const wineRecord, const GrapeInfo* const grapeInfoArray, const int grapeInfoArrayLength) {
    for (int i = 0; i < grapeInfoArrayLength; ++i) {
        if (strcmp(wineRecord -> grapeType, grapeInfoArray[i].grapeType) == 0) {
            return grapeInfoArray[i].sugarContent;
        }
    }
    return -1;
}

int isSugarContentAdequate(const WineRecord* const wineRecord, const int sugarContentGoal) {
    return abs(wineRecord -> sugarContent - sugarContentGoal) <= 1;
}

void checkWinery(const char* const filename) {
    char dataPipe[20];
    char resultPipe[20];
    char lengthPipe[20];
    sprintf(dataPipe, "/tmp/1hd6jo4%d", getpid());
    sprintf(resultPipe, "/tmp/2hd6jo4%d", getpid());
    sprintf(lengthPipe, "/tmp/3hd6jo4%d", getpid());
    int status1 = mkfifo(dataPipe, S_IRUSR | S_IWUSR);
    int status2 = mkfifo(resultPipe, S_IRUSR | S_IWUSR);
    int status3 = mkfifo(lengthPipe, S_IRUSR | S_IWUSR);
    if (status1 == -1 || status2 == -1 || status3 == -1) {
        printf("Couldn't create the pipe\n");
        return;
    }

    int child = fork();
    if (child < 0) {
        printf("Couldn't fork\n");
        unlink(dataPipe);
        unlink(resultPipe);
        return;
    }

    if (child > 0) {
        static const GrapeInfo grapeInfo[] = {
            {"Tokaji furmint", 21},
            {"Tokaji guggolós", 17},
            {"Kékfrankos", 19}
        };

        int file = open(filename, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
        if (file < 0) {
            printf("Couldn't open the file\n");
            unlink(dataPipe);
            unlink(resultPipe);
            return;
        }

        int numberOfWineRecords = countWineRecords(file);
        if (numberOfWineRecords < 1) {
            printf("File is empty\n");
            close(file);
            unlink(dataPipe);
            unlink(resultPipe);
            return;
        }

        WineRecord* wineRecords = createWineRecordArray(numberOfWineRecords, file);
        if (wineRecords == NULL) {
            close(file);
            unlink(dataPipe);
            unlink(resultPipe);
            return;
        }

        close(file);
        
        pause();

        int f = open(lengthPipe, O_WRONLY);

        write(f, &numberOfWineRecords, sizeof(int));

        close(f);

        f = open(dataPipe, O_WRONLY);
        int g = open(resultPipe, O_RDONLY);

        int foundSugarContent = 0;
        for (int i = 0; i < numberOfWineRecords; ++i) {
            printf("Record to be checked:\n");
            printWineRecord(&wineRecords[i]);
            int sugarContentGoal = findSugarContent(&wineRecords[i], grapeInfo, sizeof(grapeInfo) / sizeof(grapeInfo[0]));
            if (sugarContentGoal == -1) {
                foundSugarContent = 0;
                write(f, &foundSugarContent, sizeof(int));
                printf("Couldn't find sugar content for grape type: %s\n", wineRecords[i].grapeType);
            }
            else {
                foundSugarContent = 1;
                write(f, &foundSugarContent, sizeof(int));
                write(f, &wineRecords[i], sizeof(WineRecord));
                write(f, &sugarContentGoal, sizeof(int));

                int result;
                read(g, &result, sizeof(int));

                if (result) {
                    printf("Sugar content is adequate\n");
                }
                else {
                    printf("Sugar content is not adequate\n");
                }
            }
            printf("\n");
            freeWineRecordFields(&wineRecords[i]);
        }

        wait(NULL);

        close(f);
        close(g);

        free(wineRecords);
        unlink(dataPipe);
        unlink(resultPipe);
        unlink(lengthPipe);
    }
    else {
        sleep(1);
        kill(getppid(), SIGUSR1);

        int f = open(lengthPipe, O_RDONLY);

        int numberOfWineRecords;
        read(f, &numberOfWineRecords, sizeof(int));

        close(f);

        f = open(dataPipe, O_RDONLY);
        int g = open(resultPipe, O_WRONLY);

        for (int i = 0; i < numberOfWineRecords; ++i) {
            int foundSugarContent;

            read(f, &foundSugarContent, sizeof(int));
            if (foundSugarContent) {
                WineRecord wineRecord;
                int sugarContentGoal;

                read(f, &wineRecord, sizeof(WineRecord));
                read(f, &sugarContentGoal, sizeof(int));

                int result = isSugarContentAdequate(&wineRecord, sugarContentGoal);
                write(g, &result, sizeof(int));
            }
        }

        close(f);
        close(g);

        exit(0);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    signal(SIGUSR1, checkReadyHandler);

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
            case 5:
                printf("Check Records\n\n");
                checkWinery(filename);
                break;
            case 6: {
                printf("Exit\n");
                break;
            }
            default: {
                printf("Doesn't exist\n");
                break;
            }
        }
        printf("\n");
    } while (option != 6);

    return 0;
}
