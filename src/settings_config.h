/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_SETTINGS_CONFIG
#define INCLUDE_SETTINGS_CONFIG

typedef struct t_config_setting
{
	char *name;
	char *value;
} config_setting;

typedef struct t_settings_config
{
	char *path;
	array settings;
	bool loaded;
} settings_config;

/* Example of file:
*  NAME = "Aldrik Ramaekers"
*  AGE = "69"
*  NUMBER = "15"
*/

void settings_init(char *path);
void settings_write_to_file();
void settings_destroy();

config_setting* settings_get_setting(char *name);
char* settings_get_string(char *name);
s64 settings_get_number(char *name);
s64 settings_get_number_or_default(char *name, s64 def);

void settings_set_string(char *name, char *value);
void settings_set_number(char *name, s64 value);

#endif