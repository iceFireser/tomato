
TARGET=libtomato.a

CSOURCE=string_ex.c error_ex.c map_ex.c queue_ex.c systool.c loop.c log.c json.c  threadm.c \
	sha1.c tcps.c

CPPSOURCE=

all:$(TARGET)


INCFLAGS?=-I ../include
COMFLAG=-g -Wall -Werror

STATIC_LIBS=
DYNAMIC_LIBS=

LIBS=$(STATIC_LIBS) $(DYNAMIC_LIBS)

CPPFLAGS+=$(INCFLAGS) $(COMFLAG)
CFLAGS+=$(INCFLAGS) $(COMFLAG)

COBJS=$(CSOURCE:.c=.o)
CPPOBJS=$(CPPSOURCE:.cpp=.o)
OBJS=$(COBJS) $(CPPOBJS)

$(COBJS): %.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(CPPOBJS): %.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@


$(TARGET): $(OBJS)
	 ar -rc -o $@ $(OBJS) $(LIBS)

.PHONY:clean

clean:
	rm -f $(OBJS) $(TARGET)

