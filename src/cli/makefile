
TARGET=cli

CPPSOURCE=main.cpp t_connect.cpp
CSOURCE=

all:$(TARGET)


INCFLAGS?=-I ../include
COMFLAGS?=-g

STATIC_LIBS=../lib/libttools.a
DYNAMIC_LIBS=

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
	rm $(OBJS) $(TARGET)

