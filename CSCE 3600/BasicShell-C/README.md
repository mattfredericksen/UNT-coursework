### Names:  
Alejandro Munoz
- [Path](Path.c) command
- Signal Handling
- Terminal Control

Matthew Fredericksen
- [myhistory](myhistory.c) command
- [alias](alias.c) command

David Bugbee 
- [CD](CD.c) command
- Extending shell with I/O [redirection](Redirect.c)

Parker Brown
- exit command
- Extending shell with [pipelining](piping.c) 

### Organization of Project: 
The team consisted of 4 members as outlined above, where each person would do the part assigned to them. Since no one cared about what part they did (as long as everyone did a part), parts were assigned at random. Git Lab was used for the repository, and meetings were carried out via Discord on a weekly basis. The meetings were used to discuss coding and implementation issues, in addition to the development of main (everyone had to integrate their own part into it) and other necessities, like the README and the makefile. This setup was adopted since the team was unable to phyiscally meet due to external circumstances. Regardless, it proved effective and allowed everyone on the team to write and integrate their code into the shell.

### Design Overview: 
David 
- Responsible for creating the CD command and implementing redirection. 
- One function was created for the CD command, and it takes in a path as its parameter. If that parameter is set to null, the function sets the path to the users home directory. Afterwards, the function uses chdir to attempt to change directories. If the change is unsuccessful (in the event the path doesn't exist), a message will be printed to stderr to let the user know. In main, this function is called if cd is seen as the first passed in argument to the shell. At that point, the total number of arguments is checked, which will result in CD being called with parameter NULL if there are no other arguments, CD being called with the next parameter if there is one other argument, and an error message being displayed if there are 2 or more arguments, as CD only has one parameter. 
- For redirection, 3 functions were created: one for redirecting in, one for redirecting out, and one for parsing a command. The parsing command is called from main once a '<' or '>' is detected in a command, which then determines what happens next. The parse function takes in the command (an array of charater pointers) as its parameter. Once called, the function will look for the redirect character and make sure there is an argument after it. Once found, the process will fork, and the child will call the appropriate redirect function: redirect In (redIn) for '<', and redirect Out (redOut) for '>'. Redirect In and Redirect Out are very similar: both take in a cstring as a file name, at which point that file is opened. If the command is able to open the file, it is duplicated to stdin or stdout respectively. Otherwise it returns an error. Assuming no error is returned from the redirect functions, the parse function sets the last 2 arguments in the passed in command to null, and then uses execvp to execute the command. Afterwards, the child exits, preventing errors if the system call fails. Meanwhile, the parent function waits for the child to finish, and then exits. If no other argument is passed in after the redirection character, the parsing function gives the user an error, telling them the file there is no file to redirect to.
	
Alejandro
- Responsible for creating the path command and also implemeting Signal handling. 
- All the function for my parts were created in the Path.c file. For starters I had 3 functions that were designated just for Path, these were append_path, remove_path, and display. The display function would just print out the path for the assignemt. The append_path would add a string to the end of the path, this was done by passing in a string to the function as a parameter, this string would then be used and added to the end of the path using strcat and then from there we would do the setenv command passing "PATH" and also path global character array. The remove_path would pass also a string and instead would remove the user input from the path. Once these features were done I then create a Paths function which would be the function to be used in our shell, this function would do the function described based on the input by the user  as stated by the instructions. 
- For the signal handling, I had created 3 functions a function that would ignore the signals, one that would give signals back their usage and also a signal function. The ignore signals were called in the shell so that the program would not stop no matter if a signal was passed, unfortunately I ran into issues when creating the terminal foregound causing the program to go boom and could not figure out what was going on, the portion of that code was commented out to avoid issues.

Matthew
- Responsible for myhistory and alias.
- [alias.c](alias.c) defines [`alias_get()`](alias.c#L172), which accepts a command (string array) as an argument. If the command has an alias, substitution occurs and the new command is returned for execution. If an alias does not exist, the passed-in command is returned as-is. Both of these returned values are allocated with `malloc()`.
- When the user's command is "alias ...", the command is passed to [`alias()`](alias.c#L131), which parses the arguments and calls the appropriate function to print or modify the `aliases` linked list.
- [myhistory.c](myhistory.c) defines [`myhistory_add()`](myhistory.c#37)], which is called with a command immediately before that command is processed. Aliased commands are saved in their substituted form.
- When the user's command is "myhistory ...", the command is passed to [`myhistory()`](myhistory.c#L127), which parses the arguments and calls the appropriate function to print or modify the `history` linked list.

### Complete Specification:
- Any non-executed input is discarded by the shell.
- Ex: `command; ;  ;;;    command2;command3` is interpreted by the shell as `["command", "command2", "command3"]`
- When a bad command is mixed with good commands (E.g. `ls; wheresmybread; echo hello;`), the bad command will fail (printing something to `stderr`), and subsequent good commands will continue executing as if nothing had occurred.

### Known Bugs or Problems: 
- Implementation of batch mode file input is currently messy, but works.
- Input more than 512 characters long is not handled.
- Signal handling is not supported.
- Piping is not supported.