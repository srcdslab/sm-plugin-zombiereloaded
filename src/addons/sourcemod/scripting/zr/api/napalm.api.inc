/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          napalm.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Napalm module)
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
Handle g_hAPIFwdOnClientIgnite = INVALID_HANDLE;
Handle g_hAPIFwdOnClientIgnited = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to infection.
 */
APINapalmInit()
{
    // Infect module natives/forwards (napalm.zr.inc)

    // Natives
    CreateNative("ZR_ClassHasNapalm", APIClassHasNapalm);
    CreateNative("ZR_ClassGetNapalmTime", APIClassGetNapalmTime);

    // Forwards
    g_hAPIFwdOnClientIgnite = CreateGlobalForward("ZR_OnClientIgnite", ET_Hook, Param_CellByRef, Param_FloatByRef);
    g_hAPIFwdOnClientIgnited = CreateGlobalForward("ZR_OnClientIgnited", ET_Hook, Param_CellByRef, Param_FloatByRef);
}

/**
 * Native call function (ZR_ClassHasNapalm)
 * Get if the class has napalm grenades.
 * 
 * bool ClassGetHasNapalm(int index, int cachetype = ZR_CLASS_CACHE_PLAYER)
 */
public int APIClassHasNapalm(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    int cacheType = GetNativeCell(2);

    // Validate the client index.
    APIValidateClientIndex(client, Condition_Either);

    // Return the value.
    return view_as<int>(ClassGetHasNapalm(client, cacheType));
}

/**
 * Native call function (ZR_ClassGetNapalmTime)
 * Get the napalm grenades time from the specified class.
 * 
 * float ClassGetNapalmTime(int index, int cachetype = ZR_CLASS_CACHE_PLAYER)
 */
public any APIClassGetNapalmTime(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    int cacheType = GetNativeCell(2);

    // Validate the client index.
    APIValidateClientIndex(client, Condition_Either);

    // Return the value.
    return ClassGetNapalmTime(client, cacheType);
}

/**
 * Called when a zombie is about to be set on fire.
 *
 * @param client            The client to ignite.
 * @param duration          The burn duration.
 *
 */
Action APIOnClientIgnite(int client, float duration)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientIgnite);

    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushFloatRef(duration);

    // Get what they returned.
    Action result;
    Call_Finish(result);
    return result;
}

/**
 * Called after a client has been set on fire.
 *
 * @param client            The client to ignite.
 * @param duration          The burn duration.
 *
 */
void APIOnClientIgnited(int client, float duration)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClientIgnited);

    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushFloatRef(duration);

    // Finish the call.
    Call_Finish();
}
