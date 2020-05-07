major2: shell.o Redirect.o Path.o CD.o alias.o myhistory.o helpers.o piping.o
	gcc -o $@ $^

shell.o: shell.c shell.h
	gcc -c -Wall shell.c

Redirect.o: Redirect.c
	gcc -c -Wall Redirect.c

Path.o: Path.c
	gcc -c -Wall Path.c

CD.o: CD.c
	gcc -c -Wall CD.c

alias.o: helpers.o
	gcc -c -Wall alias.c

myhistory.o: myhistory.c
	gcc -c -Wall myhistory.c

helpers.o: helpers.c helpers.h
	gcc -c -Wall helpers.c

piping.o: piping.c piping.h
	gcc -c -Wall piping.c

.PHONY: clean
clean:
	rm *.o
