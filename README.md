# Smart Navigation System

A console-based intelligent navigation system that demonstrates advanced Data Structures and Algorithms (DSA) concepts. The system helps users find optimal routes, simulate traffic conditions, and manage road networks in real-time.

---

## Project Information

**Course:** Data Structures and Algorithms Lab  
**Institution:** COMSATS University Islamabad 
**Semester:** 3rd Semester  

### Group Members

| Name | Roll Number |
|------|------------|
| Zain Aftab | FA24-BSE-155 |
| Hunain Riasat | FA24-BSE-083 |
| Shaheer Aftab | FA24-BSE-089 |
| Ahsan Ahmed | FA24-BSE-074 |

---

## Features

### Core Navigation Features
- **Shortest Path Finding** - Dijkstra's algorithm to find optimal routes between locations
- **Manual Route Builder** - Build custom routes step-by-step using Stack data structure
- **Route History** - Track all searched paths using Singly Linked List
- **Path Cost Estimator** - Calculate fuel consumption and travel time for any route

### Location & Road Management
- **Location Management** - Add, view, delete, and search locations
- **Road Management** - Create roads, update distances, and manage one-way/bidirectional roads
- **Traffic Simulation** - Real-time traffic status updates (Normal/Heavy Traffic/Blocked)
- **Road Availability** - Mark roads under construction or reopen them

### Advanced Features
- **Traffic-Weighted Routing** - Dijkstra considers traffic conditions (1.5x multiplier for heavy traffic)
- **Automatic Rerouting** - System avoids blocked/unavailable roads
- **ASCII Map View** - Visual representation of the road network
- **System Statistics** - Monitor total locations, roads, and traffic status
- **Location Search** - Linear search to find locations by name
- **Sorting** - Selection sort to organize locations alphabetically

---

## Data Structures Used

### 1. **Array**
- Locations array (max 50)
- Roads array (max 100)
- Dijkstra's distance, visited, parent arrays

### 2. **Struct**
- `Location` - Stores location ID and name
- `Road` - Stores road details (from, to, distance, status, direction, availability)
- `GraphNode` - Stores neighbor information for adjacency list

### 3. **Vector**
- Adjacency list for graph representation
- Route path reconstruction
- Manual route builder (stack implementation)

### 4. **Stack (LIFO)**
- Manual route building - push/pop locations
- Undo functionality for route modification

### 5. **Singly Linked List**
- Route history storage
- Dynamic memory allocation for path records

### 6. **Graph**
- Adjacency list representation
- Bidirectional roads
- Weighted edges (distances)

---

## Algorithms Implemented

### 1. **Dijkstra's Shortest Path Algorithm**
- **Time Complexity:** O(V²) where V = number of vertices
- **Space Complexity:** O(V)
- Finds shortest path considering:
  - Normal roads (distance as-is)
  - Heavy traffic roads (1.5x distance multiplier)
  - Blocked roads (weight = infinity, excluded)
  - Unavailable roads (under construction, excluded)

### 2. **Linear Search**
- **Time Complexity:** O(n)
- Used for location ID validation and search

### 3. **Selection Sort**
- **Time Complexity:** O(n²)
- Sorts locations alphabetically by name

### 4. **Depth-First Traversal**
- Used in adjacency list traversal during Dijkstra

---

## Installation & Compilation

### Prerequisites
- **C++ Compiler:** GCC (g++), Clang, or MSVC
- **Standard:** C++11 or later
- **OS:** Windows, macOS, or Linux

### Compilation Instructions

#### Using Command Line (GCC)
```bash
g++ -o nav_system smart_navigation_system.cpp
```

#### Using Code::Blocks IDE
1. Create new C++ project
2. Add `smart_navigation_system.cpp` to project
3. Go to Build → Compile & Run (or press F9)
4. Executable created automatically

#### Using Visual Studio
1. Create new Empty C++ Project
2. Add source file to project
3. Build Solution (Ctrl + Shift + B)

---

## How to Run

### Linux/macOS Terminal
```bash
./nav_system
```

### Windows Command Prompt
```bash
nav_system.exe
```

