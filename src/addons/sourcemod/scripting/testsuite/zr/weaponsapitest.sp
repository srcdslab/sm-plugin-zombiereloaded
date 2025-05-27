/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          weaponinfo.sp
 *  Type:          Test plugin
 *  Description:   Dumps weapon information.
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

#include <sourcemod>
#include <zombiereloaded>

#pragma semicolon 1
#pragma newdecls required

public Plugin myinfo = 
{
    name = "Zombie:Reloaded - Weapons API Test",
    author = ".Rushaway",
    description = "Tests the weapons API for ZR",
    version = "1.0",
    url = "https://github.com/srcdslab/sm-plugin-zombiereloaded"
};

public void OnPluginStart()
{
    RegConsoleCmd("zr_testweapons", Command_TestWeapons, "Test ZR weapons API natives. Usage: zrtest_weaponslots [target]");
}

public Action Command_TestWeapons(int client, int argc)
{
    int target = -1;
    char valueString[64];

    if (argc >= 1)
    {
        GetCmdArg(1, valueString, sizeof(valueString));
        target = FindTarget(client, valueString);

        if (target == -1)
        {
            ReplyToCommand(client, "[ZR Weapons Test] Invalid target: %s", valueString);
            return Plugin_Handled;
        }
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Usage: zrtest_weaponslots [target]");
        return Plugin_Handled;
    }

    // Get active weapon
    int activeWeapon = GetEntPropEnt(target, Prop_Send, "m_hActiveWeapon");
    if (activeWeapon == -1)
    {
        ReplyToCommand(client, "[ZR Weapons Test] No active weapon found.");
        return Plugin_Handled;
    }

    char weaponName[64];
    GetEntityClassname(activeWeapon, weaponName, sizeof(weaponName));

    // Remove weapon_ prefix
    if (StrContains(weaponName, "weapon_") == 0)
        strcopy(weaponName, sizeof(weaponName), weaponName[7]);

    // Test weapon entity name
    char entityName[64];
    if (ZR_GetWeaponEntity(weaponName, entityName, sizeof(entityName)))
    {
        ReplyToCommand(client, "[ZR Weapons Test] Weapon Entity: %s", entityName);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get weapon entity for: %s", weaponName);
    }

    // Test weapon type
    char weaponType[64];
    if (ZR_GetWeaponType(weaponName, weaponType, sizeof(weaponType)))
    {
        ReplyToCommand(client, "[ZR Weapons Test] Weapon Type: %s", weaponType);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get weapon type for: %s", weaponName);
    }

    // Test weapon slot
    int slot = ZR_GetWeaponSlot(weaponName);
    if (slot != -1)
    {
        ReplyToCommand(client, "[ZR Weapons Test] Weapon Slot: %d", slot);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get weapon slot for: %s", weaponName);
    }

    // Test weapon restriction
    bool isRestricted = ZR_GetWeaponRestrictDefault(weaponName);
    ReplyToCommand(client, "[ZR Weapons Test] Weapon Restricted: %s", isRestricted ? "Yes" : "No");

    // Test weapon toggleable
    bool isToggleable = ZR_GetWeaponToggleable(weaponName);
    ReplyToCommand(client, "[ZR Weapons Test] Weapon Toggleable: %s", isToggleable ? "Yes" : "No");

    // Test weapon ammo type
    char ammoType[64];
    if (ZR_GetWeaponAmmoType(weaponName, ammoType, sizeof(ammoType)))
    {
        ReplyToCommand(client, "[ZR Weapons Test] Ammo Type: %s", ammoType);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get ammo type for: %s", weaponName);
    }

    // Test weapon ammo price
    int ammoPrice = ZR_GetWeaponAmmoPrice(weaponName);
    if (ammoPrice != -1)
    {
        ReplyToCommand(client, "[ZR Weapons Test] Ammo Price: %d", ammoPrice);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get ammo price for: %s", weaponName);
    }

    // Test weapon knockback
    float knockback = ZR_GetWeaponKnockback(weaponName);
    ReplyToCommand(client, "[ZR Weapons Test] Knockback: %.2f", knockback);

    // Test ZMarket name
    char zmarketName[64];
    if (ZR_GetWeaponZMarketName(weaponName, zmarketName, sizeof(zmarketName)))
    {
        ReplyToCommand(client, "[ZR Weapons Test] ZMarket Name: %s", zmarketName);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get ZMarket name for: %s", weaponName);
    }

    // Test ZMarket price
    int zmarketPrice = ZR_GetWeaponZMarketPrice(weaponName);
    if (zmarketPrice != -1)
    {
        ReplyToCommand(client, "[ZR Weapons Test] ZMarket Price: %d", zmarketPrice);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get ZMarket price for: %s", weaponName);
    }

    // Test ZMarket purchase max
    int purchaseMax = ZR_GetWeaponZMarketPurchaseMax(weaponName);
    if (purchaseMax != -1)
    {
        ReplyToCommand(client, "[ZR Weapons Test] ZMarket Purchase Max: %d", purchaseMax);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get ZMarket purchase max for: %s", weaponName);
    }

    // Test ZMarket command
    char zmarketCommand[64];
    if (ZR_GetWeaponZMarketCommand(weaponName, zmarketCommand, sizeof(zmarketCommand)))
    {
        ReplyToCommand(client, "[ZR Weapons Test] ZMarket Command: %s", zmarketCommand);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to get ZMarket command for: %s", weaponName);
    }

    // Test setting knockback
    float newKnockback = knockback + 5.0;
    if (ZR_SetWeaponKnockback(weaponName, newKnockback))
    {
        ReplyToCommand(client, "[ZR Weapons Test] Successfully set new knockback to: %.2f", newKnockback);
        
        // Verify the change
        float updatedKnockback = ZR_GetWeaponKnockback(weaponName);
        ReplyToCommand(client, "[ZR Weapons Test] Verified new knockback value: %.2f", updatedKnockback);
    }
    else
    {
        ReplyToCommand(client, "[ZR Weapons Test] Failed to set knockback for: %s", weaponName);
    }

    return Plugin_Handled;
} 