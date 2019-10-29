CC	=	gcc
CFLAGS	=	-g

TARGET1	=	deneme
TARGET2	=	deneme2

OBJS1	=	deneme.o
OBJS2	=	deneme2.o

all:	deneme	deneme2

$(TARGET1):	$(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) -lpthread

$(TARGET2):	$(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2) -lpthread

$(OBJS1):	deneme.c shmemsem.h
	$(CC) $(CFLAGS) -c deneme.c shmemsem.h

$(OBJS2):	deneme2.c shmemsem.h
	$(CC) $(CFLAGS) -c deneme2.c shmemsem.h

clean:	
		/bin/rm	-f *.o $(TARGET1) $(TARGET2)



