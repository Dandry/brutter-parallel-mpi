/*
Algorytm do generowania stringów na potrzeby metody brute force zaczerpnięty ze strony
http://hacksenkessel.com/2014/brute-force-algorithm-in-c/, przepisany przeze mnie z C++ na C
oraz dostosowany do bieżących potrzeb
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#include "crypter.h"
#include "main.h"

char * bruteforce(char *password, char *encrypted, int keyLength, int *resultExist)
{
	const int root = 0;
	int proc_id, world_size;
	int sizeUll = sizeof(unsigned long long);

	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	size_t keySize = sizeof(char) * (keyLength + 1);
	char* keyResult = NULL;
	unsigned long long max_perms = 0;

	//Założenie: klucz składa się tylko ze znaków, które można wywołać klawiaturą
	const int asciiMinIndex = 32; //' '
	const int asciiMaxIndex = 68; //'~'
	const int alphabetLength = asciiMaxIndex - asciiMinIndex + 1;

	unsigned long long procStartNmb, procEndNmb;

	if (proc_id == root)
	{
		//iteracje opierają się na maksymalnej liczbie permutacji tablicy z dopuszczalnymi znakami
		for (int n = 1; n <= keyLength; n++)
		{
			max_perms += (unsigned long long)pow(alphabetLength, n);
		}

		unsigned long long chunkSize = max_perms / (unsigned long long)world_size;
		unsigned long long startNumber = 0;
		unsigned long long endNumber = 0;

		for (int i = 0; i < world_size; i++)
		{
			endNumber += chunkSize;
			//printf("PID %d : endNumber =  %lld \n", proc_id, endNumber);

			if (i == 0)
			{
				procStartNmb = startNumber;
				procEndNmb = endNumber;
			}
			else 
			{
				MPI_Send(&startNumber, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
				MPI_Send(&endNumber, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
			}

			startNumber = endNumber;
		}
	}
	else
	{
		MPI_Status status;
		MPI_Recv(&procStartNmb, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&procEndNmb, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
	}
	
	//na tym etapie każdy z procesów wie juz, jaki zakres ma do iteracji
	char *encryptedResult = malloc(sizeof(char) * (strlen(password) + 1));
	char *key = malloc(keySize);
	memset(key, '\0', keySize);

	int localStop = 0;
	int globalStop;
	
	for (unsigned long long count = procStartNmb; count < procEndNmb; count++)
	{
		//printf("PID: %d, tu jestem!\n", proc_id);
		if (globalStop == 1) break;

		//printf("PID: %d, tu jestem!\n", proc_id);

		int pos = 0;
		for (unsigned long long tmp = count + 1; tmp > 0 && pos < keyLength; tmp /= (unsigned long long)alphabetLength)
		{
			key[pos++] = (char)(tmp % alphabetLength) + asciiMinIndex;
		}

		encrypt(password, key, encryptedResult);

		//printf("PID: %d, key = %s\n", proc_id, key);

		if (strcmp(encrypted, encryptedResult) == 0)
		{
			if (DEBUG_MODE)
			{
				printf("PID: %d, key = \"%s\", count = %lld, FOUND!\n", proc_id, key, count);
			}

			localStop = 1;
			keyResult = strdup(key);
			break;
		}

		MPI_Request request;
		int did_recv = 0;
		MPI_Iallreduce(&localStop, &globalStop, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD, &request);
		MPI_Test(&request, &did_recv, MPI_STATUS_IGNORE);
	}

	free(encryptedResult);
	free(key);

	*resultExist = globalStop;
	return keyResult;
}