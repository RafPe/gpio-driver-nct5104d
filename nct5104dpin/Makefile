src = $(wildcard *.c)

PREFIX = /usr/local/bin

nct5104dpin: $(src)
        gcc -Wall -o $@ $^

clean:
        rm -f $(obj) nct5104dpin

install: nct5104dpin
        mkdir -p $(PREFIX)
        cp $< $(PREFIX)/nct5104dpin