# Quoridor Engine

## About
An engine made in c simulating the board game `Quoridor` on a terminal </br>
How to play: see available commands on `Commands/Quoridor Text Protocol` </br>

### About AI Agent
Currently an alpha-beta pruning algorithm </br>
The evaluation function has the following 4 features: 
* Black's distance from goal 
* White's distance from goal
* Penalty if the minimizing agent is very close to the goal 
* Penalty if the maximizing agent has fewer walls than minimizing agent

Uses iterative deepening to get the most depth in a given fixed move time (4.5 seconds) </br>
As the game progresses and the branching factor decreases the depth the agent can see ahead increases </br>

Sorts wall placements by distance to the enemy. The furthest away a wall is from the enemy the more likely it is that it's a bad wall to place </br>

Possible improvements of the agent:
* Sort player moves by sortest distance to its goal
* Sort wall placements by `min{distance from minimizing agent, distance from maximizing agent}`.
    This will make walls that will support the maximizing agent be considered earlier
    Possible a mid to late game improvement
* Take into account the actual path the maximizing agent has to follow and place walls to make it harder for the minimizing agent to block it  

## Compilation and runtime
`make` to compile, `make run` to compile and run </br>
`make clean` to clean files created during compilation

## Commands/Quoridor Text Protocol
Commands supported by the engine used in plaing the game </br>
Assumes the input is mostly correct, no major parsing/error checking is being done

### 4.1 Adminstrative Commands
- name
    * arguments &ensp; none
    * effects &emsp; &emsp; none
    * output &emsp; &emsp; name </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; string* name - Name of the engine
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp; E.g. "IP Quoridor". 
    
- known_command
    * arguments &ensp; command_name </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; string command_name - Name of a command
    * effects &emsp; &emsp; none
    * output &emsp; &emsp; known </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; boolean known - "true" if the command is known by the engine, "false" otherwise
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp; The protocol makes no distinction between unknown commands and known but unimplemented ones. </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; Do not declare a command as known if it is known not to work.
    
- list_commands
    * arguments &ensp; none
    * effects &emsp; &emsp; none
    * output &emsp; &emsp; commands </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; string& commands - List of commands, one per row
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp; Include all known commands, including required ones and private extensions.
    
- quit
    * arguments &ensp; none
    * effects &emsp; &emsp; The session is terminated and the connection is closed.
    * output &emsp; &emsp; none
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp; The full response of this command must be sent before the engine closes the connection. </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; The controller must receive the response before the connection is closed on its side.
    
### 4.2 Setup Commands
- boardsize
    * arguments &ensp; size </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; int size - New size of the board.
    * effects &emsp; &emsp; The board size is changed. </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; The board configuration, the number of walls of each player, and move history become arbitrary.
    * output &emsp; &emsp;  none
    * fails &emsp; &emsp; &emsp; Syntax error. </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; If the engine cannot handle the new size, fails with the error message "unacceptable size".
    * comments &ensp; The controller must call clear_board and walls explicitly. </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; Even if the new board size is the same as the old one, the board configuration and the walls number become arbitrary.
    
- clear_board
    * arguments &ensp;  none
    * effects &emsp; &emsp; The board is cleared, the two pawns return to their starting position, 
        the number of walls of each player becomes arbitrary and the move history is reset to empty.
    * output &emsp; &emsp; none
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp;    
    
- walls
    * arguments &ensp; number_of_walls </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; int number_of_walls - Number of walls for each player.
    * effects &emsp; &emsp; Each player has that number of walls at the beginning.
    * output &emsp; &emsp;  none
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp;   
    
### 4.3 Core Play Commands
- playmove
    * arguments &ensp; where </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; move where - Color and vertex of the move
    * effects &emsp; &emsp;     The player of the requested color is played at the requested vertex. 
    * output &emsp; &emsp;  none
    * fails &emsp; &emsp; &emsp;    syntax error, illegal move. In the latter case, fails with the error message "illegal move".
    * comments &ensp; Consecutive moves of the same color are not considered illegal from the protocol point of view.
    
- playwall
    * arguments &ensp; placement </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; wall placement  - Color, vertex and orientation of the wall
    * effects &emsp; &emsp;     A wall place at the requested vertex and orientation. Decrease number of wall for that player by one.
    * output &emsp; &emsp;  none
    * fails &emsp; &emsp; &emsp;    syntax error, illegal placement. In the latter case, fails with the error message "illegal move".
    * comments &ensp; Consecutive moves of the same color are not considered illegal from the protocol point of view.
    
- genmove
    * arguments &ensp; player </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; color player - Color for which to generate a move.
    * effects &emsp; &emsp; The engine makes a move or wall placement for the requested color.
    * output &emsp; &emsp; vertex orientation </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; vertex||string </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; Vertex where the move was played or where the wall was placed.
    * fails &emsp; &emsp; &emsp; never
    * comments &ensp; If orientation returned a wall placed, otherwise it's a move.
    
- undo
    * arguments &ensp; times (optional) </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; int times - how many times to undo
    * effects &emsp; &emsp; The game goes 'times' moves back.
    * output &emsp; &emsp; none
    * fails &emsp; &emsp; &emsp; If times is greater than moves played or no moves are played. Fails with the error message "cannot undo".
    * comments  
    
### 4.4 Tournament Commands
- winner
    * arguments &ensp;  none
    * effects &emsp; &emsp; none
    * output &emsp; &emsp; boolean color </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; boolean - true if game ended, otherwise false </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; color - It's the color of the winner 
    * fails &emsp; &emsp; &emsp; never
    * comments 
    
### 4.5 Debug Commands
- showboard </br>
    * arguments &ensp; none 
    * effects &emsp; &emsp; none 
    * output &emsp; &emsp;  board </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; string*& board - A diagram of the board position 
    * fails &emsp; &emsp; &emsp; never 
    * comments &ensp; The engine may draw the board as it likes.  
        &emsp; &emsp; &emsp; &emsp; &ensp; It is, however, required to place the coordinates as described in section 1.11. </br>
        &emsp; &emsp; &emsp; &emsp; &ensp; This command is only intended to help humans with debugging and the output should never 
        need to be parsed by another program.




## Example flow
```
make run 
boardsize 9
walls 10
genmove black
playmove white E2
genmove black 
genmove white
...
quit
```

## Referee
To play with referee you need 2 executables (can play against itself) </br>
Executables must follow the `Quoridor Text Protocol` described previously
```
Run with:

quoridor_referee.py --white '<path to program 1>' \
                    --black '<path to program 2>' \

Possible quoridor_referee options:

  --verbose 1 (to list moves) or --verbose 2 (to draw board)
  --size <board size>               (default 9)
  --walls <number of walls>         (default 7/4*boardsize-23/4)
  --games <number of games to play> (default 1)
  --maximum_time <time(in seconds)> (default boardsize/3)
  --move_time <time(in seconds)>    (default 30)
  --memory_limit <memory(in MB)>    (default 950MB)
  --seed <seed>                     (default current unix timestamp)
```

Engines on folder `CupProgs` are (mostly) from [here](https://lists.di.uoa.gr/showpost.php?p=93145&postcount=22) (in greek)

