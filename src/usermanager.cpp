/**
 * @file usermanager.cpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief Implementation of User Management
 */

#include "usermanager.hpp"
#include <LittleFS.h>
#include <mbedtls/sha256.h>

CUserManager::CUserManager() {
    _log = CLog::Create();
    loadUsers();
}

CUserManager::~CUserManager() {}

void CUserManager::loadUsers() {
    if (!LittleFS.exists(_usersFile)) {
        _log->Message("Users file not found, creating default admin account.");
        addUser("admin", "admin", UserRole::ADMINISTRATOR);
        return;
    }

    File file = LittleFS.open(_usersFile, "r");
    if (!file) {
        _log->Message("Failed to open users file for reading.");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        _log->Message("Failed to parse users file: " + String(error.c_str()));
        return;
    }

    JsonArray array = doc.as<JsonArray>();
    _users.clear();
    for (JsonObject obj : array) {
        User user;
        user.username = obj["username"].as<String>();
        user.passwordHash = obj["passwordHash"].as<String>();
        user.salt = obj["salt"].as<String>();
        user.role = (UserRole)obj["role"].as<int>();
        _users.push_back(user);
    }
    _log->Message("Loaded " + String(_users.size()) + " users.");
}

void CUserManager::saveUsers() {
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (const auto& user : _users) {
        JsonObject obj = array.add<JsonObject>();
        obj["username"] = user.username;
        obj["passwordHash"] = user.passwordHash;
        obj["salt"] = user.salt;
        obj["role"] = (int)user.role;
    }

    File file = LittleFS.open(_usersFile, "w");
    if (!file) {
        _log->Message("Failed to open users file for writing.");
        return;
    }

    if (serializeJson(doc, file) == 0) {
        _log->Message("Failed to write to users file.");
    }
    file.close();
}

String CUserManager::hashPassword(const String& password, const String& salt) {
    String input = password + salt;
    unsigned char hash[32];
    mbedtls_sha256((const unsigned char*)input.c_str(), input.length(), hash, 0);

    String hexHash = "";
    for (int i = 0; i < 32; i++) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        hexHash += buf;
    }
    return hexHash;
}

String CUserManager::generateRandomString(int length) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    String result = "";
    for (int i = 0; i < length; i++) {
        result += charset[random(sizeof(charset) - 1)];
    }
    return result;
}

bool CUserManager::authenticate(const String& username, const String& password, String& sessionToken) {
    for (const auto& user : _users) {
        if (user.username == username) {
            String hash = hashPassword(password, user.salt);
            if (hash == user.passwordHash) {
                sessionToken = generateRandomString(32);
                _sessions[sessionToken] = {username, user.role};
                _log->Message("User " + username + " authenticated.");
                return true;
            }
        }
    }
    _log->Message("Authentication failed for user: " + username);
    return false;
}

bool CUserManager::validateSession(const String& sessionToken, UserRole& role) {
    if (_sessions.count(sessionToken)) {
        role = _sessions[sessionToken].second;
        return true;
    }
    return false;
}

void CUserManager::logout(const String& sessionToken) {
    _sessions.erase(sessionToken);
}

bool CUserManager::addUser(const String& username, const String& password, UserRole role) {
    for (const auto& user : _users) {
        if (user.username == username) return false;
    }

    User newUser;
    newUser.username = username;
    newUser.salt = generateRandomString(16);
    newUser.passwordHash = hashPassword(password, newUser.salt);
    newUser.role = role;
    _users.push_back(newUser);
    saveUsers();
    return true;
}

bool CUserManager::removeUser(const String& username) {
    if (username == "admin") return false; // Prevent removing default admin
    for (auto it = _users.begin(); it != _users.end(); ++it) {
        if (it->username == username) {
            _users.erase(it);
            saveUsers();
            return true;
        }
    }
    return false;
}

bool CUserManager::updateUserRole(const String& username, UserRole role) {
    for (auto& user : _users) {
        if (user.username == username) {
            user.role = role;
            saveUsers();
            return true;
        }
    }
    return false;
}

bool CUserManager::updateUserPassword(const String& username, const String& newPassword) {
    for (auto& user : _users) {
        if (user.username == username) {
            user.salt = generateRandomString(16);
            user.passwordHash = hashPassword(newPassword, user.salt);
            saveUsers();
            return true;
        }
    }
    return false;
}

String CUserManager::getUsersJson() {
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    for (const auto& user : _users) {
        JsonObject obj = array.add<JsonObject>();
        obj["username"] = user.username;
        obj["role"] = roleToString(user.role);
    }
    String output;
    serializeJson(doc, output);
    return output;
}

bool CUserManager::importConfig(const String& jsonContent) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonContent);
    if (error) return false;

    JsonArray array = doc.as<JsonArray>();
    _users.clear();
    for (JsonObject obj : array) {
        User user;
        user.username = obj["username"].as<String>();
        user.passwordHash = obj["passwordHash"].as<String>();
        user.salt = obj["salt"].as<String>();
        user.role = (UserRole)obj["role"].as<int>();
        _users.push_back(user);
    }
    saveUsers();
    return true;
}

String CUserManager::exportConfig() {
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    for (const auto& user : _users) {
        JsonObject obj = array.add<JsonObject>();
        obj["username"] = user.username;
        obj["passwordHash"] = user.passwordHash;
        obj["salt"] = user.salt;
        obj["role"] = (int)user.role;
    }
    String output;
    serializeJson(doc, output);
    return output;
}

String CUserManager::roleToString(UserRole role) {
    switch (role) {
        case UserRole::ADMINISTRATOR: return "Administrateur";
        case UserRole::MODERATOR: return "Modérateur";
        case UserRole::USER: return "Utilisateur";
        default: return "Inconnu";
    }
}

UserRole CUserManager::stringToRole(const String& roleStr) {
    if (roleStr == "Administrateur") return UserRole::ADMINISTRATOR;
    if (roleStr == "Modérateur") return UserRole::MODERATOR;
    return UserRole::USER;
}
