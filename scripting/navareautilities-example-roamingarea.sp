#pragma semicolon 1

#define DEBUG

#define PLUGIN_AUTHOR "Rachnus"
#define PLUGIN_VERSION "1.0"

#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <navareautilities>

#pragma newdecls required

int g_iPathLaserModelIndex;

Address g_pCurrentArea;

int g_iTimesToRun;

public Plugin myinfo = 
{
	name = "Nav Area Utilities CSGO Example - Roaming Area v1.0",
	author = PLUGIN_AUTHOR,
	description = "Nav Area Utilities Example",
	version = PLUGIN_VERSION,
	url = "https://github.com/Rachnus"
};

public void OnPluginStart()
{
	if(GetEngineVersion() != Engine_CSGO )
		SetFailState("This plugin is for CSGO only.");	
		
	RegAdminCmd("sm_nauroam", Command_Roam, ADMFLAG_ROOT);
}

public Action Command_Roam(int client, int args)
{
	g_pCurrentArea = NAU_GetClientLastKnownNavArea(client);
	if(g_pCurrentArea == Address_Null)
		return Plugin_Handled;
		
	g_iTimesToRun = 100;
	CreateTimer(0.2, Timer_Roam, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
	return Plugin_Handled;
}

public Action Timer_Roam(Handle timer, any data)
{
	if(g_iTimesToRun-- <= 0)
		return Plugin_Stop;
	float pos[3];
	
	if(NAU_IsNavLadder(g_pCurrentArea))
		NAU_GetNavLadderCenter(g_pCurrentArea, pos);
	else
		NAU_GetNavAreaCenter(g_pCurrentArea, pos);
	
	float editPos[3];  
	editPos = pos;
	
	editPos[0] += GetRandomFloat(-1000.0, 1000.0);
	editPos[1] += GetRandomFloat(-1000.0, 1000.0);
	
	if(!NAU_IsNavLadder(g_pCurrentArea))
	{
		if(NAU_GetNeighbourNavAreaCount(g_pCurrentArea, NAVDIR_UP) > 0)
		{
			editPos[2] += GetRandomFloat(-1000.0, 1000.0);
		}
		else if(NAU_GetNeighbourNavAreaCount(g_pCurrentArea, NAVDIR_DOWN) > 0)
		{
			editPos[2] += GetRandomFloat(-1000.0, 1000.0);
		}
	}
	
	g_pCurrentArea = NAU_GetClosestNeighbourNavArea(g_pCurrentArea, editPos);
	if(g_pCurrentArea == Address_Null)
		return Plugin_Stop;
		
	NAU_DebugNavArea(-1, g_pCurrentArea, g_iPathLaserModelIndex);
	return Plugin_Continue;
}


public void OnMapStart()
{
	g_iPathLaserModelIndex = PrecacheModel("materials/sprites/laserbeam.vmt");
}