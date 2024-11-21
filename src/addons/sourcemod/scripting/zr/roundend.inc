/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          roundend.inc
 *  Type:          Core
 *  Description:   Handles round end actions.
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

#if defined REQUIRE_EXTENSIONS
 #define TEMP_REQUIRE_EXTENSIONS
 #undef REQUIRE_EXTENSIONS
#endif

#tryinclude "sourcetvmanager.inc"

/* Restore old REQUIRE_EXTENSIONS value if necessary */
#if defined TEMP_REQUIRE_EXTENSIONS
 #define REQUIRE_EXTENSIONS
 #undef TEMP_REQUIRE_EXTENSIONS
#endif

/**
 * @section All round end reasons.
 */
enum e_RoundEndReason_CSGO
{
    /*
    NOTE/WARNING: these enum values are stored in demo files,
    they are explicitly numbered for consistency and editing,
    do not renumber existing elements, always add new elements
    with different numeric values!
    */
    Invalid_Round_End_Reason_CSGO = -1,
    RoundEndReason_StillInProgress_CSGO,
    Target_Bombed_CSGO,                      // Target Successfully Bombed!
    VIP_Escaped_CSGO,						// The VIP has escaped!
    VIP_Assassinated_CSGO,                   // VIP has been assassinated!
    Terrorists_Escaped_CSGO,                 // The terrorists have escaped!
    CTs_PreventEscape_CSGO,                  // The CT's have prevented most of the terrorists from escaping!
    Escaping_Terrorists_Neutralized_CSGO,    // Escaping terrorists have all been neutralized!
    Bomb_Defused_CSGO,                       // The bomb has been defused!
    CTs_Win_CSGO,                            // Counter-Terrorists Win!
    Terrorists_Win_CSGO,                     // Terrorists Win!
    Round_Draw_CSGO,                         // Round Draw!
    All_Hostages_Rescued_CSGO,               // All Hostages have been rescued!
    Target_Saved_CSGO,                       // Target has been saved!
    Hostages_Not_Rescued_CSGO,               // Hostages have not been rescued!
    Terrorists_Not_Escaped_CSGO,             // Terrorists have not escaped!
    VIP_Not_Escaped_CSGO,                    // VIP has not escaped!
    Game_Commencing_CSGO,                    // Game Commencing!
    Terrorists_Surrender_CSGO,
    CTs_Surrender_CSGO,
    Terrorists_Planted_CSGO,
    CTs_ReachedHostage_CSGO,
    RoundEndReason_Count_CSGO
};

enum e_RoundEndReason
{
    Invalid_Round_End_Reason = -1,
    Target_Bombed,                      // Target Successfully Bombed!
    VIP_Escaped,						// The VIP has escaped!
    VIP_Assassinated,                   // VIP has been assassinated!
    Terrorists_Escaped,                 // The terrorists have escaped!
    CTs_PreventEscape,                  // The CT's have prevented most of the terrorists from escaping!
    Escaping_Terrorists_Neutralized,    // Escaping terrorists have all been neutralized!
    Bomb_Defused,                       // The bomb has been defused!
    CTs_Win,                            // Counter-Terrorists Win!
    Terrorists_Win,                     // Terrorists Win!
    Round_Draw,                         // Round Draw!
    All_Hostages_Rescued,               // All Hostages have been rescued!
    Target_Saved,                       // Target has been saved!
    Hostages_Not_Rescued,               // Hostages have not been rescued!
    Terrorists_Not_Escaped,             // Terrorists have not escaped!
    VIP_Not_Escaped,                    // VIP has not escaped!
    Game_Commencing,                    // Game Commencing!
    RoundEndReason_Count
};
/**
 * @endsection
 */

/**
 * Delay between round ending and new round starting. (Normal)
 */
#define ROUNDEND_DELAY 5.0

/**
 * Possible round end outcomes.
 */
enum RoundEndOutcome
{
    Restart,        /** Round is restarting. */
    Draw,           /** Round has ended in unexpected way. */
    HumansWin,      /** Humans have killed all zombies. */
    ZombiesWin,     /** Zombies have infected all humans. */
}

/**
 * Global variable to store round win timer handle.
 */
Handle g_tRoundEnd = INVALID_HANDLE;

