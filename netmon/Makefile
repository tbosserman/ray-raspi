CFLAGS=-g

all: netmon mkquery

netmon: netmon.o alltrim.o

mkquery: mkquery.o ping_dns.o

clean:
	$(RM) *.o netmon mkquery
