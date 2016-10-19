.P "Test4 loaded by test3
.P "---------------------
.P
.P "The end :) ???
.P
.P
'affiche l'avatar
.!AVAT
.W "Press a key :
.J +5
.J +10
'.I _KEY_:GK_ESC
.T J +2
.E G test
.P
.P "You pushed the ";
.P _KEY_;
.P "" key
.J -8
.P OS(27)
.W "Appuyez sur une touche quelconque : 
.C
.P "Bye bye !!!
.P
.P
.P _VER_
.P
.P "Test Affichage d'une erreur...
.P "Le "Then" n'est pas encore definit
.P "Donc il y aura une erreur !!!
.T G test
.P "Put Debug OFF
.D OFF
@@@@@
!WAIT
.W "Press a key : 
!AVAT
.P "          \|/ ____ \|/
.P "          "@'/ ,. \'@"
.P "          /_| \__/ |_\
.P "             \__U_/
!!!!!
