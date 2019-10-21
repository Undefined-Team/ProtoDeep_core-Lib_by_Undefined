
#include "ud_protodeep_core.h"

/*void			*ud_pdc_parse_layer_param(char *param_str)
{
	return (1);
}*/

void			ud_pdc_print_layer(ud_layer *layer)
{
	char	*grades[3] = {"INPUT", "HIDDEN", "OUTPUT"};
	char	*types[4] = {"DENSE", "CONV", "MAXPOOL", "LSTM"};
	size_t	before_iter = layer->before_layer ? layer->before_layer->len : 0;
	size_t	front_iter = layer->front_layer ? layer->front_layer->len : 0;
	printf("%s[ %s\n", UD_UT_COLOR_1, (char *)layer->layer_name->val);
	printf("\t%s[ grade : %s ]\n", UD_UT_COLOR_2, grades[layer->layer_grade]);
	printf("\t%s[ type : %s ]\n", UD_UT_COLOR_2, types[layer->layer_type]);
	if (before_iter)
	{
		printf("\t%s[ before : ", UD_UT_COLOR_2);
		ud_layer	**t_l_before_layer_val = layer->before_layer ? (ud_layer **)layer->before_layer->val : NULL;
			for (ud_ut_count i = 0; i < before_iter; ++i, ++t_l_before_layer_val)
				printf("%s ", (char *)(*t_l_before_layer_val)->layer_name->val);
		printf("]\n");
	}
	if (front_iter)
	{
		printf("\t%s[ front : ", UD_UT_COLOR_2);
		ud_layer	**t_l_front_layer_val = layer->front_layer ? (ud_layer **)layer->front_layer->val : NULL;
			for (ud_ut_count i = 0; i < front_iter; ++i, ++t_l_front_layer_val)
				printf("%s ", (char *)(*t_l_front_layer_val)->layer_name->val);
		printf("]\n");
	}
	printf("%s]%s\n", UD_UT_COLOR_1, UD_UT_COLOR_N);
}

void			ud_pdc_print_network(ud_network *network)
{
	for (size_t i = 0; i < network->layers_nbr; i++) {

		ud_pdc_print_layer(&(network->layers[i]));
	}
}

ud_layer_grade	ud_pdc_get_layer_grade(char *grade_str, size_t *input_nbr, size_t *output_nbr)
{
	if (!ud_str_cmp(grade_str, "hidden"))
		return (UD_LG_HIDDEN);
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
	//valeur par default en cas d'erreur ?
	return (UD_LG_HIDDEN);
}

ud_layer_type	ud_pdc_get_layer_type(char *type_str)
{
	if (!ud_str_cmp(type_str, "dense"))
		return (UD_LT_DENSE);
	if (!ud_str_cmp(type_str, "conv"))
		return (UD_LT_CONV);
	if (!ud_str_cmp(type_str, "maxpool"))
		return (UD_LT_MAXPOOL);
	if (!ud_str_cmp(type_str, "lstm"))
		return (UD_LT_LSTM);
	//valeur par default en cas d'erreur ?
	return (UD_LT_DENSE);
}

size_t			ud_pdc_count_dep(char *dep_str)
{
	size_t	dep_nbr = 1;

	while (*dep_str)
		if (*dep_str++ == ';')
			dep_nbr++;
	return (dep_nbr);
}

size_t		ud_pdc_get_dico_index(char first_char)
{
	if (first_char >= 'a' && first_char <= 'z')
		return (first_char -'a');
	else if (first_char >= 'A' && first_char <= 'Z')
		return (first_char -'A');
	else if (first_char >= '0' && first_char <= '9')
		return (first_char -'0');
	else if (first_char == '-')
		return (62);
	else if (first_char == '_')
		return (63);
	return (0);
}

