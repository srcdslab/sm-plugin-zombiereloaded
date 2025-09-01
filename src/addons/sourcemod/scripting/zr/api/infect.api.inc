/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infect.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Infect module)
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * @section Global forward handles.
 */
GlobalForward g_hAPIFwdOnInfectCountdown = null;
GlobalForward g_hAPIFwdOnClientInfect = null;
GlobalForward g_hAPIFwdOnClientInfected = null;
GlobalForward g_hAPIFwdOnClientHuman = null;
GlobalForward g_hAPIFwdOnClientHumanPost = null;
GlobalForward g_hAPIFwdOnClientMotherZombieEligible = null;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to infection.
 */
void APIInfectInit()
{
    // Infect module natives/forwards (infect.zr.inc)

    // Natives
    CreateNative("ZR_IsClientMotherZombie", APIIsClientMotherZombie);
    CreateNative("ZR_IsClientZombie", APIIsClientZombie);
    CreateNative("ZR_IsClientHuman", APIIsClientHuman);
    CreateNative("ZR_InfectClient", APIInfectClient);
    CreateNative("ZR_HumanClient", APIHumanClient);

    // Forwards
    g_hAPIFwdOnInfectCountdown = new GlobalForward("ZR_OnInfectCountdown", ET_Hook);
    g_hAPIFwdOnClientInfect = new GlobalForward("ZR_OnClientInfect", ET_Hook, Param_CellByRef, Param_CellByRef, Param_CellByRef, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnClientInfected = new GlobalForward("ZR_OnClientInfected", ET_Ignore, Param_Cell, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
    g_hAPIFwdOnClientHuman = new GlobalForward("ZR_OnClientHuman", ET_Hook, Param_CellByRef, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnClientHumanPost = new GlobalForward("ZR_OnClientHumanPost", ET_Ignore, Param_Cell, Param_Cell, Param_Cell);
    g_hAPIFwdOnClientMotherZombieEligible = new GlobalForward("ZR_OnClientMotherZombieEligible", ET_Hook, Param_Cell);
}

/**
 * Native call function (ZR_IsClientMotherZombie)
 * Returns true if the client is a mother zombie, false if not.
 */
public int APIIsClientMotherZombie(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);

    // Validate the client index.
    APIValidateClientIndex(client, Condition_True);

    return InfectIsClientMotherInfected(client);
}

/**
 * Native call function (ZR_IsClientInfected)
 * Returns true if the client is a zombie, false if not.
 */
public int APIIsClientZombie(Handle plugin, int numParams)
{
	int client = GetNativeCell(1);
	
	// Validate the client index.
	APIValidateClientIndex(client, Condition_True);
	
	return InfectIsClientInfected(client);
}

/**
 * Native call function (ZR_IsClientHuman)
 * Returns true if the client is a human, false if not.
 *
 * bool InfectIsClientHuman(int client)
 */
public int APIIsClientHuman(Handle plugin, int numParams)
{
	int client = GetNativeCell(1);
	
	// Validate the client index.
	APIValidateClientIndex(client, Condition_True);
	
	return InfectIsClientHuman(client);
}

/**
 * Native call function (ZR_InfectClient)
 * Infects a client.
 *
 * native bool ZR_InfectClient(int client, int attacker = -1, bool motherInfect = false, bool respawnOverride = false, bool respawn = false);
 */
public int APIInfectClient(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    int attacker = GetNativeCell(2);
    bool motherInfect = view_as<bool>(GetNativeCell(3));
    bool respawnOverride = view_as<bool>(GetNativeCell(4));
    bool respawn = view_as<bool>(GetNativeCell(5));

    // Validate the client index.
    APIValidateClientIndex(client, Condition_True);

    InfectHumanToZombie(client, attacker, motherInfect, respawnOverride, respawn);
    return -1;
}

/**
 * Native call function (ZR_HumanClient)
 * Turns a zombie back into a human.
 *
 * native bool ZR_HumanClient(int client, bool respawn = false, bool protect = false);
 */
public int APIHumanClient(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    bool respawn = view_as<bool>(GetNativeCell(2));
    bool protect = view_as<bool>(GetNativeCell(2));

    // Validate the client index.
    APIValidateClientIndex(client, Condition_True);

    InfectZombieToHuman(client, respawn, protect);
    return -1;
}

/**
 * Called on infection timer to determine if timer should show.
 */
Action APIOnInfectCountdown()
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnInfectCountdown);

    // Get what they returned.
    Action result;
    Call_Finish(result);
    return result;
}

