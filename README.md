# FAN Shell

## About
FAN is a customized shell that I have written from scratch in C and C++. The main objective was not to add yet another shell to the broad swath of available shells 
but rather to acquire a deeper understanding of how system calls work and of course also how does the shell itself function.

## Language version
C++11

## OS
Linux

## How to build
After forking the repository in a directory that I shall call REPOS, delete all the files in the objects folder. Then compile every .cpp or .c file of 
the repository in this object folder by the following command: <br /><br />
$ g++ -I $(REPOS)/Shell/include -g -c <.cpp/.c file address relative to where you are currently in the terminal> <br /><br />
You should then link the object files. For this refer to the FakeMakeFile. The order of the files is laid out in there.

## Features
The FAN shell enables one to use redirection from command execution to a file only, indefinite pipelines, & and | features like any other shell,
view the last 200 commands by pressing up and down just like most other shells, and finally one can also call any of the programs in PATH by calling 
their name e.g ls. It also has a builtin cd, exit commands. For cd, it has the following features: <br />
 1. cd - <br />
 2. cd <br />
 3. cd .. <br />
 4. cd "absolute path" 

<br />Whenever you execute a command the shell executes it and then escapes to the next page. You have to scroll up to view the result of your command. This complication was due to having to turn on raw mode in order to be able to enable the history commands feature mentioned above.<br/><br/> 

 You can also exit the shell by typing exit or Ctr-q.
