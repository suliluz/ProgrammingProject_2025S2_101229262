# Project Data Structures & Patterns Analysis

## 1. 10 Data Structures and Patterns Used

### 1. Polymorphism
- **Location**: `base/headers/Element.h:5-9` and `base/headers/Visitor.h:7-12`
- **Implementation**: Abstract base classes with pure virtual functions
  - `Element` class with virtual `accept(Visitor&)` method
  - `Visitor` class with virtual `visit()` methods

### 2. Inheritance & Derived Classes
- **Location**:
  - `src/dialogue/Dialogue.h:11` - `Dialogue` extends `Element`
  - `src/dialogue/Choice.h:10` - `Choice` extends `Element`
  - `src/engine/DialogueVisitor.h:12` - `DialogueVisitor` extends `Visitor`
  - `src/engine/states/GameState.h:6-17` - Base class for state pattern
  - All game states inherit from `GameState` (MainMenuState, InGameState, LoadGameState, SettingsState)

### 3. Hash Tables
- **Location**: `base/headers/HashTable.h:12-131`
- **Implementation**: Chaining-based hash table using Lists as buckets
- **Key features**:
  - Generic key-value storage (`HashTable<K, V>`)
  - Uses separate chaining for collision resolution
  - Each bucket is a `List<HashEntry<K, V>>`

### 4. Doubly Linked List
- **Location**: `base/headers/List.h:9-314`
- **Implementation**: Template class using `DoublyLinkedNode<T>`
- **Key features**:
  - Head and tail pointers
  - Bidirectional traversal
  - Operations: push, pop, shift, insertAt, removeAt
  - Supports bidirectional iteration

### 5. Singly Linked List
- **Location**: `base/headers/SinglyLinkedList.h:8-172`
- **Implementation**: Template class using `SinglyLinkedNode<T>`
- **Key features**:
  - Head and tail pointers
  - Forward-only traversal
  - Operations: push, pushFront, popFront

### 6. Stack
- **Location**: `base/headers/Stack.h:5-32`
- **Implementation**: Adapter pattern wrapping `SinglyLinkedList<T>`
- **Key operations**: push, pop, top, isEmpty, size
- **LIFO behavior** via `pushFront()` and `popFront()` delegation

### 7. Queue
- **Location**: `base/headers/Queue.h:6-32`
- **Implementation**: Adapter pattern wrapping `SinglyLinkedList<T>`
- **Key operations**: enqueue, dequeue, front, isEmpty, size
- **FIFO behavior** via `push()` (tail) and `popFront()` (head)

### 8. N-ary Tree (Tree)
- **Location**: `base/headers/NTree.h:7-156`
- **Implementation**: Generic N-child tree with template parameter N
- **Key features**:
  - Fixed maximum children count (N)
  - NIL sentinel node pattern
  - Recursive operations: getSize(), search()
  - Tree manipulation: attachNTree(), detachNTree()

### 9. Iterator Pattern
- **Location**:
  - `base/headers/BidirectionalIterator.h:8-85` - For doubly linked lists
  - `base/headers/ForwardIterator.h` (referenced) - For singly linked lists
- **Implementation**: STL-style iterators with operator overloading
- **Supported operations**: `++`, `--`, `*`, `->`, `==`, `!=`

### 10. Visitor Pattern
- **Location**:
  - `base/headers/Visitor.h:7-12` - Abstract visitor
  - `base/headers/Element.h:5-9` - Element interface
  - `src/engine/DialogueVisitor.h:12` - Concrete visitor implementation
- **Implementation**: Classic double dispatch pattern for traversing dialogue tree

---

## 2. Where & How They Are Used in the Game

### Hash Tables - 3 Nested Levels
**Location**: `src/dialogue/DialogueGraph.h:49-52`

```cpp
HashTable<string, HashTable<string, NodeInfo*>*> fileNodeData;
HashTable<string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;
```

**Usage**:
1. **Dialogue System - File Management** (`DialogueGraph.h:49`)
   - **Purpose**: Organize dialogue nodes by file and node ID
   - **Structure**: `fileNodeData[filename][nodeId] → NodeInfo*`
   - **Game Flow**: Loads multiple dialogue files (main story, side quests, NPC conversations)

