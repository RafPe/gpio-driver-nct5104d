src = $(wildcard *.c)

PREFIX = /usr/local/bin

nct5104dreg: $(src)
	gcc -Wall -o $@ $^

clean:
	rm -f $(obj) nct5104dreg

install: nct5104dreg
	mkdir -p $(PREFIX)
	cp $< $(PREFIX)/nct5104dreg
