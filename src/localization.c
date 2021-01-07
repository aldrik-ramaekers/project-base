/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void localization_load(u8 *start_addr, u8 *end_addr, u8 *img_start, u8 *img_end,
							   char *locale_id, char *locale_name)
{
	mo_file mo;
	mo.translations = array_create(sizeof(mo_translation));
	
	{
		mo.header = *(mo_header*)start_addr;
		mo.locale_id = mem_alloc(strlen(locale_id)+1);
		string_copyn(mo.locale_id, locale_id, strlen(locale_id)+1);
		
		mo.locale_full = mem_alloc(strlen(locale_name)+1);
		string_copyn(mo.locale_full, locale_name, strlen(locale_name)+1);
		
		if (img_start) {
			mo.icon = assets_load_bitmap(img_start, img_end);
		}
		else {
			mo.icon = 0;
		}
		
		char *buffer = (char*)start_addr;
		mo_entry *identifiers = (mo_entry*)(buffer + mo.header.identifier_table_offset);
		mo_entry *translations = (mo_entry*)(buffer + mo.header.translation_table_offset);
		
		for (s32 i = 0; i < mo.header.number_of_strings; i++)
		{
			mo_entry *entry = &identifiers[i];
			mo_entry *trans = &translations[i];
			
			mo_translation translation;
			translation.identifier_len = entry->length;
			translation.identifier = buffer+entry->offset;
			translation.translation = buffer+trans->offset;
			
			array_push(&mo.translations, &translation);
		}
	}

	array_push(&global_localization.mo_files, &mo);
}

char* locale_get_name()
{
	log_assert(global_localization.loaded, "localization_init() should be called before using any localization functions");

	if (!global_localization.active_localization)
	{
		return "[NO LOCALE]";
	}
	
	return global_localization.active_localization->locale_full;
}

char* locale_get_id()
{
	log_assert(global_localization.loaded, "localization_init() should be called before using any localization functions");
	
	if (!global_localization.active_localization)
	{
		return "[NO LOCALE]";
	}
	
	return global_localization.active_localization->locale_id;
}

bool localization_set_locale(char *country_id)
{
	log_assert(global_localization.loaded, "localization_init() should be called before using any localization functions");

	if (country_id == 0 && global_localization.mo_files.length)
	{
		global_localization.active_localization = array_at(&global_localization.mo_files, 0);
		return true;
	}
	
	for (s32 i = 0; i < global_localization.mo_files.length; i++)
	{
		mo_file *file = array_at(&global_localization.mo_files, i);
		if (strcmp(file->locale_id, country_id) == 0)
		{
			global_localization.active_localization = file;
			return true;
		}
	}
	
	// if localization is not found, default to first in list, return false to report error
	if (global_localization.mo_files.length)
		global_localization.active_localization = array_at(&global_localization.mo_files, 0);
	else
		global_localization.active_localization = 0;
	
	return false;
}

char* localize(const char *identifier)
{
	log_assert(global_localization.loaded, "localization_init() should be called before using any localization functions");

	if (!global_localization.active_localization)
	{
		return (char*)identifier;
	}
	
	s32 len = strlen(identifier);
	for (s32 i = 0; i < global_localization.active_localization->translations.length; i++)
	{
		mo_translation *trans = array_at(&global_localization.active_localization->translations, i);
		
		if (trans->identifier_len == len && strcmp(identifier, trans->identifier) == 0)
		{
			return trans->translation;
		}
	}
	printf("MISSING TRANSLATION: [%s][%s]\n", identifier, global_localization.active_localization->locale_id);
	return "MISSING";
}

void localization_init()
{
	global_localization.mo_files = array_create(sizeof(mo_file));
	array_reserve(&global_localization.mo_files, 10);

	global_localization.loaded = true;
}

void localization_destroy()
{
	log_assert(global_localization.loaded, "localization_init() should be called before using any localization functions");

	for (s32 i = 0; i < global_localization.mo_files.length; i++)
	{
		mo_file *file = array_at(&global_localization.mo_files, i);
		array_destroy(&file->translations);
		mem_free(file->locale_id);
		mem_free(file->locale_full);
		
		if (file->icon)
			assets_destroy_bitmap(file->icon);
	}
	array_destroy(&global_localization.mo_files);
}