2. **Dialogue System - Node Cache** (`DialogueGraph.h:52`)
   - **Purpose**: Quick lookup of already-built dialogue tree nodes
   - **Structure**: `builtNodes[nodeId] → NTree<Dialogue>*`
   - **Game Flow**: Prevents rebuilding nodes when player revisits dialogues

3. **How They Work Together** (detailed below in section 3)

### Doubly Linked List (List)
**Locations**:
1. **Inventory System** (`src/game/Inventory.h:10`)
   ```cpp
   List<Item> items;
   ```
   - Stores player's items with bidirectional access
   - Allows forward/backward iteration during display
   - Used for searching, adding, and removing items

2. **Dialogue Choices** (`src/dialogue/Dialogue.h:15`)
   ```cpp
   List<Choice> choices;
   ```
   - Each dialogue node contains a list of player choices
   - Iterator used to display and select choices in `DialogueVisitor.cpp:57-69`

3. **Action Lists** (`src/dialogue/DialogueGraph.h:33`)
   ```cpp
   List<Action> actions;
   ```
   - Each choice can trigger multiple actions (give gold, items, XP)
   - Executed sequentially when choice is selected (`DialogueGraph.cpp:226-233`)

4. **File Tracking** (`src/dialogue/DialogueGraph.h:50`)
   ```cpp
   List<string> allFiles;
   ```
   - Tracks all loaded dialogue files for cleanup

5. **Tree Node Tracking** (`src/dialogue/DialogueGraph.h:51`)
   ```cpp
   List<NTree<Dialogue, MAX_CHOICES>*> allTreeNodes;
   ```
   - Owns all dialogue tree nodes for memory management

6. **Choice Texts in Visitor** (`src/engine/DialogueVisitor.h:50`)
   ```cpp
   List<sf::Text*> choiceTexts;
   ```
   - Stores SFML text objects for rendering dialogue choices on screen

### Singly Linked List
**Location**: `src/engine/DialogueVisitor.h:73`

**Direct Usage** - Conversation History Log:
```cpp
SinglyLinkedList<DialogueEntry> conversationLog;
```

**Game Flow**:
1. **Logging** (`DialogueVisitor.cpp:44`): Each dialogue visited is logged
   ```cpp
   conversationLog.push(DialogueEntry(dialogue.speaker, dialogue.message));
   ```

2. **Display** (`DialogueVisitor.cpp:628-664`): Forward-only iteration to display history
   - Press 'H' to view full conversation history
   - Shows speaker, message, and timestamp for each entry
   - Demonstrates single-direction traversal characteristic

3. **Why SinglyLinkedList**:
   - History only needs forward traversal (chronological order)
   - More memory-efficient than doubly-linked (no previous pointers)
   - Append-only operation (push to end)

### Stack
**Location**: `src/engine/states/InGameState.h:37`

**Direct Usage** - Dialogue Navigation History (Undo/Back Button):
```cpp
Stack<string> dialogueHistory;  // Track visited node IDs
```

**Game Flow**:
1. **Push** (`InGameState.cpp:345`): Before navigating to new node
   ```cpp
   dialogueHistory.push(currentNodeId);  // Save current before moving
   ```

2. **Pop** (`InGameState.cpp:364`): Go back to previous dialogue
   ```cpp
   string previousNodeId = dialogueHistory.pop();
   ```

3. **UI Integration** (`InGameState.cpp:240-241`): Back button in top-right
   - Purple "Back" button uses Stack to undo last choice
   - LIFO behavior: Most recent choice is undone first
   - Demonstrates Stack's undo/redo capability

### Queue
**Location**: `src/dialogue/DialogueGraph.h:70`

**Direct Usage** - Delayed Action System:
```cpp
Queue<DelayedAction> pendingActions;
```

**Game Flow**:
1. **Enqueue** (`DialogueGraph.cpp:427`): Add action with delay
   ```cpp
   pendingActions.enqueue(DelayedAction(action, delaySeconds));
   ```

