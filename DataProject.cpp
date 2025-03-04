#pragma once

#include "Btree.h"

#include <iostream>
#include <sstream>
#include <openssl/sha.h>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <sys/stat.h>
#include <vector>
#include <ctime>
#include <filesystem> // Use C++17 filesystem instead of fs::

namespace fs = std::filesystem; // Standardize filesystem namespace

class RoutingTable;
struct DHTNode;
struct DHTNode1;
class DHT;
class CircularLinkedList;
class DoublyCircularLinkedList;

using namespace std;

// Use platform-independent path separator
#ifdef _WIN32
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif

string basepath = fs::current_path().string();
struct DHTNode
{
    BigInt id;
    DHTNode *next;
    RoutingTable *rTable;
    BTree *storage;
    string FolderPath;
    DHTNode()
    {
        next = NULL;
        rTable = NULL;
        storage = NULL;
    }
    ~DHTNode()
    {
        if (rTable != nullptr)
        {
            delete rTable;
        }
        rTable = NULL;
        if (storage != nullptr)
        {
            delete storage;
        }
        storage = nullptr;
        if (fs::exists(FolderPath) && fs::is_directory(FolderPath))
        {
            fs::remove_all(FolderPath);
        }
    }
};
struct DHTNode1
{
public:
    DHTNode *id;
    DHTNode1 *next;
    DHTNode1 *prev;
    DHTNode1()
    {
        next = nullptr;
        prev = nullptr;
        id = nullptr;
    }
    DHTNode1(DHTNode *I)
    {
        next = prev = nullptr;
        id = I;
    }
};
class CircularLinkedList
{
public:
    DHTNode *head;
    DHTNode *last;

public:
    CircularLinkedList()
    {
        head = NULL;
        last = NULL;
    }
    CircularLinkedList(BigInt data)
    {
        head = new DHTNode;
        head->id = data;
        head->next = NULL;
        last = head;
    }
    void insert(int data)
    {
        BigInt temp(data);
        insert(temp);
    }
    void insert(BigInt data)
    {
        if (!head)
        {
            head = new DHTNode;
            head->id = data;
            head->next = head;
            last = head;
            head->FolderPath = basepath + PATH_SEPARATOR + "Machine_" + string(head->id);
            fs::create_directories(head->FolderPath); // Use create_directories instead of create_directory
            fs::permissions(head->FolderPath,
                            fs::perms::owner_all |
                                fs::perms::group_all |
                                fs::perms::others_read |
                                fs::perms::others_exec,
                            fs::perm_options::add);
        }
        else
        {
            DHTNode *current = head;
            while (current->next != head)
            {
                current = current->next;
            }
            DHTNode *temp = new DHTNode;
            temp->id = data;
            temp->next = head;
            current->next = temp;
            last = temp;
            temp->FolderPath = basepath + PATH_SEPARATOR + "Machine_" + string(temp->id);
            fs::create_directories(temp->FolderPath);  // Use create_directories instead of create_directory
            fs::permissions(temp->FolderPath, 
                fs::perms::owner_all | 
                fs::perms::group_all | 
                fs::perms::others_read | 
                fs::perms::others_exec, 
                fs::perm_options::add);
                    }
                }

