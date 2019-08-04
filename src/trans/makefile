
TARGET=trans

CPPSOURCE=main.cpp do_trans.cpp front_desk.cpp
CSOURCE=


INCFLAGS?=-I ../include

STATIC_LIBS=../lib/libttools.a
DYNAMIC_LIBS= -lpthread

LIBS=$(STATIC_LIBS) $(DYNAMIC_LIBS)

CPPFLAGS+=$(INCFLAGS)
CFLAGS+=$(INCFLAGS)

COBJS=$(CSOURCE:.c=.o)
CPPOBJS=$(CPPSOURCE:.cpp=.o)
OBJS=$(COBJS) $(CPPOBJS)

all:$(TARGET)

$(COBJS): %.o: %.c
	gcc $(CFLAGS) -c $< -o $@	

$(CPPOBJS): %.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@	

 
$(TARGET): $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)

.PHONY:clean

clean: 
	rm $(OBJS) $(TARGET)

