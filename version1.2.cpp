#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// Structure to store information about issued books
struct IssuedBookInfo {
    int member_id;
    int book_id;
    time_t due_date;
};

// Structure to represent a book
struct Book {
    int id;
    string name;
    int pageCount;
    string authorFirstName;
    string authorLastName;
    string bookType;
};

// Function to read books from a CSV file and return a vector of books
vector<Book> readBooksFromCSV(const string &filename) {
    vector<Book> books;
    ifstream file(filename);

// Check if the file is open
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return books;
    }

    string line;
    // Read each line from the file
    while (getline(file, line)) {
        stringstream ss(line);
        Book book;

        char comma;
        // Extract information from each line
        ss >> book.id >> comma;
        getline(ss, book.name, ',');
        ss >> book.pageCount >> comma;
        getline(ss, book.authorFirstName, ',');
        getline(ss, book.authorLastName, ',');
        getline(ss, book.bookType, ',');

    // Add the book to the vector
        books.push_back(book);
    }

// Close the file
    file.close();
    return books;
}

// Class to represent a member of the library
class Member {
public:
    int member_id;
    string name;
    vector<Book*> borrowed_books;

    Member(int id, const string& member_name) : member_id(id), name(member_name) {}
};

// Class to represent the library system
class LibrarySystem {
private:
    vector<Member*> members;
    vector<Book> books;
    vector<IssuedBookInfo> issued_books;

public:

// Function to load books from a CSV file
    void loadBooksFromCSV(const string& csv_file) {
        books = readBooksFromCSV(csv_file);
        cout << "Books loaded successfully." << endl;
    }

// Function to add a member to the library system
    void addMember(Member* member) {
        members.push_back(member);
        cout << "Member " << member->name << " added successfully. Member ID: " << member->member_id << endl;
    }

// Function to issue a book to a member
    void issueBook(int member_id, int book_id) {
        Member* member = findMember(member_id);
        Book* book = findBook(book_id);

// Check if the member and book exist
        if (member && book) {
            time_t due_date = time(nullptr) + 3 * 24 * 60 * 60; // 3 days in seconds
            issued_books.push_back({member_id, book_id, due_date});
            member->borrowed_books.push_back(book);

            cout << "Book " << book->name << " issued to " << member->name << ". Due date: "
                 << put_time(localtime(&due_date), "%Y-%m-%d %H:%M:%S") << endl;
        }
        else {
            cout << "Member or book not found. ";
            displayAvailableBookIDs();
            cout << "Member ID: " << member_id << ", Book ID: " << book_id << endl;
        }
    }

// Function to return a book from a member
    void returnBook(int member_id, int book_id) {
        Member* member = findMember(member_id);
        Book* book = findBook(book_id);

// Check if the member and book exist
        if (member && book) {
            auto it = find_if(issued_books.begin(), issued_books.end(), [member_id, book_id](const IssuedBookInfo& info) {
                return info.member_id == member_id && info.book_id == book_id;
            });
// Check if the book was issued to the member

            if (it != issued_books.end()) {
                issued_books.erase(it);
                member->borrowed_books.erase(remove(member->borrowed_books.begin(), member->borrowed_books.end(), book), member->borrowed_books.end());

                cout << "Book " << book->name << " returned by " << member->name << "." << endl;
            }
            else {
                cout << "This book was not issued to the member. ";
                displayAvailableBookIDs();
                cout << "Member ID: " << member_id << ", Book ID: " << book_id << endl;
            }
        }
        else {
            cout << "Member or book not found. ";
            displayAvailableBookIDs();
            cout << "Member ID: " << member_id << ", Book ID: " << book_id << endl;
        }
    }

// Function to display books borrowed by a member
    void displayBorrowedBooks(int member_id) {
        Member* member = findMember(member_id);

// Check if the member exists
        if (member) {
            cout << "Books borrowed by " << member->name << ": ";
            for (const auto& book : member->borrowed_books) {
                cout << book->name << ", ";
            }
            cout << endl;
        }
        else {
            cout << "Member not found. Member ID: " << member_id << endl;
        }
    }

// Function to calculate the fine for a member
    void calculateFine(int member_id) {
        Member* member = findMember(member_id);

// Check if the member exists
        if (member) {
            time_t now = time(nullptr);

            cout << "Fine for " << member->name << ": £" << calculateFineForMember(member, now) << endl;
        }
        else {
            cout << "Member not found. Member ID: " << member_id << endl;
        }
    }

// Function to display the main menu
    void displayMenu() {
        cout << "\n------ Library System Menu ------\n"
             << "1. Add Member\n"
             << "2. Issue Book\n"
             << "3. Return Book\n"
             << "4. Display Borrowed Books\n"
             << "5. Calculate Fine\n"
             << "0. Exit\n";
    }

// Function to get a valid integer input from the user
    int getValidIntInput(const string& prompt) {
        int input;
        cout << prompt;
        while (!(cin >> input)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid integer: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return input;
    }

// Function to display available book IDs
    void displayAvailableBookIDs() {
        cout << "Available Book IDs: ";
        for (const auto& book : books) {
            cout << book.id << ", ";
        }
        cout << endl;
    }

// Function to run the library system
    void run() {
        int choice;
        do {
            displayMenu();
            choice = getValidIntInput("Enter your choice: ");

            switch (choice) {
                case 1: {
                    int memberID = getValidIntInput("Enter Member ID: ");
                    string memberName;
                    cout << "Enter Member Name: ";
                    cin >> ws; // Consume any leading whitespaces
                    getline(cin, memberName);
                    addMember(new Member(memberID, memberName));
                    break;
                }
                case 2: {
                    int memberID = getValidIntInput("Enter Member ID: ");
                    displayAvailableBookIDs();
                    int bookID = getValidIntInput("Enter Book ID: ");
                    issueBook(memberID, bookID);
                    break;
                }
                case 3: {
                    int memberID = getValidIntInput("Enter Member ID: ");
                    displayAvailableBookIDs();
                    int bookID = getValidIntInput("Enter Book ID: ");
                    returnBook(memberID, bookID);
                    break;
                }
                case 4: {
                    int memberID = getValidIntInput("Enter Member ID: ");
                    displayBorrowedBooks(memberID);
                    break;
                }
                case 5: {
                    int memberID = getValidIntInput("Enter Member ID: ");
                    calculateFine(memberID);
                    break;
                }
                case 0:
                    cout << "Exiting the Library System. Goodbye!\n";
                    break;
                default:
                    cout << "Invalid choice. Please enter a valid option.\n";
            }
        } while (choice != 0);
    }

private:
// Function to find a member by ID
    Member* findMember(int member_id) {
        auto it = find_if(members.begin(), members.end(), [member_id](const Member* member) {
            return member->member_id == member_id;
        });

        return (it != members.end()) ? *it : nullptr;
    }

// Function to find a book by ID
    Book* findBook(int book_id) {
        auto it = find_if(books.begin(), books.end(), [book_id](const Book& book) {
            return book.id == book_id;
        });

        return (it != books.end()) ? &(*it) : nullptr;
    }

// Function to calculate the fine for a member
    int calculateFineForMember(const Member* member, time_t now) {
        int fine = 0;

        for (const auto& book : member->borrowed_books) {
            auto issuedBookInfoIt = find_if(issued_books.begin(), issued_books.end(), [member, book](const IssuedBookInfo& info) {
                return info.member_id == member->member_id && info.book_id == book->id;
            });

            if (issuedBookInfoIt != issued_books.end()) {
                time_t due_date = issuedBookInfoIt->due_date;
                int days_overdue = max(0, static_cast<int>((now - due_date) / (24 * 60 * 60)));
                fine += days_overdue;
            }
        }

        return fine;
    }
};

// Main function
int main() {
    LibrarySystem librarySystem;
    librarySystem.loadBooksFromCSV("library_books.csv");
    librarySystem.run();

    return 0;
}

