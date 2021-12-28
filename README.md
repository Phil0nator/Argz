# Argz

Argz is a basic lightweight commandline argument parsing library written in plain C. In order to be as lightweight as possible, the implimentation doesn't use any dynamic allocation, and relies only on the basic C standard library functions like strcpy, etc...
Argz is best suited to use cases where the expected arguments are just a simple mix of flags, keywords, and positionals. Beyond that, a more complex library would probably be more useful.

## Hello World Parser

```c
#include <argz/argz.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char** argv )
{
    // create a parser object
    ArgzParser parser;
    // populate the parser with the arguments given to the program, and a name for the program
    argzCreateParser(&parser, argc, argv, "Hello World");

    // add a positional argument to the parser (positionals are collected in the order they are added to the parser)
    ArgzPositional message;
    message.name = "message";
    message.description = "User's message for echo";

    argzAddPositional(&parser, &message);

    // Parse the arguments. If the input from the user is ill-formed, argzParse() will display an error and exit. Otherwise, the input we wanted should be in the message.value field as a cstring
    argzParse(&parser);

    printf("Hello World!\n%s\n", message.value);
    return EXIT_SUCCESS;
}
```

### Outputs:
- ```./hello_world```
```
Hello World: Missing positional: message
usage: Hello World [ -h ] message 
Flag Arguments: 
        -h, --help,     Display this help message
Positional Arguments: 
        message         User's message for echo
```

- ```./hello_world Hello?```
```
Hello World!
Hello?
```

## Extended Example Code

```c
#include <argz/argz.h>
#include <stdlib.h>
int main(int argc, const char** argv)
{

    // create a parser object
    ArgzParser parser;
    // populate the parser object with our arguments and a name for our program
    argzCreateParser( &parser, argc, argv, "My Program" );

    // create a flag argument. Flags are used for things that can be set to either true or false. After parsing, the flag's 'present' field will have a boolean value.
    ArgzFlag myFlag;
    // shortkey and longkey allow you to specify two forms that the same flag may take (both must start with '-')
    myFlag.shortkey = "-f";
    myFlag.longkey = "--flag";
    myFlag.description = "A Flag Argument is true/false";

    argzAddFlag(&parser, &myFlag);

    // create a keyword argument. Keyword arguments associate a given key with the user's inputs. 
    // e.g: ./program -k value
    ArgzKeyword myKeyword;
    // shortkey and lonkey allow you to specify two keys for the same keyword (both must start with '-')
    myKeyword.shortkey = "-k";
    myKeyword.longkey = "--keyword";
    // keywords may either be optional or required
    myKeyword.required = true;
    myKeyword.description = "A Keyword accepts a string value";

    argzAddKeyword(&parser, &myKeyword);

    // positional arguments are any arguments that are neither flags nor keywords. These will be collected in the order they are added to the parser. ALL positionals are required
    ArgzPositional myPositional;
    // this name is just use to display help messages
    myPositional.name = "positional";
    myPositional.description = "Positionals are ordered and required";

    argzAddPositional( &parser, &myPositional );


    argzParse(&parser);

    // the values for the arguments once they have been parsed will be cstrings in the 'value' field of keywords and positionals, and any flag arguments will have a boolean value in the 'present' field.
    printf("flag: %s\nKeyword: %s\nPositional: %s\n", myFlag.present ? "true" : "false", myKeyword.value, myPositional.value );
    
    // Once you've used the arguments there is no need to free anything because nothing is dynamically allocated.
    
    return EXIT_SUCCESS;

}
```

### Outputs

- ```./extended```
```
My Program: Missing positional: positional
usage: My Program [ -h ] [ -f ] [ -k KEYWORD ] positional 
Flag Arguments: 
        -h, --help,     Display this help message
        -f, --flag,     A Flag Argument is true/false
Keyword Arguments: 
        -k, --keyword,  A Keyword accepts a string value
Positional Arguments: 
        positional      Positionals are ordered and required
```
- ```./extended -f -k KeywordInput PositionalInput```
```
flag: true
Keyword: KeywordInput
Positional: PositionalInput
```
- ```./extended PositionalInput --keyword KeywordInput -f```
```
flag: true
Keyword: KeywordInput
Positional: PositionalInput
```
- ```./extended PositionalInput```
```
My Program: Missing required keyword: -k
usage: My Program [ -h ] [ -f ] [ -k KEYWORD ] positional 
Flag Arguments: 
        -h, --help,     Display this help message
        -f, --flag,     A Flag Argument is true/false
Keyword Arguments: 
        -k, --keyword,  A Keyword accepts a string value
Positional Arguments: 
        positional      Positionals are ordered and required
```
- etc...

## Building

Argz provides CMakeLists.txt for use with cmake projects. Otherwise, to build the static library for use elsewhere you can use the normal CMake routine:
```
mkdir build && cd build
cmake .. && make
```
The makefile will output a file called libargz.a which can be statically linked to other projects. The argz.h file is found in `include/argz`.