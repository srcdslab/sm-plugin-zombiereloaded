/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infect.inc
 *  Type:          Core
 *  Description:   Client infection functions.
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

#if defined REQUIRE_PLUGIN
 #define TEMP_REQUIRE_PLUGIN
 #undef REQUIRE_PLUGIN
#endif

#tryinclude "AFKManager.inc"
#tryinclude "TeamManager.inc"

/* Restore old REQUIRE_PLUGIN value if necessary */
#if defined TEMP_REQUIRE_PLUGIN
 #define REQUIRE_PLUGIN
 #undef TEMP_REQUIRE_PLUGIN
#endif

bool g_AFKManagerLoaded = false;
bool g_TeamManagerLoaded = false;

/**
 * @section Explosion flags.
 */
#define EXP_NODAMAGE               1
#define EXP_REPEATABLE             2
#define EXP_NOFIREBALL             4
#define EXP_NOSMOKE                8
#define EXP_NODECAL               16
#define EXP_NOSPARKS              32
#define EXP_NOSOUND               64
#define EXP_RANDOMORIENTATION    128
#define EXP_NOFIREBALLSMOKE      256
#define EXP_NOPARTICLES          512
#define EXP_NODLIGHTS           1024
#define EXP_NOCLAMPMIN          2048
#define EXP_NOCLAMPMAX          4096
/**
 * @endsection
 */
 
 /**
 * @section Other defines.
 */
 #define MAX_EVENT_LENGTH 32
/**
 * @endsection
 */

/**
 * @section Global variables to store infect timer handles.
 */
Handle g_tInfect = INVALID_HANDLE;
Handle g_tInfectCountdown = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Infection countdown data pack.
 */
Handle g_hInfectCountdownData = INVALID_HANDLE;

/**
 * Array for flagging client as zombie.
 */
bool g_bZombie[MAXPLAYERS + 1];

/**
 * Array for flagging client to be protected.
 */
bool g_bInfectImmune[MAXPLAYERS + 1];

/**
 * Array for storing client mother zombie last flag.
 */
bool g_bInfectMotherLast[MAXPLAYERS + 1];

/**
 * SteamID cache for storing client mother zombie protection status.
 */
Handle g_hInfectMotherCycle = INVALID_HANDLE;
Handle g_hInfectMotherCycleRTD = INVALID_HANDLE;

/**
 * Available mother zombie infection modes.
 */
enum InfectMode
{
    InfectMode_Invalid = -1,    /** Invalid mode, used by validators. */
    InfectMode_Dynamic,         /** Every n-th player is infected. */
    InfectMode_Absolute,        /** Keep n humans (negative n) or infect n zombies. */
    InfectMode_Range            /** An absolute number of zombies infected (min to max). */
}

/**
 * All plugins have finished loading.
 */
void InfectOnAllPluginsLoaded()
{
    #if defined _AFKManager_Included
    g_AFKManagerLoaded = LibraryExists("AFKManager");
    LogMessage("AFKManager: %s", (g_AFKManagerLoaded ? "loaded" : "not loaded"));
    #endif

    #if defined _TeamManager_include
    g_TeamManagerLoaded = LibraryExists("TeamManager");
    LogMessage("TeamManager: %s", (g_TeamManagerLoaded ? "loaded" : "not loaded"));
    #endif
}

/**
 * A library was added.
 */
void InfectOnLibraryAdded(const char[] name)
{
    if (StrEqual(name, "AFKManager"))
    {
        // AFKManager loaded.
        g_AFKManagerLoaded = true;
    }
    else if (StrEqual(name, "TeamManager"))
    {
        // TeamManager loaded.
        g_TeamManagerLoaded = true;
    }
}

/**
 * A library was removed.
 */
void InfectOnLibraryRemoved(const char[] name)
{
    if (StrEqual(name, "AFKManager"))
    {
        // AFKManager unloaded.
        g_AFKManagerLoaded = false;
    }
    else if (StrEqual(name, "TeamManager"))
    {
        // TeamManager unloaded.
        g_TeamManagerLoaded = false;
    }
}

/**
 * Map is ending.
 */
void InfectOnMapEnd()
{
    // Reset timers. Infect timers are invalidated on a map change if they are
    // still running.
    ZREndTimer(g_tInfect);
    ZREndTimer(g_tInfectCountdown);
    InfectStopCountdown();

    // Clear mother zombie round-robin cycle storage.
    SteamidCacheReset(g_hInfectMotherCycle);
    SteamidCacheReset(g_hInfectMotherCycleRTD);
}

/**
 * Loads downloadable content data for infect module.
 */
void InfectLoad()
{
    // Create mother zombie round-robin cycle storage.
    g_hInfectMotherCycle = SteamidCacheCreate();
    g_hInfectMotherCycleRTD = SteamidCacheCreate();
}

/**
 * Create commands specific to infect here.
 */
void InfectOnCommandsCreate()
{
    RegConsoleCmd("zr_infect", InfectInfectCommand, "Infect a client. Usage: zr_infect <filter> [respawn - 1/0]");
    RegConsoleCmd("zr_human", InfectHumanCommand, "Turn a client into a human. Usage: zr_human <filter> [respawn - 1/0]");
}

/**
 * Client is joining the server.
 *
 * @param client    The client index.
 */
