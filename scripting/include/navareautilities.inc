#if defined _navareasapi_included
 #endinput
#endif
#define _navareasapi_included

#define NAU_PREFIX " \x09[\x04Nav UTIL\x09]"

#define NAU_GAMEDATA "navareautilities.gamedata"

#define NAU_NAVAREA_SOUTH_NAV_CONNECT_VECTOR 0x5C

#define NAU_NAVAREA_LADDER_INDICATOR 0x34

#define NAU_NAVAREA_LADDER_HEIGHT 0x18
#define NAU_NAVAREA_LADDER_WIDTH 0x1C

#define NAU_NAVAREA_LADDER_NAVAREAS 0x20

enum NavDirType
{
	NAVDIR_SOUTH = 			0,
	NAVDIR_EAST = 			1,
	NAVDIR_NORTH = 			2,
	NAVDIR_WEST = 			3,
	NAVDIR_UP =				4,
	NAVDIR_DOWN =			5,
	
	NAVDIR_MAX
}

enum NavLadderDestination
{
	NAVLADDER_TOP_FORWARD = 0,
	NAVLADDER_TOP_LEFT = 	1,
	NAVLADDER_TOP_RIGHT = 	2,
	NAVLADDER_TOP_BEHIND = 	3,
	NAVLADDER_BOTTOM =		4,
	
	NAVLADDER_MAX
}

/**
 * Returns amount of nav areas
 * @return int 		amount of nav areas
 */
native int NAU_GetNavAreaCount();

/**
 * Returns an address of a navareaindex
 * @return address 		address of nav area
 */
native Address NAU_GetNavAreaAddressByIndex(int navAreaIndex);

/**
 * Called when nav areas are loaded by plugin (OnMapStart)
 * @param amount of nav areas found
 * @noreturn
 */
forward void NAU_OnNavAreasLoaded();

/**
 * Returns the center position of the nav area
 * @param address of the nav area
 * @param buffer to store the position
 * @return void
 */
public void NAU_GetNavAreaCenter(Address navAreaAddress, float result[3])
{
	float nwCorner[3], seCorner[3];
	NAU_GetNavAreaNorthWestCorner(navAreaAddress, nwCorner);
	NAU_GetNavAreaSouthEastCorner(navAreaAddress, seCorner);
	
	NAU_GetMiddleOfABox(nwCorner, seCorner, result);
}

/**
 * Get the center position of the nav ladder area
 * @param address of the nav area to check
 * @param buffer to store the position
 * @return void
 */
public void NAU_GetNavLadderCenter(Address navLadderAddress, float result[3])
{
	float nwCorner[3], seCorner[3];
	NAU_GetNavLadderTop(navLadderAddress, seCorner);
	NAU_GetNavLadderBottom(navLadderAddress, nwCorner);
	
	NAU_GetMiddleOfABox(nwCorner, seCorner, result);
}

/**
 * Returns wether or not the entity has larger X/Y values than the nav area
 * @param address of the nav area
 * @param entity index
 * @return bool		can entity fit in nav area
 */
public bool NAU_CanEntityFitInNavArea(Address navAreaAddress, int entity)
{
	float mid[3], vMins[3], vMaxs[3];
	GetEntPropVector(entity, Prop_Data, "m_vecMins", vMins);
	GetEntPropVector(entity, Prop_Data, "m_vecMaxs", vMaxs);
	
	mid[0] /= 2.0;
	mid[1] /= 2.0;
	mid[2] /= 2.0;
	
	if(mid[0] < 0.0) mid[0] *= -1;
	if(mid[1] < 0.0) mid[1] *= -1;
	if(mid[2] < 0.0) mid[2] *= -1;
	
	float nwCorner[3], seCorner[3], navAreaMid[3];
	NAU_GetNavAreaNorthWestCorner(navAreaAddress, nwCorner);
	NAU_GetNavAreaSouthEastCorner(navAreaAddress, seCorner);
	
	MakeVectorFromPoints(seCorner, nwCorner, navAreaMid);
	
	navAreaMid[0] /= 2.0;
	navAreaMid[1] /= 2.0;
	navAreaMid[2] /= 2.0;
	
	if(navAreaMid[0] < 0.0) navAreaMid[0] *= -1;
	if(navAreaMid[1] < 0.0) navAreaMid[1] *= -1;
	if(navAreaMid[2] < 0.0) navAreaMid[2] *= -1;
	
	return (mid[0] <= navAreaMid[0] && mid[1] <= navAreaMid[1]);
}

