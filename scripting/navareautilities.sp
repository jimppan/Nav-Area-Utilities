#pragma semicolon 1

#define DEBUG

#define PLUGIN_AUTHOR "Rachnus, pelipoika"

#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <navareautilities>

#pragma newdecls required

int g_iPathLaserModelIndex;

Address g_pNavAreas;
Address g_pNavAreaCount;

Handle g_hOnNavAreasLoaded;

public Plugin myinfo = 
{
	name = "Nav Area Utilities CSGO v1.01",
	author = PLUGIN_AUTHOR,
	description = "Nav Area Utilities for CSGO",
	version = NAU_VERSION,
	url = "https://github.com/Rachnus"
};

public void OnPluginStart()
{
	if(GetEngineVersion() != Engine_CSGO)
		SetFailState("This plugin is for CSGO only.");
		
	g_hOnNavAreasLoaded = CreateGlobalForward("NAU_OnNavAreasLoaded", ET_Ignore);
	
	RegAdminCmd("sm_naucount", Command_Count, ADMFLAG_ROOT);
	RegAdminCmd("sm_nauarea", Command_Area, ADMFLAG_ROOT);
#if defined DEBUG
	RegAdminCmd("sm_nautest", Command_Test, ADMFLAG_ROOT);
#endif
}

public APLRes AskPluginLoad2(Handle hMyself, bool bLate, char[] sError, int err_max)
{
	CreateNative("NAU_GetNavAreaCount", Native_GetNavAreaCount);
	CreateNative("NAU_GetNavAreaAddressByIndex", Native_GetNavAreaAddressByIndex);
	
	RegPluginLibrary("navareautilities");
	return APLRes_Success;
}

public int Native_GetNavAreaCount(Handle plugin, int numParams)
{
	return GetNavAreaCount();
}

public int Native_GetNavAreaAddressByIndex(Handle plugin, int numParams)
{
	return view_as<int>(GetNavAreaAddressByIndex(GetNativeCell(1)));
}

#if defined DEBUG

public Action Command_Test(int client, int args)
{
	CNavArea navArea = NAU_GetClientLastKnownNavArea(client);
	if(navArea.IsNullPointer())
		return Plugin_Handled;
		
	PrintToServer("CLIENT: 0x%X", GetEntityAddress(client));
	PrintToServer("0x%X", view_as<Address>(navArea));
	PrintToServer("UP: %d", navArea.GetNeighbourCount(NAVDIR_UP));
	PrintToServer("DOWN: %d", navArea.GetNeighbourCount(NAVDIR_DOWN));
	
	NAU_DebugNavArea(client, navArea, g_iPathLaserModelIndex);
	NAU_DebugNavAreaNeighbours(client, navArea, g_iPathLaserModelIndex);

	return Plugin_Handled;
}

#endif

public Action Command_Area(int client, int args)
{
	if(args < 1)
	{
		CNavArea navArea = NAU_GetClientLastKnownNavArea(client);
		if(!navArea.IsNullPointer())
			NAU_DebugNavArea(client, navArea, g_iPathLaserModelIndex);
		return Plugin_Handled;
	}
	
	char arg[65], arg2[65];
	GetCmdArg(1, arg, sizeof(arg));
	GetCmdArg(2, arg2, sizeof(arg2));
	
	int navAreaIndex = StringToInt(arg);
	int navCount = GetNavAreaCount();
	
	if(navAreaIndex >= navCount || navAreaIndex < 0)
	{
		ReplyToCommand(client, "%s Enter a nav area index between \x040\x09 and \x04%d", NAU_PREFIX, navCount);
		return Plugin_Handled;
	}
	
	CNavArea navArea = GetNavAreaAddressByIndex(navAreaIndex);
	
	NAU_DebugNavArea(client, navArea, g_iPathLaserModelIndex);
	if(StringToInt(arg2) == 1)
		NAU_DebugNavAreaNeighbours(client, navArea, g_iPathLaserModelIndex);

	return Plugin_Handled;
}

public Action Command_Count(int client, int args)
{
	PrintToChat(client, "%s Nav area count: \x04%d", NAU_PREFIX, GetNavAreaCount());
	return Plugin_Handled;
}

public int GetNavAreaCount()
{
	return LoadFromAddress(g_pNavAreaCount, NumberType_Int32);
}

public CNavArea GetNavAreaAddressByIndex(int navAreaIndex)
{
	return view_as<CNavArea>(LoadFromAddress(g_pNavAreas + view_as<Address>(4 * navAreaIndex), NumberType_Int32));
}

public void OnMapStart()
{
	g_iPathLaserModelIndex = PrecacheModel("materials/sprites/laserbeam.vmt");

	NAU_Initialize(g_pNavAreaCount, g_pNavAreas);

	Call_StartForward(g_hOnNavAreasLoaded);
	Call_Finish();
}
