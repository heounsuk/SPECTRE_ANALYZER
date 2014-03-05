#include <iostream>
#include <fstream> 
#include <string> 
#include <ctime>
#include <vector>
#include <stdio.h>  
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include "RawFile1.h"


#define RESULT "example.txt"
using namespace std;  

double outputCalculation( RawFile* rawFile );
int getdir (string dir, vector<string> &files, string ext);

int main(int argc, char* argv[]) {
	double from = 4e-7;
	
	ofstream myfile;
	myfile.open(RESULT, ios::app);

	string dir = "../"; 
	vector<string> files = vector<string>(); 
	
	getdir(dir, files, "raw"); 
	
	if (myfile.is_open()){
          time_t t = time(0); 
          struct tm * now = localtime( & t );
          myfile << "\n---> efficiency of dir (" << dir << ")" << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << endl;
	
          for ( int i=0; i< files.size(); i++) {
            cout << files.at(i) << endl; 
            RawFile* rawFile = new RawFile(files.at(i));
            cout << "test1" << endl; 	
            rawFile->read(from); //2w10c.raw
            delete rawFile;
		      
      	
            myfile << files.at(i) << "\t" << outputCalculation(rawFile) << endl;

          }
	}
	return 0;
}
/*
var[0..n-1]

var[0]	pointer 
var[1]	time 
var[2]	R4:1		pout = var[4] * var[2]
var[3]	R4:pwr		pvinc = -(var[5] * var[10])
var[4]	vout		pvinb = -(var[6] * var[9])
var[5]	vinc		pvclk = -(var[7] * var[8])
var[6]	vinb		pout/(pvinc+pvinb+pvclk)*100
var[7]	clk 
var[8]	V2:p 
var[9]	V10:p
var[10]	V11:p
*/

double outputCalculation( RawFile* rawFile ){
	// efficiency 
	vector<double> vars;

	for(int i=0; i< rawFile->vars.size(); i++) 
		vars.push_back(rawFile->vars.at(i)->sum / rawFile->time_diff);

	double pout = vars.at(4) * vars.at(2);
	double pvinc = -(vars.at(5) * vars.at(10));
	double pvinb = -(vars.at(6) * vars.at(9));
	double pvclk = -(vars.at(7) * vars.at(8));
	double eff = pout/(pvinc+pvinb+pvclk)*100;
	return eff; 

}

int getdir (string dir, vector<string> &files, string ext) {
  DIR *dp; 
  struct dirent *dirp; 
  if ( (dp = opendir(dir.c_str() ) ) == NULL ){
    cout << "Error(" << errno << ") opening " << dir << endl; 
    return errno; 
  }
  
  while( (dirp = readdir(dp)) != NULL ) {
    string filename = dirp->d_name; 
    int fs = filename.size(); 
    int es = ext.size(); 
    if( fs>es && filename.compare( fs-es, -1, ext ) == 0 ){
        files.push_back( filename );
    }
  }
  closedir(dp); 
  return 0; 
}


