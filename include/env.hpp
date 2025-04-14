/**
 * @file env.hpp
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

// Please change SSID and Password strings and remove .dist extension of this file

#ifndef ENV_HPP
#define ENV_HPP

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define WIFI_SSID "Delgia"
#define WIFI_PASSWORD "$s900mt1$So2050%"

// To activate Debug message, just uncomment the next line
//#define DBG

#endif //ENV_HPP