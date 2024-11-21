#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



// Define task handles
TaskHandle_t highPriorityHandle = NULL;
TaskHandle_t lowPriorityHandle = NULL;

// Define the binary semaphore
SemaphoreHandle_t binarySemaphore;

// Helper function to create tasks and semaphore
void setupTasksAndSemaphore() {

    printf("I'm in setupTasksAndSemaphore \n");
    // Create the binary semaphore
    binarySemaphore = xSemaphoreCreateBinary();
    if (binarySemaphore == NULL) {
        printf("Failed to create semaphore.\n");
        while (1); // Stay here on failure
    }

    // Create the tasks
    xTaskCreate(highPriorityTask, "HighPriority", 256, NULL, 2, &highPriorityHandle);
    xTaskCreate(lowPriorityTask, "LowPriority", 256, NULL, 1, &lowPriorityHandle);

    // "Give" the semaphore to make it available for acquisition
    xSemaphoreGive(binarySemaphore);
}

// Task to simulate high-priority operation
void highPriorityTask(void *pvParameters) {
    // Delay to simulate a late start
    vTaskDelay(pdMS_TO_TICKS(200));

    printf("High-priority task trying to take the semaphore...\n");
    if (xSemaphoreTake(binarySemaphore, portMAX_DELAY) == pdTRUE) {
        printf("High-priority task acquired the semaphore!\n");
        // Simulate work
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("High-priority task releasing the semaphore.\n");
        xSemaphoreGive(binarySemaphore);
    }
    vTaskDelete(NULL);
}

// Task to simulate low-priority operation
void lowPriorityTask(void *pvParameters) {
    printf("Low-priority task trying to take the semaphore...\n");
    if (xSemaphoreTake(binarySemaphore, portMAX_DELAY) == pdTRUE) {
        printf("Low-priority task acquired the semaphore!\n");
        // Simulate long processing time
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Low-priority task releasing the semaphore.\n");
        xSemaphoreGive(binarySemaphore);
    }
    vTaskDelete(NULL);
}

// Main function
int main() {

    stdio_init_all();
    printf("stdio_init_all \n");
    int count = 0;
    while(count <150){
        printf("Hello World %d\n", count);
        count++:
        sleep_ms(100);
    }

    // Initialize FreeRTOS scheduler
    setupTasksAndSemaphore();

    // Start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (1);
}





// Declare the mutex
// SemaphoreHandle_t mutex;

// // Task prototypes
// void busy_busy(void *pvParameters);
// void busy_yield(void *pvParameters);

// // Task handles
// TaskHandle_t highPriorityTaskHandle;
// TaskHandle_t lowPriorityTaskHandle;

// int main(void) {
//      stdio_init_all();
//     // Create the mutex
//     mutex = xSemaphoreCreateMutex();

//     // Check if mutex creation was successful
//     if (mutex == NULL) {
//         printf("Failed to create mutex!\n");
//         return -1;
//     }

//     // Scenario 1: Same priority
//     // 1.i. Both threads run busy_busy
//     // xTaskCreate(busy_busy, "BusyBusy1", 1000, NULL, 1, NULL);
//     // xTaskCreate(busy_busy, "BusyBusy2", 1000, NULL, 1, NULL);

//     // 1.ii. Both threads run busy_yield
//     // xTaskCreate(busy_yield, "BusyYield1", 1000, NULL, 1, NULL);
//     // xTaskCreate(busy_yield, "BusyYield2", 1000, NULL, 1, NULL);

//     // 1.iii. One runs busy_busy, one runs busy_yield
//     //xTaskCreate(busy_busy, "BusyBusy", 1000, NULL, 1, NULL);
//     //xTaskCreate(busy_yield, "BusyYield", 1000, NULL, 1, NULL);

//     // Scenario 2: Different priority
//     // 2.i. Both threads run busy_busy
//     // Higher-priority starts first
//     // xTaskCreate(busy_busy, "BusyBusyHigh", 1000, NULL, 2, &highPriorityTaskHandle);
//     // xTaskCreate(busy_busy, "BusyBusyLow", 1000, NULL, 1, &lowPriorityTaskHandle);

//     // Lower-priority starts first
//    //  xTaskCreate(busy_busy, "BusyBusyLow", 1000, NULL, 1, &lowPriorityTaskHandle);
//    //  xTaskCreate(busy_busy, "BusyBusyHigh", 1000, NULL, 2, &highPriorityTaskHandle);

//     // 2.ii. Both threads run busy_yield
//     xTaskCreate(busy_yield, "BusyYieldHigh", 1000, NULL, 2, &highPriorityTaskHandle);
//     xTaskCreate(busy_yield, "BusyYieldLow", 1000, NULL, 1, &lowPriorityTaskHandle);
//     // Start the scheduler
//     vTaskStartScheduler();

//     // The program should never reach here
//     for (;;);
// }

// // Function: busy_busy
// void busy_busy(void *pvParameters) {
//     for (int i = 0;; i++) {
//         if (xSemaphoreTake(mutex, portMAX_DELAY)) {
//             printf("Busy busy thread acquired mutex\n");
//             // Simulate critical section
//             for (volatile int j = 0; j < 1000000; j++);
//             xSemaphoreGive(mutex);
//             // Yield to ensure other threads get CPU time
//             taskYIELD();
//         }
//     }
// }

// // Function: busy_yield
// void busy_yield(void *pvParameters) {
//     for (int i = 0;; i++) {
//         if (xSemaphoreTake(mutex, portMAX_DELAY)) {
//             printf("Busy yield thread acquired mutex\n");
//             // Simulate critical section
//             for (volatile int j = 0; j < 1000000; j++);
//             xSemaphoreGive(mutex);
//         }
//         taskYIELD(); // Yield to allow other threads to run
//     }
// }



