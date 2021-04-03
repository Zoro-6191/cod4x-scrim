#include "g_shared.h"
#include "cmd.h"
// #include "cvar.h"
// #include "qcommon_io.h"
// #include "g_sv_shared.h"
#include "scr_vm.h"
#include "sv_snapshot.h"

/*
==================
CheatsOk
==================
*/
int __cdecl CheatsOk(gentity_t *ent)
{
    const char *v1;
    int result;
    const char *v3;
    bool bCreateFX;

    if ( g_cheats->boolean )
    {
        bCreateFX = 0;
        if ( Cvar_FindVar("createfx") )
            bCreateFX = 1;
        if ( ent->health > 0 || bCreateFX )
            result = 1;
        else
        {
            v3 = va("%c \"GAME_MUSTBEALIVECOMMAND\"", 101);
            SV_GameSendServerCommand(ent - g_entities, 0, v3);
            result = 0;
        }
    }
    else
    {
        v1 = va("%c \"GAME_CHEATSNOTENABLED\"", 101);
        SV_GameSendServerCommand(ent - g_entities, 0, v1);
        result = 0;
    }
    return result;
}

void __cdecl Cmd_DemiGod_f(gentity_t *ent)
{
  const char *msg;

  if ( CheatsOk(ent) )
  {
    ent->flags ^= 2u;
    if ( ent->flags & 2 )
    {
      msg = "GAME_DEMI_GODMODE_ON";
    }
    else
    {
      msg = "GAME_DEMI_GODMODE_OFF";
    }
    SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, msg));
  }
}

/*
==================
Cmd_God_f
Sets client to godmode
argv(0) god
==================
*/
void __cdecl Cmd_God_f(gentity_t *ent)
{
    const char *msg;

    if ( CheatsOk(ent) )
    {
        ent->flags ^= 1u;
        if ( ent->flags & 1 )
            msg = "GAME_GODMODE_ON";
        else msg = "GAME_GODMODE_OFF";
        SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, msg));
    }
}

/*
==================
Cmd_Notarget_f
Sets client to notarget
argv(0) notarget
==================
*/
void __cdecl Cmd_Notarget_f(gentity_t *ent)
{
    const char *msg;

    if ( CheatsOk(ent) )
    {
        ent->flags ^= 4u;
        if ( ent->flags & 4 )
            msg = va("%c \"%s\"", 101, "GAME_NOTARGETON");
        else msg = va("%c \"%s\"", 101, "GAME_NOTARGETOFF");
        SV_GameSendServerCommand(ent - g_entities, 0, msg);
    }
}

void __cdecl Cmd_UFO_f(gentity_t *ent)
{
    const char *v1; // eax@3

    if ( CheatsOk(ent) )
    {
        if ( ent->client->ufo )
        {
            ent->client->ufo = qfalse;
            v1 = va("%c \"%s\"", 101, "GAME_UFOOFF");
        }
        else
        {
            ent->client->ufo = qtrue;
            v1 = va("%c \"%s\"", 101, "GAME_UFOON");
        }
        SV_GameSendServerCommand(ent - g_entities, 0, v1);
    }
}

/*
==================
Cmd_Noclip_f
argv(0) noclip
==================
*/
void __cdecl Cmd_Noclip_f(gentity_t *ent)
{
    const char *v1;

    if ( CheatsOk(ent) )
    {
        if ( ent->client->noclip )
        {
            ent->client->noclip = qfalse;
            v1 = va("%c \"%s\"", 101, "GAME_NOCLIPOFF");
        }
        else
        {
            ent->client->noclip = qtrue;
            v1 = va("%c \"%s\"", 101, "GAME_NOCLIPON");
        }
        SV_GameSendServerCommand(ent - g_entities, 0, v1);
    }
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent ) 
{
    if(ent->client->sess.sessionState != SESS_STATE_PLAYING || !CheatsOk(ent))
        return;
    ent->flags &= 0xFFFFFFFC;
    ent->health = 0;
    ent->client->ps.stats[0] = 0;
    player_die(ent, ent, ent, 100000, 13, 0, 0, HITLOC_NONE, 0);
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent ) 
{
    char s[64];
    Com_sprintf(s, sizeof(s), "(%g %g %g)", ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]);
    SV_GameSendServerCommand( ent - g_entities, 0, va("%c \"\x15%s\n\"", 'e', s) );
}


