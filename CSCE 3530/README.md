### CSCE 3530.001 - Programming Assignment #2

**Compile**:  
Run `make` to compile everything. To compile without stdout 
messages on the server, run `make silent`. To compile only the 
server or only the client, run `make pyserver` or 
`make client`. To delete the executables, run `make clean`.
  
**Execute**:  
On `cse01.cse.unt.edu`, run `./pyserver <port_number>`.  
On any cse server, run `./client <port_number>`.
  
**Usage**:
The server will only interact with one client at a time. 
When the client connects, enter any web address up to ~1000 
characters long. The server's response will be printed on 
the client, and the connection will be closed. The client 
process will end, and the server will return to listening 
for the next client.
  
Valid web addresses begin with `www.`, not `http://`. Web
addresses that do not begin with `www.` are not guaranteed
to work.

When the cache is accessed the first time, an error will
be printed since the cache file does not exist. This
is not a problem; everything is handled by the program.  
  
The client will have to be restarted for each web
address sent to the server. This is compliant with
the assignment requirements.
