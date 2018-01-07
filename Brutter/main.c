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

#include "bruteforce.h"
#include "crypter.h"

//Parametry programu:
const char PASSWORD[] = "pwd1234";
const char SECRET_KEY[] = "!ABC";
const int MAX_KEY_LENGTH = 8;
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

	Sleep(3000); // niech u¿ytkownik zobaczy pocz¹tkowy header!

	if (PRINT_ITERATION_OUTPUT == 1)
	{
		printf("Rozpoczynamy iterowanie w celu znalezienia klucza:\n");
		Sleep(2000); // j.w.
	}
	else
	{
		printf("(Wylaczono wyswietlanie iteracji lancucha znakow)\n");
	}

	clock_t start = clock();
	result = bruteforce(PASSWORD, encrypted);
	clock_t end = clock();

	if (result != NULL)
	{
		printf("Znaleziono klucz szyfrujacy: %s\n", result);
	}
	else
	{
		printf("Nie znaleziono klucza szyfrujacego!\n");
	}

	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Operacja zajela %.3f sekund.\n", seconds);

	free(result);
}


