#include <iostream>
#include <cstdlib>
#include "cnn.h"
#include "noc.h"
#include "estimation.h"

using namespace std;

// ----------------------------------------------------------------------

typedef struct CommandLine
{
  string cnn_filename;
  string noc_filename;
  string compression_filename;
  string csvFile;
} TCommandLine;

// ----------------------------------------------------------------------

void ShowCommandLineOptions(char* app_name)
{
  cerr << "Use " << app_name << " <cnn file name> <noc file name> [<weights compression rate file name> -csv <out csv file name>]" << endl;
}

// ----------------------------------------------------------------------

TCommandLine ProcessCommandLine(int argc, char* argv[])
{
  TCommandLine cl;

  if (argc < 3)
    {
      ShowCommandLineOptions(argv[0]);
      exit(-1);
    }
  
  cl.cnn_filename = string(argv[1]);
  cl.noc_filename = string(argv[2]);

  if (argc == 4 && strcmp(argv[3], "-csv") != 0)
    {   
      cl.compression_filename = string(argv[3]);
    }
  else if (argc == 5 && strcmp(argv[3], "-csv") == 0)
    {
      cl.csvFile = string(argv[4]);
    }
  else if (argc == 6 && strcmp(argv[3], "-csv") == 0)
    {
        cl.csvFile = string(argv[4]);
        cl.compression_filename = string(argv[5]);
    }
  else if (argc == 6 && strcmp(argv[4], "-csv") == 0)
    {
        cl.compression_filename = string(argv[3]);
        cl.csvFile = string(argv[5]);
    }
  else if (argc > 3)
    {
      ShowCommandLineOptions(argv[0]);
      exit(-1);
    }

  cout << "Command Line parameters: " << endl
       << "* CNN: " << cl.cnn_filename << endl
       << "* NoC: " << cl.noc_filename << endl
       << "* Weights Compression rates: " << cl.compression_filename << endl
       << "* CSV output: " << cl.csvFile << endl
       << endl;


  return cl;
}

// ----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  TCommandLine cl = ProcessCommandLine(argc, argv);

  CNN cnn;
  
  if (!cnn.loadCNN(cl.cnn_filename))
    {
      cerr << "Error loading " << cl.cnn_filename << endl;
      return -1;
    }
  
  if (cl.compression_filename != string("") &&
      !cnn.loadCompressionRatios(cl.compression_filename))
    {
      cerr << "Error loading " << cl.compression_filename << endl;
      return -1;
    }
  
  cnn.showCNN();
  
  NoC noc;

  if (!noc.loadNoC(cl.noc_filename))
    {
      cerr << "Error loading " << cl.noc_filename << endl;
      return -1;
    }

  
  noc.showNoC();
  noc.showTopology();


  //----- test wireless
  //  noc.testRouting();
  //    noc.testCommunication();
  //    return 0;
  //-----

  
  Estimation estimation(noc, cnn);
  TGlobalStats stats;
  if (estimation.stime(stats))
    {
      estimation.showStats(stats);
      estimation.saveCSV(stats, cl.csvFile);
    }
  else
    cerr << "Estimation failed!" << endl;
  
  return 0;
}
