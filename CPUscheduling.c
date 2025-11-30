/* (Project:CPU Scheduling Simulator)
@Rena Elza Viju
Roll No: 48
Date: 18-10-2025 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------- Structure --------------------
struct Process {
    int pid;          // Process ID (e.g., 1, 2, 3)
    int arrival;      // Arrival Time (AT)
    int burst;        // Original Burst Time (BT)
    int priority;     // Priority Level (lower number = higher priority)
    int remaining;    // Remaining Burst Time (used in Preemptive algorithms like RR)
    int start;        // Time the process first starts execution
    int finish;       // Finish Time (FT)
    int waiting;      // Waiting Time (WT)
    int turnaround;   // Turnaround Time (TAT)
    struct Process* next; // pointer to next node
};

// -------------------- Global Head --------------------
struct Process* head = NULL; // Initial, uncorrupted list of processes

// -------------------- Helper: Create Process Node --------------------
struct Process* createProcess(int pid, int arrival, int burst, int priority) {
    struct Process* p = (struct Process*)malloc(sizeof(struct Process));
    if (p == NULL) {
        perror("Failed to allocate memory for process");
        exit(EXIT_FAILURE);
    }
    p->pid = pid;
    p->arrival = arrival;
    p->burst = burst;
    p->priority = priority;
    p->remaining = burst; // Initialize remaining to full burst
    p->start = 0;
    p->finish = 0;
    p->waiting = 0;
    p->turnaround = 0;
    p->next = NULL;
    return p;
}

// -------------------- Helper: Append Process to List --------------------
void appendProcess(struct Process** headRef, struct Process* newProcess) {
    if (*headRef == NULL) {
        *headRef = newProcess;
    } else {
        struct Process* temp = *headRef;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newProcess;
    }
}

// -------------------- Helper: Deep Copy Process List (Crucial for resetting state) --------------------
struct Process* copyProcessList(struct Process* source) {
    struct Process* newHead = NULL;
    struct Process* temp = source;
    while (temp != NULL) {
        // Create a fresh node with original data and reset metrics
        struct Process* newNode = createProcess(temp->pid, temp->arrival, temp->burst, temp->priority);
        appendProcess(&newHead, newNode);
        temp = temp->next;
    }
    return newHead;
}

// -------------------- Helper: Free Process List (Crucial for memory management) --------------------
void freeProcessList(struct Process* headRef) {
    struct Process *current = headRef;
    struct Process *next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

// -------------------- Display Result --------------------
void displayResult(struct Process* head, const char* algoName) {
    struct Process* temp = head;
    float totalWT = 0, totalTAT = 0;
    int count = 0;

    printf("\n--- %s Performance Metrics ---\n", algoName);
    printf("PID\tAT\tBT\tPR\tWT\tTAT\tFT\n");
    while (temp != NULL) {
        // Calculate final metrics before printing (if not calculated during RR)
        if (temp->finish > 0) {
            temp->turnaround = temp->finish - temp->arrival;
            temp->waiting = temp->turnaround - temp->burst;
        }

        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                temp->pid, temp->arrival, temp->burst, temp->priority,
                temp->waiting, temp->turnaround, temp->finish);
        totalWT += temp->waiting;
        totalTAT += temp->turnaround;
        count++;
        temp = temp->next;
    }

    if (count > 0) {
        printf("\nAverage Waiting Time: %.2f\n", totalWT / count);
        printf("Average Turnaround Time: %.2f\n", totalTAT / count);
    }
}

// --------------------------------------------------------------------------------
// 1. FCFS Scheduling
// --------------------------------------------------------------------------------
void fcfs(struct Process* sourceHead) {
    struct Process* currentList = copyProcessList(sourceHead);
    struct Process* temp = currentList;
    int time = 0;
    
    printf("\n--- FCFS Scheduling ---\n");
    printf("Gantt Chart:\n|");

    while (temp != NULL) {
        // Handle CPU idle time
        if (time < temp->arrival) {
            // Note: If you want to print idle time on Gantt chart, insert a print statement here
            time = temp->arrival;
        }
            
        temp->start = time;
        temp->waiting = temp->start - temp->arrival;
        
        time += temp->burst;
        
        temp->finish = time;
        temp->turnaround = temp->finish - temp->arrival;

        printf(" P%d |", temp->pid);
        temp = temp->next;
    }

    // Print the timeline numbers
    printf("\n0");
    temp = currentList;
    while (temp != NULL) {
        printf("    %d", temp->finish);
        temp = temp->next;
    }
    printf("\n");

    displayResult(currentList, "FCFS");
    freeProcessList(currentList);
}

// --------------------------------------------------------------------------------
// 2. SJF Scheduling (Non-preemptive)
// --------------------------------------------------------------------------------
void sjf(struct Process* sourceHead) {
    struct Process* currentList = copyProcessList(sourceHead);
    struct Process* curr;
    int time = 0;
    int completed = 0;
    int n = 0;
    curr = currentList;
    while (curr != NULL) { n++; curr = curr->next; }

    printf("\n--- SJF (Non-preemptive) Scheduling ---\n");
    printf("Gantt Chart:\n|");

    while (completed < n) {
        struct Process* best = NULL;
        curr = currentList;
        
        // Find the best process (arrived and not finished, with shortest burst)
        while (curr != NULL) {
            if (curr->finish == 0 && curr->arrival <= time) {
                // Check if it's the first eligible, or shorter than the current 'best'
                if (best == NULL || curr->burst < best->burst || (curr->burst == best->burst && curr->arrival < best->arrival))
                    best = curr; // Tie-breaker: FCFS
            }
            curr = curr->next;
        }

        if (best == NULL) {
            // CPU is idle. Advance time to the arrival of the next process.
            int next_arrival = 99999;
            curr = currentList;
            while(curr != NULL) {
                if (curr->finish == 0 && curr->arrival < next_arrival) {
                    next_arrival = curr->arrival;
                }
                curr = curr->next;
            }
            if (next_arrival != 99999) time = next_arrival;
            else time++; // Should only happen if all are finished
            continue;
        }

        // Run the selected process to completion (Non-preemptive)
        best->start = time;
        best->waiting = best->start - best->arrival;
        time += best->burst;
        best->finish = time;
        best->turnaround = best->finish - best->arrival;
        completed++;

        printf(" P%d |", best->pid);
    }

    // Print the timeline numbers
    printf("\n0");
    curr = currentList;
    while (curr != NULL) {
        printf("    %d", curr->finish);
        curr = curr->next;
    }
    printf("\n");

    displayResult(currentList, "SJF (Non-preemptive)");
    freeProcessList(currentList);
}

// --------------------------------------------------------------------------------
// 3. Priority Scheduling (Non-preemptive)
// --------------------------------------------------------------------------------
void priorityScheduling(struct Process* sourceHead) {
    struct Process* currentList = copyProcessList(sourceHead);
    struct Process* curr;
    int time = 0;
    int completed = 0;
    int n = 0;
    curr = currentList;
    while (curr != NULL) { n++; curr = curr->next; }

    printf("\n--- Priority (Non-preemptive) Scheduling ---\n");
    printf("Gantt Chart:\n|");

    while (completed < n) {
        struct Process* best = NULL;
        curr = currentList;
        
        // Find the best process (arrived and not finished, with lowest priority number)
        while (curr != NULL) {
            if (curr->finish == 0 && curr->arrival <= time) {
                // Check if it's the first eligible, or has a lower (higher) priority
                if (best == NULL || curr->priority < best->priority)
                    best = curr;
            }
            curr = curr->next;
        }

        if (best == NULL) {
            // CPU is idle. Advance time to the arrival of the next process.
            int next_arrival = 99999;
            curr = currentList;
            while(curr != NULL) {
                if (curr->finish == 0 && curr->arrival < next_arrival) {
                    next_arrival = curr->arrival;
                }
                curr = curr->next;
            }
            if (next_arrival != 99999) time = next_arrival;
            else time++;
            continue;
        }

        // Run the selected process to completion (Non-preemptive)
        best->start = time;
        best->waiting = best->start - best->arrival;
        time += best->burst;
        best->finish = time;
        best->turnaround = best->finish - best->arrival;
        completed++;

        printf(" P%d |", best->pid);
    }

    // Print the timeline numbers
    printf("\n0");
    curr = currentList;
    while (curr != NULL) {
        printf("    %d", curr->finish);
        curr = curr->next;
    }
    printf("\n");

    displayResult(currentList, "Priority Scheduling (Non-preemptive)");
    freeProcessList(currentList);
}

// --------------------------------------------------------------------------------
// 4. Round Robin (Preemptive)
// --------------------------------------------------------------------------------
void roundRobin(struct Process* sourceHead, int quantum) {
    struct Process* currentList = copyProcessList(sourceHead); 
    
    struct Process* curr;
    int time = 0;
    int completed = 0;
    int n = 0;
    
    curr = currentList;
    while (curr != NULL) { n++; curr = curr->next; }

    printf("\n--- Round Robin Scheduling (Q=%d) ---\n", quantum);
    printf("Gantt Chart:\n|");

    while (completed < n) {
        int executed_in_pass = 0; // Tracks if any process ran in this pass
        curr = currentList;
        
        // Loop through the list (simulating the circular queue)
        while (curr != NULL) {
            if (curr->remaining > 0 && curr->arrival <= time) {
                // Calculate the time slice: min(quantum, remaining time)
                int run_time = (curr->remaining > quantum) ? quantum : curr->remaining;

                // Record start time only on the very first execution
                if (curr->remaining == curr->burst) {
                    curr->start = time;
                }

                printf(" P%d ", curr->pid);
                
                // Update metrics
                curr->remaining -= run_time;
                time += run_time;
                executed_in_pass = 1;

                // Check for completion
                if (curr->remaining == 0) {
                    curr->finish = time;
                    // TAT and WT are calculated fully here
                    completed++;
                }
            }
            curr = curr->next;
        }
        
        // Handle CPU idle time: if no process ran in the pass, advance time
        if (!executed_in_pass) {
            int next_arrival = 99999;
            curr = currentList;
            while(curr != NULL) {
                if (curr->remaining > 0 && curr->arrival < next_arrival) {
                    next_arrival = curr->arrival;
                }
                curr = curr->next;
            }
            // Advance time to the arrival of the next process, but only if it's in the future
            if (next_arrival != 99999 && next_arrival > time) time = next_arrival;
            else time++; // Prevent infinite loop if all processes are somehow invalid/finished
        }
    }

    // Print the timeline numbers (simplified for RR)
    printf("|\n0...%d\n", time);

    displayResult(currentList, "Round Robin");
    freeProcessList(currentList);
}

// -------------------- Main Function --------------------
int main() {
    int n, choice, i, arrival, burst, priority, quantum;

    printf("Enter number of processes: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    for (i = 0; i < n; i++) {
        printf("\nProcess %d Arrival Time: ", i+1);
        if (scanf("%d", &arrival) != 1) return 1;
        printf("Process %d Burst Time: ", i+1);
        if (scanf("%d", &burst) != 1) return 1;
        printf("Process %d Priority (lower = higher): ", i+1);
        if (scanf("%d", &priority) != 1) return 1;

        appendProcess(&head, createProcess(i+1, arrival, burst, priority));
    }

    do {
        printf("\n\n------ CPU Scheduling Simulator ------\n");
        printf("1. FCFS\n2. SJF (Non-preemptive)\n3. Priority Scheduling (Non-preemptive)\n4. Round Robin (Preemptive)\n5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear input buffer
            choice = 0; // Force retry
            printf("Invalid input! Try again.\n");
            continue;
        }

        switch (choice) {
            case 1: fcfs(head); break;
            case 2: sjf(head); break;
            case 3: priorityScheduling(head); break;
            case 4:
                printf("Enter time quantum: ");
                if (scanf("%d", &quantum) != 1 || quantum <= 0) {
                    printf("Invalid quantum. Defaulting to 1.\n");
                    quantum = 1;
                }
                roundRobin(head, quantum);
                break;
            case 5: 
                printf("Exiting...\n"); 
                freeProcessList(head); // Free the original list before exit
                head = NULL; 
                exit(0);
            default: printf("Invalid choice!\n");
        }
    } while (choice != 5);

    return 0;
}
