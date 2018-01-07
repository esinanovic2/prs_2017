// ConsoleApplication3.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <CL/opencl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <sstream>

using std::string;
using namespace std;

// OpenCL kernel. Each work item takes care of one element of c
const char *kernelSource = "\n" \
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
"__kernel void vecAdd(  __global double *a,                       \n" \
"                       __global double *b,                       \n" \
"                       __global double *c,                       \n" \
"                       const unsigned int n)                    \n" \
"{                                                               \n" \
"    //Get our global thread ID                                  \n" \
"    int id = get_global_id(0);                                  \n" \
"                                                                \n" \
"    //Make sure we do not go out of bounds                      \n" \
"    if (id < n)                                                 \n" \
"        c[id] = a[id] * b[id];                                  \n" \
"}                                                               \n" \
"\n";


const char *kernelSum = "\n" \
"#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
"__kernel void reduction_vector(__global float4* data, __local float4* partial_sums, __global float* output) \n" \
"{																											 \n" \
"	int lid = get_local_id(0);																				 \n" \
"	int group_size = get_local_size(0);																		 \n" \
"	partial_sums[lid] = data[get_global_id(0)];																 \n" \
"	barrier(CLK_LOCAL_MEM_FENCE);																			 \n" \
"																											 \n" \
"	for (int i = group_size / 2; i>0; i >>= 1) {															 \n" \
"		if (lid < i) {																						 \n" \
"			partial_sums[lid] += partial_sums[lid + i];														 \n" \
"		}																									 \n" \
"		barrier(CLK_LOCAL_MEM_FENCE);																		 \n" \
"	}																										 \n" \
"																											 \n" \
"	if (lid == 0) {																							 \n" \
"		output[get_group_id(0)] = dot(partial_sums[0], (float4)(1.0f));										 \n" \
"	}																									     \n" \
"}																											 \n" \
"\n";


