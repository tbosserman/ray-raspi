#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>

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
    int		i;
    char	printable[64];
    sa_family_t	family;
    struct ifaddrs	*ifaddrs, *ifp;
    struct sockaddr_in	*addrp;
    struct sockaddr_ll	*ll;

    progname = argv[0];
    for (i = strlen(progname) - 1; i >= 0 && progname[i] != '/'; --i);
    progname += (i + 1);

    if (getifaddrs(&ifaddrs) < 0)
	errout("getifaddrs failed");

    for (ifp = ifaddrs; ifp != NULL; ifp = ifp->ifa_next)
    {
	family = ifp->ifa_addr->sa_family;
	addrp = (struct sockaddr_in *)ifp->ifa_addr;
#ifdef BLAP
	if (strncmp(ifp->ifa_name, "lo", 2) == 0)
	    continue;
	if (addrp->sin_family != AF_INET)
	    continue;
#endif

	printf("%10.10s  %3d  ", ifp->ifa_name, family);

	if (family == AF_PACKET)
	    ll = (struct sockaddr_ll *)ifp->ifa_addr;

	printable[0] = '\0';
	if (family == AF_INET)
	    (void)inet_ntop(AF_INET, &addrp->sin_addr, printable,
		sizeof(printable));
	printf("%s\n", printable);
    }

    exit(0);
}
