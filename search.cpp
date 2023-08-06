// Create an array of tan(x). x = 0 .. PI/2, in steps of 0.0001
#include <cstdint>
#include <cmath>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

constexpr uint16_t TanxTabSize (3.14159265 / (2 * 0.0001));
float tanx[TanxTabSize + 2];

int linSearch(float tx)
{
	// linear search for tanx in tanx[]

	for (int i = 0; i < sizeof(tanx) / sizeof(float); i++)
	   if ((tanx[i] > tx) || tanx[i] < 0)
		  return i;

	return -1;
}

int binSearch(float tx)
{
	// binary search for tanx in tanx[]

	int lower = 0;
	int upper = sizeof(tanx) / sizeof(float);

		while (upper - lower > 1)
		{
			if ((tanx[(lower + upper) / 2] < tx) || tanx[(lower + upper) / 2] < 0)
				lower = (lower + upper) / 2;

			if ((tanx[(lower + upper) / 2] > tx) || tanx[(lower + upper) / 2] < 0)
				upper = (lower + upper) / 2;

			if ((tanx[(lower + upper) / 2] == tx) || tanx[(lower + upper) / 2] < 0)
				return (lower + upper) / 2;
		}
	if (upper - lower <= 1)
		return lower;

	return -1;
}




int main()
{
	float t;

	for (int i = 0; i < sizeof(tanx) / sizeof(float); i++)
		tanx[i] = tan(i*0.0001);

	for (int i = 0; i < 10; i++)
		cout << linSearch(i) << "\n";

	auto start = high_resolution_clock::now();

	for (int i = 0; i < 10000; i++)
		//		cout << binSearch(i/10000.0) << "\n";
		 binSearch(i / 10000.0);
		//t = atan2(i / 10000.0, 1.0);

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);;
	cout << "Elapsed Time: " <<  duration.count() << " microseconds \n";


	return 0;
}