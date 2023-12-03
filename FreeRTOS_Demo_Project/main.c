#include "FreeRTOSHelper.h"

#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#define BUFFER_SIZE 200
#define VALUE_MAX_SIZE 200
#define DELAY_MS 0.5

// Memory allocation 
uint32_t volatile buffer[BUFFER_SIZE];

// Semaphore (synchronization) 
// Used to protect access to shared resource rnd_loc and buffer
SemaphoreHandle_t semaphore;

// Buffer variables
uint8_t volatile rnd_loc = 0;
volatile bool isNewDataAvail = FALSE;

void vProducerTask()
{
	srand((unsigned int)time(NULL));
	while (1)
	{
		if (xSemaphoreTake(semaphore, portMAX_DELAY))
		{
			// Generate random number between 0 and (len-1 = 199)
			uint32_t rnd_val = rand() % VALUE_MAX_SIZE;
			rnd_loc = rand() % BUFFER_SIZE;

			if (rnd_loc < 0) rnd_loc = 0;
			if (rnd_loc >= BUFFER_SIZE) rnd_loc = BUFFER_SIZE - 1;

			// Write value to buffer
			buffer[rnd_loc] = rnd_val;
			printf("Writing to position %d value: %d \n", rnd_loc, rnd_val);

			// Set the flag
			isNewDataAvail = TRUE;

			// Release the semaphore
			xSemaphoreGive(semaphore);
		}
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
	}
}

void vConsumerTask()
{
	while (1)
	{
		if (xSemaphoreTake(semaphore, portMAX_DELAY))
		{
			if (isNewDataAvail)
			{
				printf("Reading from position %d value: %d\n", rnd_loc, buffer[rnd_loc]);
				// Clean set of position in the buffer
				rnd_loc = 0;

				// Clear the flag
				isNewDataAvail = FALSE;
			}
		
			// Release the semaphore
			xSemaphoreGive(semaphore);
		}
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
	}
}

int main(void) {
	// Heap initialization
	prvInitialiseHeap();

	// Trace initializatin
	configASSERT(xTraceInitialize() == TRC_SUCCESS);

	// Clear the memory and set variables
	memset(buffer, 0, sizeof(buffer));
	rnd_loc = 0;
	isNewDataAvail = FALSE;

	// Create semaphore
	semaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(semaphore);

	xTaskCreate(vProducerTask, "Producer Task", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);
	xTaskCreate(vConsumerTask, "Consumer Task", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);

	vTaskStartScheduler();
	return 0;
}