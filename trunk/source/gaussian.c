//Courtesy of Hernan Lobert
//Email: hlobert@yahoo.com.br
//http://www.wilmott.com/messageview.cfm?catid=4&threadid=6390
//As of date Oct 16 2003.

/************************************
* Retorna uma va com distribuição normal, mi e sigma.
* Return a variable with normal distribution ( mi, sigma^2)
*
* Polar( Box-Mueller) method; See Knuth v2, 3rd ed, p122
*
* HJL, 6/6/2001
/************************************/
double uni2 = 3.;
double NormX(double mi, double sigma)
{
   double x, y, r2, uni;
	// Procura dois unif dentro do circulo unitário
   do
   {
		/* choose x,y in uniform square (-1,-1) to (+1,+1) */
		uni = Unif( (unsigned)(uni2*time(NULL)));
		x = -1 + 2 * uni;
		uni2 = Unif( (unsigned)(uni*time(NULL)));
		y = -1 + 2 * uni;
		/* see if it is in the unit circle */
		r2 = x * x + y * y;
	}
	while (r2 > 1.0 || r2 == 0);

	/* Box-Muller transform */
	double nr = sigma * y * sqrt (-2.0 * log( r2) / r2);
	nr = nr + mi;
	return nr;
}

You only need a uniform distribution generator like this one:

/************************************
* Retorna um numero aleatório de uma uniforme 0, 1
* Return a random number from a uniforme 0, 1
*
* HJL, 6/6/2001
/************************************/
double Unif(unsigned i)
{
	/* Seed the random-number generator with current time so that
	* the numbers will be different every time we run. */
	srand( i); 

	/* gera o número aleatório */
	double temp = rand() / 32767.;
	return temp; // RAND_MAX
}

