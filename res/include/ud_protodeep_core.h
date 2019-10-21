#ifndef UD_PROTODEEP_CORE_H
# define UD_PROTODEEP_CORE_H

// Lib
#include <stdio.h>
#include <stdlib.h>
#include <ud_tensor.h>
#include <ud_csv.h>

// Macro
# define UD_LT_FULLCO   UD_LT_DENSE

// Structures
typedef enum            {UD_A_RELU, UD_A_LRELU, UD_A_SOFTMAX, UD_A_SIGMOID, UD_A_TANH} ud_activation;
typedef enum            {UD_LT_DENSE, UD_LT_CONV, UD_LT_MAXPOOL, UD_LT_LSTM} ud_layer_type;
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
    ud_layer					*layers;		// !! c'est une liste de layer (ex: layer[0].param, layer[1].param etc...)
    size_t						layers_nbr;
}								ud_network;

// Prototypes

ud_network		*ud_pdc_create_network(char *csv_path);

#endif
