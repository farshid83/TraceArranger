/*
 * tracearranger.h
 *
 *  Created on: Jan 16, 2015
 *      Author: user
 */

#ifndef TRACEARRANGER_H_
#define TRACEARRANGER_H_

#define ull unsigned long long
#define ldbl long double
#define MaxTraceSize 1000000
#define MAX_TRACE_NUM 10

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct stat st = {0};

struct TRACE
{
	ull id; //identifier
	double at; //arrival time
	int dn; //device number
	int fd; //file descriptor
	int md; //read/write
	ull bn; //logical block address
	ull sz; //number of blocks
	double dt; //departure time
};

TRACE traces [MAX_TRACE_NUM][MaxTraceSize];
string line;

struct TRACESTATS
{
	ull TraceSize;
	ull sz;
	ldbl bw;
	ldbl iops;
};

TRACESTATS tracestats[MAX_TRACE_NUM];

ldbl GetNextNumber(string* Line, int* i);
ldbl GetMaxThroughput(string tracefile, int iTF, int warmupnum);


#endif /* TRACEARRANGER_H_ */
