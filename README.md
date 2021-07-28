# Socket-Programming-to-handle-HTTP-requests-from-multiple-clients
Introduction

Socket programming plays an important role whenever there is a need to communicate between the client and the server. We learned Socket network programming in C and understood HTTP and web server operation through this lab. In our program, the web server can handle multiple incoming HTTP GET requests, parse the requests and send responses back to the client, such as return a HTML file or NOT FOUND Error 404. The client can establish a TCP connection with the web server or send a request over UDP.

Architecture

The architecture of our model consists of a single web server and multiple clients. When we use TCP, each client is connected to the web server through a socket.When we use UDP, there is no connection between the client and the web server.

Working

Initially, the web server is started and it is ready to accept new clients. The client then chooses what type of protocol to use and then sends a request to the server. The server then responds to the client by sending the required webpage and if the page is not found it returns a page saying 404: page not found.

Running the program.

Compile: 

**make**

Using brown.cs.unm.edu (IP 64.106.20.206) as an example:

**./webserver 8080 TCP**

You may run the client on other machine:

**./client http://64.106.20.206:8080/lab2.html TCP**

To test the UDP version, you may replace TCP with UDP

Test

We did experiments by remote-login the lab computers. We set up the web server at the computer named Brown and set up the clients at the computer named Setebos. We used shell to simulate 100,1000,2000,3000,5000,7000 and 10000 clients requesting the same web page from the server and measured the latency of requests. We ran the tests and measured the average latency from the time client sends the request to the time the client receive using TCP implementation.Then we repeat it using UDP.

The latency graph over TCP of our test results as follow:
![The latency graph over TCP](https://user-images.githubusercontent.com/33234287/127382284-9f99def5-9ad0-46ae-9947-51043a5da857.png)

The latency graph over UDP of our test results as follow:
![The latency graph over UDP](https://user-images.githubusercontent.com/33234287/127382317-219fc16c-db14-448b-a3ff-21d368581d38.png)

<img width="601" alt="The loss in packets can be observed in UDP" src="https://user-images.githubusercontent.com/33234287/127382345-8d9a0594-eeeb-40e8-9157-f5a8b82384d2.png">
The loss in packets can be observed from the other graph while using udp. There`s barely any packet loss while running 10 and 100 clients but for 10,000 clients we can observe a significant loss.