void InfectClientInit(int client)
{
    // Reset infect immunity flag.
    g_bInfectImmune[client] = false;

    // Reset mother zombie last flag.
    g_bInfectMotherLast[client] = false;

    bool infectroundrobinrtd = GetConVarBool(g_hCvarsList.CVAR_INFECT_ROUND_ROBIN_RTD);
    if (infectroundrobinrtd && !SteamidCacheClientExists(g_hInfectMotherCycleRTD, client))
    {
        SteamidCacheAddClient(g_hInfectMotherCycleRTD, client);

        int players = 0;
        int playersInList = 0;
        for(int x = 1; x <= MaxClients; x++)
        {
            if(!IsClientInGame(x))
                continue;

            #if defined _AFKManager_Included
            if(g_AFKManagerLoaded)
            {
                if(!IsFakeClient(x) && GetClientIdleTime(x) > 3 * 60)
                    continue;
            }
            #endif

            players++;

            if(SteamidCacheClientExists(g_hInfectMotherCycle, x))
                playersInList++;
        }

        if(players && playersInList)
        {
            float mzombiechance = float(playersInList) / float(players);
            float dice = GetRandomFloat();

            if (dice < mzombiechance)
                SteamidCacheAddClient(g_hInfectMotherCycle, client);
        }
    }
}

/**
 * Client is leaving the server.
 *
 * @param client    The client index.
 */
void InfectOnClientDisconnect(int client)
{
    // If client is still connecting, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }

    // If zombie hasn't spawned, then stop.
    if (!InfectHasZombieSpawned())
    {
        return;
    }

    // If client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return;
    }

    // Initialize count variables
    int zombiecount;
    int humancount;

    // Count valid clients.
    ZRCountValidClients(zombiecount, humancount);

    // If client is a human.
    if (InfectIsClientHuman(client))
    {
        // If there are other humans (ignoring this human), then stop.
        if (humancount > 1)
        {
            return;
        }

        // If there are no more clients in the server, then stop.
        if (!ZRTeamHasClients(CS_TEAM_T))
        {
            return;
        }

        // Manually terminate round.
        RoundEndTerminateRound(ROUNDEND_DELAY, ZombiesWin);

        return;
    }

    // We know here that player is a zombie.

    // If there is 1 or less humans, then stop.
    if (humancount <= 1)
    {
        return;
    }

    // If there are other zombies (ignoring this zombie), then stop.
    if (zombiecount - 1)
    {
        return;
    }

    // Create eligible player list.
    Handle arrayEligibleClients = CreateArray();

    // Create eligible client list, with no mother infect immunities
    int eligibleclients = ZRCreateEligibleClientList(arrayEligibleClients, true, true, true);

    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        // Destroy handle.
        CloseHandle(arrayEligibleClients);
        return;
    }

    // Get a random valid array index.
    int randindex = Math_GetRandomInt(0, eligibleclients - 1);

    // Get the client stored in the random array index.
    int randclient = GetArrayCell(arrayEligibleClients, randindex);

    // Infect player.
    InfectHumanToZombie(randclient);

    // Force ztele the client chosen to replace disonnecting zombie
    bool fztelenew = ZTele_ChosenClientReplaceDisconnected();
    if (fztelenew)
        ZTele_TeleportClient(randclient);

    // Tell client they have been randomly been chosen to replace disconnecting zombie.
    TranslationPrintToChat(randclient, "Infect disconnect");

    // Destroy handle.
    CloseHandle(arrayEligibleClients);
}

/**
 * Client is joining a team.
 *
 * @param client    The client index.
 * @param team      The team index.
 */
void InfectOnClientTeam(int client, int team)
{
    // If client isn't joining spec, then stop.
    if (team != CS_TEAM_SPECTATOR)
    {
        return;
    }

    // Disable zombie flag on client.
    g_bZombie[client] = false;
}

/**
 * Client is spawning into the game.
 *
 * @param client    The client index.
 */
void InfectOnClientSpawn(int client)
{
    // Disable zombie flag on client.
    g_bZombie[client] = false;

    // Check if client is spawning on the terrorists team.
    if (ZRIsClientOnTeam(client, CS_TEAM_T) && InfectHasZombieSpawned())
    {
        CS_SwitchTeam(client, CS_TEAM_CT);
        CS_RespawnPlayer(client);
    }

    // Unglitch kevlar, set last hitgroup to HITGROUP_GENERIC
    ToolsSetClientLastHitGroup(client, HITGROUP_GENERIC);

    // Forward event to modules.
    ZSpawnOnClientSpawn(client);
}

/**
 * Client has been killed.
 *
 * @param client    The client index.
 * @param attacker  The attacker index.
 */
void InfectOnClientDeath(int client, int attacker)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return;
    }

    // If attacker isn't a human, then stop.
    if (!InfectIsClientHuman(attacker))
    {
        return;
    }

    // If client isn't a zombie, then stop.
    if (!InfectIsClientInfected(client))
    {
        return;
    }

    // Add kill bonus to attacker's score.
    int bonus = ClassGetKillBonus(client);
    int score = ToolsClientScore(attacker, true, false);
    ToolsClientScore(attacker, true, true, score + bonus);
}

/**
 * Client has been hurt.
 *
 * @param client        The client index.
 * @param attacker      The attacker index.
 * @param weapon        The weapon used.
 */
void InfectOnClientHurt(int client, int attacker, const char[] weapon)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return;
    }

    // If client isn't a human, then stop.
    if (!InfectIsClientHuman(client))
    {
        return;
    }

    // Attacker isn't a zombie, then stop.
    if (!InfectIsClientInfected(attacker))
    {
        return;
    }

    // If client has infect immunity, then stop.
    if (g_bInfectImmune[client])
    {
        return;
    }

    // If weapon isn't a knife, then stop.
    if (!StrEqual(weapon, "knife") && !StrEqual(weapon, "bayonet"))
    {
        return;
    }

    // Check if the immunity module is handling the infection.
    if (ImmunityOnClientInfect(client, attacker))
    {
        //PrintToChatAll("InfectOnClientHurt - Infect blocked.");
        return;
    }

    // Infect client.
    InfectHumanToZombie(client, attacker);
}

/**
 * The round is starting.
 */
void InfectOnRoundStart()
{
    // Stop infect timers if running.
    ZREndTimer(g_tInfect);
    ZREndTimer(g_tInfectCountdown);

    // Tell plugin there are no zombies.
    g_bZombieSpawned = false;
}

