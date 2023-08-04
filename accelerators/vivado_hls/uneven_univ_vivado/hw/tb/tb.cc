// Copyright (c) 2011-2023 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0
#include "../inc/espacc_config.h"
#include "../inc/espacc.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {

    printf("****start*****\n");

    /* <<--params-->> */
	 const unsigned data_out_size = 2;
	 const unsigned data_in_size = 8192;
	const unsigned data_stop = 8100;
	const unsigned data_L = 2;

    uint32_t in_words_adj;
    uint32_t out_words_adj;
    uint32_t in_size;
    uint32_t out_size;
    uint32_t dma_in_size;
    uint32_t dma_out_size;
    uint32_t dma_size;


    in_words_adj = round_up(data_in_size, VALUES_PER_WORD);
    out_words_adj = round_up(data_out_size, VALUES_PER_WORD);
    in_size = in_words_adj * (1);
    out_size = out_words_adj * (1);

    dma_in_size = in_size / VALUES_PER_WORD;
    dma_out_size = out_size / VALUES_PER_WORD;
    dma_size = dma_in_size + dma_out_size;

    dma_word_t *mem=(dma_word_t*) malloc(dma_size * sizeof(dma_word_t));
    word_t *inbuff=(word_t*) malloc(in_size * sizeof(word_t));
    f_word_t *outbuff=(f_word_t*) malloc(out_size * sizeof(f_word_t));
    f_word_t *outbuff_gold= (f_word_t*) malloc(out_size * sizeof(f_word_t));
    dma_info_t load;
    dma_info_t store;

    // Prepare input data
    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < data_in_size; j++)
            inbuff[i * in_words_adj + j] = (word_t) (j%5)%2;

    for(unsigned i = 0; i < dma_in_size; i++)
	for(unsigned k = 0; k < VALUES_PER_WORD; k++)
	    mem[i].word[k] = inbuff[i * VALUES_PER_WORD + k];

    int		i, j, p, L, Q, K, n;
	double	arg, sqrt2, sigma, phi, sum, p_value, c;
	long	*T, decRep;
	double	expected_value[17] = { 0, 0, 0, 0, 0, 0, 5.2177052, 6.1962507, 7.1836656,
				8.1764248, 9.1723243, 10.170032, 11.168765,
				12.168070, 13.167693, 14.167488, 15.167379 };
	double   variance[17] = { 0, 0, 0, 0, 0, 0, 2.954, 3.125, 3.238, 3.311, 3.356, 3.384,
				3.401, 3.410, 3.416, 3.419, 3.421 };
	
	L = data_L;
	n = data_stop;
	
	
	Q = 10*(int)pow(2, L);
	K = (int) (floor(n/L) - (double)Q);	 		    /* BLOCKS TO TEST */
	
	p = (int)pow(2, L);

	T = (long *)calloc(p, sizeof(long));
	
	c = 0.7 - 0.8/(double)L + (4 + 32/(double)L)*pow(K, -3/(double)L)/15;
	sigma = c * sqrt(variance[L]/(double)K);
	sqrt2 = sqrt(2);
	sum = 0.0;
	for ( i=0; i<p; i++ )
		T[i] = 0;
	for ( i=1; i<=Q; i++ ) {		/* INITIALIZE TABLE */
		decRep = 0;
		for ( j=0; j<L; j++ )
			decRep += inbuff[(i-1)*L+j] * (long)pow(2, L-1-j);
		T[decRep] = i;
	}
	for ( i=Q+1; i<=Q+K; i++ ) { 	/* PROCESS BLOCKS */
		decRep = 0;
		for ( j=0; j<L; j++ )
			decRep += inbuff[(i-1)*L+j] * (long)pow(2, L-1-j);
		sum += log(i - T[decRep])/log(2);
		T[decRep] = i;
	}
	std::cout << "sum is" << sum << std::endl;
	phi = (double)(sum/(double)K);

	arg = fabs(phi-expected_value[L])/(sqrt2 * sigma);
	p_value = erfc(arg);
    // Set golden output
    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < data_out_size; j++)
            outbuff_gold[i * out_words_adj + j] = (f_word_t) p_value;

    dma_f_word_t* f_mem = (dma_f_word_t*)mem;
    // Call the TOP function
    top(f_mem, mem,
        /* <<--args-->> */
	 	 data_out_size,
	 	 data_in_size,
		 data_stop,
		 data_L,
        load, store);

    // Validate
    uint32_t out_offset = dma_in_size;
    for(unsigned i = 0; i < dma_out_size; i++)
	for(unsigned k = 0; k < VALUES_PER_WORD; k++)
	    outbuff[i * VALUES_PER_WORD + k] = f_mem[out_offset + i].word[k];

    int errors = 0;
    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < data_out_size; j++)
	    if (outbuff[i * out_words_adj + j] != outbuff_gold[i * out_words_adj + j])
		errors++;

    if (errors)
	std::cout << "Test FAILED with " << errors << " errors.  " << outbuff[0].to_float() << "   " << outbuff_gold[0] << "    " << outbuff[1].to_float() << "   " << outbuff_gold[1] << std::endl<< std::endl;
    else
	std::cout << "Test PASSED." << std::endl;

    // Free memory

    free(mem);
    free(inbuff);
    free(outbuff);
    free(outbuff_gold);

    return 0;
}
