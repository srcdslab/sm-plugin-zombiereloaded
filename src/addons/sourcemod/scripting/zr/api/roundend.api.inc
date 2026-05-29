/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          roundend.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Round end module)
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
 * @section Global forward handles.
 */
GlobalForward g_hAPIFwdOnRoundEndStats = null;
GlobalForward g_hAPIFwdOnRoundEndPlayer = null;
/**
 * @endsection
 */

/**
 * Initializes all forwards related to round end.
 */
void APIRoundEndInit()
{
    g_hAPIFwdOnRoundEndStats = new GlobalForward("ZR_OnRoundEndStats", ET_Ignore, Param_Cell);
    g_hAPIFwdOnRoundEndPlayer = new GlobalForward("ZR_OnRoundEndPlayer", ET_Ignore, Param_Cell, Param_String, Param_String, Param_Cell, Param_Cell);
}

/**
 * Called once at round end with the number of player rows.
 */
void APIOnRoundEndStats(int playerCount)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnRoundEndStats);

    // Push the parameters.
    Call_PushCell(playerCount);

    // Finish the call.
    Call_Finish();
}

/**
 * Called once per player at round end.
 */
void APIOnRoundEndPlayer(int userid, const char[] name, const char[] auth, bool alive, bool zombie)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnRoundEndPlayer);

    // Push the parameters.
    Call_PushCell(userid);
    Call_PushString(name);
    Call_PushString(auth);
    Call_PushCell(alive);
    Call_PushCell(zombie);

    // Finish the call.
    Call_Finish();
}
