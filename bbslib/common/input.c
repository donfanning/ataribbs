/**
 * input.c - slightly higher level input routines
 */

#include "input.h"
#include "config.h"
#include "util.h"
#include "terminal.h"
#include <string.h>

extern unsigned char terminal_type; // from terminal.c

char* input_line_and_echo(unsigned char rubout, unsigned char size, char* existing_text)
{
  unsigned char c=0;
  unsigned char i=0;
  unsigned char j=0;
  unsigned char buf[64];

  buf[0]='\0';

  if(existing_text!=NULL)
    {
      strcpy(buf,existing_text);
      i=strlen(existing_text);
      j=strlen(existing_text);
    }

  while (is_a_return(c) == 0)
    {
      c=terminal_get_and_echo(i,j,size,rubout);
      if (is_a_backspace(c)==1)
	{
	  if (i>0)
	    {
	      --i;
	      j=i;
	      buf[i] = '\0';
	    }
	}
      else if (is_a_left(c) == 1)
	{
	  if (i>0)
	    {
	      --i;
	    }
	}
      else if (is_a_right(c) == 1)
	{
	  if (i<j)
	    {
	      ++i;
	    }
	}
      else if (is_a_return(c) == 0)
	{
	  if (i<size)
	    {
	  buf[i] = c;
	  ++i;
	  j=i;
	  buf[i] = '\0';
	    }
	  else
	    {
	      buf[i-1]=c;
	      buf[i]='\0';
	    }
	}
    }
  return strdup(buf);
}

char* input_line_and_echo_char(unsigned char e,unsigned char rubout, unsigned char size)
{
  unsigned char c=0;
  unsigned char i=0;
  unsigned char buf[64];

  buf[0]='\0';

  while (is_a_return(c) == 0)
    {
      c=terminal_get_and_echo_char(i,size,e,rubout);
      if (is_a_backspace(c)==1)
	{
	  if (i>0)
	    {
	      --i;
	      buf[i] = '\0';
	    }
	}
      else if (is_a_return(c) == 0)
	{
	  if (i<size)
	    {
	  buf[i] = c;
	  ++i;
	  buf[i] = '\0';
	    }
	  else
	    {
	      buf[i]=c;
	    }
	}
    }
  return strdup(buf);
}

char* prompt_line(unsigned char type, unsigned char size, char* existing_text)
{
  char b,r,e,i;
  char prompt[66];
  switch (type)
    {
    case 0:
      b=0;
      r=0;
      e=0;
      break;
    case 1:
      b='[';
      r='_';
      e=']';
      break;
    case 2:
      b='>';
      r='_';
      e='<';
    }
  memset(&prompt,0x20,66); // blank it out.
  memset(&prompt,r,size+1); // fill with rubout character
  prompt[0]=b; // beginning character
  prompt[size+1]=e; // end character
  prompt[size+2]='\0'; // and a null.

  terminal_send(prompt,0);
  for (i=0;i<size+1;++i)
    {
      terminal_send_left();
    }

  if (existing_text!=NULL)
    {
      terminal_send(existing_text,0);
    }

  return input_line_and_echo(r,size,existing_text);

}

char* prompt_password_line(unsigned char type, unsigned char size, unsigned char echo)
{
  char b,r,e,i;
  char prompt[66];
  switch (type)
    {
    case 0:
      b=0;
      r=0;
      e=0;
      break;
    case 1:
      b='[';
      r='_';
      e=']';
      break;
    case 2:
      b='>';
      r='_';
      e='<';
    }
  memset(&prompt,0x20,66); // blank it out.
  memset(&prompt,r,size+1); // fill with rubout character
  prompt[0]=b; // beginning character
  prompt[size+1]=e; // end character
  prompt[size+2]='\0'; // and a null.

  terminal_send(prompt,0);
  for (i=0;i<size+1;++i)
    {
      terminal_send_left();
    }

  return input_line_and_echo_char(echo,r,size);

}
