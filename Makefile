TARGET:=gtk-fmle

SRC=$(wildcard *.c)
OBJ=gtk-fmle.o conf.o
PREFIX=/usr

all: gtk-fmle-glade.h $(TARGET) $(TARGET).mo

clean:
	rm -f $(TARGET) *.o gtk-fmle-glade.h rcconv $(TARGET).mo

install:
	cp $(TARGET) $(PREFIX)/bin
	cp $(TARGET).mo $(PREFIX)/share/locale/zh_TW/LC_MESSAGES

%.o: %.c
	@echo "  CC	$<"
	@$(CC) -c $< -o $@  $(CFLAGS) $(shell pkg-config --cflags gtk+-2.0) -Wall

rcconv: rcconv.o
	@echo "  LD	$@"
	@$(CC) $^ -o $@

gtk-fmle-glade.h: gtk-fmle.glade rcconv
	@echo "  rcconv	$@"
	@./rcconv $< gtk_fmle_glade > $@

pot:
	+TARGET=$(TARGET) make -C po

$(TARGET).mo: po/zh_TW.po
	@echo "  msgfmt $@"
	@msgfmt $< -o $@

$(TARGET): $(OBJ)
	@echo "  LD	$@"
	@$(CC) $^ -o $@ $(shell pkg-config --libs gtk+-2.0) -export-dynamic

