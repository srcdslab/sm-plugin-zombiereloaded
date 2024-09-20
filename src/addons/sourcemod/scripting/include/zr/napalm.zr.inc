/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          napalm.zr.inc
 *  Type:          Include
 *  Description:   Napalm-related natives/forwards.
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
 * Get if the class has napalm grenades.
 *
 * @param client            The client index.
 * @param cachetype         Optional. See class.zr.inc for details.
 * @error                   Invalid client index.
 * 
 * @return                  True if the class has napalm grenades.
 */
native bool ZR_ClassHasNapalm(int client, int cachetype = ZR_CLASS_CACHE_PLAYER);

/**
 * Get the napalm grenades time from the specified class.
 *
 * @param client            The client index.
 * @param cachetype         Optional. See class.zr.inc for details.
 * @error                   Invalid client index.
 * 
 * @return                  The napalm time. (float)
 */
native float ZR_ClassGetNapalmTime(int client, int cachetype = ZR_CLASS_CACHE_PLAYER);

/**
 * Called when a zombie is about to be set on fire.
 *
 * @param client            The client to ignite.
 * @param duration          The burn duration.
 *
 * @return                  Plugin_Handled to block the zombie being set on fire. Anything else
 *                          (like Plugin_Continue) to allow it.
 */
forward Action ZR_OnClientIgnite(int &client, float &duration);

/**
 * Called after a zombie has been set on fire.
 *
 * @param client            The client to ignite.
 * @param duration          The burn duration.
 */
forward void ZR_OnClientIgnited(int client, float duration);
