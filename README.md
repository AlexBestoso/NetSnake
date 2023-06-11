# NetSnake
C++ class for creating network-enabled applications.
<h2>How does this work?</h2>
<p>
This code supports socket client and server creation. Currently it just supports INET clients and servers.<br>
Just run the respective setup functions and then start sending and receiving data!<br>
This repo also contains a unit test program that you can use to ensure that the socket communication works.<br>
The application creates a server, forks, then creates a client to test communication all in the same window.
</p>

## this code supports the following:
<ul>
<li>AF INET client and server in SOCK STREAM</li>
<li>AF UNIX client and server in SOCK STREAM</li>
</ul>

### Things that will happen in the future
Going to refactore the code to be more clean.
