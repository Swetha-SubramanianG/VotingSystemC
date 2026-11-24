#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Candidate {
    int id;
    char name[50];
    int votes;
};

struct Voter {
    char voterID[20];
    int hasVoted;
};

void adminMenu();
void addCandidates();
void viewCandidates();
void updateCandidateNames();
void vote();
void showResults();
void showWinner();
int alreadyVoted(char voterID[]);
int validateVoter(char voterID[]);

int main() {
    int choice;
    char adminPass[20];

    while (1) {
        printf("\n\n============ ONLINE VOTING SYSTEM ============\n");
        printf("1. Admin Login\n");
        printf("2. Vote\n");
        printf("3. View Results\n");
        printf("4. View Winner\n");
        printf("5. Exit\n");
        printf("---------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nEnter Admin Password: ");
                scanf("%s", adminPass);

                if (strcmp(adminPass, "admin123") == 0) {
                    adminMenu();
                } else {
                    printf("\n❌ Incorrect Password!\n");
                }
                break;

            case 2: vote(); break;
            case 3: showResults(); break;
            case 4: showWinner(); break;
            case 5: exit(0);
            default: printf("\nInvalid Choice!\n");
        }
    }

    return 0;
}

void adminMenu() {
    int choice;
    while (1) {
        printf("\n\n========== ADMIN MENU ==========\n");
        printf("1. Add Candidates\n");
        printf("2. Update Candidate Names\n");
        printf("3. View Candidates\n");
        printf("4. Back to Main Menu\n");
        printf("---------------------------------\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCandidates(); break;
            case 2: updateCandidateNames(); break;
            case 3: viewCandidates(); break;
            case 4: return;
            default: printf("Invalid choice!\n");
        }
    }
}

void addCandidates() {
    FILE *fp;
    struct Candidate c;
    int n, i;

    fp = fopen("candidates.dat", "wb");

    printf("\nEnter number of candidates: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("\nEnter Candidate %d ID: ", i + 1);
        scanf("%d", &c.id);
        printf("Enter Candidate %d Name: ", i + 1);
        scanf("%s", c.name);
        c.votes = 0;
        fwrite(&c, sizeof(c), 1, fp);
    }

    fclose(fp);
    printf("\n✔ Candidates added successfully!\n");
}

void updateCandidateNames() {
    FILE *fp;
    struct Candidate c;
    int id, found = 0;

    fp = fopen("candidates.dat", "rb+");

    if (fp == NULL) {
        printf("\n❌ Add candidates first!\n");
        return;
    }

    printf("\nEnter Candidate ID to Update: ");
    scanf("%d", &id);

    while (fread(&c, sizeof(c), 1, fp)) {
        if (c.id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf("%s", c.name);

            fseek(fp, -sizeof(c), SEEK_CUR);
            fwrite(&c, sizeof(c), 1, fp);
            printf("\n✔ Candidate updated successfully!\n");
            break;
        }
    }

    if (!found)
        printf("\n❌ Candidate not found!\n");

    fclose(fp);
}

void viewCandidates() {
    FILE *fp;
    struct Candidate c;

    fp = fopen("candidates.dat", "rb");

    if (fp == NULL) {
        printf("\nNo candidates found!\n");
        return;
    }

    printf("\n----- Candidate List -----\n");
    while (fread(&c, sizeof(c), 1, fp)) {
        printf("ID: %d  | Name: %s | Votes: %d\n", c.id, c.name, c.votes);
    }

    fclose(fp);
}

int validateVoter(char voterID[]) {
    FILE *fp = fopen("voters.dat", "rb");
    struct Voter v;

    if (fp == NULL)
        return 1; // allow all if file not created

    while (fread(&v, sizeof(v), 1, fp)) {
        if (strcmp(v.voterID, voterID) == 0)
            return 1;

    }

    fclose(fp);
    return 1;
}

int alreadyVoted(char voterID[]) {
    FILE *fp = fopen("voters.dat", "rb");
    struct Voter v;

    if (fp == NULL)
        return 0;

    while (fread(&v, sizeof(v), 1, fp)) {
        if (strcmp(v.voterID, voterID) == 0 && v.hasVoted == 1) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void vote() {
    FILE *fp, *fv;
    struct Candidate c;
    struct Voter v;
    char voterID[20];
    int id, found = 0;

    printf("\nEnter your Voter ID: ");
    scanf("%s", voterID);

    if (!validateVoter(voterID)) {
        printf("\n❌ Invalid Voter ID!\n");
        return;
    }

    if (alreadyVoted(voterID)) {
        printf("\n❌ You have already voted!\n");
        return;
    }

    fp = fopen("candidates.dat", "rb+");
    if (fp == NULL) {
        printf("\nAdd candidates first!\n");
        return;
    }

    printf("\n------ Candidates ------\n");
    while (fread(&c, sizeof(c), 1, fp)) {
        printf("%d - %s\n", c.id, c.name);
    }

    printf("\nEnter Candidate ID to vote: ");
    scanf("%d", &id);

    rewind(fp);

    while (fread(&c, sizeof(c), 1, fp)) {
        if (c.id == id) {
            found = 1;
            c.votes++;

            fseek(fp, -sizeof(c), SEEK_CUR);
            fwrite(&c, sizeof(c), 1, fp);
            break;
        }
    }

    fclose(fp);

    if (!found) {
        printf("\n❌ Invalid Candidate ID!\n");
        return;
    }

    fv = fopen("voters.dat", "ab");
    strcpy(v.voterID, voterID);
    v.hasVoted = 1;
    fwrite(&v, sizeof(v), 1, fv);
    fclose(fv);

    printf("\n✔ Vote Cast Successfully!\n");
}

void showResults() {
    FILE *fp;
    struct Candidate c;

    fp = fopen("candidates.dat", "rb");

    if (fp == NULL) {
        printf("\n❌ No candidate data!\n");
        return;
    }

    printf("\n====== Voting Results ======\n");
    while (fread(&c, sizeof(c), 1, fp)) {
        printf("Candidate: %-10s | Votes: %d\n", c.name, c.votes);
    }

    fclose(fp);
}

void showWinner() {
    FILE *fp;
    struct Candidate c, win;
    int first = 1;

    fp = fopen("candidates.dat", "rb");

    if (fp == NULL) {
        printf("\nNo candidates found!\n");
        return;
    }

    while (fread(&c, sizeof(c), 1, fp)) {
        if (first) {
            win = c;
            first = 0;
        }
        if (c.votes > win.votes)
            win = c;
    }

    fclose(fp);

    printf("\n===== WINNER =====\n");
    printf("Winner: %s\nVotes: %d\n", win.name, win.votes);
}
