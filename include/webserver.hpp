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
#include <vector>
//#include <regex>
#include <AsyncTimer.h>
#include <WiFi.h>

#include "appevent.hpp"
#include "webserverevent.hpp"
#include "log.hpp"
#include "singleton.hpp"


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
class CWebServer : public CSingleTon<CWebServer>, CAppEvent
{
    friend class CWebServerEvent;
    friend class CSingleTon<CWebServer>;
public:


protected:

    /**
    * @brief Background Update event 
    * 
    */
    void OnUpdate();

static void WiFiEventCB(WiFiEvent_t event);
    void WiFiEvent(WiFiEvent_t event);

private:
    int _counter;
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

    AsyncTimer _t1s;

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
    void OnTimer1S ();
    static void OnTimer1SCB ();
};

#endif //WEBSERVER_HPP