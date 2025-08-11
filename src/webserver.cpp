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
#include <Arduino.h>
#include "webserver.hpp"

/*****************************************************************************/

CWebServerEvent::CWebServerEvent()
{
    _webServer = CWebServer::Create();
    _webServer->_subscribe(this);
}

/*****************************************************************************/

CWebServerEvent::~CWebServerEvent()
{
    _webServer->_unSubscribe(this);
}

/*****************************************************************************/

CWebServer::CWebServer () :
    _server (80),
    _ssid (WIFI_SSID),
    _password (WIFI_PASSWORD),
    _initialized(false)
{
    _log = CLog::Create();
    _log->Message ("Starting WebServer... ");
    int cnt = 0;
    if(!SPIFFS.begin())
    {
        _log->Message ("SPIFFS Mounting Error...");
        return;
    }

    File root =  SPIFFS.open("/web");
    File file  = root.openNextFile();

    while(file)
    {
        _log->Message ("file: " + String(file.name()));
        file.close();
        file = root.openNextFile();
    }

    WiFi.begin(_ssid, _password);
    _log->Message ("Connecting to " + String(_ssid));

    while (WiFi.status() != WL_CONNECTED)
    {
        _log->Message (".", false);
        delay(500);
        cnt++;
        if (cnt > 10)
        {
            _log->Message("Failed to connect WiFi");
            return;
        }
    }

    // Print local IP address and start web server
    _log->Message ("\n");
    _log->Message ("WiFi connected.");
    _log->Message ("IP address: ");
    _log->Message (WiFi.localIP().toString());

    //--------------------------------------------SERVER
    _server.on("/",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/web/index.html", "text/html");
    });

    _server.on("/w3.css",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/web/w3.css", "text/css");
    });

    _server.on("/custom.css",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/web/custom.css", "text/javascript");
    });

    _server.on("/script.js",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/web/script.js", "text/javascript");
    });

    _server.on("/getRSSI",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetRSSI (request);
    });

    _server.on("/RepOn",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetRepOn (request);
    });

    _server.on("/RepOff",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetRepOff (request);
    });

    _server.on("/CTCSSOn",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetCTCSSOn (request);
    });

    _server.on("/CTCSSOff",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetCTCSSOff (request);
    });

    _server.on("/set",HTTP_POST, [this](AsyncWebServerRequest *request)
    {
        this->_onGetSet (request);
    });
    _server.begin();
    _initialized = true;
    _log->Message ("Server Online.");
}

/*****************************************************************************/

CWebServer::~CWebServer ()
{
    _server.end();
}

/*****************************************************************************/

void CWebServer::_onGetRepOn (AsyncWebServerRequest *request)
{
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onGET("RepOn");
        }
    }
    request->send(200);
    _log->Message ("Received ON.", true , CLog::Level::DEBUG);
}

/*****************************************************************************/

void CWebServer::_onGetRepOff (AsyncWebServerRequest *request)
{
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onGET("RepOff");
        }
    }
    request->send(200);
    _log->Message ("Received RepOff.", true , CLog::Level::DEBUG);
}

/*****************************************************************************/

void CWebServer::_onGetCTCSSOn (AsyncWebServerRequest *request)
{
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onGET("CTCSSOn");
        }
    }
    request->send(200);
    _log->Message ("Received CTCSSOn.", true, CLog::Level::DEBUG);
}

/*****************************************************************************/

void CWebServer::_onGetCTCSSOff (AsyncWebServerRequest *request)
{
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onGET("CTCSSOff");
        }
    }
    request->send(200);
    _log->Message ("Received CTCSSOff.", true, CLog::Level::DEBUG);
}

/*****************************************************************************/

void CWebServer::_onGetRSSI (AsyncWebServerRequest *request)
{
    String Value;
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Value= Subcriber->onGET("RSSI");
            if (!Value.isEmpty())
            {
                request->send(200,"text/plain", Value); 
                _log->Message ("Received Get RSSI = " + Value, true, CLog::Level::DEBUG);
                return;
            }
        }
    }
}

/*****************************************************************************/

void CWebServer::_onGetSet (AsyncWebServerRequest *request)
{
    int paramsNr = request->params();
    if (request->hasParam("rgb"))
    {

    }
    const AsyncWebParameter * j = request->getParam("rgb"); // 1st parameter
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onPOST("set",j->value());
        }
    }
    request->send(200);

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