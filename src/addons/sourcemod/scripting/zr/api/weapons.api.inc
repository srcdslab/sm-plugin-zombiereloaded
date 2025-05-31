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
    
    Handle arrayWeapon = GetArrayCell(arrayWeapons, index);
    SetArrayCell(arrayWeapon, view_as<int>(WEAPONS_DATA_KNOCKBACK), value);
    
    return true;
}

/**
 * @endsection
 */ 