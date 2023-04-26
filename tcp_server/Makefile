DIR = `basename $(CURDIR)`
NAME1 = tcp_server
NAME2 = tcp_client
NAME3 = functions
NAME4 = header
NAME5 = tcp_test
NAME6 = test_manager

EXEC1 = $(NAME1).x
OBJS1 = $(NAME1).o

EXEC2 = $(NAME2).x
OBJS2 = $(NAME2).o

OBJS3 = $(NAME3).o

HEADS = $(NAME4).h

EXEC5 = $(NAME5).x
OBJS5 = $(NAME5).o

EXEC6 = $(NAME6).x
OBJS6 = $(NAME6).o


CFLAGS = -Wall -g -Wpedantic -std=c99 -O 
LFLAGS = -Wall -Wpedantic -std=c99 -O 

CO = gcc
LD = $(CO)

%.o: %.c %.h
	$(CO) $(CFLAGS) $(HEADS) -c $< 

$(OBJS1): $(HEADS)
$(OBJS2): $(HEADS)
$(OBJS3): $(HEADS)
$(OBJS5): $(HEADS)
$(OBJS6): $(HEADS)

.PHONY: all
all: $(EXEC1) $(EXEC2) $(EXEC5) $(EXEC6)

$(EXEC1): $(OBJS3) $(OBJS1) 
	 $(LD) -o $@ $(LFLAGS) $(HEADS)  $^ -lrt
$(EXEC2): $(OBJS2) $(OBJS3) 
	 $(LD) -o $@ $(LFLAGS) $(HEADS) $^ -lrt
$(EXEC5): $(OBJS5) $(OBJS3)
	 $(LD) -o $@ $(LFLAGS) $(HEADS) $^ -lrt
$(EXEC6): $(OBJS6)  $(OBJS3)
	 $(LD) -o $@ $(LFLAGS) $(HEADS) $^ -lrt


.PHONY: tcp_s
tcp_s: $(EXEC1)
	./$(EXEC1)

.PHONY: tcp_c
tcp_c: $(EXEC2)
	./$(EXEC2)

.PHONY: tcp_t
tcp_t: $(EXEC5)
	./$(EXEC5)

.PHONY: tests
tests: $(EXEC6) $(EXEC5)
	./$(EXEC6) $(n)
	
.PHONY: clean
EXECS = $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4) $(EXEC5) $(EXEC6)

clean:                                                     
	rm -f *.o  *~ $(EXECS)

.PHONY: tar
tar: clean
	(cd ../; tar -cvzf $(DIR).tar.gz  $(DIR) )


