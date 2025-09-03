/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          menu_weapons.inc
 *  Type:          Core
 *  Description:   Handles weapons management menu.
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
 * Array to store the client's current weapon type within menu.
 */
int g_iWeaponsCurType[MAXPLAYERS + 1];

/**
 * Sends main weapon menu to client.
 *
 * @param client    The client index.
 */
bool WeaponsMenuMain(int client)
{
    // If weapons module is disabled, then stop.
    bool weapons = g_hCvarsList.CVAR_WEAPONS.BoolValue;
    if (!weapons)
    {
        return false;
    }

    // Create menu handle.
    Menu menu_weapons_main = new Menu(WeaponsMenuMainHandle);

    SetGlobalTransTarget(client);

    char title[MENU_LINE_TITLE_LENGTH];
    char restrict[MENU_LINE_SMALL_LENGTH];
    char zmarket[MENU_LINE_SMALL_LENGTH];

    Format(title, sizeof(title), "%t\n ", "Weapons menu restrict main title");
    Format(restrict, sizeof(restrict), "%t", "Weapons menu restrict main restrict");
    Format(zmarket, sizeof(zmarket), "%t", "Weapons menu restrict main market");

    // Draw items, make unselectable if module is disabled.
    menu_weapons_main.SetTitle(title);
    menu_weapons_main.AddItem("restrict", restrict, MenuGetItemDraw(g_hCvarsList.CVAR_WEAPONS_RESTRICT.BoolValue));
    menu_weapons_main.AddItem("zmarket", zmarket, MenuGetItemDraw(g_hCvarsList.CVAR_WEAPONS_ZMARKET.BoolValue));

    // Create a "Back" button to the weapons main menu.
    menu_weapons_main.ExitBackButton = true;

    // Send menu.
    menu_weapons_main.Display(client, MENU_TIME_FOREVER);

    return true;
}

/**
 * Called when client selects option in the weapons main menu, and handles it.
 *
 * @param menu_weapons_main     Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */
public int WeaponsMenuMainHandle(Menu menu_weapons_main, MenuAction action, int client, int slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            // Weapons.
            case 0:
            {
                WeaponsMenuTypes(client);
            }
            // ZMarket.
            case 1:
            {
                WeaponsMenuZMarket(client);
            }
        }
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            // Re-open admin menu.
            ZAdminMenu(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        delete menu_weapons_main;
    }
    return -1;
}

/**
 * Sends weapon type list to client.
 *
 * @param client    The client index.
 */
