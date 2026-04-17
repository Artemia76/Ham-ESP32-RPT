/**
 * @file usermanager.hpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief User Management for HAM-ESP32-RPT
 * @version 0.1
 * @date 2025-02-13
 */

#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>
#include "singleton.hpp"
#include "log.hpp"

enum class UserRole {
    USER = 0,
    MODERATOR = 1,
    ADMINISTRATOR = 2
};

struct User {
    String username;
    String passwordHash;
    String salt;
    UserRole role;
};

class CUserManager : public CSingleTon<CUserManager> {
    friend class CSingleTon<CUserManager>;

public:
    bool authenticate(const String& username, const String& password, String& sessionToken);
    bool validateSession(const String& sessionToken, UserRole& role);
    void logout(const String& sessionToken);

    // User management (Admin only)
    bool addUser(const String& username, const String& password, UserRole role);
    bool removeUser(const String& username);
    bool updateUserRole(const String& username, UserRole role);
    bool updateUserPassword(const String& username, const String& newPassword);
    
    String getUsersJson();
    bool importConfig(const String& jsonContent);
    String exportConfig();

    String roleToString(UserRole role);
    UserRole stringToRole(const String& roleStr);

private:
    CUserManager();
    ~CUserManager();

    void loadUsers();
    void saveUsers();
    String hashPassword(const String& password, const String& salt);
    String generateRandomString(int length);

    std::vector<User> _users;
    std::map<String, std::pair<String, UserRole>> _sessions; // token -> {username, role}
    CLog* _log;
    const char* _usersFile = "/users.json";
};

#endif // USERMANAGER_HPP
