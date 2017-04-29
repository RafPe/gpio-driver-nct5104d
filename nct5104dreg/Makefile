src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = ""
PREFIX = /usr/local/bin

nct5104dreg: $(obj)
    $(CC) -o $@ $^ $(LDFLAGS)

clean:
    rm -f $(obj) nct5104dreg

install: nct5104dreg
    mkdir -p $(PREFIX)
    cp $< PREFIX$(PREFIX)/nct5104dreg