/**
 * Returns wether or not the entity has larger X/Y values than the nav area
 * @param address of the nav area
 * @param Mins of box
 * @param Maxs of box
 * @return bool		can entity fit in nav area
 */
public bool NAU_CanFitInNavArea(Address navAreaAddress, float vMins[3], float vMaxs[3])
{
	float mid[3];
	MakeVectorFromPoints(vMins, vMaxs, mid);
	mid[0] /= 2.0;
	mid[1] /= 2.0;
	
	if(mid[0] < 0.0) mid[0] *= -1;
	if(mid[1] < 0.0) mid[1] *= -1;
	
	float nwCorner[3], seCorner[3], navAreaMid[3];
	NAU_GetNavAreaNorthWestCorner(navAreaAddress, nwCorner);
	NAU_GetNavAreaSouthEastCorner(navAreaAddress, seCorner);
	
	MakeVectorFromPoints(seCorner, nwCorner, navAreaMid);
	
	navAreaMid[0] /= 2.0;
	navAreaMid[1] /= 2.0;
	
	if(navAreaMid[0] < 0.0) navAreaMid[0] *= -1;
	if(navAreaMid[1] < 0.0) navAreaMid[1] *= -1;
	
	return (mid[0] <= navAreaMid[0] && mid[1] <= navAreaMid[1]);
}

/**
 * Get a random position within a nav area, returns false if the mins/maxs are bigger than the area
 * @param address of the nav area
 * @param Mins of entity you want to fit in the area
 * @param Maxs of entity you want to fit in the area
 * @param buffer to store the position
 * @return bool		can entity fit in nav area
 */
public bool NAU_GetRandomPosWithinNavArea(Address navAreaAddress, float vMins[3], float vMaxs[3], float result[3])
{
	// To stop random crashes if someone were to do stuff on a navladder
	if(NAU_IsNavLadder(navAreaAddress))
		return false;
		
	bool returnVal = true;
	float mid[3];
	MakeVectorFromPoints(vMins, vMaxs, mid);
	mid[0] /= 2.0;
	mid[1] /= 2.0;
	
	if(mid[0] < 0.0) mid[0] *= -1;
	if(mid[1] < 0.0) mid[1] *= -1;
	
	float nwCorner[3], seCorner[3], navAreaMid[3];
	NAU_GetNavAreaNorthWestCorner(navAreaAddress, nwCorner);
	NAU_GetNavAreaSouthEastCorner(navAreaAddress, seCorner);
	
	MakeVectorFromPoints(seCorner, nwCorner, navAreaMid);
	
	navAreaMid[0] /= 2.0;
	navAreaMid[1] /= 2.0;
	
	if(navAreaMid[0] < 0.0) navAreaMid[0] *= -1;
	if(navAreaMid[1] < 0.0) navAreaMid[1] *= -1;
	
	if(mid[0] > navAreaMid[0] || mid[1] > navAreaMid[1])
		returnVal = false;
	
	// Add/Subtract half of the size to the random pos (To make the entity fit properly)
	result[0] = GetRandomFloat(nwCorner[0] + mid[0], seCorner[0] - mid[0]);
	result[1] = GetRandomFloat(nwCorner[1] + mid[1], seCorner[1] - mid[1]);
	
	// Set the position to the highest point (TODO: Add function to calculate the height of the slope at a certain point of the plane)
	result[2] = seCorner[2];
	
	return returnVal;
}

