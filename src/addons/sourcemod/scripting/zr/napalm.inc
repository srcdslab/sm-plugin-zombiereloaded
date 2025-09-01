/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          napalm.inc
 *  Type:          Module
 *  Description:   Grenades burn zombies when damaged by them.
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
 * The fuse length of an hegrenade.
 */
#define GRENADE_FUSE_TIME 3.0

/**
 * @section m_nWaterLevel defines.
 */
#define NAPALM_WLEVEL_DRY 0
#define NAPALM_WLEVEL_FEET 1
#define NAPALM_WLEVEL_HALF 2
#define NAPALM_WLEVEL_FULL 3
/**
 * @endsection
 */

/**
 * Variable to store water-level offset value.
 */
int g_iToolsWaterLevel;

/**
 * Find napalm-specific offsets here.
 */
void NapalmOnOffsetsFound()
{
    // If offset "m_bInBuyZone" can't be found, then stop the plugin.
    g_iToolsWaterLevel = FindSendPropInfo("CBasePlayer", "m_nWaterLevel");
    if (g_iToolsWaterLevel == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Napalm, "Offsets", "Offset \"CBaseEntity::m_nWaterLevel\" was not found.");
    }
}

/**
 * Hook: OnTakeDamage
 * Forwarded from the damage module to check if we should extinguish any flames.
 *
 * @param client        The client index.
 * @param damagetype    The type of damage inflicted.
 * @return              Return Plugin_Handled to stop the damage to client.
 *                      Plugin_Continue to allow damage to client.
 *                      Return -1 to not return anything and let the damage module continue as usual.
 */
int NapalmOnTakeDamage(int client, int damagetype)
{
    // Client was damaged by fire.
    // TODO: Is damage type different in CS:S and CS:GO (it was checking only DMG_DIRECT for some reason). Checking both.
    if (damagetype & DMG_BURN || damagetype & DMG_DIRECT)
    {
        // Only take action if it isn't disabled, or the option is valid.
        int douse = g_hCvarsList.CVAR_NAPALM_DOUSE.IntValue;

        if (douse > NAPALM_WLEVEL_DRY && douse <= NAPALM_WLEVEL_FULL)
        {
            // If the client water-level is equal or higher than the given, then we want to extinguish the flame.
            if (NapalmGetClientWaterLevel(client) >= douse)
            {
                // Put the fire out.
                NapalmExtinguishEntity(client);

                return view_as<int>(Plugin_Continue);
            }
        }
    }

    // Let the damage module continue as usual.
    return -1;
}

/**
 *  Client has been hurt.
 *
 * @param client    The client index.
 * @param attacker  The attacker index.
 * @param weapon    The weapon name.
 */
void NapalmOnClientHurt(int client, int attacker, const char[] weapon, int dmg_health)
{
    // If there's no attacker, then stop.
    if (!attacker)
    {
        return;
    }

    // If player isn't a zombie, then stop.
    if (!InfectIsClientInfected(client))
    {
        return;
    }

    // If napalm time is invalid or 0, then stop.
    float napalm_time = ClassGetNapalmTime(client);
    if (napalm_time <= 0.0)
    {
        return;
    }

    // If the attacker can't throw napalm grenades, then stop.
    if (!ClassGetHasNapalm(attacker))
    {
        return;
    }

    // If weapon is a grenade, then ignite player.
    if (strcmp(weapon, "hegrenade", false) == 0 || strcmp(weapon, "inferno", false) == 0)
    {
        bool reset = g_hCvarsList.CVAR_NAPALM_TIME_RESET.BoolValue;
        int flags = GetEntityFlags(client);

        if (reset || !(flags & FL_ONFIRE))
        {
            float timescale = g_hCvarsList.CVAR_NAPALM_TIME_SCALE.FloatValue;
            if (timescale > 0.0)
            {
                // Figure out our scale value.
                timescale = dmg_health / timescale;

                // Make sure we dont go over 100%.
                if (timescale > 1.0)
                    timescale = 1.0;

                // Make sure we dont go below 0%.
                if (timescale < 0.0)
                    timescale = 0.0;

                // Scale our class time, accordingly.
                napalm_time = napalm_time * timescale;
            }

            // Ignite the client or extend the current flames life.
            NapalmIgniteEntity(client, napalm_time);
        }
    }
}

/**
 * Client has been killed.
 *
 * @param client    The client index.
 */
void NapalmOnClientDeath(int client)
{
    // Extinguish any flames to stop burning sounds.
    NapalmExtinguishEntity(client);
}

/**
 * When an entity is created.
 *
 * @param entity      Entity index
 * @param classname   Class name
 */
void NapalmOnEntitySpawned(int entity, const char[] classname)
{
    // Not a grenade.
    if (strcmp(classname, "hegrenade_projectile", false))
    {
        return;
    }

    // If grenade fire is disabled, then stop.
    bool napalmignite = g_hCvarsList.CVAR_NAPALM_IGNITE.BoolValue;
    if (!napalmignite)
    {
        return;
    }

    // Get client who threw this grenade.
    int client = GetEntPropEnt(entity, Prop_Send, "m_hOwnerEntity");

    // Invalid client.
    if (client > MaxClients || client <= 0)
    {
        return;
    }

    // If human class can't throw napalm grenades, then stop.
    if (!ClassGetHasNapalm(client))
    {
        return;
    }

    // Ignite the grenade.
    IgniteEntity(entity, GRENADE_FUSE_TIME);
}

/**
 * Checks the current water-level on a client.
 *
 * @param client    The client index.
 * @return          A NAPALM_WLEVEL_ define.
 */
stock int NapalmGetClientWaterLevel(int client)
{
    // Return client's water-level.
    return GetEntData(client, g_iToolsWaterLevel);
}

/**
 * Actually extinguishes an entity.
 *
 * @param client    The client index.
 */
stock void NapalmExtinguishEntity(int client)
{
    //ExtinguishEntity(client);  <-- Don't use this.  Takes off the FL_ONFIRE flag, but flame doesn't get extinguished.

    // This works.
    int fire = GetEntPropEnt(client, Prop_Data, "m_hEffectEntity");
    if (IsValidEntity(fire))
    {
        // Make sure the entity is a flame, so we can extinguish it.
        char classname[64];
        GetEdictClassname(fire, classname, sizeof(classname));
        if (strcmp(classname, "entityflame", false)  == 0)
        {
            SetEntPropFloat(fire, Prop_Data, "m_flLifetime", 0.0);
        }
        // Log what entity was in that property, for future reference.
        else
        {
            LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Napalm, "Napalm Douse", "Found unexpected entity in prop \"m_hEffectEntity\": \"%s\"", classname);
        }
    }
}

/**
 * Ignites an entity or changes the current flames lifetime.
 *
 * @param client    The client index.
 * @param time      Number of seconds to set on fire.
 */
stock void NapalmIgniteEntity(int client, float time)
{
    Action result = APIOnClientIgnite(client, time);
    // Check if infection should be blocked.
    if (result == Plugin_Handled)
    {
        return;
    }

    int fire = GetEntPropEnt(client, Prop_Data, "m_hEffectEntity");
    if (IsValidEntity(fire))
    {
        // Make sure the entity is a flame.
        char classname[64];
        GetEdictClassname(fire, classname, sizeof(classname));
        if (strcmp(classname, "entityflame", false) == 0)
        {
            SetEntPropFloat(fire, Prop_Data, "m_flLifetime", GetGameTime() + time);
            APIOnClientIgnited(client, time);
            return;
        }
    }

    // No flame entity found, ignite client.
    IgniteEntity(client, time);
    APIOnClientIgnited(client, time);
}