    bool isEmpty()
    {
        return (!head);
    }
    void update(BigInt loc, BigInt data)
    {
        DHTNode *current = head;
        while (current->next != head)
        {
            if (current->id == loc)
            {
                current->id = data;
                return;
            }
            current = current->next;
        }
    }
    bool search(BigInt loc)
    {
        if (!head)
            return false;
        DHTNode *current = head;
        while (current->next != head)
        {
            if (current->id == loc)
                return true;
            current = current->next;
        }
        if (current->id == loc)
            return true;
        return false;
    }
    DHTNode *searchNode(BigInt loc)
    {
        if (!head)
            return nullptr;
        DHTNode *current = head;
        while (current->next != head)
        {
            if (current->id == loc)
                return current;
            current = current->next;
        }
        if (current->id == loc)
            return current;
        return nullptr;
    }
    void insertAtIndex(BigInt data, int loc)
    {
        int pos = 0;
        DHTNode *current = head;
        DHTNode *prev = nullptr;
        while (current->next != head && pos != loc)
        {
            prev = current;
            current = current->next;
            pos++;
        }
        if (pos == loc)
        {
            DHTNode *temp = new DHTNode;
            temp->next = current;
            temp->id = data;
            prev->next = temp;
        }
    }
    string print()
    {
        if (head == nullptr)
        {
            return "Ring is Empty";
        }
        string d;
        DHTNode *current = head;
        if (current)
        {
            d = d + (string(current->id));
            current = current->next;
        }
        while (current != head)
        {
            d = d + " -> " + string((current->id));
            current = current->next;
        }
        d = d + " -> " + string((current->id));
        return d;
    }
    void deleteData(BigInt index)
    {
        if (!head)
        {
            return;
        }

        DHTNode *current = head;
        DHTNode *prev = nullptr;

        do
        {
            if (current->id == index)
            {
                if (current == head)
                {
                    if (head == last)
                    {
                        head = last = nullptr;
                    }
                    else
                    {
                        head = head->next;
                        last->next = head;
                    }
                }
                else if (current == last)
                {
                    prev->next = current->next;
                    last = prev;
                }
                else
                {
                    prev->next = current->next;
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        } while (current != head);
    }
    ~CircularLinkedList()
    {
        if (head == nullptr)
        {
            return;
        }

        DHTNode *current = head;
        do
        {
            DHTNode *temp = current;
            current = current->next;
            delete temp;
        } while (current != head);

        head = nullptr;
    }

    void sortList()
    {
        if (head == nullptr)
            return;

        DHTNode *current, *index;
        current = head;
        BigInt temp;
        string temppath;
        DHTNode *tempnext = nullptr;
        RoutingTable *temprt = nullptr;
        BTree *temptree = nullptr;
        do
        {
            index = current->next;
            while (index != head)
            {
                if (current->id > index->id)
                {
                    // temp = current->id;
                    // current->id = index->id;
                    // index->id = temp;
                    swap(current->id, index->id);
                    swap(current->FolderPath, index->FolderPath);
                    /*tempnext = current->next;
                    current->next = index->next;
                    index->next = tempnext;*/
                    temprt = current->rTable;
                    current->rTable = index->rTable;
                    index->rTable = temprt;
                    temptree = current->storage;
                    current->storage = index->storage;
                    index->storage = temptree;
                }
                index = index->next;
            }
            current = current->next;
        } while (current->next != head);
    }

    DHTNode *findPreviousNode(DHTNode *node)
    {
        if (head == nullptr || node == nullptr)
        {
            return nullptr;
        }

        DHTNode *current = head;
        do
        {
            if (current->next == node)
            {
                return current;
            }
            current = current->next;
        } while (current != head);

        return nullptr;
    }

    DHTNode *getHead()
    {
        return head;
    }
    DHTNode *findSuccessor(BigInt value, BigInt total)
    {
        if (!head)
        {
            return nullptr;
        }
        if (value <= head->id)
        {
            return head;
        }
        DHTNode *current = head;
        do
        {
            if (current->id < value && current->next->id >= value && current->next != head)
            {
                return current->next;
            }
            else if (current->id < value && current->next->id == head->id && value <= (total + head->id))
            {
                return head;
            }
            current = current->next;
        } while (current != head);

        return nullptr;
    }
};

class DHT
{
public:
    CircularLinkedList *Ring;
    int identifier_space;
    vector<string> AddedFiles;
    int order;
    BigInt *totalnodes;
    BigInt Machines;
    DHT();
    DHTNode *FindMachine(BigInt target, DHTNode *Mac, string &path);
    void ControlMenu();
};
class DoublyCircularLinkedList
{
public:
    DHTNode1 *head;
    DHTNode1 *last;
    BigInt *size;
    DoublyCircularLinkedList()
    {
        head = nullptr;
        last = nullptr;
        size = new BigInt(0);
    }
    string print()
    {
        string d;
        if (head == NULL)
        {
            return "Routing Table is Empty";
        }
        DHTNode1 *current = head;
        if (current != NULL)
        {
            d = d + (string(current->id->id));
            current = current->next;
        }
        while (current != head)
        {
            d = d + " -> " + string((current->id->id));
            current = current->next;
        }
        d = d + " -> " + string((current->id->id));
        return d;
    }
    DoublyCircularLinkedList(DHTNode *data)
    {
        head = new DHTNode1;
        head->id = data;
        head->next = head;
        head->prev = head;
        last = head;
        size = new BigInt(1);
    }

    void insert(DHTNode *data)
    {
        if (!head)
        {
            head = new DHTNode1;
            head->id = data;
            head->next = head;
            head->prev = head;
            last = head;
        }
        else
        {
            DHTNode1 *newNode = new DHTNode1;
            newNode->id = data;
            newNode->next = head;
            newNode->prev = last;

            last->next = newNode;
            head->prev = newNode;
            last = newNode;
        }
        size++;
    }
    DHTNode1 *findPredecessor(BigInt value, DHT *other)
    {
        if (!head)
        {
            return nullptr;
        }
        DHTNode1 *pre = nullptr;
        DHTNode1 *current = this->head;
        do
        {
            if (current->id->id < value && (!pre || current->id->id > pre->id->id))
            {
                pre = current;
            }
            current = current->next;
        } while (current != head);
        if (pre == nullptr)
        {
            DHTNode1 *ret = new DHTNode1(other->Ring->findSuccessor(value, *other->totalnodes));
            return ret;
        }
        return pre;
    }
    /*DHTNode1 findSuccessor(BigInt key) {
        DHTNode1* current = head;
        int closestPrecedingNode = -1;

        do {
            if (current->successor < key && current->successor > closestPrecedingNode) {
                closestPrecedingNode = current->successor;
            }
            current = current->next;
        } while (current != fingerTable);

        if (closestPrecedingNode == -1) {
            // If no closest preceding node was found, return the successor of the last entry in the finger table
            return fingerTable->successor;
        }
        else {
            // Return the successor of the closest preceding node
            return closestPrecedingNode;
        }
    }*/
};

// Implement other methods (update, search, insertAtIndex, print, deleteData, sortList, getHead) similar to the CircularLinkedList class but adjusted for doubly linked list behavior.

string sha1(const string &input)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<
             const unsigned char *>(input.c_str()),
         input.length(), hash);

