/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zspawn.inc
 *  Type:          Module
 *  Description:   Handles zspawn command, spawns late-joining clients into the game.
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
 * Conditions for zspawning players.
 */
enum ZSpawnCondition
{
    ZSpawn_Block = -1,          /** Block ZSpawn. */
    ZSpawn_Human = 0,           /** ZSpawn as a human. */
    ZSpawn_Zombie               /** ZSpawn as a zombie. */
}

/**
 * Global variable to store infect timer handle.
 */
Handle tZSpawn = INVALID_HANDLE;

/**
 * Global variable to store SteamID cache handle.
 */
Handle g_hZSpawnSteamIDCache = INVALID_HANDLE;
Handle g_hZspawnInfectedSteamIDCache = INVALID_HANDLE;

bool g_bZSpawnClientInfected[MAXPLAYERS + 1] = {false, ...};

/**
 * Create commands specific to ZSpawn.
 */
void ZSpawnOnCommandsCreate()
{
    // Register ZSpawn command.
    RegConsoleCmd(SAYHOOKS_KEYWORD_ZSPAWN, ZSpawnCommand, "Spawn into the game after joining late.");

    // Register admin command to force ZSpawn.
    RegConsoleCmd("zr_zspawn_force", ZSpawnForceCommand, "Force ZSpawn on a client. Usage: zr_zspawn_force <client> ['0' = Spawn as human | '1' = Spawn as zombie]");
}

/**
 * Map is starting.
 */
void ZSpawnOnMapStart()
{
    // Reset timer handle.
    tZSpawn = INVALID_HANDLE;

    // If SteamID cache hasn't been created yet, then create.
    if (g_hZSpawnSteamIDCache == INVALID_HANDLE)
    {
        g_hZSpawnSteamIDCache = SteamidCacheCreate();
    }
    if (g_hZspawnInfectedSteamIDCache == INVALID_HANDLE)
    {
        g_hZspawnInfectedSteamIDCache = SteamidCacheCreate();
    }

    // Reset the SteamID cache.
    SteamidCacheReset(g_hZSpawnSteamIDCache);
    SteamidCacheReset(g_hZspawnInfectedSteamIDCache);

    for(int client = 1; client <= MAXPLAYERS; client++)
        g_bZSpawnClientInfected[client] = false;
}

/**
 * Client is joining the server.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientPutInServer(int client)
{
    g_bZSpawnClientInfected[client] = SteamidCacheRemoveClient(g_hZspawnInfectedSteamIDCache, client);
}

/**
 * Client is leaving the server.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientDisconnect(int client)
{
    // So people who are connecting that click "cancel" aren't added to the list.
    if (!IsClientInGame(client))
    {
        return;
    }

    // Check if client is a bot.
    if (IsFakeClient(client))
    {
        return;
    }

    // Add client to the SteamID cache.
    SteamidCacheAddClient(g_hZSpawnSteamIDCache, client);

    // If this client was infected, remember until the next round or until they rejoin.
    if(g_bZSpawnClientInfected[client])
        SteamidCacheAddClient(g_hZspawnInfectedSteamIDCache, client);

    g_bZSpawnClientInfected[client] = false;
}

/**
 * Client has been killed.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientDeath(int client)
{
    // Add client to the SteamID cache.
    SteamidCacheAddClient(g_hZSpawnSteamIDCache, client);
}
/**
 * Client joined a class.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientClassPost(int client)
{
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }

    // If zspawn is disabled, then stop.
    bool zspawn = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN);
    if (!zspawn)
    {
        return;
    }

    // If auto zspawn is disabled, then stop.
    bool autozspawn = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN_AUTO);
    if (!autozspawn)
    {
        return;
    }

    // Is the client dead and not source-tv?
    if (!IsPlayerAlive(client) && !IsClientSourceTV(client))
    {
        // Get our zspawn condition.
        ZSpawnCondition condition = GetZSpawnCondition(client);

        switch(condition)
        {
            case ZSpawn_Human, ZSpawn_Zombie:
            {
                // ZSpawn client.
                ZSpawnClient(client, false, false, false);
            }
        }
    }
}

/**
 * Client is spawning into the game.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientSpawn(int client)
{
    // Check if client is spawning on a team.
    if (ZRIsClientOnTeam(client) && InfectHasZombieSpawned())
    {
        // Get our zspawn condition.
        ZSpawnCondition condition = GetZSpawnCondition(client);

        switch(condition)
        {
            case ZSpawn_Zombie:
            {
                // Infect client.
                InfectHumanToZombie(client);
            }
        }
    }
}

/**
 * Client has been infected.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientInfected(int client)
{
    g_bZSpawnClientInfected[client] = true;
}

/**
 * Client was turned back into a human.
 *
 * @param client    The client index.
 */
