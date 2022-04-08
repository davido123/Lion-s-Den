

#include <DebugUI.h>


#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

DebugUI* DebugUI::DebugUI_ = nullptr;

DebugUI* DebugUI::GetInstance(const std::string& value)
{
    if (DebugUI_ == nullptr) {
        DebugUI_ = new DebugUI(value);
    }
    return DebugUI_;
}

