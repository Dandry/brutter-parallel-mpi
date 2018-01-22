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

void assignPermsRangeToProcs(int alphabetLength, int keyLength, unsigned long long *procStartNmb, unsigned long long *procEndNmb);
void sendTerminateMsg(int proc_id, int world_size);

char * bruteforce(char *password, char *encrypted, int keyLength, int *resultProcId)
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
	const int asciiMaxIndex = 126; //'~'
	const int alphabetLength = asciiMaxIndex - asciiMinIndex + 1;

	unsigned long long procStartNmb, procEndNmb;
	assignPermsRangeToProcs(alphabetLength, keyLength, &procStartNmb, &procEndNmb);

	//na tym etapie każdy z procesów wie juz, jaki zakres ma do iteracji
	char *encryptedResult = malloc(sizeof(char) * (strlen(password) + 1));
	char *key = malloc(keySize);
	memset(key, '\0', keySize);

	//ustawiamy nieblokujacy listener dla kazdego procesu, jezeli ktorys znajdzie rozwiazanie, przerwie innym petle
	int globalStop;
	MPI_Request request;
	MPI_Irecv(resultProcId, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);
	
	if (DEBUG_MODE)
	{
		printf("PID %d : procStartNmb = %lld, procEndNmb = %lld\n", proc_id, procStartNmb, procEndNmb);
	}

	for (unsigned long long count = procStartNmb; count < procEndNmb; count++)
	{
		if (globalStop == 1) 
		{
			if (DEBUG_MODE)
			{
				printf("PID: %d, globalStop = 1, count = %lld\n", proc_id, count);
			}
			break;
		}

		int pos = 0;
		for (unsigned long long tmp = count + 1; tmp > 0 && pos < keyLength; tmp /= (unsigned long long)alphabetLength)
		{
			key[pos++] = (char)(tmp % alphabetLength) + asciiMinIndex;
		}

		encrypt(password, key, encryptedResult);

		if (strcmp(encrypted, encryptedResult) == 0)
		{
			if (DEBUG_MODE)
			{
				printf("PID: %d, key = \"%s\", count = %lld, FOUND!\n", proc_id, key, count);
			}

			int localStop = 1;
			keyResult = strdup(key);
			
			//proces, ktory znalazl rozwiazanie, wysyla komunikat do wszystkich (w tym do siebie), zeby zaprzestac iteracji
			sendTerminateMsg(proc_id, world_size);
		}

		//kazdy proces sprawdza, czy dostal informacje o ewentualnym przerwaniu petli
		MPI_Test(&request, &globalStop, MPI_STATUS_IGNORE);
	}

	free(encryptedResult);
	free(key);
	return keyResult;
}

void assignPermsRangeToProcs(int alphabetLength, int keyLength, unsigned long long *procStartNmb, unsigned long long *procEndNmb)
{
	const int root = 0;

	int proc_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

	if (proc_id == root)
	{
		int world_size;
		MPI_Comm_size(MPI_COMM_WORLD, &world_size);

		unsigned long long max_perms = 0;

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

			if (i == 0)
			{
				*procStartNmb = startNumber;
				*procEndNmb = endNumber;
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
		MPI_Recv(procStartNmb, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(procEndNmb, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
	}
}

void sendTerminateMsg(int proc_id, int world_size)
{
	for (int i = 0; i < world_size; i++)
	{
		MPI_Send(&proc_id, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
}
