/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          class.api.inc
 *  Type:          Core
 *  Description:   Native handlers for the ZR API. (Class module)
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
Handle g_hAPIFwdOnClassAttributesApplied = INVALID_HANDLE;
Handle g_hAPIFwdOnClassApplyModel = INVALID_HANDLE;
Handle g_hAPIFwdOnClassModelApplied = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Initializes all natives and forwards related to infection.
 */
void APIClassInit()
{
    // Class module natives/forwards (class.zr.inc)

    // Natives
    CreateNative("ZR_IsValidClassIndex", APIIsValidClassIndex);
    CreateNative("ZR_GetActiveClass", APIGetActiveClass);
    CreateNative("ZR_GetHumanClass", APIGetHumanClass);
    CreateNative("ZR_GetZombieClass", APIGetZombieClass);
    CreateNative("ZR_SelectClientClass", APISelectClientClass);
    CreateNative("ZR_GetClassByName", APIGetClassByName);
    CreateNative("ZR_GetClassByIdentifier", APIGetClassByIdentifier);
    CreateNative("ZR_GetClassDisplayName", APIGetClassDisplayName);
    CreateNative("ZR_GetClassIdentifier", APIGetClassIdentifier);
    CreateNative("ZR_SendMenuClass", APISendMenuClass);

    // Forwards
    g_hAPIFwdOnClassAttributesApplied = CreateGlobalForward("ZR_OnClassAttributesApplied", ET_Ignore, Param_CellByRef, Param_CellByRef);
    g_hAPIFwdOnClassApplyModel = CreateGlobalForward("ZR_OnClassApplyModel", ET_Hook, Param_CellByRef, Param_String, Param_CellByRef);
    g_hAPIFwdOnClassModelApplied = CreateGlobalForward("ZR_OnClassModelApplied", ET_Ignore, Param_CellByRef, Param_String, Param_CellByRef);
}

/**
 * Native call function (ZR_IsValidClassIndex)
 *
 * bool ZR_IsValidClassIndex(int classIndex);
 */
public int APIIsValidClassIndex(Handle plugin, int numParams)
{
	int classIndex = GetNativeCell(1);
	
	return ClassValidateIndex(classIndex);
}

/**
 * Native call function (ZR_GetActiveClass)
 *
 * native bool ZR_GetActiveClass(int client);
 */
public int APIGetActiveClass(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);

    // Validate the client index. Player must be alive.
    APIValidateClientIndex(client, Condition_True);

    return ClassGetActiveIndex(client);
}

/**
 * Native call function (ZR_GetHumanClass)
 *
 * native bool ZR_GetHumanClass(int client);
 */
public int APIGetHumanClass(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);

    // Validate the client index. Player must be alive.
    APIValidateClientIndex(client, Condition_True);

    return ClassSelected[client][ZR_CLASS_TEAM_HUMANS];
}

/**
 * Native call function (ZR_GetZombieClass)
 *
 * native bool ZR_GetZombieClass(int client);
 */
public int APIGetZombieClass(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);

    // Validate the client index. Player must be alive.
    APIValidateClientIndex(client, Condition_True);

    return ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES];
}

/**
 * Native call function (ZR_SelectClientClass)
 *
 * native ClassSelectResult ZR_SelectClientClass(int client, int classIndex, bool applyIfPossible = true, bool saveIfEnabled = true)
 */
public int APISelectClientClass(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    int classIndex = GetNativeCell(2);
    bool applyIfPossible = view_as<bool>(GetNativeCell(3));
    bool saveIfEnabled = view_as<bool>(GetNativeCell(4));

    // Validate the client index.
    APIValidateClientIndex(client, Condition_Either);

    // Validate class index.
    if (!ClassValidateIndex(classIndex))
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Invalid class index. (%d)", classIndex);
        return -1;
    }

    return view_as<int>(ClassSelectClientClass(client, classIndex, applyIfPossible, saveIfEnabled));
}

/**
 * Native call function (ZR_GetClassByName)
 *
 * native int ZR_GetClassByName(const char[] className, int cacheType = ZR_CLASS_CACHE_MODIFIED);
 */
public int APIGetClassByName(Handle plugin, int numParams)
{
    char className[64];
    className[0] = 0;

    // Get class name.
    if (GetNativeString(1, className, sizeof(className)) != SP_ERROR_NONE)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Unexpected error when reading className parameter. Possibly corrupt or missing data.");
        return -1;
    }

    int cacheType = GetNativeCell(2);
    if (cacheType == ZR_CLASS_CACHE_PLAYER)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Invalid cache type. Player cache is not allowed in this function.");
        return -1;
    }

    return ClassGetIndexByName(className, cacheType);
}

/**
 * Native call function (ZR_GetClassByIdentifier)
 *
 * native int ZR_GetClassByIdentifier(const char[] classIdent, int cacheType = ZR_CLASS_CACHE_MODIFIED);
 */
