/* --------------------------------------------------------- */
/* --------------- A Very Short Example -------------------- */
/* --------------------------------------------------------- */
#include"stdafx.h"
#include <stdio.h>
#include <stdlib.h> /* free() */
#include <stddef.h> /* NULL */
#include "cmaes_interface.h"

double fitfun(double const *x, int dim);

/* the objective (fitness) function to be minimized */
double fitfun(double const *x, int N) { /* function "cigtab" */
	int i;
	double sum = 0;// 1e4*x[0] * x[0] + 1e-4*x[1] * x[1];
	for (i = 0; i < N; ++i)
		sum += x[i] * x[i];
	return sum;
}

/* the optimization loop */
int main(int argn, char **args) {
	cmaes_t evo[1]; /* an CMA-ES type struct or "object" */


	for (int k = 0; k < 1; k++)
	{
		double *arFunvals, *const*pop, *xfinal;
		int i;

		/* Initialize everything into the struct evo, 0 means default */
		arFunvals = cmaes_init(&evo[k], 0, NULL, NULL, 0, 0, "cmaes_initials.par");


		printf("%s\n", cmaes_SayHello(&evo[k]));
		//cmaes_ReadSignals(&evo[k], "cmaes_signals.par");  /* write header and initial values */


		double best_f = 100000;
		int iter_num = 0;
														  /* Iterate until stop criterion holds */
		while (!cmaes_TestForTermination(&evo[k]))
		{
			iter_num++;
			/* generate lambda new search points, sample population */
			pop = cmaes_SamplePopulation(&evo[k]); /* do not change content of pop */

												   /* Here we may resample each solution point pop[i] until it
												   becomes feasible. function is_feasible(...) needs to be
												   user-defined.
												   Assumptions: the feasible domain is convex, the optimum is
												   not on (or very close to) the domain boundary, initialX is
												   feasible and initialStandardDeviations are sufficiently small
												   to prevent quasi-infinite looping. */
												   /* for (i = 0; i < cmaes_Get(&evo, "popsize"); ++i)
												   while (!is_feasible(pop[i]))
												   cmaes_ReSampleSingle(&evo, i);
												   */

												   /* evaluate the new search points using fitfun */
			for (i = 0; i < cmaes_Get(&evo[k], "lambda"); ++i) {
				arFunvals[i] = fitfun(pop[i], (int)cmaes_Get(&evo[k], "dim"));
			}

			for (int i = 0; i < cmaes_Get(&evo[k], "lambda"); ++i)
			{
				if (best_f > arFunvals[i])
				{
					best_f = arFunvals[i];
				}
			}
			printf("i: %d   best fitness :%lf\n", iter_num, best_f);


			/* update the search distribution used for cmaes_SamplePopulation() */
			cmaes_UpdateDistribution(&evo[k], arFunvals);

			/* read instructions for printing output or changing termination conditions */
			//cmaes_ReadSignals(&evo[k], "cmaes_signals.par");
			fflush(stdout); /* useful in MinGW */
		}
		printf("Stop:\n%s\n", cmaes_TestForTermination(&evo[k])); /* print termination reason */
		cmaes_WriteToFile(&evo[k], "all", "allcmaes.dat");         /* write final results */

		printf("%d\n", (int)cmaes_Get(&evo[k], "dim"));
		printf("%d\n", (int)cmaes_Get(&evo[k], "lambda"));

		/* get best estimator for the optimum, xmean */
		xfinal = cmaes_GetNew(&evo[k], "xmean"); /* "xbestever" might be used as well */

		cmaes_exit(&evo[k]); /* release memory */

							 /* do something with final solution and finally release memory */
		free(xfinal);
	}
	

	return 0;
}

