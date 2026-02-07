/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          weapons.api.sp
 *  Type:          Core
 *  Description:   Weapon natives for the ZR API.
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
 * @section Weapon natives.
 */

#define KBQ_MAX_RETRIES 5

StringMap g_KBJobs;

enum struct KnockbackJob
{
    char weapon[WEAPONS_MAX_LENGTH];
    float value;
    int attempts;
    Handle hTimer;
}

public void WeaponsOnMapEnd()
{
    KB_QueueClearAll();
}

static void KBQ_SaveJob(const char[] key, const KnockbackJob job)
{
    if (g_KBJobs == null)
    {
        g_KBJobs = new StringMap();
    }
    g_KBJobs.SetArray(key, job, sizeof(job));
}

static bool KBQ_GetJob(const char[] key, KnockbackJob job)
{
    if (g_KBJobs == null)
    {
        return false;
    }
    return g_KBJobs.GetArray(key, job, sizeof(job));
}

static void KBQ_RemoveJob(const char[] key)
{
    if (g_KBJobs == null)
    {
        return;
    }
    g_KBJobs.Remove(key);
}

static void KBQ_CleanupJob(KnockbackJob job, Handle currentTimer = null, bool fromCallback = false)
{
    if (job.hTimer != null)
    {
        if (fromCallback && currentTimer != null && job.hTimer == currentTimer)
        {
            // Timer just fired; SourceMod will auto-close it after callback.
            job.hTimer = null;
        }
        else
        {
            // Cancel a pending timer safely.
            KillTimer(job.hTimer);
            job.hTimer = null;
        }
    }
}

static Handle KBQ_CreateRetryTimer(const KnockbackJob job)
{
    // Backoff: 0.5s, 1s, 2s, 4s, 8s
    float delay = 0.5 * Pow(2.0, float(job.attempts));
    DataPack pack = new DataPack();
    pack.WriteString(job.weapon);
    return CreateTimer(delay, KBQ_TimerRetrySetKB, pack, TIMER_FLAG_NO_MAPCHANGE);
}

static void KBQ_EnqueueOrUpdate(const char[] weapon, float value)
{
    KnockbackJob job;
    if (KBQ_GetJob(weapon, job))
    {
        // Deduplicate by weapon; keep the latest value
        job.value = value;
        if (job.hTimer == null)
        {
            job.hTimer = KBQ_CreateRetryTimer(job);
        }
        KBQ_SaveJob(weapon, job);
        return;
    }

    strcopy(job.weapon, sizeof(job.weapon), weapon);
    job.value = value;
    job.attempts = 0;
    job.hTimer = KBQ_CreateRetryTimer(job);
    KBQ_SaveJob(weapon, job);
}

// Attempts to set knockback immediately; returns true on success, false if backend isn't ready
static bool InternalTrySetWeaponKB(const char[] weapon, float value)
{
    // Backend guard: avoid touching weapon tables if not initialized yet
    if (arrayWeapons == null)
    {
        return false;
    }

    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }

    ArrayList arrayWeapon = arrayWeapons.Get(index);
    if (arrayWeapon == null)
    {
        return false;
    }

    arrayWeapon.Set(view_as<int>(WEAPONS_DATA_KNOCKBACK), value);
    return true;
}

public Action KBQ_TimerRetrySetKB(Handle timer, any data)
{
    DataPack pack = view_as<DataPack>(data);
    pack.Reset();
    char weapon[WEAPONS_MAX_LENGTH];
    pack.ReadString(weapon, sizeof(weapon));
    delete pack;

    KnockbackJob job;
    if (!KBQ_GetJob(weapon, job))
    {
        return Plugin_Stop;
    }

    if (InternalTrySetWeaponKB(job.weapon, job.value))
    {
        KBQ_CleanupJob(job, timer, true);
        KBQ_RemoveJob(job.weapon);
        return Plugin_Stop;
    }

    job.attempts++;
    if (job.attempts > KBQ_MAX_RETRIES)
    {
        LogError("[ZR-KB] Failed to apply knockback for '%s' after %d attempts", job.weapon, KBQ_MAX_RETRIES);
        KBQ_CleanupJob(job, timer, true);
        KBQ_RemoveJob(job.weapon);
        return Plugin_Stop;
    }

    job.hTimer = KBQ_CreateRetryTimer(job);
    KBQ_SaveJob(job.weapon, job);
    return Plugin_Stop;
}

public void KB_QueueClearAll()
{
    if (g_KBJobs == null)
    {
        return;
    }

    StringMapSnapshot snap = g_KBJobs.Snapshot();
    int count = snap.Length;
    char key[WEAPONS_MAX_LENGTH];
    for (int i = 0; i < count; i++)
    {
        snap.GetKey(i, key, sizeof(key));
        KnockbackJob job;
        if (g_KBJobs.GetArray(key, job, sizeof(job)))
        {
            KBQ_CleanupJob(job);
        }
    }
    delete snap;
    delete g_KBJobs;
    g_KBJobs = new StringMap();
}

/**
 * Gets a weapon's entity name.
 */
