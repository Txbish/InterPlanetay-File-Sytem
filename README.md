# Distributed Hash Table (DHT) with IPFS Functionality

This project implements a **Ring Distributed Hash Table (DHT)** to simulate the **InterPlanetary File System (IPFS)** functionality on a single machine. It efficiently stores and retrieves files using a distributed approach, leveraging **SHA-1 hashing**, **B-Trees**, and **optimized routing tables**.

## 📌 Features
- **Configurable Identifier Space**: Allows users to define the bit size of the identifier space.
- **Machine Management**: 
  - Dynamically add/remove machines without disrupting functionality.
  - Manual or automatic ID assignment to machines.
- **Efficient File Storage & Retrieval**:
  - Files are stored based on their **SHA-1 hash values**.
  - Uses **B-Trees** for efficient storage and retrieval.
  - Lookup optimized with **O(log N) search complexity** using routing tables.
- **Routing Table Visualization**:
  - Supports **printing routing tables** for debugging.
  - Displays **path taken for insertion, deletion, and retrieval** of files.
- **Graceful Machine Exit**:
  - Ensures proper redistribution of files before a machine leaves.

## 📂 File Storage Mechanism
- Files are inserted as **(key, value)** pairs where:
  - `key = SHA-1 hash of file content`
  - `value = Actual file stored on the system`
- Files are stored in **B-Trees** within the machines.

## 🔍 Search Algorithm
1. If the requested file is found on the same machine, **B-Tree search** retrieves it.
2. If not, the request is forwarded to the **next machine** in the ring.
3. **Routing tables** speed up the search using **logarithmic lookup**.

## 🛠️ Commands & Functionality
| Command | Description |
|---------|-------------|
| `add_machine` | Add a new machine to the ring |
| `remove_machine` | Remove an existing machine gracefully |
| `insert_file <file>` | Store a file in the system |
| `remove_file <file_hash>` | Remove a file using its hash |
| `search_file <file_hash>` | Locate a file and return its storage path |
| `print_btree` | Display B-Tree of a machine |
| `print_routing_table` | Show routing table of a machine |

## 🚀 Getting Started
### Prerequisites
- **C++ (Visual Studio 2019)**
- **Basic understanding of hashing & DHTs**

### Installation & Execution
1. **Clone the repository**
   ```sh
   git clone https://github.com/yourusername/InterPlanetay-File-Sytem.git
   cd InterPlanetay-File-Sytem
