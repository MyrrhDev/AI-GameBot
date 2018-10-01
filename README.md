# AI Game Bot

Strategy game for the Data Structures and Algorithms subject.

A competition was carried out in which students played against each other, from which a ranking was obtained. To participate in the competition, the players of the students had to pass a qualification test. 

Mark: 10/10

Top 5 out of 155

#### Compilation

To compile the game, open a console and cd to the directory where you extracted the source
code, run `make all` in the terminal. Make sure your AI player starts with AInameofplayer and it's a *.cc file.

#### Playing against the Dummy player
 
To play against the Dummy player, in the Makefile you must uncomment the correct object file for your architecture (i.e. if you have macOS, uncomment `DUMMY_OBJ = AIDummy.o.macOS`).

#### Playing against other players

Use the object files of other players and add them to the Makefile by setting the variable EXTRA OBJ.

#### Running the game

To run using Dummy Players as adversaries execute:

`./Game MPDaliLlamax Dummy Dummy Dummy -s 30 -i default.cnf -o default.out`

in the terminal.

#### Changing the maps

To change the default map of competition, you have to change default.cnf for one of the other * *.cnf* files for example, to change it to maze15.cnf:

`./Game MPDaliLlamax Dummy Dummy Dummy -s 30 -i maze15.cnf -o default.out`

## Watching the Game

To watch the result of the match, open the viewer file viewer.html with your browser
and load the file default.out. Or alternatively use the script viewer.sh,
e.g. viewer.sh default.out.
 AI Game Bot
