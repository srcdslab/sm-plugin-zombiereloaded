/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          knockback.zr.inc
 *  Type:          Include
 *  Description:   Knockback-related natives/forwards.
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

#define ZR_KNOCKBACK_CUSTOM (1<<31)
#define ZR_KNOCKBACK_SCALE (1<<1)
#define ZR_KNOCKBACK_LIMITFORCE (1<<2)
#define ZR_KNOCKBACK_LIMITVEL (1<<3)

/**
 * Set a custom knockback scale.
 *
 * @param client            The client.
 * @param fScale            Custom knockback scale.
 */
native void ZR_SetClientKnockbackScale(int client, float fScale);

/**
 * Set a maximum knockback force per tick.
 *
 * @param client            The client.
 * @param fForce         	Maximum knockback force per tick.
 */
native void ZR_SetClientKnockbackMaxForce(int client, float fForce);

/**
 * Set a maximum knockback velocity.
 *
 * @param client            The client.
 * @param fVelocity         Maximum knockback velocity.
 */
native void ZR_SetClientKnockbackMaxVelocity(int client, float fVelocity);

/**
 * Set a percentage value for knockback per weapon (required for KnockbackRestrict plugin)
 *
 * @param client            The client.
 * @param weaponname        Weapon Name (not the class name (without weapon_)).
 * @param fPercentage       Percentage Value
 */
native void ZR_SetClientKnockbackPercentagePerWeapon(int client, const char[] weaponname, float fPercentage);

/**
 * Reset Percentage knockback for all weapons (required for KnockbackRestrict plugin)
 *
 * @param client            The client.
 */
native void ZR_ResetClientKnockbackPercenagePerWeapon(int client);