    stringstream a;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
    {
        a << hex << uppercase << static_cast<int>(hash[i]);
    }
    return a.str();
}

void printLine(int width, char borderChar, char fillChar)
{
    std::cout << std::setw(width) << std::setfill(borderChar) << borderChar << std::endl;
}

void printText(const std::vector<std::string> &textLines, int width)
{
    for (const auto &line : textLines)
    {
        int padding = (width - line.length() - 4) / 2;
        std::cout << "  " << std::setfill(' ') << std::setw(padding) << "" << line << std::setw(padding) << "  " << std::endl;
    }
}
void printConsoleBox(const std::vector<std::string> &messageLines)
{
    int width = 100;       // Width of the console box
    char borderChar = '*'; // Border character of the box
    char fillChar = ' ';   // Fill character inside the box
    // system("cls");
    printLine(width, borderChar, fillChar);
    printText(messageLines, width);
    printLine(width, borderChar, fillChar);
}
void printMenu()
{
    vector<string> Menu = {
        "InterPlanetary File System SETUP",
        "1.Set Size of Identifier Space",
        "2.Set No of Machines",
        "3.Set Order of B-Tree",
        "4.Ring Info",
        "5.LAUNCH"};
    printConsoleBox(Menu);
}
void printMenu1()
{
    vector<string> Menu = {
        "CONTROL MENU",
        "1.Insert File",
        "2.Remove File",
        "3.Print Routing Table",
        "4.Add Machine",
        "5.Remove Machine",
        "6.Display DHT Ring",
        "7.Search File",
        "8.Ring Info",
        "9.Print B-Tree",
        "10.EXIT"};
    printConsoleBox(Menu);
}
class RoutingTable
{
public:
    DoublyCircularLinkedList List;
    void setTable(DHT *other);
};
string Navigator(string path)
{
    vector<string> txtFiles;
    vector<string> txtContent;
    vector<string> otherFiles;
    vector<string> prevpath;
    bool exitMenu = false;
    while (exitMenu == false)
    {
        for (const auto &entry : fs::directory_iterator(path))
        {
            fs::path outfilename = entry.path();

            if (outfilename.extension() == ".txt")
            {
                txtFiles.push_back(outfilename.string());
                ifstream file(outfilename.string());
                if (file.is_open())
                {
                    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    txtContent.push_back(content);
                    file.close();
                }
            }
            else if (fs::is_directory(outfilename))
            {
                otherFiles.push_back(outfilename.string());
            }
        }
        vector<string> Display;
        Display.push_back("Other Folders");
        for (int i = 0; i < otherFiles.size(); i++)
        {
            string add = to_string(i + 1) + "-" + otherFiles[i];
            Display.push_back(add);
        }
        Display.push_back("Text Files");
        for (int i = 0; i < txtFiles.size(); i++)
        {
            string add = to_string(i + 1) + "-" + txtFiles[i];
            Display.push_back(add);
        }
        Display.push_back("1.Select Folder");
        Display.push_back("2.Select TxT File");
        Display.push_back("3.Go Back");
        Display.push_back("4.Exit");
        bool ExitMenu1 = false;
        while (ExitMenu1 == false)
        {
            printConsoleBox(Display);
            int choice;
            cin >> choice;
            while (choice < 1 || choice > 4)
            {
                printConsoleBox(Display);
                cin >> choice;
            }
            switch (choice)
            {
            case 1:
            {
                if (otherFiles.size() == 0)
                {
                    vector<string> msg = {
                        "No Further Folders"};
                    printConsoleBox(msg);
                    break;
                }
                vector<string> msg = {
                    "Choose Folder"};
                printConsoleBox(msg);
                cin >> choice;
                while (choice < 1 || choice > (otherFiles.size()))
                {
                    vector<string> msg = {
                        "Invalid Choice"};
                    printConsoleBox(msg);
                    cin >> choice;
                }
                prevpath.push_back(path);
                path = otherFiles[choice - 1];
                otherFiles.clear();
                txtFiles.clear();
                txtContent.clear();
                ExitMenu1 = true;
                break;
            }
            case 2:
            {
                if (txtFiles.size() == 0)
                {
                    vector<string> msg = {
                        "No Txt File"};
                    printConsoleBox(msg);
                    break;
                }
                vector<string> msg = {
                    "Choose Txt File"};
                printConsoleBox(msg);
                cin >> choice;
                while (choice < 1 || choice > (txtFiles.size()))
                {
                    vector<string> msg = {
                        "Invalid Choice"};
                    printConsoleBox(msg);
                    cin >> choice;
                }
                return txtFiles[choice - 1];
            }
            case 3:
            {
                if (prevpath.size() == 0)
                {
                    vector<string> msg = {
                        "No Previous Directory"};
                    printConsoleBox(msg);
                    break;
                }
                path = prevpath.back();
                prevpath.pop_back();
                otherFiles.clear();
                txtFiles.clear();
                txtContent.clear();
                ExitMenu1 = true;
                break;
            }
            case 4:
            {
                return "\0";
            }
            }
        }
    }
}
DHT::DHT()
{
    totalnodes = NULL;
    Ring = NULL;
    order = 2;
    identifier_space = 0;
    int choice;
    BigInt one("1");
    BigInt zero("0");
    bool exitMenu = false;
    while (exitMenu == false)
    {
        printMenu();
        cin >> choice;
        if (choice > 0 && choice < 6)
            switch (choice)
            {
            case 1:
            {
                vector<string> msg = {
                    "Enter Size of Identifier Space:"};
                printConsoleBox(msg);
                int temp;
                cin >> temp;
                while (temp < 1 && temp > 160)
                {
                    vector<string> msg1 = {
                        "INVALID SIZE RE-ENTER:"};
                    printConsoleBox(msg1);
                    cin >> temp;
                }
                identifier_space = temp;
                if (identifier_space > 1)
                    order = identifier_space;
                if (totalnodes != nullptr)
                {
                    delete totalnodes;
                }
                totalnodes = new BigInt(2);
                BigInt n1(2);
                Machines = BigInt(temp);
                *totalnodes = totalnodes->Power(string(Machines));
                string out;
                out = out + "Identifier Space Size: " + to_string(identifier_space) + " No of Machines: " + string(Machines) + " Max Nodes: " + string(*totalnodes) + " B-Tree Order: " + to_string(order);
                vector<string> out1 = {
                    out};
                printConsoleBox(out1);
                break;
            }
            case 2:
            {
                if (identifier_space == 0)
                {
                    vector<string> msg = {
                        "Set Size of Identfier Space FIRST"};
                    printConsoleBox(msg);
                    break;
                }
                vector<string> msg = {
                    "Enter NO of Machines: "};
                printConsoleBox(msg);
                string temp;
                cin >> temp;
                BigInt dummy(temp);
                while (dummy < zero || dummy > *totalnodes)
                {
                    vector<string> msg1 = {
                        "INVALID NO of Machines RE-ENTER:"};
                    printConsoleBox(msg1);
                    cin >> temp;
                    dummy = BigInt(temp);
                }
                string out;
                out = out + "No of Machines: " + string(dummy);
                vector<string> out1 = {
                    out};
                printConsoleBox(out1);
                Machines = dummy;
                break;
            }
            case 3:
            {
                if (identifier_space > 0)
                {
                    vector<string> msg = {
                        "Enter B-Tree Order:"};
                    printConsoleBox(msg);
                    int temp = 0;
                    cin >> temp;
                    while (temp < 2 || temp > identifier_space)
                    {
                        vector<string> msg1 = {
                            "Invalid Order | Re-Enter:"};
                        printConsoleBox(msg1);
                        cin >> temp;
                    }
                    order = temp;
                }
                else
                {
                    vector<string> msg = {
                        "Set Size of Identfier Space FIRST"};
                    printConsoleBox(msg);
                }
                break;
            }
            case 4:
            {
                if (identifier_space > 0)
                {
                    string out;
                    out = out + "Identifier Space Size: " + string(Machines) + " No of Machines: " + string(Machines) + " Max Nodes: " + string(*totalnodes) + " B-Tree Order: " + to_string(order);
                    vector<string> out1 = {
                        out};
                    printConsoleBox(out1);
                }
                else
                {
                    vector<string> msg = {
                        "No Ring Info Available"};
                    printConsoleBox(msg);
                }
                break;
            }
            case 5:
            {
                if (identifier_space == 0)
                {
                    vector<string> msg = {
                        "Set Size of Identfier Space FIRST"};
                    printConsoleBox(msg);
                    break;
                }
                int choice1;
                vector<string> msg = {
                    "1.Auto ID Assigning",
                    "2.Manual ID Assigning"};
                printConsoleBox(msg);
                cin >> choice1;
                while (choice1 < 1 && choice1 > 2)
                {
                    printConsoleBox(msg);
                    cin >> choice1;
                }
                switch (choice1)
                {
                case 1:
                {
                    Ring = new CircularLinkedList;
                    BigInt temp1(one.generateRandomBigInt(identifier_space));
                    for (BigInt i(0); i < Machines;)
                    {
                        temp1 = one.generateRandomBigInt(identifier_space);
                        if (Ring->search(temp1) == false && temp1 >= zero)
                        {
                            Ring->insert(temp1);
                            i++;
                        }
                    }
                    vector<string> msg1 = {
                        "ID's have been Auto-Assigned"};
                    Ring->sortList();
                    exitMenu = true;
                    break;
                }
                case 2:
                {
                    Ring = new CircularLinkedList;
                    BigInt zero("0");
                    for (BigInt i(0); i < Machines;)
                    {
                        vector<string> msg = {
                            "ENTER VALID ID"};
                        printConsoleBox(msg);
                        string id;
                        cin >> id;
                        BigInt temp(id);
                        if (Ring->search(temp) == false && temp < *totalnodes)
                        {
                            Ring->insert(temp);
                            i++;
                        }
                        else
                        {
                            vector<string> msg1 = {
                                "INVALID ID"};
                            printConsoleBox(msg1);
                        }
                    }
                    exitMenu = true;
                    Ring->sortList();
                    break;
                }
                }
            }
            }
    }
    if (Ring->head != NULL)
        Ring->head->rTable->setTable(this);
}
DHTNode *DHT::FindMachine(BigInt target, DHTNode *Mac, string &path)
{
    if ((Mac == Ring->head && target <= Mac->id) || Mac->id == target)
    {
        path.pop_back();
        path.pop_back();
        return Mac;
    }
    if (Mac->id < target && (target <= Mac->next->id || Mac->next == Ring->head))
    {
        path = path + string(Mac->next->id);
        return Mac->next;
    }
    DHTNode1 *Pre = Mac->rTable->List.findPredecessor(target, this);
    path = path + string(Pre->id->id) + "->";
    return FindMachine(target, Pre->id, path);
}

