#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

class _dateTime
{

public: // Methods

    static std::string dateTimeStamp()
    {
        auto timePointNow = std::chrono::system_clock::now();
        auto timeNow = std::chrono::system_clock::to_time_t(timePointNow);
        auto timeNowMS = std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&timeNow), "%Y-%m-%d %H:%M:%S.");
        ss << std::setfill('0') << std::setw(3) << timeNowMS.count();

        return ss.str();
    }

    // ISO 8601 format: "2011-09-07T12:28:29"
    static std::string iso8601DateTimeStamp()
    {
        auto timePointNow = std::chrono::system_clock::now();
        auto timeNow = std::chrono::system_clock::to_time_t(timePointNow);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&timeNow), "%Y-%m-%dT%H:%M:%S");

        return ss.str();
    }

    // ISO 8601 format with UTC: "2011-09-07T12:28:29Z"
    static std::string iso8601DateTimeStampUTC()
    {
        auto timePointNow = std::chrono::system_clock::now();
        auto timeNow = std::chrono::system_clock::to_time_t(timePointNow);

        std::stringstream ss;
        ss << std::put_time(std::gmtime(&timeNow), "%Y-%m-%dT%H:%M:%SZ");

        return ss.str();
    }

    // ISO 8601 format with milliseconds: "2011-09-07T12:28:29.123"
    static std::string iso8601DateTimeStampWithMillis()
    {
        auto timePointNow = std::chrono::system_clock::now();
        auto timeNow = std::chrono::system_clock::to_time_t(timePointNow);
        auto timeNowMS = std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&timeNow), "%Y-%m-%dT%H:%M:%S.");
        ss << std::setfill('0') << std::setw(3) << timeNowMS.count();

        return ss.str();
    }

    static std::string addDateTimeStamp(const std::string& strInput)
    {
        std::string strInputCopy = dateTimeStamp();
        strInputCopy += ": ";
        strInputCopy += strInput;

        return strInputCopy;
    }
};