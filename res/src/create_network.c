#include "ud_protodeep_core.h"

void			ud_pdc_print_layer(ud_layer *layer)
{
	char	*grades[3] = {"INPUT", "HIDDEN", "OUTPUT"};
	char	*types[4] = {"DENSE", "CONV", "MAXPOOL", "LSTM"};
	size_t	before_iter = layer->before_layer ? layer->before_layer->len : 0;
	size_t	front_iter = layer->front_layer ? layer->front_layer->len : 0;

	printf("%s[ %s\n", UD_UT_COLOR_1, (char *)layer->tst);//layer_name->val);
	printf("\t%s[ grade : %s ]\n", UD_UT_COLOR_2, grades[layer->layer_grade]);
	printf("\t%s[ type : %s ]\n", UD_UT_COLOR_2, types[layer->layer_type]);
	if (before_iter)
	{
		printf("\t%s[ before : ", UD_UT_COLOR_2);
		ud_layer	**t_l_before_layer_val = layer->before_layer ? (ud_layer **)layer->before_layer->val : NULL;
			for (ud_ut_count i = 0; i < before_iter; ++i, ++t_l_before_layer_val) printf("%s ", (char *)(*t_l_before_layer_val)->tst);//layer_name->val);
		printf("]\n");
	}
	if (front_iter)
	{
		printf("\t%s[ front : ", UD_UT_COLOR_2);
		ud_layer	**t_l_front_layer_val = layer->front_layer ? (ud_layer **)layer->front_layer->val : NULL;
			for (ud_ut_count i = 0; i < front_iter; ++i, ++t_l_front_layer_val) printf("%s ", (char *)(*t_l_front_layer_val)->tst);//layer_name->val);
		printf("]\n");
	}
	printf("%s]%s\n", UD_UT_COLOR_1, UD_UT_COLOR_N);
}

void			ud_pdc_print_network(ud_network *network)
{
	ud_layer	*layers = network->layers;
	size_t		layers_nbr = network->layers_nbr;

	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++layers)
		ud_pdc_print_layer(layers);
}

static ud_layer_grade	ud_pdc_get_layer_grade(char *grade_str, size_t *input_nbr, size_t *output_nbr)
{
	if (!ud_str_cmp(grade_str, "hidden")) return (UD_LG_HIDDEN);
	if (!ud_str_cmp(grade_str, "input"))
	{
		++(*input_nbr);
		return (UD_LG_INPUT);
	}
	if (!ud_str_cmp(grade_str, "output"))
	{
		++(*output_nbr);
		return (UD_LG_OUTPUT);
	}
	//valeur par default en cas d'erreur ? a definir
	return (UD_LG_HIDDEN);
}

static ud_layer_type	ud_pdc_get_layer_type(char *type_str)
{
	if (!ud_str_cmp(type_str, "dense")) return (UD_LT_DENSE);
	if (!ud_str_cmp(type_str, "conv")) return (UD_LT_CONV);
	if (!ud_str_cmp(type_str, "maxpool")) return (UD_LT_MAXPOOL);
	if (!ud_str_cmp(type_str, "lstm")) return (UD_LT_LSTM);
	//valeur par default en cas d'erreur ? a definir
	return (UD_LT_DENSE);
}

static size_t			ud_pdc_count_dep(char *dep_info_str)
{
	size_t	dep_nbr = 1;

	while (*dep_info_str) if (*dep_info_str++ == ';') dep_nbr++;
	return (dep_nbr);
}

static size_t			ud_pdc_get_dico_index(char first_char)
{
	if (first_char >= 'a' && first_char <= 'z') return (first_char -'a');
	if (first_char >= 'A' && first_char <= 'Z') return (first_char -'A');
	if (first_char >= '0' && first_char <= '9') return (first_char -'0');
	if (first_char == '-') return (62);
	if (first_char == '_') return (63);
	return (0);
}

static void				ud_pdc_update_dico_layer(ud_layer *layers, ud_arr *dico_layer[64], size_t dico_count[64], size_t layers_nbr)
{
	size_t		dico_index;
	size_t		*p_dico_count = dico_count;
	ud_arr		**p_dico_layer = dico_layer;

	for (ud_ut_count i = 0; i < 64; ++i, ++p_dico_count, ++p_dico_layer)
	{
		if (*p_dico_count)
		{
			*p_dico_layer = ud_arr_init(ud_layer *, *p_dico_count);
			*p_dico_count = 0;
		}
	}
	while (layers_nbr--)
	{
		dico_index = ud_pdc_get_dico_index(((char *)layers->layer_name->val)[0]);
		((ud_layer **)dico_layer[dico_index]->val)[dico_count[dico_index]++] = layers++;
	}
}

static void				ud_pdc_update_dico_count(ud_layer *layer, size_t dico_count[64])
{
	++dico_count[ud_pdc_get_dico_index(*((char *)layer->layer_name->val))];
}

