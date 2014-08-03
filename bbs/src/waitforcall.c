/**
 * waitforcall.c - functions used to wait for call.
 */

#include "waitforcall.h"
#include "config.h"
#include "util.h"
#include "globals.h"
#include "terminal.h"
#include <serial.h>
#include <stdio.h>
#include <string.h>

#define NUM_RINGS 1

unsigned char waitforcall()
{
  unsigned char i=0; // Received string from modem index.
  unsigned char c; // Last received modem character.
  char* match; // String to match.
  unsigned char ringCounter; // Ring counter.

  log(LOG_LEVEL_NOTICE,"Waiting for call.");
  printf("Waiting for call.\n");
 waitloop:

  while (ser_get(&c) == SER_ERR_NO_DATA) // TODO: Please get this out of here and into terminal!
    {
      waitforcall_check_console_switches();
      waitforcall_check_keyboard();
    }

  if (!match) // If no match yet.
    {
      if ((c==config_modemstrings->ring_string[0]))
	{
	  // First char, ring string.
	  match = config_modemstrings->ring_string;
	}
      else if ((c==config_modemstrings->connect_string[0]))
	{
	  // First char, connect string.
	  match = config_modemstrings->connect_string;
	}
    }
  else // We've started matching
    {
      if (c==match[i] && i < strlen(match))
	{
	  // We matched, scoot the comparator.
	  ++i;
	}
      else if (i=strlen(match))
	{
	  if (match==config_modemstrings->ring_string)
	    {
	      printf("Ring.\n");
	      log(LOG_LEVEL_NOTICE,"Ring.");
	      ++ringCounter;
	      if (ringCounter>=NUM_RINGS)
		{
		  return waitforcall_answer();
		}
	    }
	  else if (match==config_modemstrings->connect_string)
	    {
	      return WAITFORCALL_CONNECTED; // Connect.
	    }
	}
      else
	{ // We've slipped on the match, reset. 
	  match=NULL;
	  i=0;
	}
    }
  goto waitloop;

  return 0;

}

void waitforcall_check_console_switches()
{

}

void waitforcall_check_keyboard()
{

}

unsigned char waitforcall_answer()
{
  log(LOG_LEVEL_NOTICE,"Answering.");
  printf("Answering.\n");
  terminal_send_and_expect_response(config_modemstrings->answer_string,config_modemstrings->connect_string,0);
  return WAITFORCALL_CONNECTED;
}