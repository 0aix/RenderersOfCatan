#include "include/config.h"
#include <string>
#include <vector>
#include <map>
#include "rapidjson/document.h"
#include <iostream>
#include <stdlib.h>
#include "include/nopondsrule.h"
#include "include/nochitstouchingrule.h"
#include "include/islandcountrule.h"
#include "include/abstractrule.h"
#include "include/boardnode.h"

using namespace std;
using namespace rapidjson;

namespace Catan {
	namespace Generate {
		Config::Config() {}

		Config::Config(const char *json) {
			Document doc;
			doc.Parse(json);

			WATER_COUNT = doc["water_count"].GetInt();
			DESERT_COUNT = doc["desert_count"].GetInt();
			GOLD_COUNT = doc["gold_count"].GetInt();
			FOREST_COUNT = doc["forest_count"].GetInt();
			MINE_COUNT = doc["mine_count"].GetInt();
			MOUNTAIN_COUNT = doc["mountain_count"].GetInt();
			WHEAT_COUNT = doc["wheat_count"].GetInt();
			FIELDS_COUNT = doc["fields_count"].GetInt();

			const Value &boardArray = doc["board"];

			for (SizeType i = 0; i < boardArray.Size(); i++) {
				boardColumns.push_back(boardArray[i].GetInt());
			}

			const Value &chitArray = doc["chits"];

			for (SizeType i = 0; i < chitArray.Size(); i++) {
				chits.push_back(chitArray[i].GetInt());
			}

			if (!CheckNonNegative(boardColumns)) {
				cerr << "[Config File ERROR] The board array must has non-negative values" << endl;
				throw ConfigParseException();
			}

			if (!CheckNonNegative(chits)) {
				cerr << "[Config File ERROR] There chits array must have non-negative values" << endl;
				throw ConfigParseException();
			}

			if (!CheckChits()) {
				cerr << "[Config File ERROR] There are not enough chits for the number of tiles" << endl;
				throw ConfigParseException();
			}

			if (!CheckBoardSize()) {
				cerr << "[Config File ERROR] There are not enough tiles for the board" << endl;
				throw ConfigParseException();
			}

			if (!CheckOddDelta()) {
				cerr << "[Config File ERROR] Subsequent row sizes in the board must have an odd difference" << endl;
				throw ConfigParseException();
			}

			const Value &rulesArray = doc["rules"];

			for (SizeType i = 0; i < rulesArray.Size(); i++) {
				auto obj = rulesArray[i].GetObject();

				map<string, int> intMap;
				map<string, bool> boolMap;
				map<string, string> stringMap;
				
				for (auto it = obj.MemberBegin(); it != obj.MemberEnd(); it++) {
					string key = string((*it).name.GetString());
					auto &value = (*it).value;

					if (value.IsInt()) {
						intMap[key] = value.GetInt();
					} else if (value.IsBool()) {
						boolMap[key] = value.GetBool();
					} else if (value.IsString()) {
						stringMap[key] = value.GetString();
					} else {
						cerr << "[Config File ERROR] Unknown type in rule '" << key << "'" << endl;
						throw ConfigParseException();
					}
				}

				AbstractRule *rule = GenerateRule(stringMap["name"], intMap, boolMap, stringMap);
				rules.push_back(rule);
			}

      const Value &portsArray = doc["ports"];

      for (SizeType i = 0; i < portsArray.Size(); i++) {
        auto obj = portsArray[i].GetObject();
        int trade = obj.FindMember("trade")->value.GetInt();
        int count = obj.FindMember("count")->value.GetInt();
        string resource = obj.FindMember("resource")->value.GetString();

        for (int j = 0; j < count; j++) {
          ports.push_back(new Port(NULL, trade, resource));
        }
      }
		}

		// Factory method used to generate the rules from the JSON... as more rules are added,
		// this method should be updated. In the future we can design a better system that requires less
		// manual work
		AbstractRule *Config::GenerateRule(string name, map<string, int> &intMap, map<string, bool> &boolMap, map<string, string> &stringMap) {
			if (name == "NoPonds") {
				return new NoPondsRule(boolMap);
			} else if (name == "NoSpecialChitsTouching") {
				return new NoSpecialChitsTouchingRule(boolMap);
			} else if (name == "IslandCount") {
				return new IslandCountRule(intMap, boolMap);
			} else {
				cerr << "[Config File ERROR] Unknown rule '" << name << "'" << endl;
				throw ConfigParseException();
			}
		}

		int Config::TileCount() {
			return MOUNTAIN_COUNT
			+ MINE_COUNT
			+ GOLD_COUNT
			+ DESERT_COUNT
			+ FIELDS_COUNT
			+ FOREST_COUNT
			+ WATER_COUNT
			+ WHEAT_COUNT;
		}

		int Config::TileCountFromType(TileType t) {
			switch (t) {
				case MOUNTAIN: 
					return MOUNTAIN_COUNT;
				case MINE: 
					return MINE_COUNT;
				case GOLD: 
					return GOLD_COUNT;
				case DESERT: 
					return DESERT_COUNT;
				case FIELD: 
					return FIELDS_COUNT;
				case FOREST: 
					return FOREST_COUNT;
				case WATER: 
					return WATER_COUNT;
				case WHEAT: 
					return WHEAT_COUNT;
				default: 
					return 0;  
			}
		}

		int Config::FindMaxColSize() {
			MAX_COL_SIZE = 0;
			for (int i = 0; i < boardColumns.size(); i++) {
				if (boardColumns[i] > MAX_COL_SIZE) {
					MAX_COL_SIZE = boardColumns[i];
				}
			}
		}

		bool Config::CheckNonNegative(std::vector<int> &v) {
			for (int i = 0; i < v.size(); i++) {
				if (v[i] < 0) {
					return false;
				}
			}
			return true;
		}

		bool Config::CheckChits() {
			return Sum(chits) >= Sum(boardColumns) - WATER_COUNT - DESERT_COUNT; 
		}

		int Config::Sum(std::vector<int> &v) {
			int sum = 0;
			for (int i = 0; i < v.size(); i++) {
				sum += v[i];
			}
			return sum;
		}

		bool Config::CheckBoardSize() {
			return Sum(boardColumns) <= TileCount();
		}

		bool Config::CheckOddDelta() {
			if (boardColumns.size() == 1) {
				return true;
			}

			int prev = boardColumns[0];
			for (int i = 1; i < boardColumns.size(); i++) {
				if ((abs(boardColumns[i]) - prev) % 2 == 0) {
					return false;
				}
				prev = boardColumns[i];
			}
			return true;
		}
	}
}