/**
 * The freeze time is ending.
 */
void InfectOnRoundFreezeEnd()
{
    // Stop infect timers if running.
    ZREndTimer(g_tInfect);
    ZREndTimer(g_tInfectCountdown);

    // If the zombie has spawned already (had to be through admin) then stop.
    if (InfectHasZombieSpawned())
    {
        return;
    }

    // Warmup
    #if defined _TeamManager_include
    if(g_TeamManagerLoaded && TeamManager_HasWarmup() && TeamManager_InWarmup())
    {
        return;
    }
    #endif

    // Get min and max times.
    float infectspawntimemin = GetConVarFloat(g_hCvarsList.CVAR_INFECT_SPAWNTIME_MIN);
    float infectspawntimemax = GetConVarFloat(g_hCvarsList.CVAR_INFECT_SPAWNTIME_MAX);

    // Pick random time between min and max.
    float randomtime = GetRandomFloat(infectspawntimemin, infectspawntimemax);

    g_tInfect = CreateTimer(randomtime, InfectMotherZombie, _, TIMER_FLAG_NO_MAPCHANGE);

    // Check cvar and start a countdown timer if enabled.
    bool countdown = GetConVarBool(g_hCvarsList.CVAR_INFECT_MZOMBIE_COUNTDOWN);
    if (countdown && randomtime > 1.0)
    {
        // Stop old countdown timer, if it exists.
        InfectStopCountdown();

        // Store the time until infection, and initialize the counter.
        g_hInfectCountdownData = CreateDataPack();
        WritePackFloat(g_hInfectCountdownData, randomtime);
        WritePackFloat(g_hInfectCountdownData, 0.0);
        g_tInfectCountdown = CreateTimer(1.0, InfectCountdown, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);

        // Display initial tick.
        InfectCountdown(g_tInfectCountdown);
    }
}

/**
 * The round is ending.
 */
public Action InfectOnRoundEnd(Handle Timer)
{
    // Stop infect timers if running.
    ZREndTimer(g_tInfect);
    ZREndTimer(g_tInfectCountdown);

    // x = client index.
    for (int x = 1; x <= MaxClients; x++)
    {
        // Disable zombie flag on client.
        g_bZombie[x] = false;
    }
    return Plugin_Continue;
}

/**
 * Timer callback, chooses mother zombies.
 *
 * @param timer     The timer handle.
 */
public Action InfectMotherZombie(Handle timer)
{
    // Reset timer handle.
    g_tInfect = INVALID_HANDLE;

    // Warmup
    #if defined _TeamManager_include
    if(g_TeamManagerLoaded && TeamManager_HasWarmup() && TeamManager_InWarmup())
    {
        return Plugin_Continue;
    }
    #endif

    // Create eligible player list.
    Handle arrayEligibleClients = CreateArray();
    int eligibleclients = ZRCreateEligibleClientList(arrayEligibleClients, true, true, true, true);

    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        // Destroy handle.
        CloseHandle(arrayEligibleClients);
        return Plugin_Continue;
    }

    // Move all clients to CT.
    InfectMoveAllToCT();

    int mothercount;
    int ratio = GetConVarInt(g_hCvarsList.CVAR_INFECT_MZOMBIE_RATIO);
    int min = GetConVarInt(g_hCvarsList.CVAR_INFECT_MZOMBIE_MIN);
    int max = GetConVarInt(g_hCvarsList.CVAR_INFECT_MZOMBIE_MAX);

    // Count valid human clients.
    int humancount;
    ZRCountValidClients(_, humancount, _, true);

    // Get and validate infection mode. This will also log a warning on error.
    InfectMode mode = InfectGetModeOrFail();

    // Apply infection mode.
    switch (mode)
    {
        case InfectMode_Invalid:
        {
            // Validation failed. Fall back to one mother zombie.
            mothercount = 1;
        }
        case InfectMode_Dynamic:
        {
            // Dynamic mode. Every n-th player is infected.

            // A ratio of 0 will infect one zombie (to keep backwards compatibility).
            if (ratio == 0)
            {
                mothercount = 1;
            }

            // Calculate number of zombies to infect.
            mothercount = RoundToNearest(float(humancount) / ratio);

            // Require at least one mother zombie.
            if (mothercount == 0)
            {
                mothercount = 1;
            }
        }
        case InfectMode_Absolute:
        {
            if (ratio > 0)
            {
                // Infect n humans.
                mothercount = ratio;
            }
            else
            {
                // Infect all but n humans. Since ratio already is negative
                // just add the numbers. (Zero ratio is catched by validator.)
                mothercount = humancount + ratio;

                // Force at least one mother zombie.
                if (mothercount == 0)
                {
                    mothercount = 1;
                }
            }
        }
        case InfectMode_Range:
        {
            // Get a random number between the range.
            mothercount = Math_GetRandomInt(min, max);
        }
    }

    bool infectroundrobin = GetConVarBool(g_hCvarsList.CVAR_INFECT_ROUND_ROBIN);
    int candidatesMain = 0;
    int candidatesAlt = 0;
    Handle aCandidatesMain = CreateArray();
    Handle aCandidatesAlt = CreateArray();
    for (int n = 0; n < eligibleclients; n++)
    {
        // Get the client stored in the array index.
        int client = GetArrayCell(arrayEligibleClients, n);

        // If client hasn't been chosen last round.
        if (!g_bInfectMotherLast[client])
        {
            // If client hasn't been chosen this cycle, put into aCandidatesMain array.
            if (!infectroundrobin || !SteamidCacheClientExists(g_hInfectMotherCycle, client))
            {
                PushArrayCell(aCandidatesMain, client);
                candidatesMain++;
            }

            // Else put into aCandidatesAlt array.
            else
            {
                PushArrayCell(aCandidatesAlt, client);
                candidatesAlt++;
            }
        }
    }

    // Remove mother zombie last flag from all players.
    for (int client = 0; client <= MAXPLAYERS; client++)
    {
        g_bInfectMotherLast[client] = false;
    }

    bool resetcycle;

    // Infect players.
    int infected = 0;
    while (infected < mothercount)
    {
        // Infect one of the main candidates.
        if (candidatesMain)
        {
            // Get a random array index.
            int i = Math_GetRandomInt(0, candidatesMain - 1);

            // Get the client stored in the random array index.
            int client = GetArrayCell(aCandidatesMain, i);

            // Infect player.
            if (InfectHumanToZombie(client, _, true))
                infected++;

            // Remove player from eligible client list.
            RemoveFromArray(aCandidatesMain, i);
            candidatesMain--;
        }
        else
        {
            // No main candidates, reset the mother zombie cycle now before infecting anyone else.
            if (infectroundrobin && !resetcycle)
            {
                resetcycle = true;
                // Clear mother zombie round-robin cycle storage.
                SteamidCacheReset(g_hInfectMotherCycle);
                SteamidCacheReset(g_hInfectMotherCycleRTD);

                // Announce start of new cycle
                TranslationPrintToChatAll(true, false, "Mother zombie infect cycle reset");
            }

            // Infect one of the alternate candidates.
            if (candidatesAlt)
            {
                // Get a random array index.
                int i = Math_GetRandomInt(0, candidatesAlt - 1);

                // Get the client stored in the random array index.
                int client = GetArrayCell(aCandidatesAlt, i);

                // Infect player.
                if (InfectHumanToZombie(client, _, true))
                    infected++;

                // Remove player from eligible client list.
                RemoveFromArray(aCandidatesAlt, i);
                candidatesAlt--;
            }
            else // We have no candidates at all, abort!
            {
                break;
            }
        }
    }

    // Mother zombies have been infected.
    g_bZombieSpawned = true;

    // Destroy client list.
    CloseHandle(arrayEligibleClients);
    CloseHandle(aCandidatesMain);
    CloseHandle(aCandidatesAlt);

    return Plugin_Continue;
}