//int main() {
//	time_t startWhole = clock();
//	// Length of vectors
//	unsigned int n = 5000000;
//
//	// Host input vectors
//	double *h_a;
//	double *h_b;
//	// Host output vector
//	double *h_c;
//
//	// Device input buffers
//	cl_mem d_a;
//	cl_mem d_b;
//	// Device output buffer
//	cl_mem d_c;
//
//	cl_platform_id cpPlatform;        // OpenCL platform
//	cl_device_id device_id;           // device ID
//	cl_context context;               // context
//	cl_command_queue queue;           // command queue
//	cl_program program;               // program
//	cl_kernel kernel;                 // kernel
//
//									  // Size, in bytes, of each vector
//	size_t bytes = n * sizeof(double);
//
//	// Allocate memory for each vector on host
//	h_a = (double*)malloc(bytes);
//	h_b = (double*)malloc(bytes);
//	h_c = (double*)malloc(bytes);
//
//	size_t stringBytes = n * sizeof(string);
//
//	//try {
//	//	ifstream file("doubleVektor1.txt");
//	//	ifstream file2("doubleVektor2.txt");
//	//	if (file.is_open()){
//	//		string *myArray;
//	//		myArray= new string[n];
//	//		string *myArray2;
//	//		myArray2 = new string[n];
//	//		for (int i = 0; i < n; ++i){
//	//			getline(file, myArray[i]);
//	//			getline(file2, myArray2[i]);
//	//		}
//	//		for (int j = 0; j < n; j++) {
//	//			stringstream numberFromString(myArray[j]);
//	//			numberFromString >> h_a[j];
//	//			stringstream numberFromString2(myArray2[j]);
//	//			numberFromString2 >> h_b[j];
//	//		}
//	//	}
//	//}
//	//catch (exception e) {
//	//	
//	//}
//
//	for (int j = 0; j < n; j++) {
//		h_a[j] = (sinf(j)*sinf(j));
//		h_b[j] = (cosf(j)*cosf(j));
//	}
//
//	int i;
//
//	size_t globalSize, localSize;
//	cl_int err;
//
//	// Number of work items in each local work group
//	localSize = 64;
//
//	// Number of total work items - localSize must be devisor
//	globalSize = ceil(n / (float)localSize)*localSize;
//
//	// Bind to platform
//	err = clGetPlatformIDs(1, &cpPlatform, NULL);
//
//	// Get ID for the device
//	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
//
//	// Create a context  
//	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
//
//	// Create a command queue 
//	queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
//
//	// Create the compute program from the source buffer
//	program = clCreateProgramWithSource(context, 1,
//		(const char **)& kernelSource, NULL, &err);
//
//	// Build the program executable 
//	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
//
//	// Create the compute kernel in the program we wish to run
//	kernel = clCreateKernel(program, "vecAdd", &err);
//
//	// Create the input and output arrays in device memory for our calculation
//	d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
//	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
//	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);
//
//
//	// Write our data set into the input array in device memory
//	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
//		bytes, h_a, 0, NULL, NULL);
//	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
//		bytes, h_b, 0, NULL, NULL);
//
//	// Set the arguments to our compute kernel
//	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
//	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
//	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
//	err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);
//
//	time_t start = clock();
//	// Execute the kernel over the entire range of the data set  
//	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
//		0, NULL, NULL);
//
//	// Wait for the command queue to get serviced before reading back results
//	clFinish(queue);
//
//	// Read the results from the device
//	clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
//		bytes, h_c, 0, NULL, NULL);
//
//	//Sum up vector c and print result divided by n, this should equal 1 within error
//	double sum = 0;
//	for (i = 0; i<n; i++)
//		sum += h_c[i];
//	printf("final result: %f\n", sum);
//
//	time_t end = clock();
//	double time_spent = (double)(end - start)/(double) CLOCKS_PER_SEC;
//
//	printf("Time spent parallel: %f\n", time_spent);
//
//	// release OpenCL resources
//	clReleaseMemObject(d_a);
//	clReleaseMemObject(d_b);
//	clReleaseMemObject(d_c);
//	clReleaseProgram(program);
//	clReleaseKernel(kernel);
//	clReleaseCommandQueue(queue);
//	clReleaseContext(context);
//
//	//release host memory
//	free(h_a);
//	free(h_b);
//	free(h_c);
//
//	time_t endWhole = clock();
//	double time_spentWhole = (double)(endWhole - startWhole) / (double)CLOCKS_PER_SEC;
//	printf("Time spent whole: %f\n", time_spentWhole);
//	system("PAUSE");
//	return 0;
//}



//using std::string;
//using namespace std;
//
//// OpenCL kernel. Each work item takes care of one element of c
//
//const char *kernelSource = "\n" \
//"#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
//"__kernel void vecAdd(  __global double *a,                       \n" \
//"                       __global double *b,                       \n" \
//"                       __global double *c,                       \n" \
//"                       const unsigned int n)                    \n" \
//"{                                                               \n" \
//"    //Get our global thread ID                                  \n" \
//"    int id = get_global_id(0);                                  \n" \
//"                                                                \n" \
//"    //Make sure we do not go out of bounds                      \n" \
//"    if (id < n)                                                 \n" \
//"        c[id] = a[id] * b[id];                                  \n" \
//"}                                                               \n" \
//"\n";
//
//const char *kernel_sum = "\n" \
//"#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
//"__kernel void sum (__global const double *c,						\n" \
//"					__global double *partialSums,					\n" \
//"					__local double *localSums)						\n" \
//"{																	\n" \
//"		uint local_id = get_local_id(0);							\n" \
//"		uint group_size = get_local_size(0);						\n" \
//"		localSums[local_id] = input[get_global_id(0)];				\n" \
//"		for(uint stride = group_size/2; stride>0; stride /=2){		\n" \
//"			barrier(CLK_LOCAL_MEM_FENCE);							\n" \
//"			if(local_id < stride)									\n" \
//"				localSums[local_id] += localSums[local_id + stride];\n" \
//"		}															\n" \
//"		if(local_id == 0)											\n" \
//"			partialSums[get_group_id(0)] = localSums[0];			\n" \
//"}																	\n" \
//"\n";

