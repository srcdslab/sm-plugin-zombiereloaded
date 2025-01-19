/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zteleapitest.sp
 *  Type:          Test plugin
 *  Description:   Tests the ztele API.
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

#pragma semicolon 1
#include <sourcemod>
#include <zombiereloaded>

public Plugin myinfo =
{
    name = "Zombie:Reloaded Ztele API Test",
    author = ".Rushaway",
    description = "Tests the ztele API for ZR",
    version = "1.0.0",
    url = "http://code.google.com/p/zombiereloaded/"
};

public void OnPluginStart()
{
    LoadTranslations("common.phrases");

    RegConsoleCmd("zrtest_ztele", ZteleClient, "Ztele a player. Usage: zrtest_ztele <target> [1|0]");
}

public Action ZteleClient(int client, int argc)
{
    int target = -1;
    char valueString[64];

    if (argc >= 1)
    {
        GetCmdArg(1, valueString, sizeof(valueString));
        target = FindTarget(client, valueString);
    }

    if (target <= 0)
    {
        ReplyToCommand(client, "Ztele a player. Usage: zrtest_ztele <target> [1|0]");
        return Plugin_Handled;
    }

    bool bValue = false;

    if (argc > 1)
    {
        // Set value.
        GetCmdArg(2, valueString, sizeof(valueString));
        bValue = view_as<bool>(StringToInt(valueString));
    }

    ZR_ZteleClient(target, bValue);

    return Plugin_Handled;
}

public Action ZR_OnClientZtele(int client, bool force)
{
    PrintToChatAll("Client %d is about to be ztele. Force: %d", client, force);
    return Plugin_Continue;
}

public void ZR_OnClientZteled(int client)
{
    PrintToChatAll("Client %d zteled.", client);
}
