# battleshipSweeper
a battleship and minesweeper hibr multiplayer game in c network socket programming
it was a little project in university for computer networks 2 course
when 2 player connected the game begins
after that other 2 player can connect too and will be a separated game (fork create a new process)

## usage ##

### build ###
```bash
gcc battleshipSweeper.c -o battleshipSweeper
```
### run ###
```bash
./battleshipSweeper
```

*the server is running*

### you can connect with ###
```bash
telnet localhost 1111
```

### rules ###
* Torpedo shooting : FIRE <coordinate>
	* e.g.: FIRE A12	
* Get table (current state): TABLE

* 'X' marks the hits

* '-' mark the unsuccessful attempts

* Get rules: RULES

* When the players shut all the ships the game is over

* After that we can begin a new game or quit
	* New Game Y or YES
	* End of the game N or NO
	
* You can quit with the QUIT command.

