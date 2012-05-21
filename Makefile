SRC = $(shell find src/*.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -std=c99
PREFIX = /usr/local

imgsize: $(OBJ)
	$(CC) $^ -o $@

install: imgsize
	cp -f $< $(PREFIX)/bin/$<

uninstall:
	rm -f $(PREFIX)/bin/imgsize

test:
	@echo PNG
	@./imgsize test/firefox.jpg
	@echo
	@echo GIF
	@./imgsize test/wordpress.gif
	@echo
	@echo JPEG
	@./imgsize test/chrome.jpg

clean:
	rm -f imgsize
	rm -rf src/*.o

rebuild: clean imgsize

.PHONY: clean test install uninstall