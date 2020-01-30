/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#define LANGUAGE_CODE_SIZE 3

typedef struct t_language
{
	char code[3];
	char fullname[40];
} language;


language global_langues[] = {
	{"AD","Andorra"},
	{"AE","United Arab Emirates"},
	{"AF","Afghanistan"},
};

#define COUNTRY_CODE_COUNT (sizeof(global_langues)/sizeof(language))