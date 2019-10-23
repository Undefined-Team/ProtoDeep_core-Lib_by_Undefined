#include <ud_protodeep_core.h>

int main(int ac, char **av)
{
	ud_network *net;

	while (*++av)
	{
		net = ud_pdc_create_network(*av);
		ud_pdc_free_network(net);
	}
	if (ac == 1)
	{
		net = ud_pdc_create_network("./network.csv");
//		ud_arr *front_layer = net->layers[0]->front_layer;

		//printf("name of first next (should be dense1) : %s\n", (char *)((ud_layer **)front_layer->val)[0]->layer_name->val);
		ud_pdc_print_network(net);
		ud_pdc_free_network(net);
	}
	return 0;
}
