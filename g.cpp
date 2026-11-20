#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <iomanip>
#include <limits>
#include <cctype>

using namespace std;

// CLASS 1: ACTION
// Stores the details of one activity.
class Action
{
public:
    int id;
    string description;
    string category;
    bool active;

    Action()
    {
        id = 0;
        description = "";
        category = "";
        active = true;
    }

    Action(int i, string d, string c)
    {
        id = i;
        description = d;
        category = c;
        active = true;
    }
};

// Reads an integer safely and checks its range.
int readInteger(string message, int minimum, int maximum)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value && value >= minimum && value <= maximum)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid input. Enter a number between "
             << minimum << " and " << maximum << ".\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Reads a complete line, including spaces.
string readText(string message)
{
    string value;

    cout << message;
    getline(cin, value);

    while (value.empty())
    {
        cout << "Input cannot be empty. Try again: ";
        getline(cin, value);
    }

    return value;
}

// Converts a string to lowercase for case-insensitive searching.
string toLowerCase(string text)
{
    for (int i = 0; i < (int)text.length(); i++)
    {
        text[i] = (char)tolower((unsigned char)text[i]);
    }

    return text;
}

// CLASS 2: TRACE SYSTEM
class TraceSystem
{
private:
    stack<Action> undoStack;
    stack<Action> redoStack;
    vector<Action> history;
    queue<Action> actionQueue;
    int nextID;

    // Finds an action in the permanent history using its ID.
    int findActionIndex(int id)
    {
        for (int i = 0; i < (int)history.size(); i++)
        {
            if (history[i].id == id)
            {
                return i;
            }
        }

        return -1;
    }

    // Updates the active/undone status in the history vector.
    void updateStatus(int id, bool status)
    {
        int index = findActionIndex(id);

        if (index != -1)
        {
            history[index].active = status;
        }
    }

    // Finds an action by ID and copies it into result.
    bool getActionByID(int id, Action &result)
    {
        int index = findActionIndex(id);

        if (index == -1)
        {
            return false;
        }

        result = history[index];
        return true;
    }

    void printAction(Action action)
    {
        cout << "ID: " << action.id << "\n";
        cout << "Description: " << action.description << "\n";
        cout << "Category: " << action.category << "\n";
        cout << "Status: " << (action.active ? "Active" : "Undone") << "\n";
    }

public:
    // CONSTRUCTOR
    TraceSystem()
    {
        nextID = 1;

        cout << "\n========================================\n";
        cout << "             TRACE SYSTEM\n";
        cout << "       Undo, Redo & History Analyzer\n";
        cout << "========================================\n";
    }

    // FUNCTION 1: ADD NEW ACTION
    void addAction(string description, string category)
    {
        Action newAction(nextID, description, category);

        undoStack.push(newAction);

        // A new action makes the old redo sequence invalid.
        while (!redoStack.empty())
        {
            redoStack.pop();
        }

        history.push_back(newAction);
        actionQueue.push(newAction);

        cout << "\nAction added successfully.\n";
        cout << "Action ID: " << nextID << "\n";

        nextID++;
    }

    // FUNCTION 2: UNDO
    void undoAction()
    {
        if (undoStack.empty())
        {
            cout << "\nNo active action is available to undo.\n";
            return;
        }

        Action lastAction = undoStack.top();
        undoStack.pop();

        redoStack.push(lastAction);
        updateStatus(lastAction.id, false);

        cout << "\nUndo completed successfully.\n";
        cout << "Undone action: " << lastAction.description << "\n";
    }

    // FUNCTION 3: REDO
    void redoAction()
    {
        if (redoStack.empty())
        {
            cout << "\nNo action is available to redo.\n";
            return;
        }

        Action lastAction = redoStack.top();
        redoStack.pop();

        lastAction.active = true;
        undoStack.push(lastAction);
        updateStatus(lastAction.id, true);

        cout << "\nRedo completed successfully.\n";
        cout << "Restored action: " << lastAction.description << "\n";
    }

    // FUNCTION 4: DISPLAY COMPLETE HISTORY
    void displayHistory()
    {
        cout << "\n========== COMPLETE ACTIVITY HISTORY ==========\n";

        if (history.empty())
        {
            cout << "No activities recorded yet.\n";
            return;
        }

        for (int i = 0; i < (int)history.size(); i++)
        {
            cout << "\n";
            printAction(history[i]);
            cout << "-----------------------------------------------\n";
        }
    }

    // FUNCTION 5: DISPLAY ACTIVE ACTIONS
    void displayActiveActions()
    {
        cout << "\n========== ACTIVE ACTIONS ==========\n";

        bool found = false;

        for (int i = 0; i < (int)history.size(); i++)
        {
            if (history[i].active)
            {
                printAction(history[i]);
                cout << "-----------------------------------\n";
                found = true;
            }
        }

        if (!found)
        {
            cout << "No active actions found.\n";
        }
    }

