/**
 * @file webserver.hpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
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

#include "env.hpp"
#include "log.hpp"
#include "singleton.hpp"

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
    CWebServerEvent();

    /**
     * @brief Destroy the CWebServerEvent object
     * 
     */
    ~CWebServerEvent();

protected:

virtual String onGET(const String& pCommand)=0;

    /**
     * @brief Process HTTP GET CommandEvent Callback
     * 
     * @param pCommand // #define DEBUG
     */
virtual void onPOST(const String& pCommand, const String& pData)=0;

private:
    /**
     * @brief Store event controller
     * 
     */
    CWebServer*  _webServer;
};

/**
 * @brief Container for event clients
 * 
 */
typedef std::vector<CWebServerEvent*> v_WebServerSubscribers;

/**
 * @brief WebServer to serv frontend HTML website 
 * 
 * 
 */
class CWebServer : public CSingleTon<CWebServer>
{
friend class CWebServerEvent;
friend class CSingleTon<CWebServer>;
public:


protected:
    void _onGetRepOn (AsyncWebServerRequest *request);
    void _onGetRepOff (AsyncWebServerRequest *request);
    void _onGetCTCSSOn (AsyncWebServerRequest *request);
    void _onGetCTCSSOff (AsyncWebServerRequest *request);
    void _onGetRSSI (AsyncWebServerRequest *request);
    void _onGetSet (AsyncWebServerRequest *request);

private:

    /**
     * @brief Construct a new CWebServer object
     * 
     */
    CWebServer ();

    /**
     * @brief Destroy the CWebServer object
     * 
     */
    ~CWebServer ();

    /**
     * @brief Suscribers container for loop event
     * 
     */
    v_WebServerSubscribers _subscribers;

    /**
     * @brief Web Server 
     * 
     */
    AsyncWebServer _server;

    /**
     * @brief Log Controller
     * 
     */
    CLog* _log;

    // Wifi credential
    const char* _ssid;
    const char* _password;


    // Init State
    bool _initialized;
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