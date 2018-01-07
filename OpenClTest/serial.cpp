// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <sstream>

using std::string;
using namespace std;

//int main(int argc, wchar_t *argv[]) {
//	time_t startWhole = clock();
//	// length of vectors
//	unsigned int n = 5000000;
//	double *h_a;
//	double *h_b;
//	double *h_c;
//
//	int bytes = n * sizeof(double);
//
//	// allocate memory for each vector on host
//	h_a = (double*)malloc(bytes);
//	h_b = (double*)malloc(bytes);
//	h_c = (double*)malloc(bytes);
//	int i;
//
//	//try {
//	//	ifstream file("doubleVektor1.txt");
//	//	ifstream file2("doubleVektor2.txt");
//	//	if (file.is_open()) {
//	//		string *myarray;
//	//		myarray = new string[n];
//	//		string *myarray2;
//	//		myarray2 = new string[n];
//	//		for (int i = 0; i < n; ++i) {
//	//			getline(file, myarray[i]);
//	//			getline(file2, myarray2[i]);
//	//		}
//
//	//		for (int j = 0; j < n; j++) {
//	//			stringstream numberfromstring(myarray[j]);
//	//			numberfromstring >> h_a[j];
//	//			stringstream numberfromstring2(myarray2[j]);
//	//			numberfromstring2 >> h_b[j];
//	//		}
//	//	}
//	//}
//	//catch (exception e) {}
//
//	for (int j = 0; j < n; j++) {
//		h_a[j] = (sinf(j)*sinf(j));
//		h_b[j] = (cosf(j)*cosf(j));
//	}
//
//	time_t start = clock();
//	for (i = 0; i < n; i++)
//		h_c[i] = h_a[i] * h_b[i];
//	double sum = 0;
//	for (i = 0; i<n; i++)
//		sum += h_c[i];
//	printf("final result: %f\n", sum);
//
//	time_t end = clock();
//	double time_spent = (double)(end - start)/(double) CLOCKS_PER_SEC;
//
//	printf("Time spent: %f\n", time_spent);
//
//
//	time_t endWhole = clock();
//	double time_spentWhole = (double)(endWhole - startWhole) / (double)CLOCKS_PER_SEC;
//	printf("Time spent whole: %f\n", time_spentWhole);
//	system("PAUSE");
//	return 0;
//}



//int main(int argc, wchar_t *argv[]) {
//	// Length of vectors
//	unsigned int n = 10000000;
//	double *h_a;
//	double *h_b;
//	double *h_c;
//
//	clock_t start, end;
//	double time_spent;
//
//	int bytes = n * sizeof(double);
//
//	// Allocate memory for each vector on host
//	h_a = (double*)malloc(bytes);
//	h_b = (double*)malloc(bytes);
//	h_c = (double*)malloc(bytes);
//	int i;
//
//	start = clock();
//	for (i = 0; i < n; i++)
//	{
//		h_a[i] = sinf(i)*sinf(i);
//		h_b[i] = cosf(i)*cosf(i);
//	}
//	end = clock();
//	time_spent = difftime(end, start) / CLOCKS_PER_SEC;
//	printf("Vrijeme generisanja: %f\n", time_spent);
//
//	start = clock();
//	for (i = 0; i < n; i++)
//		h_c[i] = h_a[i] * h_b[i];
//	end = clock();
//	time_spent = difftime(end, start) / CLOCKS_PER_SEC;
//	printf("Suma vektora: %f\n", time_spent);
//	//Sum up vector c and print result divided by n, this should equal 1 within error
//	double sum = 0;
//	start = clock();
//	for (i = 0; i<n; i++)
//		sum += h_c[i];
//	end = clock();
//	time_spent = (double)difftime(end, start)/CLOCKS_PER_SEC;
//	printf("Suma konacna: %f\n", time_spent);
//	printf("final result: %f\n", sum);
//
//	// release OpenCL resources
//
//	system("PAUSE");
//	return 0;
//}