/**
 * Moves all alive clients to the CT team.
 */
void InfectMoveAllToCT()
{
    // Move all clients to CT
    for (int client = 1; client <= MaxClients; client++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(client))
        {
            continue;
        }

        // If client is dead, then stop.
        if (!IsPlayerAlive(client))
        {
            continue;
        }

        // Switch client to CT team.
        CS_SwitchTeam(client, CS_TEAM_CT);
    }
}

/**
 * Timer callback, displays countdown to clients.
 *
 * @param timer     The timer handle.
 */
public Action InfectCountdown(Handle timer)
{
    if (!GetConVarBool(g_hCvarsList.CVAR_INFECT_MZOMBIE_COUNTDOWN))
    {
        InfectStopCountdown();
        return Plugin_Stop;
    }

    // Read the info from the datapack.
    ResetPack(g_hInfectCountdownData);
    float length = ReadPackFloat(g_hInfectCountdownData);
    float counter = ReadPackFloat(g_hInfectCountdownData);

    // Check if the countdown has finished.
    if (counter >= length)
    {
        InfectStopCountdown();
        return Plugin_Stop;
    }

    // Ask plugin API if timer should be shown.
    if (APIOnInfectCountdown() == Plugin_Continue)
    {
        // Print the countdown text to the clients.
        TranslationPrintCenterTextAll(false, "Infect countdown", RoundToNearest(length - counter));

        float countdownvolume = SEffects_GetVolume();
        int countdown = RoundToNearest(length - counter);
        int iInfectCountdownSounds = SEffects_GetMax(Countdown);
        if (countdownvolume > 0.0 && iInfectCountdownSounds != 0 && countdown > 0 && countdown <= iInfectCountdownSounds)
        {
            char soundpath[PLATFORM_MAX_PATH];
            SEffects_GetPath(soundpath, Countdown);
            Format(soundpath, SOUND_MAX_PATH, soundpath, countdown);

            for (int x = 1; x <= MaxClients; x++)
            {
                if (IsClientInGame(x) && zfZSoundVolume[x] > 0)
                {
                    EmitSoundToClient(x, soundpath, _, _, SNDLEVEL_NORMAL, _, zfZSoundVolume[x]);
                }
            }
        }
    }

    counter++;

    // Write the new counter value to the datapack.
    ResetPack(g_hInfectCountdownData);
    WritePackFloat(g_hInfectCountdownData, length);
    WritePackFloat(g_hInfectCountdownData, counter);

    return Plugin_Continue;
}

/**
 * Stops the infection countdown timer.
 */
void InfectStopCountdown()
{
    // Kill the timer.
    ZREndTimer(g_tInfectCountdown);

    // Destroy data pack.
    if (g_hInfectCountdownData != INVALID_HANDLE)
    {
        CloseHandle(g_hInfectCountdownData);
        g_hInfectCountdownData = INVALID_HANDLE;
    }
}

/**
 * Infects a client. Execute events, sets attributes and flags that indicate
 * that the client is a zombie.
 *
 * @param client            The client to infect.
 * @param attacker          (Optional) The attacker who did the infect.
 * @param motherinfect      (Optional) Indicates a mother zombie infect.
 * @param respawnoverride   (Optional) Set to true to override respawn cvar.
 * @param respawn           (Optional) Value to override with.
 */
