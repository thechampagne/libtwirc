#include <stdio.h>      // NULL, fprintf(), perror()
#include <stdlib.h>     // NULL, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>     //
#include <errno.h>      // errno
#include <sys/types.h>  // ssize_t
#include <time.h>
#include "libtwirc.h"

/*
 *
 */
int read_token(char *buf, size_t len)
{
	FILE *fp;
	fp = fopen ("token", "r");
	if (fp == NULL)
	{
		return 0;
	}
	char *res = fgets(buf, len, fp);
	if (res == NULL)
	{
		fclose(fp);
		return 0;
	}
	size_t res_len = strlen(buf);
	if (buf[res_len-1] == '\n')
	{
		buf[res_len-1] = '\0';
	}
	fclose (fp);
	return 1;
}

void handle_ping(struct twirc_state *s, struct twirc_event *evt)
{
	fprintf(stdout, "*** received PING: %s\n", evt->params[0]);
}

/*
 *
 */
void handle_connect(struct twirc_state *s, struct twirc_event *evt)
{
	fprintf(stdout, "*** connected!\n");
}

/*
 *
 */
void handle_welcome(struct twirc_state *s, struct twirc_event *evt)
{
	// Let's join a lot of channels to test this bad boy!
	twirc_cmd_join(s, "#domsson");

	twirc_cmd_join(s, "#hanryang1125");
	twirc_cmd_join(s, "#toborprime");
	twirc_cmd_join(s, "#honestdangames");
	twirc_cmd_join(s, "#meowko");
	twirc_cmd_join(s, "#kitboga");
	twirc_cmd_join(s, "#hyubsama");
	twirc_cmd_join(s, "#bawnsai");
	twirc_cmd_join(s, "#bouphe");
	twirc_cmd_join(s, "#retrogaijin");
	twirc_cmd_join(s, "#yumyumyu77");
	twirc_cmd_join(s, "#esl_csgo");
}

/*
 *
 */
void handle_join(struct twirc_state *s, struct twirc_event *evt)
{
	fprintf(stdout, "*** %s joined %s\n", evt->nick, evt->params[0]);

	if (evt->nick && strcmp(evt->nick, "kaulmate") == 0
	    && strcmp(evt->params[0], "#domsson") == 0)
	{
			twirc_cmd_privmsg(s, "#domsson", "jobruce is the best!");
			twirc_cmd_action(s, "#domsson", "is the coolest bot of all times");
	}
}

void handle_privmsg(struct twirc_state *s, struct twirc_event *evt)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	fprintf(stdout, "[%02d:%02d:%02d] (%s) %s: %s\n", 
			tm.tm_hour, tm.tm_min, tm.tm_sec, 
			evt->channel, evt->nick, evt->message);
}

void handle_action(struct twirc_state *s, struct twirc_event *evt)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	fprintf(stdout, "[%02d:%02d:%02d] (%s) * %s %s\n",
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			evt->channel, evt->nick, evt->message);
}

void handle_whisper(struct twirc_state *s, struct twirc_event *evt)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	fprintf(stdout, "[%02d:%02d:%02d] (whisper) %s: %s\n",
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			evt->nick, evt->message);
}

void handle_disconnect(struct twirc_state *s, struct twirc_event *evt)
{
	fprintf(stdout, "*** connection lost\n");
}

/*
 * main
 */
int main(void)
{
	// HELLO WORLD
	fprintf(stderr, "Starting up %s version %o.%o build %f\n",
		TWIRC_NAME, TWIRC_VER_MAJOR, TWIRC_VER_MINOR, TWIRC_VER_BUILD);

	// CREATE TWIRC INSTANCE
	struct twirc_state *s = twirc_init();
	
	// SET UP CALLBACKS
	struct twirc_callbacks *cbs = twirc_get_callbacks(s);
	cbs->connect    = handle_connect;
	cbs->welcome    = handle_welcome;
	cbs->join       = handle_join;
	cbs->action     = handle_action;
	cbs->privmsg    = handle_privmsg;
	cbs->whisper    = handle_whisper;
	cbs->disconnect = handle_disconnect;

	if (s == NULL)
	{
		fprintf(stderr, "Could not init twirc state\n");
		return EXIT_FAILURE;
	}

	fprintf(stderr, "Successfully initialized twirc state...\n");
	
	// READ IN TOKEN FILE
	char token[128];
	int token_success = read_token(token, 128);
	if (token_success == 0)
	{
		fprintf(stderr, "Could not read token file\n");
		return EXIT_FAILURE;
	}

	// CONNECT TO THE IRC SERVER
	if (twirc_connect(s, "irc.chat.twitch.tv", "6667", token, "kaulmate") != 0)
	{
		fprintf(stderr, "Could not connect socket\n");
		return EXIT_FAILURE;
	}

	if (errno = EINPROGRESS)
	{
		fprintf(stderr, "Connection initiated...\n");
	}

	// MAIN LOOP
	twirc_loop(s, 1000);

	twirc_kill(s);
	fprintf(stderr, "Bye!\n");

	return EXIT_SUCCESS;
}

