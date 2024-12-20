/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void settings_config_write_to_file(settings_config *config, char *path)
{
	// @hardcoded
	s32 len = kilobytes(20);
	char *buffer = mem_alloc(len);
	buffer[0] = 0;
	
	for (s32 i = 0; i < config->settings.length; i++)
	{
		config_setting *setting = array_at(&config->settings, i);
		
		char entry_buf[MAX_INPUT_LENGTH];
		snprintf(entry_buf, MAX_INPUT_LENGTH, "%s = \"%s\"\n", setting->name, setting->value);
		string_appendn(buffer, entry_buf, MAX_INPUT_LENGTH);
	}
	
	set_active_directory(binary_path);
	platform_write_file_content(path, "w+", buffer, strlen(buffer));
	mem_free(buffer);
}

static void get_config_from_string(settings_config *config, char *string)
{
	config_setting current_entry;
	current_entry.name = 0;
	current_entry.value = 0;
	
	s32 len = 0;
	bool in_literal = false;
	char *original_string = string;
	
	while(*string)
	{
		
		// property name
		if (*string == ' ' && !current_entry.name)
		{
			current_entry.name = mem_alloc(len+1);
			string_copyn(current_entry.name, string-len, len);
			current_entry.name[len] = 0;
			string_trim(current_entry.name);
		}
		
		// property value
		if (*string == '"' && (*(string+1) == 0 || !in_literal))
		{
			in_literal = !in_literal;
			
			if (in_literal)
			{
				len = -1;
			}
			else
			{
				current_entry.value = mem_alloc(len+1);
				string_copyn(current_entry.value, string-len, len);
				current_entry.value[len] = 0;
				string_trim(current_entry.value);
			}
		}
		
		++len;
		++string;
	}
	
	array_push(&config->settings, &current_entry);
}

static void convert_crlf_to_lf(char *buffer)
{
	char *buffer_original = buffer;
	
	int write_offset = 0;
	int read_offset = 0;
	
	while(buffer[read_offset])
	{
		if (buffer[read_offset] != 0x0D)
		{
			buffer_original[write_offset] = buffer[read_offset];
			
			++write_offset;
		}
		
		++read_offset;
	}
}

settings_config settings_config_load_from_file(char *path)
{
	settings_config config;
	config.settings = array_create(sizeof(config_setting));
	
	set_active_directory(binary_path);
	
	file_content content = platform_read_file_content(path, "rb");
	
	if (!content.content || content.file_error)
	{
		platform_destroy_file_content(&content);
		return config;
	}
	
	convert_crlf_to_lf(content.content);
	
	s32 token_offset = 0;
	for (s32 i = 0; i < content.content_length; i++)
	{
		char ch = ((char*)content.content)[i];
		char prev_ch = i-1 > 0 ? ((char*)content.content)[i-1] : 255;
		
		// end of line [lf]
		if (ch == 0x0A && prev_ch != 0x0D)
		{
			char line[MAX_INPUT_LENGTH];
			
			s32 line_len = i - token_offset;
			snprintf(line, MAX_INPUT_LENGTH, "%.*s", line_len, (char*)content.content+token_offset);
			token_offset = i + 1;
			
			get_config_from_string(&config, line);
		}
	}
	
	platform_destroy_file_content(&content);
	
	return config;
}

config_setting* settings_config_get_setting(settings_config *config, char *name)
{
	for (s32 i = 0; i < config->settings.length; i++)
	{
		config_setting *setting = array_at(&config->settings, i);
		if (setting && setting->name && name && strcmp(setting->name, name) == 0)
		{
			return setting;
		}
	}
	return 0;
}

char* settings_config_get_string(settings_config *config, char *name)
{
	config_setting* setting = settings_config_get_setting(config, name);
	if (setting)
		return setting->value;
	else
		return 0;
}

s64 settings_config_get_number(settings_config *config, char *name)
{
	config_setting* setting = settings_config_get_setting(config, name);
	if (setting && setting->value)
		return string_to_u64(setting->value);
	else
		return 0;
}

s64 settings_config_get_number_or_default(settings_config *config, char *name, s64 def)
{
	config_setting* setting = settings_config_get_setting(config, name);
	if (setting && setting->value)
		return string_to_u64(setting->value);
	else
		return def;
}

void settings_config_set_string(settings_config *config, char *name, char *value)
{
	config_setting* setting = settings_config_get_setting(config, name);
	if (setting)
	{
		s32 len = strlen(value);
		mem_free(setting->value);
		setting->value = mem_alloc(len+1);
		string_copyn(setting->value, value, len+1);
	}
	else
	{
		config_setting new_entry;
		new_entry.name = 0;
		
		// name
		s32 len = strlen(name);
		new_entry.name = mem_alloc(len+1);
		string_copyn(new_entry.name, name, len+1);
		
		// value
		len = strlen(value);
		new_entry.value = mem_alloc(len+1);
		string_copyn(new_entry.value, value, len+1);
		
		array_push(&config->settings, &new_entry);
	}
}

void settings_config_set_number(settings_config *config, char *name, s64 value)
{
	config_setting* setting = settings_config_get_setting(config, name);
	if (setting)
	{
		char num_buf[20];
		snprintf(num_buf, 20, "%"PRId64"", value);
		
		s32 len = strlen(num_buf);
		mem_free(setting->value);
		setting->value = mem_alloc(len+1);
		string_copyn(setting->value, num_buf, len+1);
	}
	else
	{
		config_setting new_entry;
		
		// name
		s32 len = strlen(name);
		new_entry.name = mem_alloc(len+1);
		string_copyn(new_entry.name, name, len+1);
		
		// value
		char num_buf[20];
		snprintf(num_buf, 20, "%"PRId64"", value);
		
		len = strlen(num_buf);
		new_entry.value = mem_alloc(len+1);
		string_copyn(new_entry.value, num_buf, len+1);
		array_push(&config->settings, &new_entry);
	}
}

void settings_config_destroy(settings_config *config)
{
	for (s32 i = 0; i < config->settings.length; i++)
	{
		config_setting *entry = array_at(&config->settings, i);
		
		mem_free(entry->name);
		mem_free(entry->value);
	}
	
	array_destroy(&config->settings);
}