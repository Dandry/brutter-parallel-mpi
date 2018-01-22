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

int checkCommandLineParams(int argc, int proc_id, int world_size);
void printHeader();

//wywołanie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania]
int main(int argc, char *argv[])
{
	const int root = 0;
	int proc_id, world_size;
	
	//inicjalizacja MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//sprawdzenie parametrow wywolania programu i ewentualne zakonczenie
	if (checkCommandLineParams(argc, proc_id, world_size) == 0)
	{
		MPI_Finalize();
		return 0;
	}
		
	char *password = strdup(argv[1]);
	char *secretKey = strdup(argv[2]);
	int keyLength = strlen(secretKey);
	char encrypted[strlen(password)];
	int encryptedSize;

	if (proc_id == root)
	{
		printHeader();
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

		encryptedSize = strlen(encrypted);
	}

	//przekazujemy rezultat szyfrowania niejawnym kluczem do wszystkich procesow ale o jakiej dlugosci?
	//nie musimy wywolywac MPI_Probe bo wiemy, ze dlugosc strlen(encrypted) == strlen(password)
	MPI_Bcast(&encrypted, strlen(password) + 1, MPI_CHAR, 0, MPI_COMM_WORLD);

	//zmienna okresla id procesu, ktory znalazl rozwiazanie
	int resultProcId = -1;
	double start_time = MPI_Wtime();
	char *result = bruteforce(password, encrypted, keyLength, &resultProcId);
	double time = MPI_Wtime() - start_time;

	if (DEBUG_MODE)
	{
		printf("PID %d : resultProcId = %d\n", proc_id, resultProcId);
	}

	//niech wszystkie procesy tutaj sie zsynchronizuja, wtedy wiadomo bedzie, czy znaleziono rozwiazanie i ktory proces je ma
	MPI_Barrier(MPI_COMM_WORLD);
	if (resultProcId >= 0)
	{
		//jest rozwiazanie, resultProcId to rank procesu, ktory je ma i on wyswietla komunikat
		if (resultProcId == proc_id)
		{
			printf("PID %d : Znaleziono klucz szyfrujacy: %s\n", proc_id, result);
			printf("PID %d : Operacja zajela %.3f sekund.\n", proc_id, time);
		}
	}
	else if (proc_id == root)
	{
		//w przeciwnym wypadku tylko root wyswietla komunikat o nieznalezieniu rezultatu
		printf("PID %d : Nie znaleziono klucza szyfrujacego!\n", proc_id);
	}

	MPI_Finalize();
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
	}

	MPI_Bcast(&cmd_params_valid, 1, MPI_INT, 0, MPI_COMM_WORLD);
	return cmd_params_valid;
}

void printHeader()
{
	printf("\n############### Brutter v3.0 ###############\n");
	printf("Autor: Daniel Andraszewski\n");
	printf("\n");
}