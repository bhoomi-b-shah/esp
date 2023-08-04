// Copyright (c) 2011-2023 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0
#include "../inc/espacc_config.h"
#include "../inc/espacc.h"
#include "hls_stream.h"
#include "hls_math.h"
#include <cstring>

void load(word_t _inbuff[SIZE_IN_CHUNK_DATA], dma_word_t *in1,
          /* <<--compute-params-->> */
	 const unsigned data_out_size,
	 const unsigned data_in_size,
	  dma_info_t &load_ctrl, int chunk, int batch)
{
load_data:

    const unsigned length = 4096; // round_up(data_in_size, VALUES_PER_WORD) / 64;
    const unsigned index = length * (batch * 1 + chunk);

    #ifndef __SYNTHESIS__
            std::cout << length << "\n";
        #endif

    unsigned dma_length = length / VALUES_PER_WORD;
    unsigned dma_index = index / VALUES_PER_WORD;

    load_ctrl.index = dma_index;
    load_ctrl.length = dma_length;
    load_ctrl.size = SIZE_WORD_T;

    for (unsigned i = 0; i < dma_length; i++) {
    load_label0:for(unsigned j = 0; j < VALUES_PER_WORD; j++) {
	    _inbuff[i * VALUES_PER_WORD + j] = in1[dma_index + i].word[j];
    	}
    }
}

void store(f_word_t _outbuff[SIZE_OUT_CHUNK_DATA], dma_f_word_t *out,
          /* <<--compute-params-->> */
	 const unsigned data_out_size,
	 const unsigned data_in_size,
	   dma_info_t &store_ctrl, int chunk, int batch)
{
store_data:

    const unsigned length = round_up(data_out_size, VALUES_PER_WORD) / 1;
    const unsigned store_offset = round_up(data_in_size, VALUES_PER_WORD) * 1;
    const unsigned out_offset = store_offset;
    const unsigned index = out_offset + length * (batch * 1);

    unsigned dma_length = length / VALUES_PER_WORD;
    unsigned dma_index = index / VALUES_PER_WORD;

    store_ctrl.index = dma_index;
    store_ctrl.length = dma_length;
    store_ctrl.size = SIZE_WORD_T;

    for (unsigned i = 0; i < dma_length; i++) {
    store_label1:for(unsigned j = 0; j < VALUES_PER_WORD; j++) {

	    out[dma_index + i].word[j] = _outbuff[i * VALUES_PER_WORD + j];
        #ifndef __SYNTHESIS__
        std::cout << out[dma_index + i].word[j] << "    "<<   _outbuff[i * VALUES_PER_WORD + j] << "    "<< i << "    "<< j << std::endl;
    #endif
	}
    }
    #ifndef __SYNTHESIS__
        std::cout << out[dma_index +0].word[0] << "    "<<  _outbuff[0] << std::endl;
    #endif
}


void compute(word_t _inbuff[SIZE_IN_CHUNK_DATA],
             /* <<--compute-params-->> */
	 const unsigned data_out_size,
	 const unsigned data_in_size, f_word_t _tmpbuff[64],
            f_word_t _outbuff[SIZE_OUT_CHUNK_DATA], int chunk, int batch)
{

    // TODO implement compute functionality
    const unsigned length =  4096; //round_up(data_in_size, VALUES_PER_WORD) / 64;
    const unsigned out_length = round_up(data_out_size, VALUES_PER_WORD) / 1;

    #ifndef SYNTHESIS
    for (int i = 0; i < length; i++){
        _tmpbuff[8] = _inbuff[i];
        _tmpbuff[0] = _tmpbuff[0] + _tmpbuff[8]; //S
        if(i == 0 && chunk == 0){
                _tmpbuff[5] = 1;
            }
        else{
            if(_tmpbuff[3] != _tmpbuff[8]){
                    _tmpbuff[5] = _tmpbuff[5] + 1;
                }
            }
        _tmpbuff[3] = _tmpbuff[8];
        }

        _tmpbuff[1] =  _tmpbuff[0].to_float()/(float)data_in_size;
        _tmpbuff[6] = fabs(_tmpbuff[5].to_float() - 2.0 * data_in_size * _tmpbuff[1].to_float() * (1-_tmpbuff[1].to_float())) / (2.0 * _tmpbuff[1].to_float() * (1-_tmpbuff[1].to_float()) * sqrt(2*data_in_size));
		_tmpbuff[2] = erfc(_tmpbuff[6].to_float());

    #else
    for (int i = 0; i < length; i++){
        _tmpbuff[8] = _inbuff[i];
        _tmpbuff[0] = _tmpbuff[0] + _tmpbuff[8]; //S
        if(i == 0 && chunk == 0){
                _tmpbuff[5] = 1;
            }
        else{
            if(_tmpbuff[3] != _tmpbuff[8]){
                    _tmpbuff[5] = _tmpbuff[5] + 1;
                }
            }
        _tmpbuff[3] = _tmpbuff[8];
    }
        _tmpbuff[1] =  _tmpbuff[0]/data_in_size;
        _tmpbuff[6] = fabs(_tmpbuff[5] - 2.0 * data_in_size * _tmpbuff[1] * (1-_tmpbuff[1])) / (2.0 * _tmpbuff[1] * (1-_tmpbuff[1]) * sqrt(2*data_in_size));
		_tmpbuff[2] = erfc(_tmpbuff[6]);

    #endif //for next iteration, cleat temp0 to do

        _outbuff[0] = _tmpbuff[2];
        _outbuff[1] = _tmpbuff[1];
        #ifndef __SYNTHESIS__
        std::cout << _outbuff[0] << std::endl;
    #endif
}


void top(dma_f_word_t *out, dma_word_t *in1,
         /* <<--params-->> */
	 const unsigned conf_info_data_out_size,
	 const unsigned conf_info_data_in_size,
	 dma_info_t &load_ctrl, dma_info_t &store_ctrl)
{

    /* <<--local-params-->> */
	 const unsigned data_out_size = conf_info_data_out_size;
	 const unsigned data_in_size = conf_info_data_in_size;

    static word_t _inbuff[SIZE_IN_CHUNK_DATA];
    static f_word_t _outbuff[SIZE_OUT_CHUNK_DATA];
    static f_word_t _tmpbuff[16];

    // Batching
batching:
    for (unsigned b = 0; b < 1; b++)
    {
        // Chunking
    go:
        for (int c = 0; c < data_in_size/4096; c++)
        {


            load(_inbuff, in1,
                 /* <<--args-->> */
	 	 data_out_size,
	 	 data_in_size,
                 load_ctrl, c, b);
            compute(_inbuff,
                    /* <<--args-->> */
	 	 data_out_size,
	 	 data_in_size,
                     _tmpbuff, 
                     _outbuff, c, b);
            store(_outbuff, out,
                  /* <<--args-->> */
	 	 data_out_size,
	 	 data_in_size,
                  store_ctrl, c, b);
        }
    }
}