2. **Process** (`DialogueGraph.cpp:432-444`): Update over time (FIFO)
   ```cpp
   DelayedAction& frontAction = pendingActions.front();
   frontAction.delaySeconds -= deltaTime;
   if (frontAction.delaySeconds <= 0) {
       pendingActions.dequeue();  // Execute oldest action first
   }
   ```

3. **Use Cases**:
   - Delayed gold rewards
   - Timed item delivery
   - Cutscene action sequences
   - Demonstrates Queue's FIFO ordering for time-based events

### N-ary Tree
**Location**: Throughout dialogue system

**Structure** (`src/dialogue/DialogueGraph.h:51`):
```cpp
NTree<Dialogue, MAX_CHOICES>* rootTree;
```

**Game Flow**:
1. **Initialization**: `DialogueGraph::buildTree()` creates root node
2. **Navigation**: Each dialogue node has up to MAX_CHOICES children
3. **Branching Story**: Player choices determine which child node to visit next
4. **Example** (`DialogueGraph.cpp:187-211`):
   - Node stores Dialogue (speaker + message)
   - Each child represents a different choice
   - Clicking choice navigates to that child node

**In-Game Usage** (`src/engine/states/InGameState.h:30`):
```cpp
NTree<Dialogue, MAX_CHOICES>* currentDialogueNode;
```
- Tracks current position in dialogue tree
- Updated when player makes choice

### Iterator Pattern
**Usage Throughout Game**:

1. **Inventory Display** (`src/game/Inventory.h:113-124`):
   ```cpp
   auto it = items.getIterator();
   while (it != endIt) {
       const Item& item = it.getCurrent()->getValue();
       // Display item
       ++it;
   }
   ```

2. **Dialogue Choice Rendering** (`src/engine/DialogueVisitor.cpp:57-69`):
   ```cpp
   auto it = dialogue.choices.getIterator();
   while(it != endIt) {
       // Create SFML text for each choice
       ++it;
   }
   ```

3. **HashTable Bucket Traversal** (`base/headers/HashTable.h:47-56`):
   ```cpp
   auto it = bucket.getIterator();
   while (it != endIt) {
       if (it.getCurrent()->getValue().key == key) {
           // Found matching key
       }
       ++it;
   }
   ```

4. **Memory Cleanup** (`DialogueGraph.cpp:25-38`):
   - Iterates through lists to delete dynamically allocated objects

### Visitor Pattern
**Location**: `src/engine/DialogueVisitor.cpp:35-77`

**Game Flow**:
1. **DialogueVisitor visits Dialogue** (`DialogueVisitor.cpp:35`):
   - Extracts speaker and message
   - Creates SFML text objects for rendering
   - Wraps text to fit window

2. **DialogueVisitor visits Choice** (`DialogueVisitor.cpp:73`):
   - Executes choice's action (lambda function)
   - Actions can modify player state (gold, items, HP)

3. **Double Dispatch**:
   ```cpp
   dialogue.accept(dialogueVisitor);  // Element knows visitor
   visitor.visit(dialogue);           // Visitor knows how to handle Dialogue
   ```

**Game Usage** (`InGameState.cpp` - inferred):
- Dialogue tree node is "visited" to render it
- Player selects choice → choice is "visited" → action executes

### Polymorphism
**Dynamic Behavior**:

1. **State Pattern** (`src/engine/GameEngine.h:33-34`):
   ```cpp
   unique_ptr<GameState> currentState;
   ```
   - Runtime polymorphism for game screens
   - `MainMenuState`, `InGameState`, `LoadGameState`, `SettingsState`
   - Each overrides `handleInput()`, `update()`, `render()`

2. **Element/Visitor Pattern**:
   - `Element*` can point to `Dialogue` or `Choice`
   - Visitor dispatches to correct `visit()` method at runtime

---

## 3. How Hash Tables Work Hand in Hand - Detailed Analysis

### The Two-Level Hash Table System

#### Level 1: File Organization
```cpp
HashTable<string, HashTable<string, NodeInfo*>*> fileNodeData;
```

