/*K-Means step-by-step procedure (codebook size, k = 8 and universe size, M):
1. Initialize m=0 and the codebook with random vectors from the universe. (Now your codebook has k vectors of 12 values each.
2. for each of the vector in universe, assign it to the region based on nearest neighbor rule.
3. calculate the total distortion, say dist, of all the regions. Take the average distortion, say av_dist = dist/M . (Use Tokhura distance)
4. Calculate the centroid of each region and update the centroid for every region.
5. m = m+1;
6. perform the steps 2,3,4 till abs(distortion - dist) > delta. distortion is the previous iteration distortion and dist is the current one. (you can take delta value either 0.0001 or 0.00001)
*/


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>




//Generates random number between 0 and limit (both included)
int randInt(int limit)
{
	return (rand() % (limit+1));
}

//Converts Universe.csv to a 2D matrix
void universeToMatrix(double** universe, int uSize, int p)
{
	FILE* fp = fopen("Universe.csv","r");

	int i=0, j=0;			

	char* csv_line;			
	char mystring[1024];	

	
	while(fgets(mystring, 1024, fp))
	{
		csv_line = strtok(mystring, ",");	

		j = 0;
		while(csv_line)		
		{
			universe[i][j] = atof(csv_line);	
			j++;
			csv_line = strtok(NULL,",");		
		}
		i++;
	}
}

//Initialize 2D codebook from random vectors from universe
void codebookInit(double** universe, int uSize, double** codebook, int k, int p)
{
	int i=0, j=0;
	int universe_row = 0;

	srand(time(0));		//Random seed is current time

	
	for(i=0;i<k;i++)
	{
		universe_row = randInt(uSize-1);	//Select random value for row

		for(j=0;j<p;j++)
		{
			codebook[i][j] = universe[universe_row][j];	//Insert random row from universe to codebook
		}
	}
}

//Prints codebook
void printCodebook(double** codebook, int k, int p)
{
	int i=0,j=0;
	for(i=0;i<k;i++)
	{
		for(j=0;j<p;j++)
			printf("%f\t",codebook[i][j]);
		printf("\n");
	}
}

//Calculates tokhura distance
double* tokhuraDist(int uIndex, double** universe, int uSize, double** codebook, int k, int p)
{
	double wi[12] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};	//Wi values for Tokhura's distance

	double tokhura = 0;
	double tokhuramin[2];	//For calculation of minimum tokhura distance

	tokhuramin[0] = 1000000000000;	//Saves minimum tokhura distance
	tokhuramin[1] = -1;				//Saves region number of min. tokhura distance

	int i=0,j=0;

	for(i=0;i<k;i++)
	{
		for(j=0;j<p;j++)
			tokhura += wi[j]*(pow((universe[uIndex][j]-codebook[i][j]),2));	//Tokhura's distance formula

		//Finding minimum from the distances
		if(tokhura<tokhuramin[0])
		{
			tokhuramin[0] = tokhura;	//Current distance is minimum
			tokhuramin[1] = i;			//Current region is minimum
		}
		tokhura = 0;
	}

	return tokhuramin;
}

//Calculate resultant codebook after Kmeans algorithm
double** kmeans(double** universe, int uSize, double** codebook, int k, int p)
{
	int i=0;
	int j=0;

	double delta = 0.00001;

	double prev_distortion = 10000000000;
	double distortion = 0;

	double** region_sum = new double*[p];	//Stores sum of the c1...c12 of a region
	double* region_size = new double[k];	//Stores no. of vectors in a region

	//Initialize region_sum and region_size with zeros
	for(i=0;i<k;i++)
	{
		region_size[i] = 0;

		region_sum[i] = new double[p];	//Also create new rows in region_sum
		for(j=0;j<p;j++)
			region_sum[i][j] = 0;
	}


	double* dist = NULL;	//Stores return of tokhuraDist()
	
	while(delta < abs(prev_distortion - distortion))
	{
		prev_distortion = distortion;	//Update previous distortion
		distortion = 0;
		
		//Initialize region_sum and region_size with zeros
		for(i=0;i<k;i++)
		{
			region_size[i] = 0;

			for(j=0;j<p;j++)
				region_sum[i][j] = 0;
		}

		//Loop in all rows of universe
		for(i=0;i<uSize;i++)
		{
			dist = tokhuraDist(i,universe,uSize,codebook,k,p);

			distortion += dist[0];	//Add min tokhura distance to distortion

			for(j=0;j<p;j++)
			{
				region_sum[(int)dist[1]][j] += universe[i][j];	//Add current universe row to region_sum
			}

			region_size[(int)dist[1]]++;	//Increment region size
		}

		distortion /= uSize;	//Average out distortion to calculate

		for(i=0;i<k;i++)
		{
			for(j=0;j<p;j++)
				codebook[i][j] = (region_sum[i][j] / region_size[i]);	//Averaging out codebook
		}
	}

	printf("Distortion ----------------->>>>>>>>>>>>>>>>>>>>> %f5\n\n",distortion);

	return codebook;
}

int main()
{
	int uSize = 6340, k = 8, p = 12;			//Initialize values

	//Create matrices for codebook and universe
	double** codebook = new double*[k];
	double** universe = new double*[uSize];

	int i=0,j=0;

	//Initialize codebook and universe matrices with zeros
	for(i=0;i<k;i++)
	{
		codebook[i] = new double[p];
		for(j=0;j<p;j++)
			codebook[i][j] = 0;
	}

	for(i=0;i<uSize;i++)
	{
		universe[i] = new double[p];
		for(j=0;j<p;j++)
			universe[i][j] = 0;
	}

	universeToMatrix(universe, uSize, p);				//Convert Universe.csv to matrix
	codebookInit(universe, uSize, codebook, k, p);		//Initialize codebook using random vectors from universe


	codebook = kmeans(universe,uSize,codebook,k,p);		//Apply K means and create codebook

	printf("....................................The final codebook obtained..................................\n\n");
	printCodebook(codebook,k,p);						//Print resultant codebook

	system("PAUSE");								    //Pause console to show output

	return 0;
}
