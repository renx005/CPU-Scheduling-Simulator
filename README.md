# CPU Scheduling Simulator (Linked List Version)

### 📌 Mini Project – Data Structures using C

This project simulates major CPU scheduling algorithms used by operating systems.  
It allows users to input process details and observe how different scheduling strategies affect performance.

---

## 🧠 Algorithms Implemented

| Algorithm | Type | Preemptive | Data Structure Used |
|----------|------|-------------|-------------------|
| FCFS - First Come First Serve | Non-preemptive | No | Linked List |
| SJF - Shortest Job First | Non-preemptive | No | Linked List |
| Priority Scheduling | Non-preemptive | No | Linked List (priority-based selection) |
| Round Robin | Preemptive | Yes | Circular Linked List |

---

## 🗂 Data Structures Used

- **Singly Linked List** → Stores processes dynamically  
- **Circular Linked List** → Used as ready queue in Round Robin  
- **Dynamic memory allocation** (`malloc`) → For node creation  

Each process node contains:
```c
struct Process {
    int pid, arrival, burst, priority, remaining;
    int start, finish, waiting, turnaround;
    struct Process* next;
};