**Purpose**: Organize dialogue nodes by source file

**Structure**:
```
fileNodeData
├── "main_story.txt" → HashTable<string, NodeInfo*>*
│   ├── "root" → NodeInfo{speaker:"Merchant", message:"Hello!", ...}
│   ├── "quest_start" → NodeInfo{...}
│   └── "quest_end" → NodeInfo{...}
├── "side_quest.txt" → HashTable<string, NodeInfo*>*
│   ├── "npc_greeting" → NodeInfo{...}
│   └── "npc_farewell" → NodeInfo{...}
```

**Operations**:

1. **Loading** (`DialogueGraph.cpp:109-111`):
   ```cpp
   allFiles.push(filename);
   auto* currentFileNodes = new HashTable<string, NodeInfo*>();
   fileNodeData.insert(filename, currentFileNodes);
   ```

2. **Storing Nodes** (`DialogueGraph.cpp:125-126, 150`):
   ```cpp
   currentFileNodes->insert(currentNode->nodeId, currentNode);
   ```

3. **Retrieving Nodes** (`DialogueGraph.cpp:163-176`):
   ```cpp
   // Search across all loaded files
   auto fileIt = allFiles.getIterator();
   while (fileIt != endFileIt) {
       auto** fileNodes = fileNodeData.search(fileIt.getCurrent()->getValue());
       if (fileNodes) {
           auto* dataPtr = (*fileNodes)->search(nodeId);
           if (dataPtr) {
               data = *dataPtr;  // Found it!
               break;
           }
       }
       ++fileIt;
   }
   ```

#### Level 2: Built Node Cache
```cpp
HashTable<string, NTree<Dialogue, MAX_CHOICES>*> builtNodes;
```

**Purpose**: Cache constructed dialogue tree nodes to avoid rebuilding

**Structure**:
```
builtNodes
├── "root" → NTree<Dialogue>* (pointer to tree node)
├── "quest_start" → NTree<Dialogue>*
└── "npc_greeting" → NTree<Dialogue>*
```

**Operations**:

1. **Check Cache First** (`DialogueGraph.cpp:158-161`):
   ```cpp
   auto* existing = builtNodes.search(nodeId);
   if (existing) {
       return *existing;  // Already built, reuse it!
   }
   ```

2. **Build and Cache** (`DialogueGraph.cpp:187-189`):
   ```cpp
   auto* node = new NTree<Dialogue, MAX_CHOICES>(dialogue);
   builtNodes.insert(nodeId, node);  // Cache for future use
   allTreeNodes.push(node);          // Track for cleanup
   ```

### How They Collaborate - The Complete Flow

#### Scenario: Loading and Displaying a Dialogue

**Step 1: File Loading Phase**
```
User starts game → GameEngine loads dialogue files
↓
DialogueGraph::loadFromFile("main_story.txt")
↓
fileNodeData["main_story.txt"] = new HashTable<nodeId, NodeInfo*>
↓
For each NODE: in file:
    Parse node data
    fileNodeData["main_story.txt"]["root"] = new NodeInfo{...}
```

**Step 2: Tree Building Phase**
```
InGameState initializes → Need to display first dialogue
↓
DialogueGraph::buildTree()
↓
buildNode("root")
    ↓
    Check: builtNodes.search("root") → nullptr (not cached)
    ↓
    Search fileNodeData across all files:
        fileNodeData["main_story.txt"]->search("root") → NodeInfo*
    ↓
    Create: NTree<Dialogue>("Hello, traveler!")
    ↓
    For each choice in NodeInfo:
        Recursively buildNode(targetNodeId)
            ↓
            Check: builtNodes.search(targetNodeId) → cache hit/miss
            ↓
            If miss: look up in fileNodeData, build, cache
    ↓
    builtNodes.insert("root", newNode)  // Cache it
    ↓
    Return tree node
```

