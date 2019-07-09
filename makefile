P1 	:= os
OBJECTS	= simulation.o pcb.o ready_queue.o device_node.o \
	 device_queue.o sysgen.o cpu.o system_calls.o interrupts.o \
	 print_utilities.o user_input_utilities.o disk_queue.o job_queue.o
CFLAGS 	= -g -std=gnu11
CC	= gcc
LDLIBS 	= -lm 

$(P1): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

simulation.o: sysgen.h interrupts.h system_calls.h 
	$(CC) $(CFLAGS) -c -o $@ $*.c
pcb.o: pcb.h sysgen.h 
	$(CC) $(CFLAGS) -c -o $@ $*.c
ready_queue.o: pcb.h ready_queue.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
device_node.o: device_node.h pcb.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
device_queue.o: device_queue.h device_node.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
sysgen.o: sysgen.h device_queue.h ready_queue.h cpu.h disk_queue.h 
	$(CC) $(CFLAGS) -c -o $@ $*.c
disk_queue.o: disk_queue.h device_node.h 
	$(CC) $(CFLAGS) -c -o $@ $*.c 
cpu.o: cpu.h pcb.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
system_calls.o: system_calls.h sysgen.h user_input_utilities.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
interrupts.o: interrupts.h sysgen.h 
	$(CC) $(CFLAGS) -c -o $@ $*.c
print_utilities.o: print_utilities.h sysgen.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
user_input_utilities.o: user_input_utilities.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
job_queue.o: pcb.h job_queue.h
	$(CC) $(CFLAGS) -c -o $@ $*.c
clean:
	rm $(OBJECTS) $(P1)
