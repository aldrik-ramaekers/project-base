/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

static settings_config _settings_file = {0};

void settings_write_to_file()
{
	// @hardcoded
	s32 len = kilobytes(20);
	char *buffer = mem_alloc(len);
	buffer[0] = 0;
	
	for (s32 i = 0; i < _settings_file.settings.length; i++)
	{
		config_setting *setting = array_at(&_settings_file.settings, i);
		
		char entry_buf[MAX_INPUT_LENGTH];
		snprintf(entry_buf, MAX_INPUT_LENGTH, "%s = \"%s\"\n", setting->name, setting->value);
		string_appendn(buffer, entry_buf, len);
	}
	
	set_active_directory(binary_path);
	platform_write_file_content(_settings_file.path, "w+", buffer, strlen(buffer));
	mem_free(buffer);
}

static void get_config_from_string(settings_config *config, char *string)
{
	config_setting current_entry;
	current_entry.name = 0;
	current_entry.value = 0;
	
	s32 len = 0;
	bool in_literal = false;
	
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

void settings_init(char *directory)
{
	settings_config config;
	config.loaded = true;
	
	platform_create_config_directory(directory);

	config.path = mem_alloc(MAX_INPUT_LENGTH);
	get_config_save_location(config.path, directory);

	config.settings = array_create(sizeof(config_setting));
	
	set_active_directory(binary_path);
	
	file_content content = platform_read_file_content(config.path, "rb");
	
	if (!content.content || content.file_error)
	{
		_settings_file = config;
		platform_destroy_file_content(&content);
		return;
	}
	
	string_convert_crlf_to_lf(content.content);
	
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
	_settings_file = config;
}

config_setting* settings_get_setting(char *name)
{
	assert(_settings_file.loaded);
	
	for (s32 i = 0; i < _settings_file.settings.length; i++)
	{
		config_setting *setting = array_at(&_settings_file.settings, i);
		if (setting && setting->name && name && strcmp(setting->name, name) == 0)
		{
			return setting;
		}
	}
	return 0;
}

char* settings_get_string(char *name)
{
	assert(_settings_file.loaded);

	config_setting* setting = settings_get_setting(name);
	if (setting)
		return setting->value;
	else
		return 0;
}

s64 settings_get_number(char *name)
{
	assert(_settings_file.loaded);

	config_setting* setting = settings_get_setting(name);
	if (setting && setting->value)
		return string_to_u64(setting->value);
	else
		return 0;
}

s64 settings_get_number_or_default(char *name, s64 def)
{
	assert(_settings_file.loaded);

	config_setting* setting = settings_get_setting(name);
	if (setting && setting->value)
		return string_to_u64(setting->value);
	else
		return def;
}

void settings_set_string(char *name, char *value)
{
	assert(_settings_file.loaded);

	config_setting* setting = settings_get_setting(name);
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
		
		array_push(&_settings_file.settings, &new_entry);
	}
}

void settings_set_number(char *name, s64 value)
{
	assert(_settings_file.loaded);

	config_setting* setting = settings_get_setting(name);
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
		array_push(&_settings_file.settings, &new_entry);
	}
}

void settings_destroy()
{
	assert(_settings_file.loaded);

	for (s32 i = 0; i < _settings_file.settings.length; i++)
	{
		config_setting *entry = array_at(&_settings_file.settings, i);
		
		mem_free(entry->name);
		mem_free(entry->value);
	}
	
	array_destroy(&_settings_file.settings);
	mem_free(_settings_file.path);
}