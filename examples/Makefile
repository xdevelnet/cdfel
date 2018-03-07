all:
	rm -f printf; cc --std=c99 -O2 printf.c -o printf -Wall -Wextra -Wno-unused-parameter
andrun:
	rm -f printf; if cc --std=c99 -O2 printf.c -o printf -Wall -Wextra -Wno-unused-parameter; then printf "\n\n"; ./printf; fi
clean:
	rm -f printf;
