CC	=	gcc
CFLAGS	=	-g

TARGET1	=	oss
TARGET2	=	user

OBJS1	=	oss1.o
OBJS2	=	user1.o

all:	oss	user

$(TARGET1):	$(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) -lpthread

$(TARGET2):	$(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2) -lpthread


$(OBJS1):	oss1.c shmemsem.h 
	$(CC) $(CFLAGS) -c oss1.c shmemsem.h

$(OBJS2):	user1.c shmemsem.h
	$(CC) $(CFLAGS) -c user1.c shmemsem.h


clean:	
		/bin/rm	-f *.o $(TARGET1) $(TARGET2) 



