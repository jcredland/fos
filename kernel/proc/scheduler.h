/*
 * System V (iirc) had a nice synchronization tool (a sort of smart spinlock) for multiprocessors that allowed a thread to busy-wait for a resource as long as the resource holder was active on another CPU and to enter the SLEEP state when the holder wasn't running.
 * Round Robin is the simplest algorithm for a preemptive scheduler. Only a single queue of processes is used. When the system timer fires, the next process in the queue is switched to, and the preempted process is put back into the queue.
 */
