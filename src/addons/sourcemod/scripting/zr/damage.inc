/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          damage.inc
 *  Type:          Core
 *  Description:   Modify damage stuff here.
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
 * @section Suicide intercept defines.
 */
#define DAMAGE_SUICIDE_MAX_CMDS 5
#define DAMAGE_SUICIDE_MAX_LENGTH 16
/**
 * @endsection
 */

/**
 * Array to keep track of normal/mother zombies.
 */
bool g_bDamageMotherZombie[MAXPLAYERS + 1];

/**
 * Hook commands related to damage here.
 * Note: This isn't OnCommandsHook because this depends on cvars.
 */
bool g_bSuicideCmdsHooked = false;
void DamageLoad()
{
    if (g_bSuicideCmdsHooked)
    {
        return;
    }

    // Create command callbacks (intercepts) for listed suicide commands.
    char suicidecmds[DAMAGE_SUICIDE_MAX_CMDS * DAMAGE_SUICIDE_MAX_LENGTH];
    GetConVarString(g_hCvarsList.CVAR_DAMAGE_SUICIDE_CMDS, suicidecmds, sizeof(suicidecmds));

    // Create array to store cmds
    char arrayCmds[DAMAGE_SUICIDE_MAX_CMDS][DAMAGE_SUICIDE_MAX_LENGTH];

    // Explode string into array indexes.
    int cmdcount = ExplodeString(suicidecmds, ",", arrayCmds, sizeof(arrayCmds), sizeof(arrayCmds[]));

    // x = Array index.
    // arrayCmds[x] = suicide command.
    for (int x = 0; x <= cmdcount - 1; x++)
    {
        // Trim whitespace.
        TrimString(arrayCmds[x]);

        // Prepare intercept for this command.
        AddCommandListener(DamageSuicideIntercept, arrayCmds[x]);
    }

    // Important: If ZR can be unloaded some day, make sure to remove the listeners and set this to false.
    g_bSuicideCmdsHooked = true;
}

/**
 * Client is joining the server.
 *
 * @param client    The client index.
 */
void DamageClientInit(int client)
{
    // Hook damage callbacks.
    SDKHook(client, SDKHook_TraceAttack, DamageTraceAttack);
    SDKHook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
    SDKHook(client, SDKHook_OnTakeDamageAlivePost, DamageOnTakeDamageAlivePost);
}

/**
 * Client is leaving the server.
 *
 * @param client    The client index.
 */
void DamageOnClientDisconnect(int client)
{
    SDKUnhook(client, SDKHook_TraceAttack, DamageTraceAttack);
    SDKUnhook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
    SDKUnhook(client, SDKHook_OnTakeDamageAlivePost, DamageOnTakeDamageAlivePost);
}

/**
 * A client was infected.
 *
 * @param client        The client index.
 * @param motherinfect  True if the zombie is mother, false if not.
 */
void DamageOnClientInfected(int client, bool motherinfect)
{
    // Update if client is a mother zombie or not.
    g_bDamageMotherZombie[client] = motherinfect;
}

/**
 * Hook: TraceAttack
 * Called right before the bullet enters a client.
 *
 * @param client        The client index.
 * @param attacker      The client index of the attacker.
 * @param inflictor     The entity index of the inflictor.
 * @param damage        The amount of damage inflicted.
 * @param hitbox        The hitbox index.
 * @param hitgroup      The hitgroup index.
 * @return              Return Plugin_Handled to stop bullet from hitting client.
 *                      Plugin_Continue to allow bullet to hit client.
 */
public Action DamageTraceAttack(int client, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return Plugin_Continue;
    }

    // If client is attacking himself, then stop.
    if(attacker == client)
    {
        return Plugin_Continue;
    }

    // Get zombie flag for each client.
    bool clientzombie = InfectIsClientInfected(client);
    bool attackerzombie = InfectIsClientInfected(attacker);

    // If the flags are the same on both clients, then stop.
    if (clientzombie == attackerzombie)
    {
        // If friendly fire is blocked, then allow damage.
        bool damageblockff = GetConVarBool(g_hCvarsList.CVAR_DAMAGE_BLOCK_FF);
        if (!damageblockff)
        {
            return Plugin_Continue;
        }

        // Stop bullet from hurting client.
        return Plugin_Handled;
    }

    // Here we know that attacker and client are different teams.

    // Check if immunity module requires damage to be blocked.
    if (ImmunityOnClientTraceAttack(client, attacker, inflictor, damage, damagetype, ammotype, hitgroup))
    {
        // Block damage.
        return Plugin_Handled;
    }

    // If client is a human, then allow damage.
    if (InfectIsClientHuman(client))
    {
        // Allow damage.
        return Plugin_Continue;
    }

    // If damage hitgroups cvar is disabled, then allow damage.
    // TODO: There are two cvars: zr_hitgroups and zr_damage_hitgroups. Maybe
    //       just use zr_hitgroups?
    bool damagehitgroups = GetConVarBool(g_hCvarsList.CVAR_DAMAGE_HITGROUPS);
    if (!damagehitgroups)
    {
        // Allow damage.
        return Plugin_Continue;
    }

    // If damage is disabled for this hitgroup, then stop.
    int index = HitgroupToIndex(hitgroup);

    // If index can't be found, then allow damage.
    if (index == -1)
    {
        // Allow damage.
        return Plugin_Continue;
    }

    bool candamage = HitgroupsCanDamage(index);
    if (!candamage)
    {
        // Stop bullet from hurting client.
        return Plugin_Handled;
    }

    // Allow damage.
    return Plugin_Continue;
}

