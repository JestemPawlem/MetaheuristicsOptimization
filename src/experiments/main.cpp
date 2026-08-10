#include "pso_experiments.h"
#include "sga_experiments.h"
#include "de_experiments.h"

int main()
{
	experiments::pso::run_all();
	//experiments::sga::run_all();
	//experiments::de::run_all();

	return 0;
}