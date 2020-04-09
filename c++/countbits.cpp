/* A test of the countbits function */

#include <iostream>
using namespace std;

int main()
{
	int n, num;
	cout << "Enter an integer value between 1 and 511 inclusive: ";
	cin >> n;

	num = n;
	num = (num & 341) + ((num & 170) >> 1);
	num = (num & 307) + ((num & 204) >> 2);
	num = (num & 271) + ((num & 240) >> 4);
	num = (num & 127) + ((num & 256) >> 8);

	cout << n << " has " << num << " nonzero bits";

	return 0;
}