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
#include <WiFi.h>

#include "webserver.hpp"
#include "env.hpp"
#include "usermanager.hpp"

CWebServer::CWebServer () :
    _server (80),
    _ssid (WIFI_SSID),
    _password (WIFI_PASSWORD),
    _initialized(false)
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
    _log->Message ("IP address: " + WiFi.localIP().toString());
    _log->Message ("Wifi Channel: " + String(WiFi.channel()));

    //--------------------------------------------SERVER
    _server.on("/login.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(LittleFS, "/web/login.html", "text/html");
    });

    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String token = "";
        if (request->hasHeader("Cookie")) {
            String cookie = request->header("Cookie");
            int start = cookie.indexOf("session=");
            if (start != -1) {
                token = cookie.substring(start + 8);
                int end = token.indexOf(';');
                if (end != -1) token = token.substring(0, end);
            }
        }

        UserRole role;
        if (!CUserManager::Create()->validateSession(token, role)) {
            request->redirect("/login.html");
            return;
        }
        request->send(LittleFS, "/web/index.html", "text/html");
    });

    _server.on("/auth/login", HTTP_POST, [](AsyncWebServerRequest *request) {
        String username = "";
        String password = "";
        if (request->hasParam("username", true)) username = request->getParam("username", true)->value();
        if (request->hasParam("password", true)) password = request->getParam("password", true)->value();

        String token;
        if (CUserManager::Create()->authenticate(username, password, token)) {
            AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"ok\"}");
            response->addHeader("Set-Cookie", "session=" + token + "; Path=/; HttpOnly");
            request->send(response);
        } else {
            request->send(401, "application/json", "{\"status\":\"error\",\"message\":\"Unauthorized\"}");
        }
    });

    _server.on("/auth/logout", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasHeader("Cookie")) {
            String cookie = request->header("Cookie");
            int start = cookie.indexOf("session=");
            if (start != -1) {
                String token = cookie.substring(start + 8);
                int end = token.indexOf(';');
                if (end != -1) token = token.substring(0, end);
                CUserManager::Create()->logout(token);
            }
        }
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"ok\"}");
        response->addHeader("Set-Cookie", "session=; Path=/; Max-Age=0");
        request->send(response);
    });

    _server.on("/auth/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String token = "";
        if (request->hasHeader("Cookie")) {
            String cookie = request->header("Cookie");
            int start = cookie.indexOf("session=");
            if (start != -1) {
                token = cookie.substring(start + 8);
                int end = token.indexOf(';');
                if (end != -1) token = token.substring(0, end);
            }
        }
        UserRole role;
        if (CUserManager::Create()->validateSession(token, role)) {
            request->send(200, "application/json", "{\"authenticated\":true,\"role\":\"" + CUserManager::Create()->roleToString(role) + "\"}");
        } else {
            request->send(200, "application/json", "{\"authenticated\":false}");
        }
    });

    _server.on("/admin/users", HTTP_GET, [](AsyncWebServerRequest *request) {
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role) || role != UserRole::ADMINISTRATOR) {
            request->send(403, "application/json", "{\"error\":\"Forbidden\"}");
            return;
        }
        request->send(200, "application/json", CUserManager::Create()->getUsersJson());
    });

    _server.on("/admin/users/add", HTTP_POST, [](AsyncWebServerRequest *request) {
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role) || role != UserRole::ADMINISTRATOR) {
            request->send(403, "application/json", "{\"error\":\"Forbidden\"}");
            return;
        }
        String username = request->getParam("username", true)->value();
        String password = request->getParam("password", true)->value();
        String roleStr = request->getParam("role", true)->value();
        UserRole targetRole = CUserManager::Create()->stringToRole(roleStr);
        
        if (CUserManager::Create()->addUser(username, password, targetRole)) {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\"}");
        }
    });

    _server.on("/admin/users/delete", HTTP_POST, [](AsyncWebServerRequest *request) {
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role) || role != UserRole::ADMINISTRATOR) {
            request->send(403, "application/json", "{\"error\":\"Forbidden\"}");
            return;
        }
        String username = request->getParam("username", true)->value();
        if (CUserManager::Create()->removeUser(username)) {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\"}");
        }
    });

    _server.on("/admin/config/export", HTTP_GET, [](AsyncWebServerRequest *request) {
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role) || role != UserRole::ADMINISTRATOR) {
            request->send(403, "application/json", "{\"error\":\"Forbidden\"}");
            return;
        }
        request->send(200, "application/json", CUserManager::Create()->exportConfig());
    });

    _server.on("/admin/config/import", HTTP_POST, [](AsyncWebServerRequest *request) {
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role) || role != UserRole::ADMINISTRATOR) {
            request->send(403, "application/json", "{\"error\":\"Forbidden\"}");
            return;
        }
        if (request->hasParam("config", true)) {
            String config = request->getParam("config", true)->value();
            if (CUserManager::Create()->importConfig(config)) {
                request->send(200, "application/json", "{\"status\":\"ok\"}");
            } else {
                request->send(400, "application/json", "{\"status\":\"error\"}");
            }
        }
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
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role)) {
            request->send(401);
            return;
        }
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
        UserRole role;
        if (!CUserManager::Create()->validateSession(CWebServer::GetToken(request), role) || role < UserRole::MODERATOR) {
            request->send(403);
            return;
        }
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
    _server.begin();

    //
    // Setting Slow Timer
    //
    _t1s.setInterval(CWebServer::OnTimer1SCB,1000);

    _initialized = true;
    _log->Message ("Server Online.");
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

String CWebServer::GetToken(AsyncWebServerRequest *request)
{
    if (request->hasHeader("Cookie")) {
        String cookie = request->header("Cookie");
        int start = cookie.indexOf("session=");
        if (start != -1) {
            String token = cookie.substring(start + 8);
            int end = token.indexOf(';');
            if (end != -1) token = token.substring(0, end);
            return token;
        }
    }
    return "";
}