#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <limits>
#include <climits> // Added climits for INT_MAX constant compatibility across compilers
#include <algorithm>
#include <cmath>
#include <tuple> // Required for std::tuple

using namespace std;

// Color codes using ANSI escape sequences (basic and standard)
#define RESET   "\033[0m"       // Reset to default color
#define RED     "\033[31m"      // Red for errors
#define GREEN   "\033[32m"      // Green for success messages


#define YELLOW  "\033[33m"      // Yellow for warnings and important data
#define BLUE    "\033[34m"      // Blue for information
#define CYAN    "\033[36m"      // Cyan for headings and titles
#define WHITE   "\033[37m"      // White for regular text

// ============================================
// CONSTANTS
// ============================================
const int MAX_LOCATIONS = 50;
const int MAX_ROADS = 100;

// ============================================
// STRUCTURES
// ============================================

struct Location {
    int id;
    string name;
};

struct Road {
    int fromID;
    int toID;
    int distance;
    string status;
    bool isOneWay;      // true if road is one-way, false if bidirectional
    bool isAvailable;   // false if road is under construction or unavailable
};

struct Node {
    string path;
    Node* next;
};

struct GraphNode {
    int destination;
    int distance;
};

// ============================================
// GLOBAL VARIABLES
// ============================================

Location locations[MAX_LOCATIONS];
int locationCount = 0;

Road roads[MAX_ROADS];
int roadCount = 0;

Node* routeHistoryHead = nullptr;

// ============================================
// UTILITY FUNCTIONS
// ============================================

void displayHeading(string text) {
    cout << "\n";
    cout << CYAN << "+=================================================+\n";
    cout << "|  " << left << setw(45) << text << "|\n";
    cout << "+=================================================+" << RESET << "\n";
}

void displaySection(string text) {
    cout << "\n";
    cout << BLUE << "+--------------------------------------------------+\n";
    cout << "|  " << left << setw(45) << text << "|\n";
    cout << "+--------------------------------------------------+" << RESET << "\n";
}

void closeSectionBorder() {
    cout << BLUE << "+--------------------------------------------------+" << RESET << "\n";
}

void clearBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int findLocationIndexByID(int id) {
    for (int i = 0; i < locationCount; i++) {
        if (locations[i].id == id) {
            return i;
        }
    }
    return -1;
}

bool locationExists(int id) {
    return findLocationIndexByID(id) != -1;
}

string findLocationNameByID(int id) {
    for (int i = 0; i < locationCount; i++) {
        if (locations[i].id == id) {
            return locations[i].name;
        }
    }
    return "Unknown Location";
}

// ============================================
// LOCATION MANAGEMENT
// ============================================

void addLocation() {
    if (locationCount >= MAX_LOCATIONS) {
        cout << RED << "\n[ERROR] Maximum location limit reached!\n" << RESET;
        return;
    }

    int id;
    string name;

    cout << "\nEnter Location ID: ";
    if (!(cin >> id)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid ID input!\n" << RESET;
        return;
    }

    // Check if ID already exists
    for (int i = 0; i < locationCount; i++) {
        if (locations[i].id == id) {
            cout << RED << "[ERROR] Location ID already exists!\n" << RESET;
            return;
        }
    }

    clearBuffer();
    cout << "Enter Location Name: ";
    getline(cin, name);

    if (name.empty()) {
        cout << RED << "[ERROR] Location name cannot be empty!\n" << RESET;
        return;
    }

    locations[locationCount].id = id;
    locations[locationCount].name = name;
    locationCount++;

    cout << GREEN << "[SUCCESS] Location added successfully!\n" << RESET;
}

void viewLocations() {
    if (locationCount == 0) {
        cout << YELLOW << "\n[INFO] No locations added yet.\n" << RESET;
        return;
    }

    displaySection("ALL LOCATIONS");

    cout << "| " << left << setw(6) << "ID" << "| " << left << setw(35) << "Location Name" << "|\n";
    cout << "+-------+---------------------------------------+\n";

    for (int i = 0; i < locationCount; i++) {
        cout << "| " << YELLOW << left << setw(6) << locations[i].id << RESET
             << "| " << left << setw(35) << locations[i].name << "|\n";
    }
    closeSectionBorder();
}

void deleteLocation() {
    if (locationCount == 0) {
        cout << YELLOW << "\n[INFO] No locations available to delete.\n" << RESET;
        return;
    }

    viewLocations();

    int id;
    cout << "\nEnter Location ID to delete: ";
    if (!(cin >> id)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid ID input!\n" << RESET;
        return;
    }

    int foundIndex = -1;
    for (int i = 0; i < locationCount; i++) {
        if (locations[i].id == id) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        cout << RED << "[ERROR] Location not found!\n" << RESET;
        return;
    }

    // Shift elements left to delete
    for (int i = foundIndex; i < locationCount - 1; i++) {
        locations[i] = locations[i + 1];
    }
    locationCount--;

    cout << GREEN << "[SUCCESS] Location deleted successfully!\n" << RESET;
}

void sortLocationsByName() {
    if (locationCount == 0) {
        cout << YELLOW << "\n[INFO] No locations to sort.\n" << RESET;
        return;
    }

    for (int i = 0; i < locationCount - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < locationCount; j++) {
            if (locations[j].name < locations[minIndex].name) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            Location temp = locations[i];
            locations[i] = locations[minIndex];
            locations[minIndex] = temp;
        }
    }

    cout << GREEN << "\n[SUCCESS] Locations sorted alphabetically!\n" << RESET;
    viewLocations();
}

