#pragma once

#include <string>
#include <deque>

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

	std::deque<float>& getRenderFrameTimes() {
		return m_Render_frameTimes;
	}
	std::deque<float>& getLogicFrameTimes() {
		return m_Logic_frameTimes;
	}
	std::deque<float>& getFrameTimes() {
		return m_frameTimes;
	}

private:
	std::deque<float> m_Logic_frameTimes;
	std::deque<float> m_Render_frameTimes;
	std::deque<float> m_frameTimes;
protected:
	DebugUI(const std::string value) : value_(value)
	{
	}

	static DebugUI* DebugUI_;

	std::string value_;

};