    // FUNCTION 6: DISPLAY UNDO STACK
    void displayUndoStack()
    {
        cout << "\n========== UNDO STACK (TOP FIRST) ==========\n";

        if (undoStack.empty())
        {
            cout << "Undo stack is empty.\n";
            return;
        }

        stack<Action> temp = undoStack;

        while (!temp.empty())
        {
            cout << "ID: " << temp.top().id
                 << " | " << temp.top().description << "\n";
            temp.pop();
        }
    }

    // FUNCTION 7: DISPLAY REDO STACK
    void displayRedoStack()
    {
        cout << "\n========== REDO STACK (TOP FIRST) ==========\n";

        if (redoStack.empty())
        {
            cout << "Redo stack is empty.\n";
            return;
        }

        stack<Action> temp = redoStack;

        while (!temp.empty())
        {
            cout << "ID: " << temp.top().id
                 << " | " << temp.top().description << "\n";
            temp.pop();
        }
    }

    // FUNCTION 8: SEARCH ACTIVITY
    void searchAction(string searchText)
    {
        cout << "\n========== SEARCH RESULTS ==========\n";

        string key = toLowerCase(searchText);
        bool found = false;

        for (int i = 0; i < (int)history.size(); i++)
        {
            string description = toLowerCase(history[i].description);
            string category = toLowerCase(history[i].category);

            if (description.find(key) != string::npos ||
                category.find(key) != string::npos)
            {
                printAction(history[i]);
                cout << "-----------------------------------\n";
                found = true;
            }
        }

        if (!found)
        {
            cout << "No matching activity found.\n";
        }
    }

    // FUNCTION 9: HISTORY ANALYZER
    void analyzeHistory()
    {
        int activeActions = 0;
        int undoneActions = 0;
        map<string, int> categoryCount;
        map<string, int> activeCategoryCount;

        for (int i = 0; i < (int)history.size(); i++)
        {
            categoryCount[history[i].category]++;

            if (history[i].active)
            {
                activeActions++;
                activeCategoryCount[history[i].category]++;
            }
            else
            {
                undoneActions++;
            }
        }

        cout << "\n========== HISTORY ANALYZER ==========\n";
        cout << "Total recorded actions: " << history.size() << "\n";
        cout << "Active actions: " << activeActions << "\n";
        cout << "Undone actions: " << undoneActions << "\n";

        cout << "\n---------- CATEGORY REPORT ----------\n";

        if (categoryCount.empty())
        {
            cout << "No category data available.\n";
        }
        else
        {
            for (map<string, int>::iterator it = categoryCount.begin();
                 it != categoryCount.end(); ++it)
            {
                cout << it->first << ": " << it->second
                     << " total, "
                     << activeCategoryCount[it->first]
                     << " active\n";
            }
        }
    }

    // FUNCTION 10: PROCESS ONE ITEM FROM THE QUEUE
    void processQueue()
    {
        cout << "\n========== PROCESS ACTIVITY QUEUE ==========\n";

        if (actionQueue.empty())
        {
            cout << "No activities are waiting for processing.\n";
            return;
        }

        Action currentAction = actionQueue.front();
        actionQueue.pop();

        cout << "Activity processed.\n";
        cout << "Action ID: " << currentAction.id << "\n";
        cout << "Description: " << currentAction.description << "\n";
    }

    // FUNCTION 11: DISPLAY PENDING QUEUE
    void displayQueue()
    {
        cout << "\n========== PENDING ACTIVITY QUEUE ==========\n";

        if (actionQueue.empty())
        {
            cout << "Queue is empty.\n";
            return;
        }

        queue<Action> temp = actionQueue;

        while (!temp.empty())
        {
            cout << "ID: " << temp.front().id
                 << " | " << temp.front().description << "\n";
            temp.pop();
        }
    }

    // FUNCTION 12: DISPLAY STATISTICS
    void displayStatistics()
    {
        int active = 0;
        int undone = 0;

        for (int i = 0; i < (int)history.size(); i++)
        {
            if (history[i].active)
                active++;
            else
                undone++;
        }

        cout << "\n========== SYSTEM STATISTICS ==========\n";
        cout << "Total actions: " << history.size() << "\n";
        cout << "Active actions: " << active << "\n";
        cout << "Undone actions: " << undone << "\n";
        cout << "Actions available to undo: " << undoStack.size() << "\n";
        cout << "Actions available to redo: " << redoStack.size() << "\n";
        cout << "Activities waiting in queue: " << actionQueue.size() << "\n";
    }

