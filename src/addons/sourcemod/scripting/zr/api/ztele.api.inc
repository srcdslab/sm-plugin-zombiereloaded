/*
* ============================================================================
*
*  Zombie:Reloaded
*
*  File:          ztele.api.inc
*  Type:          Core
*  Description:   Native handlers for the ZR API. (Ztele module)
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
Handle g_hAPIFwdOnClientZtele = INVALID_HANDLE;
Handle g_hAPIFwdOnClientZteled = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to ztele.
 */
void APIZteleInit()
{
    // Ztele module natives/forwards (ztele.zr.inc)

    // Natives
    CreateNative("ZR_ZteleClient", APIOnClientZtele);

    // Forwards
    g_hAPIFwdOnClientZtele = CreateGlobalForward("ZR_OnClientZtele", ET_Hook, Param_Cell);
    g_hAPIFwdOnClientZteled = CreateGlobalForward("ZR_OnClientZteled", ET_Ignore, Param_Cell);
}

/**
 * Native call function (ZR_ZteleClient)
 * Ztele a player following rules set by cvars.
 *
 * bool ZTeleClient(int client, bool force = false)
 */
public int APIOnClientZtele(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    bool force = GetNativeCell(2);
    
    // ZTele the client.
    ZTeleClient(client, force);
    return -1;
}

/**
 * Called before a player was ztele.
 *
 * @param client            The client index.
 * @return                  Plugin_Continue to continue the call, Plugin_Handled to stop it.
 */
Action APIOnClientZtelePre(int client)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientZtele);

    // Push the parameters.
    Call_PushCell(client);

    // Finish the call.
    Action result;
    Call_Finish(result);
    return Plugin_Continue;
}

/**
 * Called after a player was ztele.
 *
 * @param client            The client index.
 */
Action APIOnClientZtelePost(int client)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientZteled);

    // Push the parameters.
    Call_PushCell(client);

    // Finish the call.
    Call_Finish();
    return Plugin_Continue;
}