// ============================================
// ROAD MANAGEMENT
// ============================================

bool roadExists(int fromID, int toID) {
    for (int i = 0; i < roadCount; i++) {
        if (roads[i].fromID == fromID && roads[i].toID == toID) {
            return true;
        }
    }
    return false;
}

void addRoad() {
    if (roadCount >= MAX_ROADS) {
        cout << RED << "\n[ERROR] Maximum road limit reached!\n" << RESET;
        return;
    }

    if (locationCount < 2) {
        cout << YELLOW << "\n[INFO] Add at least 2 locations before adding roads.\n" << RESET;
        return;
    }

    int fromID, toID, distance;
    string status;
    bool isOneWay;
    bool isAvailable;

    viewLocations();

    cout << "\nEnter From Location ID: ";
    if (!(cin >> fromID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter To Location ID: ";
    if (!(cin >> toID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    if (fromID == toID) {
        cout << RED << "[ERROR] Cannot create a road from a location to itself!\n" << RESET;
        return;
    }

    if (!locationExists(fromID) || !locationExists(toID)) {
        cout << RED << "[ERROR] One or both locations do not exist!\n" << RESET;
        return;
    }

    if (roadExists(fromID, toID)) {
        cout << RED << "[ERROR] Road from " << fromID << " to " << toID << " already exists!\n" << RESET;
        return;
    }

    cout << "Enter Distance (km): ";
    if (!(cin >> distance) || distance <= 0) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid distance!\n" << RESET;
        return;
    }

    clearBuffer();
    cout << "Enter Road Status (Normal/Heavy Traffic/Blocked): ";
    getline(cin, status);

    if (status != "Normal" && status != "Heavy Traffic" && status != "Blocked") {
        cout << RED << "[ERROR] Invalid status! Use 'Normal', 'Heavy Traffic', or 'Blocked'.\n" << RESET;
        return;
    }

    cout << "Is this a one-way road? (1 = Yes, 0 = No): ";
    int oneWayChoice;
    if (!(cin >> oneWayChoice) || (oneWayChoice != 0 && oneWayChoice != 1)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input! Using bidirectional (No) by default.\n" << RESET;
        isOneWay = false;
    } else {
        isOneWay = (oneWayChoice == 1);
    }

    cout << "Is this road available (not under construction)? (1 = Yes, 0 = No): ";
    int availableChoice;
    if (!(cin >> availableChoice) || (availableChoice != 0 && availableChoice != 1)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input! Using available (Yes) by default.\n" << RESET;
        isAvailable = true;
    } else {
        isAvailable = (availableChoice == 1);
    }

    roads[roadCount].fromID = fromID;
    roads[roadCount].toID = toID;
    roads[roadCount].distance = distance;
    roads[roadCount].status = status;
    roads[roadCount].isOneWay = isOneWay;
    roads[roadCount].isAvailable = isAvailable;
    roadCount++;

    if (!isOneWay) {
        if (roadCount < MAX_ROADS && !roadExists(toID, fromID)) {
            roads[roadCount].fromID = toID;
            roads[roadCount].toID = fromID;
            roads[roadCount].distance = distance;
            roads[roadCount].status = status;
            roads[roadCount].isOneWay = false;
            roads[roadCount].isAvailable = isAvailable;
            roadCount++;
            cout << GREEN << "[SUCCESS] Bidirectional road added successfully!\n" << RESET;
        } else {
            cout << YELLOW << "[WARNING] Forward road added, but couldn't add reverse direction.\n" << RESET;
        }
    } else {
        cout << GREEN << "[SUCCESS] One-way road added successfully!\n" << RESET;
    }
}

void viewRoads() {
    if (roadCount == 0) {
        cout << YELLOW << "\n[INFO] No roads added yet.\n" << RESET;
        return;
    }

    displaySection("ALL ROADS");

    cout << "| " << left << setw(6) << "From" << "| " << left << setw(6) << "To"
         << "| " << left << setw(10) << "Distance" << "| " << left << setw(17) << "Status"
         << "| " << left << setw(10) << "Direction" << "| " << left << setw(12) << "Available" << "|\n";
    cout << "+-------+-------+------------+-------------------+------------+--------------+\n";

    for (int i = 0; i < roadCount; i++) {
        string statusColor = RESET;
        if (roads[i].status == "Normal") statusColor = GREEN;
        else if (roads[i].status == "Heavy Traffic") statusColor = YELLOW;
        else if (roads[i].status == "Blocked") statusColor = RED;

        string direction = roads[i].isOneWay ? "One-Way" : "Both";
        string available = roads[i].isAvailable ? "Yes" : "No";
        string availColor = roads[i].isAvailable ? GREEN : RED;

        cout << "| " << left << setw(6) << roads[i].fromID
             << "| " << left << setw(6) << roads[i].toID
             << "| " << YELLOW << left << setw(10) << (to_string(roads[i].distance) + " km") << RESET
             << "| " << statusColor << left << setw(17) << roads[i].status << RESET
             << "| " << CYAN << left << setw(10) << direction << RESET
             << "| " << availColor << left << setw(12) << available << RESET << "|\n";
    }
    closeSectionBorder();
}

void deleteRoad() {
    if (roadCount == 0) {
        cout << YELLOW << "\n[INFO] No roads available to delete.\n" << RESET;
        return;
    }

    viewRoads();

    int fromID, toID;
    cout << "\nEnter From Location ID: ";
    if (!(cin >> fromID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter To Location ID: ";
    if (!(cin >> toID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    int foundIndex = -1;
    for (int i = 0; i < roadCount; i++) {
        if (roads[i].fromID == fromID && roads[i].toID == toID) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        cout << RED << "[ERROR] Road not found!\n" << RESET;
        return;
    }

    bool wasOneWay = roads[foundIndex].isOneWay;

    // Shift elements left to delete
    for (int i = foundIndex; i < roadCount - 1; i++) {
        roads[i] = roads[i + 1];
    }
    roadCount--;

    // If bidirectional, also delete reverse direction
    if (!wasOneWay) {
        for (int i = 0; i < roadCount; i++) {
            if (roads[i].fromID == toID && roads[i].toID == fromID) {
                for (int j = i; j < roadCount - 1; j++) {
                    roads[j] = roads[j + 1];
                }
                roadCount--;
                break;
            }
        }
    }

    cout << GREEN << "[SUCCESS] Road deleted successfully!\n" << RESET;
}

void updateRoadDistance() {
    if (roadCount == 0) {
        cout << YELLOW << "\n[INFO] No roads available.\n" << RESET;
        return;
    }

    viewRoads();

    int fromID, toID, newDistance;
    cout << "\nEnter From Location ID: ";
    if (!(cin >> fromID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter To Location ID: ";
    if (!(cin >> toID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter new distance (km): ";
    if (!(cin >> newDistance) || newDistance <= 0) {
        clearBuffer();
        cout << RED << "[ERROR] Distance must be positive!\n" << RESET;
        return;
    }

    bool found = false;
    bool wasOneWay = false;
    for (int i = 0; i < roadCount; i++) {
        if (roads[i].fromID == fromID && roads[i].toID == toID) {
            roads[i].distance = newDistance;
            wasOneWay = roads[i].isOneWay;
            found = true;

            if (!wasOneWay) {
                for (int j = 0; j < roadCount; j++) {
                    if (roads[j].fromID == toID && roads[j].toID == fromID) {
                        roads[j].distance = newDistance;
                        break;
                    }
                }
            }
            break;
        }
    }

    if (found) {
        cout << GREEN << "[SUCCESS] Distance updated!\n" << RESET;
    } else {
        cout << RED << "[ERROR] Road not found!\n" << RESET;
    }
}

void updateRoadStatus() {
    if (roadCount == 0) {
        cout << YELLOW << "\n[INFO] No roads available.\n" << RESET;
        return;
    }

    viewRoads();

    int fromID, toID;
    string newStatus;

    cout << "\nEnter From Location ID: ";
    if (!(cin >> fromID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter To Location ID: ";
    if (!(cin >> toID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    clearBuffer();
    cout << "Enter New Status (Normal/Heavy Traffic/Blocked): ";
    getline(cin, newStatus);

    if (newStatus != "Normal" && newStatus != "Heavy Traffic" && newStatus != "Blocked") {
        cout << RED << "[ERROR] Invalid status!\n" << RESET;
        return;
    }

    bool found = false;
    bool wasOneWay = false;
    for (int i = 0; i < roadCount; i++) {
        if (roads[i].fromID == fromID && roads[i].toID == toID) {
            roads[i].status = newStatus;
            wasOneWay = roads[i].isOneWay;
            found = true;

            if (!wasOneWay) {
                for (int j = 0; j < roadCount; j++) {
                    if (roads[j].fromID == toID && roads[j].toID == fromID) {
                        roads[j].status = newStatus;
                        break;
                    }
                }
            }
            break;
        }
    }

    if (found) {
        cout << GREEN << "[SUCCESS] Road status updated!\n" << RESET;
    } else {
        cout << RED << "[ERROR] Road not found!\n" << RESET;
    }
}

void toggleRoadAvailability() {
    if (roadCount == 0) {
        cout << YELLOW << "\n[INFO] No roads available.\n" << RESET;
        return;
    }

    viewRoads();

    int fromID, toID;
    cout << "\nEnter From Location ID: ";
    if (!(cin >> fromID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter To Location ID: ";
    if (!(cin >> toID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    bool found = false;
    bool wasOneWay = false;
    for (int i = 0; i < roadCount; i++) {
        if (roads[i].fromID == fromID && roads[i].toID == toID) {
            roads[i].isAvailable = !roads[i].isAvailable;
            wasOneWay = roads[i].isOneWay;
            found = true;

            string status = roads[i].isAvailable ? "available" : "unavailable (under construction)";

            // Update reverse if bidirectional
            if (!wasOneWay) {
                for (int j = 0; j < roadCount; j++) {
                    if (roads[j].fromID == toID && roads[j].toID == fromID) {
                        roads[j].isAvailable = roads[i].isAvailable;
                        break;
                    }
                }
            }

            cout << GREEN << "[SUCCESS] Road is now " << status << "!\n" << RESET;
            break;
        }
    }

    if (!found) {
        cout << RED << "[ERROR] Road not found!\n" << RESET;
    }
}

// ============================================
// MANUAL ROUTE BUILDER (STACK)
// ============================================

void manualRouteBuilder() {
    stack<int> routeStack;

    cout << "\n--- Manual Route Builder (using Stack) ---\n";
    cout << "You can push locations to create your custom path.\n";

    while (true) {
        cout << "\nCurrent Path: ";
        if (routeStack.empty()) {
            cout << "(empty)";
        } else {
            // Collect all locations in a vector to reverse the order
            stack<int> temp = routeStack;
            vector<int> pathVector;
            while (!temp.empty()) {
                pathVector.push_back(temp.top());
                temp.pop();
            }
            // Display from bottom of stack to top (travel direction)
            for (int i = pathVector.size() - 1; i >= 0; i--) {
                if (i < pathVector.size() - 1) cout << " -> ";
                cout << pathVector[i];
            }
        }
        cout << "\n\n1. Push Location\n";
        cout << "2. Pop Location\n";
        cout << "3. View Current Path\n";
        cout << "4. Save Path to History\n";
        cout << "5. Exit Manual Builder\n";
        cout << "Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            clearBuffer();
            cout << RED << "[ERROR] Invalid input!\n" << RESET;
            continue;
        }

        if (choice == 1) {
            cout << "Enter Location ID to push: ";
            int id;
            if (!(cin >> id)) {
                clearBuffer();
                cout << RED << "[ERROR] Invalid input!\n" << RESET;
                continue;
            }

            if (!locationExists(id)) {
                cout << RED << "[ERROR] Location not found!\n" << RESET;
                continue;
            }

            // Check if road exists from current top location to new location
            if (!routeStack.empty()) {
                int currentTop = routeStack.top();
                if (!roadExists(currentTop, id)) {
                    string fromName = findLocationNameByID(currentTop);
                    string toName = findLocationNameByID(id);
                    cout << RED << "[ERROR] No road exists from " << fromName
                         << " (ID: " << currentTop << ") to " << toName
                         << " (ID: " << id << ")!\n" << RESET;
                    cout << YELLOW << "[INFO] Please add a road between these locations first using 'Manage Roads'.\n" << RESET;
                    continue;
                }
            }

            routeStack.push(id);
            cout << GREEN << "[SUCCESS] Location " << id << " pushed!\n" << RESET;
        } else if (choice == 2) {
            if (!routeStack.empty()) {
                cout << GREEN << "[SUCCESS] Popped: " << routeStack.top() << "\n" << RESET;
                routeStack.pop();
            } else {
                cout << YELLOW << "[INFO] Stack is empty!\n" << RESET;
            }
        } else if (choice == 3) {
            cout << "\n--- Current Stack (Top to Bottom) ---\n";
            if (routeStack.empty()) {
                cout << "(empty)\n";
            } else {
                stack<int> temp = routeStack;
                int count = 1;
                while (!temp.empty()) {
                    cout << count << ". Location ID: " << temp.top() << "\n";
                    temp.pop();
                    count++;
                }
            }
        } else if (choice == 4) {
            if (routeStack.empty()) {
                cout << RED << "[ERROR] Path is empty!\n" << RESET;
            } else {
                stack<int> temp = routeStack;
                vector<int> pathVector;
                while (!temp.empty()) {
                    pathVector.push_back(temp.top());
                    temp.pop();
                }

                string pathString = "";
                for (int i = pathVector.size() - 1; i >= 0; i--) {
                    if (i < pathVector.size() - 1) pathString += " -> ";
                    pathString += findLocationNameByID(pathVector[i]);
                }
                pathString += " (end)";

                Node* newNode = new Node();
                newNode->path = pathString;
                newNode->next = routeHistoryHead;
                routeHistoryHead = newNode;

                cout << GREEN << "[SUCCESS] Path saved to history!\n" << RESET;
                cout << "Path: " << YELLOW << pathString << RESET << "\n";
            }
        } else if (choice == 5) {
            break;
        } else {
            cout << RED << "[ERROR] Invalid choice!\n" << RESET;
        }
    }
}

// ============================================
// ROUTE HISTORY (LINKED LIST)
// ============================================

void viewRouteHistory() {
    if (routeHistoryHead == nullptr) {
        cout << YELLOW << "\n[INFO] No route history available.\n" << RESET;
        return;
    }

    displaySection("ROUTE HISTORY");
    Node* current = routeHistoryHead;
    int count = 1;

    while (current != nullptr) {
        cout << count << ". " << YELLOW << current->path << RESET << "\n";
        current = current->next;
        count++;
    }
    closeSectionBorder();
}

void clearRouteHistory() {
    while (routeHistoryHead != nullptr) {
        Node* temp = routeHistoryHead;
        routeHistoryHead = routeHistoryHead->next;
        delete temp;
    }
    cout << GREEN << "\n[SUCCESS] Route history cleared!\n" << RESET;
}

// ============================================
// DIJKSTRA'S SHORTEST PATH
// ============================================

void shortestPathDijkstra() {
    if (locationCount < 2) {
        cout << YELLOW << "\n[INFO] Add at least 2 locations first.\n" << RESET;
        return;
    }

    viewLocations();

    int startID, endID;
    cout << "\nEnter Start Location ID: ";
    if (!(cin >> startID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter End Location ID: ";
    if (!(cin >> endID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    if (!locationExists(startID) || !locationExists(endID)) {
        cout << RED << "[ERROR] Invalid location IDs!\n" << RESET;
        return;
    }

    if (startID == endID) {
        cout << YELLOW << "[INFO] Start and end locations are the same!\n" << RESET;
        return;
    }

    vector<GraphNode> adjacencyList[500];

    for (int i = 0; i < roadCount; i++) {
        // Skip blocked roads and unavailable roads
        if (roads[i].status == "Blocked" || !roads[i].isAvailable) {
            continue;
        }

        GraphNode gn;
        gn.destination = roads[i].toID;

        // Apply traffic multiplier: Normal = 1.0x, Heavy Traffic = 1.5x
        double multiplier = 1.0;
        if (roads[i].status == "Heavy Traffic") {
            multiplier = 1.5;
        }
        gn.distance = (int)(roads[i].distance * multiplier);

        adjacencyList[roads[i].fromID].push_back(gn);
    }

    const int INF = INT_MAX;
    int dist[500];
    int parent[500];
    bool visited[500];

    for (int i = 0; i < 500; i++) {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = false;
    }

    dist[startID] = 0;

    for (int count = 0; count < locationCount; count++) {
        int u = -1;
        int minDist = INF;

        for (int i = 0; i < 500; i++) {
            if (!visited[i] && dist[i] < minDist) {
                u = i;
                minDist = dist[i];
            }
        }

        if (u == -1 || dist[u] == INF) break;

        visited[u] = true;

        for (size_t j = 0; j < adjacencyList[u].size(); j++) {
            int v = adjacencyList[u][j].destination;
            int weight = adjacencyList[u][j].distance;

            if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
        }
    }

    if (dist[endID] == INF) {
        cout << RED << "\n[ERROR] No path exists between the locations!\n" << RESET;
        cout << YELLOW << "[INFO] This may be due to blocked or unavailable roads.\n" << RESET;
        return;
    }

    vector<int> path;
    int current = endID;
    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());

    displaySection("SHORTEST PATH FOUND");

    cout << CYAN << "Path: " << RESET;
    for (size_t i = 0; i < path.size(); i++) {
        int locationIndex = findLocationIndexByID(path[i]);
        if (locationIndex != -1) {
            cout << YELLOW << locations[locationIndex].name << RESET;
        } else {
            cout << path[i];
        }
        if (i < path.size() - 1) cout << " -> ";
    }
    cout << "\n";

    cout << GREEN << "Total Weighted Distance: " << dist[endID] << " km" << RESET;
    cout << YELLOW << " (includes traffic delays)\n" << RESET;

    double fuel = dist[endID] * 0.12;
    double time = (double)dist[endID] / 40.0;
    int hours = (int)time;
    int minutes = (int)((time - hours) * 60);
    double fuelCost = fuel * 1.5;

    displaySection("COST ESTIMATION");
    cout << CYAN << "Route: " << RESET << findLocationNameByID(startID) << " -> " << findLocationNameByID(endID) << "\n";
    cout << "+-------------------------------------------------+\n";
    cout << "| " << left << setw(35) << "Detail" << "| " << right << setw(10) << "Value" << "|\n";
    cout << "+-------------------------------------------------+\n";
    cout << "| " << left << setw(35) << "Total Distance (weighted)"
         << "| " << YELLOW << right << setw(10) << (to_string(dist[endID]) + " km") << RESET << "|\n";
    cout << "| " << left << setw(35) << "Fuel Required"
         << "| " << GREEN << right << setw(10) << fixed << setprecision(2) << fuel << " L" << RESET << "|\n";
    cout << "| " << left << setw(35) << "Estimated Time"
         << "| " << CYAN << right << setw(10) << hours << "h " << minutes << "m" << RESET << "|\n";
    cout << "| " << left << setw(35) << "Fuel Cost ($1.50/L)"
         << "| " << YELLOW << right << setw(10) << "$" << fixed << setprecision(2) << fuelCost << RESET << "|\n";
    cout << "+-------------------------------------------------+\n";

    string pathStr = "";
    for (size_t i = 0; i < path.size(); i++) {
        int locationIndex = findLocationIndexByID(path[i]);
        if (locationIndex != -1) {
            pathStr += locations[locationIndex].name;
        } else {
            pathStr += to_string(path[i]);
        }
        if (i < path.size() - 1) pathStr += " -> ";
    }
    pathStr += " (Distance: " + to_string(dist[endID]) + " km)";

    Node* newNode = new Node();
    newNode->path = pathStr;
    newNode->next = routeHistoryHead;
    routeHistoryHead = newNode;

    cout << GREEN << "\n[SUCCESS] Path saved to history!\n" << RESET;
}

// ============================================
// ASCII MAP VIEW
// ============================================

void displayASCIIMap() {
    if (locationCount == 0) {
        cout << YELLOW << "\n[INFO] Add locations first to see the map.\n" << RESET;
        return;
    }

    displaySection("ASCII MAP VIEW");
    cout << "\nLocations and Road Network:\n\n";

    for (int i = 0; i < locationCount; i++) {
        cout << "[" << YELLOW << locations[i].id << RESET << "] " << locations[i].name;
        cout << "\n";

        int hasRoads = 0;
        for (int j = 0; j < roadCount; j++) {
            if (roads[j].fromID == locations[i].id) {
                // Find destination location name
                string destName = "Unknown";
                for(int k=0; k<locationCount; ++k) {
                    if(roads[j].toID == locations[k].id) {
                        destName = locations[k].name;
                        break;
                    }
                }

                // Determine color for status
                string statusColor = RESET;
                if (roads[j].status == "Normal") statusColor = GREEN;
                else if (roads[j].status == "Heavy Traffic") statusColor = YELLOW;
                else if (roads[j].status == "Blocked") statusColor = RED;

                // Determine color for availability
                string availColor = roads[j].isAvailable ? RESET : RED;
                string availableStatus = roads[j].isAvailable ? "Available" : "Unavailable";

                cout << "    |--(" << YELLOW << roads[j].distance << "km" << RESET
                     << ", " << statusColor << roads[j].status << RESET << ", "
                     << availColor << availableStatus << RESET << ")--";

                // Indicate one-way road
                if (roads[j].isOneWay) {
                    cout << " (->)";
                }

                cout << "[" << YELLOW << roads[j].toID << RESET << "] " << destName << "\n";
                hasRoads++;
            }
        }

        if (hasRoads == 0) {
            cout << "    (no outgoing roads)\n";
        }
    }

    cout << "\n";
}

// ============================================
// SYSTEM STATISTICS
// ============================================

void displaySystemStatistics() {
    displaySection("SYSTEM STATISTICS");

    cout << CYAN << "Total Locations: " << RESET << locationCount << "/" << MAX_LOCATIONS << "\n";
    cout << CYAN << "Total Roads: " << RESET << roadCount << "/" << MAX_ROADS << "\n";

    if (roadCount > 0) {
        int normalCount = 0, heavyCount = 0, blockedCount = 0;
        int availableCount = 0, unavailableCount = 0;
        int oneWayCount = 0, bidirectionalCount = 0;

        for (int i = 0; i < roadCount; i++) {
            if (roads[i].status == "Normal") normalCount++;
            else if (roads[i].status == "Heavy Traffic") heavyCount++;
            else if (roads[i].status == "Blocked") blockedCount++;

            if (roads[i].isAvailable) availableCount++;
            else unavailableCount++;

            if (roads[i].isOneWay) oneWayCount++;
            else bidirectionalCount++;
        }

        cout << "\nRoad Status Breakdown:\n";
        cout << "  " << GREEN << "Normal Roads: " << normalCount << RESET << "\n";
        cout << "  " << YELLOW << "Heavy Traffic: " << heavyCount << RESET << "\n";
        cout << "  " << RED << "Blocked Roads: " << blockedCount << RESET << "\n";

        cout << "\nRoad Availability:\n";
        cout << "  " << GREEN << "Available Roads: " << availableCount << RESET << "\n";
        cout << "  " << RED << "Unavailable Roads: " << unavailableCount << RESET << "\n";

        cout << "\nRoad Direction:\n";
        cout << "  " << CYAN << "One-Way Roads: " << oneWayCount << RESET << "\n";
        cout << "  " << BLUE << "Bidirectional Roads: " << (bidirectionalCount / 2) << RESET << "\n";

        int totalDistance = 0;
        for (int i = 0; i < roadCount; i++) {
            totalDistance += roads[i].distance;
        }
        double avgDistance = (double)totalDistance / roadCount;
        cout << fixed << setprecision(2);
        cout << "\n" << CYAN << "Average Road Distance: " << RESET << avgDistance << " km\n";
    }

    // Count routes in history
    int routeCount = 0;
    Node* current = routeHistoryHead;
    while (current != nullptr) {
        routeCount++;
        current = current->next;
    }
    cout << CYAN << "Paths in History: " << RESET << routeCount << "\n";
}

// ============================================
// TRAFFIC SIMULATION
// ============================================

void trafficSimulation() {
    if (roadCount == 0) {
        cout << YELLOW << "\n[INFO] No roads available for traffic simulation.\n" << RESET;
        return;
    }

    displaySection("TRAFFIC SIMULATION");
    cout << "1. View Current Traffic Status\n";
    cout << "2. Update Road Status\n";
    // Added new menu option
    cout << "3. Toggle Road Availability\n";
    cout << "4. Back to Main Menu\n";
    cout << "Enter choice: ";

    int choice;
    if (!(cin >> choice)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    if (choice == 1) {
        viewRoads();
    } else if (choice == 2) {
        updateRoadStatus();
    } else if (choice == 3) {
        toggleRoadAvailability();
    } else if (choice == 4) {
        // Back to Main Menu is handled by the caller implicitly
    } else {
        cout << RED << "[ERROR] Invalid choice!\n" << RESET;
    }
}

// ============================================
// PATH COST ESTIMATOR
// ============================================

void pathCostEstimator() {
    if (locationCount < 2) {
        cout << YELLOW << "\n[INFO] Add at least 2 locations first.\n" << RESET;
        return;
    }

    viewLocations();

    int startID, endID;
    cout << "\nEnter Start Location ID: ";
    if (!(cin >> startID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    cout << "Enter End Location ID: ";
    if (!(cin >> endID)) {
        clearBuffer();
        cout << RED << "[ERROR] Invalid input!\n" << RESET;
        return;
    }

    if (!locationExists(startID) || !locationExists(endID)) {
        cout << RED << "[ERROR] Invalid location IDs!\n" << RESET;
        return;
    }

    if (startID == endID) {
        cout << YELLOW << "[INFO] Start and end locations are the same! No travel needed.\n" << RESET;
        return;
    }

    vector<GraphNode> adjacencyList[500];
    for (int i = 0; i < roadCount; i++) {
        if (roads[i].status != "Blocked" && roads[i].isAvailable) {
            GraphNode gn;
            gn.destination = roads[i].toID;

            double multiplier = 1.0;
            if (roads[i].status == "Heavy Traffic") {
                multiplier = 1.5;
            }
            gn.distance = (int)(roads[i].distance * multiplier);

            adjacencyList[roads[i].fromID].push_back(gn);
        }
    }

    const int INF = INT_MAX;
    int dist[500];
    int parent[500];
    bool visited[500];

    for (int i = 0; i < 500; i++) {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = false;
    }

    dist[startID] = 0;

    for (int count = 0; count < locationCount; count++) {
        int u = -1;
        int minDist = INF;

        for (int i = 0; i < 500; i++) {
            if (!visited[i] && dist[i] < minDist) {
                u = i;
                minDist = dist[i];
            }
        }

        if (u == -1 || dist[u] == INF) break;

        visited[u] = true;

        for (size_t j = 0; j < adjacencyList[u].size(); j++) {
            int v = adjacencyList[u][j].destination;
            int weight = adjacencyList[u][j].distance;

            if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
        }
    }

    if (dist[endID] == INF) {
        cout << RED << "\n[ERROR] No path exists between the locations!\n" << RESET;
        return;
    }

    int totalDistance = dist[endID];

    displaySection("PATH COST ESTIMATE");

    string startLocationName = findLocationNameByID(startID);
    string endLocationName = findLocationNameByID(endID);

    cout << CYAN << "Route: " << RESET << startLocationName << " -> " << endLocationName << "\n";
    cout << "+-------------------------------------------------+\n";
    cout << "| " << left << setw(35) << "Detail" << "| " << right << setw(10) << "Value" << "|\n";
    cout << "+-------------------------------------------------+\n";
    cout << "| " << left << setw(35) << "Total Distance (weighted)"
         << "| " << YELLOW << right << setw(10) << (to_string(totalDistance) + " km") << RESET << "|\n";

    double fuelEstimate = totalDistance * 0.12;
    cout << "| " << left << setw(35) << "Fuel Required"
         << "| " << GREEN << right << setw(10) << fixed << setprecision(2) << fuelEstimate << " L" << RESET << "|\n";

    double timeEstimate = (double)totalDistance / 40.0;
    int hours = (int)timeEstimate;
    int minutes = (int)((timeEstimate - hours) * 60);
    cout << "| " << left << setw(35) << "Estimated Time"
         << "| " << CYAN << right << setw(10) << hours << "h " << minutes << "m" << RESET << "|\n";

    double fuelCost = fuelEstimate * 1.50;
    cout << "| " << left << setw(35) << "Fuel Cost ($1.50/L)"
         << "| " << YELLOW << right << setw(10) << "$" << fixed << setprecision(2) << fuelCost << RESET << "|\n";

    cout << "+-------------------------------------------------+\n";
}

// ============================================
// LOCATION MANAGEMENT MENU
// ============================================

void locationManagementMenu() {
    while (true) {
        displaySection("LOCATION MANAGEMENT");
        cout << "1. Add Location\n";
        cout << "2. View All Locations\n";
        cout << "3. Delete Location\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            clearBuffer();
            cout << RED << "[ERROR] Invalid input!\n" << RESET;
            continue;
        }

        if (choice == 1) {
            addLocation();
        } else if (choice == 2) {
            viewLocations();
        } else if (choice == 3) {
            deleteLocation();
        } else if (choice == 4) {
            break;
        } else {
            cout << RED << "[ERROR] Invalid choice!\n" << RESET;
        }
    }
}

// ============================================
// ROAD MANAGEMENT MENU
// ============================================

void roadManagementMenu() {
    while (true) {
        displaySection("ROAD MANAGEMENT");
        cout << "1. Add Road\n";
        cout << "2. View All Roads\n";
        cout << "3. Update Road Distance\n";
        cout << "4. Update Road Status\n";
        cout << "5. Delete Road\n";
        cout << "6. Toggle Road Availability\n";
        cout << "7. Back to Main Menu\n";
        cout << "Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            clearBuffer();
            cout << RED << "[ERROR] Invalid input!\n" << RESET;
            continue;
        }

        if (choice == 1) {
            addRoad();
        } else if (choice == 2) {
            viewRoads();
        } else if (choice == 3) {
            updateRoadDistance();
        } else if (choice == 4) {
            updateRoadStatus();
        } else if (choice == 5) {
            deleteRoad();
        } else if (choice == 6) {
            toggleRoadAvailability();
        } else if (choice == 7) {
            break;
        } else {
            cout << RED << "[ERROR] Invalid choice!\n" << RESET;
        }
    }
}

// ============================================
// MAIN MENU
// ============================================

void mainMenu() {
    while (true) {
        displayHeading("SMART NAVIGATION SYSTEM");

        cout << "\n1. Manage Locations\n";
        cout << "2. Manage Roads\n";
        cout << "3. Traffic Simulation\n";
        cout << "4. Manual Route Builder (Stack)\n";
        cout << "5. Shortest Path (Dijkstra)\n";
        cout << "6. Route History (Linked List)\n";
        cout << "7. Path Cost Estimator (Fuel + Time)\n";
        cout << "8. Sort Locations by Name\n";
        cout << "9. ASCII Map View\n";
        cout << "10. System Statistics\n";
        cout << "0. Exit\n";
        cout << "-------------------------------------------\n";
        cout << "Enter your choice: ";

        int choice;
        if (!(cin >> choice)) {
            clearBuffer();
            cout << RED << "\n[ERROR] Invalid input! Please try again.\n" << RESET;
            continue;
        }

        if (choice == 1) {
            locationManagementMenu();
        } else if (choice == 2) {
            roadManagementMenu();
        } else if (choice == 3) {
            trafficSimulation();
        } else if (choice == 4) {
            manualRouteBuilder();
        } else if (choice == 5) {
            shortestPathDijkstra();
        } else if (choice == 6) {
            displaySection("ROUTE HISTORY");
            cout << "1. View History\n";
            cout << "2. Clear History\n";
            cout << "3. Back\n";
            cout << "Enter choice: ";
            int histChoice;
            if (!(cin >> histChoice)) {
                clearBuffer();
                cout << RED << "[ERROR] Invalid input!\n" << RESET;
                continue;
            }
            if (histChoice == 1) viewRouteHistory();
            else if (histChoice == 2) clearRouteHistory();
            else if (histChoice == 3) {} // Do nothing, return to main menu
            else { cout << RED << "[ERROR] Invalid choice!\n" << RESET; }
        } else if (choice == 7) {
            pathCostEstimator();
        } else if (choice == 8) {
            sortLocationsByName();
        } else if (choice == 9) {
            displayASCIIMap();
        } else if (choice == 10) {
            displaySystemStatistics();
        } else if (choice == 0) {
            cout << "\n";
            displayHeading("THANK YOU FOR USING SMART NAVIGATION!");
            cout << GREEN << "Exiting...\n\n" << RESET;
            break;
        } else {
            cout << RED << "\n[ERROR] Invalid choice! Please try again.\n" << RESET;
        }
    }
}

// ============================================
// PRELOADED DATA INITIALIZATION
// ============================================

void initializePreloadedData() {
    locations[0] = {1, "DHA"};
    locations[1] = {2, "Sadar Bazaar"};
    locations[2] = {3, "Anarkali"};
    locations[3] = {4, "Model Town"};
    locations[4] = {5, "Gulberg"};
    locations[5] = {6, "Defence Road"};
    locations[6] = {7, "Canal Road"};
    locations[7] = {8, "Mall Road"};
    locations[8] = {9, "Johar Town"};
    locations[9] = {10, "Bahria Town"};
    locationCount = 10;

    vector<tuple<int, int, int, string, bool, bool>> initialRoads = {
        {1, 2, 8, "Normal", false, true}, {2, 3, 6, "Normal", false, true},
        {3, 4, 12, "Heavy Traffic", false, true}, {4, 5, 5, "Normal", false, true},
        {5, 6, 9, "Normal", false, true}, {6, 7, 7, "Heavy Traffic", false, true},
        {7, 8, 4, "Normal", false, true}, {8, 9, 10, "Blocked", false, true},
        {9, 10, 14, "Normal", false, true}, {10, 1, 18, "Heavy Traffic", false, true},
        {1, 3, 14, "Normal", false, true}, {2, 4, 15, "Normal", false, true},
        {3, 5, 11, "Normal", false, true}, {4, 6, 8, "Heavy Traffic", false, true},
        {5, 7, 13, "Normal", false, true}, {6, 8, 9, "Normal", false, true},
        {7, 9, 12, "Normal", false, true}, {8, 10, 16, "Heavy Traffic", false, true},
        {2, 6, 18, "Normal", false, true}, {9, 1, 20, "Normal", false, true}
    };

    for(const auto& road_data : initialRoads) {
        int from = get<0>(road_data);
        int to = get<1>(road_data);
        int dist = get<2>(road_data);
        string status = get<3>(road_data);
        bool isOneWay = get<4>(road_data);
        bool isAvailable = get<5>(road_data);

        if (roadCount < MAX_ROADS) {
            roads[roadCount].fromID = from;
            roads[roadCount].toID = to;
            roads[roadCount].distance = dist;
            roads[roadCount].status = status;
            roads[roadCount].isOneWay = isOneWay;
            roads[roadCount].isAvailable = isAvailable;
            roadCount++;
        }
        // For preloaded data, only add the reverse direction if it's not one-way and if there's space.
        // The `roadExists` check is important here to avoid duplicates if the `initialRoads` list were to contain redundant bidirectional entries.
        if (!isOneWay && roadCount < MAX_ROADS && !roadExists(to, from)) {
            roads[roadCount].fromID = to;
            roads[roadCount].toID = from;
            roads[roadCount].distance = dist;
            roads[roadCount].status = status;
            roads[roadCount].isOneWay = false; // Explicitly false for the reverse road
            roads[roadCount].isAvailable = isAvailable;
            roadCount++;
        }
    }
}

// ============================================
// MAIN FUNCTION
// ============================================

int main() {
    initializePreloadedData();

    mainMenu();
    return 0;
}

