TARGET:=gtk-fmle

SRC=$(wildcard *.c)
OBJ=$(subst .c,.o,$(SRC))
PREFIX=/usr



all: $(TARGET)

clean:
	rm $(TARGET) *.o

install:
	cp $(TARGET) $(PREFIX)/bin
	cp $(TARGET).mo $(PREFIX)/share/locale/zh_TW/LC_MESSAGES

%.o: %.c
	@echo "  CC	$<"
	@$(CC) -c $< -o $@  $(CFLAGS) $(shell pkg-config --cflags gtk+-2.0) -Wall
	
$(TARGET): $(OBJ)
	@echo "  LD	$@"
	@$(CC) $^ -o $@ $(shell pkg-config --libs gtk+-2.0) -export-dynamic