**Step 3: Runtime Navigation**
```
Player clicks choice "Accept Quest"
↓
Choice action executes: navigate to "quest_accepted" node
↓
DialogueGraph::getNode("quest_accepted")
    ↓
    Check: builtNodes.search("quest_accepted")
    ↓
    If cached: return immediately (FAST!)
    ↓
    If not: buildNode("quest_accepted") → searches fileNodeData → builds → caches
```

### Why This Design?

1. **File-based Organization** (`fileNodeData`):
   - **Modularity**: Different quests in different files
   - **Cleanup**: Can unload entire dialogue files
   - **Debugging**: Easy to identify which file a node came from

2. **Node Caching** (`builtNodes`):
   - **Performance**: Building tree nodes is expensive (recursive)
   - **Reusability**: Same dialogue can be visited multiple times
   - **Memory Management**: Tracks all nodes for cleanup in destructor

3. **Separation of Concerns**:
   - **fileNodeData**: Stores raw parsed data (NodeInfo)
   - **builtNodes**: Stores constructed game objects (NTree)
   - This allows reloading files without rebuilding trees

### Hash Table Chaining Details

**Collision Resolution** (`HashTable.h:44-60`):
```cpp
size_t index = getIndex(key);  // Hash function
Bucket& bucket = table[index]; // Get list at this index

// Traverse list to find matching key
auto it = bucket.getIterator();
while (it != endIt) {
    if (it.getCurrent()->getValue().key == key) {
        // Found it! Update or return
    }
    ++it;
}
// Not found, append to list
bucket.push(Entry(key, value));
```

**Why Chaining?**:
- Multiple keys can hash to same index
- Each bucket (table slot) is a List (doubly-linked)
- List allows unlimited entries per bucket (no load factor issues)

### Memory Management Flow

**Destruction Order** (`DialogueGraph.cpp:23-51`):
```cpp
~DialogueGraph() {
    // 1. Delete all NodeInfo objects
    for (NodeInfo* in allNodeInfos) delete it;

    // 2. Delete all NTree nodes
    for (NTree* in allTreeNodes) delete it;

    // 3. Delete nested hash tables
    for (file in allFiles) {
        HashTable<string, NodeInfo*>** ptr = fileNodeData.search(file);
        delete *ptr;  // Delete inner hash table
    }

    // 4. Clear outer hash table
    fileNodeData.clear();
}
```

**Why Track Everything?**:
- `allNodeInfos`: Raw data ownership
- `allTreeNodes`: Tree node ownership
- `builtNodes`: Just pointers (doesn't own)
- `fileNodeData`: Owns nested hash tables

---

## Summary: Data Structure Integration

The game demonstrates sophisticated data structure usage with **all 10 data structures fully utilized**:

1. **Hash tables** (2 nested levels) provide O(1) dialogue lookup across modular files
2. **N-ary Trees** model branching narrative structure with up to MAX_CHOICES children
3. **Doubly Linked Lists** manage dynamic collections (inventory, choices, actions)
4. **Singly Linked List** stores chronological conversation history (forward-only)
5. **Stack** enables undo functionality for dialogue navigation (LIFO)
6. **Queue** processes delayed actions in order received (FIFO)
7. **Iterators** (bidirectional & forward) enable uniform traversal across all containers
8. **Visitor pattern** separates dialogue data from rendering logic via double dispatch
9. **Polymorphism** enables flexible game state management (GameState hierarchy)
10. **Inheritance** creates extensible class hierarchies (Element→Dialogue/Choice)

### Key Design Patterns:

**Nested Hash Tables** - Elegant two-level system balancing:
- **Modularity** (file-based organization)
- **Performance** (node caching)
- **Memory safety** (explicit ownership tracking)

**Stack/Queue/SinglyLinkedList Triangle** - Each serves distinct purpose:
- **Stack**: Undo/redo (most recent first)
- **Queue**: Time-ordered events (oldest first)
- **SinglyLinkedList**: Append-only logs (forward traversal)

This architecture supports a complex, data-driven RPG dialogue system with:
- Branching narratives with conditional choices
- Dynamic game state modification
- **Player-friendly features**: Back button, delayed actions, conversation history
- Type-safe generic programming (templates throughout)
