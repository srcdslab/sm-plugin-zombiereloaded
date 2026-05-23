/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          roundend.zr.inc
 *  Type:          Include
 *  Description:   Round end-related forwards.
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
 * Called once at round end before player rows are sent.
 *
 * @param playerCount   Number of player rows that will be sent in ZR_OnRoundEndPlayer.
 */
forward void ZR_OnRoundEndStats(int playerCount);

/**
 * Called once per player at round end.
 *
 * @param userid        Userid of the player.
 * @param name          Player name.
 * @param auth          Player auth id according to zr_roundend_authid_type.
 * @param alive         True if the player is alive.
 * @param zombie        True if the player is a zombie, false if human.
 */
forward void ZR_OnRoundEndPlayer(int userid, const char[] name, const char[] auth, bool alive, bool zombie);
