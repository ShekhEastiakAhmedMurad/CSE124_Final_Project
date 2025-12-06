#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================
   STANDARD COLOR Setup
   =========================
*/
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define PURPLE  "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define BOLD    "\033[1m"

/* =========================
   FILE NAME CONSTANTS
   =========================
*/
#define FILE_CONTACT "contacts.txt"
#define FILE_HISTORY "history.txt"
#define FILE_FAV     "favorites.txt"

/* =========================
   DATA STRUCTURES
   =========================
*/
typedef struct Contact {
    char name[50];
    char phone[20];
    char email[50];
    struct Contact* next;
} Contact;

typedef struct CallLog {
    char name[50];
    char phone[20];
    char type[15];
    char duration[20];
    struct CallLog* next;
} CallLog;

typedef struct Favorite {
    char name[50];
    char phone[20];
    struct Favorite* next;
} Favorite;

/* =========================
   GLOBAL VARIABLE DECLARE
   =========================
*/
Contact* head = NULL;
CallLog* topStack = NULL;
Favorite* frontQ = NULL;
Favorite* rearQ = NULL;

/* =========================
   CONSOLE HELPER
   =========================
*/
// For screen clear
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/* Print cyan dashed border line */
void printLine(int len) {
    printf("\t%s", CYAN);
    for (int i = 0; i < len; i++) printf("-");
    printf("%s\n", RESET);
}


//Wait for Enter key
void pressEnter() {
    printf("\n\t%s>> Press ENTER to return...%s", YELLOW, RESET);
    while (getchar() != '\n');
    getchar();
}

/* =========================
   HELPER FUNCTIONS
   =========================
*/
//Fine Name by Number
void resolveName(char* number, char* destination) {
    Contact* temp = head;
    while (temp) {
        if (strcmp(temp->phone, number) == 0) {
            strcpy(destination, temp->name);
            return;
        }
        temp = temp->next;
    }
    strcpy(destination, "Unknown");
}

//Find Number By Name
int getNumberByName(char* name, char* destPhone) {
    Contact* temp = head;
    while (temp) {
        if (strcasecmp(temp->name, name) == 0) {
            strcpy(destPhone, temp->phone);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/* =========================
   FILE IO: Persistence
   =========================
*/
//saveData
void saveData() {
    FILE* fp;

    /* Contacts */
    fp = fopen(FILE_CONTACT, "w");
    Contact* tempC = head;
    while (tempC) {
        fprintf(fp, "%s,%s,%s\n", tempC->name, tempC->phone, tempC->email);
        tempC = tempC->next;
    }
    if (fp) fclose(fp);

    /* Call History */
    fp = fopen(FILE_HISTORY, "w");
    CallLog* tempH = topStack;
    while (tempH) {
        fprintf(fp, "%s,%s,%s,%s\n", tempH->name, tempH->phone, tempH->type, tempH->duration);
        tempH = tempH->next;
    }
    if (fp) fclose(fp);

    /* Favorites */
    fp = fopen(FILE_FAV, "w");
    Favorite* tempF = frontQ;
    while (tempF) {
        fprintf(fp, "%s,%s\n", tempF->name, tempF->phone);
        tempF = tempF->next;
    }
    if (fp) fclose(fp);

    printf("\n\t%s[Success] All Data Saved Successfully!%s\n", GREEN, RESET);
}

//loadData
void loadData() {
    FILE* fp;
    char buffer[200], f1[50], f2[20], f3[50], f4[20];

    /* Load Contacts */
    fp = fopen(FILE_CONTACT, "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            /* Parse: name,phone,email */
            sscanf(buffer, "%[^,],%[^,],%s", f1, f2, f3);
            Contact* newC = (Contact*)malloc(sizeof(Contact));
            strcpy(newC->name, f1);
            strcpy(newC->phone, f2);
            strcpy(newC->email, f3);
            newC->next = NULL;

            /* Insert sorted by name (ascending) */
            if (!head || strcmp(head->name, f1) >= 0) {
                newC->next = head;
                head = newC;
            } else {
                Contact* curr = head;
                while (curr->next && strcmp(curr->next->name, f1) < 0) curr = curr->next;
                newC->next = curr->next;
                curr->next = newC;
            }
        }
        fclose(fp);
    }

    /* Load History (stack) */
    fp = fopen(FILE_HISTORY, "r");
    if (fp) {
        CallLog* tail = NULL;
        while (fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%[^,],%[^,],%[^,],%s", f1, f2, f3, f4);
            CallLog* newH = (CallLog*)malloc(sizeof(CallLog));
            strcpy(newH->name, f1);
            strcpy(newH->phone, f2);
            strcpy(newH->type, f3);
            strcpy(newH->duration, f4);
            newH->next = NULL;
            if (topStack == NULL) { topStack = newH; tail = newH; }
            else { tail->next = newH; tail = newH; }
        }
        fclose(fp);
    }

    /* Load Favorites (queue) */
    fp = fopen(FILE_FAV, "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%[^,],%s", f1, f2);
            Favorite* newF = (Favorite*)malloc(sizeof(Favorite));
            strcpy(newF->name, f1);
            strcpy(newF->phone, f2);
            newF->next = NULL;
            if (!frontQ) frontQ = rearQ = newF;
            else { rearQ->next = newF; rearQ = newF; }
        }
        fclose(fp);
    }
}

/* =========================
   UI HEADER
   =========================
*/
void header(char* title) {
    clearScreen();
    printf("\t%s.=======================================================.%s\n", CYAN, RESET);
    printf("\t%s|%s  ____  _                      _                 _     %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s |  _ \\| |__   ___  _ __   ___| |__   ___   ___ | | __ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s | |_) | '_ \\ / _ \\| '_ \\ / _ \\ '_ \\ / _ \\ / _ \\| |/ / %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s |  __/| | | | (_) | | | |  __/ |_) | (_) | (_) |   <  %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s |_|   |_| |_|\\___/|_| |_|\\___|_.__/ \\___/ \\___/|_|\\_\\ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|                                                       |%s\n", CYAN, RESET);
    printf("\t%s|             %sPHONEBOOK MANAGEMENT SYSTEM%s               %s|%s\n", CYAN, BOLD, RESET, CYAN, RESET);
    printf("\t%s'======================================================='%s\n", CYAN, RESET);
    printf("\n\t                 %s[ %s ]%s\n\n", YELLOW, title, RESET);
}

/* =========================
   CORE FEATURES: Contacts
   =========================
*/
//addContact
void addContact() {
    header("ADD NEW CONTACT");

    char n[50], p[20], e[50];

    printf("\t%s              : Enter Contact Details :%s\n", CYAN, RESET);
    printLine(57);

    printf("\t%s Name  : %s", BOLD, RESET); scanf(" %[^\n]s", n);
    printf("\t%s Phone : %s", BOLD, RESET); scanf(" %[^\n]s", p);
    printf("\t%s Email : %s", BOLD, RESET); scanf(" %[^\n]s", e);
    printLine(57);

    // Duplicate check
    Contact* temp = head;
    while (temp) {
        if (strcmp(temp->name, n) == 0 &&
            strcmp(temp->phone, p) == 0 &&
            strcmp(temp->email, e) == 0) {
            printf("\n\t%s[!] Contact already exists!%s\n", RED, RESET);
            pressEnter();
            return;
        }
        temp = temp->next;
    }

    // New contact add
    Contact* newC = (Contact*)malloc(sizeof(Contact));
    strcpy(newC->name, n);
    strcpy(newC->phone, p);
    strcpy(newC->email, e);
    newC->next = NULL;

    // Alphabetically insert
    if (!head || strcmp(head->name, n) >= 0) {
        newC->next = head;
        head = newC;
    } else {
        Contact* curr = head;
        while (curr->next && strcmp(curr->next->name, n) < 0) curr = curr->next;
        newC->next = curr->next;
        curr->next = newC;
    }

    printf("\n\t%s[Success] Contact saved!%s\n", GREEN, RESET);
    saveData();
    pressEnter();
}
//Delete Contact
void deleteContact() {
    header("DELETE CONTACT");

    printLine(57);
    if (!head) {
        printf("\t%sNo contacts to delete.%s\n", RED, RESET);
        printLine(57);
        pressEnter();
        return;
    }

    char nameDel[50];
    printf("\n\tEnter Name of Contact to Delete: ");
    scanf(" %[^\n]", nameDel);

    Contact* temp = head;
    Contact* prev = NULL;

    while (temp) {
        if (strcmp(temp->name, nameDel) == 0) {
            if (prev == NULL) {
                head = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            printf("\n\t%s[Success] Contact Deleted!%s\n", GREEN, RESET);
            saveData();
            pressEnter();
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("\n\t%s[!] Contact Not Found!%s\n", RED, RESET);
    pressEnter();
}

//displayContacts
void displayContacts() {
    header("ALL CONTACTS (SORTED)");
    if (!head) {
        printf("\t%sNo contacts found.%s\n", RED, RESET);
        pressEnter();
        return;
    }

    printLine(57);
    printf("\t|      %s%-15s  %-18s  %-11s%s |\n", YELLOW, "NAME", "PHONE", "EMAIL", RESET);
    printLine(57);

    Contact* temp = head;
    while (temp) {
        printf("\t| %-17s  %-15s  %-17s |\n", temp->name, temp->phone, temp->email);
        printLine(57);
        temp = temp->next;
    }
    pressEnter();
}

/* =========================
   CALL SYSTEM (Dialer + History)
   =========================
*/
//processCall
void processCall(char* phoneNumber, char* type) {
    char name[50], duration[20];
    resolveName(phoneNumber, name);

    printf("\n\t%s%s Call Initiated%s\n", YELLOW, type, RESET);
    if (strcmp(name, "Unknown") != 0) {
        printf("\t%s%s (%s)%s\n\n", GREEN, name, phoneNumber, RESET);
    } else {
        printf("\t%s%s (Unknown Number)%s\n\n", GREEN, phoneNumber, RESET);
    }

    printf("\t%s[Call Connected]\n\tTalking..\n\tTalking...\n\tTalking....%s\n", BOLD, RESET);
    /* call simulation ends */
    printf("\t%s[Call Ended]%s\n\n", RED, RESET);
    printLine(57);

    printf("\tEnter Duration (e.g. 2:30) : ");
    scanf(" %[^\n]", duration);
    printLine(57);

    /* push to stack (LIFO): place new node at head */
    CallLog* newLog = (CallLog*)malloc(sizeof(CallLog));
    strcpy(newLog->name, name);
    strcpy(newLog->phone, phoneNumber);
    strcpy(newLog->type, type);
    strcpy(newLog->duration, duration);
    newLog->next = topStack;
    topStack = newLog;

    printf("\n\t%s[Saved] Call added to History.%s\n", GREEN, RESET);
    saveData();
}

//makeCall
void makeCall() {
    header("CALL DIALER");

    int choice;
    char input[50];

    printLine(57);
    printf("\t%s[1]%s Dial a Number\n", YELLOW, RESET);
    printf("\t%s[2]%s Call a Saved Contact\n", YELLOW, RESET);
    printf("\t%s[3]%s Simulate Incoming Call\n", YELLOW, RESET);
    printf("\t%s[0]%s Back\n", RED, RESET);
    printLine(57);
    printf("\n\tSelect : ");
    scanf("%d", &choice);

    if (choice == 1) {
        printLine(57);
        printf("\n\tEnter Number : ");
        scanf(" %[^\n]", input);
        processCall(input, "Outgoing");
    } else if (choice == 2) {
        printLine(57);
        printf("\n\tEnter Name : ");
        scanf(" %[^\n]", input);
        char foundPhone[20];
        if (getNumberByName(input, foundPhone)) {
            processCall(foundPhone, "Outgoing");
        } else {
            printf("\n\t%s[!] Contact not found.%s\n", RED, RESET);
        }
    } else if (choice == 3) {
        printLine(57);
        printf("\n\tEnter Incoming Number : ");
        scanf(" %[^\n]", input);
        processCall(input, "Incoming");
    } else if (choice == 0) {
        return;
    }

    pressEnter();
}

//viewHistory
void viewHistory() {
    header("CALL HISTORY (STACK)");
    if (!topStack) {
        printf("\t%sNo recent calls.%s\n", RED, RESET);
        pressEnter();
        return;
    }

    printLine(57);
    printf("\t|   %s%-12s %-17s %-13s %-6s%s |\n", YELLOW, "TYPE", "NAME", "NUMBER", "TIME", RESET);
    printLine(57);

    CallLog* temp = topStack;
    while (temp) {
        char* color = (strcmp(temp->type, "Incoming") == 0) ? GREEN : BLUE;
        printf("\t| %s%-12s%s %-16s %-15s %-7s |\n", color, temp->type, RESET, temp->name, temp->phone, temp->duration);
        printLine(57);
        temp = temp->next;
    }
    pressEnter();
}
//Delete History
void deleteLastCall() {
    header("DELETE CALL HISTORY");

    if (!topStack) {
        printf("\t%sNo call history to delete.%s\n", RED, RESET);
        pressEnter();
        return;
    }

    // Display call history with serial numbers
    CallLog* temp = topStack;
    int count = 1;
    printLine(57);
    printf("\t| %s%-6s %-10s %-16s %-13s %-4s%s |\n", YELLOW, "No.", "TYPE", "NAME", "NUMBER", "TIME", RESET);
    printLine(57);
    while (temp) {
        char* color = (strcmp(temp->type, "Incoming") == 0) ? GREEN : BLUE;
        printf("\t| %-4d %s%-10s%s %-16s %-14s %-5s |\n", count, color, temp->type, RESET, temp->name, temp->phone, temp->duration);
        printLine(57);
        temp = temp->next;
        count++;
    }

    // Ask user which call to delete
    int delNo;
    printf("\n\tEnter Serial Number to Delete : ");
    scanf("%d", &delNo);

    if (delNo < 1 || delNo >= count) {
        printf("\n\t%s[!] Invalid Serial Number !%s\n", RED, RESET);
        pressEnter();
        return;
    }

    // Delete selected node
    temp = topStack;
    CallLog* prev = NULL;
    count = 1;
    while (temp) {
        if (count == delNo) {
            if (prev == NULL) {
                // delete first node
                topStack = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            printf("\n\t%s[Success] Selected Call Deleted!%s\n", GREEN, RESET);
            saveData();
            pressEnter();
            return;
        }
        prev = temp;
        temp = temp->next;
        count++;
    }
}

/* =========================
   FAVORITES (Queue)
   =========================
*/
//addFavorite
void addFavorite() {
    header("ADD TO FAVORITES");

    int choice;
    char nameInput[50], phoneInput[20];

    printLine(57);
    printf("\t%s[1]%s Select from Contact List\n", YELLOW, RESET);
    printf("\t%s[2]%s Add New Number Manually\n", YELLOW, RESET);
    printf("\t%s[0]%s Back\n", RED, RESET);
    printLine(57);
    printf("\n\tSelect : ");
    scanf("%d", &choice);
    printLine(57);

    if (choice == 1) {
        printf("\n\tEnter Name to Search : ");
        scanf(" %[^\n]", nameInput);

        if (getNumberByName(nameInput, phoneInput)) {
            printf("\n\t%s[Found] %s (%s)%s\n\n", GREEN, nameInput, phoneInput, RESET);
            printLine(57);
        } else {
            printf("\n\t%s[!] Contact not found in list.%s\n\n", RED, RESET);
            printLine(57);
            pressEnter();
            return;
        }
    } else if (choice == 2) {
        printf("\n\tEnter Name : "); scanf(" %[^\n]", nameInput);
        printf("\tEnter Phone : "); scanf(" %[^\n]", phoneInput);
    } else {
        return;
    }

    Favorite* newF = (Favorite*)malloc(sizeof(Favorite));
    strcpy(newF->name, nameInput);
    strcpy(newF->phone, phoneInput);
    newF->next = NULL;

    if (!frontQ) frontQ = rearQ = newF;
    else { rearQ->next = newF; rearQ = newF; }

    printf("\n\t%s[Success] Added to Favorites Queue!%s\n", GREEN, RESET);
    saveData();
    pressEnter();
}
//Delete Favorites
void deleteFavorite() {
    header("DELETE FAVORITE");

    printLine(57);
    if (!frontQ) {
        printf("\t%sNo favorites to delete.%s\n", RED, RESET);
        pressEnter();
        return;
    }

    char nameDel[50];
    printf("\n\tEnter Name of Favorite to Delete: ");
    scanf(" %[^\n]", nameDel);

    Favorite* temp = frontQ;
    Favorite* prev = NULL;

    while (temp) {
        if (strcmp(temp->name, nameDel) == 0) {
            if (prev == NULL) {
                frontQ = temp->next;
                if (temp == rearQ) rearQ = NULL;
            } else {
                prev->next = temp->next;
                if (temp == rearQ) rearQ = prev;
            }
            free(temp);
            printf("\n\t%s[Success] Favorite Deleted!%s\n", GREEN, RESET);
            saveData();
            pressEnter();
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    printf("\n\t%s[!] Favorite Not Found!%s\n", RED, RESET);
    pressEnter();
}


//viewFavorites
void viewFavorites() {
    header("FAVORITES (QUEUE)");

    printLine(57);
    if (!frontQ) {
        printf("\t%sFavorite list is empty.%s\n", RED, RESET);
        pressEnter();
        return;
    }

    printf("\t|           %s%-20s  %-21s%s |\n", YELLOW, "NAME", "PHONE", RESET);
    printLine(57);

    Favorite* temp = frontQ;
    while (temp) {
        printf("\t|       %-20s  %-25s |\n", temp->name, temp->phone);
        printLine(57);
        temp = temp->next;
    }
    pressEnter();
}
/*
Project Member :
@ Shekh Eastiak Ahmed Murad -- 251-15-172 -- 68_L2
@         Jannatin Nure Mim -- 251-15-183 -- 68_L2
@          MD Yeasin Rashid -- 251-15-787 -- 68_L2
@     Md. Azizul Hakim Nahol-- 251-15-851 -- 68_L2
*/
/* =========================
   MAIN FUNCTION
   =========================
*/
int main() {
    clearScreen();
    loadData();

    int choice;

    while (1) {
        clearScreen();

        /* Menu Bar */
        printf("\t%s.=======================================================.%s\n", CYAN, RESET);
        printf("\t%s|%s  ____  _                      _                 _     %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s |  _ \\| |__   ___  _ __   ___| |__   ___   ___ | | __ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s | |_) | '_ \\ / _ \\| '_ \\ / _ \\ '_ \\ / _ \\ / _ \\| |/ / %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s |  __/| | | | (_) | | | |  __/ |_) | (_) | (_) |   <  %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s |_|   |_| |_|\\___/|_| |_|\\___|_.__/ \\___/ \\___/|_|\\_\\ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|             %sPHONEBOOK MANAGEMENT SYSTEM%s               %s|%s\n", CYAN, BOLD, RESET, CYAN, RESET);
        printf("\t%s|=======================================================|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[1]%s Add New Contact                          %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[2]%s Display Contacts                         %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[3]%s Delete Contact                           %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|-------------------------------------------------------|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[4]%s Make a Call                              %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[5]%s Call History                             %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[6]%s Delete Last Call                         %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|-------------------------------------------------------|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[7]%s Add Favorite                             %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[8]%s View Favorites                           %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[9]%s Delete Favorite                          %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|-------------------------------------------------------|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[0] EXIT%s                                     %s|%s\n", CYAN, RED, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s'======================================================='%s\n\n", CYAN, RESET);
        printf("\t Select Option : ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: addContact(); break;
            case 2: displayContacts(); break;
            case 3: deleteContact(); break;
            case 4: makeCall(); break;
            case 5: viewHistory(); break;
            case 6: deleteLastCall(); break;
            case 7: addFavorite(); break;
            case 8: viewFavorites(); break;
            case 9: deleteFavorite(); break;
            case 0:
                saveData();
                printf("\n\t%sExiting System... Goodbye!%s\n", PURPLE, RESET);
                exit(0);
            default:
                printf("\n\t%sInvalid Choice!%s\n", RED, RESET);
        }
    }

    return 0;
}