bool InfectHumanToZombie(int client, int attacker = -1, bool motherinfect = false, bool respawnoverride = false, bool respawn = false)
{
    // Forward pre-event to modules.
    Action result = APIOnClientInfect(client, attacker, motherinfect, respawnoverride, respawn);

    // Check if infection should be blocked.
    if (result == Plugin_Handled)
    {
        return false;
    }

    // Mark player as zombie.
    g_bZombie[client] = true;

    // Check if consecutive infection protection is enabled.
    bool infectconsecutiveblock = GetConVarBool(g_hCvarsList.CVAR_INFECT_CONSECUTIVE_BLOCK);
    bool infectroundrobin = GetConVarBool(g_hCvarsList.CVAR_INFECT_ROUND_ROBIN);

    // If this is a mother infect, update the mother zombie protection status
    if (motherinfect)
    {
        g_bInfectMotherLast[client] = infectconsecutiveblock;
        if(infectroundrobin)
        {
            SteamidCacheAddClient(g_hInfectMotherCycle, client);
        }
    }

    // Apply effects.
    InfectFireEffects(client);

    // Stop coundown, if running.
    InfectStopCountdown();

    // If attacker is valid, then continue.
    if (ZRIsClientValid(attacker))
    {
        // Create and send custom player_death event.
        Handle event = CreateEvent("player_death");
        if (event != INVALID_HANDLE)
        {
            char eventWeapon[MAX_EVENT_LENGTH];
            GetConVarString(g_hCvarsList.CVAR_INFECT_EVENT_WEAPON, eventWeapon, MAX_EVENT_LENGTH);
            SetEventInt(event, "userid", GetClientUserId(client));
            SetEventInt(event, "attacker", GetClientUserId(attacker));
            SetEventString(event, "weapon", eventWeapon);
            FireEvent(event, false);
        }

        // Apply score and health gain.
        InfectUpdateScore(attacker, client);
    }

    // Check if weapons drop is enabled.
    bool weaponsdrop = GetConVarBool(g_hCvarsList.CVAR_INFECT_WEAPONS_DROP);

    // This must be after the event forwarding because it fixes a problem caused by changing models in ClassOnClientInfected.
    // Remove all weapons but knife.
    WeaponsRemoveAllClientWeapons(client, weaponsdrop);

    // Zombie won't be able to use their knife for this amount of time
    float knifecooldown = GetConVarFloat(g_hCvarsList.CVAR_INFECT_KNIFE_COOLDOWN);
    SetEntPropFloat(client, Prop_Send, "m_flNextAttack", GetGameTime() + knifecooldown);

    // Switch the player to terrorists.
    // TODO: A solution to stop confusing bots? Respawn and teleport?
    CS_SwitchTeam(client, CS_TEAM_T);

    // If respawn is enabled, then teleport mother zombie back to spawnpoint.
    if (motherinfect)
    {
        bool zombierespawn = GetConVarBool(g_hCvarsList.CVAR_INFECT_MZOMBIE_RESPAWN);
        if(zombierespawn)
        {
            ZTele_TeleportClient(client);
        }
    }
    // Check override.
    else
    {
        bool teleport = GetConVarBool(g_hCvarsList.CVAR_INFECT_TELEPORT);
        if (respawnoverride && respawn)
        {
            ZTele_TeleportClient(client);
        }
        // Teleport to attacker when infected
        else if(teleport && ZRIsClientValid(attacker))
        {
            // Get client's position.
            float clientloc[3];
            GetClientAbsOrigin(attacker, clientloc);

            TeleportEntity(client, clientloc, NULL_VECTOR, NULL_VECTOR);
        }
    }

    // Remove kevlar and helmet
    SetEntProp(client, Prop_Send, "m_ArmorValue", 0, 1);
    SetEntProp(client, Prop_Send, "m_bHasHelmet", 0);

    // Print message to client.
    TranslationPrintToChat(client, "Infect infected");

    // Forward event to modules.
    ClassOnClientInfected(client, motherinfect);
    RoundEndOnClientInfected();
    DamageOnClientInfected(client, motherinfect);
    SEffectsOnClientInfected(client);
    ZTele_OnClientInfected(client);
    ZHPOnClientInfected(client);
    APIOnClientInfected(client, attacker, motherinfect, respawnoverride, respawn);
    ImmunityOnClientInfected(client);
    ZSpawnOnClientInfected(client);

    return true;
}

/**
 * Turns a zombie back into a human. Execute events, sets attributes and flags that indicate
 * that the client is a human.
 *
 * @param client        The client to make human.
 * @param respawn       Teleport client back to spawn if true.
 * @param protect       Start spawn protection on new human.
 */
bool InfectZombieToHuman(int client, bool respawn = false, bool protect = false)
{
    // Forward pre-event to modules.
    Action result = APIOnClientHuman(client, respawn, protect);

    // Check if action should be blocked.
    if (result == Plugin_Handled)
    {
        return false;
    }

    // Mark player as human.
    g_bZombie[client] = false;

    // Switch the player to counter-terrorists.
    CS_SwitchTeam(client, CS_TEAM_CT);

    // Set client as translation target.
    SetGlobalTransTarget(client);

    // Print message to client.
    TranslationPrintToChat(client, "Infect human");

    // Forward event to modules.
    ClassReloadPlayer(client);
    RoundEndOnClientInfected();
    ZTele_OnClientInfected(client);

    // Remove all knifes.
    WeaponsClearClientWeaponSlot(client, Slot_Melee, false);

    // Give human a new knife. (If you leave the old one there will be glitches with the knife positioning)
    GivePlayerItem(client, "weapon_knife");

    // Check if we should respawn the client.
    if (respawn)
    {
        ZTele_TeleportClient(client);
    }

    // Check if we should spawn protect the client.
    if (protect)
    {
        SpawnProtectStart(client);
    }

    // Unglitch kevlar, set last hitgroup to HITGROUP_GENERIC
    ToolsSetClientLastHitGroup(client, HITGROUP_GENERIC);

    // Forward event to modules.
    SEffectsOnClientHuman(client);
    APIOnClientHumanPost(client, respawn, protect);
    ImmunityOnClientHuman(client);
    ZSpawnOnClientHuman(client);

    return true;
}

/**
 * Updates score for attacker and victim. Applies health gain for attacker.
 */
