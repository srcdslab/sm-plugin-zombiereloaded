/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          admintools.inc
 *  Type:          Core
 *  Description:   Functions for checking extended admin privileges.
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
 * @section Pre-defined SourceMod group names for authenticating players.
 */
#define ZR_GROUP_ADMINS         "zr_admins"
#define ZR_GROUP_MODERATORS     "zr_moderators"
#define ZR_GROUP_CONFIGURATORS  "zr_configurators"
/**
 * @endsection
 */

/**
 * List of operation types to specify the category of a admin operation.
 */
enum OperationTypes
{
    OperationType_Invalid = -1,     /** Invalid operation type. */
    OperationType_Generic,          /** Generic events like infecting or teleporting players. */
    OperationType_Configuration,    /** Changing settings. */
}

/**
 * Resolves an admin flag from the first character in a cvar.
 * Falls back to defaultFlag if the cvar is empty or invalid.
 *
 * @param cvar          Cvar with flag character.
 * @param defaultFlag   Fallback admin flag.
 * @return              Resolved admin flag.
 */
stock AdminFlag ZRGetPermissionFlagFromCvar(ConVar cvar, AdminFlag defaultFlag)
{
    char flagString[16];
    cvar.GetString(flagString, sizeof(flagString));

    switch (flagString[0])
    {
        case 'a', 'A': return Admin_Reservation;
        case 'b', 'B': return Admin_Generic;
        case 'c', 'C': return Admin_Kick;
        case 'd', 'D': return Admin_Ban;
        case 'e', 'E': return Admin_Unban;
        case 'f', 'F': return Admin_Slay;
        case 'g', 'G': return Admin_Changemap;
        case 'h', 'H': return Admin_Convars;
        case 'i', 'I': return Admin_Config;
        case 'j', 'J': return Admin_Chat;
        case 'k', 'K': return Admin_Vote;
        case 'l', 'L': return Admin_Password;
        case 'm', 'M': return Admin_RCON;
        case 'n', 'N': return Admin_Cheats;
        case 'o', 'O': return Admin_Custom1;
        case 'p', 'P': return Admin_Custom2;
        case 'q', 'Q': return Admin_Custom3;
        case 'r', 'R': return Admin_Custom4;
        case 's', 'S': return Admin_Custom5;
        case 't', 'T': return Admin_Custom6;
        case 'z', 'Z': return Admin_Root;
    }

    return defaultFlag;
}


/**
 * Returns whether a player is allowed to do a certain operation or not.
 *
 * @param client            The client index.
 * @param operationType     The operation category.
 * @return                  True if allowed, false otherwise.
 */
stock bool ZRIsClientPrivileged(int client, OperationTypes operationType = OperationType_Generic)
{
    // Check if console.
    if (client == 0)
    {
        // Console always has full access no matter what.
        return true;
    }

    // Validate client index.
    if (!ZRIsClientValid(client))
    {
        return false;
    }

    // Check if group authentication is used.
    bool groupauth = g_hCvarsList.CVAR_PERMISSIONS_USE_GROUPS.BoolValue;
    if (groupauth)
    {
        /**********************************
         *                                *
         *   GROUP BASED AUTHENTICATION   *
         *                                *
         **********************************/

        // Check if client is full admin.
        if (ZRIsClientInGroup(client, ZR_GROUP_ADMINS))
        {
            return true;
        }

        // Check operation type.
        switch (operationType)
        {
            case OperationType_Generic:
            {
                return ZRIsClientInGroup(client, ZR_GROUP_MODERATORS);
            }
            case OperationType_Configuration:
            {
                return ZRIsClientInGroup(client, ZR_GROUP_CONFIGURATORS);
            }
        }

        // Invalid operation type.
        return false;
    }
    else
    {
        /*********************************
         *                               *
         *   FLAG BASED AUTHENTICATION   *
         *                               *
         *********************************/

        AdminFlag flag;

        // Check operation type.
        switch (operationType)
        {
            case OperationType_Generic:
            {
                flag = ZRGetPermissionFlagFromCvar(g_hCvarsList.CVAR_PERMISSIONS_FLAG_GENERIC, Admin_Ban);
            }
            case OperationType_Configuration:
            {
                flag = ZRGetPermissionFlagFromCvar(g_hCvarsList.CVAR_PERMISSIONS_FLAG_CONFIGURATION, Admin_Config);
            }
            default:
            {
                // Invalid operation type.
                return false;
            }
        }

        return GetAdminFlag(GetUserAdmin(client), flag);
    }
}

/**
 * Returns whether a player is in a spesific group or not.
 *
 * @param client        The client index.
 * @param groupName     SourceMod group name to check.
 * @return              True if in the group, false otherwise.
 */
stock bool ZRIsClientInGroup(int client, const char[] groupName)
{
    AdminId id = GetUserAdmin(client);

    // Validate id.
    if (id == INVALID_ADMIN_ID)
    {
        return false;
    }

    // Get number of groups.
    int groupnum = GetAdminGroupCount(id);
    char groupname[64];

    // Validate number of groups.
    if (groupnum > 0)
    {
        // Loop through each group.
        for (int group = 0; group < groupnum; group++)
        {
            // Get group name.
            GetAdminGroup(id, group, groupname, sizeof(groupname));

            // Compare names.
            if (strcmp(groupName, groupname, false) == 0)
            {
                return true;
            }
        }
    }

    // No groups or no match.
    return false;
}
