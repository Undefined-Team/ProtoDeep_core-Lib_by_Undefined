#ifndef UD_PROTODEEP_CORE_H
# define UD_PROTODEEP_CORE_H

// Lib
#include <stdio.h>
#include <stdlib.h>
#include <ud_tensor.h>
#include <ud_csv_array.h>

// Macro

# define UD_LT_FULLCO   UD_LT_DENSE

// Structures
typedef enum            {UD_A_RELU, UD_A_LRELU, UD_A_SOFTMAX, UD_A_SIGMOID, UD_A_TANH} ud_activation;
typedef enum            {UD_LT_DENSE, UD_LT_CONV, UD_LT_MAXPOOL, UD_LT_RNN, UD_LT_LSTM, UD_LT_DROPOUT} ud_layer_type;
typedef enum            {UD_LG_INPUT, UD_LG_HIDDEN, UD_LG_OUTPUT} ud_layer_grade;

typedef struct					uds_layer {
    ud_tensor					*actv_and_grad;
    ud_tensor					*before_weight;
    ud_arr						*before_layer;
    ud_arr						*front_layer;
    ud_layer_type				layer_type;
    void						*param;
    ud_layer_grade				layer_grade;
    ud_arr						*layer_name;
	char						*tst;
    // ud_layer_state  layer_state;
}								ud_layer;

/*	------OLD STRUCT FROM TITOU-------
**	typedef struct          uds_network {
**		ud_arr				*inputs_layer;
**		ud_arr				*outputs_layer;
**		ud_layer            **layers;
**		size_t              layers_nbr;
**	}                       ud_network;
**	-----------------------------------	
*/

typedef struct					uds_pdc_tmp_layer {
	ud_layer					*layer;
	struct uds_pdc_tmp_layer	*next;
}								ud_pdc_tmp_layer;

typedef struct				    uds_network {
    ud_arr						*inputs_layer;
    ud_arr						*outputs_layer;
    ud_layer					*layers; //attention layer[0]... layer[1]
    size_t						layers_nbr;
}								ud_network;

typedef struct              uds_dense_params {
    char                    *activation;
    size_t                  *neurons_shape;
}                           ud_dense_params;

typedef struct              uds_conv_params {
    char                    *activation;
    size_t                  *kernel_size;
    size_t                  strides;
}                           ud_conv_params;

typedef struct              uds_maxpool_params {
    size_t                  *pool_size;
    size_t                  strides;
}                           ud_maxpool_params;

typedef struct              uds_rnn_params {
    char                    *activation;
}                           ud_rnn_params;

typedef struct              uds_lstm_params {
    char                    *activation;
    char                    *recurrent_activation;
    char                    *padding;
}                           ud_lstm_params;

typedef struct              uds_dropout_params {
    char                    *activation;
    size_t                  *neurons_shape;
    double                  dropout_rate;
}                           ud_dropout_params;

// Prototypes

ud_network		*ud_pdc_create_network(char *csv_path);
void			ud_pdc_free_network(ud_network *network);
void			ud_pdc_print_network(ud_network *network);
void			ud_pdc_print_layer(ud_layer *layer);

#endif
