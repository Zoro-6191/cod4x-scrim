#include <ctype.h>
#include "cmd_completion.h"
#include "qcommon_io.h"

/*
===========================================
command line completion
===========================================
*/
#define MAX_TOKEN_CHARS 1024
/*
==================
Field_Clear
==================
*/
void Field_Clear( field_t *edit )
{
	memset(edit->buffer, 0, MAX_EDIT_LINE);
	edit->cursor = 0;
	edit->scroll = 0;
}

static const char	*completionString;
static char		shortestMatch[MAX_TOKEN_CHARS];
static int		matchCount;
// field we are working on, passed to Field_AutoComplete(&g_consoleCommand for instance)
static field_t		*completionField;

/*
===============
FindMatches
===============
*/
static void FindMatches( const char *s )
{
	int i;
	if ( Q_stricmpn( s, completionString, strlen( completionString ) ) )
		return;

	matchCount++;
	if ( matchCount == 1 )
	{
		Q_strncpyz( shortestMatch, s, sizeof( shortestMatch ) );
		return;
	}

	// cut shortestMatch to the amount common with s
	for ( i = 0 ; shortestMatch[i] ; i++ )
	{
		if ( i >= strlen( s ) ) 
		{
			shortestMatch[i] = 0;
			break;
		}
		if ( tolower(shortestMatch[i]) != tolower(s[i]) )
			shortestMatch[i] = 0;
	}
}
/*
===============
PrintMatches
===============
*/
static void PrintMatches( const char *s )
{
	if ( !Q_stricmpn( s, shortestMatch, strlen( shortestMatch ) ) )
		Com_Printf(CON_CHANNEL_DONT_FILTER, "    %s\n", s );
}

/*
===============
PrintCvarMatches
===============
*/
static void PrintCvarMatches( const char *s )
{
	char value[ TRUNCATE_LENGTH ];
	char c_str[2048];

	if ( !Q_stricmpn( s, shortestMatch, strlen( shortestMatch ) ) )
	{
		Com_TruncateLongString( value, Cvar_VariableStringBuffer( s , c_str, sizeof(c_str)) );
		Com_Printf(CON_CHANNEL_DONT_FILTER, "    %s = \"%s\"\n", s, value );
	}
}
/*
===============
Field_FindFirstSeparator
===============
*/
static char *Field_FindFirstSeparator( char *s )
{
	int i;
	for( i = 0; i < strlen( s ); i++ )
		if( s[ i ] == ';' )
			return &s[ i ];
	return NULL;
}

/*
===============
Field_Complete
===============
*/
static qboolean Field_Complete( void )
{
	int completionOffset;

	if( matchCount == 0 )
		return qtrue;

	completionOffset = strlen( completionField->buffer ) - strlen( completionString );

	Q_strncpyz( &completionField->buffer[ completionOffset ], shortestMatch,
		sizeof( completionField->buffer ) - completionOffset );

	completionField->cursor = strlen( completionField->buffer );

	if( matchCount == 1 )
	{
		Q_strncat( completionField->buffer, sizeof( completionField->buffer ), " " );
		completionField->cursor++;
		return qtrue;
	}
	Com_Printf(CON_CHANNEL_DONT_FILTER, "]%s\n", completionField->buffer );
	return qfalse;
}
/*
===============
Field_CompleteCommand
===============
*/
void Field_CompleteCommand( char *cmd, qboolean doCommands, qboolean doCvars )
{
	int completionArgument = 0;
	// Skip leading whitespace and quotes
	cmd = Com_SkipCharset( cmd, " \"" );

	Cmd_TokenizeString( cmd );
	completionArgument = Cmd_Argc( );

	// If there is trailing whitespace on the cmd
	if( *( cmd + strlen( cmd ) - 1 ) == ' ' )
	{
		completionString = "";
		completionArgument++;
	}
	else completionString = Cmd_Argv( completionArgument - 1 );

	if( completionArgument > 1 )
	{
		const char *baseCmd = Cmd_Argv( 0 );
		char *p;

		if( ( p = Field_FindFirstSeparator( cmd ) ) )
			Field_CompleteCommand( p + 1, qtrue, qtrue ); // Compound command
		else Cmd_CompleteArgument( baseCmd, cmd, completionArgument );
	}
	else
	{
		if( completionString[0] == '\\' || completionString[0] == '/' )
			completionString++;

		matchCount = 0;
		shortestMatch[ 0 ] = 0;

		if( strlen( completionString ) == 0 )
		{
			Cmd_EndTokenizedString( );
			return;
		}

		if( doCommands )
			Cmd_CommandCompletion( FindMatches, completionString );
		if( doCvars )
			Cvar_CommandCompletion( FindMatches );

		if( !Field_Complete( ) )
		{
			// run through again, printing matches
			if( doCommands )
				Cmd_CommandCompletion( PrintMatches, completionString );
			if( doCvars )
				Cvar_CommandCompletion( PrintCvarMatches );
		}
	}
	Cmd_EndTokenizedString( );
}
/*
===============
Field_AutoComplete
Perform Tab expansion
===============
*/
void Field_AutoComplete( field_t *field )
{
	completionField = field;
	Field_CompleteCommand( completionField->buffer, qtrue, qtrue );
}
