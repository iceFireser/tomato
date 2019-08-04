
TARGET=libttools.a

CPPSOURCE=
CSOURCE=systool.c

all:$(TARGET)


INCFLAGS?=-I ../include

STATIC_LIBS=
DYNAMIC_LIBS=

LIBS=$(STATIC_LIBS) $(DYNAMIC_LIBS)

CPPFLAGS+=$(INCFLAGS)
CFLAGS+=$(INCFLAGS)

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
	rm $(OBJS) $(TARGET)

