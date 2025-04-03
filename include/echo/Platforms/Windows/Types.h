// Windows.h really screws with nice naming.
//
// Undefine any macros you find interfering with the engine. The way things are
// structure means that any Generic Echo Engine code or Projects should never be
// using Windows code anyway. Any Windows specific code that needs to use the
// macros should include the relevant Windows files after Echo files, but you
// might still run into problems regarding macros not being defined. To work
// around this issue, this file will use push_macro("MACRO_NAME") then undefine
// it. This means your code can later pop_macro("MACRO_NAME") after all of the
// includes.


#pragma push_macro("ABSOLUTE")
#undef ABSOLUTE
#pragma push_macro("RELATIVE")
#undef RELATIVE
#pragma push_macro("IGNORE")
#undef IGNORE
#pragma push_macro("INFINITE")
#undef INFINITE