    // FUNCTION 13: SORT HISTORY
    void sortHistory()
    {
        if (history.empty())
        {
            cout << "\nThere is no history to sort.\n";
            return;
        }

        cout << "\nSort history by:\n";
        cout << "1. Action ID\n";
        cout << "2. Description (alphabetical)\n";
        cout << "3. Category (alphabetical)\n";

        int choice = readInteger("Enter choice: ", 1, 3);

        // Sort a copy so the original history order is not changed.
        vector<Action> sortedHistory = history;

        if (choice == 1)
        {
            sort(sortedHistory.begin(), sortedHistory.end(),
                 [](const Action &a, const Action &b)
                 {
                     return a.id < b.id;
                 });
        }
        else if (choice == 2)
        {
            sort(sortedHistory.begin(), sortedHistory.end(),
                 [](const Action &a, const Action &b)
                 {
                     return toLowerCase(a.description) <
                            toLowerCase(b.description);
                 });
        }
        else
        {
            sort(sortedHistory.begin(), sortedHistory.end(),
                 [](const Action &a, const Action &b)
                 {
                     return toLowerCase(a.category) <
                            toLowerCase(b.category);
                 });
        }

        cout << "\n========== SORTED HISTORY ==========\n";

        for (int i = 0; i < (int)sortedHistory.size(); i++)
        {
            printAction(sortedHistory[i]);
            cout << "-----------------------------------\n";
        }
    }

    // FUNCTION 14: SAVE DATA TO A FILE
    bool saveToFile(string filename)
    {
        ofstream file(filename.c_str());

        if (!file)
        {
            cout << "\nUnable to open file for saving.\n";
            return false;
        }

        file << "TRACE_DATA_V1\n";

        // Save complete history.
        file << history.size() << "\n";

        for (int i = 0; i < (int)history.size(); i++)
        {
            file << history[i].id << " "
                 << history[i].active << " "
                 << quoted(history[i].description) << " "
                 << quoted(history[i].category) << "\n";
        }

        // Save redo stack from top to bottom.
        stack<Action> redoCopy = redoStack;
        vector<int> redoIDs;

        while (!redoCopy.empty())
        {
            redoIDs.push_back(redoCopy.top().id);
            redoCopy.pop();
        }

        file << redoIDs.size() << "\n";

        for (int i = 0; i < (int)redoIDs.size(); i++)
        {
            file << redoIDs[i] << "\n";
        }

        // Save pending queue in FIFO order.
        queue<Action> queueCopy = actionQueue;
        vector<int> queueIDs;

        while (!queueCopy.empty())
        {
            queueIDs.push_back(queueCopy.front().id);
            queueCopy.pop();
        }

        file << queueIDs.size() << "\n";

        for (int i = 0; i < (int)queueIDs.size(); i++)
        {
            file << queueIDs[i] << "\n";
        }

        file.close();

        cout << "\nData saved successfully in " << filename << "\n";
        return true;
    }

    // FUNCTION 15: LOAD DATA FROM A FILE
    bool loadFromFile(string filename)
    {
        ifstream file(filename.c_str());

        if (!file)
        {
            cout << "\nUnable to open file. No data loaded.\n";
            return false;
        }

        string header;
        getline(file, header);

        if (header != "TRACE_DATA_V1")
        {
            cout << "\nInvalid TRACE data file format.\n";
            return false;
        }

        int historyCount;

        if (!(file >> historyCount) || historyCount < 0)
        {
            cout << "\nFile contains invalid history data.\n";
            return false;
        }

        vector<Action> loadedHistory;
        int maximumID = 0;

        for (int i = 0; i < historyCount; i++)
        {
            Action action;
            int activeValue;

            if (!(file >> action.id >> activeValue
                      >> quoted(action.description)
                      >> quoted(action.category)))
            {
                cout << "\nFile is incomplete or damaged.\n";
                return false;
            }

            action.active = (activeValue != 0);
            loadedHistory.push_back(action);

            if (action.id > maximumID)
            {
                maximumID = action.id;
            }
        }

        int redoCount;

        if (!(file >> redoCount) || redoCount < 0)
        {
            cout << "\nInvalid redo-stack data.\n";
            return false;
        }

        vector<int> loadedRedoIDs;

        for (int i = 0; i < redoCount; i++)
        {
            int id;

            if (!(file >> id))
            {
                cout << "\nInvalid redo-stack entry.\n";
                return false;
            }

            loadedRedoIDs.push_back(id);
        }

        int queueCount;

        if (!(file >> queueCount) || queueCount < 0)
        {
            cout << "\nInvalid queue data.\n";
            return false;
        }

        vector<int> loadedQueueIDs;

        for (int i = 0; i < queueCount; i++)
        {
            int id;

            if (!(file >> id))
            {
                cout << "\nInvalid queue entry.\n";
                return false;
            }

            loadedQueueIDs.push_back(id);
        }

        // Replace the current data only after the file is read successfully.
        history = loadedHistory;
        undoStack = stack<Action>();
        redoStack = stack<Action>();
        actionQueue = queue<Action>();

        // Rebuild undo stack in chronological order.
        for (int i = 0; i < (int)history.size(); i++)
        {
            if (history[i].active)
            {
                undoStack.push(history[i]);
            }
        }

        // Rebuild redo stack. The saved list is top-first,
        // so push its elements in reverse order.
        for (int i = (int)loadedRedoIDs.size() - 1; i >= 0; i--)
        {
            Action action;

            if (getActionByID(loadedRedoIDs[i], action))
            {
                redoStack.push(action);
            }
        }

        // Rebuild the pending queue in its original order.
        for (int i = 0; i < (int)loadedQueueIDs.size(); i++)
        {
            Action action;

            if (getActionByID(loadedQueueIDs[i], action))
            {
                actionQueue.push(action);
            }
        }

        nextID = maximumID + 1;

        file.close();

        cout << "\nData loaded successfully from " << filename << "\n";
        cout << "Loaded actions: " << history.size() << "\n";

        return true;
    }

