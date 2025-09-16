/**
 * @file rssi.hpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2025-07-20
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

#ifndef RSSI_HPP
#define RSSI_HPP

#include <map>
#include <algorithm>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "log.hpp"

class RSSIData
{
public:
    RSSIData ()
    {
        V= 0.0;
        dBm = 0.0;
        S = 0;
    }
    float V;
    float dBm;
    int S;
};

class CRSSI
{
public:

    /**
     * @brief Construct a new CRSSI object
     * 
     */
    CRSSI();

    /**
     * @brief Destroy the CRSSI object
     * 
     */
    ~CRSSI();

    RSSIData getByVoltage(float pV);
private:

    bool loadRSSIData();
    /**
     * @brief Map to store RSSI data
     * 
     */
    std::map<float, RSSIData> _rssiData;

    /**
     * @brief Logger instance
     * 
     */
    CLog* _log = CLog::Create();
    /**
     * @brief Mutex to protect against concurrent access
     * 
     */

    float _low_lim;

    float _high_lim;

    std::mutex _mutex;
    /**
     * @brief Prevent copy construction
     * 
     */
    CRSSI(const CRSSI&) = delete;
    /**
     * @brief Prevent assignment
     * 
     */
    CRSSI& operator=(const CRSSI&) = delete;
    /**
     * @brief Prevent move construction
     * 
     */
    CRSSI(CRSSI&&) = delete;
    /**
     * @brief Prevent move assignment
     * 
     */
    CRSSI& operator=(CRSSI&&) = delete;

};

#endif // RSSI_HPP
