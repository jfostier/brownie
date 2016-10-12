/***************************************************************************
 *   Copyright (C) 2014 - 2016 Jan Fostier (jan.fostier@intec.ugent.be)    *
 *   This file is part of Brownie                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "settings.h"
#include "library.h"

#include <iostream>
#include <fstream>

using namespace std;

// ============================================================================
// SETTINGS CLASS PRIVATE
// ============================================================================

void Settings::printProgramVersion() const
{
        cout << "brownie version " << BROWNIE_MAJOR_VERSION << "."
             << BROWNIE_MINOR_VERSION << "." << BROWNIE_PATCH_LEVEL << "\n";

        cout << "Copyright (C) 2016 Jan Fostier (jan.fostier@ugent.be)\n";
        cout << "This is free software; see the source for copying conditions. "
                "There is NO\nwarranty; not even for MERCHANTABILITY or "
                "FITNESS FOR A PARTICULAR PURPOSE.\n" << endl;

        cout << "Compilation settings:\n";
        cout << "  MAXKMERLENGTH = " << MAXKMERLENGTH << "\n";
#ifdef HAVE_ZLIB
        cout << "  ZLIB support = enabled";
#else
        cout << "  ZLIB support = disabled";
#endif
        cout << endl;
}

void Settings::printUsage() const
{
        cout << "Usage: brownie command [options]\n\n";

        cout << " command\n";
        cout << "  assemble\t\tassemble short-read data\n";
        cout << "  visualize\t\tproduce a Cytoscape-compatible de Bruijn graph visualization\n";
        cout << "  compare\t\tcompare reference sequences to the de Brijn graph\n\n";

        cout << " [options]\n";
        cout << "  -h\t--help\t\tdisplay help page\n";
        cout << "  -v\t--version\tdisplay version\n\n";

        cout << "Report bugs to Jan Fostier <jan.fostier@ugent.be>\n";
}

void Settings::printUsageAssemble() const
{
        cout << "Usage: brownie assemble [options] [file_options] file1 [[file_options] file2]...\n\n";

        cout << " [options]\n";
        cout << "  -h\t--help\t\tdisplay help page\n";
        //cout << "  -s\t--singlestranded\tenable single-stranded DNA [default = false]\n";  // This has never been tested!
        cout << "  -sX\t--stageX\trun only stage X (X = 1,2,...,6)\n\n";

        cout << " [options arg]\n";
        cout << "  -k\t--kmersize\tkmer size [default = 31]\n";
        cout << "  -t\t--threads\tnumber of threads [default = available cores]\n";
        cout << "  -v\t--visits\tsearch depth during bubble detection [default = 1000]\n";
        cout << "  -d\t--depth\t\tsearch depth during read correction [default = 1000]\n";
        cout << "  -e\t--essa\t\tsparseness factor of index structure [default = 1]\n";
        cout << "  -c\t--cutoff\tcoverage threshold during error correction [default = auto]\n";
        cout << "  -p\t--pathtotmp\tpath to directory to store temporary files [default = .]\n\n";

        cout << " [file_options]\n";
        cout << "  -o\t--output\tcorrected output read file name [default = inputfile.corr]\n\n";

        cout << " examples:\n";
        cout << "  ./brownie assemble inputA.fastq\n";
        cout << "  ./brownie assemble -k 29 -t 4 -o outputA.fasta inputA.fasta -o outputB.fasta inputB.fastq\n\n";

        cout << "Report bugs to Jan Fostier <jan.fostier@ugent.be>\n";
}

void Settings::printUsageCompare() const
{
        cout << "Usage: brownie compare [options] reference.fasta\n\n";

        cout << " [options]\n";
        cout << "  -h\t--help\t\tdisplay help page\n";
        //cout << "  -s\t--singlestranded\tenable single-stranded DNA [default = false]\n";  // This has never been tested!
        cout << "  -sX\t--stageX\tcompare with stage X (X = 1,2,...,6)\n\n";

        cout << " [options arg]\n";
        cout << "  -k\t--kmersize\tkmer size [default = 31]\n";
        cout << "  -t\t--threads\tnumber of threads [default = available cores]\n";
        cout << "  -v\t--visits\tsearch depth during bubble detection [default = 1000]\n";
        cout << "  -d\t--depth\t\tsearch depth during read correction [default = 1000]\n";
        cout << "  -e\t--essa\t\tsparseness factor of index structure [default = 1]\n";
        cout << "  -c\t--cutoff\tcoverage threshold during error correction [default = auto]\n";
        cout << "  -p\t--pathtotmp\tpath to directory to store temporary files [default = .]\n\n";

        cout << " [file_options]\n";
        cout << "  -o\t--output\tcorrected output read file name [default = inputfile.corr]\n\n";

        cout << " examples:\n";
        cout << "  ./brownie assemble inputA.fastq\n";
        cout << "  ./brownie assemble -k 29 -t 4 -o outputA.fasta inputA.fasta -o outputB.fasta inputB.fastq\n\n";

        cout << "Report bugs to Jan Fostier <jan.fostier@ugent.be>\n";
}

// ============================================================================
// SETTINGS CLASS PUBLIC
// ============================================================================

Settings::Settings() : command(Command::assemble), kmerSize(31),
        numThreads(std::thread::hardware_concurrency()), doubleStranded(true),
        essaMEMSparsenessFactor(1), bubbleDFSNodeLimit(1000),
        readCorrDFSNodeLimit(1000), covCutoff(0) {}

void Settings::parseCommandLineArguments(int argc, char** args,
                                         LibraryContainer& libCont)
{
        if (argc > 1) {
                string arg(args[1]);

                if ((arg == "-h") || (arg == "--help")) {
                        printUsage();
                        exit(EXIT_SUCCESS);
                } else if ((arg == "-v") || (arg == "--version")) {
                        printProgramVersion();
                        exit(EXIT_SUCCESS);
                } else if (arg == "assemble") {
                        command = Command::assemble;
                        parseCommandLineArgAssemble(argc, args, libCont);
                } else if (arg == "visualize") {
                        cout << "This option is not available yet";
                        exit(EXIT_SUCCESS);
                } else if (arg == "compare") {
                        command = Command::compare;
                        parseCommandLineArgCompare(argc, args, libCont);
                } else {
                        cerr << "brownie: no command specified\n";
                        cerr << "Try 'brownie --help' for more information" << endl;
                        exit(EXIT_FAILURE);
                }
        }
}

void Settings::parseCommandLineArgAssemble(int argc, char** args,
                                           LibraryContainer& libCont)
{
        // parse all input arguments
        string inputFilename, outputFilename;
        vector<pair<string, string> > libraries;
        for (int i = 2; i < argc; i++) {
                string arg(args[i]);

                if ((arg == "-h") || (arg == "--help")) {
                        printUsageAssemble();
                        exit(EXIT_SUCCESS);
                } else if ((arg == "-k") || (arg == "--kmersize")) {
                        i++;
                        if (i < argc)
                                kmerSize = atoi(args[i]);
                } else if ((arg == "-t") || (arg == "--threads")) {
                        i++;
                        if (i < argc)
                                numThreads = atoi(args[i]);
                } else if ((arg == "-e") || (arg == "--essa")) {
                        i++;
                        if (i < argc)
                                essaMEMSparsenessFactor = atoi(args[i]);
                } else if ((arg == "-v") || (arg == "--visits")) {
                        i++;
                        if (i < argc)
                                bubbleDFSNodeLimit = atoi(args[i]);
                } else if ((arg == "-d") || (arg == "--depth")) {
                        i++;
                        if (i < argc)
                                readCorrDFSNodeLimit = atoi(args[i]);
                } else if ((arg == "-c") || (arg == "--cutoff")) {
                        i++;
                        if (i < argc)
                                covCutoff = atoi(args[i]);
                } /*else if ((arg == "-s") || (arg == "--singlestranded")) {
                        doubleStranded = false;
                }*/ else if ((arg == "-p") || (arg == "--pathtotmp")) {
                        i++;
                        if (i < argc)
                                pathtotemp = args[i];
                } else if ((arg == "-o") || (arg == "--output")) {
                        i++;
                        if (i < argc)
                                outputFilename = args[i];
                } else {        // it must be an input file
                        inputFilename = args[i];
                        libraries.push_back(pair<string, string>(inputFilename, outputFilename));
                        outputFilename.clear();
                }
        }

        // perform sanity check on input parameters
        if (numThreads == 0)
                numThreads = 1;

        if (numThreads > std::thread::hardware_concurrency()) {
                cerr << "WARNING: number of threads is bigger than the available number of cores" << endl;
        }

        if (kmerSize <= KMERBYTEREDUCTION *4) {
                cerr << "The kmer size must be at least " << 4*KMERBYTEREDUCTION + 1 << endl;
                throw ("Invalid argument");
        }

        if (kmerSize % 2 == 0) {
                cerr << "The kmer size must be odd" << endl;
                throw ("Invalid argument");
        }

        if (kmerSize > MAXKMERLENGTH) {
                size_t maxKmerLength = MAXKMERLENGTH;
                if (maxKmerLength % 2 == 0)
                        maxKmerLength--;
                cerr << "The kmer size can be at most " << maxKmerLength << endl;
                cerr << "Recompile Brownie with a higher MAXKMERLENGTH if a higher kmer size is desired" << endl;
                throw ("Invalid argument");
        }

        if (!pathtotemp.empty()) {
                if ((pathtotemp.back() != '/') && (pathtotemp.back() != '\\'))
                        pathtotemp.push_back('/');
        }

        // final check: see if we can write to the temporary directory
        ofstream ofs(pathtotemp + "log.txt");
        if (!ofs.good()) {
                cerr << "brownie: cannot write to directory: " << pathtotemp << "\n";
                cerr << "Please make sure the path exists" << endl;
                exit(EXIT_FAILURE);
        }

        for (int i = 0; i < argc; i++)
                ofs << args[i] << " ";
        ofs << endl;

        ofs.close();

        // add the libaries to the library container
        for (auto it : libraries) {
                ReadLibrary lib = ReadLibrary(it.first, it.second, getTempDirectory());
                libCont.insert(lib);
        }

        if (libCont.getSize() == 0) {
                cerr << "brownie: missing input read file(s)\n";
                cerr << "Try 'brownie assemble --help' for more information" << endl;
                exit(EXIT_FAILURE);
        }

        // try to read the metadata for each library
        libCont.readMetadata(getTempDirectory());
}

