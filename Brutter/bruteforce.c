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

char * bruteforce(char *password, char *encrypted) 
{
	char *encryptedResult = malloc(sizeof(char) * (strlen(password) + 1));

	char key[MAX_KEY_LENGTH];
	char keyResult[MAX_KEY_LENGTH];
	memset(key, '\0', sizeof(key));

	int n;
	int	pos = 0;
	unsigned long long count = 0;
	unsigned long long max_perms = 0;

	//Za³o¿enie: klucz sk³ada siê tylko ze znaków, które mo¿na wywo³aæ klawiatur¹
	const int asciiMinIndex = 32;
	const int asciiMaxIndex = 127;
	const int charRange = asciiMaxIndex - asciiMinIndex;

	char *charArray = malloc((sizeof(char) * (charRange + 1)));

	int index = 0;
	for (char i = asciiMinIndex; i < asciiMaxIndex; i++)
	{
		charArray[index] = i;
		index++;
	}

	charArray[index] = '\0'; //NULL terminator na koñcu tablicy

	//iteracje opieraj¹ siê na maksymalnej liczbie permutacji tablicy z dopuszczalnymi znakami
	int alphabetLength = strlen(charArray);
	for (int n = 1; n <= MAX_KEY_LENGTH; n++)
	{
		max_perms += (unsigned long long)pow(alphabetLength, n);
	}


	omp_set_dynamic(0);     // Explicitly disable dynamic teams
	omp_set_num_threads(2); // Use 4 threads for all consecutive parallel regions

	int match = 0;
	while (count < max_perms)
	//#pragma omp parallel
	{
		#pragma omp parallel for shared(match) firstprivate(key, encryptedResult)
		for (int a = 0; a < alphabetLength; a++)
		{
			#pragma omp flush(match)
			if (match == 0)
			{
				//printf("Thread: %d, count = %d, a = %d, match = %d\n", omp_get_thread_num(), count, a, match);
				key[pos] = charArray[a];

				if (PRINT_ITERATION_OUTPUT)
				{
					printf("Thread: %d, key = %s\n", omp_get_thread_num(), key);
				}

				encrypt(password, key, encryptedResult);

				if (strcmp(encrypted, encryptedResult) == 0)
				{
					match = 1;
					printf("Znaleziono klucz! W¹tek: %d, a = %d, count = %d\n", omp_get_thread_num(), a, count);
					strcpy(keyResult, key);
				}
			}

			#pragma omp atomic
			count++;
		}

		if (match)
		{
			free(charArray);
			free(encryptedResult);
			//free(key);
			return keyResult;
		}

		if (count >= max_perms) break;

		for (n = pos; n >= 0; n--) {
			if (key[n] != charArray[alphabetLength - 1]) {
				key[n]++;
				break;
			}
			else {
				if (n == 0) {
					memset(key, (int)charArray[0], ++pos + 1);
					break;
				}
				key[n] = charArray[0];
			}
		}
	}

	free(charArray);
	free(encryptedResult);
	return NULL;
}