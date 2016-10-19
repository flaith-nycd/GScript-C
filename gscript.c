/******************************************
 ***                                    ***
 *** GScript "ala" Mask Of The Sun Game ***
 ***      Copyleft 2005 par Flaith      ***
 ***                                    ***
 ******************************************/

//****************************************************************************
//*** Includes
//****************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "myLib.h"

#define __COLOR_ON__						// Affichage avec des couleurs

#ifdef __COLOR_ON__
	#include <conio2.h>
	// #pragma comment(lib,"libconio.a")
#endif

//****************************************************************************
//*** Define
//****************************************************************************

#define VERSION		"0.1.4"

#define DATE_VERSION	"(200705.060805)"	// 1ere version - dernière version

#define FALSE		0
#define TRUE		1

#define INF 		1
#define SUP 		2
#define EQU 		3
#define DIF 		4

// Type de variables que l'on créé
#define _INT		0
#define _CHAR		1

//*** Errors
#define ERR_JUMP_SUP	101		// jump to long (> _TOTAL_LINES)
#define ERR_JUMP_ZERO	102
#define ERR_JUMP_SIGN	103
#define ERR_DIVISION	104
#define ERR_THEN_NOIF	105
#define ERR_ELSE_NOTH	106
#define ERR_STACK_OVER	107
#define ERR_STACK_ZERO	108

#define ERR_LOAD_SCR		501	// unable to load script
#define ERR_VAR_NOTFOUND	502	// Var not found
#define ERR_FUNCTION	503	// Function not found

//***
#define STACKMAX	256		// nb max stack
#define LONGMAX 	80		// nb max char by line
#define LINEMAX		10000		// nb line max
#define END_SCRIPT	"@@@@@"		// end of a script/lib

//****************************************************************************
//*** Structures
//****************************************************************************
struct _LINE {
	int	_num;
	char	_line[LONGMAX];
};

struct _STA {
	char	*_val;		//[LONGMAX];
};

typedef struct {
	char	_variable[LONGMAX];
	char	_valeur[LONGMAX];
	int	_indice;	// cas ou il y a un tableau
	int	_type;		// 0:int, 1:char
} _VAR;

struct _LINE	_script_line[LINEMAX];	// = {0};
struct _LINE	_library_line[LINEMAX]; // = {0};

_VAR	_vars[LINEMAX];

//****************************************************************************
//*** Global vars
//****************************************************************************

FILE *fdebug;

// Scripts
int _EIP_ 		= 0;		// * Instruction en cours d'execution
int _KEY_;
struct _STA _STACK[STACKMAX];		// * conserve adr (numligne) lors
					//   d'appel à des fonctions ou lors
					//   de jump (+/- nbligne)
int _POS_STACK		= 0;		// * ligne du stack : augmente/diminue
					//   si PUSH/POP
int _TOTAL_LINES	= 0;		// * Nbre total de ligne dans le script
					//   en cours

// Library
int _EIP_LIB_ 		= 0;
struct _STA _STACK_LIB[STACKMAX];
int _POS_STACK_LIB	= 0;
int _TOTAL_LINES_LIB	= 0;

//***

char _SCRIPT_LOADED[LONGMAX] = {0};
int _DEBUG		= FALSE;
int _if_flag		= FALSE;
int _then_flag		= FALSE;
int _else_flag		= FALSE;

//*** Définitions des fonctions
//
void _BEEP(void);
void _ERROR(char *,int);
void _PUSH(char *);
char *_POP(void);
int _check_division(double);
int _jump(char *);
int _if_compare(char);
int _load_library(char *);
int _load_script(char *);
int _exec_line(int);
int _add_var(char *, char *, int);
int _run_script(char *);

//****************************************************************************

//****************************************************************************
void _BEEP(void)
{
	printf("\a");
}

