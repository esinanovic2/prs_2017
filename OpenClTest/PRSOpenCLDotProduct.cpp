#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <CL/opencl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <sstream>
#include <time.h>
#include <chrono>

using std::string;
using namespace std;
using namespace std::chrono;

int sequential(unsigned long n) {
	auto beginTotal = std::chrono::high_resolution_clock::now();
	float *h_a;
	float *h_b;
	float *h_c;

	int bytes = n * sizeof(float);

	// Allocate memory for each vector on host
	h_a = (float*)malloc(bytes);
	h_b = (float*)malloc(bytes);
	h_c = (float*)malloc(bytes);
	int i;

	for (i = 0; i < n; i++){
		h_a[i] = sinf(i)*sinf(i);
		h_b[i] = cosf(i)*cosf(i);
	}

	auto begin = std::chrono::high_resolution_clock::now();
	for (i = 0; i < n; i++)
		h_c[i] = h_a[i] * h_b[i];

	float sum = 0;
	for (i = 0; i<n; i++)
		sum += h_c[i];

	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Ukupno vrijeme množenja i sabiranja vektora a i b: " << duration_cast<std::chrono::nanoseconds>(finish - begin).count()*0.000000001 << " s" << std::endl;

	printf("Izracunati rezultat: %f\n", sum);

	auto finishTotal = std::chrono::high_resolution_clock::now();
	std::cout << "Ukupno vrijeme programa: " << duration_cast<std::chrono::nanoseconds>(finishTotal - beginTotal).count()*0.000000001 << " s" << std::endl;
	//system("PAUSE");
	return 0;
}

int parallelNoReduction(unsigned long n, size_t localSizeParameter) {
	// OpenCL kernel. Each work item takes care of one element of c
	auto beginTotal = std::chrono::high_resolution_clock::now();
	const char *kernelSource = "\n" \
		"#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
		"__kernel void vecAdd(  __global float *a,                       \n" \
		"                       __global float *b,                       \n" \
		"                       __global float *c,                       \n" \
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

	// Host input vectors
	float *h_a;
	float *h_b;
	// Host output vector
	float *h_c;

	// Device input buffers
	cl_mem d_a;
	cl_mem d_b;
	// Device output buffer
	cl_mem d_c;

	cl_platform_id cpPlatform;        // OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;               // context
	cl_command_queue queue;           // command queue
	cl_program program;               // program
	cl_kernel kernel;                 // kernel

									  // Size, in bytes, of each vector
	size_t bytes = n * sizeof(float);

	// Allocate memory for each vector on host
	h_a = (float*)malloc(bytes);
	h_b = (float*)malloc(bytes);
	h_c = (float*)malloc(bytes);

	size_t stringBytes = n * sizeof(string);

	for (int j = 0; j < n; j++) {
		h_a[j] = (sinf(j)*sinf(j));
		h_b[j] = (cosf(j)*cosf(j));
	}

	int i;

	size_t globalSize, localSize;
	cl_int err;

	// Number of work items in each local work group
	localSize = localSizeParameter;

	// Number of total work items - localSize must be devisor
	globalSize = ceil(n / (float)localSize)*localSize;

	// Bind to platform
	err = clGetPlatformIDs(1, &cpPlatform, NULL);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

	// Create a context  
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

	// Create a command queue 
	queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1,
		(const char **)& kernelSource, NULL, &err);

	// Build the program executable 
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	// Create the compute kernel in the program we wish to run
	kernel = clCreateKernel(program, "vecAdd", &err);

	// Create the input and output arrays in device memory for our calculation
	d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);


	// Write our data set into the input array in device memory
	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
		bytes, h_a, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
		bytes, h_b, 0, NULL, NULL);

	// Set the arguments to our compute kernel
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
	err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);

	auto begin = std::chrono::high_resolution_clock::now();
	// Execute the kernel over the entire range of the data set  
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
		0, NULL, NULL);

	// Wait for the command queue to get serviced before reading back results
	clFinish(queue);

	// Read the results from the device
	clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
		bytes, h_c, 0, NULL, NULL);

	//Sum up vector c and print result divided by n, this should equal 1 within error
	float sum = 0;
	for (i = 0; i<n; i++)
		sum += h_c[i];
	printf("final result: %f\n", sum);

	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Ukupno vrijeme mnozenja i sabiranja vektora a i b paralelno: " << duration_cast<std::chrono::nanoseconds>(finish - begin).count()*0.000000001 << " s" << std::endl;

	// release OpenCL resources
	clReleaseMemObject(d_a);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_c);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	//release host memory
	free(h_a);
	free(h_b);
	free(h_c);

	auto finishTotal = std::chrono::high_resolution_clock::now();
	std::cout << "Ukupno vrijeme programa: " << duration_cast<std::chrono::nanoseconds>(finishTotal - beginTotal).count()*0.000000001 << " s" << std::endl;
	//system("PAUSE");
	return 0;

}

