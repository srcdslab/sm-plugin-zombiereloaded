/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          ztele.zr.inc
 *  Type:          Include
 *  Description:   Ztele-related natives/forwards.
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
 * Spawns a player into the round.
 *
 * @param client            The client index.
 * @param force             Optional. True to teleport instantly, false to follow rules
 * 
 * @error                   Invalid client index, not connected or already alive.
 */
native void ZR_ZteleClient(int client, bool force = false);

/**
 * Called right before ZR is about to ztele a player.
 * Here you can modify any variable or stop the action entirely.
 *
 * @param client            The client index.
 *
 * @return      Plugin_Handled to block ztele.
 */
forward Action ZR_OnClientZtele(int client, bool force);

/**
 * Called after ZR ztele a player.
 *
 * @param client            The client index.
 */
forward void ZR_OnClientZteled(int client);