### CodeBlocks IDE
Click "Build and Run" button or press F9

---

## Usage Guide

### Main Menu Options

```
1. Manage Locations
   - Add Location
   - View All Locations
   - Delete Location
   - Search Location

2. Manage Roads
   - Add Road
   - View All Roads
   - Update Road Distance
   - Update Road Status
   - Delete Road
   - Toggle Road Availability

3. Traffic Simulation
   - View Current Traffic Status
   - Update Road Status
   - Toggle Road Availability

4. Manual Route Builder (Stack)
   - Push Location to route
   - Pop Location from route
   - View Current Path
   - Save Path to History

5. Shortest Path (Dijkstra)
   - Find optimal route between locations
   - Considers traffic conditions
   - Saves route to history

6. Route History (Linked List)
   - View all searched paths
   - Clear history

7. Path Cost Estimator
   - Enter start and end location
   - Get distance, fuel, and time estimates

8. Sort Locations by Name
   - Alphabetically sort all locations
   - Uses Selection Sort

9. ASCII Map View
   - Visual network representation
   - Shows connections and distances

10. System Statistics
    - View total locations and roads
    - Traffic status breakdown
    - Route history count

0. Exit

```

### Example Workflow

**Scenario: Finding shortest path from DHA to Defence Road**

```
1. System auto-loads 10 locations and 20 roads
2. Select option 5: Shortest Path (Dijkstra)
3. Enter start location ID: 1 (DHA)
4. Enter end location ID: 6 (Defence Road)
5. System calculates and displays:
   - Shortest path with intermediate stops
   - Total distance
   - Cost estimation (fuel + time)
6. Route automatically saved to history
```

---

## Pre-loaded Data

### 10 Locations
- DHA
- Sadar Bazaar
- Anarkali
- Model Town
- Gulberg
- Defence Road
- Canal Road
- Mall Road
- Johar Town
- Bahria Town

### 20 Sample Roads
Bidirectional connections with realistic distances (4-20 km) and varied traffic statuses.

---

## Color Coding

| Color | Meaning |
|-------|---------|
| GREEN | Success messages, Normal traffic |
| RED | Errors, Blocked roads |
| YELLOW | Warnings, Heavy traffic, Important data |
| CYAN | Headers and titles |
| BLUE | Information messages |

---

## Project Structure

```
smart_navigation_system.cpp
├── Libraries & Includes
├── Color Definitions
├── Constants & Data Structures
├── Utility Functions
│   ├── clearScreen()
│   ├── clearBuffer()
│   ├── displayHeader()
│   ├── locationExists()
│   └── findLocationIndexByID()
├── Location Management
│   ├── addLocation()
│   ├── viewLocations()
│   ├── deleteLocation()
│   └── searchLocation()
├── Road Management
│   ├── addRoad()
│   ├── viewRoads()
│   ├── updateRoadStatus()
│   ├── updateRoadDistance()
│   ├── deleteRoad()
│   └── toggleRoadAvailability()
├── Traffic Simulation
│   └── trafficSimulation()
├── Shortest Path (Dijkstra)
│   └── findShortestPath()
├── Manual Route Builder
│   └── manualRouteBuilder()
├── Route History (Linked List)
│   ├── addRouteHistory()
│   ├── viewHistory()
│   └── clearHistory()
├── Analytics
│   ├── pathCostEstimator()
│   ├── displayASCIIMap()
│   └── showStatistics()
├── Sorting & Searching
│   ├── sortLocationsByName()
│   └── searchLocationLinear()
├── Pre-loaded Data
│   └── initializePreloadedData()
└── Main Function
    └── main()
```

---

## Sample Test Cases

### Test Case 1: Basic Shortest Path
```
Input: Start = 1 (DHA), End = 6 (Defence Road)
Expected: Path found with optimal distance
Status: PASS
```

### Test Case 2: Heavy Traffic Rerouting
```
Input: Update road to "Heavy Traffic"
       Find same path again
Expected: Alternative route selected if shorter
Status: PASS
```

