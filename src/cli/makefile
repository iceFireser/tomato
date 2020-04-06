
TARGET=cli

CPPSOURCE=
CSOURCE=main.c


all:$(TARGET)


INCFLAGS?=-I ../include -I../
COMFLAGS?=-g

STATIC_LIBS=../lib/libtomato.a
DYNAMIC_LIBS= -lpthread

LIBS=$(STATIC_LIBS) $(DYNAMIC_LIBS)

CPPFLAGS+=$(INCFLAGS) $(COMFLAGS)
CFLAGS+=$(INCFLAGS) $(COMFLAGS)

COBJS=$(CSOURCE:.c=.o)
CPPOBJS=$(CPPSOURCE:.cpp=.o)
OBJS=$(COBJS) $(CPPOBJS)

$(COBJS): %.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(CPPOBJS): %.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@


$(TARGET): $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)

.PHONY:clean

clean:
	rm -f $(OBJS) $(TARGET)

