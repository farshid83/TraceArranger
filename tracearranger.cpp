/*
 * testdisk.cpp
 *
 *  Created on: Sep 19, 2014
 *      Author: user
 */

#include "tracearranger.h"

int main ( int arc, char **argv )
{//StatCol filename
	/////////////////////////Just4debug
	string logfilepath = "/home/user/workspace/disksim-4.0/frashtest/frash_simpledisk.trace"; //argv[1]; //"Debug/output";
	int warmupnum = 1000;
	TRACE trace[100000];
	///////////////////////////////////
	ifstream logfile (&logfilepath[0]); // command line program gets log filename
	int ti = 0;
	if (logfile.is_open())
	{
		while(warmupnum) {
			getline (logfile,line); // warmup
			warmupnum--;
		}
		while ( (ti<10000) && getline (logfile,line) )
		{
			int i = 0;
			//traces[ti].id = GetNextNumber(&i);
			trace[ti].at = GetNextNumber(&line,&i);
			trace[ti].dn = GetNextNumber(&line,&i);
			trace[ti].bn = GetNextNumber(&line,&i);
			trace[ti].sz = GetNextNumber(&line,&i);
			trace[ti].md = GetNextNumber(&line,&i);
			trace[ti].dt = GetNextNumber(&line,&i);
			//if( traces[ti].at < 1)
			//	traces[ti].at = 0;
			ti++;
		}
	}
	logfile.close();

	ull tSize = 0;
	for(int i = 0; i < ti; i++)
	{
		tSize += trace[i].sz;
	}

	cout << (double) ((1000 * tSize) / (trace[ti-1].at - trace[0].at)) << "\n";
	cout << (double) ((1000 * tSize) / (trace[ti-1].dt - trace[0].dt)) << "\n";

	return 0;
}

void GetTraceStat(string tracefile, int iTF, int warmupnum)
{
	ifstream logfile;
	logfile.open(&tracefile[0]); // command line program gets log filename
	ull tByte = 0;
	ull tIO = 0;
	ull min = 1000000000; //traces[0].bn;
	ull max = 0; //traces[0].bn + traces[0].sz;
	//int wunum = WARMUPNUM; // warm up
	ull ti = 0;
	if (logfile.is_open())
	{
		int i = 0;
		while(warmupnum) {
			getline (logfile,line); // warmup
			warmupnum--;
		}
		getline (logfile,line);
		GetNextNumber(&line,&i);
		ldbl BaseIAT = GetNextNumber(&line,&i) / 1000;
		do
		{
			i = 0;
			GetNextNumber(&line,&i); // Just to skip the row number
			traces[iTF][ti].at = (GetNextNumber(&line,&i) - BaseIAT) / 1000; // us -> ms
			traces[iTF][ti].dn = GetNextNumber(&line,&i);
			traces[iTF][ti].fd = GetNextNumber(&line,&i);
			traces[iTF][ti].md = GetNextNumber(&line,&i);
			traces[iTF][ti].bn = (ull)(GetNextNumber(&line,&i));
			traces[iTF][ti].sz = (ull)(GetNextNumber(&line,&i));
			if(traces[iTF][ti].bn < min)
				min = traces[iTF][ti].bn;
			if(traces[iTF][ti].bn + traces[iTF][ti].sz > max)
				max = traces[iTF][ti].bn + traces[iTF][ti].sz;
			tByte += traces[iTF][ti].sz;
			tIO ++;
			if(traces[iTF][ti].sz) // sometimes bn == sz == 0 !!!
				ti++;
			//else
			//	printf("is zero!\n");
		} while ( (ti < MaxTraceSize) && getline (logfile,line) );
		tracestats[iTF].TraceSize = ti;
	}
	logfile.close();

	//cout << (ldbl) ((1000 * tSize) / (traces[tracestats[iTF].TraceSize - 1].at - traces[0].at)) << "\n";
	//cout << (ldbl) ((1000 * tSize) / (traces[tracestats[iTF].TraceSize - 1].dt - traces[0].dt)) << "\n";
	// Primary Requirements
	tracestats[iTF].sz = max; // - min + 1;
	tracestats[iTF].bw = (ldbl) (tByte * (1000 / (traces[iTF][tracestats[iTF].TraceSize - 1].at - traces[iTF][0].at))); // Bytes/s
	tracestats[iTF].iops = (ldbl) (tIO * (1000 / (traces[iTF][tracestats[iTF].TraceSize - 1].at - traces[iTF][0].at))); // IO/s
	cout << "App[" << iTF << "]: MaxBlockNum = " << tracestats[iTF].sz << ", IOPS = " << tracestats[iTF].iops << ", BW = " << tracestats[iTF].bw << "\n";
	//tracestats[index].rt;
}

