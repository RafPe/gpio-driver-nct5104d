src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = ""
PREFIX = /usr/local/bin

nct5104dpin: $(obj)
    $(CC) -o $@ $^ $(LDFLAGS)

clean:
    rm -f $(obj) nct5104dpin

install: nct5104dpin
    mkdir -p $(PREFIX)
    cp $< PREFIX$(PREFIX)/nct5104dpin