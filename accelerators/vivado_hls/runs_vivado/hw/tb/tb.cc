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
            inbuff[i * in_words_adj + j] = (word_t) (j%451)%2;

    for(unsigned i = 0; i < dma_in_size; i++)
	for(unsigned k = 0; k < VALUES_PER_WORD; k++)
	    mem[i].word[k] = inbuff[i * VALUES_PER_WORD + k];

    int		S, k;
	float	pi, V, erfc_arg, p_value;

	S = 0;
	for ( k=0; k<data_in_size; k++ )
		if ( inbuff[k] )
			S++;
	pi = (float)S / (float)data_in_size;

	if ( fabs(pi - 0.5) > (2.0 / sqrt(data_in_size)) ) {
		p_value = 0.0;
	}
	else {

		V = 1;
		for ( k=1; k<data_in_size; k++ )
			if ( inbuff[k] != inbuff[k-1] )
				V++;
	
		erfc_arg = fabs(V - 2.0 * data_in_size * pi * (1-pi)) / (2.0 * pi * (1-pi) * sqrt(2*data_in_size));
		p_value = erfc(erfc_arg);

      
		}
      std::cout << "p_value is" << p_value<<std::endl;
      
      std::cout << "V is " << V <<std::endl;
      
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
	std::cout << "Test FAILED with " << errors << " errors.  " << outbuff[0].to_float() << "   " << outbuff_gold[0] << std::endl;
    else
	std::cout << "Test PASSED." << std::endl;

    // Free memory

    free(mem);
    free(inbuff);
    free(outbuff);
    free(outbuff_gold);

    return 0;
}
