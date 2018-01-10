/*
################## Brutter v1.0 ######################
Autor: Daniel Andraszewski    
Program na potrzeby z projektu przedmiotu
Programowanie Równoleg³e i Rozproszone
Politechnika Warszawska, 2017/2018
Program do ³amania has³a zaszyfrowanego algorytmem XOR
przy u¿yciu metody bruteforce
######################################################
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <windows.h>

#include "bruteforce.h"
#include "crypter.h"

//Parametry programu:
const char PASSWORD[] = "pwd1234";

void printHeader();
int checkCommandLineParams(int argc);
void checkOMPCancellation(int argc, char *argv[]);


//wywo³anie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania] [pokaz_wyniki_iteracji(0/1)]
int main(int argc, char *argv[])
{
	printf("argc = %d\n", argc);
	printf("%s, %s, %s\n", argv[1], argv[2], argv[3]);
	printHeader();
	if (checkCommandLineParams(argc) == 0) return 0;
	checkOMPCancellation(argv[0], argv);
	
	Sleep(10000);
	char *password = _strdup(argv[1]);
	char *secretKey = _strdup(argv[2]);

	printf("Copied pwd: %s\n", password);
	printf("Copied sk: %s\n", secretKey);

	int keyLength = strlen(secretKey);
	int printIterationOutput = atoi(argv[3]);
	char encrypted[strlen(password)];

	encrypt(password, secretKey, encrypted);

	printf("Zdefiniowane haslo: %s\n", password);
	printf("Otrzymane zaszyfrowane haslo: %s\n", encrypted);
	printf("\n");

	Sleep(3000); // niech u¿ytkownik zobaczy pocz¹tkowy header!

	if (printIterationOutput)
	{
		printf("Rozpoczynamy iterowanie w celu znalezienia klucza:\n");
		Sleep(2000); // j.w.
	}
	else
	{
		printf("(Wylaczono wyswietlanie iteracji lancucha znakow)\n");
	}

	double start_time = omp_get_wtime();
	char *result = bruteforce(password, encrypted, keyLength, printIterationOutput);
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

	return 0;
}

void printHeader()
{
	printf("############### Brutter v1.0 ###############\n");
	printf("Autor: Daniel Andraszewski    \n");
	printf("\n");
}

int checkCommandLineParams(int argc)
{
	if (argc != 4)
	{
		printf("Niepoprawne parametry wywolania programu.\n");
		printf("Wywolanie: Brutter [haslo_do_zaszyfrowania] [sekretny_klucz_szyfrowania] [pokaz_wyniki_iteracji(0/1)]\n");
		return 0;
	}

	return 1;
}

void checkOMPCancellation(int argc, char *argv[])
{
	if (!omp_get_cancellation())
	{
		printf("Przerwania OMP (cancellations) s¹ wylaczone. Nast¹pi ich w³aczenie i ponowne wykonanie programu.\n");
		_putenv("OMP_CANCELLATION=true");
		execv(argv[0], argv);
	}
}

