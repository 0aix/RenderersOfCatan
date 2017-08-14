#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include "include/config.h"
#include "include/abstractrule.h"
#include "include/boardgraph.h"
#include "include/textrenderer.h"

using namespace std;

const int CLI_ARG_ERROR_CODE = 1;
const int CONFIG_PARSE_ERROR_CODE = 2; 

// Command line arguments
// -c [Config file path]     
// -l [float hex side length]
// -r [float chit radius]    
// -m [float margin size]
// -f [output file name]    
// -h

int main(int argc, char **argv)
{
  srand(time(NULL));
	Catan::Generate::Config config;
  fstream fin;
  string contents;

  float length = 400.0f;
  float radius = 100.0f;
  float margin = 5.0f;
  string filename = "board.png";

	// Handle command line arguments
  int c;
	while ((c = getopt(argc, argv, "c:l:r:m:f:h")) != -1) {
    switch (c) {
      case 'c':
        fin = fstream(optarg);
        contents = string((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
        try {
          config = Catan::Generate::Config(contents.c_str());
        } catch (...) {
          return CONFIG_PARSE_ERROR_CODE;
        }
        break;
      case 'l':
        length = stof(optarg);
        break;
      case 'r':
        radius = stof(optarg);
        break;
      case 'm':
        margin = stof(optarg);
        break;
      case 'f':
        filename = optarg;
        break;
      case 'h':
        cout << "Commands for catangen:" << endl;
        cout << "-c [Config file path] (required)" << endl;
        cout << "-l [float hex side length in pixels] (optional)" << endl;
        cout << "-r [float chit radius length in pixels] (optional)" << endl;
        cout << "-m [float margin size in pixels] (optional)" << endl;
        cout << "-h (optional) Display help" << endl;
        return 0;
        break;
    }
  }
	
  if (!config.initialized) {
    return CLI_ARG_ERROR_CODE;
  }

  Catan::Generate::BoardGraph graph = Catan::Generate::BoardGraph(&config);
	graph.RandomizeWithRules();

	Catan::Draw::RenderAsText(graph);
	if (Catan::Draw::Initialize())
	{
		Catan::Draw::Render(graph, length, radius, margin, filename.c_str());
		Catan::Draw::Uninitialize();
	}

  return 0;
}