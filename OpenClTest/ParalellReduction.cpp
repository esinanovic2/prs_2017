// ParalelJedanKernel.cpp : Defines the entry point for the console application.
//

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

using std::string;
using namespace std;


//int main()
//{
//	system("PAUSE");
//	// Length of vectors
//	unsigned int n = 5000000;
//
//	// Host input vectors
//	float *h_a;
//	float *h_b;
//	// Host output vector
//	float *h_c;
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
//	cl_event prof_event;
//	// Size, in bytes, of each vector
//	size_t bytes = n * sizeof(float);
//
//	cl_ulong time_start, time_end;
//	cl_int num_groups;
//	// Allocate memory for each vector on host
//	h_a = (float*)malloc(bytes);
//	h_b = (float*)malloc(bytes);
//
//
//	clock_t start, end;
//	double time_spent;
//
//	// Initialize vectors on host
//	int i;
//	for (i = 0; i < n; i++) {
//		h_a[i] = (sinf(i)*sinf(i));
//		h_b[i] = (cosf(i)*cosf(i));
//	}
//
//	size_t globalSize, localSize;
//	cl_int err;
//
//	localSize = 256;
//	globalSize = ceil(n / (float)localSize)*localSize;
//	printf("local = %d global = %d\n", localSize, globalSize);
//	num_groups = n / 4;
//	h_c = (float*)malloc(num_groups * sizeof(float));
//	start = clock();
//
//	err = clGetPlatformIDs(1, &cpPlatform, NULL);
//
//	// Get ID for the device
//	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
//
//	// Create a context  
//	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
//
//
//	// Create the compute program from the source buffer
//	FILE *fp;
//	char *source_str;
//	size_t source_size, program_size;
//
//	fp = fopen("dot_product.cl", "rb");
//	if (!fp) {
//		printf("Failed to load kernel\n");
//		return 1;
//	}
//
//	fseek(fp, 0, SEEK_END);
//	program_size = ftell(fp);
//	rewind(fp);
//	source_str = (char*)malloc(program_size + 1);
//	source_str[program_size] = '\0';
//	fread(source_str, sizeof(char), program_size, fp);
//	fclose(fp);
//	program = clCreateProgramWithSource(context, 1,
//		(const char **)& source_str, &program_size, &err);
//
//	if (program == NULL) {
//		printf("Error while creating program!\n");
//		return 1;
//	}
//
//	// Build the program executable 
//	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
//	if (err < 0) {
//		size_t log_size = 0;
//		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
//		char* log = (char*)malloc(log_size + 1);
//		log[log_size] = '\0';
//		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size + 1, log, NULL);
//		printf("%s\n", log);
//		free(log);
//		return 1;
//	}
//
//
//	kernel = clCreateKernel(program, "dot_product", &err);
//
//	if (err != NULL) {
//		printf("Error while creating kernel! Error: %d\n", err);
//		system("pause");
//		return 1;
//	}
//
//	d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
//	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
//	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, num_groups * sizeof(float), NULL, &err);
//
//	// Create a command queue 
//	cl_queue_properties props[] = {
//		CL_QUEUE_PROPERTIES,
//		CL_QUEUE_PROFILING_ENABLE,
//		0
//	};
//
//	queue = clCreateCommandQueueWithProperties(context, device_id, props, &err);
//
//	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
//		bytes, h_a, 0, NULL, NULL);
//	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
//		bytes, h_b, 0, NULL, NULL);
//
//	/* Create arguments for multiplication kernel */
//	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
//	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
//	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
//	err |= clSetKernelArg(kernel, 3, localSize * 4 * sizeof(float), NULL);
//
//	end = clock();
//	time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
//	printf("Vrijeme inicijalizacije OpenCL: %.8f\n", time_spent);
//
//	//Pocetak izvrsavanja kernela
//	start = clock();
//	// Execute the kernel over the entire range of the data set  
//	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
//		0, NULL, &prof_event);
//	// Wait for the command queue to get serviced before reading back results
//	clFinish(queue);
//
//	end = clock();
//	time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
//
//	printf("Vrijeme paralelno: %.8f\n ", time_spent);
//	// Read the results from the device
//	err = clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
//		num_groups * sizeof(float), h_c, 0, NULL, NULL);
//
//	clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START,
//		sizeof(time_start), &time_start, NULL);
//	clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END,
//		sizeof(time_end), &time_end, NULL);
//	printf("On the device, computation completed in in %lu ns.\n",
//		(time_end - time_start));
//
//	// sekvencijalna suma
//
//	//Sum up vector c and print result divided by n, this should equal 1 within error
//	float sum = 0;
//	start = clock();
//	for (i = 0; i<num_groups; i++)
//		sum += h_c[i];
//
//	end = clock();
//	time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
//	printf("Vrijeme sekvencijalne sume: %.8f\n", time_spent);
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
//
//	return 0;
//}
