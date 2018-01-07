#include <stdio.h>
#include "stdafx.h"
#include <stdlib.h>
#include <malloc.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <sstream>



#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

//int main(void) {
//	LARGE_INTEGER frequency;        // ticks per second
//	LARGE_INTEGER t1, t2,tp1,tp2;           // ticks
//	double elapsedTime,elapsedParallelTime;
//
//	// get ticks per second
//	QueryPerformanceFrequency(&frequency);
//
//	// start timer
//	QueryPerformanceCounter(&t1);
//
//	time_t startWhole = clock();
//	printf("started running\n");
//	using std::string;
//	using namespace std;
//
//	// Create the two input vectors
//	int i;
//	const int LIST_SIZE = 5000000;
//	const int LOCAL_ITEM_SIZE = 1;
//	
//	printf("Alocating arrays\n");
//	double *A = (double*)malloc(sizeof(double)*LIST_SIZE);
//	double *B = (double*)malloc(sizeof(double)*LIST_SIZE);
//	string *myArray;
//	myArray = new string[LIST_SIZE];
//	string *myArray2;
//	myArray2 = new string[LIST_SIZE];
//
//	printf("Filling arrays\n");
//	//try {
//	//	ifstream file("doubleVektor1.txt");
//	//	ifstream file2("doubleVektor2.txt");
//	//	if (file.is_open()) {
//	//		for (int i = 0; i < LIST_SIZE; ++i) {
//	//			getline(file, myArray[i]);
//	//			getline(file2, myArray2[i]);
//	//		}
//	//		for (int j = 0; j < LIST_SIZE; j++) {
//	//			stringstream numberFromString(myArray[j]);
//	//			numberFromString >> A[j];
//	//			stringstream numberFromString2(myArray2[j]);
//	//			numberFromString2 >> B[j];
//	//		}
//	//	}
//	//}
//	//catch (exception e) {
//
//	//}
//
//
//	for (int j = 0; j < LIST_SIZE; j++) {
//		A[j] = j * j*3;
//		B[j] = j*j*7;
//	}
//
//
//	printf("Arrays filled\n");
//
//	// Load the kernel source code into the array source_str
//	FILE *fp;
//	char *source_str;
//	size_t source_size;
//
//	fp = fopen("kernel.cl", "r");
//	if (!fp) {
//		fprintf(stderr, "Failed to load kernel.\n");
//		exit(1);
//	}
//	source_str = (char*)malloc(MAX_SOURCE_SIZE);
//	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
//	fclose(fp);
//	printf("kernel loading done\n");
//	// Get platform and device information
//
//	cl_device_id device_id = NULL;
//	cl_uint ret_num_devices;
//	cl_uint ret_num_platforms;
//	cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
//	cl_platform_id *platforms = NULL;
//
//	platforms = (cl_platform_id*)malloc(ret_num_platforms * sizeof(cl_platform_id));
//
//	ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
//
//	ret = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,1,&device_id,&ret_num_devices);
//	
//	// Create an OpenCL context
//	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
//
//	// Create a command queue
//	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
//
//	// Create memory buffers on the device for each vector 
//	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
//		LIST_SIZE * sizeof(double), NULL, &ret);
//	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
//		LIST_SIZE * sizeof(double), NULL, &ret);
//	cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
//		LIST_SIZE * sizeof(double), NULL, &ret);
//
//	// Copy the lists A and B to their respective memory buffers
//	ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
//		LIST_SIZE * sizeof(double), A, 0, NULL, NULL);
//
//	ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
//		LIST_SIZE * sizeof(double), B, 0, NULL, NULL);
//
//	printf("before building\n");
//	// Create a program from the kernel source
//	cl_program program = clCreateProgramWithSource(context, 1,
//		(const char **)&source_str, (const size_t *)&source_size, &ret);
//
//	// Build the program
//	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
//
//	printf("after building\n");
//	// Create the OpenCL kernel
//	cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
//
//	// Set the arguments of the kernel
//	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
//
//	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
//
//	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
//
//	// Read the memory buffer C on the device to the local variable C
//	double *C = (double*)malloc(sizeof(double)*LIST_SIZE);
//
//	//added this to fix garbage output problem
//	//ret = clSetKernelArg(kernel, 3, sizeof(int), &LIST_SIZE);
//	time_t start = clock();
//	QueryPerformanceCounter(&tp1);
//	printf("before execution\n");
//	// Execute the OpenCL kernel on the list
//	size_t global_item_size = LIST_SIZE; // Process the entire lists
//	size_t local_item_size = LOCAL_ITEM_SIZE; // Divide work items into groups of 64
//	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
//		&global_item_size, &local_item_size, 0, NULL, NULL);
//	printf("after execution\n");
//
//	ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
//		LIST_SIZE * sizeof(double), C, 0, NULL, NULL);
//	printf("after copying\n");
//
//	double sum = 0;
//	for (i = 0; i<LIST_SIZE; i++)
//		sum += C[i];
//	printf("final result: %f\n", sum);
//	QueryPerformanceCounter(&tp2);
//	time_t end = clock();
//	double time_spent = (double)(end - start)/(double) CLOCKS_PER_SEC;
//
//	printf("Time spent paralell: %f\n", time_spent);
//
//	elapsedParallelTime = (tp2.QuadPart - tp1.QuadPart) * 1000 / frequency.QuadPart;
//	printf("Time spent paralell 2: %f\n", elapsedParallelTime / 1000.0);
//
//	// Display the result to the screen
//	//for (i = 0; i < LIST_SIZE; i++)
//	//	printf("%d * %d = %d\n", A[i], B[i], C[i]);
//
//	// Clean up
//	ret = clFlush(command_queue);
//	ret = clFinish(command_queue);
//	ret = clReleaseKernel(kernel);
//	ret = clReleaseProgram(program);
//	ret = clReleaseMemObject(a_mem_obj);
//	ret = clReleaseMemObject(b_mem_obj);
//	ret = clReleaseMemObject(c_mem_obj);
//	ret = clReleaseCommandQueue(command_queue);
//	ret = clReleaseContext(context);
//	free(A);
//	free(B);
//	free(C);
//	//time_t endWhole = clock();
//	//double time_spentWhole = (double)(endWhole - startWhole) / (double)CLOCKS_PER_SEC;
//	//printf("Time spent whole: %f\n", time_spentWhole);
//
//	// stop timer
//	QueryPerformanceCounter(&t2);
//
//	// compute and print the elapsed time in millisec
//	elapsedTime = (t2.QuadPart - t1.QuadPart)*1000 / frequency.QuadPart;
//	printf("Time spent whole: %f\n", elapsedTime/1000.0);
//  system("PAUSE");
//	return 0;
//}