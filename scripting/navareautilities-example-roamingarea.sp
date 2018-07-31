#pragma semicolon 1

#define DEBUG

#define PLUGIN_AUTHOR "Rachnus"

#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <navareautilities>

#pragma newdecls required

int g_iPathLaserModelIndex;

CNavArea g_pCurrentArea;

int g_iTimesToRun;

public Plugin myinfo = 
{
	name = "Nav Area Utilities CSGO Example - Roaming Area v1.01",
	author = PLUGIN_AUTHOR,
	description = "Nav Area Utilities Example",
	version = NAU_VERSION,
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
	if(g_pCurrentArea.IsNullPointer())
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
	
	if(g_pCurrentArea.IsNavLadder())
		g_pCurrentArea.GetCenter(pos);
	else
	{
		CNavLadder ladder = view_as<CNavLadder>(g_pCurrentArea);
		ladder.GetCenter(pos);
	}

	pos[0] += GetRandomFloat(-1000.0, 1000.0);
	pos[1] += GetRandomFloat(-1000.0, 1000.0);
	
	if(!g_pCurrentArea.IsNavLadder())
	{
		if(g_pCurrentArea.GetNeighbourCount(NAVDIR_UP) > 0)
		{
			pos[2] += GetRandomFloat(-1000.0, 1000.0);
		}
		else if(g_pCurrentArea.GetNeighbourCount(NAVDIR_DOWN) > 0)
		{
			pos[2] += GetRandomFloat(-1000.0, 1000.0);
		}
	}
	
	g_pCurrentArea = NAU_GetClosestNavAreaNeighbour(g_pCurrentArea, pos);
	if(g_pCurrentArea.IsNullPointer())
		return Plugin_Stop;
		
	NAU_DebugNavArea(-1, g_pCurrentArea, g_iPathLaserModelIndex);
	return Plugin_Continue;
}


public void OnMapStart()
{
	g_iPathLaserModelIndex = PrecacheModel("materials/sprites/laserbeam.vmt");
}