void WeaponsMenuTypes(int client)
{
    // Create menu handle.
    Menu menu_weapons_types = new Menu(WeaponsMenuTypesHandle);

    SetGlobalTransTarget(client);

    char title[MENU_LINE_TITLE_LENGTH];
    Format(title, sizeof(title), "%t\n ", "Weapons menu restrict types title");
    menu_weapons_types.SetTitle(title);

    char typename[WEAPONS_MAX_LENGTH];

    // x = Array index.
    int size = arrayWeaponTypes.Length;
    for (int x = 0; x < size; x++)
    {
        // Get name of type.
        RestrictWeaponTypeGetName(x, typename, sizeof(typename));

        // Add item to menu.
        menu_weapons_types.AddItem(typename, typename);
    }

    // If there are no weapons, add an "(Empty)" line.
    if (size == 0)
    {
        SetGlobalTransTarget(client);

        char empty[MENU_LINE_SMALL_LENGTH];
        Format(empty, sizeof(empty), "%t", "Menu empty");

        menu_weapons_types.AddItem("empty", empty, ITEMDRAW_DISABLED);
    }

    // Set exit back button.
    menu_weapons_types.ExitBackButton = true;

    menu_weapons_types.Display(client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapons list menu, and handles it.
 *
 * @param menu_weapons_types    Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */
public int WeaponsMenuTypesHandle(Menu menu_weapons_types, MenuAction action, int client, int slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Menu slot index is = weapon type index.
        g_iWeaponsCurType[client] = slot;

        // Send weapons of the selected type in a menu to client.
        WeaponsMenuTypeWeapons(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            WeaponsMenuMain(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        delete menu_weapons_types;
    }
    return -1;
}

/**
 * Sends a list of weapons of a certain type in a menu to the client.
 *
 * @param client    The client index.
 */
void WeaponsMenuTypeWeapons(int client)
{
    // Create menu handle.
    Menu menu_weapons_typeweapons = new Menu(WeaponsMenuTypeWeaponsHandle);

    char typename[WEAPONS_MAX_LENGTH];
    RestrictWeaponTypeGetName(g_iWeaponsCurType[client], typename, sizeof(typename));

    // Set translation language.
    SetGlobalTransTarget(client);

    char title[MENU_LINE_TITLE_LENGTH];
    char restrictall[MENU_LINE_REG_LENGTH];
    char unrestrictall[MENU_LINE_REG_LENGTH];

    Format(title, sizeof(title), "%t\n ", "Weapons menu restrict types weapon type title", typename);
    Format(restrictall, sizeof(restrictall), "%t", "Weapons menu restrict types restrict all", typename);
    Format(unrestrictall, sizeof(unrestrictall), "%t\n ", "Weapons menu restrict types unrestrict all", typename);

    // Draw items as selectable only if not all weapons within the type are restricted or unrestricted.
    menu_weapons_typeweapons.SetTitle(title);
    menu_weapons_typeweapons.AddItem("restrictall", restrictall, MenuGetItemDraw(!RestrictIsTypeUniform(true, g_iWeaponsCurType[client])));
    menu_weapons_typeweapons.AddItem("unrestrictall", unrestrictall, MenuGetItemDraw(!RestrictIsTypeUniform(false, g_iWeaponsCurType[client])));

    char typeweapon[MENU_LINE_SMALL_LENGTH];
    char display[MENU_LINE_SMALL_LENGTH + 2]; // +2 to allow room for the [ ] if its restricted.

    // Get an array populated with all weapons of the given type.
    ArrayList arrayTypeWeapons;
    int count = RestrictGetTypeWeapons(g_iWeaponsCurType[client], arrayTypeWeapons);

    // x = Array index.
    for (int x = 0; x < count; x++)
    {
        // Get weapon index to check restricted status of.
        int weaponindex = arrayTypeWeapons.Get(x);

        // Get name of weapon.
        WeaponsGetName(weaponindex, typeweapon, sizeof(typeweapon));
        strcopy(display, sizeof(display), typeweapon);

        if (RestrictIsWeaponRestricted(weaponindex))
        {
            Format(display, sizeof(display), "[%s]", typeweapon);
        }

        // Disable option if it isn't toggleable.
        menu_weapons_typeweapons.AddItem(typeweapon, display, MenuGetItemDraw(WeaponsGetToggleable(weaponindex)));
    }

    // Destroy the array handle.
    delete arrayTypeWeapons;

    // Set menu back button.
    menu_weapons_typeweapons.ExitBackButton = true;

    // Display menu to client.
    menu_weapons_typeweapons.Display(client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapon group menu, and handles it.
 *
 * @param menu_weapons_typeweapons  Handle of the menu being used.
 * @param action                    The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                    The client index.
 * @param slot                      The slot index selected (starting from 0).
 */
public int WeaponsMenuTypeWeaponsHandle(Menu menu_weapons_typeweapons, MenuAction action, int client, int slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get name of current weapon type.
        char typename[WEAPONS_MAX_LENGTH];
        RestrictWeaponTypeGetName(g_iWeaponsCurType[client], typename, sizeof(typename));

        RestrictQuery query;
        bool single;
        bool restrict;
        char returntarget[WEAPONS_MAX_LENGTH];

        switch(slot)
        {
            case 0:
            {
                // Restrict all weapons of this type.
                restrict = true;
                query = RestrictWeapon(true, typename, single, returntarget, sizeof(returntarget));
                LogAction(client, -1, "[ZR] %L Restricted weapon type : %s.", client, typename);
            }
            case 1:
            {
                // Unrestrict all weapons of this type.
                restrict = false;
                query = RestrictWeapon(false, typename, single, returntarget, sizeof(returntarget));
                LogAction(client, -1, "[ZR] %L Unrestricted weapon type : %s.", client, typename);
            }
            default:
            {
                // Get weappon name.
                char typeweapon[MENU_LINE_REG_LENGTH];
                menu_weapons_typeweapons.GetItem(slot, typeweapon, sizeof(typeweapon));

                // Get weapon index.
                int weaponindex = WeaponsNameToIndex(typeweapon);

                // If weapon index is -1, then something went very wrong.
                if (weaponindex == -1)
                {
                    delete menu_weapons_typeweapons;
                    return -1;
                }

                // If weapon isn't restricted, then restrict it.
                if (!RestrictIsWeaponRestricted(weaponindex))
                {
                    // Restrict this weapon.
                    restrict = true;
                    query = RestrictWeapon(true, typeweapon, single, returntarget, sizeof(returntarget));
                    LogAction(client, -1, "[ZR] %L Restricted weapon type : %s.", client, typeweapon);
                }
                else
                {
                    // Unrestrict this weapon.
                    restrict = false;
                    query = RestrictWeapon(false, typeweapon, single, returntarget, sizeof(returntarget));
                    LogAction(client, -1, "[ZR] %L Unrestricted weapon type : %s.", client, typeweapon);
                }
            }
        }

        // Print query response.
        RestrictPrintQueryResponse(client, query, single, restrict, returntarget);

        // Resend menu.
        WeaponsMenuTypeWeapons(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            WeaponsMenuTypes(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        delete menu_weapons_typeweapons;
    }
    return -1;
}

/**
 * Sends ZMarket options menu to client.
 *
 * @param client    The client index.
 */
void WeaponsMenuZMarket(int client)
{
    // Create menu handle.
    Menu menu_weapons_market = new Menu(WeaponsMenuZMarketHandle);

    // Get "yes" or "no" settings from respective cvar.
    char buyzonesetting[MENU_LINE_SMALL_LENGTH];
    ConfigBoolToSetting(g_hCvarsList.CVAR_WEAPONS_ZMARKET_BUYZONE.BoolValue, buyzonesetting, sizeof(buyzonesetting), true, client);

    SetGlobalTransTarget(client);

    char title[MENU_LINE_TITLE_LENGTH];
    char buyzone[MENU_LINE_REG_LENGTH];

    // Add options to menu.
    Format(title, sizeof(title), "%t\n ", "Weapons menu restrict zmarket title");
    Format(buyzone, sizeof(buyzone), "%t", "Weapons menu restrict zmarket buyzone", buyzonesetting);

    menu_weapons_market.SetTitle(title);
    menu_weapons_market.AddItem("buyzone", buyzone);

    // Create a "Back" button to the weapons main menu.
    menu_weapons_market.ExitBackButton = true;

    // Send menu
    menu_weapons_market.Display(client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapons main menu, and handles it.
 *
 * @param menu_weapons_market   Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */
public int WeaponsMenuZMarketHandle(Menu menu_weapons_market, MenuAction action, int client, int slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            // Buyzone.
            case 0:
            {
                // Toggle cvar.
                bool zmarketbuyzone = g_hCvarsList.CVAR_WEAPONS_ZMARKET_BUYZONE.BoolValue;
                g_hCvarsList.CVAR_WEAPONS_ZMARKET_BUYZONE.BoolValue = !zmarketbuyzone;
                if (zmarketbuyzone == false)
                {
                    LogAction(client, -1, "[ZR] \"%L\" Requires player to be inside a buyzone to use ZMarket.", client);
                }
                else
                {
                    LogAction(client, -1, "[ZR] \"%L\" Does not requires player to be inside a buyzone to use ZMarket.", client);
                }
            }
        }

        // Resend menu.
        WeaponsMenuZMarket(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            WeaponsMenuMain(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        delete menu_weapons_market;
    }
    return -1;
}
