#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>  // Provides std::quoted
using namespace std;

// These members and helper functions belong inside TraceSystem:
private:
    stack<Action> undoStack;
    stack<Action> redoStack;
    vector<Action> history;
    int nextID;

    int findActionIndex(int id)
    {
        for (int i = 0; i < (int)history.size(); i++)
        {
            if (history[i].id == id)
                return i;
        }
        return -1;
    }

    void updateStatus(int id, bool status)
    {
        int index = findActionIndex(id);
        if (index != -1)
            history[index].active = status;
    }

    bool getActionByID(int id, Action &result)
    {
        int index = findActionIndex(id);
        if (index == -1)
            return false;

        result = history[index];
        return true;
    }

public:
    // Undo the most recent active action.
    void undoAction()
    {
        if (undoStack.empty())
        {
            cout << "\nNo active action is available to undo.\n";
            return;
        }

        Action action = undoStack.top();
        undoStack.pop();

        redoStack.push(action);
        updateStatus(action.id, false);

        cout << "\nUndo completed: " << action.description << "\n";
    }

    // Redo the most recently undone action.
    void redoAction()
    {
        if (redoStack.empty())
        {
            cout << "\nNo action is available to redo.\n";
            return;
        }

        Action action = redoStack.top();
        redoStack.pop();

        action.active = true;
        undoStack.push(action);
        updateStatus(action.id, true);

        cout << "\nRedo completed: " << action.description << "\n";
    }

    // Display the undo stack without removing its entries.
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

    // Display the redo stack without removing its entries.
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

    // Load saved history, undo/redo state, and pending action IDs.
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
            cout << "\nInvalid data file format.\n";
            return false;
        }

        int historyCount;
        if (!(file >> historyCount) || historyCount < 0)
        {
            cout << "\nInvalid history data.\n";
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
                maximumID = action.id;
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

        // Apply loaded data after reading it successfully.
        history = loadedHistory;
        undoStack = stack<Action>();
        redoStack = stack<Action>();
        actionQueue = queue<Action>();

        for (int i = 0; i < (int)history.size(); i++)
        {
            if (history[i].active)
                undoStack.push(history[i]);
        }

        // The saved redo IDs are listed from top to bottom.
        for (int i = (int)loadedRedoIDs.size() - 1; i >= 0; i--)
        {
            Action action;
            if (getActionByID(loadedRedoIDs[i], action))
                redoStack.push(action);
        }

        for (int i = 0; i < (int)loadedQueueIDs.size(); i++)
        {
            Action action;
            if (getActionByID(loadedQueueIDs[i], action))
                actionQueue.push(action);
        }

        nextID = maximumID + 1;

        cout << "\nData loaded successfully from " << filename << "\n";
        cout << "Loaded actions: " << history.size() << "\n";
        return true;
    }
