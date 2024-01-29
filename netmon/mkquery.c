#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#include "ping_dns.h"

char		*progname;

/************************************************************************
 ********************             USAGE              ********************
 ************************************************************************/
void
usage(void)
{
    fprintf(stderr, "usage: %s [-h host_ip] [-p port]\n", progname);
    exit(1);
}

/************************************************************************
 ********************              MAIN              ********************
 ************************************************************************/
int
main(int argc, char *argv[])
{
    int		i, ch, port, code;
    char	*host_ip;

    progname = argv[0];
    for (i = strlen(progname) - 1; i >= 0 && progname[i] != '/'; --i);
    progname += (i + 1);

    port = 53;
    host_ip = "8.8.8.8";
    while ((ch = getopt(argc, argv, "h:p:")) != -1)
    {
	switch(ch)
	{
	    case 'h':
		host_ip = optarg;
		break;

	    case 'p':
		port = atoi(optarg);
		break;

	    default:
		usage();
	}
    }

    if ((code = ping_dns(host_ip, port)) != 0)
	fprintf(stderr, "ping_dns returned %d\n", code);

    exit(code);
}
