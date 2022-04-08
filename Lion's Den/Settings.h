#pragma once


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

using namespace rapidjson;
class Settings
{
public:

	Settings(Settings& other) = delete;
	void operator=(const Settings&) = delete;
	static Settings* GetInstance(const std::string& value);


	void Load_File(std::string filename, Document &document);
	void Save_File(std::string filename, Document &document);


	std::string value() const {
		return value_;
	}

	Document d;

	Document SaveSlot1;
	Document SaveSlot2;
	Document SaveSlot3;
private:


	std::string path;
	
protected:
	Settings(const std::string value) : value_(value)
	{
	}

	static Settings* settings_;

	std::string value_;

	

};