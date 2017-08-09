#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "include/config.h"
#include "include/abstractrule.h"
#include "include/boardgraph.h"
#include "include/textrenderer.h"

using namespace std;

const int CLI_ARG_ERROR_CODE = 1;
const int CONFIG_PARSE_ERROR_CODE = 2; 

// Command line arguments
// -c [Config file path]
// -r [text|image]

int main(int argv, char **argc)
{
  srand(time(NULL));
	Catan::Generate::Config config;
	string renderType = "text";
	// Handle command line arguments
	for (int i = 1; i < argv; i++) {
		const char *arg = argc[i];
		if (strcmp(arg, "-c") == 0) {
			i++;
			if (i != argv) {
				cout << "Looking for config file at location: " << argc[i] << endl;
				fstream fin(argc[i]);
				string contents((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
				try {
					config = Catan::Generate::Config(contents.c_str());
				} catch (...) {
					return CONFIG_PARSE_ERROR_CODE;
				}
			} else {
				cerr << "Usage: -c [Config File Path]" << endl;
				return CLI_ARG_ERROR_CODE;
			}
		} else if (strcmp(arg, "-r") == 0) {
			i++;
			if (i != argv) {
				renderType = argc[i];
			} else {
				cerr << "Usage: -r [text|image]" << endl;
				return CLI_ARG_ERROR_CODE;
			}
		}
	}

	Catan::Generate::BoardGraph graph = Catan::Generate::BoardGraph(&config);
	graph.RandomizeWithRules();

	Catan::Draw::RenderAsText(graph);
	if (Catan::Draw::Initialize())
	{
		Catan::Draw::Render(graph, 200.0f, 50.0f, 2.5f, 1);
		Catan::Draw::Uninitialize();
	}

  return 0;
}