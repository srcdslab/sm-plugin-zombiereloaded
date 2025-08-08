/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          knockback.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Knockback module)
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
 * Initializes all natives and forwards related to infection.
 */
APIKnockbackInit()
{
    // Class module natives/forwards (knockback.zr.inc)

    // Natives
    CreateNative("ZR_SetClientKnockbackScale", APISetClientKnockbackScale);
    CreateNative("ZR_SetClientKnockbackMaxForce", APISetClientKnockbackMaxForce);
    CreateNative("ZR_SetClientKnockbackMaxVelocity", APISetClientKnockbackMaxVelocity);
    CreateNative("ZR_SetClientKnockbackPercentagePerWeapon", APISetClientKnockbackPercentagePerWeapon);
    CreateNative("ZR_ResetClientKnockbackPercenagePerWeapon", APIResetClientKnockbackPercenagePerWeapon);
}

/**
 * Native call function (ZR_SetClientKnockbackScale)
 *
 * int ZR_SetClientKnockbackScale(int client, float fScale);
 */
public int APISetClientKnockbackScale(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    float fScale = view_as<float>(GetNativeCell(2));

    APIValidateClientIndex(client);

    KnockbackSetClientScale(client, fScale);
    return -1;
}

/**
 * Native call function (ZR_SetClientKnockbackMaxForce)
 *
 * int ZR_SetClientKnockbackMaxForce(int client, float fForce);
 */
public int APISetClientKnockbackMaxForce(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    float fForce = view_as<float>(GetNativeCell(2));

    APIValidateClientIndex(client);

    KnockbackSetClientMaxForce(client, fForce);
    return -1;
}

/**
 * Native call function (ZR_SetClientKnockbackMaxVelocity)
 *
 * int ZR_SetClientKnockbackMaxVelocity(int client, float fVelocity);
 */
public int APISetClientKnockbackMaxVelocity(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    float fVelocity = view_as<float>(GetNativeCell(2));

    APIValidateClientIndex(client);

    KnockbackSetClientMaxVelocity(client, fVelocity);
    return -1;
}

/**
 * Native call function (ZR_SetClientKnockbackPercentagePerWeapon)
 *
 * int ZR_SetClientKnockbackPercentagePerWeapon(int client, const char[] weaponname, float fPercentage);
 */
public int APISetClientKnockbackPercentagePerWeapon(Handle plugin, int numParams)
{
	int client = GetNativeCell(1);

	char weapon[64];
	GetNativeString(2, weapon, sizeof(weapon));

	float fPercentage = view_as<float>(GetNativeCell(3));

	APIValidateClientIndex(client);

	KnockbackSetClientPercentagePerWeapon(client, weapon, fPercentage);
	return -1;
}

/**
 * Native call function (ZR_ResetClientKnockbackPercenagePerWeapon)
 *
 * int ZR_ResetClientKnockbackPercenagePerWeapon(int client, const char[] weaponname, float fPercentage);
 */
public int APIResetClientKnockbackPercenagePerWeapon(Handle plugin, int numParams)
{
	int client = GetNativeCell(1);

	APIValidateClientIndex(client);

	KnockbackResetClientPercenagePerWeapon(client);
	return -1;
}
