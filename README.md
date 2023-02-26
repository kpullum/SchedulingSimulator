# SchedulingSimulator
 C++ program for testing and visualizing 4 different process scheduling algorithms:

 - First Come First Serve: each process is executed in the order of time of arrival (earliest to latest, ignores priority)
 - Shortest Job First: each process is executed in order of the length of time it requires to be completed (ignores priority)
 - Preemptive Priority: each process is executed in order of priority, and  a process can be preempted/interrupted by a higher priority process arriving in the middle of its execution
 - Round Robin: processes are executed in order of arrival time, but every n seconds (time quantum) the current process is preempted by the next-in-queue
 
 The program reads data from the input.txt file, where each process consist of 4 integer attributes (in order, separated by single space):
 
 - Chronological process ID (base 1, not 0)
 - Arrival time (time at which the process enters the ready queue)
 - Burst time (how many time units the process requires to fully execute)
 - Priority (higher numbers take priority in execution over lower numbers, only used for preemptive algorithm)
 
 Once run, the program will prompt the user to decide which algorithm to test. After obtaining the input (txt file must be in same directory), the program will print out the current process executing at each time unit, and how much longer that process will take to complete. The program will also log whenever the current process changes, or is completed (for this implementation, a new process will start at the same time unit that another process finishes). See sampleoutput.txt for more information