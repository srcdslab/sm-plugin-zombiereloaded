/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          knockback.inc
 *  Type:          Module
 *  Description:   Handles knockback on clients.
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

float g_fKnockbackScale[MAXPLAYERS + 1];
float g_fKnockbackForceLimit[MAXPLAYERS + 1];
bool g_bKnockbackFrame[MAXPLAYERS + 1];
bool g_bKnockbackFrameLimitVel[MAXPLAYERS + 1];
float g_fKnockbackVectors[MAXPLAYERS + 1][3];
float g_fKnockbackVelLimit[MAXPLAYERS + 1] = {-1.0, ...};

/**
 * Client is joining the server.
 *
 * @param client    The client index.
 */
void KnockbackClientInit(int client)
{
    g_fKnockbackScale[client] = 1.0;

    g_fKnockbackForceLimit[client] = 0.0;
    g_bKnockbackFrame[client] = false;
    g_bKnockbackFrameLimitVel[client] = false;
    g_fKnockbackVectors[client][0] = 0.0;
    g_fKnockbackVectors[client][1] = 0.0;
    g_fKnockbackVectors[client][2] = 0.0;

    g_fKnockbackVelLimit[client] = -1.0;
}

/**
 * Client is leaving the server.
 *
 * @param client    The client index.
 */
void KnockbackOnClientDisconnect(int client)
{
    KnockbackClientInit(client);
}

/**
 * Client died.
 */
void KnockbackOnClientDeath(int client)
{
    KnockbackClientInit(client);
}

void KnockbackSetClientScale(int client, float fScale)
{
    g_fKnockbackScale[client] = fScale;
}

void KnockbackSetClientMaxForce(int client, float fForce)
{
    if(g_fKnockbackForceLimit[client] >= 0.0)
        g_fKnockbackForceLimit[client] = fForce;
}

void KnockbackSetClientMaxVelocity(int client, float fVelocity)
{
    g_fKnockbackForceLimit[client] = fVelocity;
}

/**
 * Called before every server frame.  Note that you should avoid
 * doing expensive computations or declaring large local arrays.
 */
void KnockbackOnGameFrame()
{
    for(int client = 1; client <= MaxClients; client++)
    {
        if (g_bKnockbackFrame[client] && g_fKnockbackForceLimit[client] > 0.0)
        {
            // Get the knockback force
            float magnitude = GetVectorLength(g_fKnockbackVectors[client]);

            // ... and limit it if it's too high.
            if(magnitude > g_fKnockbackForceLimit[client])
            {
                ScaleVector(g_fKnockbackVectors[client], g_fKnockbackForceLimit[client] / magnitude);
            }

            // Apply the knockback.
            KnockbackApplyVector(client, g_fKnockbackVectors[client], g_bKnockbackFrameLimitVel[client]);

            g_bKnockbackFrame[client] = false;
            g_bKnockbackFrameLimitVel[client] = false;
            g_fKnockbackVectors[client][0] = 0.0;
            g_fKnockbackVectors[client][1] = 0.0;
            g_fKnockbackVectors[client][2] = 0.0;
        }
    }
}

/**
 * Hook: KnockbackOnClientHurt
 * Called after client has been hurt.
 *
 * @param victim        The client index.
 * @param attacker      The client index of the attacker.
 * @param inflictor     The entity index of the inflictor.
 * @param weaponname    The weapon used.
 * @param hitgroup      Hitgroup attacker has damaged.
 * @param damage        The amount of damage inflicted.
 */
public void KnockbackOnClientHurt(int victim, int attacker, int inflictor, const char[] weaponname, int hitgroup, float damage, int damagetype, int weapon, int damagecustom)
{
    // Client is a human, then stop.
    if (InfectIsClientHuman(victim))
    {
        return;
    }

    // If attacker is a zombie, then stop.
    if (InfectIsClientInfected(attacker))
    {
        return;
    }

    // Block knock back if an immunity mode is handling this.
    if (ImmunityOnClientKnockBack(victim))
    {
        return;
    }

    // Get zombie knockback value.
    float knockback = ClassGetKnockback(victim);

    float clientloc[3];
    float attackerloc[3];

    GetClientAbsOrigin(victim, clientloc);

    // Check if a grenade was thrown.
    if (strcmp(weaponname, "hegrenade", false) == 0)
    {
        // Get the location of the grenade.
        if (KnockbackFindExplodingGrenade(attackerloc) == -1)
        {
            // If the grenade wasn't found, then stop.
            return;
        }
    }
    else
    {
        // Get attackers eye position.
        GetClientEyePosition(attacker, attackerloc);

        // Get attackers eye angles.
        float attackerang[3];
        GetClientEyeAngles(attacker, attackerang);

        // Calculate knockback end-vector.
        TR_TraceRayFilter(attackerloc, attackerang, MASK_ALL, RayType_Infinite, KnockbackTRFilter);
        TR_GetEndPosition(clientloc);
    }

    bool weapons = GetConVarBool(g_hCvarsList.CVAR_WEAPONS);
    if (weapons)
    {
        int weaponindex = WeaponsNameToIndex(weaponname);
        if (weaponindex != -1)
        {
            // Apply weapon knockback multiplier.
            knockback *= WeaponsGetKnockback(weaponindex);
        }
    }

    bool custom = view_as<bool>(damagecustom & ZR_KNOCKBACK_CUSTOM);
    bool scale = custom && view_as<bool>(damagecustom & ZR_KNOCKBACK_SCALE);
    bool limitforce = custom && view_as<bool>(damagecustom & ZR_KNOCKBACK_LIMITFORCE);
    bool limitvel = custom && view_as<bool>(damagecustom & ZR_KNOCKBACK_LIMITVEL);

    if (custom)
        ToolsSetClientLastHitGroup(victim, HITGROUP_GENERIC);

    bool hitgroups = GetConVarBool(g_hCvarsList.CVAR_HITGROUPS);
    if (hitgroups)
    {
        int hitgroupindex = HitgroupToIndex(hitgroup);
        if (hitgroupindex != -1)
        {
            // Apply hitgroup knockback multiplier.
            knockback *= HitgroupsGetKnockback(hitgroupindex);
        }
    }

    // Apply damage knockback multiplier.
    knockback *= damage;

    // Custom knockback scale.
    if (scale)
        knockback *= g_fKnockbackScale[victim];

    // Apply knockback.
    if (knockback)
        KnockbackSetVelocity(victim, attackerloc, clientloc, knockback, limitforce, limitvel);
}

