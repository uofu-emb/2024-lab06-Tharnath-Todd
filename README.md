# Lab 6. Scheduling

### Activity 0
Induce priority inversion.

1. Create two preemptable threads (and a supervisor to manage them).
1. Create a semaphore shared between the two. Create it with xSemaphoreCreateBinary.
1. Set one thread to higher priority. set it to delay start.
1. Have the lower priority thread acquire the semphore first.
1. Predict the behavior of the system.

```
    #include <FreeRTOS.h>
    #include <task.h>
    #include <semphr.h>
    #include <stdio.h>

    // Semaphore handle
    SemaphoreHandle_t xBinarySemaphore;

    // Function prototypes
    void vTaskHighPriority(void *pvParameters);
    void vTaskLowPriority(void *pvParameters);
    void vTaskSupervisor(void *pvParameters);

    int main() {
        // Initialize the semaphore as a binary semaphore
        xBinarySemaphore = xSemaphoreCreateBinary();

        // Check if the semaphore was created successfully
        if (xBinarySemaphore == NULL) {
            printf("Failed to create semaphore.\n");
            while (1); // Halt
        }

        // Create the supervisor task
        xTaskCreate(vTaskSupervisor, "Supervisor", 1024, NULL, 3, NULL);

        // Start the FreeRTOS scheduler
        vTaskStartScheduler();

        // Will never reach here
        while (1);
        return 0;
    }

    // Supervisor task to create and manage other tasks
    void vTaskSupervisor(void *pvParameters) {
        // Create lower-priority task
        xTaskCreate(vTaskLowPriority, "LowPriorityTask", 1024, NULL, 1, NULL);

        // Delay to ensure the low-priority task acquires the semaphore first
        vTaskDelay(pdMS_TO_TICKS(100));

        // Create higher-priority task with delayed start
        xTaskCreate(vTaskHighPriority, "HighPriorityTask", 1024, NULL, 2, NULL);

        // Suspend supervisor task
        vTaskSuspend(NULL);
    }

    // Low-priority task
    void vTaskLowPriority(void *pvParameters) {
        while (1) {
            printf("Low Priority Task: Attempting to acquire semaphore...\n");
            // Acquire the semaphore
            if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY)) {
                printf("Low Priority Task: Acquired semaphore. Working...\n");
                // Simulate some work
                vTaskDelay(pdMS_TO_TICKS(2000));

                // Release the semaphore
                printf("Low Priority Task: Releasing semaphore.\n");
                xSemaphoreGive(xBinarySemaphore);
            }
        }
    }

    // High-priority task
    void vTaskHighPriority(void *pvParameters) {
        while (1) {
            printf("High Priority Task: Attempting to acquire semaphore...\n");
            // Try to acquire the semaphore
            if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY)) {
                printf("High Priority Task: Acquired semaphore. Working...\n");
                // Simulate some work
                vTaskDelay(pdMS_TO_TICKS(1000));

                // Release the semaphore
                printf("High Priority Task: Releasing semaphore.\n");
                xSemaphoreGive(xBinarySemaphore);
            }
        }
    }

```
#### Predicted Behavior
The high-priority task (vTaskHighPriority) will block and wait for the semaphore, even though it has a higher priority.
Without priority inheritance, a medium-priority task could preempt the low-priority task, causing further delays.

### Activity 1
Repeat the previous experiment, but this time create the semaphore with xSemaphoreCreateMutex

```
    #include <FreeRTOS.h>
    #include <task.h>
    #include <semphr.h>
    #include <stdio.h>

    // Mutex handle
    SemaphoreHandle_t xMutex;

    // Function prototypes
    void vTaskHighPriority(void *pvParameters);
    void vTaskLowPriority(void *pvParameters);
    void vTaskSupervisor(void *pvParameters);

    int main() {
        // Initialize the mutex
        xMutex = xSemaphoreCreateMutex();

        // Check if the mutex was created successfully
        if (xMutex == NULL) {
            printf("Failed to create mutex.\n");
            while (1); // Halt
        }

        // Create the supervisor task
        xTaskCreate(vTaskSupervisor, "Supervisor", 1024, NULL, 3, NULL);

        // Start the FreeRTOS scheduler
        vTaskStartScheduler();

        // Will never reach here
        while (1);
        return 0;
    }

    // Supervisor task to create and manage other tasks
    void vTaskSupervisor(void *pvParameters) {
        // Create lower-priority task
        xTaskCreate(vTaskLowPriority, "LowPriorityTask", 1024, NULL, 1, NULL);

        // Delay to ensure the low-priority task acquires the mutex first
        vTaskDelay(pdMS_TO_TICKS(100));

        // Create higher-priority task with delayed start
        xTaskCreate(vTaskHighPriority, "HighPriorityTask", 1024, NULL, 2, NULL);

        // Suspend supervisor task
        vTaskSuspend(NULL);
    }

    // Low-priority task
    void vTaskLowPriority(void *pvParameters) {
        while (1) {
            printf("Low Priority Task: Attempting to acquire mutex...\n");
            // Acquire the mutex
            if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
                printf("Low Priority Task: Acquired mutex. Working...\n");
                // Simulate some work
                vTaskDelay(pdMS_TO_TICKS(2000));

                // Release the mutex
                printf("Low Priority Task: Releasing mutex.\n");
                xSemaphoreGive(xMutex);
            }
        }
    }

    // High-priority task
    void vTaskHighPriority(void *pvParameters) {
        while (1) {
            printf("High Priority Task: Attempting to acquire mutex...\n");
            // Try to acquire the mutex
            if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
                printf("High Priority Task: Acquired mutex. Working...\n");
                // Simulate some work
                vTaskDelay(pdMS_TO_TICKS(1000));

                // Release the mutex
                printf("High Priority Task: Releasing mutex.\n");
                xSemaphoreGive(xMutex);
            }
        }
    }
```
#### Predicted Behavior

The low-priority task (vTaskLowPriority) acquires the mutex first.
When the high-priority task (vTaskHighPriority) attempts to acquire the mutex, it blocks, and the priority of the low-priority task is raised to match the high-priority task's.
The low-priority task completes its work and releases the mutex quickly.
The high-priority task acquires the mutex without undue delay.

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

Write tests for two threads running the following scenarios. Try to predict the runtime for each thread

### Scenario 1
1. Threads with same priority:
    1. Both run `busy_busy`.
#### Code
```
        run_analyzer(busy_busy, tskIDLE_PRIORITY+(3), 0, &first_stats,
             busy_busy, tskIDLE_PRIORITY+(3), 0, &second_stats,
             &elapsed_stats, &elapsed_ticks);
```

#### Behavior
- Both threads (busy_busy) run at the same priority without yielding.
- FreeRTOS doesn't preempt threads of equal priority unless they explicitly yield.
- The first thread created (first_stats) monopolizes the CPU indefinitely.
#### Prediction:
- Only the first thread runs (first_stats).
- The second thread (second_stats) gets very little or no runtime.
    1. Both run `busy_yield`
    1. One run `busy_busy` one run `busy_yield`
### Scenario 2
1. Threads with different priority.
    1. Both run `busy_busy`.
        1. Higher priority starts first.
        1. Lower priority starts first.
    1. Both run `busy_yield`.

Make sure you are setting priorities according to the priority order presented in the documentation.


