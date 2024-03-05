/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zombiereloaded.inc
 *  Type:          Include
 *  Description:   Main API include file.
 *  Notes:         Include this file to include the whole ZR API.
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

#if defined _zr_included
 #endinput
#endif
#define _zr_included

#include <zr/infect.zr>
#include <zr/respawn.zr>
#include <zr/class.zr>
#include <zr/napalm.zr>
#include <zr/knockback.zr>

public SharedPlugin __pl_zombiereloaded =
{
    name = "zombiereloaded",
    file = "zombiereloaded.smx",
#if defined REQUIRE_PLUGIN
    required = 1
#else
    required = 0
#endif
};

#if !defined REQUIRE_PLUGIN
public void __pl_zombiereloaded_SetNTVOptional()
{
    MarkNativeAsOptional("ZR_IsValidClassIndex");
    MarkNativeAsOptional("ZR_GetActiveClass");
    MarkNativeAsOptional("ZR_SelectClientClass");
    MarkNativeAsOptional("ZR_GetClassByName");
    MarkNativeAsOptional("ZR_GetClassByIdentifier");
    MarkNativeAsOptional("ZR_GetClassDisplayName");
    MarkNativeAsOptional("ZR_GetClassIdentifier");
    MarkNativeAsOptional("ZR_SendMenuClass");

    MarkNativeAsOptional("ZR_IsClientMotherZombie");
    MarkNativeAsOptional("ZR_IsClientZombie");
    MarkNativeAsOptional("ZR_IsClientHuman");
    MarkNativeAsOptional("ZR_InfectClient");
    MarkNativeAsOptional("ZR_HumanClient");

    MarkNativeAsOptional("ZR_RespawnClient");
    MarkNativeAsOptional("ZR_SetKilledByWorld");
    MarkNativeAsOptional("ZR_GetKilledByWorld");

    MarkNativeAsOptional("ZR_SetClientKnockbackScale");
    MarkNativeAsOptional("ZR_SetClientKnockbackMaxForce");
    MarkNativeAsOptional("ZR_SetClientKnockbackMaxVelocity");
}
#endif