/**
 * Sets velocity on a player.
 *
 * @param client        The client index.
 * @param startpoint    The starting coordinate to push from.
 * @param endpoint      The ending coordinate to push towards.
 * @param magnitude     Magnitude of the push.
 */
void KnockbackSetVelocity(int client, const float startpoint[3], const float endpoint[3], float magnitude, bool limitforce, bool limitvel)
{
    // Create vector from the given starting and ending points.
    float vector[3];
    MakeVectorFromPoints(startpoint, endpoint, vector);

    // Normalize the vector (equal magnitude at varying distances).
    NormalizeVector(vector, vector);

    // Apply the magnitude by scaling the vector (multiplying each of its components).
    ScaleVector(vector, magnitude);

    if (limitforce && g_fKnockbackForceLimit[client])
    {
        AddVectors(g_fKnockbackVectors[client], vector, g_fKnockbackVectors[client]);
        g_bKnockbackFrame[client] = true;
        g_bKnockbackFrameLimitVel[client] |= limitvel;
        return;
    }

    KnockbackApplyVector(client, vector, limitvel);
}

void KnockbackApplyVector(int client, float vector[3], bool limitvel)
{
    // Get their current velocity.
    float velocity[3];
    ToolsGetClientVelocity(client, velocity);

    // Calculate their speed.
    float magnitude_pre = GetVectorLength(velocity);

    // Add the knockback to their velocity.
    AddVectors(vector, velocity, velocity);

    // Should we limit their knockback velocity?
    float maxvel = GetConVarFloat(g_hCvarsList.CVAR_KNOCKBACK_MAXVEL);
    if (limitvel && g_fKnockbackVelLimit[client] >= 0.0 || maxvel > 0.0)
    {
        if(g_fKnockbackVelLimit[client] >= 0.0 && g_fKnockbackVelLimit[client] < maxvel)
            maxvel = g_fKnockbackVelLimit[client];

        // Calculate their new speed.
        float magnitude_post = GetVectorLength(velocity);

        // Did we actually push them back or just slow them down?
        if (magnitude_post > magnitude_pre)
        {
            // Would their knockback velocity be higher than wanted?
            if(magnitude_post > maxvel)
            {
                // ... then scale it down.
                ScaleVector(velocity, maxvel / magnitude_post);
            }
        }
    }

    // Set the new client's velocity.
    ToolsSetClientVelocity(client, velocity);
}

/**
 * Trace Ray forward, used as a filter to continue tracing if told so. (See sdktools_trace.inc)
 *
 * @param entity        The entity index.
 * @param contentsMask  The contents mask.
 * @return              True to allow hit, false to continue tracing. 
 */
public bool KnockbackTRFilter(int entity, int contentsMask)
{
    // If entity is a player, continue tracing.
    if (entity > 0 && entity < MAXPLAYERS)
    {
        return false;
    }

    // Allow hit.
    return true;
}

/**
 * Find the location of an exploding grenade (currently inflicting damage in player_hurt).
 *
 * @param heLoc     The location of the exploding grenade.
 * @return          The entity index of the grenade.
 */
int KnockbackFindExplodingGrenade(float heLoc[3])
{
    char classname[64];

    // Find max entities and loop through all of them.
    int maxentities = GetMaxEntities();
    for (int x = MaxClients; x <= maxentities; x++)
    {
        // If entity is invalid, then stop.
        if (!IsValidEdict(x))
        {
            continue;
        }

        // If entity isn't a grenade, then stop.
        GetEdictClassname(x, classname, sizeof(classname));
        if (strcmp(classname, "hegrenade_projectile", false) != 0)
        {
            continue;
        }

        // If m_takedamage is set to 0, we found our grenade.
        int takedamage = GetEntProp(x, Prop_Data, "m_takedamage");
        if (takedamage == 0)
        {
            // Return its location.
            GetEntPropVector(x, Prop_Send, "m_vecOrigin", heLoc);

            // Return its entity index.
            return x;
        }
    }

    // Didn't find the grenade.
    return -1;
}