/**
 * Hook: OnTakeDamage
 * Called right before damage is done.
 *
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param damagetype    The type of damage inflicted.
 * @param ammotype      The ammo type of the attacker's weapon.
 * @return              Return Plugin_Handled to stop the damage to client.
 *                      Plugin_Continue to allow damage to client.
 */
public Action DamageOnTakeDamage(int client, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon,
        float damageForce[3], float damagePosition[3], int damagecustom)
{
    bool custom = view_as<bool>(damagecustom & ZR_KNOCKBACK_CUSTOM);
    if (custom)
        return Plugin_Continue;

    // Get classname of the inflictor.
    char classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));

    // Get the attacker weapon name.
    char weaponname[64];
    if(attacker && attacker >= 1 && attacker <= MAXPLAYERS && IsClientInGame(attacker))
        GetClientWeapon(attacker, weaponname, sizeof(weaponname));

    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1)
    {
        return Plugin_Continue;
    }

    // Forward this hook to another module an return (or not) what it wants.
    int action = NapalmOnTakeDamage(client, damagetype);

    // If the napalm module wants to return here, then return the int casted into the action type.
    if (action > -1)
    {
        return view_as<Action>(action);
    }

    // Client was shot or knifed.
    if (damagetype & DMG_BULLET || damagetype & DMG_NEVERGIB)
    {
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }

        // Get zombie flag for each client.
        bool clientzombie = InfectIsClientInfected(client);
        bool attackerzombie = InfectIsClientInfected(attacker);

        // If client and attacker are on the same team, then let CS:S handle the rest.
        if (clientzombie == attackerzombie)
        {
            return Plugin_Continue;
        }

        // We know that clientzombie is the opposite of attacker zombie.

        // If the client is a zombie, then allow damage.
        if (clientzombie)
        {
            return Plugin_Continue;
        }

        if (!clientzombie && attackerzombie && !(strcmp(weaponname, "weapon_bayonet", false) == 0 || strncmp(weaponname, "weapon_knife", 12) == 0))
        {
            damage = 1.0;
            return Plugin_Changed;
        }

        // Check if immunity module blocked or modified the damage.
        Action immunityAction = ImmunityOnClientDamage(client, attacker, damage);
        if (immunityAction != Plugin_Continue)
        {
            // Damage was blocked or modified.
            return immunityAction;
        }

        // Check if distance between clients is too high. (Anti lagcompensation / longknife)
        float maxDistance = GetConVarFloat(g_hCvarsList.CVAR_INFECT_MAX_DISTANCE);
        if (maxDistance != 0.0)
        {
            float clientPosition[3];
            float attackerPosition[3];

            GetClientAbsOrigin(client, clientPosition);
            GetClientAbsOrigin(attacker, attackerPosition);

            // ignore z axis
            clientPosition[2] = 0.0;
            attackerPosition[2] = 0.0;

            float distance = GetVectorDistance(clientPosition, attackerPosition);

            // Block infection if distance is higher than allowed.
            if (distance > maxDistance)
            {
                return Plugin_Handled;
            }
        }

        // Client is about to be infected, re-add HP so they aren't killed by
        // knife. But only do this when immunity mode is disabled.
        if (ClassGetImmunityMode(client) == Immunity_None)
        {
            int health = GetClientHealth(client);
            SetEntityHealth(client, health + RoundToNearest(damage));

            // Allow damage.
            return Plugin_Continue;
        }
    }
    // Client was damaged by explosion.
    else if (damagetype & DMG_BLAST)
    {
        // If blast damage is blocked, then stop.
        bool damageblockblast = GetConVarBool(g_hCvarsList.CVAR_DAMAGE_BLOCK_BLAST);
        if (!damageblockblast)
        {
            return Plugin_Continue;
        }

        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }

        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return Plugin_Continue;
        }

        // Stop damage.
        return Plugin_Handled;
    }
    // Client was damaged by fire.
    else if (damagetype & DMG_BURN)
    {
        // If its not an inferno, then allow damage.
        if (strncmp(classname, "inferno", 7) != 0)
        {
            return Plugin_Continue;
        }

        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }

        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return Plugin_Continue;
        }

        // Stop damage.
        return Plugin_Handled;
    }
    // Client was damaged by falling.
    else if (damagetype & DMG_FALL)
    {
        // If class has "nofalldamage" disabled, then allow damage.
        bool blockfalldamage = ClassGetNoFallDamage(client);
        if (!blockfalldamage)
        {
            return Plugin_Continue;
        }

        // Stop damage.
        return Plugin_Handled;
    }

    // Allow damage.
    return Plugin_Continue;
}

