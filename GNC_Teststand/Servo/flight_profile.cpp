#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>

using namespace std;

//Function that takes in a Hertz value and returns
//a vector of positions for the throttle
vector<float> flight_profile(int hz)
{
	//Input File Stream Directory Location
	ifstream file ("Book1.csv");
	string singline[3];
	float incr;
	float lastval;
	vector<float> output;

	//While there are still values in the csv file
	//get them
	while (getline(file, singline[0], ','))
	{
		getline(file, singline[1], ',');
		getline(file, singline[2]);


		//Check if throttle position is between 1 to 2
		if (stof(singline[0]) < 1 || stof(singline[0]) > 2)
		{
			cout << "Throttle Position Not Within Range 1 to 2 \n";
			cout << "Flight_Profile incomplete \n";
			break;
		}

		//if type "h" (hold)
		//append position value to end of output vector
		//Seconds*Hertz times
		if (singline[1] == "h" || singline[1] == "H")
		{
			for (int i = 0; i < stof(singline[2])*hz; i++)
			{
				output.push_back(stof(singline[0]));
			}

		}

		//if type "s" (sweep)
		//calculate increment value
		//increase the throttle position by increment value in a loop 
		//and append each to the output until desired position is reached
		else if (singline[1] == "s" || singline[1] == "S")
		{
			lastval = output.back();
			incr = (stof(singline[0])-lastval)/(stof(singline[2])*hz);
			for (int q = 0; q < stof(singline[2])*hz; q++)
			{
				lastval += incr;
				output.push_back(lastval);
			}
		}

		//type is not one of the acceptable values
		else
		{
			cout << "Type of movement not recognized";
			cout << "Flight_Profile Incomplete";
			break;
		}

	}


	return output;
}

int main()
{
	vector<float> a = flight_profile(2);
	for (vector<float>::const_iterator i = a.begin(); i != a.end(); i++){
		cout << *i << "\n";
	}

}