void __cdecl Scr_PlayerVote(gentity_t *self, char *option)
{
    Scr_AddString(option);
    Scr_Notify(self, scr_const.vote, 1u);
}

void __cdecl Cmd_Vote_f(gentity_t *ent)
{
    const char *msg;

    if ( g_oldVoting->boolean )
    {
        if ( !level.voteTime )
        {
            SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_NOVOTEINPROGRESS\"", 'e'));
            return;
        }
        if ( ent->client->ps.eFlags & 0x100000 )
        {
            SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTEALREADYCAST\"", 'e'));
            return;
        }

        SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTECAST\"", 'e'));
        ent->client->ps.eFlags |= 0x100000u;
    }
    msg = SV_Cmd_Argv(1);
    if ( msg[0] != 'y' && msg[1] != 'Y' && msg[1] != '1' )
    {
        if ( g_oldVoting->boolean )
            SV_SetConfigstring(CS_VOTE_NO, va("%i", ++level.voteNo));
        else Scr_PlayerVote(ent, "no");
    }
    else if ( g_oldVoting->boolean )
        SV_SetConfigstring(CS_VOTE_YES, va("%i", ++level.voteYes));
    else Scr_PlayerVote(ent, "yes");
}


/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) 
{
    vec3_t origin, angles;
    int i;

    if ( !g_cheats->boolean ) 
    {
        SV_GameSendServerCommand( ent - g_entities, 0, va("%c \"GAME_CHEATSNOTENABLED\"", 101) );
        return;
    }
    if ( SV_Cmd_Argc() < 4 || SV_Cmd_Argc() > 6 )
    {
        SV_GameSendServerCommand( ent - g_entities, 0, va("%c \"GAME_USAGE\x15: setviewpos x y z yaw\"", 101) );
        return;
    }

    VectorClear( angles );
    for ( i = 0 ; i < 3 ; i++ )
        origin[i] = atof( SV_Cmd_Argv( i + 1 ) );

    angles[YAW] = atof( SV_Cmd_Argv( 4 ) );
    angles[PITCH] = atof( SV_Cmd_Argv( 5 ) );

    TeleportPlayer( ent, origin, angles );
}

/*
==============
Cmd_EntityCount_f
==============
*/
void Cmd_EntityCount_f( gentity_t *ent ) 
{
    if ( !g_cheats->integer )
        return;

    Com_Printf(CON_CHANNEL_DONT_FILTER, "entity count = %i\n", level.num_entities );
}

void Cmd_PrintEntities_f()
{
    G_PrintEntities();
}

void __cdecl Cmd_Say_f(gentity_t *ent, int mode, int arg0)
{
    char *p;
    char b[1024];

    if ( SV_Cmd_Argc() >= 2 || arg0 )
    {
        if ( arg0 )
            p = SV_Cmd_Argsv(0, b, sizeof(b));
        else p = SV_Cmd_Argsv(1, b, sizeof(b));
        G_Say(ent, 0, mode, p);
    }
}

void __cdecl Cmd_MenuResponse_f(gentity_t *pEnt)
{
    char szServerId[1024];
    char szMenuName[1024];
    int iMenuIndex;
    char szResponse[1024];

    iMenuIndex = -1;
    if ( SV_Cmd_Argc() == 4 )
    {
        SV_Cmd_ArgvBuffer(1, szServerId, 1024);
        if ( atoi(szServerId) != Cvar_GetInt("sv_serverId"))
            return;

        SV_Cmd_ArgvBuffer(2, szMenuName, 1024);
        iMenuIndex = atoi(szMenuName);
        if ( iMenuIndex >= 0 && iMenuIndex < 32 )
            SV_GetConfigstring(iMenuIndex + 1970, szMenuName, 1024);
        SV_Cmd_ArgvBuffer(3, szResponse, 1024);
    }
    else
    {
        szMenuName[0] = 0;
        strcpy(szResponse, "bad");
    }
    Scr_AddString(szResponse);
    Scr_AddString(szMenuName);
    Scr_Notify(pEnt, scr_const.menuresponse, 2u);
}