/**
 * Command callback (kill, spectate, jointeam, joinclass)
 * Block command if plugin thinks they are trying to commit suicide.
 *
 * @param client    The client index.
 * @param argc      The number of arguments in command string.
 */
public Action DamageSuicideIntercept(int client, const char[] command, int argc)
{
    // Get suicide interception settings.
    bool suicideAfterInfect = GetConVarBool(g_hCvarsList.CVAR_SUICIDE_AFTER_INFECT);
    bool suicideZombie = GetConVarBool(g_hCvarsList.CVAR_DAMAGE_SUICIDE_ZOMBIE);
    bool suicideZombieMother = GetConVarBool(g_hCvarsList.CVAR_DAMAGE_SUICIDE_MZOMBIE);
    bool suicideHuman = GetConVarBool(g_hCvarsList.CVAR_DAMAGE_SUICIDE_HUMAN);

    // Check various criterias that will _allow_ the command. If no criterias
    // match, block it.

    // Check general criterias.
    if ((suicideAfterInfect && !InfectHasZombieSpawned()) || // Check if it should block suicides before mother zombie.
        !ZRIsClientValid(client) ||                     // Validate client (to stop console).
        !IsPlayerAlive(client))                         // Check if dead.
    {
        // Allow command.
        return Plugin_Continue;
    }

    // Check zombie criterias.
    if (InfectIsClientInfected(client))
    {
        if (g_bDamageMotherZombie[client] && !suicideZombieMother ||    // Check if suicide is allowed for mother zombies.
            (!g_bDamageMotherZombie[client] && !suicideZombie))         // Check if suicide is allowed for regular zombies.
        {
            // Allow command.
            return Plugin_Continue;
        }
    }

    // Check human criterias.
    // Allow suicide if player is a human and humans can suicide.
    if (InfectIsClientHuman(client) && !suicideHuman)
    {
        // Allow command.
        return Plugin_Continue;
    }

    // Tell client their command has been intercepted, and log.
    TranslationPrintToChat(client, "Damage suicide intercept");
    LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "\"%L\" attempted suicide.", client);

    // Block command.
    return Plugin_Handled;
}

public void DamageOnTakeDamageAlivePost(int victim, int attacker, int inflictor, float damage, int damagetype, int weapon,
        const float damageForce[3], const float damagePosition[3], int damagecustom)
{
    // If attacker is invalid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return;
    }

    bool custom = view_as<bool>(damagecustom & ZR_KNOCKBACK_CUSTOM);

    char weaponname[64];
    if (inflictor == attacker)
    {
        weapon = WeaponsGetActiveWeaponIndex(attacker);
        if (weapon > 0)
            GetEdictClassname(weapon, weaponname, sizeof(weaponname));
    }
    else
        GetEdictClassname(inflictor, weaponname, sizeof(weaponname));

    ReplaceString(weaponname, sizeof(weaponname), "weapon_", "");
    ReplaceString(weaponname, sizeof(weaponname), "_projectile", "");

    int hitgroup = HITGROUP_GENERIC;
    if (!(damagetype & DMG_BLAST) && !custom)
        hitgroup = ToolsGetClientLastHitGroup(victim);

    KnockbackOnClientHurt(victim, attacker, inflictor, weaponname, hitgroup, damage, damagetype, weapon, damagecustom);

    // If attacker is a human and is getting damaged through DamageProxy, then stop.
    // We don't want to get infected if we're being knifed inside of a human item.
    if (custom && InfectIsClientHuman(victim))
    {
        return;
    }

    ClassAlphaUpdate(victim);
    InfectOnClientHurt(victim, attacker, weaponname);
    AccountOnClientHurt(victim, attacker, RoundToFloor(damage));
    SEffectsOnClientHurt(victim);
    NapalmOnClientHurt(victim, attacker, weaponname, RoundToFloor(damage));
    ZHPOnClientHurt(victim);
}
