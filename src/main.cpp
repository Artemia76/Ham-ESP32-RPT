#include <Arduino.h>
#include "app.hpp"

CApp* Application;

/**
 * @brief Initialize MCU
 * 
 */
void setup(void)
{
  Application = CApp::Create();
}

/**
 * @brief Main loop of MCU
 * 
 */
void loop()
{
  Application->Loop();
}