void APIWeaponsInit()
{
    // Weapons module natives/forwards (weapons.zr.inc)

    // Natives
    CreateNative("ZR_GetWeaponEntity", Native_GetWeaponEntity);
    CreateNative("ZR_GetWeaponType", Native_GetWeaponType);
    CreateNative("ZR_GetWeaponSlot", Native_GetWeaponSlot);
    CreateNative("ZR_GetWeaponRestrictDefault", Native_GetWeaponRestrictDefault);
    CreateNative("ZR_GetWeaponToggleable", Native_GetWeaponToggleable);
    CreateNative("ZR_GetWeaponAmmoType", Native_GetWeaponAmmoType);
    CreateNative("ZR_GetWeaponAmmoPrice", Native_GetWeaponAmmoPrice);
    CreateNative("ZR_GetWeaponKnockback", Native_GetWeaponKnockback);
    CreateNative("ZR_GetWeaponZMarketName", Native_GetWeaponZMarketName);
    CreateNative("ZR_GetWeaponZMarketPrice", Native_GetWeaponZMarketPrice);
    CreateNative("ZR_GetWeaponZMarketPurchaseMax", Native_GetWeaponZMarketPurchaseMax);
    CreateNative("ZR_GetWeaponZMarketCommand", Native_GetWeaponZMarketCommand);
    CreateNative("ZR_SetWeaponKnockback", Native_SetWeaponKnockback);
	
    // Restrict Natives
    CreateNative("ZR_SetClientWeaponRestrictAll", Native_SetClientWeaponRestrictAll);
    CreateNative("ZR_SetAllRestrictAll", Native_SetAllRestrictAll);
    CreateNative("ZR_SetClientWeaponRestrict", Native_SetClientWeaponRestrict);
    CreateNative("ZR_SetAllWeaponRestrict", Native_SetAllWeaponRestrict);
	
    // Forwards
}

public int Native_GetWeaponEntity(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    char entity[WEAPONS_MAX_LENGTH];
    WeaponsGetEntity(index, entity, sizeof(entity));
    
    SetNativeString(2, entity, GetNativeCell(3));
    return true;
}

public int Native_GetWeaponType(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    char type[WEAPONS_MAX_LENGTH];
    WeaponsGetType(index, type, sizeof(type));
    
    SetNativeString(2, type, GetNativeCell(3));
    return true;
}

public int Native_GetWeaponSlot(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return -1;
    }
    
    return view_as<int>(WeaponsGetSlot(index));
}

public int Native_GetWeaponRestrictDefault(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    return WeaponsGetRestrictDefault(index);
}

public int Native_GetWeaponToggleable(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    return WeaponsGetToggleable(index);
}

public int Native_GetWeaponAmmoType(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    char ammotype[WEAPONS_MAX_LENGTH];
    WeaponsGetAmmoType(index, ammotype, sizeof(ammotype));
    
    SetNativeString(2, ammotype, GetNativeCell(3));
    return true;
}

public int Native_GetWeaponAmmoPrice(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return -1;
    }
    
    return WeaponsGetAmmoPrice(index);
}

public int Native_GetWeaponKnockback(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return view_as<int>(0.0);
    }
    
    return view_as<int>(WeaponsGetKnockback(index));
}

public int Native_GetWeaponZMarketName(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    char name[WEAPONS_MAX_LENGTH];
    WeaponsGetZMarketName(index, name, sizeof(name));
    
    SetNativeString(2, name, GetNativeCell(3));
    return true;
}

public int Native_GetWeaponZMarketPrice(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return -1;
    }
    
    return WeaponsGetZMarketPrice(index);
}

public int Native_GetWeaponZMarketPurchaseMax(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return -1;
    }
    
    return WeaponsGetZMarketPurchaseMax(index);
}

public int Native_GetWeaponZMarketCommand(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    char command[WEAPONS_MAX_LENGTH];
    WeaponsGetZMarketCommand(index, command, sizeof(command));
    
    SetNativeString(2, command, GetNativeCell(3));
    return true;
}

public int Native_SetWeaponKnockback(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));

    // If backend not ready, enqueue without resolving index to avoid invalid handle errors
    if (arrayWeapons == null)
    {
        float pendingValue = GetNativeCell(2);
        if (pendingValue <= 0.0)
        {
            return false;
        }
        KBQ_EnqueueOrUpdate(weapon, pendingValue);
        return true;
    }

    int index = WeaponsNameToIndex(weapon);
    if (index == -1)
    {
        return false;
    }
    
    float value = GetNativeCell(2);
    if (value <= 0.0)
    {
        return false;
    }

    // Immediate attempt; if backend not ready (e.g., invalid arrays), enqueue bounded retries
    if (InternalTrySetWeaponKB(weapon, value))
    {
        return true;
    }

    KBQ_EnqueueOrUpdate(weapon, value);
    return true; // request accepted; application will occur asynchronously
}

public int Native_SetClientWeaponRestrictAll(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client);
    
    bool restrictvalue = view_as<bool>(GetNativeCell(2));
    WeaponsSetClientWeaponRestrictAll(client, restrictvalue);
    return 1;
}

public int Native_SetAllRestrictAll(Handle plugin, int numParams)
{
    bool restrictvalue = view_as<bool>(GetNativeCell(1));
    WeaponsSetAllRestrictAll(restrictvalue);
    return 1;
}

public int Native_SetClientWeaponRestrict(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    
    // Validate the client index.
    APIValidateClientIndex(client);
    
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(2, weapon, sizeof(weapon));
    
    bool restrictvalue = view_as<bool>(GetNativeCell(3));
    
    return WeaponsSetClientWeaponRestrict(client, weapon, restrictvalue);
}

public int Native_SetAllWeaponRestrict(Handle plugin, int numParams)
{
    char weapon[WEAPONS_MAX_LENGTH];
    GetNativeString(1, weapon, sizeof(weapon));
    
    bool restrictvalue = view_as<bool>(GetNativeCell(2));
    
    return WeaponsSetAllWeaponRestrict(weapon, restrictvalue);
}

/**
 * @endsection
 */ 