static ud_layer			*ud_pdc_get_layer(char *layer_name, ud_arr *dico_layer[64])
{
	size_t		dico_index = ud_pdc_get_dico_index(layer_name[0]);
	ud_arr		*layer_l = dico_layer[dico_index];
	ud_layer	**t_layer_l_val = (ud_layer **)layer_l->val;
	size_t		nb_iter = layer_l->len;

	for (ud_ut_count i = 0; i < nb_iter; ++i, ++t_layer_l_val) if (!ud_str_cmp((char *)(*t_layer_l_val)->layer_name->val, layer_name)) return (*t_layer_l_val);
	return (NULL);
}

static void				update_tmp_front_layer(void *v_tmp_front_layers, ud_layer *front)
{
	ud_pdc_tmp_layer	**tmp_front_layers = (ud_pdc_tmp_layer **)v_tmp_front_layers;
	ud_pdc_tmp_layer	*new_tmp_layer;
	
	UD_UT_PROT_MALLOC_VOID(new_tmp_layer = ud_ut_malloc(sizeof(ud_pdc_tmp_layer)));
	new_tmp_layer->layer = front;
	new_tmp_layer->next = NULL;
	if (!(*tmp_front_layers)) *tmp_front_layers = new_tmp_layer;
	else
	{
		ud_pdc_tmp_layer	*list_browser = *tmp_front_layers;
		while (list_browser->next) list_browser = list_browser->next;
		list_browser->next = new_tmp_layer;
	}
}

static void				ud_pdc_link_layers(ud_layer *layers, ud_arr **layers_info, ud_arr *dico_layer[64])
{
	while (*layers_info)
	{
		if (layers->layer_grade != UD_LG_INPUT)
		{
			ud_arr		**t_layer_info_val = (ud_arr **)(*layers_info)->val;
			ud_arr		*before_layers_name = ud_stra_split(t_layer_info_val[4], "; ");
			size_t		nb_iter = before_layers_name->len;
			ud_layer	**t_before_layer_val = (ud_layer **)layers->before_layer->val;
			ud_arr		**t_before_name_val = (ud_arr **)before_layers_name->val;
			for (ud_ut_count i = 0; i < nb_iter; ++i, ++t_before_layer_val, ++t_before_name_val)
			{
				*t_before_layer_val = ud_pdc_get_layer((char *)(*t_before_name_val)->val, dico_layer);
				update_tmp_front_layer(((*t_before_layer_val)->front_layer->val), layers);
			}
			ud_arr_free(before_layers_name);
		}
		++layers;
		++layers_info;
	}
}

static size_t			ud_pdc_count_front_layers(ud_pdc_tmp_layer *front_layers_lst)
{
	size_t	count = 0;

	while (front_layers_lst)
	{
		front_layers_lst = front_layers_lst->next;
		++count;
	}
	return (count);
}

static void				ud_pdc_link_layers_front(ud_layer *layers, size_t layers_nbr)
{
	size_t				nb_front;
	ud_pdc_tmp_layer	*front_layers_lst;
	ud_pdc_tmp_layer	*before_tmp_to_free;

	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++layers)
	{
		ud_arr	*l_front_layer = layers->front_layer;

		if (l_front_layer && l_front_layer->val)
		{
			front_layers_lst = ((ud_pdc_tmp_layer **)l_front_layer->val)[0];
			nb_front = ud_pdc_count_front_layers(front_layers_lst);
			ud_ut_free(l_front_layer->val);
			ud_ut_free(l_front_layer);
			layers->front_layer = ud_arr_init(ud_layer *, nb_front);
			ud_layer	**t_l_front_layer_val = (ud_layer **)layers->front_layer->val;
			while (front_layers_lst)
			{
				*t_l_front_layer_val = front_layers_lst->layer;
				before_tmp_to_free = front_layers_lst;
				front_layers_lst = front_layers_lst->next;
				++t_l_front_layer_val;
				ud_ut_free(before_tmp_to_free);
			}
		}
	}
}

static ud_layer			ud_pdc_create_layer(ud_arr *layer_info, size_t *input_nbr, size_t *output_nbr, size_t dico_count[64])
{
	ud_layer	layer;
	ud_arr		**t_layer_info_val = (ud_arr **)layer_info->val;

	layer.actv_and_grad = NULL;
	layer.before_weight = NULL;
	layer.layer_type =	ud_pdc_get_layer_type((char *)t_layer_info_val[2]->val);
	layer.param = NULL; //ud_pdc_parse_layer_param((char *)layer_info[3]->val); TO DO
	layer.layer_grade = ud_pdc_get_layer_grade((char *)t_layer_info_val[1]->val, input_nbr, output_nbr);
	if (layer.layer_grade != UD_LG_INPUT) layer.before_layer = ud_arr_init(ud_layer *, ud_pdc_count_dep((char *)t_layer_info_val[4]->val)); else layer.before_layer = NULL;
	if (layer.layer_grade != UD_LG_OUTPUT) layer.front_layer = ud_arr_set(ud_pdc_tmp_layer *, NULL); else layer.front_layer = NULL;
	layer.layer_name = ud_stra_new((char *)t_layer_info_val[0]->val);
	ud_pdc_update_dico_count(&layer, dico_count);
	layer.tst = strdup((char *)layer.layer_name->val);
	return (layer);
}

