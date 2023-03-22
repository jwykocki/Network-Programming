DIR = `basename $(CURDIR)`
NAME1 = udp_server
NAME2 = udp_client
NAME3 = functions
NAME4 = header
NAME5 = udp_test

EXEC1 = $(NAME1).x
OBJS1 = $(NAME1).o

EXEC2 = $(NAME2).x
OBJS2 = $(NAME2).o

OBJS3 = $(NAME3).o

HEADS = $(NAME4).h

EXEC5 = $(NAME5).x
OBJS5 = $(NAME5).o


CFLAGS = -Wall -Wpedantic -std=c99 -O 
LFLAGS = -Wall -Wpedantic -std=c99 -O 

CO = gcc
LD = $(CO)

%.o: %.c %.h
	$(CO) $(CFLAGS) $(HEADS) -c $< 

$(OBJS1): $(HEADS)
$(OBJS2): $(HEADS)
$(OBJS3): $(HEADS)
$(OBJS5): $(HEADS)

.PHONY: all
all: $(EXEC1) $(EXEC2) $(EXEC5)

$(EXEC1): $(OBJS1) $(OBJS3) 
	 $(LD) -o $@ $(LFLAGS) $(HEADS) $^ -lrt
$(EXEC2): $(OBJS2) $(OBJS3) 
	 $(LD) -o $@ $(LFLAGS) $(HEADS) $^ -lrt
$(EXEC5): $(OBJS5) $(OBJS3)
	 $(LD) -o $@ $(LFLAGS) $(HEADS) $^ -lrt


.PHONY: udp_s
udp_s: $(EXEC1)
	./$(EXEC1) 2020 

.PHONY: udp_c
udp_c: $(EXEC2)
	./$(EXEC2) 127.0.0.1 2020

.PHONY: udp_t
udp_t: $(EXEC5)
	./$(EXEC5) 127.0.0.1 2020
	od -A d -t u1 -t c < results.txt
	diff -s results.txt expected_results.txt 
	
.PHONY: clean
EXECS = $(EXEC1) $(EXEC2) $(EXEC5) 

clean:                                                     
	rm -f *.o  *~ $(EXECS)

.PHONY: tar
tar: clean
	(cd ../; tar -cvzf $(DIR).tar.gz  $(DIR) )


