#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "semphr.h"
#define Button 6
#define LEDGreen 7
#define LEDRed 8
void TaskLEDRed();
void TaskLEDGreen();
void TaskButton();
SemaphoreHandle_t xBinarySemaphore;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up :)");
  pinMode(Button, INPUT_PULLUP);
  pinMode(LEDGreen, OUTPUT);
  pinMode(LEDRed, OUTPUT);
  xTaskCreate(TaskLEDRed, "RedOn", 100, NULL, 1, NULL);
  xTaskCreate(TaskLEDGreen, "GreenOn", 100, NULL, 1, NULL);
  xTaskCreate(TaskButton, "ButtonPress", 100, NULL, 2, NULL);
  xBinarySemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xBinarySemaphore);
}
void loop() {}

void TaskLEDRed()
{
  // Tager Semaphore, tænder cirka 333 ms og slukker cirka 777 ms, giver slip på semaphore
  while (true)
  {
    if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
    {
      digitalWrite(LEDRed, HIGH);
      Serial.println("Red on");
      vTaskDelay(22);
      digitalWrite(LEDRed, LOW);
      Serial.println("Red off");
      vTaskDelay(52);
      xSemaphoreGive(xBinarySemaphore);
    }
  }
}
void TaskLEDGreen()
{
  // blinker cirka 500ms tænd og sluk
  while (true)
  {
    digitalWrite(LEDGreen, HIGH);
    Serial.println("green on");
    vTaskDelay(33);
    digitalWrite(LEDGreen, LOW);
    Serial.println("green off");
    vTaskDelay(33);
  }
}
void TaskButton()
{
  // Hvis der bliver trykket på knappen sættes en toggle bool.
  // hvis toggle er true, og der ikke er taget en semaphore, så tages semaphore
  // hvis toggle er false, og der er taget en semaphore, så gives semaphore
  bool buttonToggle = false;
  bool semaphoreTaken = false;
  while (true)
  {
    vTaskDelay(1);
    if (digitalRead(Button) == 0)
    {
      buttonToggle = !buttonToggle;
      vTaskDelay(10);
      if (buttonToggle == true)
      {
        if (semaphoreTaken == false)
        {
          xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
          semaphoreTaken = true;
        }
      }
      else
      {
        if (semaphoreTaken == true)
        {
          xSemaphoreGive(xBinarySemaphore);
          semaphoreTaken = false;
        }
      }
    }
  }
}