void _ERROR(char *_error_,int _num_error_)
{
  char *msg = {0};
  char _BUF[LONGMAX];
  int  n = 0;
  
  switch (_num_error_) {
	case ERR_JUMP_SUP	: msg = "Jump over maxline"; break;
	case ERR_JUMP_ZERO	: msg = "Jump to line < 0"; break;
	case ERR_JUMP_SIGN	: msg = "Jump without +/-"; break;
	case ERR_DIVISION	: msg = "Division by zero"; break;
	case ERR_THEN_NOIF	: msg = "Then without if"; break;
	case ERR_ELSE_NOTH	: msg = "Else without then"; break;
	case ERR_STACK_OVER	: msg = "Stack overflow"; break;
	case ERR_STACK_ZERO	: msg = "Stack reached top"; break;

	case ERR_LOAD_SCR	: msg = "Unable to load script \"%s\"."; break;
	case ERR_VAR_NOTFOUND	: msg = "\"%s\": variable undeclared"; break;
	case ERR_FUNCTION	: msg = "\"%s\": function undeclared"; break;
	default			: msg = "Unknow"; break;
  }
  
  if (_num_error_ >= 500) n = sprintf(_BUF,msg,_error_);
  else strcpy(_BUF, msg);

  _BEEP();

  #ifdef __COLOR_ON__
    textcolor(BROWN);
  #endif
  printf("[GSCRIPT ERROR] %s.gs(%05d): ",_SCRIPT_LOADED, _EIP_ + 1);
  
  #ifdef __COLOR_ON__
    textcolor(YELLOW);
  #endif
  printf("*** E%05d: %s. ***\n",_num_error_, _BUF);

  #ifdef __COLOR_ON__
    textcolor(WHITE);
  #endif
  
  if (_DEBUG == TRUE) {
  	fprintf (fdebug, "MODE DEBUG >>>[GSCRIPT ERROR] %s.gs(%05d): *** E%05d: %s. ***\n",
			_SCRIPT_LOADED,
			_EIP_ + 1,
			_num_error_,
			_BUF);
	fclose(fdebug);  
  }
  
  exit(0);
}

void _PUSH(char *_valstack)
{
	_STACK[_POS_STACK]._val = (char *) malloc (sizeof(char) * LONGMAX + 1);
	strcpy(_STACK[_POS_STACK]._val,_valstack);
	_POS_STACK++;
	if (_POS_STACK > STACKMAX) _ERROR("",ERR_STACK_OVER);
}

char *_POP()
{
	_POS_STACK--;
	if (_POS_STACK < 0) _ERROR("",ERR_STACK_ZERO);
	else return (_STACK[_POS_STACK]._val);
	return 0;
}
  
int _check_division(double val)
{
  if (val == 0.0) _ERROR("",ERR_DIVISION);
	return 0;
}

int _jump(char *valeur)
{
  char eip[5], sig[2]={0}, *signe = NULL;
  int n;
  int nb_lignes=0;
  
  	nb_lignes = strtoint(valeur);
  
  	signe = _POP();
  	strncpy(sig,signe,1); 
  	sig[1] = '\0';
  	n = sprintf(eip,"%d",_EIP_);		//eip = inttostr(_EIP_);
//	_PUSH(eip);
	if (strcmp(sig,"-")==0) _EIP_ = _EIP_ - nb_lignes;
	else if (strcmp(sig,"+")==0) _EIP_ = _EIP_ + nb_lignes;
	else _ERROR("",ERR_JUMP_SIGN);
	if (_EIP_ > _TOTAL_LINES) _ERROR("",ERR_JUMP_SUP);
	if (_EIP_ < 0) _ERROR("",ERR_JUMP_ZERO);
	
	_exec_line(_EIP_);
	return 0;
//	_POP();					// il y a eu un PUSH, donc ...
}

int _if_compare(char valeur)
{
  switch (valeur) {
	case '<' : return INF;
	case '>' : return SUP;
	case ':' : return EQU;
	case '#' : return DIF;
	default  : return (0);
  }
}

//****************************************************************************
//*** Chargement Script & Lib
//****************************************************************************
int _load_library(char *_lib)
{
  FILE *fl;
  char ligne[LONGMAX] = {0}, lib_file[256] = {0};
  int  numligne = 0;

  strcpy(lib_file, _lib);
  strcat(lib_file, ".gsi");

  fl = fopen(lib_file,"r");

  if (fl == NULL) {
	printf("*** ERROR - Unable to load library \"%s\"\n",lib_file);
	exit(0);
  }
  else
  {
	while (fgets(ligne, LONGMAX, fl) != NULL) {
		_library_line[numligne]._num = numligne;
		strncpy(_library_line[numligne]._line, ligne, strlen(ligne)-1);
		  
		++numligne;
	}
	fclose(fl);
	_TOTAL_LINES_LIB = numligne;
	return (numligne);
  }
}

