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

CRSSI::CRSSI()
{
    _log = CLog::Create();
    _log->Message("Starting RSSI... ");
}

CRSSI::~CRSSI()
{
    _log->Message("Destroying RSSI... ");
}

void CRSSI::addRSSIData(const RSSIData& pRSSI)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _rssiData[pRSSI.V] = pRSSI; // Use V as key
    _log->Message("Added RSSI data: V=" + String(pRSSI.V) + ", dBm=" + String(pRSSI.dBm) + ", S=" + String(pRSSI.S), true, CLog::Level::DEBUG);
}

std::map<float, RSSIData> CRSSI::getRSSIData()
{
    const std::lock_guard<std::mutex> lock(_mutex);
    return _rssiData;
}


void CRSSI::clearRSSIData()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _rssiData.clear();
    _log->Message("Cleared all RSSI data", true, CLog::Level::DEBUG);
}

bool CRSSI::saveRSSIDataToSPIFFS()
{
    std::lock_guard<std::mutex> lock(_mutex);
    File file = SPIFFS.open("/conf/rssi_data.json", "w");
    if (!file) {
        _log->Message("Failed to open file for writing", true, CLog::Level::ERROR);
        return false;
    }

    JsonDocument doc;
    for (const auto& entry : _rssiData) {
        JsonObject obj = doc.add<JsonObject>();
        obj["dBm"] = entry.first;
        obj["V"] = entry.second.V;
        obj["S"] = entry.second.S;
    }

    if (serializeJson(doc, file) == 0) {
        _log->Message("Failed to write RSSI data to file", true, CLog::Level::ERROR);
        return false;
    }

    file.close();
    _log->Message("RSSI data saved successfully", true, CLog::Level::DEBUG);
    return true;
}

bool CRSSI::loadRSSIDataFromSPIFFS()
{
    const std::lock_guard<std::mutex> lock(_mutex);
    File file = SPIFFS.open("/conf/rssi_data.json", "r");
    if (!file) {
        _log->Message("Failed to open file for reading", true, CLog::Level::ERROR);
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        _log->Message("Failed to parse RSSI data from file: " + String(error.c_str()), true, CLog::Level::ERROR);
        file.close();
        return false;
    }

    _rssiData.clear();
    for (JsonObject obj : doc.as<JsonArray>()) {
        RSSIData rssiData;
        rssiData.dBm = obj["dBm"].as<float>();
        rssiData.V = obj["V"].as<float>();
        rssiData.S = obj["S"].as<float>();
        addRSSIData(rssiData);
    }

    file.close();
    _log->Message("RSSI data loaded successfully", true, CLog::Level::DEBUG);
    return true;
}
