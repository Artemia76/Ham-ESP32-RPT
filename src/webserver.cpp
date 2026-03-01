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
    _t1s(1000,std::bind(&CWebServer::OnTimer1S, this)),
    _t30s(30000,std::bind(&CWebServer::OnTimer30S, this))
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

    // Setup Wifi
    WiFi.disconnect(true);
    delay (1000);

    WiFi.onEvent(CB_WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(CB_WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(CB_WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    WiFi.begin(_ssid, _password);
    _log->Message ("Connecting to " + String(_ssid) + " ...");

    /*while (WiFi.status() != WL_CONNECTED)
    {
        _log->Message (".", false);
        delay(500);
        cnt++;
        if (cnt > 10)
        {
            _log->Message("Failed to connect WiFi");
            return;
        }

    // Print local IP address and start web server
    _log->Message ("\n");
    _log->Message ("WiFi connected.");
    _log->Message ("IP address: " + WiFi.localIP().toString());
    _log->Message ("Wifi Channel: " + String(WiFi.channel()));
    */

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
        _log->Message ("Received get Key = " + Key + " : Value = " + Param, CLog::DEBUG);
        for (auto Subcriber : _subscribers)
        {
            if (Subcriber != nullptr)
            {
                String Value = Subcriber->onGet(Key, Param);
                if (!Value.isEmpty())
                {
                    request->send(200,"text/plain", Value); 
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
        _log->Message ("Received set Key = " + Key + " : Value = " + Param, CLog::DEBUG);
        for (auto Subcriber : _subscribers)
        {
            if (Subcriber != nullptr)
            {
                Subcriber->onSet(Key, Param);
            }
        }
        request->send(200);
    });
    //_server.begin();

    _initialized = true;
    //_log->Message ("Server Online.");
}

/*****************************************************************************/

CWebServer::~CWebServer ()
{
    _server.end();
}

/*************************************************************************** */
void CWebServer::CB_WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    CWebServer::Create()->WiFiStationConnected(event, info);
}

/*************************************************************************** */
void CWebServer::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    _log->Message ("WiFi connected.");
}

/*************************************************************************** */
void CWebServer::CB_WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    CWebServer::Create()->WiFiGotIP(event, info);
}

/*************************************************************************** */
void CWebServer::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    _log->Message ("IP address: " + WiFi.localIP().toString());
    _log->Message ("Wifi Channel: " + String(WiFi.channel()));
    _log->Message ("Restarting Web Server.");
    _server.begin();
}

/*************************************************************************** */
void CWebServer::CB_WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    CWebServer::Create()->WiFiStationDisconnected(event, info);
}

/*************************************************************************** */
void CWebServer::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    _log->Message ("WiFi lost connection. Reason: "+ String(info.wifi_sta_disconnected.reason));
    WiFi.disconnect(true);
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

/*****************************************************************************/

void CWebServer::OnTimer30S()
{
    if (!WiFi.isConnected())
    {
        _log->Message ("WiFi Trying to reconnect...");
        WiFi.begin(_ssid, _password);
    }
}

/*****************************************************************************/

void CWebServer::OnTimer1S()
{
    _log->Message("Wifi RSSI = " + String(WiFi.RSSI()),CLog::VERBOSE);
}

/*****************************************************************************/

void CWebServer::OnUpdate()
{
    _t1s.Update();
    _t30s.Update();
}