int _load_script(char *_file)
{
  FILE *fd;
  char ligne[LONGMAX] = {0}, script_file[256] = {0};
  int  numligne = 0;
  
  _TOTAL_LINES = 0;			// on commence le script à la 1ère ligne

  strcpy(script_file, _file);
  strcat(script_file, ".gs");

  fd = fopen(script_file,"r");
  if (fd == NULL) _ERROR(script_file, ERR_LOAD_SCR);
  else
  {
	while (fgets(ligne, LONGMAX, fd) != NULL) {
		_script_line[numligne]._num = numligne;
		strncpy(_script_line[numligne]._line, ligne, strlen(ligne)-1);
		_script_line[numligne]._line[strlen(ligne)-1]='\0';
		++numligne;
	}
	fclose(fd);
	_TOTAL_LINES = numligne;
	strcpy(_SCRIPT_LOADED, _file);
	return (numligne);
  }
}

//****************************************************************************
//*** Execution
//****************************************************************************
int _exec_line(int numline)
{
  int nb_len, flag_value=0, flag_continous=0;
  char *line = NULL;
  char valeur[LONGMAX] = {0}, valeur_tmp[LONGMAX] = {0};
  char cmd[2]={0};
  unsigned char c;
  
  // pour DEBUG
  char _BUF[LONGMAX];
  int n;
  struct tm * pdh;
  time_t intps; 
  intps=time(NULL);
  pdh = localtime(&intps);
  // **********
  
  line = (char *) malloc (sizeof(char) * LONGMAX + 1);
  strcpy(line,_script_line[numline]._line);

again:							// on revient ici après un
							// T(hen) ou E(lse)
  line = trim(line, " ");
  nb_len = strlen(line);

  if (line[0] == '.') {
  	if (line[1] != '!') {
	  strncpy(cmd, line+1, 1);
	  cmd[1] = '\0';
	  if (nb_len > 2) {
	  	  flag_value = 1;			//il y a qqchose à afficher
		  strncpy(valeur, line+3, nb_len);	//strlen(line));
		  nb_len -= 3;				//nb_len=strlen(line)-3;
	  } else flag_value = 0;
	  valeur[nb_len]='\0';
	}
  }
  
  // Début DEBUG
  if (_DEBUG == TRUE) {
	  n = sprintf(_BUF,">>>[%02d/%02d/%02d]-[%02d:%02d]-[%s.gs]-EIP[%05d]-KEY[%03d]-[%s]\n",
  		pdh->tm_mday, 		/* jour du mois, en chiffres */
  		pdh->tm_mon,		/* mois */
  		1900+pdh->tm_year, 	/* année */
  		pdh->tm_hour,
  		pdh->tm_min,
  		_SCRIPT_LOADED,
  		_EIP_ + 1,
  		_KEY_,
  		_script_line[_EIP_]._line);
	  fputs(_BUF, fdebug);	   
  }
  // Fin DEBUG
  
  switch (cmd[0]) {
 	case 'C' : /* Clear Screen */
 		   system("cls");	// GNU/Linux : system("clear");
		   break;
	case 'D' : /* Mode Debug ON/OFF */
		   if (strcmp(valeur,"ON")==0)  _DEBUG = TRUE;
		   if (strcmp(valeur,"OFF")==0) _DEBUG = FALSE;
		   break;
	case 'G' : /* Go script */
		   _run_script(valeur);
		   break;
	case 'H' : /* HTab */
		   break;
	case 'I' : /* IF */
		   _if_flag = TRUE;
		   _else_flag = FALSE;
		   
		   
		   
		   
		   break;
	case 'J' : /* Jump */
		   _PUSH(valeur);	// Obligatoire, car on récupère le signe
		   _jump(valeur);	// car possibilité de	.J +LL(5)
		   			// ou 			.J +7 
					// ou 			.J -2
		   			// ou encore		.J +L+L
		   break;
	case 'S' : /* Store val */
		   
		   break;
	case 'T' : /* THEN */
		   _then_flag = TRUE;
		   if (_if_flag == FALSE) _ERROR("",ERR_THEN_NOIF);
		   // strcpy(valeur_tmp,".");
		   // strcat(valeur,valeur_tmp);
		   // strcpy(line,valeur);
		   
		   _if_flag = FALSE;
		   // goto again;
		   break;
	case 'E' : /* ELSE */
		   _else_flag = TRUE;
		   if (_then_flag == FALSE) _ERROR("",ERR_ELSE_NOTH);
		   // strcpy(valeur_tmp,".");
		   // strcat(valeur,valeur_tmp);
		   // strcpy(line,valeur);
		   
		   _then_flag = FALSE;
		   // goto again;
		   break;
	case 'P' : /* Print */
		   if (valeur[0] == '"') {			// ya du texte
		   	if (flag_value) {			// on est sur qu'il y en a
		   		if (valeur[nb_len-1] == ';') {	// il y a un ';' à la fin
		   			flag_continous = 1;	// donc pas de saut de ligne
		   			strncpy(valeur, line+4, strlen(line)-1);
		   			nb_len=strlen(line)-4-1;
		   		}				// on sélectionne entre les '"'
		   		else {
		   			flag_continous = 0;	// on sélectionne entre les '"' aussi
		   			strncpy(valeur, line+4, strlen(line));
		   			nb_len=strlen(line)-4;
		   		}
		 	  	valeur[nb_len]='\0';		// obligatoire
		   		if (flag_continous) printf("%s",valeur);
		   		else printf("%s\n",valeur);	// et on affiche
		   	} else printf("\n");			// sinon saut de ligne
		   }		   				// recherche une variable et l'affiche
		   else if (flag_value) {			// il n'y a pas de '"'
		   		if (valeur[nb_len-1] == ';') {	// kif kif, saut de ligne ?
		   			flag_continous = 1;	// yen a un donc pas de saut
		   			strncpy(valeur, line+3, strlen(line)-1);
		   			nb_len=strlen(line)-3-1;
		   		}
		   		else {
		   			flag_continous = 0;	// pas de saut
		   			strncpy(valeur, line+3, strlen(line));
		   			nb_len=strlen(line)-3;
		   		}
		 		valeur[nb_len]='\0';		// tjrs obligatoire
		   	if (strcmp(valeur,"_VER_")==0) {	// aff la version
		   		#ifdef __COLOR_ON__
					textcolor(LIGHTGREEN);
  					printf("Color ");
	   		  	#endif
 					printf("GScript par Flaith V%s.%s\n",VERSION,DATE_VERSION);
		   		#ifdef __COLOR_ON__
    				textcolor(WHITE);
  				#endif
		   	}
		   	if (strcmp(valeur,"_KEY_")==0) {	// affiche la touche saisie
	   			#ifdef __COLOR_ON__
				  	textcolor(LIGHTGREEN);
  				#endif
		   		if (_KEY_ > 32 && _KEY_ < 128) {
		   			if (flag_continous) printf("GK_%c", toupper(_KEY_));
		   			else printf("GK_%c\n", toupper(_KEY_));
		   		} else  if (flag_continous) printf("GK_%d", _KEY_);
		   			else printf("GK_%d\n", _KEY_);
		   	}
		   } else printf("\n");
	   	   #ifdef __COLOR_ON__
    		   	textcolor(WHITE);
		   #endif
		   break;
	case 'V' : /* VTab */
		   break;
	case 'W' : /* Wait */
		   if (flag_value) {
			   strncpy(valeur, line+4, strlen(line));
			   nb_len=strlen(line)-4;
			   valeur[nb_len]='\0';
			   printf("%s",valeur);
		   }
		   c = getch();				// c=getchar(); // sous GNU/Linux
		   if (c == 0) c = getch();
		   _KEY_ = c;				// on conserve la clé dans une var
		   break;
	default  : break;
  }
}

