#pragma once


#include <string>


class DebugUI
{
public:

	DebugUI(DebugUI& other) = delete;
	void operator=(const DebugUI&) = delete;
	static DebugUI* GetInstance(const std::string& value);



	std::string value() const {
		return value_;
	}

	virtual void RenderDebug();




private:



protected:
	DebugUI(const std::string value) : value_(value)
	{
	}

	static DebugUI* DebugUI_;

	std::string value_;

};