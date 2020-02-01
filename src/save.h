/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_SAVE
#define INCLUDE_SAVE

#define SAVE_FILE_EXTENSION "*.json"

void load_project_from_file(char *path_buf);
void load_project();
void save_project_to_file(char *path_buf);
void save_project();

#endif