void ZSpawnOnClientHuman(int client)
{
    g_bZSpawnClientInfected[client] = false;
}

/**
 * The round is starting.
 */
void ZSpawnOnRoundStart()
{
    // Reset the SteamID cache.
    SteamidCacheReset(g_hZSpawnSteamIDCache);
    SteamidCacheReset(g_hZspawnInfectedSteamIDCache);

    for(int client = 1; client <= MAXPLAYERS; client++)
        g_bZSpawnClientInfected[client] = false;

    // If zspawn timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);

        // Reset timer handle.
        tZSpawn = INVALID_HANDLE;
    }
}

/**
 * The freeze time is ending.
 */
void ZSpawnOnRoundFreezeEnd()
{
    // If infect timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
    }

    // If zspawn is disabled, then stop.
    bool zspawn = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN);
    if (!zspawn)
    {
        return;
    }

    // If timelimit is disabled, then stop.
    bool zspawntimelimit = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN_TIMELIMIT);
    if (!zspawntimelimit)
    {
        return;
    }

    // Get timelimit
    float zspawntime = GetConVarFloat(g_hCvarsList.CVAR_ZSPAWN_TIMELIMIT_TIME);

    // Start timer.
    tZSpawn = CreateTimer(zspawntime, ZSpawnTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 */
void ZSpawnOnRoundEnd()
{
    // If zspawn timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);

        // Reset timer handle.
        tZSpawn = INVALID_HANDLE;
    }
}

/**
 * Spawns a late-joining client into the game.
 *
 * @param client    The client index.
 * @param force     (Optional) True to force spawning of the client, false to follow rules.
 * @param zombie    (Optional) If you are forcing spawn, you must override the team here.
 * @return          True if successful, false otherwise.
 */
bool ZSpawnClient(int client, bool force = false, bool zombie = false, bool error = true)
{
    // If zspawn is disabled, then stop.
    bool zspawn = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN);
    if (!force && !zspawn)
    {
        if (error)
        {
            TranslationPrintToChat(client, "Feature is disabled");
        }

        return false;
    }

    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        if (!force && error)
        {
            // Tell client the command may only be used when on a team.
            TranslationPrintToChat(client, "Must be on team");
        }

        return false;
    }

    // If client is alive, then stop.
    if (IsPlayerAlive(client))
    {
        if (!force && error)
        {
            // Tell client the command may only be used when dead.
            TranslationPrintToChat(client, "Must be dead");
        }

        return false;
    }

    // Block if client has already played during this round.
    int blockrejoin = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN_BLOCK_REJOIN);
    if (!force && SteamidCacheClientExists(g_hZSpawnSteamIDCache, client) && blockrejoin)
    {
        if (error)
        {
            // Tell client the command may only be used when joining late.
            TranslationPrintToChat(client, "ZSpawn double spawn");
        }

        return false;
    }

    // Client has been infected this round, respawning as a zombie
    bool teamzombie;

    if (!force)
    {
        // Get our zspawn condition.
        ZSpawnCondition condition = GetZSpawnCondition(client);

        switch(condition)
        {
            case ZSpawn_Block:
            {
                // Get timelimit.
                float zspawntime = GetConVarFloat(g_hCvarsList.CVAR_ZSPAWN_TIMELIMIT_TIME);

                if (error)
                {
                    // Tell client the timelimit for this command has expired.
                    TranslationPrintToChat(client, "ZSpawn timelimit", RoundToNearest(zspawntime));
                }

                return false;
            }
            case ZSpawn_Human:
            {
                teamzombie = false;
            }
            case ZSpawn_Zombie:
            {
                teamzombie = true;
            }
        }
    }
    else
    {
        // Use the override team in the function if were forcing the spawn.
        teamzombie = zombie;
    }

    // Tell respawn module to respawn client.
    return RespawnSpawnClient(client, teamzombie, false);
}