int _add_var(char *varname, char *valeur, int typevar)
{
  // if(_check_var(varname) == -1)	// verifie si la variable existe déjà
  					// si = -1 pas trouvé donc on créer 
}

int _run_script(char *name_script)
{
  int i=0, nb_line_script;
  
  nb_line_script = _load_script(name_script);
  
  if (nb_line_script != -1) {
  	_EIP_ = 0;

  	while (i < nb_line_script) {
  		_exec_line(_EIP_);
  		_EIP_++; i = _EIP_;
  	}
  }
	
  return (0);
}

//****************************************************************************
//*** Main
//****************************************************************************

int main(void)
{
  char *script = "test";		// Nom du 1er script à lancer !!!

  #ifdef __COLOR_ON__
		textcolor(WHITE);
	#endif

  fdebug = fopen("DEBUG.LOG","w+");

  _DEBUG = FALSE;
  _if_flag = FALSE;
  _then_flag = FALSE;
  _else_flag = FALSE;

  _TOTAL_LINES_LIB = _load_library("library");

  strcpy(_SCRIPT_LOADED, script);	// Le faire la 1ère fois pour affichage
					// du nom du script ou se trouve l'erreur
  
  // _add_var("_KEY_","",_INT););
  // _add_var("_VER_",version,_CHAR);
  
  _run_script(script);

  //_ERROR("GE(26)",ERR_VAR_NOTFOUND);	// test affichage de l'erreur 
  
  fclose(fdebug);  
  return (0);
}
