#pragma once
#include <vector>
#include "abstractrule.h"

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

			Config(const char *json);
			Config();
		private:
			std::vector<int> boardColumns;

			std::vector<int> chits;

			std::vector<AbstractRule*> rules;

			int FindMaxColSize();

			bool CheckNonNegative(std::vector<int> &v);

			bool CheckChits();

			int Sum(std::vector<int> &v);

			bool CheckBoardSize();

			bool CheckOddDelta();
		};

		// Config throws this when it encounters an error 
		class ConfigParseException {};
	}
}