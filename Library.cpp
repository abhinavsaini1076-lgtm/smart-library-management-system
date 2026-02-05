#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// Forward declarations
class Book;
class User;
class LibrarySystem;

// Book class to represent individual books
class Book {
public:
    string isbn;
    string title;
    string author;
    string genre;
    bool isAvailable;
    int borrowCount;

    Book() : isAvailable(true), borrowCount(0) {}
    
    Book(string isbn, string title, string author, string genre) 
        : isbn(isbn), title(title), author(author), genre(genre), 
          isAvailable(true), borrowCount(0) {}

    void display() const {
        cout << "ISBN: " << isbn << "\n"
             << "Title: " << title << "\n"
             << "Author: " << author << "\n"
             << "Genre: " << genre << "\n"
             << "Available: " << (isAvailable ? "Yes" : "No") << "\n"
             << "Borrow Count: " << borrowCount << "\n";
    }
};

// Node for User Linked List
class UserNode {
public:
    User* user;
    UserNode* next;
    
    UserNode(User* u) : user(u), next(nullptr) {}
};

// User class
class User {
public:
    int userId;
    string name;
    string email;
    queue<string> borrowedBooks; // Queue for FIFO book management
    
    User() : userId(0) {}
    
    User(int id, string name, string email) 
        : userId(id), name(name), email(email) {}

    void display() const {
        cout << "User ID: " << userId << "\n"
             << "Name: " << name << "\n"
             << "Email: " << email << "\n"
             << "Books Borrowed: " << borrowedBooks.size() << "\n";
    }
};

// Hash Table for Book Inventory with Chaining for collision resolution
class BookHashTable {
private:
    static const int TABLE_SIZE = 100;
    vector<vector<Book*>> table;
    
    int hashFunction(const string& key) {
        int hash = 0;
        for (char c : key) {
            hash = (hash * 31 + c) % TABLE_SIZE;
        }
        return hash;
    }

public:
    BookHashTable() : table(TABLE_SIZE) {}
    
    void insert(Book* book) {
        int index = hashFunction(book->isbn);
        table[index].push_back(book);
    }
    
    Book* search(const string& isbn) {
        int index = hashFunction(isbn);
        for (Book* book : table[index]) {
            if (book->isbn == isbn) {
                return book;
            }
        }
        return nullptr;
    }
    
    void remove(const string& isbn) {
        int index = hashFunction(isbn);
        auto& chain = table[index];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if ((*it)->isbn == isbn) {
                delete *it;
                chain.erase(it);
                return;
            }
        }
    }
    
    vector<Book*> getAllBooks() {
        vector<Book*> allBooks;
        for (const auto& chain : table) {
            for (Book* book : chain) {
                allBooks.push_back(book);
            }
        }
        return allBooks;
    }
};

// Binary Search Tree for efficient searching
class BSTNode {
public:
    Book* book;
    BSTNode* left;
    BSTNode* right;
    
    BSTNode(Book* b) : book(b), left(nullptr), right(nullptr) {}
};

class BookBST {
private:
    BSTNode* root;
    
    BSTNode* insert(BSTNode* node, Book* book) {
        if (!node) return new BSTNode(book);
        
        if (book->title < node->book->title) {
            node->left = insert(node->left, book);
        } else {
            node->right = insert(node->right, book);
        }
        return node;
    }
    
    Book* search(BSTNode* node, const string& title) {
        if (!node) return nullptr;
        
        if (title == node->book->title) return node->book;
        
        if (title < node->book->title) {
            return search(node->left, title);
        } else {
            return search(node->right, title);
        }
    }
    
    void inorderTraversal(BSTNode* node, vector<Book*>& books) {
        if (node) {
            inorderTraversal(node->left, books);
            books.push_back(node->book);
            inorderTraversal(node->right, books);
        }
    }

public:
    BookBST() : root(nullptr) {}
    
    void insert(Book* book) {
        root = insert(root, book);
    }
    
    Book* searchByTitle(const string& title) {
        return search(root, title);
    }
    
    vector<Book*> getSortedBooks() {
        vector<Book*> books;
        inorderTraversal(root, books);
        return books;
    }
};

// User Management using Linked List
class UserLinkedList {
private:
    UserNode* head;
    int nextUserId;

public:
    UserLinkedList() : head(nullptr), nextUserId(1) {}
    
