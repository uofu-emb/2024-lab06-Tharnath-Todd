#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

// Define task handles
TaskHandle_t highPriorityHandle = NULL;
TaskHandle_t lowPriorityHandle = NULL;

// Define the binary semaphore
SemaphoreHandle_t binarySemaphore;

// Helper function to create tasks and semaphore
void setupTasksAndSemaphore() {
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
    // Initialize FreeRTOS scheduler
    setupTasksAndSemaphore();

    // Start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (1);
}
