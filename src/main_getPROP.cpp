/**********************************************************************************//**
 * \file main_getPROP.cpp
 *
 * \author Kjetil A. Johannessen
 * \date August 2010
 *
 *************************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "SplineModel.h"
#include "TopologySet.h"
#include "primitives.h"

#include <GoTools/trivariate/SplineVolume.h>

using namespace std;
using namespace Go;

bool   verbose         = false;
bool   inFile          = false;
string inFileName      = "";
string fileUsage       = "\
File usage: gpm [-v] <inputFile> \n\
  \n\
  Arguments\n\
    <inputFile>  : one or more .g2-files describing the spline volumes \n\
  FLAGS\n\
    -v           : verbose output  \n\
    -in <inFile> : uses <inFile> as command input instead of standard in\n\
    -help        : display this help screen";
SplineModel model;

void processParameters(int argc, char** argv) {
	bool fileRead = false;
	for(int argi=1; argi<argc; argi++) {
		const char *arg = argv[argi];
		if(strcmp(arg, "-v") == 0) {
			verbose = true;
		} else if(strcmp(arg, "-help") == 0) {
			cout << fileUsage << endl;
			exit(0);
		} else if(strcmp(arg, "-in") == 0) {
			if(argi >= argc-1) {
				cerr << "Error: must specify an input filename to read" << endl;
				exit(1);
			}
			inFileName = argv[++argi];
			inFile     = true;
		} else {
			ifstream inFile;
			inFile.open(arg);
			if(!inFile.good()) {
				cerr << "Error reading input file \"" << arg << "\"" << endl;
				exit(1);
			}
			model.readSplines(inFile);
			inFile.close();
			fileRead = true;
		}
	}
	if(!fileRead) {
		cout << fileUsage << endl;
		exit(1);
	}
}

/**********************************************************************************//**
 * \brief Main program for the generation of the property codes (.prop-file)
 *
 * This program compiles into a command-line tool that takes as input all the spline volumes
 * given as one or more .g2-files and creates the topology based on these, after which the
 * local-to-global mapping is created. The mapping is then stored as a .gno-file which has the
 * following format (all numbers as integers)          <br>
 * <Block#0>                                           <br>
 * <Corner#0> <Corner#1> ... <Corner#7>                <br>
 * <Line#0 start> <Line#0 step>                        <br>
 * <Line#1 start> <Line#1 step>                        <br>
 * ...                                                 <br>
 * <Line#11 start> <Line#11 step>                      <br>
 * <Face#0 start> <Face#0 step1> <Face#0 step2>        <br>
 * <Face#1 start> <Face#1 step1> <Face#1 step2>        <br>
 * ...                                                 <br>
 * <Face#5 start> <Face#5 step1> <Face#1 step2>        <br>
 * <Block#1>                                           <br>
 * ...
 *
 * The program accepts the following parameters 
 * \arg -v : verbose output (for debugging purposes mostly)
 * \arg -help : command-line help info
 *
 * Note that in case of a model with left-handed system is given as input, the program will
 * automatically reparametrize this to be right-handed. The reparameterized model will be stored
 * as reparameterized.g2 and a .gno-file based on the NEW model will be generated.
 *************************************************************************************/

int main(int argc, char **argv) {
	processParameters(argc, argv);
	// SplineModel model(volumes); // epsilon 1e-4
	TopologySet *topology = model.getTopology();
	if( model.enforceRightHandSystem() ) {
		cerr << "WARNING: system reparameterized to strict right-hand-system. \n";
		cerr << "         stored in \"reparameterized.g2\"\n";
		ofstream outFile;
		outFile.open("reparameterized.g2");
		model.writeSplines(outFile);
		outFile.close();
	}


	if(verbose) {
		cout << "Total number of vertices: " << topology->numbVertices() << endl;
		cout << "Total number of lines   : " << topology->numbLines() << " (" << topology->numbNonDegenLines() << " non-degenerate ones) " << endl;
		cout << "Total number of faces   : " << topology->numbFaces() << " (" << topology->numbNonDegenFaces() << " non-degenerate ones) " << endl;
		cout << "Total number of volumes : " << topology->numbVolumes() << endl;
	}

	if(inFile) { // read from input file
		ifstream fs;
		fs.open(inFileName.data());
		if(!fs.good()) {
			cerr << "Error opening file: \"" << inFileName << "\"" << endl;
			exit(1);
		}
		model.readModelProperties(fs);
		fs.close();
	} else {     // read from standard in
		string str;
		do {
			cout << "> ";
			getline(cin, str);
			stringstream ss(str);
			model.readModelProperties(ss);
		} while(str.size() > 0);
	}
	
	model.writeModelProperties(cout);
	return 0;
}
