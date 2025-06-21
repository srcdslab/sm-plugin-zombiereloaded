/*
* ============================================================================
*
*  Zombie:Reloaded
*
*  File:          knockback_weapons.inc
*  Type:          Core
*  Description:   Command for setting the knockback of a weapon.
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
 * Create commands specific to Knockback Weapons.
 */
void KnockbackWeaponsOnCommandsCreate()
{
    // Register Knockback Weapons command.
    RegConsoleCmd("zr_setknockback", Command_SetKnockback, "Set the knockback of a weapon. Usage: zr_setknockback <weapon> <knockback>");
}

public Action Command_SetKnockback(int client, int argc)
{
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Configuration))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }

    if (argc < 2)
    {
        TranslationReplyToCommand(client, "Weapons command setknockback syntax");
        return Plugin_Handled;
    }

    char weapon[64];
    GetCmdArg(1, weapon, sizeof(weapon));

    char knockback[16];
    GetCmdArg(2, knockback, sizeof(knockback));

    float knockbackFloat = StringToFloat(knockback);

    // If negative convert to positive
    if (knockbackFloat < 0.0)
        knockbackFloat = -knockbackFloat;

    if (ZR_SetWeaponKnockback(weapon, knockbackFloat))
    {
        TranslationReplyToCommand(client, "Weapons command setknockback success", weapon, knockbackFloat);
    }
    else
    {
        TranslationReplyToCommand(client, "Weapons command setknockback failed", weapon, knockbackFloat);
    }

    return Plugin_Handled;
}
