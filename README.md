# myShell

Apostolou Orestis 8587
AUTH Electric Engineering Faculty

myShell implemantation

The shell supports the operands ; and &&. 
The first operand executes the first and the second command and prints the output to screen.
The second operand executes the first command, and only if the execution was succesful it continues to the execution of the second command
which can either succeed or fail.

The built-in functions of the shell are quit and cd.
By typing quit the program exits.
By typing cd you change the working directory.

This version does not support usage of cd with operand &&, and only supports usage of cd with ; , using cd as the first argument.

For example: "cd .. ; ls -a; touch new.txt" is supported.
"ls -a ; cd ..;" is not supported
" cd .. && ls " is not supported
