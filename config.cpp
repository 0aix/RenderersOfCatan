#include "include/config.h"
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace rapidjson;

namespace Catan {
	namespace Generate {
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
			return Sum(boardColumns) <= MOUNTAIN_COUNT
			+ MINE_COUNT
			+ GOLD_COUNT
			+ DESERT_COUNT
			+ FIELDS_COUNT
			+ FOREST_COUNT
			+ WATER_COUNT
			+ WHEAT_COUNT;
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
			}
			return true;
		}
	}
}