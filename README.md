# Lab 6. Scheduling

### Activity 0
Induce priority inversion.

1. create two preemptable threads (and a supervisor to manage them).
1. create a semaphore shared between the two. Create it with xSemaphoreCreateBinary.
1. set one thread to higher priority. set it to delay start.
1. have the lower priority thread acquire the semphore first.
1. Predict the behavior of the system.

You may find it useful to create a helper function to setup and teardown the threads.

### Activity 1
Repeat the previous experiment, but this time create the semaphore with xSemaphoreCreateMutex

### Activity 2
In this activity, you'll create two competing threads that use the following functions
```
void busy_busy(void)
{
    for (int i = 0; ; i++);
}

void busy_yield(void)
{
    for (int i = 0; ; i++) {
        taskYIELD();
    }
}
```
Write tests for two threads running the following scenarios. Try to predict the runtime for each thread.
1. Threads with same priority:
    1. Both run `busy_busy`.
    1. Both run `busy_yield`
    1. One run `busy_busy` one run `busy_yield`
1. Threads with different priority.
    1. Both run `busy_busy`.
        1. Higher priority starts first.
        1. Lower priority starts first.
    1. Both run `busy_yield`.

Make sure you are setting priorities according to the priority order presented in the documentation.

# Reference implementation
Reference implementation is located at https://github.com/uofu-emb/rtos.06
