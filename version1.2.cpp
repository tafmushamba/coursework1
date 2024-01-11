#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

struct IssuedBookInfo {
    int member_id;
    int book_id;
    time_t due_date;
};

struct Book {
    int id;
    string name;
    int pageCount;
    string authorFirstName;
    string authorLastName;
    string bookType;
};

vector<Book> readBooksFromCSV(const string &filename) {
    vector<Book> books;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return books;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Book book;

        char comma;
        ss >> book.id >> comma;
        getline(ss, book.name, ',');
        ss >> book.pageCount >> comma;
        getline(ss, book.authorFirstName, ',');
        getline(ss, book.authorLastName, ',');
        getline(ss, book.bookType, ',');

        books.push_back(book);
    }

    file.close();
    return books;
}

class Member {
public:
    int member_id;
    string name;
    vector<Book*> borrowed_books;

    Member(int id, const string& member_name) : member_id(id), name(member_name) {}
};

class LibrarySystem {
private:
    vector<Member*> members;
    vector<Book> books;
    vector<IssuedBookInfo> issued_books;

public:
    void loadBooksFromCSV(const string& csv_file) {
        books = readBooksFromCSV(csv_file);
        cout << "Books loaded successfully." << endl;
    }

    void addMember(Member* member) {
        members.push_back(member);
        cout << "Member " << member->name << " added successfully. Member ID: " << member->member_id << endl;
    }

    void issueBook(int member_id, int book_id) {
        Member* member = findMember(member_id);
        Book* book = findBook(book_id);

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

    void returnBook(int member_id, int book_id) {
        Member* member = findMember(member_id);
        Book* book = findBook(book_id);

        if (member && book) {
            auto it = find_if(issued_books.begin(), issued_books.end(), [member_id, book_id](const IssuedBookInfo& info) {
                return info.member_id == member_id && info.book_id == book_id;
            });

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

    void displayBorrowedBooks(int member_id) {
        Member* member = findMember(member_id);

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

    void calculateFine(int member_id) {
        Member* member = findMember(member_id);

        if (member) {
            time_t now = time(nullptr);

            cout << "Fine for " << member->name << ": £" << calculateFineForMember(member, now) << endl;
        }
        else {
            cout << "Member not found. Member ID: " << member_id << endl;
        }
    }

    void displayMenu() {
        cout << "\n------ Library System Menu ------\n"
             << "1. Add Member\n"
             << "2. Issue Book\n"
             << "3. Return Book\n"
             << "4. Display Borrowed Books\n"
             << "5. Calculate Fine\n"
             << "0. Exit\n";
    }

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

    void displayAvailableBookIDs() {
        cout << "Available Book IDs: ";
        for (const auto& book : books) {
            cout << book.id << ", ";
        }
        cout << endl;
    }

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
    Member* findMember(int member_id) {
        auto it = find_if(members.begin(), members.end(), [member_id](const Member* member) {
            return member->member_id == member_id;
        });

        return (it != members.end()) ? *it : nullptr;
    }

    Book* findBook(int book_id) {
        auto it = find_if(books.begin(), books.end(), [book_id](const Book& book) {
            return book.id == book_id;
        });

        return (it != books.end()) ? &(*it) : nullptr;
    }

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

int main() {
    LibrarySystem librarySystem;
    librarySystem.loadBooksFromCSV("library_books.csv");
    librarySystem.run();

    return 0;
}

