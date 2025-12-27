/**
 * @file webserver.cpp
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

#include <algorithm>
#include <LittleFS.h>
#include <AsyncTCP.h>

#include "webserver.hpp"
#include "env.hpp"

CWebServer::CWebServer () :
    _server (80),
    _ssid (WIFI_SSID),
    _password (WIFI_PASSWORD),
    _initialized(false),
    _counter(0)
{
    _log = CLog::Create();
    _log->Message ("Starting WebServer... ");
    int cnt = 0;
    if(!LittleFS.begin())
    {
        _log->Message ("LittleFS Mounting Error...");
        return;
    }

    File root =  LittleFS.open("/web");
    File file  = root.openNextFile();

    while(file)
    {
        _log->Message ("file: " + String(file.name()));
        file.close();
        file = root.openNextFile();
    }
    //Start WiFI
    WiFi.mode(WIFI_STA);
    WiFi.enableIPv6();
    WiFi.onEvent(CWebServer::WiFiEventCB);
    WiFi.begin(_ssid, _password);

    //--------------------------------------------SERVER
    _server.on("/",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(LittleFS, "/web/index.html", "text/html");
    });

    _server.on("/w3.css",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(LittleFS, "/web/w3.css", "text/css");
    });

    _server.on("/custom.css",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(LittleFS, "/web/custom.css", "text/css");
    });

    _server.on("/script.js",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(LittleFS, "/web/script.js", "text/javascript");
    });

    _server.on("/get",HTTP_POST, [this](AsyncWebServerRequest *request)
    {
        if (request->params() < 1) return;
        String Key = request->getParam(0)->name();
        String Param = request->getParam(0)->value();
        _log->Message ("Received get Key = " + Key + " : Param = " + Param, CLog::DEBUG);
        for (auto Subcriber : _subscribers)
        {
            if (Subcriber != nullptr)
            {
                String Result = Subcriber->onGet(Key, Param);
                if (!Result.isEmpty())
                {
                    request->send(200,"text/plain", Result); 
                    return;
                }
            }
        }
        request->send(404);
    });

    _server.on("/set",HTTP_POST, [this](AsyncWebServerRequest *request)
    {
        if (request->params() < 1) return;
        String Key = request->getParam(0)->name();
        String Param = request->getParam(0)->value();
        _log->Message ("Received set Key = " + Key + " : Param = " + Param, CLog::DEBUG);
        for (auto Subcriber : _subscribers)
        {
            if (Subcriber != nullptr)
            {
                Subcriber->onSet(Key, Param);
            }
        }
        request->send(200);
    });

    //
    // Setting Slow Timer
    //
    _t1s.setInterval(CWebServer::OnTimer1SCB,1000);

    _initialized = true;
}

/*****************************************************************************/

CWebServer::~CWebServer ()
{
    _server.end();
}

/*****************************************************************************/

void CWebServer::_subscribe(CWebServerEvent* pSubscriber)
{
    if (std::find(_subscribers.begin(), _subscribers.end(),pSubscriber) == _subscribers.end())
    {
        _subscribers.push_back(pSubscriber);
    }
}

/*****************************************************************************/

void CWebServer::_unSubscribe(CWebServerEvent* pSubscriber)
{
    _subscribers.erase(std::remove(_subscribers.begin(), _subscribers.end(), pSubscriber), _subscribers.end());
}

void CWebServer::WiFiEventCB(WiFiEvent_t event)
{
  CWebServer::Create()->WiFiEvent(event);
}

void CWebServer::WiFiEvent(WiFiEvent_t event)
{
    _log->Message(String(_counter) + ": ", false);
    _counter++;
    switch (event) 
    {
        case ARDUINO_EVENT_WIFI_STA_START:
        {
            _log->Message ("WiFi STA Started");
            break;
        }
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        {
            _log->Message ("WiFi Connected to SSID: " + String(WiFi.SSID()));
            _server.begin();
            _log->Message ("Server Online.");
            break;
        }
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        {
            IPAddress ip = WiFi.localIP();
            _log->Message ("IP address assigned: " + ip.toString());
            break;
        }
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        {
            IPAddress ipv6 = WiFi.STA.globalIPv6();
            _log->Message ("Global IPV6 address assigned: " + ipv6.toString());
            break;
        }
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        {
            _log->Message ("WiFi Disconnected from SSID: " + String(WiFi.SSID()));
            _server.end();
            WiFi.reconnect();
            break;
        }
        default:
            break;
    }
}
/*****************************************************************************/

void CWebServer::OnTimer1SCB()
{
  CWebServer::Create()->OnTimer1S();
}

/*****************************************************************************/

void CWebServer::OnTimer1S()
{
    _log->Message("Wifi RSSI = " + String(WiFi.RSSI()),CLog::VERBOSE);
}

/*****************************************************************************/

void CWebServer::OnUpdate()
{
    _t1s.handle();
}