/**
 * Get the difference in Z positions of the nav area (Used to check if its a slope or not, returns 0 if plane surface)
 * @param address of the nav area
 * @return float		Z position difference
 */
public float NAU_GetNavAreaZDifference(Address navAreaAddress)
{
	float nwCorner[3], seCorner[3];
	NAU_GetNavAreaNorthWestCorner(navAreaAddress, nwCorner);
	NAU_GetNavAreaSouthEastCorner(navAreaAddress, seCorner);
	
	return seCorner[2] - nwCorner[2];
}

/**
 * Returns the north west corner of the nav area
 * @param address of the nav area
 * @param buffer to store the position
 * @return void
 */
public void NAU_GetNavAreaNorthWestCorner(Address navAreaAddress, float result[3])
{
	result[0] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(4), NumberType_Int32));
	result[1] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(8), NumberType_Int32));
	result[2] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(12), NumberType_Int32));
}

/**
 * Returns the south east corner of the nav area
 * @param address of the nav area
 * @param buffer to store the position
 * @return void
 */
public void NAU_GetNavAreaSouthEastCorner(Address navAreaAddress, float result[3])
{
	result[0] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(16), NumberType_Int32));
	result[1] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(20), NumberType_Int32));
	result[2] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(24), NumberType_Int32));
}

/**
 * Get the top left position of the nav ladder area
 * @param address of the ladder nav area
 * @param buffer to store the position
 * @return void
 */
public void NAU_GetNavLadderTop(Address navAreaAddress, float result[3])
{
	result[0] = view_as<float>(LoadFromAddress(navAreaAddress, NumberType_Int32));
	result[1] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(4), NumberType_Int32));
	result[2] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(8), NumberType_Int32));
}

/**
 * Get the bottom right position of the nav ladder area
 * @param address of the nav area to check
 * @param buffer to store the position
 * @return void
 */
public void NAU_GetNavLadderBottom(Address navAreaAddress, float result[3])
{
	result[0] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(12), NumberType_Int32));
	result[1] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(16), NumberType_Int32));
	result[2] = view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(20), NumberType_Int32));
}

/**
 * Get the height of the ladder
 * @param address of the nav ladder to check
 * @return float	height of ladder
 */
public float NAU_GetNavLadderHeight(Address navAreaAddress)
{
	return view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(NAU_NAVAREA_LADDER_HEIGHT), NumberType_Int32));
}

/**
 * Get the width of the ladder
 * @param address of the nav ladder to check
 * @return float	width of ladder
 */
public float NAU_GetNavLadderWidth(Address navAreaAddress)
{
	return view_as<float>(LoadFromAddress(navAreaAddress + view_as<Address>(NAU_NAVAREA_LADDER_WIDTH), NumberType_Int32));
}

/**
 * Get the amount of nav areas in direction
 * @param address of the nav area
 * @param direction
 * @return int		Amount of neighbours in direction
 */
public int NAU_GetNeighbourNavAreaCount(Address navAreaAddress, NavDirType direction)
{
	Address navConnectVector =	view_as<Address>(LoadFromAddress(navAreaAddress + view_as<Address>(NAU_NAVAREA_SOUTH_NAV_CONNECT_VECTOR + (0x4 * view_as<int>(direction))), NumberType_Int32));
	return LoadFromAddress(navConnectVector, NumberType_Int32);
}

/**
 * Get address of a neighbour nav area
 * @param address of the nav area
 * @param direction
 * @param neighbour index
 * @return address 		address of the neighbour nav area
 */