int parallelReduction(unsigned long n, size_t localSizeParameter, int groupDivider) {
	auto beginTotal = std::chrono::high_resolution_clock::now();
	// Host input vectors
	float *h_a;
	float *h_b;
	// Host output vector
	float *h_c;

	// Device input buffers
	cl_mem d_a;
	cl_mem d_b;
	// Device output buffer
	cl_mem d_c;

	cl_platform_id cpPlatform;        // OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;               // context
	cl_command_queue queue;           // command queue
	cl_program program;               // program
	cl_program program_suma;
	cl_kernel kernel;                 // kernel
	cl_event prof_event;
	// Size, in bytes, of each vector
	size_t bytes = n * sizeof(float);

	cl_ulong time_start, time_end;
	cl_int num_groups;
	// Allocate memory for each vector on host
	h_a = (float*)malloc(bytes);
	h_b = (float*)malloc(bytes);

	// Initialize vectors on host
	int i;
	for (i = 0; i < n; i++) {
		h_a[i] = (sinf(i)*sinf(i));
		h_b[i] = (cosf(i)*cosf(i));
	}

	size_t globalSize, localSize;
	cl_int err;

	localSize = localSizeParameter;
	globalSize = ceil(n / (float)localSize)*localSize;
	printf("local = %d global = %d\n", localSize, globalSize);
	num_groups = n / groupDivider;
	h_c = (float*)malloc(num_groups * sizeof(float));
	err = clGetPlatformIDs(1, &cpPlatform, NULL);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

	// Create a context  
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	
	// Create the compute program from the source buffer
	FILE *fp;
	char *source_str;
	size_t source_size, program_size;

	fp = fopen("dot_product.cl", "rb");
	if (!fp) {
		printf("Failed to load kernel\n");
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	program_size = ftell(fp);
	rewind(fp);
	source_str = (char*)malloc(program_size + 1);
	source_str[program_size] = '\0';
	fread(source_str, sizeof(char), program_size, fp);
	fclose(fp);
	program = clCreateProgramWithSource(context, 1,
		(const char **)& source_str, &program_size, &err);

	if (program == NULL) {
		printf("Error while creating program!\n");
		return 1;
	}

	// Build the program executable 
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {
		size_t log_size = 0;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char* log = (char*)malloc(log_size + 1);
		log[log_size] = '\0';
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size + 1, log, NULL);
		printf("%s\n", log);
		free(log);
		return 1;
	}

	kernel = clCreateKernel(program, "dot_product", &err);

	if (err != NULL) {
		printf("Error while creating kernel! Error: %d\n", err);
		system("pause");
		return 1;
	}

	d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, num_groups * sizeof(float), NULL, &err);

	// Create a command queue 
	cl_queue_properties props[] = {
		CL_QUEUE_PROPERTIES,
		CL_QUEUE_PROFILING_ENABLE,
		0
	};

	queue = clCreateCommandQueueWithProperties(context, device_id, props, &err);

	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
		bytes, h_a, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
		bytes, h_b, 0, NULL, NULL);

	/* Create arguments for multiplication kernel */
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
	err |= clSetKernelArg(kernel, 3, localSize * 4 * sizeof(float), NULL);

	//Pocetak izvrsavanja kernela
	auto begin = std::chrono::high_resolution_clock::now();
	// Execute the kernel over the entire range of the data set  
	globalSize = n / 4;
	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
		0, NULL, &prof_event);
	// Wait for the command queue to get serviced before reading back results
	clFinish(queue);

	// Read the results from the device
	err = clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
		num_groups * sizeof(float), h_c, 0, NULL, NULL);

	// sekvencijalna suma

	//Sum up vector c and print result divided by n, this should equal 1 within error
	float sum = 0;
	for (i = 0; i<num_groups; i++)
		sum += h_c[i];

	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Ukupno vrijeme mnozenja i sabiranja vektora a i b paralelno: " << duration_cast<std::chrono::nanoseconds>(finish - begin).count()*0.000000001 << " s" << std::endl;
	printf("final result: %f\n", sum);

	// release OpenCL resources
	clReleaseMemObject(d_a);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_c);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	//release host memory
	free(h_a);
	free(h_b);
	free(h_c);

	auto finishTotal = std::chrono::high_resolution_clock::now();
	std::cout << "Ukupno vrijeme programa: " << duration_cast<std::chrono::nanoseconds>(finishTotal - beginTotal).count()*0.000000001 << " s" << std::endl;
	return 0;
}

int main() {	      //200000000 max number 
	/*
		10000	1249.986206
		50000	6250.014160
	   100000	12500.190430
	   500000	6250.375000
	  1000000	124999.765625
	 10000000	1250000.875000
	200000000	4198531.500000
	*/
	unsigned long n = 10000;
	size_t localSize = 256; 
	int groupDivider = 256;
	//int result = sequential(n);
	//int result = parallelNoReduction(n,localSize);
	int result = parallelReduction(n,localSize,groupDivider);
}