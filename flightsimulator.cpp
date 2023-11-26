#include <iostream>
#include <fstream>	
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

//function prototypes:
void loadconfiguration(double winddata[], double stormdata[], double burstdata[]);
void generatewindspeeddata(double winddata[], int wsize);
void generatestormdata(double winddata[], double stormdata[], int wsize, int ssize);
void generateburstdata(double winddata[], double stormdata[], double burstdata[], int wsize, int ssize, int bsize);
void generatewindsimulation();
double frand(double max, double min);


int main()
{
	const int wsize = 4;//wind speed configuration data array size
	const int ssize = 5;//storm configuration data array size
	const int bsize = 5;//microbursts configuration data array size
	double winddata[wsize]; //wind speed configuration data array 
	double stormdata[ssize];//storm configuration data array 
	double burstdata[bsize];//microbursts configuration data array
	loadconfiguration(winddata, stormdata, burstdata);//function to read configuration from the simulation configuration file into the relevant arrays
	generatewindspeeddata(winddata, wsize);//function to create a file of time and corresponding wind speed using the simulation configuration data
	generatestormdata(winddata, stormdata, wsize, ssize);//function to create a file of time and corresponding storm magnitude using the simulation configuration data
	generateburstdata(winddata, stormdata, burstdata, wsize, ssize, bsize);//function to create a file of time and corresponding microburst magnitude using the simulation configuration data
	generatewindsimulation();//function to create a file that aggregates the windspeed, storm and microburts data 

	return 0;
}

void loadconfiguration(double winddata[], double stormdata[], double burstdata[])
{
	ifstream myfile;//opening an input file stream
	myfile.open("simulationConfiguration.txt");//opening a pre-existing file named "simulationConfiguration.txt"
	if (myfile.fail())//file cannot be opened
	{
		cerr << "Unsuccessful in loading configurations" << endl;
		exit(-1);
	}
	//This software assumes that all values in the simulation configuration file are valid and logical
	for (int i = 0;i < 4;i++)//the first 4 values on the configuration file are wind speed data
		myfile >> winddata[i];//reading value into the wind speed data array
	
	for (int i = 0;i < 5;i++)//the next 5 values are storm data
		myfile >> stormdata[i];//reading value into the storm data array

	for (int i = 0;i < 5;i++)//the last 5 values are burst data
		myfile >> burstdata[i];//reading value into the burst data array

	myfile.close();

}

void generatewindspeeddata(double winddata[], int wsize)
{
	ofstream myfile;//opening an output file stream
	myfile.open("WindSpeedData.txt");
	if (myfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	double max = winddata[0] + winddata[1];//maximum wind speed = average + gust value
	double min = winddata[0] - winddata[1];//minimum wind speed = average - gust value
	srand(time(0));//setting a seed to ensure that different set of random integers are created by the following rand() function each time the program runs
	for (int t = 0;t <= winddata[2];t += winddata[3])//loop to start from time at 0 seconds upto the given simulation duration, incrementing using the given step size
	{
		myfile << t << "\t";
		myfile << setprecision(4) << frand(max,min) << endl;//generating random floating point numbers between the minimum and maximum wind speed values of 4 significant digits
	}
	myfile.close();
}

void generatestormdata(double winddata[], double stormdata[], int wsize, int ssize)
{
	ofstream myfile;
	myfile.open("StormData.txt");
	if (myfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	srand(time(0));
	for(int t=0; t<=winddata[2];t+=winddata[3])
	{
		double pstorm = (frand(0.0, 1.0));//probability of storm occurence
		if (pstorm <= stormdata[0])
		{
			int i = t;
			int stormdmin = stormdata[3];//minimum storm amplitude
			int stormdmax = stormdata[4];//maximum storm amplitude
			int stormd = t+ ((rand() % (stormdmax-stormdmin + 1)) + stormdmin);//storm duration 
			for (i=t;i<=winddata[2]&&i<=stormd;i+=winddata[3])
			{
				double stormmag = frand(stormdata[1], stormdata[2]);//random floating point number between the minimum and maximum storm amplitude
				myfile << i <<"\t" << setprecision(4) << stormmag << endl;
				t = i;//to ensure that the outer loop continues without repeating the inner loop values
			}
		}
		else
		{
			myfile << t << "\t0" << endl;//storm magnitude will be 0 in case the probability condition isn't fulfilled
		}
	}
	myfile.close();
}

void generateburstdata(double winddata[], double stormdata[], double burstdata[], int wsize, int ssize, int bsize)
{
	ifstream sfile;
	sfile.open("StormData.txt");//create an input file stream of the StormData.txt
	if (sfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	ofstream myfile;
	myfile.open("BurstData.txt");
	if (myfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	srand(time(0));
	int time = 0;
	for (int t = 0; t <= winddata[2];t += winddata[3])
	{
		sfile >> time;
		double stormmag;
		sfile >> stormmag;
		if (stormmag != 0.0)//burst occurs only in the event that a storm is taking place
		{
			double bstorm = (frand(0.0, 1.0));
			if (bstorm <= burstdata[0])
			{
				int i = t;
				int burstdmin = burstdata[3];//minimum burst amplitude
				int burstdmax = burstdata[4];//maximum burst amplitude
				int burstd = t + ((rand() % (burstdmax - burstdmin + 1)) + burstdmin);//burst duration
				for (i = t;i <= winddata[2] && i <=burstd;i += winddata[3])
				{
					double burstmag = frand(burstdata[1], burstdata[2]);
					myfile << i << "\t" << setprecision(4) << burstmag <<endl;
					t = i;
				}
			}
			else
			{
				myfile << t << "\t0" << endl;//the burst does not occur in case the probability case isn't fulfilled
			}
		}
		else
		{
			myfile << t << "\t0" << endl;//the burst does not occur in case there is no storm at all
		}
	}
	myfile.close();
	sfile.close();
}

void generatewindsimulation()
{
	ifstream wfile;
	wfile.open("WindSpeedData.txt");
	if (wfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	ifstream sfile;
	sfile.open("StormData.txt");
	if (sfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	ifstream bfile;
	bfile.open("BurstData.txt");
	if (bfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	ofstream wsfile;
	wsfile.open("WindSimulation.txt");//creating an output file stream for the final wind simulation data
	if (wsfile.fail())
	{
		cerr << "Unsuccessful in creating file";
		exit(-1);
	}
	int t;
	wfile >> t;
	sfile >> t;
	bfile >> t;
	//to move the pointer and ensure that the last line is not duplicated in the WindSimulation file
	while (wfile.good() && sfile.good() && bfile.good())//while all the three input file streams exist
	{
		wsfile << t << "\t";//reading the time value into the wind simulation file
		double wspeed, stormmag, burstmag;
		wfile >> wspeed;
		sfile >> stormmag;
		bfile >> burstmag;
		double wsmag = wspeed + stormmag + burstmag;//adding the wind speed, storm and burst magnitudes to create the final wind simulation magnitude
		wsfile << wsmag;
		if (stormmag != 0.0)
			wsfile << "\t1";//binary value to signify that a storm is active at this stage of the simulation
		wsfile << endl;
		//Moving to the next line of values:
		wfile >> t;
		sfile >> t;
		bfile >> t;
	}
	wfile.close();
	sfile.close();
	bfile.close();
	wsfile.close();
}

double frand(double max, double min)//function to generate a floating point number between two floating point numbers
{
	return (min + (double)(rand()) / (double)(RAND_MAX / (max - min))); // Reference: https://www.delftstack.com/howto/cpp/how-to-generate-random-doubles-cpp/
}
