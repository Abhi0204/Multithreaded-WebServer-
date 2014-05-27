CC=g++

ALL:
	$(CC) -o myhttpd myhttpd.cpp -pthread

clean:
	rm myhttpd