void DHT::ControlMenu()
{
    int choice;
    BigInt zero(0);
    bool exitMenu = false;
    while (exitMenu == false)
    {
        printMenu1();
        cin >> choice;
        if (choice > 0 && choice < 11)
        {
            switch (choice)
            {
            case 1:
            {
                if (Machines == zero)
                {
                    vector<string> error = {
                        "No Machines in DHT RING"};
                    printConsoleBox(error);
                    break;
                }
                vector<string> msg = {
                    "Select Machine:",
                    Ring->print()};
                printConsoleBox(msg);
                string temp;
                cin >> temp;
                BigInt temp1(temp);
                DHTNode *dummy = Ring->searchNode(temp1);
                while (dummy == nullptr)
                {
                    vector<string> msg1 = {
                        "INVALID MACHINE",
                        "Re-Select Machine:",
                        Ring->print()};
                    printConsoleBox(msg1);
                    cin >> temp;
                    temp1 = BigInt(temp);
                    dummy = Ring->searchNode(temp1);
                }
                vector<string> Find = {
                    "Find File to Store"};
                printConsoleBox(Find);
                string fpath = Navigator(basepath + PATH_SEPARATOR + "aTextFiles/");
                int res = fpath.compare("\0");
                if (res == 0)
                {
                    vector<string> error = {
                        "No File Selected"};
                    printConsoleBox(error);
                    break;
                }
                // Enhanced file insertion method

                // Check if file path exists and is readable
                if (!fs::exists(fpath))
                {
                    vector<string> error = {"File does not exist"};
                    printConsoleBox(error);
                    break;
                }

                // Check file size before reading
                uintmax_t fileSize = fs::file_size(fpath);
                const uintmax_t MAX_FILE_SIZE = 100 * 1024 * 1024; // 100 MB limit

                if (fileSize > MAX_FILE_SIZE)
                {
                    vector<string> error = {"File too large. Max 100MB allowed"};
                    printConsoleBox(error);
                    break;
                }

                // Use more robust file reading
                std::ifstream file(fpath, std::ios::binary);
                if (!file)
                {
                    vector<string> error = {"Cannot open file"};
                    printConsoleBox(error);
                    break;
                }

                // Use vector for safer memory management
                std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
                file.close();

                // Check if file read was successful
                if (buffer.empty())
                {
                    vector<string> error = {"Failed to read file content"};
                    printConsoleBox(error);
                    break;
                }

                // Convert buffer to string
                string content(buffer.begin(), buffer.end());

                string hashed = sha1(content);
                BigInt a;
                a = a.HexToDecimal(hashed);
                a = a % *totalnodes;

                string path;
                path = path + string(dummy->id) + "->";

                // Add null checks
                if (dummy == nullptr)
                {
                    vector<string> error = {"Invalid machine selection"};
                    printConsoleBox(error);
                    break;
                }

                DHTNode *Destination = FindMachine(a, dummy, path);

                if (Destination == nullptr)
                {
                    vector<string> error = {"No destination machine found"};
                    printConsoleBox(error);
                    break;
                }

                // Use modern C++ nullptr instead of NULL
                if (Destination->storage == nullptr)
                {
                    try
                    {
                        Destination->storage = new BTree(order);
                    }
                    catch (const std::bad_alloc &e)
                    {
                        vector<string> error = {"Memory allocation failed for B-Tree"};
                        printConsoleBox(error);
                        break;
                    }
                }

                // Safer file path handling
                string originalFileName = fs::path(fpath).filename().string();

                // Ensure destination folder exists
                fs::create_directories(Destination->FolderPath);

                string destinationFilePath = Destination->FolderPath + PATH_SEPARATOR + originalFileName;

                try
                {
                    // Add error handling for file copy
                    fs::copy_file(fpath, destinationFilePath,
                                  fs::copy_options::overwrite_existing);
                }
                catch (const fs::filesystem_error &e)
                {
                    vector<string> error = {"File copy failed: " + string(e.what())};
                    printConsoleBox(error);
                    break;
                }

                try
                {
                    Destination->storage->insert(a, destinationFilePath);
                }
                catch (const std::exception &e)
                {
                    vector<string> error = {"B-Tree insertion failed: " + string(e.what())};
                    printConsoleBox(error);
                    break;
                }

                vector<string> insertion = {
                    "Path Taken:",
                    path,
                    "File Key: ",
                    a,
                    "1.Print B-Tree",
                    "2.EXIT"};

                choice = -1;
                while (choice < 1 || choice > 2)
                {
                    printConsoleBox(insertion);
                    cin >> choice;
                }

                if (choice == 1)
                {
                    string t;
                    Destination->storage->print(t);
                    vector<string> btree = {
                        "B-Tree of Machine " + string(Destination->id) + " is: ",
                        t};
                    printConsoleBox(btree);
                }

                // Safe addition to vector
                try
                {
                    AddedFiles.push_back(destinationFilePath);
                }
                catch (const std::bad_alloc &e)
                {
                    vector<string> error = {"Failed to add file to list"};
                    printConsoleBox(error);
                }
                break;
            }

            case 2:
            {
                if (Machines == zero)
                {
                    vector<string> error = {
                        "No Machine Present in Ring"};
                    printConsoleBox(error);
                    break;
                }
                else if (AddedFiles.size() == 0)
                {
                    vector<string> error = {
                        "No Files Present in Ring"};
                    printConsoleBox(error);
                    break;
                }

                else
                {
                    vector<string> msg = {
                        "Select Machine:",
                        Ring->print()};
                    printConsoleBox(msg);
                    string temp;
                    cin >> temp;
                    BigInt temp1(temp);
                    DHTNode *dummy = Ring->searchNode(temp1);
                    while (dummy == nullptr)
                    {
                        vector<string> msg1 = {
                            "INVALID MACHINE",
                            "Re-Select Machine:",
                            Ring->print()};
                        printConsoleBox(msg1);
                        cin >> temp;
                        temp1 = BigInt(temp);
                        dummy = Ring->searchNode(temp1);
                    }
                    vector<string> Display;
                    Display.push_back("Added Files");
                    for (int i = 0; i < AddedFiles.size(); i++)
                    {
                        string add = to_string(i + 1) + "-" + AddedFiles[i];
                        Display.push_back(add);
                    }
                    Display.push_back("1.Select File");
                    Display.push_back("2.EXIT");
                    bool exitMenu2 = false;
                    while (exitMenu2 == false)
                    {
                        int choice2 = 0;
                        while (choice2 < 1 || choice2 > 2)
                        {
                            printConsoleBox(Display);
                            cin >> choice2;
                        }
                        switch (choice2)
                        {
                        case 1:
                        {
                            vector<string> options = {
                                "Enter File Index"};
                            printConsoleBox(options);
                            int choice3 = -1;
                            cin >> choice3;
                            while (choice < 1 && choice > (AddedFiles.size() + 1))
                            {
                                vector<string> Err = {
                                    "Invalid Choice | Re-Enter: "};
                                printConsoleBox(Err);
                                cin >> choice3;
                            }
                            string fpath = AddedFiles[choice3 - 1];
                            string content;
                            ifstream file(fpath);
                            if (file.is_open())
                            {
                                string content1((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                                content = content1;
                                file.close();
                            }
                            string hashed = sha1(content);
                            BigInt a;
                            a = a.HexToDecimal(hashed);
                            a = a % *totalnodes;
                            string path;
                            path = path + string(dummy->id) + "->";
                            DHTNode *Destination = FindMachine(a, dummy, path);
                            vector<string> insertion = {
                                "Path Taken:",
                                path,
                                "File Key: ",
                                a,
                                "File Value: ",
                                fpath};
                            fs::remove(fpath);
                            Destination->storage->deleteKey(a, fpath);
                            printConsoleBox(insertion);
                            if (AddedFiles.size() > 1)
                                swap(AddedFiles[choice3 - 1], AddedFiles.back());
                            AddedFiles.pop_back();
                            string t;
                            if (Destination->storage == nullptr || Destination->storage->getRoot()->values.size() == 0)
                            {
                                t = "B-Tree is Empty";
                            }
                            else
                            {
                                Destination->storage->print(t);
                            }
                            vector<string> btree = {
                                "B-Tree of Machine " + string(Destination->id) + " is: ",
                                t};
                            printConsoleBox(btree);
                            exitMenu2 = true;
                            break;
                        }
                        case 2:
                        {
                            exitMenu2 = true;
                            break;
                        }
                        }
                    }
                }
                break;
            }
            case 3:
            {
                if (Machines == zero)
                {
                    vector<string> msg = {
                        Ring->print()};
                    printConsoleBox(msg);
                    break;
                }
                vector<string> msg = {
                    "Select Machine:",
                    Ring->print()};
                printConsoleBox(msg);
                string temp;
                cin >> temp;
                BigInt temp1(temp);
                DHTNode *dummy = Ring->searchNode(temp1);
                while (dummy == nullptr)
                {
                    vector<string> msg1 = {
                        "INVALID MACHINE",
                        "Re-Select Machine:",
                        Ring->print()};
                    printConsoleBox(msg1);
                    cin >> temp;
                    temp1 = BigInt(temp);
                    dummy = Ring->searchNode(temp1);
                }
                string output;
                output += "Machine " + string(dummy->id) + "'s Routing Table: ";
                vector<string> msg2 = {
                    output,
                    dummy->rTable->List.print()};
                printConsoleBox(msg2);
                break;
            }
            case 4:
            {
                if (Machines == (*totalnodes))
                {
                    vector<string> error = {
                        "Max Machines Already Present"};
                    printConsoleBox(error);
                    break;
                }
                vector<string> msg = {
                    "Current Machines:",
                    Ring->print(),
                    "Enter ID of New Machine:"};
                printConsoleBox(msg);
                string temp;
                cin >> temp;
                BigInt temp1(temp);
                bool go = Ring->search(temp1);
                while (temp1 >= *totalnodes || go == true)
                {
                    if (temp1 >= *totalnodes)
                    {
                        vector<string> msg1 = {
                            "INVALID Machine ID",
                            "Re-Enter:"};
                        printConsoleBox(msg1);
                    }
                    else if (go == true)
                    {
                        vector<string> msg1 = {
                            "Machine Already Present",
                            "Re-Enter:"};
                        printConsoleBox(msg1);
                    }
                    string temp;
                    cin >> temp;
                    temp1 = BigInt(temp);
                    go = Ring->search(temp1);
                }
                Ring->insert(temp1);
                Machines++;
                Ring->sortList();
                DHTNode *Dest = Ring->searchNode(temp1);
                if (Dest->storage == nullptr)
                {
                    Dest->storage = new BTree(order);
                }
                if (Dest->next->storage != nullptr && Dest->next->storage->getRoot() != nullptr && Dest->next->storage->getRoot()->keys.size() != 0)
                {
                    Dest->next->storage->CompareAndMerge(Dest->storage, Dest->id, Dest->next->id, AddedFiles, Ring->head->id, Ring->last->id);
                }
                Ring->head->rTable->setTable(this);
                break;
            }
            case 5:
            {
                if (Machines == zero)
                {
                    vector<string> error = {
                        "No Machines in DHT RING"};
                    printConsoleBox(error);
                    break;
                }
                vector<string> msg = {
                    "Select Machine:",
                    Ring->print()};
                printConsoleBox(msg);
                string temp;
                cin >> temp;
                BigInt temp1(temp);
                DHTNode *dummy = Ring->searchNode(temp1);
                while (dummy == nullptr)
                {
                    vector<string> msg1 = {
                        "INVALID MACHINE",
                        "Re-Select Machine:",
                        Ring->print()};
                    printConsoleBox(msg1);
                    cin >> temp;
                    temp1 = BigInt(temp);
                    dummy = Ring->searchNode(temp1);
                }
                BTree *deletedtree = dummy->storage;
                if (dummy->next != dummy)
                    if (deletedtree != nullptr)
                    {
                        if (deletedtree->getRoot() != nullptr)
                        {
                            if (dummy->next->storage == nullptr)
                            {
                                dummy->next->storage = new BTree(order);
                            }
                            dummy->storage->emptyAndMerge(dummy->next->storage, dummy->next->id, dummy->id, AddedFiles);
                        }
                    }
                Ring->deleteData(temp1);
                Machines--;
                if (Machines > zero)
                    Ring->head->rTable->setTable(this);
                else
                {
                    AddedFiles.clear();
                }
                break;
            }
            case 6:
            {
                vector<string> msg = {
                    "DHT RING",
                    Ring->print()};
                printConsoleBox(msg);
                break;
            }
            case 7:
            {

                if (Machines == zero)
                {
                    vector<string> error = {
                        "No Machine Present in Ring"};
                    printConsoleBox(error);
                    break;
                }
                else if (AddedFiles.size() == 0)
                {
                    vector<string> error = {
                        "No Files Present in Ring"};
                    printConsoleBox(error);
                    break;
                }

                else
                {
                    vector<string> msg = {
                        "Select Machine:",
                        Ring->print()};
                    printConsoleBox(msg);
                    string temp;
                    cin >> temp;
                    BigInt temp1(temp);
                    DHTNode *dummy = Ring->searchNode(temp1);
                    while (dummy == nullptr)
                    {
                        vector<string> msg1 = {
                            "INVALID MACHINE",
                            "Re-Select Machine:",
                            Ring->print()};
                        printConsoleBox(msg1);
                        cin >> temp;
                        temp1 = BigInt(temp);
                        dummy = Ring->searchNode(temp1);
                    }
                    vector<string> Display;
                    Display.push_back("Added Files");
                    for (int i = 0; i < AddedFiles.size(); i++)
                    {
                        string add = to_string(i + 1) + "-" + AddedFiles[i];
                        Display.push_back(add);
                    }
                    Display.push_back("1.Select File");
                    Display.push_back("2.EXIT");
                    bool exitMenu2 = false;
                    while (exitMenu2 == false)
                    {
                        int choice2 = 0;
                        while (choice2 < 1 || choice2 > 2)
                        {
                            printConsoleBox(Display);
                            cin >> choice2;
                        }
                        switch (choice2)
                        {
                        case 1:
                        {
                            vector<string> options = {
                                "Enter File Index"};
                            printConsoleBox(options);
                            int choice3 = -1;
                            cin >> choice3;
                            while (choice < 1 && choice > (AddedFiles.size() + 1))
                            {
                                vector<string> Err = {
                                    "Invalid Choice | Re-Enter: "};
                                printConsoleBox(Err);
                                cin >> choice3;
                            }
                            string fpath = AddedFiles[choice3 - 1];
                            string content;
                            ifstream file(fpath);
                            if (file.is_open())
                            {
                                string content1((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                                content = content1;
                                file.close();
                            }
                            string hashed = sha1(content);
                            BigInt a;
                            a = a.HexToDecimal(hashed);
                            a = a % *totalnodes;
                            string path;
                            path = path + string(dummy->id) + "->";
                            DHTNode *Destination = FindMachine(a, dummy, path);
                            vector<string> insertion = {
                                "Path Taken:",
                                path,
                                "File Key: ",
                                a,
                                "File Content: ",
                                content};
                            printConsoleBox(insertion);
                            exitMenu2 = true;
                            break;
                        }
                        case 2:
                        {
                            exitMenu2 = true;
                            break;
                        }
                        }
                    }
                }
                break;
            }
            case 8:
            {
                if (identifier_space > 0)
                {
                    string out;
                    out = out + "Identifier Space Size: " + to_string(identifier_space) + " No of Machines: " + string(Machines) + " Max Nodes: " + string(*totalnodes) + " B-Tree Order: " + to_string(order);
                    vector<string> out1 = {
                        out};
                    printConsoleBox(out1);
                }
                else
                {
                    vector<string> msg = {
                        "No Ring Info Available"};
                    printConsoleBox(msg);
                }
                break;
            }
            case 9:
            {
                if (Machines == zero)
                {
                    vector<string> error = {
                        "No Machines in DHT RING"};
                    printConsoleBox(error);
                    break;
                }
                vector<string> msg = {
                    "Select Machine:",
                    Ring->print()};
                printConsoleBox(msg);
                string temp;
                cin >> temp;
                BigInt temp1(temp);
                DHTNode *dummy = Ring->searchNode(temp1);
                while (dummy == nullptr)
                {
                    vector<string> msg1 = {
                        "INVALID MACHINE",
                        "Re-Select Machine:",
                        Ring->print()};
                    printConsoleBox(msg1);
                    cin >> temp;
                    temp1 = BigInt(temp);
                    dummy = Ring->searchNode(temp1);
                }
                if (dummy->storage == nullptr || dummy->storage->getRoot()->values.size() == 0)
                {
                    vector<string> msg1 = {
                        "B-Tree is Empty"};
                    printConsoleBox(msg1);
                    break;
                }
                string tree;
                dummy->storage->print(tree);
                vector<string> btree = {"B-Tree of Machine " + string(dummy->id) + " is: ", tree};
                printConsoleBox(btree);
                break;
            }
            case 10:
            {
                exitMenu = true;
                this->Ring->~CircularLinkedList();
                break;
            }
            }
        }
    }
}

void RoutingTable::setTable(DHT *other)
{
    DHTNode *tempNode = other->Ring->last;
    for (BigInt i(0); i < other->Machines; i++)
    {
        tempNode = tempNode->next;
        if (tempNode->rTable != nullptr)
        {
            delete tempNode->rTable;
            tempNode->rTable = NULL;
        }
        tempNode->rTable = new RoutingTable;
        for (BigInt j(0); j < other->Machines; j++)
        {
            BigInt temp = tempNode->id;
            BigInt two(2);
            two = two.Power(string(j));
            temp = temp + two;
            while (temp >= (*other->totalnodes + other->Ring->head->id))
            {
                temp = temp % *other->totalnodes;
            }
            tempNode->rTable->List.insert(other->Ring->findSuccessor(temp, *other->totalnodes));
        }
    }
}

int main()
{
    srand(time(0));
    DHT dhtring;
    dhtring.ControlMenu();
    return 0;
}