bool g_SourceTVManagerLoaded = false;

/**
 * All plugins have finished loading.
 */
void RoundEndOnAllPluginsLoaded()
{
    #if defined _stvmngr_included
    g_SourceTVManagerLoaded = LibraryExists("sourcetvmanager");
    LogMessage("SourceTV Manager: %s", (g_SourceTVManagerLoaded ? "loaded" : "not loaded"));
    #endif
}

/**
 * A library was added.
 */
void RoundEndOnLibraryAdded(const char[] name)
{
    if (strcmp(name, "sourcetvmanager", false) == 0)
    {
        // sourcetvmanager loaded.
        g_SourceTVManagerLoaded = true;
    }
}

/**
 * A library was removed.
 */
void RoundEndOnLibraryRemoved(const char[] name)
{
    if (strcmp(name, "sourcetvmanager") == 0)
    {
        // sourcetvmanager unloaded.
        g_SourceTVManagerLoaded = false;
    }
}

/**
 * Map is starting.
 */
void RoundEndOnMapStart()
{
    // Reset timer handle.
    g_tRoundEnd = INVALID_HANDLE;
}

/**
 * Client has been killed.
 */
void RoundEndOnClientDeath()
{
    // Terminate the round if the last player was killed.
    RoundEndOutcome outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(ROUNDEND_DELAY, outcome);
    }
}

/**
 * Client has been infected.
 */
void RoundEndOnClientInfected()
{
    // Terminate the round if the last player was infected.
    RoundEndOutcome outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(ROUNDEND_DELAY, outcome);
    }
}

/**
 * The round is starting.
 */
void RoundEndOnRoundStart()
{
    // Stop all overlays.
    RoundEndOverlayStop();

    // If round end timer is running, then kill it.
    if (g_tRoundEnd != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(g_tRoundEnd);

        // Reset timer handle.
        g_tRoundEnd = INVALID_HANDLE;
    }
}

/**
 * The freeze time is ending.
 */