static ud_arr			**ud_pdc_parse_csv(char *csv_path, size_t *layers_nbr)
{
	ud_arr	**layers_info;
	ud_arr	*csv = ud_file_read(csv_path);
	ud_arr	*csv_lines = ud_stra_split(csv, "\n");
	ud_arr	**t_csv_lines_val = (ud_arr **)csv_lines->val;

	*layers_nbr = csv_lines->len - 1;
	UD_UT_PROT_MALLOC(layers_info = ud_ut_malloc(sizeof(ud_arr *) * (*layers_nbr + 1)));
	ud_arr	**p_layers_info = layers_info;
	for (ud_ut_count i = 0; i < *layers_nbr; ++i, ++p_layers_info, ++t_csv_lines_val)
		*p_layers_info = ud_stra_split(*t_csv_lines_val, ", ");
	*p_layers_info = NULL;
	ud_arr_free(csv_lines);
	ud_arr_free(csv);
	return (layers_info);
}

static void				ud_pdc_free_dico(ud_arr *dico_layer[64])
{
	for (ud_ut_count i = 0; i < 64; ++i)
	{
		ud_ut_free((*dico_layer)->val);
		ud_ut_free(*dico_layer++);
	}
}

static void				ud_pdc_free_csv_info(ud_arr **layers_info)
{
	ud_arr	**p_layers_info = layers_info;

	while (*p_layers_info) ud_arr_free(*p_layers_info++);
	ud_ut_free(layers_info);
}

static void				ud_pdc_free_layer_content(ud_layer *layer)
{
	if (layer->before_layer) ud_arr_ofree(layer->before_layer);
	if (layer->front_layer) ud_arr_ofree(layer->front_layer);
	//ud_arr_free(layer->layer_name); TITOU A TOI DE JOUER !!!
	if (layer->actv_and_grad) ud_tens_free(layer->actv_and_grad);
	if (layer->before_weight) ud_tens_free(layer->actv_and_grad);
	(void)layer->param;
	//ud_pdc_free_layer_param(layer.param);
	free(layer->tst);
}

void					ud_pdc_free_network(ud_network *network)
{
	size_t		layers_nbr = network->layers_nbr + 1;
	ud_layer	*n_p_layers = network->layers;

	ud_arr_ofree(network->inputs_layer);
	ud_arr_ofree(network->outputs_layer);
	while (--layers_nbr)
		ud_pdc_free_layer_content(n_p_layers++);
	ud_ut_free(network->layers);
	ud_ut_free(network);
}

ud_network				*ud_pdc_create_network(char *csv_path)
{
	ud_network	*network;
	size_t		layers_nbr;
	size_t		dico_count[64] = {0};
	size_t		input_nbr = 0;
	size_t		output_nbr = 0;
	ud_arr		*dico_layer[64] = {NULL}; // [a-zA-Z0-9] et - _

	UD_UT_PROT_MALLOC(network = ud_ut_malloc(sizeof(ud_network)));
	ud_arr		**layers_info = ud_pdc_parse_csv(csv_path, &layers_nbr);
	UD_UT_PROT_MALLOC(network->layers = ud_ut_malloc(sizeof(ud_layer) * layers_nbr));
	ud_layer	*p_layers = network->layers;
	ud_arr		**p_layers_info = layers_info;
	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++p_layers, ++p_layers_info) *p_layers = ud_pdc_create_layer(*p_layers_info, &input_nbr, &output_nbr, dico_count);
	ud_pdc_update_dico_layer(network->layers, dico_layer, dico_count, layers_nbr);
	ud_pdc_link_layers(network->layers, layers_info, dico_layer);
	ud_pdc_link_layers_front(network->layers, layers_nbr);
	network->inputs_layer = ud_arr_init(ud_layer *, input_nbr);
	network->outputs_layer = ud_arr_init(ud_layer *, output_nbr);
	ud_layer	**p_i_layer = (ud_layer **)network->inputs_layer->val;
	ud_layer	**p_o_layer = (ud_layer **)network->outputs_layer->val;
	p_layers = network->layers;
	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++p_layers)
	{
		if (p_layers->layer_grade == UD_LG_INPUT) *(p_i_layer++) = p_layers;
		else if (p_layers->layer_grade == UD_LG_OUTPUT) *(p_o_layer++) = p_layers;
	}
	network->layers_nbr = layers_nbr;
	ud_pdc_free_dico(dico_layer);
	ud_pdc_free_csv_info(layers_info);
	return (network);
}
