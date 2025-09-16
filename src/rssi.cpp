/**
 * @file rssi.cpp
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

#include "rssi.hpp"

CRSSI::CRSSI() :
    _low_lim(5.0),
    _high_lim(0.0)
{
    _log = CLog::Create();
    _log->Message("Starting RSSI... ");
    loadRSSIData();
}

CRSSI::~CRSSI()
{
    _log->Message("Destroying RSSI... ");
}


bool CRSSI::loadRSSIData()
{
    const std::lock_guard<std::mutex> lock(_mutex);
    
    if(!LittleFS.begin())
    {
        _log->Message ("LittleFS Mounting Error...",CLog::Level::ERROR);
        return false;
    }
    File file = LittleFS.open("/conf/rssi_data.json", "r");
    if (!file) {
        _log->Message("Failed to open file for reading", CLog::Level::ERROR);
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        _log->Message("Failed to parse RSSI data from file: " + String(error.c_str()), CLog::Level::ERROR);
        file.close();
        return false;
    }

    _rssiData.clear();
    for (JsonObject obj : doc.as<JsonArray>()) {
        RSSIData rssiData;
        rssiData.dBm = obj["dBm"].as<float>();
        rssiData.V = obj["V"].as<float>();
        rssiData.S = obj["S"].as<int>();
        _rssiData[rssiData.V]=rssiData;
        if (_low_lim > rssiData.V)
        {
            _low_lim = rssiData.V;
        }
        if (_high_lim < rssiData.V)
        {
            _high_lim = rssiData.V;
        }
    }

    file.close();
    _log->Message("RSSI data loaded successfully", true, CLog::Level::DEBUG);
    return true;
}

RSSIData CRSSI::getByVoltage(float pV)
{
    float low = _low_lim;
    float high = _high_lim;
    RSSIData data;
    for (const auto& [key, value] : _rssiData)
    {
        if ((key >= pV) && (key < high))
        {
            high = key;
        }
        if ((key <= pV) && (key > low))
        {
            low = key;
        }
    }
    if (low < high)
    {
        float ratio = (pV - low)/(high -low);
        data.V = pV;
        data.dBm = ((_rssiData[high].dBm - _rssiData[low].dBm) * ratio) + _rssiData[low].dBm;
        data.S = _rssiData[low].S;
        return data;
    }
    else if (( low == high) || ( pV < low))
    {
        return _rssiData[low];
    }
    return data;
}