void InfectUpdateScore(int attacker, int victim)
{
    // Give client's infector a point.
    int score = ToolsClientScore(attacker, true, false);
    ToolsClientScore(attacker, true, true, ++score);

    // Add a death to the zombie's score.
    int deaths = ToolsClientScore(victim, false, false);
    ToolsClientScore(victim, false, true, ++deaths);

    // Apply infect HP gain.
    int healthgain = ClassGetHealthInfectGain(attacker);
    int health = GetClientHealth(attacker);

    // Set attacker's new health.
    SetEntityHealth(attacker, health + healthgain);

    // Forward event to modules.
    ZHPOnHealthInfectGain(attacker);
}

/**
 * Creates effects on a newly infected client.
 *
 * @param client    The client index.
 */
void InfectFireEffects(int client)
{
    // Initialize vector variables.
    float clientloc[3];
    float direction[3] = {0.0, 0.0, 0.0};

    // Get client's position.
    GetClientAbsOrigin(client, clientloc);
    clientloc[2] += 30;

    bool explosion = GetConVarBool(g_hCvarsList.CVAR_INFECT_EXPLOSION);
    if (explosion)
    {
        // Initialize explosion flags variable.
        int flags;

        // Set "nofireball" flag if fireball is disabled.
        bool fireball = GetConVarBool(g_hCvarsList.CVAR_INFECT_FIREBALL);
        if (!fireball)
        {
            flags = flags | EXP_NOFIREBALL;
        }

        // Set "nosmoke" flag if smoke is disabled.
        bool smoke = GetConVarBool(g_hCvarsList.CVAR_INFECT_SMOKE);
        if (!smoke)
        {
            flags = flags | EXP_NOSMOKE;
        }

        // Set "nosparks" flag if sparks are disabled.
        bool sparks = GetConVarBool(g_hCvarsList.CVAR_INFECT_SPARKS);
        if (!sparks)
        {
            flags = flags | EXP_NOSPARKS;
        }

        // Create explosion at client's origin.
        VEffectsCreateExplosion(clientloc, flags);
    }

    // Emit scream sound if enabled.
    ZombieSoundsScream(client);

    // If energy splash effect is enabled, then continue.
    bool esplash = GetConVarBool(g_hCvarsList.CVAR_INFECT_ESPLASH);
    if (esplash)
    {
        // Create energy splash effect.
        VEffectsCreateEnergySplash(clientloc, direction, true);
    }

    // If shake effect is enabled, then continue.
    bool shake = GetConVarBool(g_hCvarsList.CVAR_INFECT_SHAKE);
    if (shake)
    {
        // Get shake info.
        float shakeamp = GetConVarFloat(g_hCvarsList.CVAR_INFECT_SHAKE_AMP);
        float shakefrequency = GetConVarFloat(g_hCvarsList.CVAR_INFECT_SHAKE_FREQUENCY);
        float shakeduration = GetConVarFloat(g_hCvarsList.CVAR_INFECT_SHAKE_DURATION);

        // Shake client's screen.
        VEffectsShakeClientScreen(client, shakeamp, shakefrequency, shakeduration);
    }
}

/**
 * Sends list of clients to infect/human.
 *
 * @param client    The client index.
 */
