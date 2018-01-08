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
#include <windows.h>
#include <time.h>
#include <omp.h>

#include "bruteforce.h"
#include "crypter.h"

//Parametry programu:
const char PASSWORD[] = "pwd1234";
const char SECRET_KEY[] = "zABC";
const char MAX_KEY_LENGTH = 8;
const int PRINT_ITERATION_OUTPUT = 0;

void printHeader()
{
	printf("############### Brutter v1.0 ###############\n");
	printf("Autor: Daniel Andraszewski    \n");
	printf("\n");
}

int main(int argc, char *argv[]) 
{
	char * result = NULL;
	char encrypted[sizeof(PASSWORD)];

	printHeader();
	secretEncrypt(PASSWORD, encrypted);

	printf("Zdefiniowane haslo: %s\n", PASSWORD);
	printf("Otrzymane zaszyfrowane haslo: %s\n", encrypted);
	printf("\n");

	Sleep(2000); // niech u¿ytkownik zobaczy pocz¹tkowy header!

	if (PRINT_ITERATION_OUTPUT)
	{
		printf("Rozpoczynamy iterowanie w celu znalezienia klucza:\n");
		Sleep(1000); // j.w.
	}
	else
	{
		printf("(Wylaczono wyswietlanie iteracji lancucha znakow)\n");
	}

	double start_time = omp_get_wtime();
	result = bruteforce(PASSWORD, encrypted);
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

	//free(result);
	return 0;
}


