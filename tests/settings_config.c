s32 _settings_config_write(int argc, char** argv) {
    platform_init(argc, argv);
    settings_init(CONFIG_DIRECTORY);

    settings_set_number("number1", 1);
    settings_set_number("number2", 2);
    settings_set_string("string1", "String 123");
    settings_set_string("string2", "321 String");

    settings_write_to_file();

    error_if(_settings_file.settings.length != 4);

    settings_destroy();
    platform_destroy();
    success;
}

s32 _settings_config_read(int argc, char** argv) {
    platform_init(argc, argv);
    settings_init(CONFIG_DIRECTORY);

    int num1 = settings_get_number("number1");
    int num2 = settings_get_number("number2");

    char* string1 = settings_get_string("string1");
    char* string2 = settings_get_string("string2");

    int def = settings_get_number_or_default("wrong", 15);

    error_if(num1 != 1);
    error_if(num2 != 2);
    error_if(!string_equals(string1, "String 123"));
    error_if(!string_equals(string2, "321 String"));
    error_if(def != 15);

    settings_destroy();
    platform_destroy();
    success;
}