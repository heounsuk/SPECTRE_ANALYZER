#include <iostream> 
#include <cstring> 
#include <fstream>
#include <sstream>
#include <ctime> 
#include <vector>

#include "Var.h"

using namespace std; 


class RawFile{
public:
	RawFile(){
		state = false; 
		time_diff = 0;
	}

	RawFile(string f) {
		filename = f; 
		state = false; 
		time_diff = 0;
	}
	~RawFile(){
		for (vector<Var*>::iterator it = vars.begin(); it != vars.end(); it++){
			delete [] (*it); 
		}
	}

	bool check_Date( string line) ;
	bool check_No_Pointer( string line);
	bool check_No_Variables ( string line );
	bool check_Variables ( string line , ifstream& ifs);
	bool check_No_Values ( string line );
	void read(double from);
	void print_var(); 
	void storeToVar( int no_bucket, long double value );
	void split (const string& str, vector<Var*>& bucket, int& start_bucket );
	string split(const string& str, int ith);

	void setFilename(string f );
	string getFilename();
	void setNo_var(int vs);
	int getNo_var();

	string filename; 
	time_t date; 
	int no_points; 
	int no_var; 
	vector<Var*> vars;

	bool state; 
	double threashold; 
	long double time_diff; 
};

void RawFile::print_var(){
	for (int i=0; i< no_var+1; i++) {
		cout << vars.at(i)->getName() << ",\t" << vars.at(i)->value << ",\t" << vars.at(i)->sum << endl; 	
	}
}; 


void RawFile::read(double from = 0){
	string line; 
        cout << "1.5" << endl; 
	ifstream myfile(filename.c_str());
        cout << "2" << endl; 
	if ( myfile.is_open()){
		// 1. crawl the data information 
		while ( ! myfile.eof() ) {
                  cout << "3" << endl; 
                  getline( myfile , line ); 
                  if ( check_Date(line) || check_No_Pointer(line) || check_No_Variables(line) || check_Variables(line, myfile) ) {
                    continue;
                  }else if (check_No_Values(line)) {
                    break; 
                  }
		}
                
		string temp; 
		long double point; 
		// 2. setting threashold value of status 
		threashold = from;
		bool on = false; 

		// 3. getting the values 
		int tmp = 0; 
		while ( ! myfile.eof() ) {
			getline( myfile , line ); 
			split (line, vars, tmp );
//			if( state ) 
//				print_var();
		}

		// 4. save time_diff 
		time_diff = vars.at(1)->value - from; 
                
		myfile.close(); 
	}else{
		cout << "unable to open file" << endl; 
	}

}

bool RawFile::check_Date( string line) {
	string check_word = "Date: ";
	unsigned pos = line.find(check_word );
	if ( pos < line.size() ) {
          cout << "check_Date :" << line << endl;

          istringstream ss(
			line.substr( pos+check_word.size(), line.size()-1 ));
		ss >> date;
		return true; 
	}
	else {
		return false; 
	}
};

bool RawFile::check_No_Pointer( string line) {
	string check_word = "No. Points: ";
	unsigned pos = line.find(check_word );
	if ( pos < line.size()) {
          cout << "check_No_Pointer > "  << line << endl;
		istringstream ss(
			line.substr( pos+check_word.size(), line.size()-1 ));
		ss >> no_points;
		return true; 
	}
	else {
		return false; 
	}

};

bool RawFile::check_No_Variables ( string line ){
	string check_word = "No. Variables: ";
	unsigned pos = line.find(check_word );
        
	int _no_var = 0;
	if ( pos < line.size() ) {
          cout << "check_No_Varable > " << line << endl;  
		istringstream ss(
			line.substr( pos+check_word.size(), line.size()-1 ));
		ss >> _no_var;
		setNo_var(_no_var); 
		return true; 
	}
	else {
		return false; 
	}
};

bool RawFile::check_Variables ( string line, ifstream& ifs ){
	string check_word = "Variables:";
	unsigned pos = line.find(check_word );
        
	if ( no_var != 0 && pos < line.size() ) {
	 cout << "check_Varables > " << line << endl; 
         string var_line = line.substr( pos+check_word.size(), line.size()-1 );

		int index = 1;

		string val_name = split( var_line , 2); 
		vars.at(index)->setName(val_name); 

		for ( index=2; index<no_var+1; index++) {
			getline( ifs, var_line ); 	
			string val_name = split( var_line , 2); 
			vars.at(index)->setName(val_name); 
		}
		return true; 
	}
	else {
		return false; 
	}
};

bool RawFile::check_No_Values ( string line ){
  //  cout << line << endl; 
  string check_word = "Values:";
  unsigned pos = line.find(check_word );
   
  if ( pos < line.size() ) {		
    cout << "check_No_Values > " << line << endl; 
    return true; 	
  }
  else {	       
    return false; 
  }
};

void RawFile::setFilename(string f ){
	filename = f; 
};

string RawFile::getFilename(){
	return filename; 
};

void RawFile::setNo_var(int vs){
	no_var = vs; 
	vars.push_back ( new Var("Pointer") ); 

	for (int i=0; i<vs; i++) 
		vars.push_back( new Var()); 
};

int RawFile::getNo_var(){
	return no_var; 
};

void RawFile::storeToVar( int no_bucket, long double value ) {
	// here, I supposed to compare with thredshld value
	if ( !state ){ 
		if (no_bucket == 1 && value >= threashold){
			state = true;
			vars.at(no_bucket)->value = value; 
		}
	}else{
		if (no_bucket == 1) 
			time_diff = value - vars.at(no_bucket)->value;  
		else if (no_bucket > 1) 
			vars.at(no_bucket)->sum += (time_diff * (vars.at(no_bucket)->value + value)/2);

		vars.at(no_bucket)->value = value; 
		
	}
}

string RawFile::split(const string& str, int ith){
	char* char_str = new char [str.length()+1]; 
	strcpy( char_str, str.c_str() );
	int index=0;

	int start = 0, end = 0; 
	int i=0;
	char temp;
	bool wording = true, target = false; 
	while( (temp = *(char_str+i) ) != '\0' ) {
		i++;
		if( temp == ' ' || temp == '\t' ) {
			if( target == true ) {
				target = false;
				end = i;
			}
			wording= false; 
			continue; 
		}

		if( wording == false ) {
			if ( ++index == ith) {
				start = i; 
				target = true;
			};
			wording = true;
		}
	}

	string s(char_str+start-1, char_str+end-1); 
	delete [] char_str; 
	return s;

}

void RawFile::split (const string& str, vector<Var*>& bucket, int& start_bucket ){
	if (str == "") return; 
	char* char_str = new char [str.length()+1]; 
	strcpy( char_str, str.c_str() );
	int index=0;
//	cout << "line : "  << str << endl; 

	int start = 0, end = 0; 
	int i=0;
	char temp; 
	bool wording = false; 
	long double temp_val; 

	while( (temp = *(char_str+i) ) != NULL ) {
		i++;
		if( temp == ' ' || temp == '\t' ) {

			if( wording ) {
				end = i; 
				istringstream iss (string(char_str+start-1, char_str+end-1) );

				iss >> temp_val;
				storeToVar(start_bucket + index-1, temp_val); 
			}
			wording= false; 

		} else if( wording == false ) {
			wording = true;
			index++;
			start = i; 
		}

	}
	end = i; 
	istringstream iss (string(char_str+start-1, char_str+end) );
	iss >> temp_val;
	storeToVar(start_bucket + index-1, temp_val); 

	start_bucket += index; 
	start_bucket %= bucket.size(); 

	delete[] char_str; 
	//	return start_bucket;
}

