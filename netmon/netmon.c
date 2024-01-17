#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

extern int	alltrim(char *s);

struct s_info {
    char	interface[16];
    char	gateway[16];
    char	ipaddr[16];
    int		dns_ok;
    int		all_ok;
};
typedef struct s_info t_info;

t_info		netmon_info[2];
int		cur_info_index = -1;

int
default_route(t_info *info)
{
    FILE	*fp;
    char	line[256];

    if ((fp = popen("/bin/ip -4 -o route show default", "r")) == NULL)
	return(-1);
    line[sizeof(line)-1] = '\0';
    if (fgets(line, sizeof(line)-1, fp) == NULL || strlen(line) == 0)
    {
	fclose(fp);
	return(-1);
    }
    // default via 192.168.100.1 dev eth0 proto dhcp src 192.168.100.174 metric 100
    (void)strtok(line, " \t"); /* the word "default" */
    (void)strtok(NULL, " \t"); /* the word "via" */
    strncpy(info->gateway, strtok(NULL, " \t"), sizeof(info->gateway)-1);
    (void)strtok(NULL, " \t"); /* the word "dev" */
    strncpy(info->interface, strtok(NULL, " \t"), sizeof(info->interface)-1);

    fclose(fp);
    return(0);
}

int
my_ipaddr(t_info *info)
{
    FILE	*fp;
    char	cmd[256], line[256], *p;

    cmd[sizeof(cmd)-1] = '\0';
    snprintf(cmd, sizeof(cmd)-1, "/bin/ip -4 -br addr show dev %s",
	info->interface);
    if ((fp = popen(cmd, "r")) == NULL)
	return(-1);

    line[sizeof(line)-1] = '\0';
    p = fgets(line, sizeof(line)-1, fp);
    fclose(fp);
    if (p == NULL || strlen(line) < 2)
	return(-1);
    line[strlen(line)-1] = '\0';
    alltrim(line);

    // wlan0            UP             192.168.100.175/24 
    if ((p = strrchr(line, ' ')) == NULL)
	return(-1);
    strncpy(info->ipaddr, p+1, sizeof(info->ipaddr)-1);

    return(0);
}

int
check_dns(t_info *info)
{
    int			code;
    struct addrinfo	hints, *ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = PF_UNSPEC;
    code = getaddrinfo("google.com", NULL, &hints, &ai);
    info->dns_ok = (code == 0);
    return(code);
}

void
all_tests(t_info *info)
{
    memset(info, 0, sizeof(t_info));
    if (default_route(info) == 0)
	(void)my_ipaddr(info);
    (void)check_dns(info);
    info->all_ok = (info->interface[0] != '\0' && info->gateway[0] != '\0' &&
		    info->ipaddr[0] != '\0' && info->dns_ok);
}

int
netmon(void)
{
    int		cur, prev;
    t_info	*curp, *prevp;

    prev = cur_info_index;
    cur = (prev < 0) ? 0 : !prev;
    curp = &netmon_info[cur];
    prevp = (prev < 0) ? NULL : &netmon_info[prev];
    cur_info_index = cur;
    all_tests(curp);

    if (curp->all_ok)
    {
	if (prevp && !prevp->all_ok)
	    printf("Networking OK\n\n");
	return(TRUE);
    }

    if (curp->gateway[0] == '\0')
    {
	if (prevp == NULL || strcmp(curp->gateway, prevp->gateway))
	    printf("No default route\n");
    }
    else if (curp->ipaddr[0] == '\0')
    {
	if (prevp == NULL || strcmp(curp->ipaddr, prevp->ipaddr))
	    printf("No IP address assigned\n");
    }
    if (!curp->dns_ok)
	if (prevp == NULL || curp->dns_ok != prevp->dns_ok)
	    printf("DNS not working\n");

    return(TRUE);
}

int
main(int argc, char *argv[])
{
    int		code;

    for (;;)
    {
	netmon();
	sleep(1);
    }

    exit(0);
}