void Settings::parseCommandLineArgCompare(int argc, char** args,
                                          LibraryContainer& libCont)
{
        // parse all input arguments
        string inputFilename, outputFilename;
        vector<pair<string, string> > libraries;
        for (int i = 2; i < argc; i++) {
                string arg(args[i]);

                if ((arg == "-h") || (arg == "--help")) {
                        printUsageAssemble();
                        exit(EXIT_SUCCESS);
                } else if ((arg == "-k") || (arg == "--kmersize")) {
                        i++;
                        if (i < argc)
                                kmerSize = atoi(args[i]);
                } else if ((arg == "-p") || (arg == "--pathtotmp")) {
                        i++;
                        if (i < argc)
                                pathtotemp = args[i];
                }
        }

        if (kmerSize <= KMERBYTEREDUCTION *4) {
                cerr << "The kmer size must be at least " << 4*KMERBYTEREDUCTION + 1 << endl;
                throw ("Invalid argument");
        }

        if (kmerSize % 2 == 0) {
                cerr << "The kmer size must be odd" << endl;
                throw ("Invalid argument");
        }

        if (kmerSize > MAXKMERLENGTH) {
                size_t maxKmerLength = MAXKMERLENGTH;
                if (maxKmerLength % 2 == 0)
                        maxKmerLength--;
                cerr << "The kmer size can be at most " << maxKmerLength << endl;
                cerr << "Recompile Brownie with a higher MAXKMERLENGTH if a higher kmer size is desired" << endl;
                throw ("Invalid argument");
        }

        if (!pathtotemp.empty()) {
                if ((pathtotemp.back() != '/') && (pathtotemp.back() != '\\'))
                        pathtotemp.push_back('/');
        }

        // final check: see if we can write to the temporary directory
        ofstream ofs(pathtotemp + "log.txt");
        if (!ofs.good()) {
                cerr << "brownie: cannot write to directory: " << pathtotemp << "\n";
                cerr << "Please make sure the path exists" << endl;
                exit(EXIT_FAILURE);
        }

        for (int i = 0; i < argc; i++)
                ofs << args[i] << " ";
        ofs << endl;

        ofs.close();
}
