#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FILENAME "wallet.txt"
#define MAX_CARD_NUMBER 20
#define MAX_CARDS 100
#define MAX_NAME_LENGTH 50

// ANSI color codes
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

// Card structure definition
typedef struct {
    char number[MAX_CARD_NUMBER];
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
    float balance;
} Card;

// Function to read all cards from file into an array
int read_cards_from_file(Card cards[], int max_cards) {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        return 0;
    }

    int count = 0;
    while (count < max_cards && fscanf(file, "%s %s %s %f", 
           cards[count].number, 
           cards[count].firstName, 
           cards[count].lastName, 
           &cards[count].balance) == 4) {
        count++;
    }

    fclose(file);
    return count;
}

// Function to write cards array to file
void write_cards_to_file(Card cards[], int count) {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf(COLOR_RED "Could not open wallet file for writing!\n" COLOR_RESET);
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s %s %.2f\n", 
                cards[i].number, 
                cards[i].firstName, 
                cards[i].lastName, 
                cards[i].balance);
    }

    fclose(file);
}

void display_help() {
    printf(COLOR_BLUE "Wallet commands:\n" COLOR_RESET);
    printf(COLOR_YELLOW "  add <number> <first_name> <last_name> <balance> - Add a new card\n");
    printf("  view - View all cards\n");
    printf("  help - Show commands\n");
    printf("  remove - Remove card details\n" COLOR_RESET);
}

int is_valid_card_number(const char* number) {
    for (int i = 0; number[i] != '\0'; i++) {
        if (!isdigit(number[i])) {
            return 0;
        }
    }
    return 1;
}

void add_card(int argc, char* argv[]) {
    if (argc != 6) {
        printf(COLOR_RED "Usage: %s add <card_number> <first_name> <last_name> <balance>\n" COLOR_RESET, argv[0]);
        return;
    }

    // Validate card number contains only digits
    if (!is_valid_card_number(argv[2])) {
        printf(COLOR_RED "Error: Card number must contain only digits!\n" COLOR_RESET);
        return;
    }

    // Read existing cards
    Card cards[MAX_CARDS];
    int card_count = read_cards_from_file(cards, MAX_CARDS);

    // Check if card already exists
    for (int i = 0; i < card_count; i++) {
        if (strcmp(cards[i].number, argv[2]) == 0) {
            printf(COLOR_RED "Error: Card number already exists!\n" COLOR_RESET);
            return;
        }
    }

    // Add new card
    if (card_count < MAX_CARDS) {
        strncpy(cards[card_count].number, argv[2], MAX_CARD_NUMBER - 1);
        cards[card_count].number[MAX_CARD_NUMBER - 1] = '\0';
        
        strncpy(cards[card_count].firstName, argv[3], MAX_NAME_LENGTH - 1);
        cards[card_count].firstName[MAX_NAME_LENGTH - 1] = '\0';
        
        strncpy(cards[card_count].lastName, argv[4], MAX_NAME_LENGTH - 1);
        cards[card_count].lastName[MAX_NAME_LENGTH - 1] = '\0';
        
        cards[card_count].balance = atof(argv[5]);
        card_count++;

        // Write updated cards back to file
        write_cards_to_file(cards, card_count);
        printf(COLOR_GREEN "Card added successfully!\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "Error: Maximum number of cards reached!\n" COLOR_RESET);
    }
}

void view_cards() {
    Card cards[MAX_CARDS];
    int card_count = read_cards_from_file(cards, MAX_CARDS);

    if (card_count == 0) {
        printf(COLOR_YELLOW "No cards found.\n" COLOR_RESET);
        return;
    }

    printf(COLOR_BLUE "\nYour cards:\n" COLOR_RESET);
    printf(COLOR_YELLOW "Number              Name                    Balance\n");
    printf("------------------------------------------------\n" COLOR_RESET);

    for (int i = 0; i < card_count; i++) {
        printf("%-18s %-12s %-12s %.2f\n", 
               cards[i].number, 
               cards[i].firstName, 
               cards[i].lastName, 
               cards[i].balance);
    }
}

void remove_card(int argc, char* argv[]) {
    if (argc != 3) {
        printf(COLOR_RED "Usage: %s remove <card_number>\n" COLOR_RESET, argv[0]);
        return;
    }

    // Read existing cards
    Card cards[MAX_CARDS];
    int card_count = read_cards_from_file(cards, MAX_CARDS);
    int found = 0;

    // Find and remove the card
    for (int i = 0; i < card_count; i++) {
        if (strcmp(cards[i].number, argv[2]) == 0) {
            // Shift remaining cards
            for (int j = i; j < card_count - 1; j++) {
                cards[j] = cards[j + 1];
            }
            card_count--;
            found = 1;
            break;
        }
    }

    if (found) {
        // Write updated cards back to file
        write_cards_to_file(cards, card_count);
        printf(COLOR_GREEN "Card removed successfully!\n" COLOR_RESET);
    } else {
        printf(COLOR_YELLOW "Card not found!\n" COLOR_RESET);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        display_help();
        return 0;
    }

    if (strcmp(argv[1], "help") == 0) {
        display_help();
    } else if (strcmp(argv[1], "add") == 0) {
        add_card(argc, argv);
    } else if (strcmp(argv[1], "view") == 0) {
        view_cards();
    } else if (strcmp(argv[1], "remove") == 0) {
        remove_card(argc, argv);
    } else {
        printf(COLOR_RED "Unknown command. Use 'help' to see available commands!\n" COLOR_RESET);
    }

    return 0;
}