/*
################## Brutter v3.0 ######################
################### Wersja MPI ######################
Autor: Daniel Andraszewski
Program na potrzeby z projektu przedmiotu
Programowanie Równoległe i Rozproszone
Politechnika Warszawska, 2017/2018
Program do łamania hasła zaszyfrowanego algorytmem XOR
przy użyciu metody bruteforce
######################################################
*/

#include <mpi.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <string.h>

#include "bruteforce.h"
#include "crypter.h"

//Diagnostic
const int DEBUG_MODE = 0;

//int main(int argc, char** argv) {
//
//	printf("PID : argc = %d\n", argc);
//
//	int ierr, procRank, worldSize;
//	int rootStartNumber, rootEndNumber;
//
//	ierr = MPI_Init(&argc, &argv);
//	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
//	ierr = MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
//
//	if (procRank == 0)
//	{
//		int max_perms = 100;
//
//		printf("Tu proces %d, dokonuje podzialu zmiennej %d pomiedzy procesy w liczbie %d.\n", procRank, max_perms, worldSize);
//		
//		int minChuckSize = (int) round((double)max_perms / (double)worldSize);
//		int startNumber = 0;
//		int endNumber = 0;
//		for (int i = 0; i < worldSize; i++)
//		{
//			endNumber += minChuckSize;
//			if (i == 0)
//			{
//				rootStartNumber = startNumber;
//				rootEndNumber = endNumber;
//			}
//			else 
//			{
//				if (i + 1 == worldSize)
//				{
//					endNumber = max_perms;
//				}
//				ierr = MPI_Send(&startNumber, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
//				ierr = MPI_Send(&endNumber, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
//			}
//			startNumber = endNumber;
//		}
//	}
//	else
//	{
//		MPI_Status status;
//		ierr = MPI_Recv(&rootStartNumber, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
//		ierr = MPI_Recv(&rootEndNumber, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
//	}
//	printf("Proces %d dostal zakres od %d do %d\n", procRank, rootStartNumber, rootEndNumber);
//	ierr = MPI_Finalize();
//}


int checkCommandLineParams(int argc, int proc_id, int world_size);
void printHeader();

void sendDataToSlaves(void* data);
void receiveDataFromMaster(void* data);

//wywołanie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania]
int main(int argc, char *argv[])
{
	int ierr, proc_id, world_size;
	//inicjalizacja MPI
	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//sprawdzenie parametrow wywolania programu i ewentualne zakonczenie
	if (checkCommandLineParams(argc, proc_id, world_size) == 0)
	{
		MPI_Finalize();
		return 0;
	}
		
	char *password = _strdup(argv[1]);
	char *secretKey = _strdup(argv[2]);

	int keyLength = strlen(secretKey);
	char encrypted[strlen(password)];


	if (proc_id == 0)
	{
		//printHeader();
		encrypt(password, secretKey, encrypted);
		printf("Zdefiniowane haslo: %s\n", password);
		printf("Otrzymane zaszyfrowane haslo: %s\n", encrypted);
		printf("Procesy: %d\n", world_size);
		printf("\n");

		if (DEBUG_MODE)
		{
			printf("DEBUG MODE ON\n");
		}
		else
		{
			printf("Szukam...\n");
			printf("\n");
		}
	}
	else
	{

	}

	double start_time = MPI_Wtime();
//	char *result = bruteforce(password, encrypted, keyLength, numOfThreads);
	char *result = NULL;
	double time = MPI_Wtime() - start_time;

	if (result != NULL)
	{
		printf("Znaleziono klucz szyfrujacy: %s\n", result);
	}
	else
	{
		printf("PID %d : Nie znaleziono klucza szyfrujacego!\n", proc_id);
	}

	printf("PID %d : Operacja zajela %.3f sekund.\n", proc_id, time);
	
	printf("PID %d : Konczy dzialanie.\n", proc_id);
	
	ierr = MPI_Finalize();
	return 0;
}

int checkCommandLineParams(int argc, int proc_id, int world_size)
{
	int cmd_params_valid = 1;

	if (proc_id == 0)
	{
		if (argc != 3)
		{
			cmd_params_valid = 0;

			printf("Niepoprawne parametry wywolania programu.\n");
			printf("Wywolanie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania]\n");
		}

		sendDataToSlaves(&cmd_params_valid);
	}
	else
	{
		receiveDataFromMaster(&cmd_params_valid);
	}
	
	return cmd_params_valid;
}

void printHeader()
{
	printf("\n############### Brutter v1.0 ###############\n");
	printf("Autor: Daniel Andraszewski\n");
	printf("\n");
}

void sendDataToSlaves(void* data)
{
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	for (int i = 1; i < world_size; i++)
	{
		MPI_Send(data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
}

void receiveDataFromMaster(void* data)
{
	MPI_Status status;
	MPI_Recv(data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
}