void Cmd_Score_f(gentity_t* ent)
{
    SendScoreboard(ent);
}


int __cdecl Cmd_FollowCycle_f(gentity_t *ent, int dir)
{
    int i;
    int health;
    int clientNum;
    int otherFlags;
    struct clientState_s archcs;
    struct playerState_s ps;

    if ( dir != 1 && dir != -1 )
    {
      Com_Error(ERR_DROP, "Cmd_FollowCycle_f: bad dir %i", dir);
    }

    assert ( ent->client != NULL);

    if ( ent->client->sess.sessionState != SESS_STATE_SPECTATOR )
        return qfalse;

    ent->client->lastFollowedClient = -1; //Clear this so we never jump back onto old player
    if ( ent->client->sess.forceSpectatorClient < 0 )
    {
        clientNum = ent->client->spectatorClient;
        if ( clientNum < 0 )
            clientNum = 0;

        i = clientNum;
        do
        {
          clientNum += dir;
          if ( clientNum >= level.maxclients )
              clientNum = 0;
          if ( clientNum < 0 )
              clientNum = level.maxclients - 1;

          if ( SV_GetArchivedClientInfo(clientNum, &ent->client->sess.archiveTime, &ps, &archcs, 0, &health, &otherFlags) )
          {
              assert(otherFlags & POF_PLAYER);
              if ( G_ClientCanSpectateTeamOrLocalPlayer(ent->client, &archcs) )
              {
                  ent->client->spectatorClient = clientNum;
                  ent->client->sess.sessionState = SESS_STATE_SPECTATOR;
                  return qtrue;
              }
          }
        }
        while ( clientNum != i );
    }
    return qfalse;
}

