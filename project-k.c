#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <conio.h> 
#define MAX_ROOMS 10
#define ROOM_COST 2000
void reviewMenu(const char* city, const char* hotelName) {
    int choice;
    char filename[100], review[500];
    FILE *file;

    snprintf(filename, sizeof(filename), "%s_%s_reviews.txt", city, hotelName); // unique per city + hotel

    printf("\n--- Review Menu for %s, %s ---\n", city, hotelName);
    printf("1. View Reviews\n");
    printf("2. Add a Review\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // consume newline

    if (choice == 1) {
        file = fopen(filename, "r");
        if (file == NULL) {
            printf("No reviews found yet.\n");
            return;
        }

        printf("\n--- Reviews for %s, %s ---\n", city, hotelName);
        while (fgets(review, sizeof(review), file)) {
            printf("- %s", review);
        }
        fclose(file);
    } else if (choice == 2) {
        printf("Enter your review: ");
        fgets(review, sizeof(review), stdin);

        file = fopen(filename, "a");
        if (file == NULL) {
            printf("Error saving review.\n");
            return;
        }

        fprintf(file, "%s", review);
        fclose(file);
        printf("✅ Review added successfully.\n");
    } else {
        printf("Invalid choice.\n");
    }
}

void addMoneyToWallet(const char* username) {
    char passwordInput[50], fileUsername[50], filePassword[50];
    int alreadyAdded = 0;

    // Check if money already added
    char flagFile[100];
    sprintf(flagFile, "%s_addmoneyflag.txt", username);
    FILE* flag = fopen(flagFile, "r");
    if (flag) {
        fscanf(flag, "%d", &alreadyAdded);
        fclose(flag);
    }

    if (alreadyAdded == 1) {
        printf("You have already added money once. This action is allowed only once.\n");
        return;
    }

    // Ask password to confirm
    printf("Enter your password to confirm: ");
    scanf("%s", passwordInput);

    FILE* userFile = fopen("users.txt", "r");
    if (!userFile) {
        printf("Error: User file not found.\n");
        return;
    }

    int found = 0;
    while (fscanf(userFile, "%s %s", fileUsername, filePassword) != EOF) {
        if (strcmp(fileUsername, username) == 0) {
            found = 1;
            break;
        }
    }
    fclose(userFile);

    if (!found || strcmp(filePassword, passwordInput) != 0) {
        printf("Incorrect password. Transaction denied.\n");
        return;
    }

    int amount;
    printf("Enter amount to add (Max Rs. 10000): ");
    scanf("%d", &amount);

    if (amount <= 0 || amount > 10000) {
        printf("Invalid amount. Must be between Rs.1 and Rs.10000.\n");
        return;
    }

    // Open wallet and update balance
    char walletFile[100];
    sprintf(walletFile, "%s_wallet.txt", username);

    FILE* wallet = fopen(walletFile, "r+");
    if (!wallet) {
        printf("Error: Wallet file not found.\n");
        return;
    }

    int balance;
    fscanf(wallet, "%d", &balance);
    balance += amount;

    rewind(wallet);
    fprintf(wallet, "%d\n", balance);
    fclose(wallet);

    // Mark that money was added once
    flag = fopen(flagFile, "w");
    fprintf(flag, "1");
    fclose(flag);

    // Log in portfolio
    char portfolioFile[100];
    sprintf(portfolioFile, "%s_portfolio.txt", username);
    FILE* pf = fopen(portfolioFile, "a");
    if (pf) {
        fprintf(pf, "Wallet Recharge: Rs.%d added. New Balance: Rs.%d\n", amount, balance);
        fclose(pf);
    }

    printf("Rs.%d added successfully! New balance: Rs.%d\n", amount, balance);
}

void showWalletBalance(const char* username) {
    char walletFile[100];
    sprintf(walletFile, "%s_wallet.txt", username);

    FILE* wfile = fopen(walletFile, "r");
    if (!wfile) {
        printf("Error: Wallet file not found.\n");
        return;
    }

    int balance;
    if (fscanf(wfile, "%d", &balance) == 1) {
        printf("Wallet Balance for %s: Rs.%d\n", username, balance);
    } else {
        printf("Error: Could not read balance.\n");
    }

    fclose(wfile);
}

typedef struct {
    int roomNumber;
    int isBooked; // 0 = available, 1 = booked
    char bookedBy[50]; // Username of who booked
} Room;
void bookRoom(const char* city, const char* hotel, const char* username) {
    Room rooms[MAX_ROOMS];// Array to hold room status
    char filename[100];
    // Ensure RoomStatus directory exists
    #ifdef _WIN32
        system("if not exist RoomStatus mkdir RoomStatus");
    #else
        system("mkdir -p RoomStatus");
    #endif
    sprintf(filename, "RoomStatus/%s_%s_bookings.txt", city, hotel);

    // Initialize all rooms as available
    for (int i = 0; i < MAX_ROOMS; i++) {
        rooms[i].roomNumber = i + 1;
        rooms[i].isBooked = 0;
        strcpy(rooms[i].bookedBy, "none");
    }

    // Load existing bookings if file exists
    FILE* file = fopen(filename, "r");
    if (file) {
        int roomNum, isBooked;
        char bookedBy[50];
        while (fscanf(file, "%d %d %s\n", &roomNum, &isBooked, bookedBy) == 3) {
            if (roomNum >= 1 && roomNum <= MAX_ROOMS) {
                rooms[roomNum - 1].isBooked = isBooked;
                strcpy(rooms[roomNum - 1].bookedBy, bookedBy);
            }
        }
        fclose(file);
    }

    // Show room map
    printf("\nRoom Availability (1-10):\n");
    for (int i = 0; i < MAX_ROOMS; i++) {
        printf("Room %2d [%s]\n", rooms[i].roomNumber, rooms[i].isBooked ? "Booked" : "Available");
    }

    // Ask for room to book
    int roomToBook;
    printf("\nEnter room number to book (1-10): ");
    scanf("%d", &roomToBook);
    if (roomToBook < 1 || roomToBook > MAX_ROOMS) {
        printf("Error: Invalid room number.\n");
        return;
    }

    if (rooms[roomToBook - 1].isBooked) {
        printf("Error: Room already booked by %s.\n", rooms[roomToBook - 1].bookedBy);
        return;
    }

    // Deduct money from wallet
    char walletFile[100];
    sprintf(walletFile, "%s_wallet.txt", username);

    FILE* wfile = fopen(walletFile, "r+");
    if (!wfile) {
        printf("Error: Wallet file not found.\n");
        return;
    }

    int balance;
    fscanf(wfile, "%d", &balance);
    if (balance < ROOM_COST) {
        printf("Error: Not enough balance. Rs.%d in wallet.\n", balance);
        fclose(wfile);
        return;
    }

    // Deduct money and update wallet file correctly
    balance -= ROOM_COST;
    fclose(wfile);
    wfile = fopen(walletFile, "w");
    if (!wfile) {
        printf("Error: Could not update wallet file.\n");
        return;
    }
    fprintf(wfile, "%d\n", balance);
    fclose(wfile);

    // Save new room status
    rooms[roomToBook - 1].isBooked = 1;
    strcpy(rooms[roomToBook - 1].bookedBy, username);
    file = fopen(filename, "w");
    if (file) {
        for (int i = 0; i < MAX_ROOMS; i++) {
            fprintf(file, "%d %d %s\n", rooms[i].roomNumber, rooms[i].isBooked, rooms[i].bookedBy);
        }
        fclose(file);
    } else {
        printf("Error: Could not update room status file.\n");
        return;
    }

    // Save to user's portfolio
    char portfolioFile[100];
    sprintf(portfolioFile, "%s_portfolio.txt", username);
    FILE* pf = fopen(portfolioFile, "a");
    fprintf(pf, "Hotel Booking: %s (%s), Room %d, Rs.%d\n", hotel, city, roomToBook, ROOM_COST);
    fclose(pf);

    printf("Success: Room %d booked successfully at %s, %s!\n", roomToBook, hotel, city);
    printf("Info: Rs.%d deducted. New wallet balance: Rs.%d\n", ROOM_COST, balance);
}

void getPassword(char* password) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();  // get one char without echo
        if (ch == 13) { // Enter key
            break;
        } else if (ch == 8 && i > 0) { // Backspace
            printf("\b \b");
            i--;
        } else if (i < 49 && ch != 8) { // Normal characters
            password[i++] = ch;
            printf("#");
        }
    }
    password[i] = '\0';
    printf("\n");
}


