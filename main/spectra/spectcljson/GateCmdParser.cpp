
#include "GateCmdParser.h"

#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include "GateInfo.h"
#include <json/json.h>

using namespace std;

namespace SpJs
{
  static std::map<std::string, SpJs::GateType> typeTranslations;
  static SpJs::GateType typeStrToType(std::string typeName) {
    if (typeTranslations.empty()) {
      typeTranslations["c2band"] = SpJs::C2BandGate;
      typeTranslations["gs"]     = SpJs::GammaSliceGate;
      typeTranslations["gb"]     = SpJs::GammaBandGate;
      typeTranslations["gc"]     = SpJs::GammaContourGate;
      typeTranslations["em"]     = SpJs::EqualMaskGate;
      typeTranslations["am"]     = SpJs::AndMaskGate;
      typeTranslations["nm"]     = SpJs::NotMaskGate;
      typeTranslations["-"]      = SpJs::NotGate;
      typeTranslations["+"]      = SpJs::OrGate;
      typeTranslations["*"]      = SpJs::AndGate;
      typeTranslations["T"]      = SpJs::TrueGate;
      typeTranslations["F"]      = SpJs::FalseGate;
    }
    return typeTranslations[typeName];
  }
  
  vector<unique_ptr<GateInfo>> GateCmdParser::parseList(const Json::Value& value)
  {
    using Json::Value;

    if (value["status"].asString()!="OK") {
      throw std::runtime_error("Cannot parse json because status != OK");
    }

    vector<unique_ptr<GateInfo> > result;
    const Value& detail = value["detail"];

    int nGates = detail.size();

    // Loop over all the gates.
    // Note some gates are not displayable (for example +, -, *, T, F).
    // these must still be parsed and added to the list so that the gate listbox
    // contains them.

    for (int index=0; index<nGates; ++index) {
      unique_ptr<GateInfo> pInfo;
      const Value& gate = detail[index];

      // parse each gate object

      auto typeStr = gate["type"].asString();
      if (typeStr == "s") {
          pInfo = parseSlice(gate);

      } else if (typeStr == "b") {
          pInfo = parseBand(gate);

      } else if (typeStr == "c") {
          pInfo = parseContour(gate);

      // Missing gate types are:
      // c2band - contour from 2 bands (compound).
      // gs     - gamma slice.
      // gb     - gamma band
      // gc     - gamma contour
      // em     - Makd equal
      // am     - And mask.
      // nm     - Nand mask.
      // -      - Not gate.
      // +      - OR gate.
      // *      - And gate.
      
      // For now just make these bare GateInfo Structs with only the
      // type and name filled in.  That may at least put them in the
      // list of displayed gates and keep us from emitting error messages
      // see the kludge comment.
      
      } else if (typeStr == "c2band"               ||
                 typeStr == "gs"                   ||
                 typeStr == "gb"                   ||
                 typeStr == "gc"                   ||
                 typeStr == "em"                   ||
                 typeStr == "am"                   ||
                 typeStr == "nm"                   ||
                 typeStr == "-"                    ||
                 typeStr == "+"                    ||
                 typeStr == "*"                    ||
                 typeStr == "F"                    ||
                 typeStr == "T"  ) {
        
        SpJs::GateType type = typeStrToType(typeStr);
        GateInfo* pUndisplayable = new GateInfo(gate["name"].asString(), type);
        pInfo.reset(pUndisplayable);
        
      } else {
      
        GateInfo* pGate = new GateInfo(gate["name"].asString(), SpJs::UnrecognizedGateType);
        pInfo.reset(pGate);
      }
      result.push_back(std::move(pInfo));
    }
    
    return result;
  }


  std::unique_ptr<GateInfo> GateCmdParser::parseSlice(const Json::Value &gate)
  {
      Slice* pDerived;
      unique_ptr<GateInfo> pInfo(pDerived = new Slice());

      if (gate["parameters"].size() != 1) {
          throw runtime_error("Gate type \"s\" expects only "
                              "1 parameter but a different number were provided");
      } else {
          pDerived->setParameter(gate["parameters"][0].asString());
      }

      pDerived->setLowerLimit( gate["low"].asDouble() );
      pDerived->setUpperLimit( gate["high"].asDouble() );

      pDerived->setName( gate["name"].asString() );

      return std::move(pInfo);
  }

  std::unique_ptr<GateInfo> GateCmdParser::parseBand(const Json::Value &gate)
  {
      Band* pDerived;
      unique_ptr<GateInfo> pInfo(pDerived = new Band());
      int nParams = gate["parameters"].size();

      if (gate["parameters"].size() == 2) {
          pDerived->setParameter0(gate["parameters"][0].asString());
          pDerived->setParameter1(gate["parameters"][1].asString());
      } else {
          throw runtime_error("Gate type \"b\" expects "
                              "2 parameters but a different amount was provided");
      }

      vector<pair<double,double> > points;
      auto iter = gate["points"].begin();
      auto end = gate["points"].end();
      while (iter!=end) {
            auto& val = *iter;
            auto x = val["x"].asDouble();
            auto y = val["y"].asDouble();

            points.push_back({x,y});

            ++iter;
      }
      pDerived->setPoints(points);
      pDerived->setName( gate["name"].asString() );

      return std::move(pInfo);
  }


  std::unique_ptr<GateInfo> GateCmdParser::parseContour(const Json::Value &gate)
  {
      Contour* pDerived;
      unique_ptr<GateInfo> pInfo(pDerived = new Contour());
      int nParams = gate["parameters"].size();

      if (gate["parameters"].size() == 2) {
          pDerived->setParameter0(gate["parameters"][0].asString());
          pDerived->setParameter1(gate["parameters"][1].asString());
      } else {
          throw runtime_error("Gate type \"c\" expects "
                              "2 parameters but a different amount was provided");
      }

      vector<pair<double,double> > points;
      auto iter = gate["points"].begin();
      auto end = gate["points"].end();
      while (iter!=end) {
            auto& val = *iter;
            auto x = val["x"].asDouble();
            auto y = val["y"].asDouble();

            points.push_back({x,y});

            ++iter;
      }
      pDerived->setPoints(points);
      pDerived->setName( gate["name"].asString() );

      return std::move(pInfo);
  }


} // end of namespace


