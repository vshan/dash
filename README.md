# dash
Distributed access shell (or dash) is a new way to interconnect processes for distributed command execution, redirection and remote piping between multiple hosts by combining the two fundamental UNIX abstractions: sockets and pipes using C, C++, Make. 
It allows you to run shell commands on multiple servers at once and gather see their output in local terminal.

Distributed access shell example command : 
host1:bin1 | host2:bin2 >host3
