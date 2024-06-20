# CPU-Scheduler
### Dependencies
1. **wxWidgets library** (version 3.x or later)
2. **C++ compiler** that supports C++11 standards (e.g., GCC, Clang, MSVC)

## Internal Working of This Project

### FCFS (First Come, First Served)
- Processes are executed in the order they arrive.
- Processes are sorted by their arrival time, and their start and end times are computed sequentially.

### Round Robin
- Processes are executed with a fixed time quantum.
- If a process cannot finish within the quantum, it's preempted and added back to the queue.

### Priority Scheduling
- Processes are scheduled based on their priority.
- Higher priority processes are executed before lower priority ones, with ties broken by arrival time.

### SJF (Shortest Job First)
- Processes are executed in order of their burst time (execution time).
- Shorter jobs are scheduled first, with ties broken by arrival time.

## Learning Takeaways from the Project
- Understanding and implementing different CPU scheduling algorithms.
- Practicing graphical representation using wxWidgets.
- Handling user input and managing process data structures.
