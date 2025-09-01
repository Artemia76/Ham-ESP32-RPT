/**
 * @file app.hpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
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

#ifndef APP_HPP
#define APP_HPP

#include <Arduino.h>
#include "appevent.hpp"
#include "webserver.hpp"
#include "log.hpp"

#include <vector>
#include "singleton.hpp"


/**
 * @brief Container for event clients
 * 
 */
typedef std::vector<CAppEvent*> v_CAppSubscribers;

/**
 * @brief Main application class
 * @details This singleton is used to call class derivated from CAppEvent in main loop
 */
class CApp : public CSingleTon<CApp>
{
    friend class CSingleTon<CApp>;
    friend class CAppEvent;
public:

    /**
     * @brief Main loop handler
     * 
     */
    void Loop ();

private:
    /**
     * @brief Construct a new CApp object
     * 
     */
    CApp();

    /**
     * @brief Destroy the main application object
     * 
     */
    ~CApp();

    /**
     * @brief Suscribers container for loop event
     * 
     */
    v_CAppSubscribers _subscribers;

    /**
     * @brief Suscribe to webserver events
     * 
     * @param pSubscriber 
     */
    void _subscribe (CAppEvent* pSubscriber);

    /**
     * @brief Unsuscribe to webserver events
     * 
     * @param pSubscriber 
     */
    void _unSubscribe (CAppEvent* pSubscriber);

    /**
     * @brief Log handler to write message on terminal
     * 
     */
    CLog* _log;
};

#endif