/**
 * Menu callback (zspawn_force)
 * Forces ZSpawn on a client.
 *
 * @param menu      The menu handle.
 * @param action    Action client is doing in menu.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public int ZSpawnForceHandle(Handle menu_zspawn_force, MenuAction action, int client, int slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get the client index of the selected client.
        int target = MenuGetClientIndex(menu_zspawn_force, slot);

        // If the target is 0, then the client left before being selected from the menu.
        if (target == 0)
        {
            // Re-send the menu.
            MenuClientList(client, ZSpawnForceHandle, true, true, false, "ZSpawn clients title");

            return -1;
        }

        // Get the target's name for future use.
        char targetname[MAX_NAME_LENGTH], adminname[MAX_NAME_LENGTH];
        GetClientName(target, targetname, sizeof(targetname));

        // Get admin's name for future use.
        if(client > 0)
            GetClientName(client, adminname, sizeof(adminname));
        else
            strcopy(adminname, sizeof(adminname), "Console");

        // Force ZSpawn on the target.
        bool success = ZSpawnClient(target, true);

        // Tell admin the outcome of the action.
        if (success)
        {
            // Log action to game events.
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_ZSpawn, "Force ZSpawn", "\"%L\" force zspawned \"%L\" as human", client, target);
            LogAction(client, -1, "[ZR] \"%L\" force zspawned \"%L\" as human.", client, target);

            TranslationReplyToCommand(client, "ZSpawn command force successful", targetname);
            TranslationPrintToChatAllExcept(false, false, client, "ZSpawn command force successful public", adminname, targetname, "human");
        }
        else
        {
            TranslationReplyToCommand(client, "ZSpawn command force unsuccessful", targetname);
        }

        // Re-send the menu.
        MenuClientList(client, ZSpawnForceHandle, true, false, true, "ZSpawn clients title");
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
    // Client exited menu.
    if (action == MenuAction_End)
    {
        CloseHandle(menu_zspawn_force);
    }
    return -1;
}

/**
 * Command callback (zspawn)
 * Spawn into the game after joining late.
 *
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action ZSpawnCommand(int client, int argc)
{
    // If client is console, then stop and tell them this feature is for players only.
    if (ZRIsConsole(client))
    {
        TranslationPrintToServer("Must be player");
        return Plugin_Handled;
    }

    // Spawn client.
    ZSpawnClient(client);

    // This stops the "Unknown command" message in client's console.
    return Plugin_Handled;
}

/**
 * Command callback (zr_zspawn_force)
 * Force ZSpawn on a client.
 *
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action ZSpawnForceCommand(int client, int argc)
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
        TranslationReplyToCommand(client, "ZSpawn command force syntax");
        return Plugin_Handled;
    }

    char target[MAX_NAME_LENGTH], targetname[MAX_NAME_LENGTH], adminname[MAX_NAME_LENGTH];
    int targets[MAXPLAYERS], result;
    bool tn_is_ml;

    // Get targetname.
    GetCmdArg(1, target, sizeof(target));

    // Get admin's name for later use.
    if(client > 0)
        GetClientName(client, adminname, sizeof(adminname));
    else
        strcopy(adminname, sizeof(adminname), "Console");

    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_DEAD | COMMAND_FILTER_NO_IMMUNITY, targetname, sizeof(targetname), tn_is_ml);

    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        ZRReplyToTargetError(client, result);
        return Plugin_Handled;
    }

    // Get item to give to client.
    char strZombie[4];
    GetCmdArg(2, strZombie, sizeof(strZombie));

    // Copy value of second (optional) parameter to 'zombie'.
    // It will be false if the parameter wasn't specified.
    bool zombie = view_as<bool>(StringToInt(strZombie));

    // Initialize a flag to track if the action is logged.
    bool actionLogged = false;

    // x = Client index.
    for (int x = 0; x < result; x++)
    {
        // Verify if client is in a valid team
        if (!ZRIsClientOnTeam(targets[x], CS_TEAM_T) && !ZRIsClientOnTeam(targets[x], CS_TEAM_CT))
            continue;

        // Spawn client
        bool success = ZSpawnClient(targets[x], true, zombie);
        {
            // Log to game events.
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_ZSpawn, "Force ZSpawn", "\"%L\" force zspawned \"%L\" as %s", client, targets[x], zombie ? "zombie" : "human");

            // Tell admin the outcome of the command.
            if (success)
            {
                // Execute this only if it hasn't been done before.
                if (!actionLogged)
                {
                    if (result > 1)
                        LogAction(client, -1, "[ZR] \"%L\" force zspawned \"%s\" as %s.", client, targetname, zombie ? "zombie" : "human");
                    else
                        LogAction(client, -1, "[ZR] \"%L\" force zspawned \"%L\" as %s.", client, targets[x], zombie ? "zombie" : "human");

                    TranslationReplyToCommand(client, "ZSpawn command force successful", targetname);
                    TranslationPrintToChatAllExcept(false, false, client, "ZSpawn command force successful public", adminname, targetname, zombie ? "zombie" : "human");
                    actionLogged = true;
                }
            }
            else
            {
                TranslationReplyToCommand(client, "ZSpawn command force unsuccessful", targetname);
            }
        }
    }

    return Plugin_Handled;
}

/**
 * Timer callback, resets handle.
 *
 * @param timer     The timer handle.
 */
