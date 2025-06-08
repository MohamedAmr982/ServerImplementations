# ServerImplementations
# Introduction
The aim of this project is to compare two server implementations:
## 1. Multi-threaded server 
* A single thread is spawned for each new connection.
* The thread is joined after the connection is closed.
## 2. Multi-process server
* Multiple processes run simultaneously (each on a different port).
* Requests are managed with non-blocking I/O, using Linux ```epoll``` events.
* Each process spawns two threads: an acceptor thread and a worker thread.
* The acceptor threads waits for new connections, accepts them, then registers them in the an interest list (managed by the OS).
* The worker thread is notified by the OS of the ready sockets. It then handles each request sequentially (per process).
## Notes
Both implementations:
*  Support HTTP GET requests only.
* Utilize *Zero-Copy* ```sendfile()``` system call to avoid unnecessary copying to user space while sending files to the client.
* Use ```p_thread``` library for spawning threads.

# How to run
## Requirements
* Linux environment.
* CMake >= 3.16 to generate makefiles.
* (Optional) Python to run the test script under ```test/```.
1. Clone the repository then ```cd``` to ```ServerImplementations``` directory.
2. Add a build directory:
	```bash
	mkdir build
	cd build
	```
3. Inside the build directory, run CMake:
	```bash
	cmake -G "Unix Makefiles" ..
	```
	> Note: the option ```-DCMAKE_BUILD_TYPE=Debug``` can be used to enable debugging the executable.
	
4. Run make:
	```bash
	make
	```
	This creates an executable called server.
5. Run the server without options to display the required arguments:
	```bash
	./server
	```
	Output:
	```
	Usage: server <queue_len> <port> <data_directory> <mode>
	mode = 0: single threaded server.
	mode = 1: 1 process/port.
	mode = 2: 1 thread/request.
	```
	 An example usage (executing from ```build``` directory):
	 ```bash
	 ./server 1000 8080 ../docs 2
	 ```
	 This runs a multi-threaded server on port 8080. The directory ../docs is included in the repository (from which the server looks for and fetches requested files).
# Comparison
Both implementations were tested as follows:
* Multi-threaded implementation: 10 client processes were spawned to send N requests then exit. The requests included one file chosen randomly, with uniform probability, from the ```docs/``` directory, in addition to a non-existing file to test 404 response.
* Multi-process implementation: same as above but the processes were allowed to choose one of the running servers randomly (with uniform probability) each time a request is sent.

## Results
> Tested on a 16-core machine.

* Multi-threaded server:

|N| Average response time (ms) |
|--|--|
| 10 | 2.14570431 |
|100|1.64499062|
|1000|1.7278872275|
|10000|1.73084160237|
|100000|1.746397762359|

* Single process:

|N| Average response time (ms) |
|--|--|
| 10 | 2.05655226 |
|100|1.514470052|
|1000|1.636461071|
|10000|1.65908015472|
|100000|1.694643693065|

* 2 processes:

|N| Average response time (ms) |
|--|--|
| 10 | 1.712241446157 |
|100|1.161660115|
|1000|1.2719023661|
|10000|1.35127155352|
|100000|1.43666886853|

* 3 processes:

|N| Average response time (ms) |
|--|--|
| 10 | 1.7156659 |
|100|1.334765551|
|1000|1.3251647213|
|10000|1.40321279763|
|100000|1.451819735966|

![Comparison results](https://github.com/MohamedAmr982/ServerImplementations/blob/main/comp.png)
