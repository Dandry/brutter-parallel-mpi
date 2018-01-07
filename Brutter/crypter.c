/*
Algorytm szyfrowania XOR zaczerpniêty ze repozytorium https://github.com/KyleBanks/XOREncryption/tree/master/C.
Uzupe³niony o drobne poprawki z mojej strony
*/

#include "main.h"

void encryptDecrypt(char *input, char *key, char *output)
{
	int i;
	for (i = 0; i < strlen(input); i++) {
		output[i] = input[i] ^ key[i % (strlen(key) * sizeof(char))];
	}

	output[i] = '\0';
}

void secretEncrypt(char *input, char *output)
{
	encryptDecrypt(input, SECRET_KEY, output);
}

void encrypt(char *input, char *key, char *output)
{
	encryptDecrypt(input, key, output);
}

