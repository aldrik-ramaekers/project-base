
s32 test_string_to_number()
{
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

s32 test_string_contains()
{
    error_if(string_contains("lll", "llll*"));
    error_if(string_contains("lllll", "l*lop"));
    error_if(string_contains("hello world", "h?lo"));
    error_if(string_contains("hello world", "h*lo")); // The first L(1) matches with the first L(2), so the next check would be l == o, hence failure.
    error_if(string_contains("lllll", "llll*l"));
    error_if(string_contains("lllll", "*llllll*"));

    error_if(!string_contains("22lllll", "l*l"));
    error_if(!string_contains(" wsdf asd \"hello sailor\" asdf asdf ", "sailor"));
    error_if(!string_contains(" wsdf asd \"hello sailor\" asdf asdf ", "*sailor"));
    error_if(!string_contains(" wsdf asd \"hello sailor\" asdf asdf ", "*sailor\""));
    error_if(!string_contains(" wsdf asd \"hello sailor\" asdf asdf ", "*sailor*"));
    error_if(!string_contains(" wsdf asd \"hello sailor\" asdf asdf ", "sailor*"));
    error_if(!string_contains("22lllll pi23hjp rbksje LSKJDh l", "LS*"));
    error_if(!string_contains("22lllll lal", "l*l"));
    error_if(!string_contains("lllll", "*l*l"));
    error_if(!string_contains("hello world", "hello"));
    error_if(!string_contains("hello world", "h?llo"));
    error_if(!string_contains("hello world", "h????"));
    error_if(!string_contains("hello world", "*"));
    error_if(!string_contains("hello world", "h*"));
    error_if(!string_contains("hello world", "*o"));
    error_if(!string_contains("hello world", "h*o"));
    error_if(!string_contains("hello world", "*lo"));
    error_if(!string_contains("hello world", "hello"));
    error_if(!string_contains("lllll", "l*l"));
    error_if(!string_contains("lllll", "lllll"));
    error_if(!string_contains("lllll", "l*lll"));
    error_if(!string_contains("lllll", "*"));
    error_if(!string_contains("lllll", "l?lll"));
    error_if(!string_contains("lllll", "lllll"));
    error_if(!string_contains("lllll", "*llll"));
    error_if(!string_contains("lllll", "llll*"));
    error_if(!string_contains("lllll", "*llll*"));
    error_if(!string_contains("lllll", "*lllll*"));
    error_if(!string_contains("lllll", "*ll*"));

    success;
}