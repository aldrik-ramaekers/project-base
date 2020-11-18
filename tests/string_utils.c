
s32 string_to_number() {
    error_if(string_to_u64("5294967295") != 5294967295);
    error_if(string_to_u32("294967295") != 294967295);
    error_if(string_to_u16("2544") != 2544);
    error_if(string_to_u8("244") != 244);

    error_if(string_to_s64("-12") != -12);
    error_if(string_to_s32("-12") != -12);
    error_if(string_to_s16("-12") != -12);
    error_if(string_to_s8("-12") != -12);

    error_if(string_to_f32("-1.259223") != -1.259223f);
    error_if(string_to_f64("346.12323") != 346.12323);

    success;
}