public Address NAU_GetNeighbourNavArea(Address navAreaAddress, NavDirType direction, int directionListIndex)
{
	Address navConnectVector =	view_as<Address>(LoadFromAddress(navAreaAddress + view_as<Address>(NAU_NAVAREA_SOUTH_NAV_CONNECT_VECTOR + (0x4 * view_as<int>(direction))), NumberType_Int32));
	return view_as<Address>(LoadFromAddress(navConnectVector + view_as<Address>(0x4 + (0x8 * view_as<int>(directionListIndex))), NumberType_Int32));
}

/**
 * Get address of a destination nav area from a ladder (check NavLadderDestination)
 * @param address of the nav ladder
 * @param ladder destination
 * @return address 		address of the destination nav area (Address_Null if invalid navarea)
 */
public Address NAU_GetDestinationNavAreaFromLadder(Address navLadderAddress, NavLadderDestination destination)
{
	return view_as<Address>(LoadFromAddress(navLadderAddress + view_as<Address>(NAU_NAVAREA_LADDER_NAVAREAS + (0x4 * view_as<int>(destination))), NumberType_Int32));
}

/**
 * Hacky way to find out if the nav area a CNavLadder or CNavArea
 * @param address of the nav area to check
 * @return bool 	if ladder area or not
 */
public bool NAU_IsNavLadder(Address navAddress)
{
	int hack = view_as<int>(LoadFromAddress(navAddress + view_as<Address>(NAU_NAVAREA_LADDER_INDICATOR), NumberType_Int32));
	return hack == -1;
}

/**
 * Get closes neighbour nav area (By checking their center positions)
 * @param address of the nav area to check
 * @param a position to check which nav area is closest
 * @return address 		address of the closest neighbour area
 */
public Address NAU_GetClosestNeighbourNavArea(Address navAreaAddress, float pos[3])
{
	Address closestNavArea = navAreaAddress;
	float startPos[3];
	//GetNavAreaCenter(navAreaAddress, startPos);
	if(!NAU_IsNavLadder(closestNavArea))
		NAU_GetNavAreaCenter(navAreaAddress, startPos);
	else
		NAU_GetNavLadderCenter(navAreaAddress, startPos);
		
	float closestDistance = GetVectorDistance(pos, startPos, true);
	bool gotfirst = false;
	if(NAU_IsNavLadder(navAreaAddress))
	{
		ArrayList ladderDestinations = new ArrayList();
		for (int i = 0; i < view_as<int>(NAVLADDER_MAX); i++)
		{
			Address destination = NAU_GetDestinationNavAreaFromLadder(navAreaAddress, view_as<NavLadderDestination>(i));
			if(destination != Address_Null)
				ladderDestinations.Push(destination);
		}
		
		Address destination = ladderDestinations.Get(GetRandomInt(0, ladderDestinations.Length - 1));
		
		float navPos[3];
		if(!NAU_IsNavLadder(destination))
			NAU_GetNavAreaCenter(destination, navPos);
		else
			NAU_GetNavLadderCenter(destination, navPos);

		closestNavArea = destination;
		delete ladderDestinations;
	}
	else
	{
		for (int i = 0; i < view_as<int>(NAVDIR_MAX); i++)
		{
			int neighbourCount = NAU_GetNeighbourNavAreaCount(navAreaAddress, view_as<NavDirType>(i));
			for (int j = 0; j < neighbourCount; j++)
			{
				Address neighbour = NAU_GetNeighbourNavArea(navAreaAddress, view_as<NavDirType>(i), j);
				
				float navPos[3];
				//GetNavAreaCenter(neighbour, navPos);
				
				if(!NAU_IsNavLadder(closestNavArea))
					NAU_GetNavAreaCenter(neighbour, navPos);
				else
					NAU_GetNavLadderCenter(neighbour, navPos);
	
				float dist = 0.0;
				if((dist = GetVectorDistance(navPos, pos, true)) < closestDistance || !gotfirst)
				{
					closestNavArea = neighbour;
					closestDistance = dist;
					gotfirst = true;
				}
			}
		}
	}
	
	return closestNavArea;
}