//int main() {
//	// Length of vectors
//	unsigned int n = 10000000;
//
//	// Host input vectors
//	double *h_a;
//	double *h_b;
//	// Host output vector
//	double *h_c;
//
//	// Device input buffers
//	cl_mem d_a;
//	cl_mem d_b;
//	// Device output buffer
//	cl_mem d_c;
//
//	cl_platform_id cpPlatform;        // OpenCL platform
//	cl_device_id device_id;           // device ID
//	cl_context context;               // context
//	cl_command_queue queue;           // command queue
//	cl_program program;               // program
//	cl_program program_suma;
//	cl_kernel kernel;                 // kernel
//	cl_kernel kernel_suma;
//	// Size, in bytes, of each vector
//	size_t bytes = n * sizeof(double);
//
//	// Allocate memory for each vector on host
//	h_a = (double*)malloc(bytes);
//	h_b = (double*)malloc(bytes);
//	h_c = (double*)malloc(bytes);
//
//	clock_t start, end;
//	double time_spent;
//
//	// Initialize vectors on host
//	int i;
//	for (i = 0; i < n; i++) {
//		h_a[i] = sinf(i)*sinf(i);
//		h_b[i] = cosf(i)*cosf(i);
//	}
//
//	size_t globalSize, localSize;
//	cl_int err;
//
//	// Number of work items in each local work group
//	localSize = 32;
//
//	// Number of total work items - localSize must be devisor
//	globalSize = ceil(n / (float)localSize)*localSize;
//
//	// Bind to platform
//	err = clGetPlatformIDs(1, &cpPlatform, NULL);
//
//	// Get ID for the device
//	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
//
//	// Create a context  
//	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
//
//	// Create a command queue 
//	queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
//
//	// Create the compute program from the source buffer
//	program = clCreateProgramWithSource(context, 1,
//		(const char **)& kernelSource, NULL, &err);
//
//	/*	program_suma = clCreateProgramWithSource(context, 1,
//	(const char **)& kernel_sum, NULL, &err);*/
//
//	// Build the program executable 
//	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
//	//clBuildProgram(program_suma, 0, NULL, NULL, NULL, NULL);
//	// Create the compute kernel in the program we wish to run
//	kernel = clCreateKernel(program, "vecAdd", &err);
//	//	kernel_suma = clCreateKernel(program_suma, "sum", &err); //treba uvesti posebne varijable
//	// Create the input and output arrays in device memory for our calculation
//
//	d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
//	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
//	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);
//
//	// Write our data set into the input array in device memory
//	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
//		bytes, h_a, 0, NULL, NULL);
//	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
//		bytes, h_b, 0, NULL, NULL);
//
//	// Set the arguments to our compute kernel
//	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
//	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
//	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
//	err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);
//	//Pocetak izvrsavanja kernela
//	start = clock();
//	// Execute the kernel over the entire range of the data set  
//	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
//		0, NULL, NULL);
//
//	// Wait for the command queue to get serviced before reading back results
//	clFinish(queue);
//	end = clock();
//	time_spent = difftime(end, start)/CLOCKS_PER_SEC;
//	printf("Vrijeme paralelno: %f\n ", time_spent);
//	// Read the results from the device
//	clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
//		bytes, h_c, 0, NULL, NULL);
//
//	// sekvencijalna suma
//
//	//Sum up vector c and print result divided by n, this should equal 1 within error
//	double sum = 0;
//	start = clock();
//	for (i = 0; i<n; i++)
//		sum += h_c[i];
//	end = clock();
//	time_spent = difftime(end, start)/CLOCKS_PER_SEC;
//	printf("Vrijeme sekvencijalne sume: %f\n ", time_spent);
//	printf("final result: %f\n", sum);
//
//	// release OpenCL resources
//	clReleaseMemObject(d_a);
//	clReleaseMemObject(d_b);
//	clReleaseMemObject(d_c);
//	clReleaseProgram(program);
//	clReleaseKernel(kernel);
//	clReleaseCommandQueue(queue);
//	clReleaseContext(context);
//
//	//release host memory
//	free(h_a);
//	free(h_b);
//	free(h_c);
//
//	system("PAUSE");
//	return 0;
//}