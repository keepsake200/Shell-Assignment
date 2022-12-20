a.out:
	gcc msh.c

clean:
	rm ./a.out

test: a.out
	 ./test.sh