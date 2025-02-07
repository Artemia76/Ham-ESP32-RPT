#include <Arduino.h>
#include "app.hpp"
#include "webserver.hpp"
#include "log.hpp"

CLog* Logger;
CWebServer* WebServer;
CApp* Application;

/**
 * @brief Initialize MCU
 * 
 */
void setup(void)
{
  Logger = new CLog();
  WebServer = new CWebServer(*Logger);
  Application = new CApp (*Logger, *WebServer);
}

/**
 * @brief Main loop of MCU
 * 
 */
void loop()
{
  Application->Loop();
}