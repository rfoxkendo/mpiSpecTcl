
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <TH1.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "HistInfo.h"
#include "BinInfo.h"
#include "ParameterInfo.h"

#include "config.h"

#define private public
#define protected public
#include "JsonParser.h"
#undef protected
#undef private

using namespace std;

class JsonParserTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(JsonParserTest);
  CPPUNIT_TEST( parseListCmd_0 );
  CPPUNIT_TEST( parseListDetail_0 );
  CPPUNIT_TEST( parseContentCmd_0 );
  CPPUNIT_TEST( json_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:

  void parseListDetail_0();
  void parseListCmd_0();
  void parseContentCmd_0();
  void json_0();
  void parseParameterCmd_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(JsonParserTest);


void JsonParserTest::parseListCmd_0() 
{
  stringstream ss;
  ss << JSON_TEST_DIR << "/list1d.json";

  Json::Value value;
  std::ifstream file (ss.str().c_str());
  file >> value;

  std::vector<SpJs::HistInfo> parsedResult = SpJs::JsonParser().parseListCmd(value);

  SpJs::HistInfo expected;
  expected.s_name = "raw00";
  expected.s_type = 1;
  expected.s_params = {"event.raw.00"};
  expected.s_axes = {{1.0, 100.0, 100}};
  expected.s_chanType = SpJs::Long;

  CPPUNIT_ASSERT(1 == parsedResult.size());
  CPPUNIT_ASSERT( expected == parsedResult[0] );

}
 

void JsonParserTest::parseListDetail_0() 
{
  stringstream ss;
  ss << JSON_TEST_DIR << "/list1d_detail.json";

  Json::Value value;
  std::ifstream file (ss.str().c_str());
  file >> value;

  SpJs::HistInfo parsedResult = SpJs::JsonParser().parseListDetail(value);

  SpJs::HistInfo expected;
  expected.s_name = "raw00";
  expected.s_type = 1;
  expected.s_params = {"event.raw.00"};
  expected.s_axes = {{1.0, 100.0, 100}};
  expected.s_chanType = SpJs::Long;

  CPPUNIT_ASSERT( expected == parsedResult );

}
 

void JsonParserTest::parseContentCmd_0 () 
{
  stringstream ss;
  ss << JSON_TEST_DIR << "/content1d.json";

  Json::Value value;
  std::ifstream file (ss.str().c_str());
  file >> value;
  file.close();

  auto parsedResult = SpJs::JsonParser().parseContentCmd(value);

  vector<SpJs::BinInfo> expected = {{0,0,0,3.},
                                        {1,0,0,3.},
                                        {2,0,0,1.},
                                        {3,0,0,2.},
                                        {4,0,0,4.},
                                        {5,0,0,2.},
                                        {6,0,0,1.},
                                        {7,0,0,1.},
                                        {8,0,0,5.},
                                        {9,0,0,2.},
                                        {10,0,0,2.}};

  CPPUNIT_ASSERT( expected == parsedResult );
}

void JsonParserTest::json_0 () 
{

    std::string jsonStr(
    "{\n"
    "  \"x\" : 34\n"
    "}"
                );

  Json::Value value;
  Json::Reader reader;
  reader.parse(jsonStr,value);

  CPPUNIT_ASSERT_NO_THROW(value["x"]);
}