void		ud_pdc_update_dico_layer(ud_layer *layers, ud_arr *dico_layer[64], size_t dico_count[64], size_t layers_nbr)
{
	//char		first_char;
	//ud_layer	**p_layer;
	size_t		dico_index;

	for (ud_ut_count i = 0; i < 64; ++i)
	{
		if (dico_count[i])
		{
			dico_layer[i] = ud_arr_init(ud_layer *, dico_count[i]);
			dico_count[i] = 0;
		}
	}
	while (layers_nbr--)
	{
		dico_index = ud_pdc_get_dico_index(((char *)layers->layer_name->val)[0]);
		/*if (first_char >= 'a' && first_char <= 'z')
			dico_index = first_char -'a';
		else if (first_char >= 'A' && first_char <= 'Z')
			dico_index = first_char -'A';
		else if (first_char >= '0' && first_char <= '9')
			dico_index = first_char -'0';
		else if (first_char == '-')
			dico_index = 62;
		else if (first_char == '_')
			dico_index = 63;*/
		((ud_layer **)dico_layer[dico_index]->val)[dico_count[dico_index]++] = layers++;
	}
}

void		ud_pdc_update_dico_count(ud_layer *layer, size_t dico_count[64])
{
	++dico_count[ud_pdc_get_dico_index(((char *)layer->layer_name->val)[0])];
}

ud_layer	*ud_pdc_get_layer(char *layer_name, ud_arr *dico_layer[64])
{
	//char	first_char = layer_name[0];
	size_t	dico_index = ud_pdc_get_dico_index(layer_name[0]);
	ud_arr	*layer_l = dico_layer[dico_index];
	ud_layer	**t_layer_l_val = (ud_layer **)layer_l->val;
	//char	**t_layer_l_val = (char **)layer_l->val;
	size_t	nb_iter = layer_l->len;
	for (ud_ut_count i = 0; i < nb_iter; ++i, ++t_layer_l_val)
	{
		if (!ud_str_cmp((char *)(*t_layer_l_val)->layer_name->val, layer_name))
		{
			return (*t_layer_l_val);
		}
	}
	return (NULL);
}

void		update_tmp_front_layer(void *v_tmp_front_layers, ud_layer *front)
{
	ud_pdc_tmp_layer	**tmp_front_layers = (ud_pdc_tmp_layer **)v_tmp_front_layers;
	ud_pdc_tmp_layer	*new_tmp_layer;
	UD_UT_PROT_MALLOC_VOID(new_tmp_layer = ud_ut_malloc(sizeof(ud_pdc_tmp_layer)));
	new_tmp_layer->layer = front;
	new_tmp_layer->next = NULL;
	//printf("%s\n", (char *)list_browser->next->layer->layer_name->val);
	if (!(*tmp_front_layers))
	{

		//printf("first assign ! %s\n", (char *)new_tmp_layer->layer->layer_name->val);
		*tmp_front_layers = new_tmp_layer;
	}
	else
	{
		ud_pdc_tmp_layer	*list_browser = *tmp_front_layers;
		while (list_browser->next)
		{
			//printf("%s->", (char *)list_browser->layer->layer_name->val);
			list_browser = list_browser->next;
		}
		list_browser->next = new_tmp_layer;
		//printf("%s->", (char *)list_browser->layer->layer_name->val);
		//printf("%s\n", (char *)list_browser->next->layer->layer_name->val);
		//printf("%s\n");
	}
}

void		ud_pdc_link_layers(ud_layer *layers, ud_arr **layers_info, ud_arr *dico_layer[64])//, ud_pdc_tmp_layer **tmp_front_layers)
{
	while (*layers_info)
	{
		if (layers->layer_grade != UD_LG_INPUT)
		{
			ud_arr		**t_layer_info_val = (ud_arr **)(*layers_info)->val;
			ud_arr		*before_layers_name = ud_stra_split(t_layer_info_val[4], "; ");
			size_t	nb_iter = before_layers_name->len;
			ud_layer	**t_before_layer_val = (ud_layer **)layers->before_layer->val;

			ud_arr		**t_before_name_val = (ud_arr **)before_layers_name->val;
			//printf("nb_iter %zu\n", nb_iter);
			for (ud_ut_count i = 0; i < nb_iter; ++i, ++t_before_layer_val, ++t_before_name_val)
			{
				//printf("? ? \n");
				*t_before_layer_val = ud_pdc_get_layer((char *)(*t_before_name_val)->val, dico_layer);
				//ud_pdc_print_layer(*t_before_layer_val);
				//printf("? ? \n");
				//printf("test wefwef \n");
			//	printf("layer %s : front : ", (*t_before_layer_val)->layer_name->val);
			//	ud_pdc_tmp_layer	**
				update_tmp_front_layer(((*t_before_layer_val)->front_layer->val), layers);
				//printf("hallo\n");
				//while ()
			}
		}
	//	layer->before_layer = ud_arr_init(ud_layer *, ud_pdc_count_dep((char *)t_layer_info_val[4]->val));
		
		layers++;
		layers_info++;
	}
	if (layers && dico_layer)
		;
}