// Declare a mutex
// SemaphoreHandle_t mutex;

// // Task prototypes
// void highPriorityTask(void *pvParameters);
// void lowPriorityTask(void *pvParameters);
// void supervisorTask(void *pvParameters);

// int main(void) {
//      stdio_init_all();
//     // Create the mutex
//     mutex = xSemaphoreCreateMutex();

//     // Check if mutex creation was successful
//     if (mutex == NULL) {
//         printf("Failed to create mutex!\n");
//         return -1;
//     }

//     // Create the tasks
//     xTaskCreate(lowPriorityTask, "LowPriority", 1000, NULL, 1, NULL);
//     xTaskCreate(highPriorityTask, "HighPriority", 1000, NULL, 2, NULL);
//     xTaskCreate(supervisorTask, "Supervisor", 1000, NULL, 3, NULL);

//     // Start the scheduler
//     vTaskStartScheduler();

//     // The program should never reach here
//     for (;;);
// }

// // Low-priority task
// void lowPriorityTask(void *pvParameters) {
//     for (;;) {
//         // Attempt to acquire the mutex
//         if (xSemaphoreTake(mutex, portMAX_DELAY)) {
//             printf("Low-priority task acquired mutex!\n");
//             // Simulate work
//             vTaskDelay(pdMS_TO_TICKS(2000));
//             printf("Low-priority task releasing mutex.\n");
//             xSemaphoreGive(mutex);
//         }
//         vTaskDelay(pdMS_TO_TICKS(500)); // Delay to allow for context switching
//     }
// }

// // High-priority task
// void highPriorityTask(void *pvParameters) {
//     // Delay the start
//     vTaskDelay(pdMS_TO_TICKS(1000));

//     for (;;) {
//         printf("High-priority task attempting to acquire mutex...\n");
//         if (xSemaphoreTake(mutex, portMAX_DELAY)) {
//             printf("High-priority task acquired mutex!\n");
//             // Simulate work
//             vTaskDelay(pdMS_TO_TICKS(1000));
//             printf("High-priority task releasing mutex.\n");
//             xSemaphoreGive(mutex);
//         }
//         vTaskDelay(pdMS_TO_TICKS(500)); // Delay to simulate periodic activity
//     }
// }

// // Supervisor task to observe behavior
// void supervisorTask(void *pvParameters) {
//     for (;;) {
//         printf("Supervisor monitoring system...\n");
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

// // Declare a binary semaphore
// // SemaphoreHandle_t binarySemaphore;

// // // Task prototypes
// // void highPriorityTask(void *pvParameters);
// // void lowPriorityTask(void *pvParameters);
// // void supervisorTask(void *pvParameters);

// // int main(void) {
// //     stdio_init_all();
// //     // Create the binary semaphore
// //     binarySemaphore = xSemaphoreCreateBinary();

// //     // Create the tasks
// //     xTaskCreate(lowPriorityTask, "LowPriority", 1000, NULL, 1, NULL);
// //     xTaskCreate(highPriorityTask, "HighPriority", 1000, NULL, 2, NULL);
// //     xTaskCreate(supervisorTask, "Supervisor", 1000, NULL, 3, NULL);

// //     // Start the scheduler
// //     vTaskStartScheduler();

// //     // The program should never reach here
// //     for (;;);
// // }

// // // Low-priority task
// // void lowPriorityTask(void *pvParameters) {
// //     for (;;) {
// //         if (xSemaphoreTake(binarySemaphore, portMAX_DELAY)) {
// //             printf("Low-priority task acquired semaphore!\n");
// //         }
// //     }
// // }

// // // High-priority task
// // void highPriorityTask(void *pvParameters) {
// //     // Delay the start
// //     vTaskDelay(pdMS_TO_TICKS(1000));

// //     for (;;) {
// //         printf("High-priority task giving semaphore...\n");
// //         xSemaphoreGive(binarySemaphore);
// //         vTaskDelay(pdMS_TO_TICKS(2000)); // Simulate work
// //     }
// // }

// // // Supervisor task to observe behavior
// // void supervisorTask(void *pvParameters) {
// //     for (;;) {
// //         printf("Supervisor task is monitoring...\n");
// //         vTaskDelay(pdMS_TO_TICKS(500));
// //     }
// // }


// // // int count = 0;
// // // bool on = false;

// // // #define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
// // // #define BLINK_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )
// // // #define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
// // // #define BLINK_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

// // // void blink_task(__unused void *params) {
// // //     hard_assert(cyw43_arch_init() == PICO_OK);
// // //     while (true) {
// // //         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
// // //         if (count++ % 11) on = !on;
// // //         vTaskDelay(250);
// // //     }
// // // }

// // // void main_task(__unused void *params) {
// // //     xTaskCreate(blink_task, "BlinkThread",
// // //                 BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
// // //     char c;
// // //     while(c = getchar()) {
// // //         if (c <= 'z' && c >= 'a') putchar(c - 32);
// // //         else if (c >= 'A' && c <= 'Z') putchar(c + 32);
// // //         else putchar(c);
// // //     }
// // // }

// // // int main( void )
// // // {
// // //     //printf("hello world");
// // //     stdio_init_all();
// // //      printf("hello world");
// // //     const char *rtos_name;
// // //     rtos_name = "FreeRTOS";
// // //     TaskHandle_t task;
// // //     xTaskCreate(main_task, "MainThread",
// // //                 MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);
// // //     vTaskStartScheduler();
// // //     return 0;
// // // }
