#pragma once
#include <string>
#include <memory>
#include "../jsoncpp/json/json.h"
/* 改变JsonCpp的某些行为,用于处理只读的Json */
class MJson:public Json::Value
{
public:
	//解析Json字符串
	MJson(Json::Value v)
	{
		this->swap(v);
	}
	MJson(const std::string & str) 
	{
		std::unique_ptr<Json::CharReader> const jsonReader(Json::CharReaderBuilder().newCharReader());
		jsonReader->parse(str.c_str(), str.c_str() + str.length(), this, &errs);
	}
	/* 根据key从 json 字符串中取value*/
	MJson operator[](const std::string & key) {
		int i = 3;
		if (!(isMember(key))) {
			throw std::exception(("The key '" + key + "' is not exists").c_str());
		}
		return Json::Value::operator[](key);
	}
private:
	JSONCPP_STRING errs;
};