size_t		ud_pdc_count_front_layers(ud_pdc_tmp_layer *front_layers_lst)
{
	size_t	count = 0;
	while (front_layers_lst)
	{
		front_layers_lst = front_layers_lst->next;
		++count;
	}
	return (count);
}

void		ud_pdc_link_layers_front(ud_layer *layers, size_t layers_nbr)
{
	size_t	nb_front;
	ud_pdc_tmp_layer	*front_layers_lst;
	ud_pdc_tmp_layer	*before_tmp_to_free;
	
	for (ud_ut_count i = 0; i < layers_nbr; ++i)
	{
		if (layers->front_layer && layers->front_layer->val)
		{
			front_layers_lst = ((ud_pdc_tmp_layer **)layers->front_layer->val)[0];
			nb_front = ud_pdc_count_front_layers(front_layers_lst);
			ud_ut_free(layers->front_layer->val);
			ud_ut_free(layers->front_layer);
			layers->front_layer = ud_arr_init(ud_layer *, nb_front);
			ud_layer	**p_front_layer = (ud_layer **)layers->front_layer->val;
			while (front_layers_lst)
			{
				*p_front_layer = front_layers_lst->layer;
				before_tmp_to_free = front_layers_lst;
				front_layers_lst = front_layers_lst->next;
				++p_front_layer;
				ud_ut_free(before_tmp_to_free);
			}
			/*printf("%s front: ", (char *)layers->layer_name->val);
			for (size_t i = 0 ; i < nb_front; ++i)
			{
				printf("%s ", (char *)((ud_layer **)layers->front_layer->val)[i]->layer_name->val);
					
			}
			printf("\n");*/
		}
		++layers;
	}
}

ud_layer	ud_pdc_create_layer(ud_arr *layer_info, size_t *input_nbr, size_t *output_nbr, size_t dico_count[64])
{
	ud_layer	layer;
	//ud_arr		*layer_info = ud_stra_split(csv_line, ", ");
	ud_arr		**t_layer_info_val = (ud_arr **)layer_info->val;

	layer.actv_and_grad = NULL;
	layer.before_weight = NULL;
	layer.layer_type =	ud_pdc_get_layer_type((char *)t_layer_info_val[2]->val);
	layer.param = NULL; //ud_pdc_parse_layer_param((char *)layer_info[3]->val);
	layer.layer_grade = ud_pdc_get_layer_grade((char *)t_layer_info_val[1]->val, input_nbr, output_nbr);
	if (layer.layer_grade != UD_LG_INPUT)
		layer.before_layer = ud_arr_init(ud_layer *, ud_pdc_count_dep((char *)t_layer_info_val[4]->val));
	else
		layer.before_layer = NULL;
	if (layer.layer_grade != UD_LG_OUTPUT)
		layer.front_layer = ud_arr_set(ud_pdc_tmp_layer *, NULL);
	else
		layer.front_layer = NULL;
	//ud_arr_print(layer.front_layer);
	//printf("%p\n", ((ud_pdc_tmp_layer **)layer.front_layer->val)[0]);
	layer.layer_name = ud_stra_new((char *)t_layer_info_val[0]->val);
	ud_pdc_update_dico_count(&layer, dico_count);
	return (layer);
}

