/*
Algorytm do generowania stringów na potrzeby metody brute force zaczerpniêty ze strony
http://hacksenkessel.com/2014/brute-force-algorithm-in-c/, przepisany przeze mnie z C++ na C
oraz dostosowany do bie¿¹cych potrzeb
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#include "crypter.h"
#include "main.h"

char * bruteforce(char *password, char *encrypted, int keyLength, int numOfThreads)
{
	size_t keySize = sizeof(char) * (keyLength + 1);

	char* keyResult = NULL;
	unsigned long long max_perms = 0;

	//Za³o¿enie: klucz sk³ada siê tylko ze znaków, które mo¿na wywo³aæ klawiatur¹
	const int asciiMinIndex = 32; //' '
	const int asciiMaxIndex = 126; //'~'
	const int alphabetLength = asciiMaxIndex - asciiMinIndex + 1;

	//iteracje opieraj¹ siê na maksymalnej liczbie permutacji tablicy z dopuszczalnymi znakami
	for (int n = 1; n <= keyLength; n++)
	{
		max_perms += (unsigned long long)pow(alphabetLength, n);
	}

	omp_set_dynamic(0);     // Explicitly disable dynamic teams
	omp_set_num_threads(numOfThreads); // Use 4 threads for all consecutive parallel regions

	if (DEBUG_MODE)
	{
		printf("OMP max threads: %d, alphabetLength = %d, MAX_KEY_LENGTH = %d, max_perms = %lld\n", omp_get_max_threads(), alphabetLength, keyLength, max_perms);
	}
	

	#pragma omp parallel
	{
		char *encryptedResult = malloc(sizeof(char) * (strlen(password) + 1));
		char *key = malloc(keySize);

		memset(key, '\0', keySize);
		int pos = 0;

		if (DEBUG_MODE)
		{
			printf("Thread: %d starting\n", omp_get_thread_num());
		}

		#pragma omp for
		for (unsigned long long count = 0; count < max_perms; count++)
		{
			int pos = 0;
			unsigned long long tmp = count + 1;
			for (unsigned long long tmp = count + 1; tmp > 0; tmp /= (unsigned long long)alphabetLength)
			{
				key[pos++] = (char)(tmp % alphabetLength) + asciiMinIndex;
			}

			encrypt(password, key, encryptedResult);

			if (strcmp(encrypted, encryptedResult) == 0)
			{
				if (DEBUG_MODE)
				{
					printf("Thread: %d, key = \"%s\", count = %lld, FOUND!\n", omp_get_thread_num(), key, count);
				}
				
				#pragma omp atomic write
				keyResult = _strdup(key);
				#pragma omp cancel for
			}

			#pragma omp cancellation point for
		}

		free(encryptedResult);
		free(key);
	}

	return keyResult;
}