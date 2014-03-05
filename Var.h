#include <string> 
using namespace std; 

class Var {
public:
	Var(){ 
		value = 0; 
		sum = 0.0; 
	} 
	Var(string n ){
		name = n; 
		value = 0; 
		sum = 0.0; 
	} 
	void calculate_sum(){

	}

	void setName(string n) {
		name = n; 
	}
	string getName(){
		return name; 
	}
	long double value; 
	string name; 
	long double sum; 
};