    User* addUser(const string& name, const string& email) {
        User* newUser = new User(nextUserId++, name, email);
        UserNode* newNode = new UserNode(newUser);
        
        if (!head) {
            head = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        return newUser;
    }
    
    User* findUser(int userId) {
        UserNode* current = head;
        while (current) {
            if (current->user->userId == userId) {
                return current->user;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    vector<User*> getAllUsers() {
        vector<User*> users;
        UserNode* current = head;
        while (current) {
            users.push_back(current->user);
            current = current->next;
        }
        return users;
    }
};

// Main Library System
class LibrarySystem {
private:
    BookHashTable bookInventory;
    BookBST bookSearchTree;
    UserLinkedList userManager;
    queue<pair<int, string>> issueQueue; // Queue for book issue requests
    queue<pair<int, string>> returnQueue; // Queue for book return requests
    map<string, int> borrowFrequency; // For most borrowed books report

public:
    void addBook() {
        string isbn, title, author, genre;
        cout << "\n--- Add New Book ---\n";
        cout << "Enter ISBN: ";
        cin.ignore();
        getline(cin, isbn);
        cout << "Enter Title: ";
        getline(cin, title);
        cout << "Enter Author: ";
        getline(cin, author);
        cout << "Enter Genre: ";
        getline(cin, genre);
        
        // Check if book already exists
        if (bookInventory.search(isbn)) {
            cout << "Book with ISBN " << isbn << " already exists!\n";
            return;
        }
        
        Book* newBook = new Book(isbn, title, author, genre);
        bookInventory.insert(newBook);
        bookSearchTree.insert(newBook);
        
        cout << "Book added successfully!\n";
    }
    
    void addUser() {
        string name, email;
        cout << "\n--- Register New User ---\n";
        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Email: ";
        getline(cin, email);
        
        User* newUser = userManager.addUser(name, email);
        cout << "User registered successfully! User ID: " << newUser->userId << "\n";
    }
    
    void searchBooks() {
        int choice;
        cout << "\n--- Search Books ---\n";
        cout << "1. Search by ISBN\n";
        cout << "2. Search by Title\n";
        cout << "3. Search by Author\n";
        cout << "4. Display all books\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1: {
                string isbn;
                cout << "Enter ISBN: ";
                cin.ignore();
                getline(cin, isbn);
                Book* book = bookInventory.search(isbn);
                if (book) {
                    cout << "\nBook Found:\n";
                    book->display();
                } else {
                    cout << "Book not found!\n";
                }
                break;
            }
            case 2: {
                string title;
                cout << "Enter Title: ";
                cin.ignore();
                getline(cin, title);
                Book* book = bookSearchTree.searchByTitle(title);
                if (book) {
                    cout << "\nBook Found:\n";
                    book->display();
                } else {
                    cout << "Book not found!\n";
                }
                break;
            }
            case 3: {
                string author;
                cout << "Enter Author: ";
                cin.ignore();
                getline(cin, author);
                vector<Book*> allBooks = bookInventory.getAllBooks();
                bool found = false;
                cout << "\nBooks by " << author << ":\n";
                cout << string(50, '-') << "\n";
                for (Book* book : allBooks) {
                    if (book->author == author) {
                        book->display();
                        cout << string(30, '-') << "\n";
                        found = true;
                    }
                }
                if (!found) {
                    cout << "No books found by this author!\n";
                }
                break;
            }
            case 4: {
                vector<Book*> allBooks = bookInventory.getAllBooks();
                if (allBooks.empty()) {
                    cout << "No books in the library!\n";
                } else {
                    cout << "\nAll Books in Library:\n";
                    cout << string(50, '=') << "\n";
                    for (Book* book : allBooks) {
                        book->display();
                        cout << string(30, '-') << "\n";
                    }
                }
                break;
            }
            default:
                cout << "Invalid choice!\n";
        }
    }
    
    void issueBook() {
        int userId;
        string isbn;
        cout << "\n--- Issue Book ---\n";
        cout << "Enter User ID: ";
        cin >> userId;
        cout << "Enter Book ISBN: ";
        cin.ignore();
        getline(cin, isbn);
        
        // Add to issue queue
        issueQueue.push({userId, isbn});
        cout << "Issue request added to queue.\n";
        
        // Process the queue
        processIssueQueue();
    }
    
    void returnBook() {
        int userId;
        string isbn;
        cout << "\n--- Return Book ---\n";
        cout << "Enter User ID: ";
        cin >> userId;
        cout << "Enter Book ISBN: ";
        cin.ignore();
        getline(cin, isbn);
        
        // Add to return queue
        returnQueue.push({userId, isbn});
        cout << "Return request added to queue.\n";
        
        // Process the queue
        processReturnQueue();
    }
    
    void processIssueQueue() {
        while (!issueQueue.empty()) {
            auto request = issueQueue.front();
            issueQueue.pop();
            
            int userId = request.first;
            string isbn = request.second;
            
            User* user = userManager.findUser(userId);
            Book* book = bookInventory.search(isbn);
            
            if (!user) {
                cout << "User with ID " << userId << " not found!\n";
                continue;
            }
            
            if (!book) {
                cout << "Book with ISBN " << isbn << " not found!\n";
                continue;
            }
            
            if (!book->isAvailable) {
                cout << "Book '" << book->title << "' is not available!\n";
                continue;
            }
            
            // Issue the book
            book->isAvailable = false;
            book->borrowCount++;
            user->borrowedBooks.push(isbn);
            borrowFrequency[isbn]++;
            
            cout << "Book '" << book->title << "' issued to " << user->name << " successfully!\n";
        }
    }
    
    void processReturnQueue() {
        while (!returnQueue.empty()) {
            auto request = returnQueue.front();
            returnQueue.pop();
            
            int userId = request.first;
            string isbn = request.second;
            
            User* user = userManager.findUser(userId);
            Book* book = bookInventory.search(isbn);
            
            if (!user) {
                cout << "User with ID " << userId << " not found!\n";
                continue;
            }
            
            if (!book) {
                cout << "Book with ISBN " << isbn << " not found!\n";
                continue;
            }
            
            // Check if user has this book
            queue<string> tempQueue;
            bool bookFound = false;
            
            while (!user->borrowedBooks.empty()) {
                string borrowedIsbn = user->borrowedBooks.front();
                user->borrowedBooks.pop();
                
                if (borrowedIsbn == isbn) {
                    bookFound = true;
                } else {
                    tempQueue.push(borrowedIsbn);
                }
            }
            
            // Restore the queue without the returned book
            user->borrowedBooks = tempQueue;
            
            if (!bookFound) {
                cout << "User " << user->name << " has not borrowed this book!\n";
                continue;
            }
            
            // Return the book
            book->isAvailable = true;
            cout << "Book '" << book->title << "' returned by " << user->name << " successfully!\n";
        }
    }
    
    void generateReports() {
        int choice;
        cout << "\n--- Reports ---\n";
        cout << "1. Currently Borrowed Books\n";
        cout << "2. Most Borrowed Books\n";
        cout << "3. Active Users\n";
        cout << "4. All Users Summary\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1: {
                cout << "\n--- Currently Borrowed Books ---\n";
                vector<Book*> allBooks = bookInventory.getAllBooks();
                bool found = false;
                for (Book* book : allBooks) {
                    if (!book->isAvailable) {
                        book->display();
                        cout << string(30, '-') << "\n";
                        found = true;
                    }
                }
                if (!found) {
                    cout << "No books are currently borrowed.\n";
                }
                break;
            }
            case 2: {
                cout << "\n--- Most Borrowed Books ---\n";
                if (borrowFrequency.empty()) {
                    cout << "No books have been borrowed yet.\n";
                } else {
                    // Convert to vector for sorting
                    vector<pair<string, int>> sortedBooks(borrowFrequency.begin(), borrowFrequency.end());
                    sort(sortedBooks.begin(), sortedBooks.end(), 
                         [](const pair<string, int>& a, const pair<string, int>& b) {
                             return a.second > b.second;
                         });
                    
                    cout << left << setw(20) << "ISBN" << setw(10) << "Count" << "Title\n";
                    cout << string(50, '-') << "\n";
                    for (const auto& entry : sortedBooks) {
                        Book* book = bookInventory.search(entry.first);
                        if (book) {
                            cout << left << setw(20) << entry.first 
                                 << setw(10) << entry.second 
                                 << book->title << "\n";
                        }
                    }
                }
                break;
            }
            case 3: {
                cout << "\n--- Active Users (Users with borrowed books) ---\n";
                vector<User*> allUsers = userManager.getAllUsers();
                bool found = false;
                for (User* user : allUsers) {
                    if (!user->borrowedBooks.empty()) {
                        user->display();
                        cout << "Borrowed Books: ";
                        queue<string> tempQueue = user->borrowedBooks;
                        while (!tempQueue.empty()) {
                            Book* book = bookInventory.search(tempQueue.front());
                            if (book) {
                                cout << book->title << "; ";
                            }
                            tempQueue.pop();
                        }
                        cout << "\n" << string(30, '-') << "\n";
                        found = true;
                    }
                }
                if (!found) {
                    cout << "No active users found.\n";
                }
                break;
            }
            case 4: {
                cout << "\n--- All Users Summary ---\n";
                vector<User*> allUsers = userManager.getAllUsers();
                if (allUsers.empty()) {
                    cout << "No users registered.\n";
                } else {
                    for (User* user : allUsers) {
                        user->display();
                        cout << string(30, '-') << "\n";
                    }
                }
                break;
            }
            default:
                cout << "Invalid choice!\n";
        }
    }
    
