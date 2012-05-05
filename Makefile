TARGET:=gtk-fmle

SRC=$(wildcard *.c)
OBJ=gtk-fmle.o conf.o
PREFIX=/usr

all: gtk-fmle-glade.h $(TARGET) $(TARGET).mo

clean:
	rm -f $(TARGET) *.o gtk-fmle-glade.h $(TARGET).mo

install:
	cp $(TARGET) $(PREFIX)/bin
	cp $(TARGET).mo $(PREFIX)/share/locale/zh_TW/LC_MESSAGES

%.o: %.c
	@echo "  CC	$<"
	@$(CC) -c $< -o $@  $(CFLAGS) $(shell pkg-config --cflags gtk+-2.0) -Wall

gtk-fmle-glade.h: gtk-fmle.glade
	@echo "  xxd	$@"
	@xxd -i $< $@

pot:
	+TARGET=$(TARGET) make -C po

$(TARGET).mo: po/zh_TW.po
	@echo "  msgfmt $@"
	@msgfmt $< -o $@

$(TARGET): $(OBJ)
	@echo "  LD	$@"
	@$(CC) $^ -o $@ $(shell pkg-config --libs gtk+-2.0) -export-dynamic