void __cdecl StopFollowing(gentity_t *ent)
{
    gclient_t *client;
    vec3_t vAngles;
    vec3_t vEnd;
    vec3_t vMins;
    trace_t trace;
    vec3_t vForward;
    vec3_t vPos;
    vec3_t vUp;
    vec3_t vMaxs;

    client = ent->client;

    assert(client != NULL);

    client->sess.forceSpectatorClient = -1;
    client->sess.killCamEntity = -1;
    client->spectatorClient = -1;
    if ( client->ps.otherFlags & 2 )
    {
        G_GetPlayerViewOrigin(&client->ps, vPos);
        BG_GetPlayerViewDirection(&client->ps, vForward, 0, vUp);
        vAngles[0] = client->ps.viewangles[0];
        vAngles[1] = client->ps.viewangles[1];
        vAngles[2] = client->ps.viewangles[2];
        vAngles[0] = vAngles[0] + 15.0;
        vEnd[0] = (float)(-40.0 * vForward[0]) + vPos[0];
        vEnd[1] = (float)(-40.0 * vForward[1]) + vPos[1];
        vEnd[2] = (float)(-40.0 * vForward[2]) + vPos[2];
        vEnd[0] = (float)(10.0 * vUp[0]) + vEnd[0];
        vEnd[1] = (float)(10.0 * vUp[1]) + vEnd[1];
        vEnd[2] = (float)(10.0 * vUp[2]) + vEnd[2];
        vMins[0] = -8.0;
        vMins[1] = -8.0;
        vMins[2] = -8.0;
        vMaxs[0] = 8.0;
        vMaxs[1] = 8.0;
        vMaxs[2] = 8.0;
        G_TraceCapsule(&trace, vPos, vMins, vMaxs, vEnd, 1023, 0x810011/*, &context*/);
        Vec3Lerp(vPos, vEnd, trace.fraction, vPos);
        client->ps.clientNum = ent - g_entities;

    //Not in Blackops
        client->ps.eFlags &= 0xFFFFFCFF;
        client->ps.viewlocked = 0;
        client->ps.viewlocked_entNum = 1023;
    ///
        client->ps.pm_flags &= 0xFFFEFFEF;
        client->ps.weapFlags &= 0xFFFFFFBF;
        client->ps.otherFlags &= 0xFFFFFFFD;
        client->ps.fWeaponPosFrac = 0.0;
        G_SetOrigin(ent, vPos);
        client->ps.origin[0] = vPos[0];
        client->ps.origin[1] = vPos[1];
        client->ps.origin[2] = vPos[2];
        SetClientViewAngle(ent, vAngles);
        if ( !ent->tagInfo )
            ent->r.currentAngles[0] = 0.0;

        client->ps.shellshockIndex = 0;
        client->ps.shellshockTime = 0;
        client->ps.shellshockDuration = 0;
    }
}

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum ) 
{
	gentity_t *ent;
	char cmd[MAX_TOKEN_CHARS];

	ent = g_entities + clientNum;
	if ( !ent->client )
		return;     // not fully in game yet

	SV_Cmd_ArgvBuffer( 0, cmd, sizeof( cmd ) );

	if ( Q_stricmp( cmd, "say" ) == 0 ) 
    {
		Cmd_Say_f( ent, SAY_ALL, qfalse );
		return;
	}
	if ( Q_stricmp( cmd, "say_team" ) == 0 ) 
    {
		Cmd_Say_f( ent, SAY_TEAM, qfalse );
		return;
	}
	// -NERVE - SMF

	if ( Q_stricmp( cmd, "tell" ) == 0 ) 
		return;

	if ( Q_stricmp( cmd, "score" ) == 0 ) 
    {
		Cmd_Score_f( ent );
		return;
	}
	if ( Q_stricmp( cmd, "mr" ) == 0 ) 
    {
		Cmd_MenuResponse_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "give" ) == 0 ) 
    {
		Cmd_Give_f( ent );
	} 
    else if ( Q_stricmp( cmd, "god" ) == 0 )  
    {
		Cmd_God_f( ent );
	} 
    else if ( Q_stricmp( cmd, "demigod" ) == 0 )  
    {
		Cmd_DemiGod_f( ent );
	} 
    else if ( Q_stricmp( cmd, "take" ) == 0 )  
    {
		Cmd_Take_f( ent );
	} 
    else if ( Q_stricmp( cmd, "notarget" ) == 0 )  
    {
		Cmd_Notarget_f( ent );
	} 
    else if ( Q_stricmp( cmd, "noclip" ) == 0 )  
    {
		Cmd_Noclip_f( ent );
	} 
    else if ( Q_stricmp( cmd, "ufo" ) == 0 )  
    {
		Cmd_UFO_f( ent );
	} 
    else if ( Q_stricmp( cmd, "kill" ) == 0 )  
    {
		Cmd_Kill_f( ent );
	} 
    else if ( Q_stricmp( cmd, "where" ) == 0 ) 
    {
		Cmd_Where_f( ent );
	} 
    else if ( Q_stricmp( cmd, "callvote" ) == 0 )  
    {
		Cmd_CallVote_f( ent );
	} 
    else if ( Q_stricmp( cmd, "vote" ) == 0 )  
    {
		Cmd_Vote_f( ent );
	} 
    else if ( Q_stricmp( cmd, "setviewpos" ) == 0 ) 
    {
		Cmd_SetViewpos_f( ent );
	} 
    else if ( Q_stricmp( cmd, "entitycount" ) == 0 )  
    {
		Cmd_EntityCount_f( ent );
	} 
    else if ( Q_stricmp( cmd, "printentities" ) == 0 )  
    {
		Cmd_PrintEntities_f( ent );
	} 
    else SV_GameSendServerCommand( clientNum, 0, va( "%c \"GAME_UNKNOWNCLIENTCOMMAND\x15%s\"", 101, cmd ) );
}