/**
 * Get the address of the clients last known nav area (Private hidden variable: offset 0x8D8 as of 7/31/2018)
 * @param client index
 * @return address 		address of the last known nav area (Address_Null if player has no last known nav area)
 */
public Address NAU_GetClientLastKnownNavArea(int client)
{
	// Make shit break less
	return view_as<Address>(GetEntData(client, FindSendPropInfo("CBaseCombatCharacter", "m_nRelativeDirectionOfLastInjury") + 0x8));
}

public void NAU_Initialize(Address& navCount, Address& navAreas)
{
	Handle hConf = LoadGameConfigFile(NAU_GAMEDATA);
	
	navCount = GameConfGetAddress(hConf, "navarea_count");
#if defined DEBUG
	PrintToServer("Found \"navarea_count\" @ 0x%X", navCount);
#endif
	navAreas = view_as<Address>(LoadFromAddress(navCount + view_as<Address>(0x4), NumberType_Int32));
#if defined DEBUG
	PrintToServer("Found \"TheNavAreas\" @ 0x%X", navAreas);
#endif
	delete hConf;
	
#if defined DEBUG
	int navAreaCount = NAU_GetNavAreaCount();
	PrintToServer("Nav area count: %d", navAreaCount);
#endif
}

public void NAU_GetMiddleOfABox(const float vec1[3], const float vec2[3], float result[3])
{
	float mid[3];
	MakeVectorFromPoints(vec1, vec2, mid);
	mid[0] /= 2.0;
	mid[1] /= 2.0;
	mid[2] /= 2.0;
	AddVectors(vec1, mid, result);
}

public bool NAU_IsPositionBlocked(float pos[3], float vMins[3], float vMaxs[3])
{
	Handle ray = TR_TraceHullFilterEx(pos, pos, vMins, vMaxs, MASK_PLAYERSOLID, NAU_TraceFilterNothing);
	if(TR_DidHit(ray))
		return false;
	return true;
}

public bool NAU_TraceFilterNothing(int entityhit, int mask, any entity)
{
	if(entityhit > -1)
		return true;
	
	return false;
}

public bool NAU_IsPositionBlockedIgnoreSelf(float pos[3], float vMins[3], float vMaxs[3], int entity)
{
	Handle ray = TR_TraceHullFilterEx(pos, pos, vMins, vMaxs, MASK_PLAYERSOLID, NAU_TraceFilterIgnoreSelf, entity);
	if(TR_DidHit(ray))
		return false;
	return true;
}

public bool NAU_TraceFilterIgnoreSelf(int entityhit, int mask, any entity)
{
	if(entityhit > -1 && entityhit != entity)
		return true;
	
	return false;
}

public void NAU_PrintVector(int client, char[] prefix, float pos[3])
{
	PrintToChat(client, "%s %s\x02%.2f \x04%.2f \x0C%.2f", NAU_PREFIX, prefix, pos[0], pos[1], pos[2]);
}

public void NAU_SendBox(float vMins[3], float vMaxs[3], int modelIndex, int color[4], float lifetime)
{
	float vPos1[3], vPos2[3], vPos3[3], vPos4[3], vPos5[3], vPos6[3];
	vPos1 = vMaxs;
	vPos1[0] = vMins[0];
	vPos2 = vMaxs;
	vPos2[1] = vMins[1];
	vPos3 = vMaxs;
	vPos3[2] = vMins[2];
	vPos4 = vMins;
	vPos4[0] = vMaxs[0];
	vPos5 = vMins;
	vPos5[1] = vMaxs[1];
	vPos6 = vMins;
	vPos6[2] = vMaxs[2];
	NAU_SendBeam(vMaxs, vPos1, modelIndex, color, lifetime);
	NAU_SendBeam(vMaxs, vPos2, modelIndex, color, lifetime);
	NAU_SendBeam(vMaxs, vPos3, modelIndex, color, lifetime);	//Vertical
	NAU_SendBeam(vPos6, vPos1, modelIndex, color, lifetime);
	NAU_SendBeam(vPos6, vPos2, modelIndex, color, lifetime);
	NAU_SendBeam(vPos6, vMins, modelIndex, color, lifetime);	//Vertical
	NAU_SendBeam(vPos4, vMins, modelIndex, color, lifetime);
	NAU_SendBeam(vPos5, vMins, modelIndex, color, lifetime);
	NAU_SendBeam(vPos5, vPos1, modelIndex, color, lifetime);	//Vertical
	NAU_SendBeam(vPos5, vPos3, modelIndex, color, lifetime);
	NAU_SendBeam(vPos4, vPos3, modelIndex, color, lifetime);
	NAU_SendBeam(vPos4, vPos2, modelIndex, color, lifetime);	//Vertical
}

