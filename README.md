# UNIX 'Nush' Shell

A shell operates as follows:

* Accept an optional script file as the first command line argument.
* If there's no script, show a prompt. Your prompt should be "nush$ ". Command input is accepted on the same line after the prompt.
* Read one line of commands from either stdin or the script.
* Execute that line of commands.
* Repeat until EOF or an "exit" command.
* Command lines are made up of:

* Programs to execute.
* Built-in commands.
* Command line arguments to programs / builtins.
* Operators.

Operators either modify the behavior of one command or chain together multiple commands into a single command line.

Your shell should support seven operators:

* Redirect input: sort < foo.txt
* Redirect output: sort foo.txt > output.txt
* Pipe: sort foo.txt | uniq
* Background: sleep 10 &
* And: true && echo one
* Or: true || echo one
* Semicolon: echo one; echo two