    // DESTRUCTOR
    ~TraceSystem()
    {
        cout << "\nTRACE System object destroyed.\n";
    }
};

// FUNCTION: DISPLAY MENU
void displayMenu()
{
    cout << "\n========================================\n";
    cout << "              TRACE MENU\n";
    cout << "========================================\n";
    cout << "1.  Add New Action\n";
    cout << "2.  Undo Last Action\n";
    cout << "3.  Redo Last Action\n";
    cout << "4.  Display Complete History\n";
    cout << "5.  Display Active Actions\n";
    cout << "6.  Display Undo Stack\n";
    cout << "7.  Display Redo Stack\n";
    cout << "8.  Search Activity\n";
    cout << "9.  Analyze History\n";
    cout << "10. Process Activity Queue\n";
    cout << "11. Display Activity Queue\n";
    cout << "12. Display System Statistics\n";
    cout << "13. Sort History\n";
    cout << "14. Save Data to File\n";
    cout << "15. Load Data from File\n";
    cout << "16. Show Menu\n";
    cout << "0.  Exit Program\n";
    cout << "========================================\n";
}

// MAIN FUNCTION
int main()
{
    TraceSystem trace;

    int choice;
    string description;
    string category;
    string searchText;
    string filename;

    // Show the full menu only once when the program starts.
    displayMenu();

    do
    {
        // After each operation, ask only for the next choice.
        choice = readInteger("\nEnter your choice (0-16): ", 0, 16);

        switch (choice)
        {
        case 1:
        {
            description = readText("\nEnter activity description: ");

            cout << "\nSelect category:\n";
            cout << "1. Study\n";
            cout << "2. Coding\n";
            cout << "3. Personal\n";
            cout << "4. Other\n";

            int categoryChoice = readInteger("Enter category number: ", 1, 4);

            if (categoryChoice == 1)
                category = "Study";
            else if (categoryChoice == 2)
                category = "Coding";
            else if (categoryChoice == 3)
                category = "Personal";
            else
                category = "Other";

            trace.addAction(description, category);
            break;
        }

        case 2:
            trace.undoAction();
            break;

        case 3:
            trace.redoAction();
            break;

        case 4:
            trace.displayHistory();
            break;

        case 5:
            trace.displayActiveActions();
            break;

        case 6:
            trace.displayUndoStack();
            break;

        case 7:
            trace.displayRedoStack();
            break;

        case 8:
            searchText = readText("\nEnter description or category to search: ");
            trace.searchAction(searchText);
            break;

        case 9:
            trace.analyzeHistory();
            break;

        case 10:
            trace.processQueue();
            break;

        case 11:
            trace.displayQueue();
            break;

        case 12:
            trace.displayStatistics();
            break;

        case 13:
            trace.sortHistory();
            break;

        case 14:
            filename = readText("\nEnter filename (example: trace_data.txt): ");
            trace.saveToFile(filename);
            break;

        case 15:
            filename = readText("\nEnter filename to load: ");
            trace.loadFromFile(filename);
            break;

        case 16:
            displayMenu();
            break;

        case 0:
            cout << "\nThank you for using TRACE!\n";
            cout << "Program closed successfully.\n";
            break;
        }
    }
    while (choice != 0);

    return 0;
}