    void displayMenu() {
        cout << "\n" << string(50, '=') << "\n";
        cout << "      SMART LIBRARY MANAGEMENT SYSTEM\n";
        cout << string(50, '=') << "\n";
        cout << "1. Add Book\n";
        cout << "2. Register User\n";
        cout << "3. Search Books\n";
        cout << "4. Issue Book\n";
        cout << "5. Return Book\n";
        cout << "6. Generate Reports\n";
        cout << "7. Initialize Sample Data (Optional)\n";
        cout << "8. Exit\n";
        cout << string(50, '-') << "\n";
        cout << "Enter your choice: ";
    }
    
    void run() {
        cout << "\nWelcome to Smart Library Management System!\n";
        cout << "Please start by adding books and registering users.\n";
        
        int choice;
        do {
            displayMenu();
            cin >> choice;
            
            switch(choice) {
                case 1:
                    addBook();
                    break;
                case 2:
                    addUser();
                    break;
                case 3:
                    searchBooks();
                    break;
                case 4:
                    issueBook();
                    break;
                case 5:
                    returnBook();
                    break;
                case 6:
                    generateReports();
                    break;
                case 7:
                    initializeSampleData();
                    break;
                case 8:
                    cout << "Thank you for using Smart Library Management System!\n";
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } while(choice != 8);
    }
    
    
    void initializeSampleData() {
        cout << "\n--- Initialize Sample Data ---\n";
        cout << "This will add sample books and users to the system.\n";
        cout << "Do you want to continue? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Sample data initialization cancelled.\n";
            return;
        }
        
        // Add sample books
        Book* book1 = new Book("978-0134685991", "Effective Modern C++", "Scott Meyers", "Programming");
        Book* book2 = new Book("978-0321563842", "The C++ Programming Language", "Bjarne Stroustrup", "Programming");
        Book* book3 = new Book("978-0596809485", "97 Things Every Programmer Should Know", "Kevlin Henney", "Programming");
        Book* book4 = new Book("978-0132350884", "Clean Code", "Robert C. Martin", "Programming");
        Book* book5 = new Book("978-0201633610", "Design Patterns", "Gang of Four", "Software Engineering");
        
        bookInventory.insert(book1);
        bookInventory.insert(book2);
        bookInventory.insert(book3);
        bookInventory.insert(book4);
        bookInventory.insert(book5);
        
        bookSearchTree.insert(book1);
        bookSearchTree.insert(book2);
        bookSearchTree.insert(book3);
        bookSearchTree.insert(book4);
        bookSearchTree.insert(book5);
        
        // Add sample users
        userManager.addUser("Alice Johnson", "alice@email.com");
        userManager.addUser("Bob Smith", "bob@email.com");
        userManager.addUser("Charlie Brown", "charlie@email.com");
        
        cout << "\nSample data initialized successfully!\n";
        cout << "Added 5 programming books and 3 sample users to the system.\n";
        cout << "\nSample Books Added:\n";
        cout << "- Effective Modern C++ by Scott Meyers\n";
        cout << "- The C++ Programming Language by Bjarne Stroustrup\n";
        cout << "- 97 Things Every Programmer Should Know by Kevlin Henney\n";
        cout << "- Clean Code by Robert C. Martin\n";
        cout << "- Design Patterns by Gang of Four\n";
        cout << "\nSample Users Added:\n";
        cout << "- Alice Johnson (ID: 1)\n";
        cout << "- Bob Smith (ID: 2)\n";
        cout << "- Charlie Brown (ID: 3)\n";
    }
};

int main() {
    LibrarySystem library;
    library.run();
    return 0;
}