public Action ZSpawnTimer(Handle timer)
{
    // Reset timer handle.
    tZSpawn = INVALID_HANDLE;
    return Plugin_Continue;
}

/**
 * Get ZSpawn condition.
 */
ZSpawnCondition GetZSpawnCondition(int client)
{
    ZSpawnCondition condition;

    // Check if zspawn override is enabled, and if so use overriden value.
    if (GetConVarBool(g_hCvarsList.CVAR_ZSPAWN_TEAM_OVERRIDE))
    {
        // Convert boolean to zspawn condition.
        condition = GetConVarBool(g_hCvarsList.CVAR_ZSPAWN_TEAM_ZOMBIE) ? ZSpawn_Zombie : ZSpawn_Human;
    }
    else
    {
        // Convert boolean to zspawn condition.
        condition = GetConVarBool(g_hCvarsList.CVAR_RESPAWN_TEAM_ZOMBIE) ? ZSpawn_Zombie : ZSpawn_Human;
    }

    // Check if zspawn timelimit is enabled.
    if (GetConVarBool(g_hCvarsList.CVAR_ZSPAWN_TIMELIMIT))
    {
        ZSpawnCondition zspawntimelimitzombie = view_as<ZSpawnCondition>(GetConVarInt(g_hCvarsList.CVAR_ZSPAWN_TIMELIMIT_ZOMBIE));
        // Check if zspawn timelimit is expired.
        if (!ZRIsTimerRunning(tZSpawn))
        {
            switch(zspawntimelimitzombie)
            {
                case ZSpawn_Block:
                {
                    condition = ZSpawn_Block;
                }
                case ZSpawn_Human:
                {
                    condition = ZSpawn_Human;
                    // Client has been infected this round, respawning as a zombie.
                    if(g_bZSpawnClientInfected[client])
                        condition = ZSpawn_Zombie;
                }
                case ZSpawn_Zombie:
                {
                    condition = ZSpawn_Zombie;
                }
            }
        }
        else
        {
            // Client has been infected this round, respawning as a zombie.
            if(condition == ZSpawn_Human && g_bZSpawnClientInfected[client])
                condition = ZSpawn_Zombie;
        }
    }

    return condition;
}
