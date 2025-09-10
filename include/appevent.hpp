/**
 * @file appevent.hpp
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

#ifndef APPEVENT_HPP
#define APPEVENT_HPP

#include <Arduino.h>

class CApp;

/**
 * @brief Virtual Abstract Class for main loop event in application
 * 
 * @details Derivate from this class to subscribe main loop event in application
 */
class CAppEvent
{
    /**
     * @brief Friend to CApp class to can call protected method to subscribe and unsuscribe main loop event
     * 
     */
    friend CApp;
public:

    /**
     * @brief Construct a new CAppEvent object
     * @details Constructor call subscribe method from main app 
     */
    CAppEvent ();

    /**
     * @brief Destroy the CAppEvent object
     * @details Destructor unsubscribe from main loop event 
     */
    ~CAppEvent ();

protected:

    /**
     * @brief Virtual method to implement on subscriber class 
     * @details This method is called by the main loop on CApp class manager
     */
    virtual void OnUpdate()=0;

private:

    /**
     * @brief Pointer to Application Singleton 
     * 
     */
    CApp* _app;
};

#endif //APPEVENT_HPP