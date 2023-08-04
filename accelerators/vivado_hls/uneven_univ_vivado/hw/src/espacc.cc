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
     const unsigned data_stop,
     const unsigned data_L,
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
     const unsigned data_stop,
     const unsigned data_L,
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
	 const unsigned data_in_size, 
     const unsigned data_stop,
     const unsigned data_L,
     f_word_t _tmpbuff[4],
     word_t _tmpbuff_w[4],
     d_word_t _t_rep_buff[1024],
            f_word_t _outbuff[SIZE_OUT_CHUNK_DATA], int chunk, int batch)
{

    // TODO implement compute functionality
    const unsigned length =  4096; //round_up(data_in_size, VALUES_PER_WORD) / 64;
    const unsigned out_length = round_up(data_out_size, VALUES_PER_WORD) / 1;
    
    
#ifndef __SYNTHESIS__
        std::cout << "Here " << std::endl;
    #endif

    #ifndef SYNTHESIS
     for (int i = 0; i < length; i++){
        word_t tmp1;
        f_word_t tmp2;
        word_t input_tmp = _inbuff[i];
        word_t  tmp_index = i + chunk*4096; //actual index
        //_tmpbuff[1] is the length count
        _tmpbuff_w[2] =  _tmpbuff_w[2] + input_tmp*(1<<(data_L - 1 - _tmpbuff_w[1]));
        _tmpbuff_w[1] = _tmpbuff_w[1] + 1;
        tmp1 = _tmpbuff_w[2];
        tmp2 = _tmpbuff_w[0];
        if (_tmpbuff_w[1] == data_L){
            if(i >= 10*(1<<data_L) && tmp_index < data_stop){
            
                _tmpbuff[3] = _tmpbuff[3].to_float() + log(tmp2.to_float() - _t_rep_buff[tmp1])/log(2);
            }
            else{
                _tmpbuff[3] = _tmpbuff[3];
            }
            _t_rep_buff[tmp1] = tmp2; //potentially problematic T[decRep]
            _tmpbuff_w[2] = 0;
            _tmpbuff_w[1] = 0;
            _tmpbuff_w[0] = _tmpbuff_w[0] + 1;
        }
        else{
            _tmpbuff[3] = _tmpbuff[3];
            _tmpbuff_w[2] = _tmpbuff_w[2];
            _tmpbuff_w[1] = _tmpbuff_w[1];
            _tmpbuff_w[0] = _tmpbuff_w[0];
        }
     }
    #else
      for (int i = 0; i < length; i++){
        word_t tmp1;
        f_word_t tmp2;
        word_t input_tmp = _inbuff[i];
        word_t  tmp_index = i + chunk*4096; //actual index
        //_tmpbuff[1] is the length count
        _tmpbuff_w[2] =  _tmpbuff_w[2] + input_tmp*(1<<(data_L - 1 - _tmpbuff_w[1]));
        _tmpbuff_w[1] = _tmpbuff_w[1] + 1;
        tmp1 = _tmpbuff_w[2];
        tmp2 = _tmpbuff_w[0];
        if (_tmpbuff_w[1] == data_L){
            if(i >= 10*(1<<data_L) && tmp_index < data_stop){
            
                _tmpbuff[3] = _tmpbuff[3] + log(tmp2 - _t_rep_buff[tmp1])/log(2);
            }
            else{
                _tmpbuff[3] = _tmpbuff[3];
            }
            _t_rep_buff[tmp1] = tmp2; //potentially problematic T[decRep]
            _tmpbuff_w[2] = 0;
            _tmpbuff_w[1] = 0;
            _tmpbuff_w[0] = _tmpbuff_w[0] + 1;
        }
        else{
            _tmpbuff[3] = _tmpbuff[3];
            _tmpbuff_w[2] = _tmpbuff_w[2];
            _tmpbuff_w[1] = _tmpbuff_w[1];
            _tmpbuff_w[0] = _tmpbuff_w[0];
        }
     }

    #endif
    for (int i = 0; i < out_length; i++)
    {
        _outbuff[i] = _tmpbuff[3];
        }
}


void top(dma_f_word_t *out, dma_word_t *in1,
         /* <<--params-->> */
	 const unsigned conf_info_data_out_size,
	 const unsigned conf_info_data_in_size,
     const unsigned conf_info_data_stop,
	 const unsigned conf_info_data_L,
	 dma_info_t &load_ctrl, dma_info_t &store_ctrl)
{

    /* <<--local-params-->> */
	 const unsigned data_out_size = conf_info_data_out_size;
	 const unsigned data_in_size = conf_info_data_in_size;
     const unsigned data_stop = conf_info_data_stop;
     const unsigned data_L = conf_info_data_L;

    static word_t _inbuff[SIZE_IN_CHUNK_DATA];
    static f_word_t _outbuff[SIZE_OUT_CHUNK_DATA];
    static f_word_t _tmpbuff[4];
    static word_t _tmpbuff_w[4];
    static d_word_t _t_rep_buff[1024];

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
         data_stop,
         data_L,
                 load_ctrl, c, b);
            compute(_inbuff,
                    /* <<--args-->> */
	 	 data_out_size,
	 	 data_in_size,
         data_stop,
         data_L,
                     _tmpbuff, 
                     _tmpbuff_w,
                     _t_rep_buff,
                     _outbuff, c, b);
            store(_outbuff, out,
                  /* <<--args-->> */
	 	 data_out_size,
	 	 data_in_size,
         data_stop,
         data_L,
                  store_ctrl, c, b);
        }
    }
}