void InfectMenuClients(int client)
{
    // Create menu handle.
    Handle menu_infect_clients = CreateMenu(InfectMenuClientsHandle);

    // Set client as translation target.
    SetGlobalTransTarget(client);

    char title[MENU_LINE_TITLE_LENGTH];
    char clientoption[MENU_LINE_REG_LENGTH];
    char clientuserid[8];

    // x = Client index.
    for (int x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }

        // If client isn't alive, then stop.
        if (!IsPlayerAlive(x))
        {
            continue;
        }

        // Get client info.
        GetClientName(x, clientoption, sizeof(clientoption));
        IntToString(GetClientUserId(x), clientuserid, sizeof(clientuserid));

        // Append client's current team to the option.
        if (InfectIsClientInfected(x))
        {
            Format(clientoption, sizeof(clientoption), "%s [%t]", clientoption, "Zombie");
        }
        else
        {
            Format(clientoption, sizeof(clientoption), "%s [%t]", clientoption, "Human");
        }

        // Add option to menu.
        AddMenuItem(menu_infect_clients, clientuserid, clientoption);
    }

    Format(title, sizeof(title), "%t\n ", "Infect menu clients title");
    SetMenuTitle(menu_infect_clients, title);

    // Create a "Back" button to the main admin menu.
    SetMenuExitBackButton(menu_infect_clients, true);

    // Send menu.
    DisplayMenu(menu_infect_clients, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the infect clients menu, and handles it.
 * @param menu_infect_clients   Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */
public int InfectMenuClientsHandle(Handle menu_infect_clients, MenuAction action, int client, int slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get selected client index.
        int target = MenuGetClientIndex(menu_infect_clients, slot);

        // If target has left the server, then stop.
        if (!target)
        {
            // Re-send menu.
            InfectMenuClients(client);
            return -1;
        }

        // Create an array with a single slot and set target to it.
        int targets[1];
        targets[0] = target;

        // Toggle infect on the client.
        if (InfectIsClientInfected(target))
        {
            InfectManualHuman(client, targets, 1);
        }
        else
        {
            InfectManualInfect(client, targets, 1);
        }

        // Re-send menu.
        InfectMenuClients(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            // Re-open admin menu.
            ZAdminMenu(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_infect_clients);
    }
    return -1;
}

bool InfectHasZombieSpawned()
{
    return g_bZombieSpawned;
}

/**
 * Returns if a client is mother infected.
 *
 * @param client    The client index.
 * @return          True if the client has been mother infected, false otherwise.
 */
bool InfectIsClientMotherInfected(int client)
{
    // If client is invalid, then stop.
    if (!ZRIsClientValid(client))
    {
        return false;
    }

    // Return client's zombie flag.
    return g_bInfectMotherLast[client];
}

/**
 * Returns if a client is infected.
 * 
 * @param client	The client index.
 * @return		  True if the client has been infected, false otherwise.
 */	
bool InfectIsClientInfected(int client)
{
    // If client is invalid, then stop.
    if (!ZRIsClientValid(client))
    {
        return false;
    }

    // Return client's zombie flag.
    return g_bZombie[client];
}

/**
 * Returns if a client is a human.
 *
 * @param client    The client index.
 * @return          True if the client is a human, false otherwise.
 */
bool InfectIsClientHuman(int client)
{
    // If client is invalid, then stop.
    if (!ZRIsClientValid(client))
    {
        return true;
    }

    // Return opposite of client's zombie flag.
    return !g_bZombie[client];
}

/**
 * Infecting a client manually (via zr_infect or the "Zombie Management" menu)
 *
 * @param client            The client index infecting another client.
 * @param targets           Array containing all clients to infect.
 * @param count             The number of clients in the array.
 * @param respawnoverride   (Optional) True to override respawn cvar.
 * @param respawn           (Optional) True to respawn client on infect.
 */
stock void InfectManualInfect(int client, int[] targets, int count, bool respawnoverride = false, bool respawn = false)
{
    bool zombiespawned = InfectHasZombieSpawned();

    // If zombie hasn't spawned, then make targetted player(s) mother zombies.
    if (!zombiespawned)
    {
        // Stop mother infect timer.
        if (g_tInfect != INVALID_HANDLE)
        {
            KillTimer(g_tInfect);
            g_tInfect = INVALID_HANDLE;
        }

        // Move all clients to CT
        for (int x = 1; x <= MaxClients; x++)
        {
            // If client isn't in-game, then stop.
            if (!IsClientInGame(x))
            {
                continue;
            }

            // If client is dead, then stop.
            if (!IsPlayerAlive(x))
            {
                continue;
            }

            // Switch client to CT team.
            CS_SwitchTeam(x, CS_TEAM_CT);
        }

        // Tell the plugin a mother zombie has spawned.
        g_bZombieSpawned = true;
    }

    char targetname[MAX_NAME_LENGTH];
    char adminname[MAX_NAME_LENGTH];

    // Get admin's name for later use.
    if(client > 0)
        GetClientName(client, adminname, sizeof(adminname));
    else
        strcopy(adminname, sizeof(adminname), "Console");

    bool success = false;

    // x = Client index.
    for (int x = 0; x < count; x++)
    {
        // Get client's name for later use.
        GetClientName(targets[x], targetname, sizeof(targetname));

        // Check if client is a human before turning into zombie.
        if (!InfectIsClientHuman(targets[x]))
        {
            continue;
        }

        // If zombie hasn't spawned, then make targetted player(s) mother zombies.
        if (!zombiespawned)
        {
            // Turn client into a mother zombie.
            InfectHumanToZombie(targets[x], _, true, respawnoverride, respawn);

            // Log action to game events.
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Infect, "Manual Infect", "\"%L\" turned \"%L\" into a mother zombie", client, targets[x]);
            LogAction(client, -1, "[ZR] \"%L\" turned \"%L\" into a mother zombie.", client, targets[x]);

            success = true;
            continue;
        }

        // Turn client into a zombie.
        InfectHumanToZombie(targets[x], _, false, respawnoverride, respawn);

        // Log action to game events.
        LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Infect, "Manual Infect", "\"%L\" turned \"%L\" into a zombie", client, targets[x]);
        LogAction(client, -1, "[ZR] \"%L\" turned \"%L\" into a zombie.", client, targets[x]);

        success = true;
    }

    // Tell admin the outcome of the command.
    if(success)
    {
        if (!zombiespawned)
        {
            TranslationReplyToCommand(client, "Infect command infect mother successful", targetname);
            TranslationPrintToChatAllExcept(false, false, client, "Infect command infect mother successful public", adminname, targetname);
        }
        else
        {
            TranslationReplyToCommand(client, "Infect command infect successful", targetname);
            TranslationPrintToChatAllExcept(false, false, client, "Infect command infect successful public", adminname, targetname);
        }
    }
    else
    {
        // Tell admin command was unsuccessful.
        TranslationReplyToCommand(client, "Infect command infect unsuccessful", targetname);
    }
}

/**
 * Infecting a client manually (via zr_human or the "Zombie Management" menu)
 *
 * @param client    The client index changing a zombie to human.
 * @param targets   Array containing all clients to make human.
 * @param count     The number of clients in the array.
 * @param respawn   (Optional) True to respawn client upon changing to human.
 * @param protect   (Optional) True to protect client upon changing to human.
 */
stock void InfectManualHuman(int client, int[] targets, int count, bool respawn = false, bool protect = false)
{
    char targetname[MAX_NAME_LENGTH];
    char adminname[MAX_NAME_LENGTH];

    // Get admin's name for later use.
    if(client > 0)
        GetClientName(client, adminname, sizeof(adminname));
    else
        strcopy(adminname, sizeof(adminname), "Console");

    bool success = false;

    // x = Client index.
    for (int x = 0; x < count; x++)
    {
        // Get client's name for later use.
        GetClientName(targets[x], targetname, sizeof(targetname));

        // Check if client is a human before turning into zombie.
        if (InfectIsClientInfected(targets[x]))
        {
            // Turn client into a zombie.
            InfectZombieToHuman(targets[x], respawn, protect);

            // Log action to game events.
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Infect, "Manual Human", "\"%L\" turned \"%L\" into a human", client, targets[x]);
            LogAction(client, -1, "[ZR] \"%L\" turned \"%L\" into a human.", client, targets[x]);

            success = true;
        }
    }

    // Tell admin the outcome of the command.
    if (success)
    {
        TranslationReplyToCommand(client, "Infect command human successful", targetname);
        TranslationPrintToChatAllExcept(false, false, client, "Infect command human successful public", adminname, targetname);
    }
    else
    {
        TranslationReplyToCommand(client, "Infect command human unsuccessful", targetname);
    }
}

