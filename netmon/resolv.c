#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

char		*progname;

/************************************************************************
 ********************             ERROUT             ********************
 ************************************************************************/
void
errout(char *fmt, ...)
{
    va_list	ap;
    char	temp[256];

    sprintf(temp, "%s: ", progname);
    va_start(ap, fmt);
    vsprintf(temp+strlen(temp), fmt, ap);
    va_end(ap);
    if (errno)
	perror(temp);
    else
	fprintf(stderr, "%s\n", temp);
    exit(3);
}

/************************************************************************
 ********************              MAIN              ********************
 ************************************************************************/
int
main(int argc, char *argv[])
{
    int		i, code;
    res_state	statep;
    struct __res_state		state;
    union res_sockaddr_union	set[16];

    progname = argv[0];
    for (i = strlen(progname) - 1; i >= 0 && progname[i] != '/'; --i);
    progname += (i + 1);

    statep = &state;
    if ((code = res_ninit(statep)) != 0)
	errout("res_ninit() returned %d", code);

    code = res_getservers(statep, set, 16);
    printf("res_getservers returned %d\n", code);

    exit(0);
}