public void NAU_SendBeam(const float vMins[3], const float vMaxs[3], int modelIndex, const int color[4], float lifetime)
{
	TE_SetupBeamPoints(vMins, vMaxs, modelIndex, modelIndex, 0, 0, lifetime, 1.0, 1.0, 1, 0.0, color, 0);
	TE_SendToAll();
}

public void NAU_DebugNavArea(int client, Address navAreaAddress, int laserModelIndex)
{
	float navAreaNW[3], navAreaSE[3], center[3];
	
	if(!NAU_IsNavLadder(navAreaAddress))
	{
		NAU_GetNavAreaNorthWestCorner(navAreaAddress, navAreaNW);
		NAU_GetNavAreaSouthEastCorner(navAreaAddress, navAreaSE);
		NAU_GetNavAreaCenter(navAreaAddress, center);
	}
	else
	{
		NAU_GetNavLadderTop(navAreaAddress, navAreaNW);
		NAU_GetNavLadderBottom(navAreaAddress, navAreaSE);
		NAU_GetNavLadderCenter(navAreaAddress, center);
	}

	if(client > 0 && client <= MaxClients && IsClientInGame(client))
	{
		NAU_PrintVector(client, "North West: ", navAreaNW);
		NAU_PrintVector(client, "South East: ", navAreaSE);
		NAU_PrintVector(client, "Center: ", center);
	}
	
	NAU_SendBox(navAreaSE, navAreaNW, laserModelIndex, { 255, 0, 0, 255 }, 5.0);
}


public void NAU_DebugNavAreaNeighbours(int client, Address navAreaAddress, int laserModelIndex)
{
	if(NAU_IsNavLadder(navAreaAddress))
	{
		for (int i = 0; i < view_as<int>(NAVLADDER_MAX); i++)
		{
			Address destination = NAU_GetDestinationNavAreaFromLadder(navAreaAddress, view_as<NavLadderDestination>(i));
			NAU_DebugNavArea(client, destination, laserModelIndex);
		}
	}
	else
	{
		for (int i = 0; i < view_as<int>(NAVDIR_MAX); i++)
		{
			int neighbourCount = NAU_GetNeighbourNavAreaCount(navAreaAddress, view_as<NavDirType>(i));
			for (int j = 0; j < neighbourCount; j++)
			{
				Address neighbour = NAU_GetNeighbourNavArea(navAreaAddress, view_as<NavDirType>(i), j);
				NAU_DebugNavArea(client, neighbour, laserModelIndex);
			}
		}
	}
}

public SharedPlugin __pl_navareautilities  =
{
	name = "navareautilities",
	file = "navareautilities.smx",
#if defined REQUIRE_PLUGIN
	required = 1
#else
	required = 0
#endif
};

#if !defined REQUIRE_PLUGIN
public __pl_navareautilities_SetNTVOptional()
{	
	MarkNativeAsOptional("NAU_GetNavAreaCount");
	MarkNativeAsOptional("NAU_GetNavAreaAddressByIndex");
}
#endif