/**
 * @file webserver.hpp
 * @author Artemia
 * @brief Test-ESP32 Project
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <vector>
#include <regex>

#include "env.h"
#include "log.hpp"

// Use atomic to share async mutlithreaded 

class CWebServer;

/**
 * @brief Abstract class for process callback
 * 
 */
class CWebServerEvent
{
friend class CWebServer;
public:
 /**
     * @brief Construct a new CWebServerEvent object
     * 
     * @param pWebServer 
     */
    CWebServerEvent(CWebServer& pWebServer);

    /**
     * @brief Destroy the CWebServerEvent object
     * 
     */
    ~CWebServerEvent();

protected:

    /**
     * @brief Process HTTP GET CommandEvent Callback
     * 
     * @param pCommand 
     */
virtual String onGET(const String& pCommand)=0;

    /**
     * @brief Process HTTP POST CommandEvent Callback
     * 
     * @param pCommand POST Command received
     * @param pData Data received 
     */
virtual void onPOST(const String& pCommand, const String& pData)=0;

private:
    /**
     * @brief Store event controller
     * 
     */
    CWebServer&  _webServer;
};

/**
 * @brief Container for event clients
 * 
 */
typedef std::vector<CWebServerEvent*> v_subscribers;

/**
 * @brief WebServer to serv frontend HTML website 
 * 
 * 
 */
class CWebServer 
{
friend class CWebServerEvent;
public:
    /**
     * @brief Construct a new CWebServer object
     * 
     */
    CWebServer (CLog& pLog);

    /**
     * @brief Destroy the CWebServer object
     * 
     */
    ~CWebServer ();

protected:
    void _onGetON (AsyncWebServerRequest *request);
    void _onGetOFF (AsyncWebServerRequest *request);
    void _onGetLuminosite (AsyncWebServerRequest *request);
    void _onGetSet (AsyncWebServerRequest *request);

private:

    /**
     * @brief Suscribers container for loop event
     * 
     */
    v_subscribers _subscribers;

    /**
     * @brief Web Server 
     * 
     */
    AsyncWebServer _server;

    CLog& _log;

    // Replace with your network credentials
    const char* _ssid;
    const char* _password;

        /**
     * @brief Suscribe to webserver events
     * 
     * @param pSubscriber 
     */
    void _subscribe (CWebServerEvent* pSubscriber);

    /**
     * @brief Unsuscribe to webserver events
     * 
     * @param pSubscriber 
     */
    void _unSubscribe (CWebServerEvent* pSubscriber);
};

#endif