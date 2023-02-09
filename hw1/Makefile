# $Id: Makefile,v 1.18 2023/01/24 13:42:50 leavens Exp leavens $
# Makefile for P-machine

CC = gcc
CFLAGS = -g -std=c17 -Wall
RM = rm -f
ZIPFILE = ~/WWW/COP3402/homeworks/hw1-tests.zip

vm: *.c *.h
	$(CC) $(CFLAGS) -o vm `cat sources.txt`

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

sources.txt:
	echo *.c >sources.txt

clean:
	$(RM) *~ *.o *.out2 '#'*
	$(RM) vm.exe
	$(RM) *.stackdump

# make file.out from file.txt by running the VM
%.out: %.txt vm
	./vm $< > $@ 2>&1

%.out2: %.txt vm
	./vm $< > $@ 2>&1

check-outputs: vm hw1-test*.txt
	DIFFS=0; \
	for f in `echo hw1-test*.txt | sed -e 's/\\.txt//g'`; \
	do \
		echo running "$$f.txt"; \
		./vm "$$f.txt" >"$$f.out2" 2>&1; \
		diff -w "$$f.out2" "$$f.out" && echo 'passed!' || DIFFS=1; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All tests passed!'; \
	else \
		echo 'Test(s) failed!'; \
	fi

create-outputs: vm hw1-test*.txt
	for f in `echo hw1-test*.txt | sed -e 's/\\.txt//g'`; \
	do \
		echo running "$$f.txt"; \
		./vm "$$f.txt" >"$$f.out" 2>&1; \
	done

zip hw1-tests.zip: hw1-test*.txt Makefile
	rm -f $(ZIPFILE)
	chmod 755 hw1-test*.txt hw1-test*.out Makefile
	zip -9 $(ZIPFILE) hw1-test*.txt hw1-test*.out Makefile
