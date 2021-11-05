#include <argz/argz.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{

    ArgzParser parser =  ( ArgzParser ) {argc, argv, "testprog"};
    ArgzFlag f = ( ArgzFlag ) { "-f", "--flag", "test flag" };
    ArgzFlag f2 = ( ArgzFlag ) { "-f2", "--flag2", "test flag2" };
    ArgzKeyword k = ( ArgzKeyword ) { "-J", "--jobs", "Job count" };
    ArgzPositional p = ( ArgzPositional ) { "file", "Some file input" };

    argzAddFlag(&parser, &f);
    argzAddFlag(&parser, &f2);
    argzAddKeyword(&parser, &k);
    argzAddPositional(&parser, &p);
    argzParse(&parser);

    printf("%d", f.present);
    printf("%d", f2.present);

    printf("%s", k.value);

    printf("%s", p.value);

    return 0;
}
