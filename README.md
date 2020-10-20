# libsrvcli
A barebones library for multi-threaded client server interactions.

## Dependencies
- g++
- openssl
- pthread

## Build

```shell
make
```

## Using libsrvcli 

### Define connection configuratoin for server

```c++
ConnectionConfig conConfig(
    ip,
    port,
    protocol
);
```

### Starting a server

```c++
void handleRequest(int sockfd) 
{
	// connection accepted from client 
    // handle incoming requests (e.g. read, write, send , recv, etc.)

    //...
    recv(sockfd, ...);
    //...
}

int main () 
{
	Server* srv = new Server(&handleRequest, conConfig);
	srv->start(); // This is not a blocking call

	// To prevent program from exiting by making a blocing call of some sort.
	// Otherwise, the server threads will exit.
	// Handle program exit with manual input or by using signals/interrupts.
	while(true);
}
```

### Connecting to a server

``` c++
void makeRequest(int sockfd) 
{
	// connection accepted by server
    // make requests (e.g. read, write, send , recv, etc.)

	//...
    send(sockfd, ...);
    //...
}

int main()
{
	Client* cli = new Client(&makeRequests, conConfig);
	if (cli->establishConnection() == -1)
    	cout << "Error in connecting to node server" << endl;
}
```