/**
 * Command callback (zr_infect)
 * Infects a client.
 *
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action InfectInfectCommand(int client, int argc)
{
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Generic))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }

    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Infect command infect syntax");
        return Plugin_Handled;
    }

    char target[MAX_NAME_LENGTH], targetname[MAX_NAME_LENGTH];
    int targets[MAXPLAYERS], result;
    bool tn_is_ml;

    // Get targetname.
    GetCmdArg(1, target, sizeof(target));

    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE | COMMAND_FILTER_NO_IMMUNITY, targetname, sizeof(targetname), tn_is_ml);

    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        ZRReplyToTargetError(client, result);
        return Plugin_Handled;
    }

    // Get respawn parameter.
    char strRespawn[8];
    GetCmdArg(2, strRespawn, sizeof(strRespawn));

    bool respawnoverride, respawn;

    // If parameter exists then cast it into a bool and feed it to infect function.
    if (strRespawn[0])
    {
        respawnoverride = true;
        respawn = view_as<bool>(StringToInt(strRespawn));
    }

    // Infect player.
    InfectManualInfect(client, targets, result, respawnoverride, respawn);

    return Plugin_Handled;
}

/**
 * Command callback (zr_human)
 * Turns a client into a human.
 *
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action InfectHumanCommand(int client, int argc)
{
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Generic))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }

    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Infect command human syntax");
        return Plugin_Handled;
    }

    char target[MAX_NAME_LENGTH], targetname[MAX_NAME_LENGTH];
    int targets[MAXPLAYERS], result;
    bool tn_is_ml;

    // Get targetname.
    GetCmdArg(1, target, sizeof(target));

    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE | COMMAND_FILTER_NO_IMMUNITY, targetname, sizeof(targetname), tn_is_ml);

    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        ZRReplyToTargetError(client, result);
        return Plugin_Handled;
    }

    // Get respawn&protect parameters
    char strRespawn[8], strProtect[8];
    GetCmdArg(2, strRespawn, sizeof(strRespawn));
    GetCmdArg(3, strProtect, sizeof(strProtect));

    // If parameter exists then cast it into a bool and feed it to "humanize" function.
    bool respawn = (strRespawn[0]) ? (view_as<bool>(StringToInt(strRespawn))) : false;
    bool protect = (strProtect[0]) ? (view_as<bool>(StringToInt(strProtect))) : false;

    // Turn client into human.
    InfectManualHuman(client, targets, result, respawn, protect);

    return Plugin_Handled;
}

/**
 * Converts a string to an infection mode.
 *
 * @param mode      Mode string to convert.
 *
 * @return          Infection mode or InfectMode_Invalid on error.
 */
InfectMode InfectStringToMode(const char[] mode)
{
    if (strlen(mode) == 0)
    {
        return InfectMode_Invalid;
    }

    if (StrEqual(mode, "dynamic", false))
    {
        return InfectMode_Dynamic;
    }
    else if (StrEqual(mode, "absolute", false))
    {
        return InfectMode_Absolute;
    }
    else if (StrEqual(mode, "range", false))
    {
        return InfectMode_Range;
    }

    return InfectMode_Invalid;
}

/**
 * Gets and validates the infection mode. On error it will log a warning.
 *
 * @return  Infection mode or InfectMode_Invalid on error.
 */
InfectMode InfectGetModeOrFail()
{
    char modeName[16];
    GetConVarString(g_hCvarsList.CVAR_INFECT_MZOMBIE_MODE, modeName, sizeof(modeName));

    InfectMode mode = InfectStringToMode(modeName);
    int ratio = GetConVarInt(g_hCvarsList.CVAR_INFECT_MZOMBIE_RATIO);
    int min = GetConVarInt(g_hCvarsList.CVAR_INFECT_MZOMBIE_MIN);
    int max = GetConVarInt(g_hCvarsList.CVAR_INFECT_MZOMBIE_MAX);

    // Validate.
    switch (mode)
    {
        case InfectMode_Invalid:
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Infect, "Config Validation", "Warning: Invalid infection mode (\"%s\"). Falling back to one mother zombie.", modeName);
        }
        case InfectMode_Dynamic:
        {
            if (ratio < 0)
            {
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Infect, "Config Validation", "Warning: Invalid infection ratio (\"%d\"). Must be zero or positive in dynamic mode. Falling back to one mother zombie.", ratio);
                return InfectMode_Invalid;
            }
        }
        case InfectMode_Absolute:
        {
            if (ratio == 0)
            {
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Infect, "Config Validation", "Warning: Invalid infection ratio (\"%d\"). Must be nonzero in absolute mode. Falling back to one mother zombie.", ratio);
                return InfectMode_Invalid;
            }
        }
        case InfectMode_Range:
        {
            bool failed = false;
            if (min <= 0)
            {
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Infect, "Config Validation", "Warning: Invalid infection range (\"%d\"). Cvar zr_infect_mzombie_min must be nonzero and positive. Falling back to one mother zombie.", min);
                failed = true;
            }
            if (max <= 0)
            {
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Infect, "Config Validation", "Warning: Invalid infection range (\"%d\"). Cvar zr_infect_mzombie_max must be nonzero and positive. Falling back to one mother zombie.", max);
                failed = true;
            }
            if (min > max || max < min)
            {
                LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Infect, "Config Validation", "Warning: Infection range values are overlapping or reversed. Check zr_infect_mzombie_min and zr_infect_mzombie_min. Falling back to one mother zombie.");
                failed = true;
            }

            if (failed)
            {
                return InfectMode_Invalid;
            }
        }
    }

    return mode;
}
