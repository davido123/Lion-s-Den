

#include <Settings.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <filesystem>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

Settings* Settings::settings_= nullptr;

Settings* Settings::GetInstance(const std::string& value)
{
    if (settings_ == nullptr) {
        settings_ = new Settings(value);
    }
    return settings_;
}


void Settings::Load_File(std::string filename,Document &document)
{
    //"Data/Settings.json"
    std::ifstream ifs{ filename };
    if (!ifs.is_open())
    {
        std::cerr << "Could not open file for reading!\n";
    }

    IStreamWrapper isw{ ifs };

    document.ParseStream(isw);
    StringBuffer buffer{};
    Writer<StringBuffer> writer{ buffer };
    document.Accept(writer);

    if (document.HasParseError())
    {
        std::cout << "Error  : " << document.GetParseError() << '\n'
            << "Offset : " << document.GetErrorOffset() << '\n';
        
    }
    ifs.close();


}

void Settings::Save_File(std::string filename, Document &document)
{
    //d["test"] = 6;
    std::ofstream ofs{ filename };
    if (!ofs.is_open())
    {
        std::cerr << "Could not open file for writing!\n";
    }
   
    OStreamWrapper osw{ ofs };
    //Writer<OStreamWrapper> writer2{ osw };
    PrettyWriter<OStreamWrapper> writer2{ osw };

    document.Accept(writer2);
    ofs.close();
}
