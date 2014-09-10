/**
 * menu.c - temporary crap.
 */

#include "menu.h"

#include "terminal.h"
#include "config.h"
#include "util.h"
#include "mboard.h"
#include "user.h"
#include <serial.h>
#include <6502.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <atari.h>
#include <stdlib.h>
#include <ctype.h>

unsigned char mode=MODE_MAIN_MENU;
MMUFile mmufd=0;
MMUEntry* mmuentry;

void _menu_display_screen(unsigned char mode)
{
  char filename[40];
  switch (mode)
    {
    case MODE_MAIN_MENU:
      strcpy(filename,"MENU>MAIN");
      break;
    case MODE_MSG_MENU:
      strcpy(filename,"MENU>MSGMAIN");
    }

  terminal_send_screen(filename);

}

void _menu_confirm(unsigned char c, const char* prompt)
{
  terminal_send_char(c);
  terminal_send_right();
  terminal_send_right();
  terminal_send_right();
  terminal_send(prompt,0);
  terminal_send_eol();
  terminal_send_eol();
}

void _menu_msg_open()
{
  char output[80];
  terminal_close_port();
  mmuentry = calloc(1,sizeof(MMUEntry));
  mmufd = mboard_open("D1:MAIN.MMU");
  mboard_get_default(mmufd,mmuentry);
  sprintf(output,"Current board is %s",mmuentry->itemName);
  terminal_open_port();
  terminal_send(output,0);
  terminal_send_eol();
}

void _menu_msg_close()
{
  mboard_close(mmufd);
  free(mmuentry);
}



unsigned char _menu_msg(unsigned char c)
{
  switch(toupper(c))
    {
    case 'G':
      _menu_confirm('G',"Goodbye");
      return 1;
    case 'X':
      _menu_confirm('X',"Exit to Main Menu");
      mode=MODE_MAIN_MENU;
      _menu_msg_close();
      return 0;
    default:
      terminal_beep();
      return 0;
    }
}

void _menu_user_list()
{
  UserRecord* record;
  char output[120];
  record = calloc(1,sizeof(UserRecord));
  terminal_close_port();
  terminal_send_screen("USERLIST");
  terminal_close_port();
  user_scan_begin();
  while (user_scan_next(record) == sizeof(UserRecord))
    {
      terminal_open_port();
      sprintf(output,"%5u %-13s %-15s",record->user_id,record->username,record->from);
      terminal_send(output,0);
      terminal_send_eol();
      memset(record,0,sizeof(UserRecord));
      terminal_close_port();
    }
  user_scan_end();
  terminal_open_port();
  free(record);
}

unsigned char _menu_main(unsigned char c)
{
  switch(toupper(c))
    {
    case 'G':
      _menu_confirm('G',"Goodbye");
      return 1;
      break;
    case 'M':
      _menu_confirm('M',"Message Board");
      mode=MODE_MSG_MENU;
      _menu_msg_open();
      return 0;
      break;
    case 'U':
      _menu_confirm('U',"User List");
      _menu_user_list();
    default:
      terminal_beep();
      return 0;
    }
  return 0;
}

unsigned char _is_valid_char(unsigned char mode, unsigned char c)
{
  switch(mode)
    {
    case MODE_MAIN_MENU:
      return (toupper(c)=='G' ||
	      toupper(c)=='M' ||
	      toupper(c)=='U');
      break;
    case MODE_MSG_MENU:
      return (toupper(c)=='G' ||
	      toupper(c)=='X');
      break;
    }
  return 0;
}

unsigned char _menu(unsigned char mode)
{
  char c;
  terminal_send(">> [_]",0);
  terminal_send_left();
  terminal_send_left();

 get:  c=terminal_get_char();

  if (!_is_valid_char(mode,c))
    {
      terminal_beep();
      goto get;
    }
  else // valid char
    { 
      switch(mode)
	{
	case MODE_MAIN_MENU:
	  return _menu_main(c);
	  break;
	case MODE_MSG_MENU:
	  return _menu_msg(c);
	  break;
	}
    }
}

/**
 * main entry point for menu. Exiting implies goodbye.
 */
void menu()
{
  unsigned char bQuit=0;
  mode=0;
  while (!bQuit)
    {
      _menu_display_screen(mode);
      bQuit = _menu(mode);
    }
}