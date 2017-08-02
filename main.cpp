#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include <string>
#include <iostream>
#include <fstream>
#include "include/config.h"
#include "include/abstractrule.h"

using namespace std;

const int CLI_ARG_ERROR_CODE = 1;
const int CONFIG_PARSE_ERROR_CODE = 2; 

// Command line arguments
// -c [Config file path]
int main(int argv, char **argc)
{
	Catan::Generate::Config config;
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
		}
	}



    if (Catan::Draw::Initialize())
    {
        Catan::Draw::Render();
        Catan::Draw::Uninitialize();
    }

    return 0;
}