CC=g++ 
CFLAGS=-fPIC -c
LDFLAGS=-Iinclude -lpthread

libsrvcli.so: server.o client.o
	$(CC) -shared -o libsrvcli.so server.o client.o $(LDFLAGS)

server.o: Server.cpp
	$(CC) $(CFLAGS) Server.cpp -o server.o $(LDFLAGS)

client.o: Client.cpp
	$(CC) $(CFLAGS) Client.cpp -o client.o $(LDFLAGS)

clean: 
	rm -f libsrvcli.so server.o client.o