// Function to check if a user exists and password is correct
bool login(const char* username, const char* password) {
    FILE* fp = fopen("users.txt", "r");
    if (!fp) return false;

    char storedUsername[50], storedPassword[50];
    while (fscanf(fp, "%s %s", storedUsername, storedPassword) != EOF) {
        if (strcasecmp(storedUsername, username) == 0 && strcmp(storedPassword, password) == 0) {
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}

// Function to check if username already exists
bool userExists(const char* username) {
    FILE* fp = fopen("users.txt", "r");
    if (!fp) return false;

    char storedUsername[50], storedPassword[50];
    while (fscanf(fp, "%s %s", storedUsername, storedPassword) != EOF) {
        if (strcasecmp(storedUsername, username) == 0) {
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}

// Login or Signup handler
void createWalletFile(const char* username) {
    char filename[100];
    sprintf(filename, "%s_wallet.txt", username);

    FILE* wf = fopen(filename, "r");
    if (wf == NULL) {  // Wallet doesn't exist
        wf = fopen(filename, "w");
        if (wf != NULL) {
            fprintf(wf, "10000\n");  // Initial balance
            fclose(wf);
        } else {
            printf("Error: Failed to create wallet file.\n");
        }
    } else {
        fclose(wf);  // Wallet already exists
    }
}

void loginOrSignup(char* loggedInUsername) {
    int choice;
    char username[50], password[50];

    while (1) {
        printf("\n1. Login\n");
        printf("2. Signup\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();  // consume newline

        printf("Enter username: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = 0;
        printf("Enter password: ");
        getPassword(password);  // Masked input

        if (choice == 1) {
            if (login(username, password)) {
                printf("Login successful! Welcome, %s.\n", username);

                createWalletFile(username);  // Create wallet file if it doesn't exist

                break;
            } else {
                printf("Error: Incorrect username or password. Try again.\n");
            }
        } else if (choice == 2) {
            if (userExists(username)) {
                printf("Error: Username already exists. Try a different one.\n");
            } else {
                FILE* fp = fopen("users.txt", "a");
                fprintf(fp, "%s %s\n", username, password);
                fclose(fp);

                // Create portfolio file
                char filename[100];
                sprintf(filename, "%s_portfolio.txt", username);
                FILE* pf = fopen(filename, "w");
                if (pf) {
                    fprintf(pf, "User: %s\nWallet: 10000\n", username);
                    fclose(pf);
                }

                createWalletFile(username);  // Create wallet on signup too

                printf("Signup successful! Please log in now.\n");
            }
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    // Save username for later use
    strcpy(loggedInUsername, username);
}

// Cancel a booking for a given city and hotel by the logged-in user
void cancelBooking(const char* city, const char* hotelName, const char* username) {
    Room rooms[MAX_ROOMS];
    char filename[100];
    sprintf(filename, "RoomStatus/%s_%s_bookings.txt", city, hotelName);

    // Load existing bookings
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No bookings found for this hotel.\n");
        return;
    }

    // Initialize all rooms as available
    for (int i = 0; i < MAX_ROOMS; i++) {
        rooms[i].roomNumber = i + 1;
        rooms[i].isBooked = 0;
        strcpy(rooms[i].bookedBy, "none");
    }

    int roomNum, isBooked;
    char bookedBy[50];
    while (fscanf(file, "%d %d %s\n", &roomNum, &isBooked, bookedBy) == 3) {
        if (roomNum >= 1 && roomNum <= MAX_ROOMS) {
            rooms[roomNum - 1].isBooked = isBooked;
            strcpy(rooms[roomNum - 1].bookedBy, bookedBy);
        }
    }
    fclose(file);

    // Show user's booked rooms
    int found = 0;
    printf("\nYour booked rooms in %s, %s:\n", hotelName, city);
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i].isBooked && strcmp(rooms[i].bookedBy, username) == 0) {
            printf("Room %d\n", rooms[i].roomNumber);
            found = 1;
        }
    }
    if (!found) {
        printf("You have no bookings to cancel in this hotel.\n");
        return;
    }

    // Ask which room to cancel
    int roomToCancel;
    printf("Enter room number to cancel: ");
    scanf("%d", &roomToCancel);
    if (roomToCancel < 1 || roomToCancel > MAX_ROOMS) {
        printf("Invalid room number.\n");// Error handling
        return;
    }
    if (!rooms[roomToCancel - 1].isBooked || strcmp(rooms[roomToCancel - 1].bookedBy, username) != 0) {
        printf("You have not booked this room.\n");
        return;
    }

    // Cancel booking
    rooms[roomToCancel - 1].isBooked = 0;
    strcpy(rooms[roomToCancel - 1].bookedBy, "none");

    // Refund to wallet
    char walletFile[100];
    sprintf(walletFile, "%s_wallet.txt", username);
    FILE* wfile = fopen(walletFile, "r+");
    if (!wfile) {
        printf("Error: Wallet file not found for refund.\n");
        return;
    }
    int balance;
    fscanf(wfile, "%d", &balance);
    balance += 500;// Assuming Rs.500 refund for cancellation
    fclose(wfile);
    wfile = fopen(walletFile, "w");
    if (wfile) {
        fprintf(wfile, "%d\n", balance);
        fclose(wfile);
    }

    // Save updated room status
    file = fopen(filename, "w");
    if (file) {
        for (int i = 0; i < MAX_ROOMS; i++) {
            fprintf(file, "%d %d %s\n", rooms[i].roomNumber, rooms[i].isBooked, rooms[i].bookedBy);
        }
        fclose(file);
    }

    // Optionally, update user's portfolio (remove/cancel entry)
    // For simplicity, just append a cancellation record
    char portfolioFile[100];
    sprintf(portfolioFile, "%s_portfolio.txt", username);
    FILE* pf = fopen(portfolioFile, "a");
    if (pf) {
        fprintf(pf, "Booking Cancelled: %s (%s), Room %d, Refund Rs.%d\n", hotelName, city, roomToCancel, 500);
        fclose(pf);
    }

    printf("Booking for Room %d at %s, %s cancelled. Rs.%d refunded. New wallet balance: Rs.%d\n",
           roomToCancel, hotelName, city, ROOM_COST, balance);
}
// void cancelBooking(const char* city, const char* hotelName) {
//     printf("Cancel booking feature not implemented yet for %s, %s.\n", city, hotelName);
// }

// Function prototype for orderFood
void orderFood(const char* city, const char* hotelName, const char* username);

void hotelBookingMenu(const char* city, const char* hotelName, const char* loggedInUsername) {
    int choice = 0;
    do {
        printf("\n--- Hotel Menu for %s, %s ---\n", city, hotelName);
        printf("1. Book a Room\n");
        printf("2. Cancel a Booking\n");
        printf("3. View/Add Review\n");
        printf("4. Order Food\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline left by scanf

        switch (choice) {
            case 1:
                printf("Booking a Room...\n");
                bookRoom(city, hotelName, loggedInUsername);
                break;
            case 2:
                printf("Canceling a Booking...\n");
                cancelBooking(city, hotelName, loggedInUsername);
                break;
            case 3:
                printf("Review Options...\n");
                reviewMenu(city, hotelName);
                break;
            case 4:
                printf("Ordering Food...\n");
                orderFood(city, hotelName, loggedInUsername);
                break;
            case 5:
                printf("Returning to Main Menu...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);
}
 


void food_ordering(char username[]) {
    char walletFileName[50], portfolioFileName[50];
    int current_balance, total = 0, choice, quantity;

    struct Food {
        char name[30];
        int price;
    };

    struct Food menu[] = {
        {"Biryani", 180},
        {"Pizza", 150},
        {"Burger", 100},
        {"Coffee", 60},
        {"Ice Cream", 80}
    };

    // File paths
    sprintf(walletFileName, "%s_wallet.txt", username);
    sprintf(portfolioFileName, "%s_portfolio.txt", username);

    // Load wallet balance
    FILE *wallet = fopen(walletFileName, "r");
    if (!wallet) {
        printf("Could not open wallet file.\n");
        return;
    }
    fscanf(wallet, "%d", &current_balance);
    fclose(wallet);

    printf("\n=========== Food Menu ===========\n");
    for (int i = 0; i < 5; i++)
        printf("%d. %s - Rs. %d\n", i + 1, menu[i].name, menu[i].price);
    printf("6. Done Ordering\n");

    while (1) {
        printf("\nEnter your choice (1-6): ");
        scanf("%d", &choice);

        if (choice == 6) break;
        if (choice < 1 || choice > 5) {
            printf("Invalid choice.\n");
            continue;
        }

        printf("Enter quantity for %s: ", menu[choice - 1].name);
        scanf("%d", &quantity);

        int cost = menu[choice - 1].price * quantity;

        if (cost > current_balance) {
            printf("Not enough balance! Skipping item.\n");
        } else {
            total += cost;
            current_balance -= cost;
            printf("Added %s x%d = Rs. %d\n", menu[choice - 1].name, quantity, cost);
        }
    }

    if (total == 0) {
        printf("No food ordered.\n");
        return;
    }

    // Update wallet
    wallet = fopen(walletFileName, "w");
    if (!wallet) {
        printf("Failed to update wallet.\n");
        return;
    }
    fprintf(wallet, "%d", current_balance);
    fclose(wallet);

    // Log to portfolio
    FILE *portfolio = fopen(portfolioFileName, "a");
    if (!portfolio) {
        printf("Failed to update portfolio.\n");
        return;
    }
    fprintf(portfolio, "Food ordered: Rs. %d\n", total);
    fclose(portfolio);

    printf("\n✅ Order successful! Total food bill: Rs. %d\n", total);
}
void orderFood(const char* city, const char* hotelName, const char* username) {
    printf("Ordering food for %s at %s, %s...\n", username, hotelName, city);
    food_ordering((char*)username);
}
void showIndiaMap() {
    printf("\n");
    printf("                 * Kashmir\n");
    printf("                    |\n");
    printf("         * Chandigarh    * Delhi\n");
    printf("              |            |\n");
    printf("         * Jaipur       * Lucknow\n");
    printf("              |            |\n");
    printf("        * Ahmedabad    * Patna\n");
    printf("              |            |\n");
    printf("          * Mumbai           \n");
    printf("             |               \n");
    printf("         * Pune          * Hyderabad\n");
    printf("                           |\n");
    printf("                     * Bangalore\n");
    printf("                           |\n");
    printf("                      * Chennai\n");
    printf("\n");
    printf("Note: Enter city exactly as shown above.\n");
}
 
int showMainMenu() {
    int choice;

    printf("\n--- Main Menu ---\n");
    printf("1. Hotel Booking\n");
    printf("2. Check Wallet Balance\n");
     printf("3. Add Money to Wallet\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    return choice;
}
 
struct Hotel {
    char name[50];
    int x;  // horizontal position (0-19)
    int y;  // vertical position (0-6)
};

void showHotelMap(char city[]) {
    struct Hotel hotels[10];  // max 10 hotels, just in case
    int count = 0;

    if (strcmp(city, "Kashmir") == 0) {
        struct Hotel temp[] = {
            {"Hotel Lalit Grand Palace", 4, 1},
            {"The Grand Mumtaz", 8, 2},
            {"Hotel Heevan Retreat", 12, 3},
            {"Vivanta Dal View", 6, 4},
            {"Hotel Pine Palace", 14, 1},
            {"Khyber Himalayan Resort", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Kashmir:\n\n");
    }
    else if (strcmp(city, "Chandigarh") == 0) {
        struct Hotel temp[] = {
            {"JW Marriott", 3, 1},
            {"The Lalit", 7, 2},
            {"Hotel Mountview", 11, 3},
            {"Hyatt Regency", 15, 1},
            {"Taj Chandigarh", 5, 5},
            {"Fortune", 13, 4}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Chandigarh:\n\n");
    }
    else if (strcmp(city, "Jaipur") == 0) {
        struct Hotel temp[] = {
            {"Rambagh Palace", 4, 1},
            {"Fairmont Jaipur", 8, 2},
            {"Trident Jaipur", 12, 3},
            {"ITC Rajputana", 6, 4},
            {"Hilton Jaipur", 14, 1},
            {"Alsisar Haveli", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Jaipur:\n\n");
    }
    else if (strcmp(city, "Ahmedabad") == 0) {
        struct Hotel temp[] = {
            {"The House of MG", 4, 1},
            {"Hyatt Ahmedabad", 8, 2},
            {"Courtyard Marriott", 12, 3},
            {"Novotel", 6, 4},
            {"Fortune Park", 14, 1},
            {"Lemon Tree", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Ahmedabad:\n\n");
    }
    else if (strcmp(city, "Mumbai") == 0) {
        struct Hotel temp[] = {
            {"The Taj Mahal Palace", 3, 1},
            {"The Oberoi", 7, 2},
            {"Trident Nariman Point", 11, 3},
            {"ITC Maratha", 15, 4},
            {"The St. Regis", 5, 5},
            {"Four Seasons", 13, 2}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Mumbai:\n\n");
    }
    else if (strcmp(city, "Pune") == 0) {
        struct Hotel temp[] = {
            {"The Westin", 3, 1},
            {"Hyatt Pune", 7, 2},
            {"Marriott Suites", 11, 3},
            {"Radisson Blu", 15, 4},
            {"Sheraton Grand", 5, 5},
            {"Le Meridien", 13, 2}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Pune:\n\n");
    }
    else if (strcmp(city, "Delhi") == 0) {
        struct Hotel temp[] = {
            {"The Imperial", 4, 1},
            {"Taj Mahal Hotel", 8, 2},
            {"The Leela Palace", 12, 3},
            {"ITC Maurya", 6, 4},
            {"Shangri-La Eros", 14, 1},
            {"JW Marriott", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Delhi:\n\n");
    }
    else if (strcmp(city, "Lucknow") == 0) {
        struct Hotel temp[] = {
            {"Clarks Avadh", 4, 1},
            {"Taj Mahal Lucknow", 8, 2},
            {"Lebua Lucknow", 12, 3},
            {"Hyatt Regency", 6, 4},
            {"Ramada", 14, 1},
            {"Lemon Tree", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Lucknow:\n\n");
    }
    else if (strcmp(city, "Patna") == 0) {
        struct Hotel temp[] = {
            {"The Patna Marriott", 4, 1},
            {"Hotel Chanakya", 8, 2},
            {"Hotel Kautilya", 12, 3},
            {"Hotel Gargee Grand", 6, 4},
            {"Mithila Residency", 14, 1},
            {"Hotel Patliputra", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Patna:\n\n");
    }
    else if (strcmp(city, "Hyderabad") == 0) {
        struct Hotel temp[] = {
            {"Taj Falaknuma", 4, 1},
            {"ITC Kakatiya", 8, 2},
            {"Park Hyatt", 12, 3},
            {"Novotel", 6, 4},
            {"Trident", 14, 1},
            {"Marriott", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Hyderabad:\n\n");
    }
    else if (strcmp(city, "Bangalore") == 0) {
        struct Hotel temp[] = {
            {"The Leela Palace", 4, 1},
            {"Taj West End", 8, 2},
            {"ITC Gardenia", 12, 3},
            {"The Ritz Carlton", 6, 4},
            {"Shangri-La", 14, 1},
            {"JW Marriott", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Bangalore:\n\n");
    }
    else if (strcmp(city, "Chennai") == 0) {
        struct Hotel temp[] = {
            {"The Leela Palace", 4, 1},
            {"ITC Grand Chola", 8, 2},
            {"Taj Coromandel", 12, 3},
            {"Hyatt Regency", 6, 4},
            {"Park Hyatt", 14, 1},
            {"The Raintree", 10, 5}
        };
        memcpy(hotels, temp, sizeof(temp));
        count = 6;
        printf("\nHotels map in Chennai:\n\n");
    }
    else {
        printf("\nNo hotel map available for this city.\n");
        return;
    }

    // Print the map grid (7 rows × 20 columns)
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 20; col++) {
            int isHotel = 0;
            for (int i = 0; i < count; i++) {
                if (hotels[i].x == col && hotels[i].y == row) {
                    printf("%d",i+1);
                    isHotel = 1;
                    break;
                }
            }
            if (!isHotel) printf(".");
        }
        printf("\n");
    }

    printf("\nHotel List:\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i+1, hotels[i].name);
    }
}


int main(){
    printf("Welcome to the Hotel Booking System!\n");
    showIndiaMap();
    char loggedInUsername[50];
    char city[50] = "";
    int choice;
    char hotelName[100];

    loginOrSignup(loggedInUsername);
    printf("\nEnter your city name from the map above: ");
    fgets(city, sizeof(city), stdin);
    city[strcspn(city, "\n")] = 0;  

    while (1) {
        choice = showMainMenu();

        switch (choice) {
            case 1:
                printf("You chose Hotel Booking\n");
                printf("Enter the hotel name you want to select: ");
                showHotelMap(city);
                getchar(); // consume leftover newline from previous input
                fgets(hotelName, sizeof(hotelName), stdin);
                hotelName[strcspn(hotelName, "\n")] = 0; // remove newline
                hotelBookingMenu(city, hotelName, loggedInUsername);
                break;
             
            case 2:
                printf("Showing wallet balance...\n");
                showWalletBalance(loggedInUsername);
                break;
            case 3:
                addMoneyToWallet(loggedInUsername); 
                break;
            case 4:
                printf("Thank you for using the system! Goodbye.\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}