void RoundEndOnRoundFreezeEnd()
{
    // Calculate round length, in seconds.
    // Get mp_roundtime. (in minutes)
    float roundtime = GetConVarFloat(FindConVar("mp_roundtime"));

    // Convert to seconds.
    roundtime *= 60.0;

    // Subtract one second if running CS: GO to prevent round draw when round
    // ends. For some reason the timing doesn't match the actual round end.
    // Thanks to Jargon.
    if (g_Game == Game_CSGO)
    {
        roundtime--;
    }

    // Start timer.
    g_tRoundEnd = CreateTimer(roundtime, RoundEndTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 *
 * @param reason    Reason the round has ended.
 */
void RoundEndOnRoundEnd(int reason)
{
    // If round end timer is running, then kill it.
    if (g_tRoundEnd != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(g_tRoundEnd);

        // Reset timer handle.
        g_tRoundEnd = INVALID_HANDLE;
    }

    // Tell plugin no zombies have been spawned.
    g_bZombieSpawned = false;

    // Get outcome of the round.
    RoundEndOutcome outcome = RoundEndReasonToOutcome(reason);

    // Update team scores.
    int teamscore;
    switch(outcome)
    {
        // Zombies won the round.
        case ZombiesWin:
        {
            // Increment T score.
            teamscore = GetTeamScore(CS_TEAM_T);
            SetTeamScore(CS_TEAM_T, ++teamscore);
        }
        // Humans won the round.
        case HumansWin:
        {
            // Increment CT score.
            teamscore = GetTeamScore(CS_TEAM_CT);
            SetTeamScore(CS_TEAM_CT, ++teamscore);
        }
    }

    // Display the overlay to all clients.
    RoundEndOverlayStart(outcome);

    RoundEndDisplayStats();

    // Balance teams if enabled.
    if (GetConVarBool(g_hCvarsList.CVAR_ROUNDEND_BALANCE_TEAMS))
    {
        RoundEndBalanceTeams();
    }
}

void RoundEndDisplayStats()
{
    AuthIdType authType = view_as<AuthIdType>(GetConVarInt(g_hCvarsList.CVAR_ROUNDEND_AUTHID_TYPE));

    for(int player = 1; player <= MaxClients; player++)
    {
        if(!IsClientInGame(player) || (IsFakeClient(player) && !IsClientSourceTV(player)))
            continue;

        static char sPlayerID[8];
        static char sPlayerName[MAX_NAME_LENGTH + 2];
        static char sPlayerAuth[24];
        static char sPlayerTeam[8];
        static char sPlayerState[8];

        FormatEx(sPlayerID, sizeof(sPlayerID), "%d", GetClientUserId(player));
        FormatEx(sPlayerName, sizeof(sPlayerName), "\"%N\"", player);

        if(!GetClientAuthId(player, authType, sPlayerAuth, sizeof(sPlayerAuth)))
            FormatEx(sPlayerAuth, sizeof(sPlayerAuth), "STEAM_ID_PENDING");

        if(IsPlayerAlive(player))
            FormatEx(sPlayerState, sizeof(sPlayerState), "alive");
        else
            FormatEx(sPlayerState, sizeof(sPlayerState), "dead");

        if(InfectIsClientInfected(player))
            FormatEx(sPlayerTeam, sizeof(sPlayerTeam), "zombie");
        else
            FormatEx(sPlayerTeam, sizeof(sPlayerTeam), "human");

        for(int client = 1; client <= MaxClients; client++)
        {
            if(!IsClientInGame(client))
                continue;

            PrintToConsole(client, "# %8s %40s %24s %5s %6s",
                sPlayerID, sPlayerName, sPlayerAuth, sPlayerState, sPlayerTeam);
        }

        #if defined _stvmngr_included
        if(g_SourceTVManagerLoaded)
        {
            SourceTV_PrintToDemoConsole("# %8s %40s %24s %5s %6s",
                sPlayerID, sPlayerName, sPlayerAuth, sPlayerState, sPlayerTeam);
        }
        #endif
    }
}

/**
 * Convert a round_end reason, to a round winner, or draw.
 *
 * @param reason    The round_end reason.
 * @return          The winner of the round. (see enum RoundEndOutcome)
 */
RoundEndOutcome RoundEndReasonToOutcome(int reason)
{
    // CTs won the round.
    if ((g_Game != Game_CSGO && view_as<e_RoundEndReason>(reason) == CTs_Win)
    || (g_Game == Game_CSGO && view_as<e_RoundEndReason_CSGO>(reason) == CTs_Win_CSGO))
        return HumansWin;

    // Ts won the round.
    if ((g_Game != Game_CSGO && view_as<e_RoundEndReason>(reason) == Terrorists_Win)
    || (g_Game == Game_CSGO && view_as<e_RoundEndReason_CSGO>(reason) == Terrorists_Win_CSGO))
        return ZombiesWin;

    return HumansWin;
}

/**
 * Timer callback, called when round time reaches 0.
 *
 * @param timer     The timer handle.
 */
public Action RoundEndTimer(Handle timer)
{
    // Set the global timer handle variable to INVALID_HANDLE.
    g_tRoundEnd = INVALID_HANDLE;

    // If there aren't clients on both teams, then stop.
    if (!ZRTeamHasClients())
    {
        return Plugin_Continue;
    }

    bool zombies_win = GetConVarBool(g_hCvarsList.CVAR_ROUNDEND_ZOMBIES_WIN);
    if (zombies_win)
        RoundEndTerminateRound(ROUNDEND_DELAY, ZombiesWin);
    else
        RoundEndTerminateRound(ROUNDEND_DELAY, HumansWin);
    return Plugin_Continue;
}

/**
 * Checks if the round is over.
 *
 * @param outcome   Set to the outcome of the round, if round is over.
 * @return          True if the round is over, false otherwise.
 */
bool RoundEndGetRoundStatus(RoundEndOutcome &outcome)
{
    // If zombie hasn't spawned, then stop.
    if (!InfectHasZombieSpawned())
    {
        // Round isn't over.
        return false;
    }

    // Initialize count variables
    int zombiecount;
    int humancount;

    // Count valid clients. (true to only allow living clients)
    ZRCountValidClients(zombiecount, humancount, true);

    // If there are no clients on either teams, then stop.
    if (!zombiecount && !humancount)
    {
        // Round isn't active.
        return false;
    }

    // If there are clients on both teams, then stop.
    if (zombiecount && humancount)
    {
        // Round isn't over.
        return false;
    }

    // We know here, that either zombiecount or humancount is 0. (not both)

    // If there are zombies, then zombies won the round.
    if (zombiecount)
    {
        outcome = ZombiesWin;
    }
    // If there are no zombies, that means there must be humans, they win the round.
    else
    {
        outcome = HumansWin;
    }

    // Round is over.
    return true;
}

/**
 * Ends the round with the given outcome and delay.
 *
 * @param delay     Delay before new round starts.
 * @param outcome   The outcome of the round.
 */
void RoundEndTerminateRound(float delay, RoundEndOutcome outcome = Restart)
{
    switch(outcome)
    {
        // Round is restarting.
        case Restart:
        {
            CS_TerminateRound(delay, CSRoundEnd_GameStart, false);
        }
        // Round was a draw.
        case Draw:
        {
            CS_TerminateRound(delay, CSRoundEnd_Draw, false);
        }
        // Zombies won.
        case ZombiesWin:
        {
            CS_TerminateRound(delay, CSRoundEnd_TerroristWin, false);
        }
        // Humans won.
        case HumansWin:
        {
            CS_TerminateRound(delay, CSRoundEnd_CTWin, false);
        }
    }
}

/**
 * Balances teams.
 */
void RoundEndBalanceTeams()
{
    // Create eligible player list.
    Handle arrayEligibleClients = CreateArray();
    int eligibleclients = ZRCreateEligibleClientList(arrayEligibleClients, true);

    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        // Destroy handle.
        CloseHandle(arrayEligibleClients);

        return;
    }

    int client;

    // Move all clients to T

    // x = Array index.
    // client = client index.
    for (int x = 0; x < eligibleclients; x++)
    {
        // Get client stored in array index.
        client = GetArrayCell(arrayEligibleClients, x);

        // Switch client to T
        CS_SwitchTeam(client, CS_TEAM_T);
    }

    // Move every other client back to CT

    // x = array index
    // client = client index.
    for (int x = 0; x < eligibleclients; x += 2)
    {
        // Get client stored in array index.
        client = GetArrayCell(arrayEligibleClients, x);

        // Switch client to CT
        CS_SwitchTeam(client, CS_TEAM_CT);
    }

    // Destroy handle.
    CloseHandle(arrayEligibleClients);
}

/**
 * Displays overlays to clients, depending on the outcome.
 *
 * @param time      Time to display overlays.
 * @param outcome   The outcome of the round.
 */
void RoundEndOverlayStart(RoundEndOutcome outcome)
{
    // If round end overlays are disabled, then stop.
    bool overlay = GetConVarBool(g_hCvarsList.CVAR_ROUNDEND_OVERLAY);
    if (!overlay)
    {
        return;
    }

    char overlaypath[PLATFORM_MAX_PATH];

    switch(outcome)
    {
        // Show "zombies win" overlay.
        case ZombiesWin:
        {
            GetConVarString(g_hCvarsList.CVAR_ROUNDEND_OVERLAY_ZOMBIE, overlaypath, sizeof(overlaypath));
        }
        // Show "humans win" overlay.
        case HumansWin:
        {
            GetConVarString(g_hCvarsList.CVAR_ROUNDEND_OVERLAY_HUMAN, overlaypath, sizeof(overlaypath));
        }
        // Show no overlay.
        default:
        {
            strcopy(overlaypath, sizeof(overlaypath), "");
        }
    }

    // x = client index.
    for (int x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }

        // If client is fake (or bot), then stop.
        if (IsFakeClient(x))
        {
            continue;
        }

        OverlaysClientSetChannelPath(x, OVERLAYS_CHANNEL_ROUNDEND, overlaypath);
        OverlaysClientSetChannelState(x, OVERLAYS_CHANNEL_ROUNDEND, true, false, true);
    }
}

void RoundEndOverlayStop()
{
    // x = client index.
    for (int x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }

        // If client is fake (or bot), then stop.
        if (IsFakeClient(x))
        {
            continue;
        }

        // Disable roundend overlay channel.
        OverlaysClientSetChannelState(x, OVERLAYS_CHANNEL_ROUNDEND, true, false, false, true);
    }
}