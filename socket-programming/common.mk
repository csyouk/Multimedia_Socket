CC = arm-none-linux-gnueabi-gcc 
CFLAGS = -c

TARGETS = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
all: $(TARGETS)

ifdef TARGET1
TARGET1_OBJS = $(TARGET1).o
$(TARGET1): $(TARGET1_OBJS)
	$(CC) $(LFLAGS) -o $@ $(TARGET1_OBJS)
	sudo cp $(TARGET1) /nfsroot
endif

ifdef TARGET2
TARGET2_OBJS = $(TARGET2).o
$(TARGET2): $(TARGET2_OBJS)
	$(CC) $(LFLAGS) -o $@ $(TARGET2_OBJS)
	sudo cp $(TARGET2) /nfsroot
endif

ifdef TARGET3
TARGET3_OBJS = $(TARGET3).o
$(TARGET3): $(TARGET3_OBJS)
	$(CC) $(LFLAGS) -o $@ $(TARGET3_OBJS)
	sudo cp $(TARGET3) /nfsroot
endif

ifdef TARGET4
TARGET4_OBJS = $(TARGET4).o
$(TARGET4): $(TARGET4_OBJS)
	$(CC) $(LFLAGS) -o $@ $(TARGET4_OBJS)
	sudo cp $(TARGET4) /nfsroot
endif

%.o:%.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(TARGETS) *.o

