#include "Engine.hpp"

#include "picojson.hpp"

int main(int argc, char* argv[]) {
    /*
    //char buffer[] = "{\"name\" : \"Jack\", \"age\" : 27}";

    size_t rawBytesRead;
    char* rawBuffer = FileSystem::GetInstance()->readAllBytes("../run_tree/Data/Levels/jsTest.scene", rawBytesRead);

    size_t size;
    char* buffer = StripComments(rawBuffer, rawBytesRead, size);

    std::string json(buffer, size);
    picojson::value v;
    std::string err = picojson::parse(v, json);
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!v.is < picojson::object>()) {
        std::cerr << "JSON is not an object" << std::endl;
    }
    else {
        picojson::value::object& obj = v.get<picojson::object>();

        auto k = obj.operator[](std::string("Scene"));

        if (k.is<picojson::object>()) {
            auto k2 = k.get<picojson::object>();
            auto k3 = k2["name"].get<std::string>();
        }
        //for (picojson::value::object::const_iterator i = obj.begin(); i != obj.end(); ++i) {
        //    std::cout << i->first << ": " << i->second.to_str() << std::endl;
        //}

        DataNode node;

        DataNode s;
        s.data["name"] = MultiData("Level 1");
        s.data["gravity"] = MultiData(-9.81);
        s.data["raining"] = MultiData(true);
        s.data["size"] = MultiData(1.25);
        node.children["Scene"] = s;

        DataNode r;
        r.data["num"] = MultiData("Level 1");

        DataNode f;
        f.data["path"] = MultiData("resources/level.mcf");
        f.data["size"] = MultiData(5);
        r.children["file"] = f;

        node.children["Resources"] = r;

        {
            ScopeTimer t("DataNode 100000 accesses");

            for (int n = 0; n < 100000; n++) {
                std::string name = node.getChild("Scene").getData("name").asString();
            }
        }

        {
            ScopeTimer t("JSON     100000 accesses");

            for (int n = 0; n < 100000; n++) {
                std::string name = obj["Scene"].get<picojson::object>()["name"].get<std::string>();
            }
        }
    }

    free(buffer);
    return 0;*/

    Engine engine;
    if (!engine.Init(argc, argv)) {
        Console::logMessage("Failed to initialize Engine");

        system("pause");
        return 0;
    }

    Console::logMessage("Starting Engine loop...");
    engine.Start();
    Console::logMessage("Engine loop finished...");

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    return 0;
}