all: hello.mod

%.mod: %.o
	$(LD) -o $@ -r $^
