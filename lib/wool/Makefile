CC = gcc

ifdef CCOMP
  CC = ${CCOMP}
endif

# ifndef SOLARIS
#   threadsflag = -pthread 
# else
  threadsflag = -lpthread
# endif

buildparams = 
CFLAGS = -g -O3 -Wall $(buildparams)
LDFLAGS = $(threadsflag)
examples = fib stress loop2 mm1 mm2 mm3 mm4 memstress mm5 mm6 mm7
MAX_ARITY = 10

ifdef TASK_PAYLOAD
  buildparams += -DTASK_PAYLOAD=$(TASK_PAYLOAD)
endif

ifdef FINEST_GRAIN
  buildparams += -DFINEST_GRAIN=$(FINEST_GRAIN)
endif

ifdef COUNT_EVENTS
  buildparams += -DCOUNT_EVENTS=$(COUNT_EVENTS)
endif

ifdef THE_SYNC
  buildparams += -DTHE_SYNC=$(THE_SYNC)
endif

ifdef LOG_EVENTS
  buildparams += -DLOG_EVENTS=$(LOG_EVENTS)
  LDFLAGS += -lrt
endif

all : $(examples)

fib    : fib.o wool.o
mm1    : mm1.o wool.o
mm2    : mm2.o wool.o
mm3    : mm3.o wool.o
mm4    : mm4.o wool.o
mm5    : mm5.o wool.o
mm6    : mm6.o wool.o
mm7    : mm7.o wool.o
stress : stress.o wool.o loop.o
loop2  : loop2.o wool.o loop.o
memstress : memstress.o wool.o reads.o

loop.o : loop.c
	gcc -O -c loop.c
reads.o : reads.c
	gcc -O -c reads.c

wool.h : wool.sh
	./wool.sh $(MAX_ARITY) > wool.h

wool.o : wool.h

fib.o    : wool.h
mm1.o    : wool.h 
mm2.o    : wool.h 
mm3.o    : wool.h 
mm4.o    : wool.h 
mm5.o    : wool.h
mm6.o    : wool.h 
mm7.o    : wool.h 
stress.o : wool.h 
loop2.o  : wool.h 
memstress.o : wool.h


clean : 
	rm -f wool.h *.o $(examples)

