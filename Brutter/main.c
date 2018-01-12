/*
################## Brutter v1.0 ######################
Autor: Daniel Andraszeski
Program na potrzeby z projektu przedmiotu
Programowanie Równoległe i Rozproszone
Politechnika Warszawska, 2017/2018
Program do łamania hasła zaszyfrowanego algorytmem XOR
przy użyciu metody bruteforce
######################################################
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "bruteforce.h"
#include "crypter.h"

//Diagnostic
const int DEBUG_MODE = 0;

void printHeader();
int checkCommandLineParams(int argc);
void checkOMPCancellation(int argc, char *argv[]);

//wywołanie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania] [ilosc_watkow]
int main(int argc, char *argv[])
{
	if (checkCommandLineParams(argc) == 0) return 0;
	checkOMPCancellation(argv[0], argv);

	printHeader();

	char *password = _strdup(argv[1]);
	char *secretKey = _strdup(argv[2]);

	int keyLength = strlen(secretKey);
	int numOfThreads = atoi(argv[3]);
	char encrypted[strlen(password)];

	encrypt(password, secretKey, encrypted);

	printf("Zdefiniowane haslo: %s\n", password);
	printf("Otrzymane zaszyfrowane haslo: %s\n", encrypted);
	printf("Watki: %d\n", numOfThreads);
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

	double start_time = omp_get_wtime();
	char *result = bruteforce(password, encrypted, keyLength, numOfThreads);
	double time = omp_get_wtime() - start_time;

	if (result != NULL)
	{
		printf("Znaleziono klucz szyfrujacy: %s\n", result);
	}
	else
	{
		printf("Nie znaleziono klucza szyfrujacego!\n");
	}

	printf("Operacja zajela %.3f sekund.\n", time);
}

void printHeader()
{
	printf("\n############### Brutter v1.0 ###############\n");
	printf("Autor: Daniel Andraszewski\n");
	printf("\n");
}

int checkCommandLineParams(int argc)
{
	if (argc != 4)
	{
		printf("Niepoprawne parametry wywolania programu.\n");
		printf("Wywolanie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania] [ilosc_watkow]\n");
		return 0;
	}

	return 1;
}

void checkOMPCancellation(int argc, char *argv[])
{
	if (!omp_get_cancellation())
	{
		printf("Przerwania OMP (cancellations) sa wylaczone. Nastapi ich wlaczenie i ponowne wykonanie programu.\n");
		_putenv("OMP_CANCELLATION=true");
		execv(argv[0], argv);
	}
}