/**
 * Called when a client is about to become a zombie.
 *
 * @param client            The client to infect.
 * @param attacker          The attacker who did the infect.
 * @param motherinfect      Indicates a mother zombie infect.
 * @param respawnoverride   Set to true to override respawn cvar.
 * @param respawn           Value to override with.
 *
 * bool InfectHumanToZombie(int client, int attacker = -1, bool motherinfect = false, bool respawnoverride = false, bool respawn = false)
 */
Action APIOnClientInfect(int &client, int &attacker, bool &motherinfect, bool &respawnoverride, bool &respawn)
{
	// Start forward call.
	Call_StartForward(g_hAPIFwdOnClientInfect);
	
	// Push the parameters.
	Call_PushCellRef(client);
	Call_PushCellRef(attacker);
	Call_PushCellRef(motherinfect);
	Call_PushCellRef(respawnoverride);
	Call_PushCellRef(respawn);
	
	// Get what they returned.
	Action result;
	Call_Finish(result);
	return result;
}

/**
 * Called after a client has become a zombie.
 *
 * @param client            The client to infect.
 * @param attacker          The attacker who did the infect.
 * @param motherinfect      Indicates a mother zombie infect.
 * @param respawnoverride   Set to true to override respawn cvar.
 * @param respawn           Value to override with.
 *
 * bool InfectHumanToZombie(int client, int attacker = -1, bool motherinfect = false, bool respawnoverride = false, bool respawn = false)
 */
void APIOnClientInfected(int client, int attacker, bool motherinfect, bool respawnoverride, bool respawn)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientInfected);

    // Push the parameters.
    Call_PushCell(client);
    Call_PushCell(attacker);
    Call_PushCell(motherinfect);
    Call_PushCell(respawnoverride);
    Call_PushCell(respawn);

    // Finish the call.
    Call_Finish();

    // NOTE: Following code is legacy, map should rely on logic_eventlistener
    // See: https://github.com/srcdslab/sm-plugin-zombiereloaded/issues/89
    if (g_hCvarsList.CVAR_INFECT_SKIP_TRIGGER_BRUSH.BoolValue)
        return;

    // Forward event to map.
    if (!motherinfect)
    {
        int entity = INVALID_ENT_REFERENCE;
        while((entity = FindEntityByClassname(entity, "trigger_brush")) != INVALID_ENT_REFERENCE)
        {
            static char sTargetname[64];
            GetEntPropString(entity, Prop_Data, "m_iName", sTargetname, sizeof(sTargetname));

            if (client > 0 && strcmp(sTargetname, "game_playerdie", false) == 0)
                AcceptEntityInput(entity, "Use", client, client);
            else if (attacker > 0 && strcmp(sTargetname, "game_playerkill", false) == 0)
                AcceptEntityInput(entity, "Use", attacker, attacker);
        }
    }
}

/**
 * Called when a client is about to become a human. (Through either zr_human or admin menu)
 *
 * @param client            The client index.
 * @param respawn           True if the client was respawned, false if not.
 * @param protect           True if the client spawn protected, false if not.
 *
 * bool InfectZombieToHuman(int client, bool respawn = false, bool protect = false)
 */
Action APIOnClientHuman(int &client, bool &respawn, bool &protect)
{
	// Start forward call.
	Call_StartForward(g_hAPIFwdOnClientHuman);
	
	// Push the parameters.
	Call_PushCellRef(client);
	Call_PushCellRef(respawn);
	Call_PushCellRef(protect);
	
	// Get what they returned.
	Action result;
	Call_Finish(result);
	return result;
}

/**
 * Called after a client has become a human. (Through either zr_human or admin menu)
 *
 * @param client            The client index.
 * @param respawn           True if the client was respawned, false if not.
 * @param protect           True if the client spawn protected, false if not.
 *
 * bool InfectZombieToHuman(int client, bool respawn = false, bool protect = false)
 */
void APIOnClientHumanPost(int client, bool respawn, bool protect)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientHumanPost);

    // Push the parameters.
    Call_PushCell(client);
    Call_PushCell(respawn);
    Call_PushCell(protect);

    // Finish the call.
    Call_Finish();
}

/**
 * Called in ZRCreateEligibleClientList to determine if a client is eligible to become infected.
 *
 * @param client            The client to check.
 */
Action APIOnClientMotherZombieEligible(int client)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientMotherZombieEligible);

    // Push the parameters.
    Call_PushCell(client);

    // Get what they returned.
    Action result;
    Call_Finish(result);
    return result;
}
