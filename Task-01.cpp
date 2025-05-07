#include <iostream>
#include <string>
using namespace std;

struct Event
{
    int id;
    string name;
    string startDateTime;
    int duration;
    Event *left;
    Event *right;

    Event(int eventId, const string &eventName, const string &start, int dur)
    {
        id = eventId;
        name = eventName;
        startDateTime = start;
        duration = dur;
        left = nullptr;
        right = nullptr;
    }
};

class EventSchedulingSystem
{
private:
    Event *root;

    Event *addEvent(Event *node, int id, const string &name, const string &startDateTime, int duration)
    {
        if (!node)
        {
            return new Event(id, name, startDateTime, duration);
        }

        if (startDateTime < node->startDateTime)
        {
            node->left = addEvent(node->left, id, name, startDateTime, duration);
        }
        else if (startDateTime > node->startDateTime)
        {
            node->right = addEvent(node->right, id, name, startDateTime, duration);
        }
        else
        {
            cout << "Error: Event overlap detected!";
        }
        return node;
    }

    Event *findMin(Event *node)
    {
        while (node && node->left)
        {
            node = node->left;
        }
        return node;
    }

    Event *deleteEvent(Event *node, int id)
    {
        if (!node)
        {
            return nullptr;
        }

        if (id < node->id)
        {
            node->left = deleteEvent(node->left, id);
        }
        else if (id > node->id)
        {
            node->right = deleteEvent(node->right, id);
        }
        else
        {
            if (!node->left)
            {
                Event *temp = node->right;
                delete node;
                return temp;
            }
            else if (!node->right)
            {
                Event *temp = node->left;
                delete node;
                return temp;
            }

            Event *temp = findMin(node->right);
            node->id = temp->id;
            node->name = temp->name;
            node->startDateTime = temp->startDateTime;
            node->duration = temp->duration;
            node->right = deleteEvent(node->right, temp->id);
        }
        return node;
    }

    void findOverlappingEvents(Event *node, const string &start, const string &end, bool &overlapFound)
    {
        if (!node)
        {
            return;
        }

        string eventEnd = calculateEndTime(node->startDateTime, node->duration);

        if (!(end <= node->startDateTime || start >= eventEnd))
        {
            overlapFound = true;
            cout << "\nOverlap Found: Event ID: " << node->id << ", Name: " << node->name << " \n";
        }

        findOverlappingEvents(node->left, start, end, overlapFound);
        findOverlappingEvents(node->right, start, end, overlapFound);
    }

    string calculateEndTime(const string &startTime, int duration)
    {
        int hour = stoi(startTime.substr(11, 2));
        int minute = stoi(startTime.substr(14, 2)) + duration;
        hour += minute / 60;
        minute %= 60;
        string endTime = startTime.substr(0, 11) + (hour < 10 ? "0" : "") + to_string(hour) + ":" + (minute < 10 ? "0" : "") + to_string(minute);
        return endTime;
    }

    void calculateFreeTimeSlots(Event *node, const string &day, string &lastEndTime, bool &firstEvent)
    {
        if (!node)
        {
            return;
        }

        calculateFreeTimeSlots(node->left, day, lastEndTime, firstEvent);

        if (node->startDateTime.substr(0, 10) == day)
        {
            if (firstEvent && node->startDateTime.substr(11, 5) != "00:00")
            {
                cout << "\nFree Time Slot: 00:00 - " << node->startDateTime.substr(11, 5) <<endl;
            }

            if (!lastEndTime.empty() && lastEndTime < node->startDateTime.substr(11, 5))
            {
                cout << "\nFree Time Slot: " << lastEndTime << " - " << node->startDateTime.substr(11, 5) <<endl;
            }

            lastEndTime = calculateEndTime(node->startDateTime, node->duration).substr(11, 5);
            firstEvent = false;
        }

        calculateFreeTimeSlots(node->right, day, lastEndTime, firstEvent);
    }

    void printSchedule(Event *node)
    {
        if (!node)
        {
            return;
        }
        printSchedule(node->left);
        cout << "\nEvent ID: " << node->id << ", Name: " << node->name
             << ", Start: " << node->startDateTime << ", Duration: " << node->duration << " minutes" << endl;
        printSchedule(node->right);
    }

public:
    EventSchedulingSystem(){
        root = nullptr;
    }

    void addEvent(int id, const string &name, const string &startDateTime, int duration)
    {
        root = addEvent(root, id, name, startDateTime, duration);
    }

    void deleteEvent(int id)
    {
        root = deleteEvent(root, id);
    }

    void findOverlappingEvents(const string &start, const string &end)
    {
        bool overlapFound = false;
        findOverlappingEvents(root, start, end, overlapFound);
        if (!overlapFound)
        {
            cout << "\nNo overlapping events found.";
        }
    }

    void calculateFreeTimeSlots(const string &day)
    {
        string lastEndTime;
        bool firstEvent = true;
        calculateFreeTimeSlots(root, day, lastEndTime, firstEvent);
        if (!firstEvent && lastEndTime != "24:00")
        {
            cout << "\nFree Time Slot: " << lastEndTime << " - 24:00";
        }
    }

    void printSchedule()
    {
        cout << "\nFull Event Schedule:";
        printSchedule(root);
    }
};

int main()
{
    EventSchedulingSystem scheduler;
    int choice;

    do
    {
        cout << "\nEvent Scheduling System Menu:" << endl;
        cout << "1. Add Event" << endl;
        cout << "2. Delete Event" << endl;
        cout << "3. Find Overlapping Events" << endl;
        cout << "4. Calculate Free Time Slots" << endl;
        cout << "5. Print Full Schedule" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int id, duration;
            string name, startDateTime;
            cout << "\nEnter Event ID: ";
            cin >> id;
            cin.ignore();
            cout << "Enter Event Name: ";
            getline(cin, name);
            cout << "Enter Start Date and Time (YYYY-MM-DD HH:MM): ";
            getline(cin, startDateTime);
            cout << "Enter Duration (in minutes): ";
            cin >> duration;
            scheduler.addEvent(id, name, startDateTime, duration);
            break;
        }
        case 2:
        {
            int id;
            cout << "\nEnter Event ID to delete: ";
            cin >> id;
            scheduler.deleteEvent(id);
            break;
        }
        case 3:
        {
            string start, end;
            cout << "\nEnter Start Time (YYYY-MM-DD HH:MM): ";
            cin.ignore();
            getline(cin, start);
            cout << "Enter End Time (YYYY-MM-DD HH:MM): ";
            getline(cin, end);
            scheduler.findOverlappingEvents(start, end);
            break;
        }
        case 4:
        {
            string day;
            cout << "\nEnter Date (YYYY-MM-DD): ";
            cin.ignore();
            getline(cin, day);
            scheduler.calculateFreeTimeSlots(day);
            break;
        }
        case 5:
            scheduler.printSchedule();
            break;
        case 6:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}
