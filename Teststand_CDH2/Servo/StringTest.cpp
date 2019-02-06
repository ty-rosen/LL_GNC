#include <iostream>
#include <string>

int main(){
	std::string data = "67$-0.587$-0.572$";


/*	
	int position = 0;
	while(test[position]!='\n'){
		position++;
	}
	test.erase(position,1);
	position = 0;
	while(test[position]!='\n'){
		position++;
	}
	test.erase(position,test.length()-position);
*/
	std::cout << "Raw String: " << data << std::endl;
	int position = data.length()-1;
	while(data[position]!='$'){
		data.erase(position,1);
		position--;
	}
	std::cout << "Edit 1: " << data << std::endl;
	while(data[position]=='$'){
		data.erase(position,1);
		position--;
	}
	std::cout << "Edit 2: " << data << std::endl;
	while(data[position]!='$'){
		position--;
	}
	std::cout << "Position: " << position << std::endl;
	data.erase(data.begin(),data.begin()+position+1);




	std::cout << data << std::endl;

	return 0;
}
