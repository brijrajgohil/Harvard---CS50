/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
       
#include <cs50.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int array[], int n)
{
    	if (n < 1)
		return false; //zero or negative n is invalid.

	int min = 0; //the first item (starts at 0)
	int max = n - 1; //the last item (starts at n-1)
	int range,middle;
	int i = 0;
	//range < 3 is an endless loop. so, avoid that.
	// 'foo = EXPR' as a whole, evaluates to EXPR
	while ((range = max - min + 1) > 2)
	////same as: for (int range = max - min + 1; range > 2; range = max - min + 1)
	{
		middle = (range / 2) + min;

		//bingo?
		if (array[middle] == value)
			return true;

		//change max or min depending on what's in array[ middle ]
		if (array[middle] > value)
			max = middle - 1;
		else if (array[middle] < value)
			min = middle + 1;
		i++;
	}
	//only two (or one) remain, check manually or we enter endless loop.
	if (array[max] == value)
		return true;
	if (array[min] == value)
		return true;
	return false;
}



/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    int min; 

	for (int i = 0; i < n; i++)
	{
		min = i;
		for (int j = i + 1; j < n; j++)
		{
			if (values[j] < values[min])
			{
				min = j;
			}
		}
		if (min != i)
		{
			values[min] ^= values[i];
			values[i] ^= values[min];
			values[min] ^= values[i];
		}
	}
    return;
}
