#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <arpa/inet.h>

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
    int		i, dest, gw;
    FILE	*fp;
    char	line[1024], iface[32], pr1[16], pr2[16];
    struct in_addr	addr;

    progname = argv[0];
    for (i = strlen(progname) - 1; i >= 0 && progname[i] != '/'; --i);
    progname += (i + 1);

    if ((fp = fopen("/proc/net/route", "r")) == NULL)
	errout("error opening /proc/net/route for reading");

    fgets(line, sizeof(line), fp); /* Skip the header line */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
	sscanf(line, "%s %X %X ", iface, &dest, &gw);
	addr.s_addr = dest;
	inet_ntop(AF_INET, &addr, pr1, 16);
	addr.s_addr = gw;
	inet_ntop(AF_INET, &addr, pr2, 16);
	/*printf("%-4s  %08X  %08X\n", iface, dest, gw); */
	printf("%-4s  %-16.16s  %-16.16s\n", iface, pr1, pr2);
    }

    if (ferror(fp))
	errout("error reading from /proc/net/route");
    fclose(fp);

    exit(0);
}