public int APIGetClassByIdentifier(Handle plugin, int numParams)
{
    char classIdent[64];
    classIdent[0] = 0;

    // Get class name.
    if (GetNativeString(1, classIdent, sizeof(classIdent)) != SP_ERROR_NONE)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Unexpected error when reading classIdent parameter. Possibly corrupt or missing data.");
        return -1;
    }

    int cacheType = GetNativeCell(2);
    if (cacheType == ZR_CLASS_CACHE_PLAYER)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "Invalid cache type. Player cache is not allowed in this function.");
        return -1;
    }

    return ClassGetIndexByIdentifier(classIdent, cacheType);
}

/**
 * Native call function (ZR_GetClassDisplayName)
 *
 * native int ZR_GetClassDisplayName(int classIndex, char[] buffer, int maxlen, int cacheType = ZR_CLASS_CACHE_MODIFIED);
 */
public int APIGetClassDisplayName(Handle plugin, int numParams)
{
    int index = GetNativeCell(1);
    int maxlen = GetNativeCell(3);
    int cacheType = GetNativeCell(4);

    if (maxlen <= 0)
    {
        // No buffer size.
        return 0;
    }

    // Validate index.
    if (cacheType == ZR_CLASS_CACHE_PLAYER)
    {
        // Client index.
        APIValidateClientIndex(index, Condition_Either);
    }
    else
    {
        // Class index.
        if (!ClassValidateIndex(index))
        {
            ThrowNativeError(SP_ERROR_NATIVE, "Invalid class index. (%d)", index);
            return 0;
        }
    }

    char[] displayName = new char[maxlen];

    int bytes = ClassGetName(index, displayName, maxlen, cacheType);
    if (bytes <= 0)
    {
        // The class doesn't have a name for some reason. Make sure the buffer is empty.
        displayName[0] = 0;
    }

    SetNativeString(2, displayName, maxlen);

    return bytes;
}

/**
 * Native call function (ZR_GetClassIdentifier)
 *
 * native int ZR_GetClassIdentifier(int classIndex, char[] buffer, int maxlen, int cacheType = ZR_CLASS_CACHE_MODIFIED);
 */
public int APIGetClassIdentifier(Handle plugin, int numParams)
{
    int index = GetNativeCell(1);
    int maxlen = GetNativeCell(3);
    int cacheType = GetNativeCell(4);

    if (maxlen <= 0)
    {
        // No buffer size.
        return 0;
    }

    // Validate index.
    if (cacheType == ZR_CLASS_CACHE_PLAYER)
    {
        // Client index.
        APIValidateClientIndex(index, Condition_Either);
    }
    else
    {
        // Class index.
        if (!ClassValidateIndex(index))
        {
            ThrowNativeError(SP_ERROR_NATIVE, "Invalid class index. (%d)", index);
            return 0;
        }
    }

    char[] classIdent = new char[maxlen];

    int bytes = ClassGetIdentifier(index, classIdent, maxlen, cacheType);
    if (bytes <= 0)
    {
        // The class doesn't have a name for some reason. Make sure the buffer is empty.
        classIdent[0] = 0;
    }

    SetNativeString(2, classIdent, maxlen);

    return bytes;
}

/**
 * Native call function (ZR_SendMenuClass)
 *
 * native int ZR_SendMenuClass(int client, int teamid);
 * See: playerclasses/playerclasses.inc#L68-#L70
 */
public int APISendMenuClass(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    int teamid = GetNativeCell(2);

    // Send the class menu based on the teamid.
    // If not provided or invalid teamid, send class main menu
    if (teamid < ZR_CLASS_TEAM_ZOMBIES || teamid > ZR_CLASS_TEAM_ADMINS)
        ClassMenuMain(client);
    else
        ClassMenuSelect(client, teamid);

    return 0;
}

/**
 * Called after ZR has applied all classes attributes.
 *
 * @param client            The client index.
 * @param skinIndex         The skin index.
 * @noreturn
 */
void APIOnClassAttributesApplied(int client, int classindex)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClassAttributesApplied);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushCellRef(classindex);
    
    Call_Finish();
}

/**
 * Called right before ZR is about to apply the model to a player.
 *
 * @param client            The client index.
 * @param modelpath         Buffer to store the modelpath in.
 * @param skinIndex         The skin index.
 * @return result           Return the result of the Call.
 */
Action APIOnClassApplyModel(int client, char[] modelpath, int skinIndex)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClassApplyModel);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushString(modelpath);
    Call_PushCellRef(skinIndex);
    
    // Get what they returned.
    Action result;
    Call_Finish(result);
    return result;
}

/**
 * Called right after ZR has applied the model to a player.
 *
 * @param client            The client index.
 * @param modelpath         Buffer to store the modelpath in.
 * @param skinIndex         The skin index.
 * @noreturn
 */
void APIOnClassModelApplied(int client, char[] modelpath, int skinIndex)
{
    // Start forward call.
    Call_StartForward(g_hAPIFwdOnClassModelApplied);
    
    // Push the parameters.
    Call_PushCellRef(client);
    Call_PushString(modelpath);
    Call_PushCellRef(skinIndex);
    
    Call_Finish();
}