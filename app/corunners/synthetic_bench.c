/*
 * synthetic_bench.c
 *
 *  Created on: Nov 11, 2019
 *      Author: Caspar Treijtel
 
*/

#include "synthetic_bench.h"
#include <stddef.h>

int array_access_linear(volatile bigstruct_t* data)
{
	int sum = 0;
	if (data != NULL) {
		for (int i=0; i<SYNBENCH_DATASIZE; ++i) {
			sum += data[i].id;
		}
	}
	return sum;
}

void array_write_linear(volatile bigstruct_t* data)
{
	if (data != NULL) {
		for (int i=0; i<SYNBENCH_DATASIZE; ++i) {
			data[i].id = 0xff;
		}
	}
}

void array_access_randomize(volatile int* idx, int corenum)
{
	int nNumber = 50;
	for (int i=0; i<SYNBENCH_DATASIZE; i++) {
		idx[i] = nNumber % SYNBENCH_DATASIZE;
	}
}

int array_access_random(volatile bigstruct_t* data, volatile int* idx)
{
	int sum = 0;
	if (data != NULL && idx != NULL) {
		for (int i=0; i<SYNBENCH_DATASIZE; ++i) {
			sum += data[idx[i]].id;
		}
	}
	return sum;
}

void array_write_random(volatile bigstruct_t* data, volatile int* idx)
{
	if (data != NULL && idx != NULL) {
		for (int i=0; i<SYNBENCH_DATASIZE; ++i) {
			data[idx[i]].id = 0xff;
		}
	}
}

void array_access_alternate(volatile bigstruct_t* data)
{
	/* TODO: make array access alternate! */
	if (data != NULL) {
		for (int i=0; i<SYNBENCH_DATASIZE; ++i) {
			data[i].id;
		}
	}
}
