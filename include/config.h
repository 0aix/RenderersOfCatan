#pragma once
#include <vector>
#include "boardnode.h"
#include <map>
#include <string>

// The config file schema looks like this:
/*
{
	"board" : [<int>, ...],
	"chits" : [<int>, ...],
	"rules" : [<rule object>, ...],
	"water_count" : <int>,
	"desert_count" : <int>,
	"gold_count" : <int>,
	"forest_count" : <int>,
	"mine_count" : <int>,
	"mountain_count" : <int>,
	"wheat_count" : <int>,
	"fields_count" : <int>
}
*/

namespace Catan {
	namespace Generate {
		class AbstractRule;
		class Config {
		public:
			int WATER_COUNT;
			int DESERT_COUNT;
			int GOLD_COUNT;
			int FOREST_COUNT;
			int MINE_COUNT;
			int MOUNTAIN_COUNT;			
			int WHEAT_COUNT;
			int FIELDS_COUNT;
			int MAX_COL_SIZE;
			std::vector<int> boardColumns;
			std::vector<int> chits;
			std::vector<AbstractRule*> rules;

			Config(const char *json);
			Config();
			int TileCount();
			int TileCountFromType(TileType t);
		private:
			int FindMaxColSize();

			bool CheckNonNegative(std::vector<int> &v);

			bool CheckChits();

			int Sum(std::vector<int> &v);

			bool CheckBoardSize();

			bool CheckOddDelta();

			AbstractRule *GenerateRule(std::string name, 
				std::map<std::string, int> &intMap, 
				std::map<std::string, bool> &boolMap, 
				std::map<std::string, std::string> &stringMap);
		};

		// Config throws this when it encounters an error 
		class ConfigParseException {};
	}
}