### Test Case 3: Blocked Road Handling
```
Input: Mark road as Blocked
       Find path using that road
Expected: System avoids blocked road
Status: PASS
```

### Test Case 4: Cost Estimation
```
Input: Route distance = 26 km
Expected: Fuel ≈ 3.12 L, Time ≈ 39 min, Cost ≈ $4.68
Status: PASS
```

### Test Case 5: Manual Route Building
```
Input: Push 3 locations, Pop 1
Expected: Stack correctly maintains LIFO order
Status: PASS
```

---

## Known Limitations

1. **Console-Only Interface** - No GUI, text-based interface only
2. **No Persistence** - Data lost when program closes (no file save)
3. **Fixed Capacity** - Max 50 locations, 100 roads
4. **No Multi-Threading** - Single-threaded operation only
5. **No Real-Time Traffic API** - Manual status updates only
6. **Local System Only** - Doesn't connect to real mapping services

---

## Future Enhancements

### Short Term
- Add delete location confirmation dialog
- Implement location name uniqueness check
- Add estimated time of arrival (ETA)
- Support for multiple shortest paths

### Medium Term
- File I/O for saving/loading routes
- Database integration (SQLite)
- Graphical user interface (Qt/OpenGL)
- Historical traffic data tracking

### Long Term
- Mobile app development
- Real-time GPS integration
- Machine learning for traffic prediction
- Integration with real mapping APIs (Google Maps)
- Multi-user support with authentication

---

## Technical Details

### Memory Management
- **Stack allocation:** Location array, Road array, Dijkstra arrays
- **Heap allocation:** Linked list nodes (manually deleted)
- **Dynamic allocation:** Vectors for adjacency list
- **RAII:** Automatic cleanup of local objects

### Input Validation
- Integer type checking with `cin.fail()`
- ID existence validation before operations
- Range validation for array indexing
- Empty collection checks before operations

### Error Handling
- Graceful handling of invalid inputs
- Informative error messages with color coding
- Recovery without program termination
- User guidance for correct operations

---

## Compilation & Execution Time

| Compiler | Size | Time |
|----------|------|------|
| GCC | ~40 KB | <0.5s compile |
| Clang | ~38 KB | <0.5s compile |
| MSVC | ~45 KB | <1s compile |

**Runtime:** Instant (initialization + first operation < 100ms)

---

## Credits & References

### Data Structures Reference
- Dijkstra's Algorithm: [GeeksforGeeks](https://www.geeksforgeeks.org/dijkstras-algorithm/)
- Stack: [cppreference.com](https://en.cppreference.com/w/cpp/container/stack)
- Linked List: [Tutorials Point](https://www.tutorialspoint.com/data_structures_algorithms/linked_list_basics.htm)

### Tools Used
- **IDE:** Code::Blocks, Visual Studio
- **Compiler:** GCC, MSVC
- **Language:** C++11/C++14/C++17

---

## License

Educational Project - Free for academic use

---

## Contact & Support

**For questions or issues:**
- Contact group members
- Refer to code comments for detailed explanations
- Check viva preparation document for algorithm details

### Author
**Zain Aftab** (FA24-BSE-155)

**Group Members:**
- Zain Aftab (FA24-BSE-155)
- Hunain Riasat (FA24-BSE-083)
- Shaheer Aftab (FA24-BSE-089)
- Ahsan Ahmed (FA24-BSE-074)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | Current | Initial release with core features |
| Future | TBD | File persistence, GUI, database |

---

## Acknowledgments

Thanks to:
- Faculty for guidance and support
- Classmates for feedback
- Open-source C++ community for resources

---

**Last Updated:** January 2026  
**Status:** Complete and tested

---

## Quick Reference

### Keyboard Shortcuts (CodeBlocks)
- **F9** - Build and Run
- **F10** - Debug
- **Ctrl+Shift+B** - Build (VS)
- **Ctrl+F5** - Run without debug (VS)

### Common Commands
```bash
# Clear screen (in program)
Menu → Select option → Auto-clears

# Exit program
Select option 0 from main menu
```

---

**Happy Navigation! 🗺️**
