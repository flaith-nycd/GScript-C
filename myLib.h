// Routine to convert int to string
//
/*
char *itoa(int i)
{
  char *s=malloc(42); // Enough for a 128 bit integer
  if (s) sprintf(s,"%d",i);
  return s;
}
*/
/*
static char *i2a(unsigned i, char *a, unsigned r)
{
  if (i/r > 0) a = i2a(i/r,a,r);
  *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i%r];
  return a+1;
}

char *itoa(int i, char *a, int r)
{
  if ((r < 2) || (r > 36)) r = 10;
  if (i<0) {
    *a = '-';
    *i2a(-(unsigned)i,a+1,r) = 0;
  } else *i2a(i,a,r) = 0;
  return a;
}
*/

char *inttostr(int i)
{
   //char s[20] = { 0 };
   char *s = malloc(42);

   if (itoa(i, s, 10)) return s;
}

int strtoint( char *s )
{
  int val = 0;
  int i;
  
  for (i = 0; s[i] != '\0'; i++)
      if (isdigit(s[i])) val = val * 10 + s[i] - '0';
  return val;
}

long strtolong( char *s )
{
  long val = 0;
  int i;

  for (i = 0; s[i] != '\0'; i++)
      if (isdigit(s[i])) val = val * 10 + s[i] - '0';
  return val;
}

double strtodouble( char *s, char *punct )
{
  int i, start, length, punctPos;
  double result, sign, fractPart;
  
  result = fractPart = 0.0;
  length = punctPos = strlen(s);

  start = 0;
  sign = 1.0;
  if (s[0] == '-')
    {
      sign = -1.0;
      start = 1;
    }

  for (i = start; i < length; i++)  //parse de gauche a droite
    {                               //conversion de la partie entière
      if (s[i] != punct[0])
	{
          if (isdigit(s[i])) result = result * 10.0 + (s[i] - '0');
	  else return 0.0;
	}
      else
	{
	  punctPos = i;
	  break;
	}
    }

  if (s[punctPos] == punct[0])  //parse de la fin jusqu'au car de ponctuation
    {                           //conversion da la partie fraction
      for (i = length - 1; i > punctPos; i--)
	{
	  if (isdigit(s[i])) fractPart = fractPart / 10.0 + (s[i] - '0');
	  else return 0.0;
	}
      result += fractPart / 10.0;
    }

  return result * sign;
}

// Converti une chaine de texte en minuscule
char * strtoupper(char *dest, const char *src)
{
	int i;

	i = strlen(src);
	dest[i] = '\0';

	while(i>0)
		{
			dest[i - 1] = toupper(src[i - 1]);
			i--;
		}
	return dest;
}

int is_sep( char *_ll, char *_sep )
{
  // Recherche dans une ligne (_ll)
  // le car (sep) et renvoi sa position
  int _ind = 0;
  for (_ind; _ind <= strlen(_ll) && (_ll[_ind] != _sep[0]); _ind++);
      
  if (_ll[_ind] == _sep[0]) // si on trouve le car. on renvoi sa position
      return _ind;
    else 
      return -1;
}

//****************************************************************************
//*** posFind
//****************************************************************************

int posFind(char *line, int car)
{
  // Recherche dans une "line"
  // le "car" et renvoi sa position
  
  int _ind = 0;
  for (_ind; _ind <= strlen(line) && (line[_ind] != car); _ind++);
      
  if (line[_ind] == car) // si on trouve le car. on renvoi sa position
      return _ind;
    else 
      return -1;
}

const char *find( const char *str, char c )
{
    while( *str ) {
        if ( *str == c ) return str;
        ++str;
    }
    return 0;
}

/*
int find( char *str, char c )
{
    int the_pos = 0;
        
    while( *str ) {
    	printf("%c,%d",c,the_pos);
        if ( *str == c ) return the_pos;
        ++str; the_pos++;
    }
    return 0;
}
*/
/*
int find( char *str, char *c )
{
  int the_pos = 0;
  
  for (the_pos; the_pos <= strlen(str) && (str[the_pos] != c[0]); the_pos++);

  if ( str[the_pos] == c[0] ) 
      return the_pos;
    else
      return -1;
}
*/

char* copy( char* dst, const char* src ) {
    char* cur = dst;
    while( *cur++ = *src++ )
        ;
    return dst;
}

char *rtrim( char *str, const char *t )
{
    char *curEnd = str, *end = str;

    char look[ 256 ] = { 1, 0 };
    while( *t )
       look[ (unsigned char)*t++ ] = 1;
 
    while( *end ) {
      if ( !look[ *end ] )
          curEnd = end + 1;
      ++end;
    }
    *curEnd = '\0';
 
    return str;
}
 
char *ltrim( char *str, const char *t )
{
    char *curStr = NULL;
 
    char look[ 256 ] = { 1, 0 };
    while( *t )
        look[ (unsigned char)*t++ ] = 1;
 
    curStr = str;
    while( *curStr && look[ *curStr ] )
        ++curStr;
 
    return copy( str, curStr );
}
 
char *trim( char *str, const char *t )
{
    return ltrim( rtrim( str, t ), t );
}
 
char *replace( char *str, const char *r, const char *b )
{
    char *curDst = NULL;
 
    char look[ 256 ] = { 0 };
    while( *r && *b )
        look[ (unsigned char)*r++ ] = *b++;
 
    curDst = str;
    while( *curDst ) {
        char c = look[ *curDst ];
        if ( c ) *curDst = c;
        ++curDst;
    }
    return str;
}
 
char *reverse( char *str )
{
    char *right = str, *left = NULL;
    while( *right )
        ++right;
 
    left = str;
    while( left < right ) {
        char tmp = *left;
        *left++ = *--right;
        *right = tmp;
    }
    return str;
}

char *delchar( char *s, char *car )
{
  int i, y = 0;
  char *result = s;
  
  for (i = 0; i < strlen(s); i++)
    if (s[i] != *car)
      {
        result[y] = s[i];
	y++;
      }

  result[y] = '\0';
  return result;
}
