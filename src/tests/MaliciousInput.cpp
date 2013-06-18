#include "Tests.h"
#include "BasicStruct.h"

using namespace std;
using namespace Srl;
using namespace Tests;

struct TestStruct {

    vector<BasicStruct> vec { BasicStruct() };

    template<Mode M>
    void srl_resolve(Context<M>& ctx)
    {
        ctx ("vec", vec);
    }
};

bool malicious_input()
{
    print_log("\nTest malicious input Tree\n");

    bool success = false;
    TestStruct m;
    auto tree = Tree::From_Type(m);
    auto& node = *tree.root();

    try {
        print_log("\tOut of bounds access...");
        node.node(2);
        print_log("failed.\n");

    } catch(Exception& ex) {
        success = true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tWrong field access.....");
        node.node("cev");
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tType mismatch..........");
        node.unwrap_field<int>("vec");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tOverflow int string....");
        node.insert("string", to_string(numeric_limits<uint64_t>::max()) + "1");
        node.unwrap_field<uint64_t>("string");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tOverflow integer max...");
        auto i = (uint64_t)(numeric_limits<int64_t>::max()) + 1;
        node.insert("uint", i);
        node.unwrap_field<int64_t>("uint");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tOverflow integer min...");
        int64_t i = numeric_limits<int64_t>::min();
        node.insert("int", i);
        node.unwrap_field<int32_t>("int");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tOverflow fp max........");
        auto d = numeric_limits<double>::max();
        node.insert("double_max", d);
        node.unwrap_field<float>("double_max");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tOverflow fp min........");
        auto d = numeric_limits<double>::min();
        node.insert("double_min", d);
        node.unwrap_field<float>("double_min");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }
    try {
        print_log("\tNegative to unsigned...");
        short s = -1;
        node.insert("short", s);
        node.unwrap_field<unsigned short>("short");
        success = false;
        print_log("failed.\n");

    } catch(Exception& ex) {
        success &= true;
        print_log("ok. -> " + string(ex.what()) + "\n");
    }

    return success;
}

template<class TParser, class... Tail>
bool malicious_input(const TParser& parser, const string& parser_name, const Tail&... tail)
{
    print_log("\nTest malicious input " + parser_name + "\n");

    bool success = false;
    TestStruct m;
    auto data = Store(m, parser);

    assert(data.size() > 100);
    /* wild guess */
    for(auto i = 100U; i < data.size(); i += 91) {
        data[i] ^= 1U;
    }

    try {
        if(Verbose) cout<<"\tParsing...";
        Tree::From_Source(data, parser);
        print_log("failed.\n");

    } catch(Exception& ex) {
        print_log("ok. -> " + string(ex.what()) + "\n");
        success = true;
    }
    return malicious_input(tail...) && success;
}


bool Tests::test_malicious_input()
{
    bool success = malicious_input (
        PSrl(),  "Srl",  PBson(), "Bson",
        PJson(), "Json", PXml(),  "Xml",
        PJson(false), "Json w/o space", PXml(false), "Xml w/o space"
    );

    return success;
}


