#pragma once
#include <json/json.h>
#include <string>

inline std::string jsonToString(const Json::Value &v)
{
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, v);
}
