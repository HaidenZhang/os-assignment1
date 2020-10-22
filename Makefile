CC = gcc
CFLAGS = -Wall
RT = -lrt
POSIXT = -lpthread
MATH = -lm
RM = rm 

all:
	$(CC) $(CFLAGS) prod.c -o prod $(RT) $(POSIXT) $(MATH)
	$(CC) $(CFLAGS) cons.c -o cons $(RT) $(POSIXT) $(MATH)
	$(CC) $(CFLAGS) dph.c -o dph $(RT) $(POSIXT)

prod:
	$(CC) $(CFLAGS) prod.c -o prod $(RT) $(POSIXT) $(MATH)

cons:
	$(CC) $(CFLAGS) cons.c -o cons $(RT) $(POSIXT) $(MATH)

dph:
	$(CC) $(CFLAGS) dph.c -o dph $(RT) $(POSIXT)
    
clean:
	$(RM) -rf prod
	$(RM) -rf cons
	$(RM) -rf dph