ud_arr		**ud_pdc_parse_csv(char *csv_path, size_t *layers_nbr)
{
	ud_arr	**layers_info;
	ud_arr	*csv = ud_file_read(csv_path);
	ud_arr	*csv_lines = ud_stra_split(csv, "\n");
	ud_arr	**t_csv_lines_val = (ud_arr **)csv_lines->val;
	*layers_nbr = csv_lines->len - 1; // dernier element vide car '\n' a la fin

	UD_UT_PROT_MALLOC(layers_info = ud_ut_malloc(sizeof(ud_arr *) * (*layers_nbr + 1)));
	ud_arr	**p_layers_info = layers_info;
	for (ud_ut_count i = 0; i < *layers_nbr; ++i, ++p_layers_info, ++t_csv_lines_val)
		*p_layers_info = ud_stra_split(*t_csv_lines_val, ", ");
	*p_layers_info = NULL;
	return (layers_info);
}
/*
ud_pdc_tmp_layer	**ud_pdc_init_tmp_front_layer(size_t layers_nbr)
{
	ud_pdc_tmp_layer	**tmp_front_layers;
	UD_UT_PROT_MALLOC(tmp_front_layers = ud_ut_malloc(sizeof(ud_pdc_tmp_layer*) * layers_nbr));
	ud_pdc_tmp_layer	**p_tmp_front_layers = tmp_front_layers;
	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++p_tmp_front_layers)
		*p_tmp_front_layers = NULL;
}
*/
ud_network	*ud_pdc_create_network(char *csv_path)
{
	ud_network	*network;
	size_t		layers_nbr;
    //	= csv_lines->len - 1; // dernier element vide car '\n' a la fin
	ud_arr		**layers_info = ud_pdc_parse_csv(csv_path, &layers_nbr);
	//ud_pdc_tmp_layer	**tmp_front_layers = ud_pdc_init_tmp_front_layer(layers_nbr);
	UD_UT_PROT_MALLOC(network = ud_ut_malloc(sizeof(ud_network)));
	//ud_arr	*csv = ud_file_read(csv_path);
	//ud_arr	*csv_lines = ud_stra_split(csv, "\n");
	//ud_arr_print(csv_lines);

	size_t	dico_count[64] = {0};
	ud_arr	*dico_layer[64] = {NULL}; // [a-zA-Z0-9] et - _
	UD_UT_PROT_MALLOC(network->layers = ud_ut_malloc(sizeof(ud_layer) * layers_nbr));
	ud_layer	*p_layers = network->layers;
	//ud_arr		**t_csv_lines_val = (ud_arr **)csv_lines->val;
	size_t		input_nbr = 0;
	size_t		output_nbr = 0;
	ud_arr		**p_layers_info = layers_info;
	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++p_layers, ++p_layers_info)
	{
		*p_layers = ud_pdc_create_layer(*p_layers_info, &input_nbr, &output_nbr, dico_count);
		/*// !! pas sur du p_layers-> sinon c'est (*p_layers).
		if (p_layers->layer_grade == UD_LG_)*/
	}
	ud_pdc_update_dico_layer(network->layers, dico_layer, dico_count, layers_nbr);
	ud_pdc_link_layers(network->layers, layers_info, dico_layer);
	ud_pdc_link_layers_front(network->layers, layers_nbr);
	/*for (int i = 0; i < 64; i++)
	{
		if (dico_layer[i])
		{
			printf("%d: ", i);
			ud_arr_print(dico_layer[i]);
		}
			//printf("%d : %zu\n", i, dico_count[i]);
	}*/
	network->inputs_layer = ud_arr_init(ud_layer *, input_nbr);
	network->outputs_layer = ud_arr_init(ud_layer *, output_nbr);
	ud_layer	**p_i_layer = (ud_layer **)network->inputs_layer->val;
	ud_layer	**p_o_layer = (ud_layer **)network->outputs_layer->val;
	p_layers = network->layers;
	for (ud_ut_count i = 0; i < layers_nbr; ++i, ++p_layers)
	{
		if (p_layers->layer_grade == UD_LG_INPUT)
			*(p_i_layer++) = p_layers;
		else if (p_layers->layer_grade == UD_LG_OUTPUT)
			*(p_o_layer++) = p_layers;
	}
	network->layers_nbr = layers_nbr;
	ud_pdc_print_network(network);
	return (network);
}