ldbl GetMaxThroughput(string tracefile, int iTF, int warmupnum)
{
	ifstream logfile (&tracefile[0]); // command line program gets log filename
	int itr = 0;
	ull tByte = 0;
	ull tIO = 0;
	//TRACE traces[1000000];//[MaxTraceSize];
	if (logfile.is_open())
	{
		while(warmupnum) {
			getline (logfile,line); // warmup
			warmupnum--;
		}
		while ( ( itr < MaxTraceSize ) && getline (logfile,line) )
		{
			int i = 0;
			traces[iTF][itr].id = GetNextNumber(&line, &i);
			traces[iTF][itr].at = GetNextNumber(&line, &i);
			traces[iTF][itr].dn = GetNextNumber(&line, &i);
			traces[iTF][itr].bn = GetNextNumber(&line, &i);
			traces[iTF][itr].sz = GetNextNumber(&line, &i);
			traces[iTF][itr].md = GetNextNumber(&line, &i);
			traces[iTF][itr].dt = GetNextNumber(&line, &i);
			//if( traces[iTF][itr].at < 1) // just for debug
			//	traces[iTF][itr].at = 0; // just for debug
			tByte += traces[iTF][itr].sz;
			tIO ++;
			itr++;
		}
	}
	logfile.close();

	//ldbl bps = (ldbl) ((1000 * tByte) / (traces[iTF][itr-1].dt - traces[iTF][0].at));
	ldbl iops = (ldbl) ((1000 * tIO) / (traces[iTF][itr-1].dt - traces[iTF][0].at));
	return iops;
}

ldbl GetNextNumber(string* Line, int* i)
{
	//string line = (*Line);
	ldbl num = 0;
	unsigned mode = 0;
	while(  ((*i)<(int)(Line->length())) &&
			( 		   (((int)((*Line)[*i])) < (int)'0' )
					|| (((int)((*Line)[*i])) > (int)'9' )
					//|| (((int)((*Line)[*i])) == 'W')
					//|| (((int)((*Line)[*i])) == 'R')
					) )
	{
		if( ((int)((*Line)[*i])) == 'W')
			mode = (unsigned)('W');
		if( ((int)((*Line)[*i])) == 'R')
			mode = (unsigned)('R');
		(*i)++;
	}
	if(mode)
		return mode;

	while(  ((*i)<(int)(Line->length()))
			&& ( ((int)((*Line)[*i])) >= (int)'0' ) && (((int)((*Line)[*i])) <= (int)'9' ) )
	{
		num *= 10;
		num += (ldbl)((unsigned)((*Line)[*i])) - (unsigned)('0');
		(*i)++;
	}
	if( ((int)((*Line)[*i])) == '.' ) {
		(*i)++;
		ldbl dec = 1;
		while(  ((*i)<(int)(Line->length()))
				&& ( ((int)((*Line)[*i])) >= (int)'0' ) && (((int)((*Line)[*i])) <= (int)'9' ) )
		{
			dec /= 10;
			num += (dec * ((ldbl)((unsigned)((*Line)[*i])) - (unsigned)('0')));
			(*i)++;
		}
	}
	return num;
}
