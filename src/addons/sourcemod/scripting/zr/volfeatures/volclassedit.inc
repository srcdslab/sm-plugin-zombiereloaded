/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volclassedit.inc
 *  Type:           Module
 *  Description:    Class editor volumetric feature.
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

#define VOL_CLASSNAME_SIZE  64

/**
 * Empty attribute structure with all settings set to be ignored.
 */
ClassEditableAttributes VolEmptyAttributes = {
    -1,                             /** ModelSkinIndex */
    -1,                             /** AlphaInitial */
    -1,                             /** AlphaDamaged */
    -1,                             /** AlphaDamage */

    "nochange",                     /** OverlayPath */
    -1,                             /** Nvgs */
    -1,                             /** Fov */

    -1,                             /** HasNapalm */
    -1.0,                           /** NapalmTime */
    Immunity_Invalid,               /** ImmunityMode */
    -1,                             /** ImmunityAmount */
    -1,                             /** ImmunityCooldown */
    -1,                             /** NoFallDamage */

    -1.0,                           /** RegenInterval */
    -1,                             /** RegenAmount */
    -1,                             /** InfectGain */
    -1,                             /** KillBonus */

    -1.0,                           /** Speed */
    ZR_CLASS_KNOCKBACK_IGNORE,      /** KnockBack */
    -1.0,                           /** JumpHeight */
    -1.0,                           /** JumpDistance */
};

/**
 * List of class edit modes.
 */
enum VolClassEditMode
{
    ClassEditMode_Name,         /** Change whole player class. */
    ClassEditMode_Attributes    /** Override class attributes. */
}

/**
 * Data structure for class editor.
 */
enum struct VolTypeClassEdit
{
    bool VolClassEdit_InUse;

    VolClassEditMode VolClassEdit_Mode;
    char VolClassEdit_ClassName[VOL_CLASSNAME_SIZE];
    ClassEditableAttributes VolClassEdit_ClassData;
}

/**
 * Class editor data.
 */
VolTypeClassEdit VolClassEditData[ZR_VOLUMES_MAX];

/**
 * The player's selected class index. Used by volumes in "name" mode.
 */
int VolClassEditSelectedClass[MAXPLAYERS + 1];

/**
 * Checks if the specified index is in use.
 *
 * @param dataIndex     Index to check.
 * @return              True if in use, false otherwise.
 */
bool VolClassEditInUse(int dataIndex)
{
    return VolClassEditData[dataIndex].VolClassEdit_InUse;
}

/**
 * Gets the first free data index.
 *
 * @return  Data index or -1 if failed.
 */
int VolClassEditGetFreeIndex()
{
    // Loop through all indexes.
    for (int dataindex = 0; dataindex < ZR_VOLUMES_MAX; dataindex++)
    {
        // Check if unused.
        if (!VolClassEditInUse(dataindex))
        {
            // Mark as in use.
            VolClassEditData[dataindex].VolClassEdit_InUse = true;

            // Unused index found.
            return dataindex;
        }
    }

    // Unused index not found.
    return -1;
}

void VolClassEditReset(int dataIndex)
{
    VolClassEditData[dataIndex].VolClassEdit_InUse = false;
    VolClassEditData[dataIndex].VolClassEdit_Mode = ClassEditMode_Attributes;

    strcopy(VolClassEditData[dataIndex].VolClassEdit_ClassName, 64, "");
    VolClassEditData[dataIndex].VolClassEdit_ClassData = VolEmptyAttributes;
}

/**
 * Initialize class editor.
 */
void VolClassEditInit()
{
    for (int dataindex = 0; dataindex < ZR_VOLUMES_MAX; dataindex++)
    {
        VolClassEditReset(dataindex);
    }
}

/**
 * Sets class editor spesific attributes on a anticamp volume.
 *
 * @param dataIndex     Local data index.
 * @param attribName    Attribute to modify.
 * @param attribVlue    Attribute value to set.
 * @return              True if successfully set, false otherwise.
 */
int VolClassEditSetAttribute(int dataIndex, const char[] attribName, const char[] attribValue)
{
    // Validate index.
    if (!VolIsValidIndex(dataIndex))
    {
        return false;
    }

    /* Class Editor Attributes */
    if (strcmp(attribName, "mode", false) == 0)
    {
        if (VolClassEditSetMode(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "name", false) == 0)
    {
        if (VolClassEditSetName(dataIndex, attribValue))
        {
            return true;
        }
    }

    /* Model */
    else if (strcmp(attribName, "alpha_initial", false) == 0)
    {
        if (VolClassEditSetAlphaInitial(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "alpha_damaged", false) == 0)
    {
        if (VolClassEditSetAlphaDamaged(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "alpha_damage", false) == 0)
    {
        if (VolClassEditSetAlphaDamage(dataIndex, attribValue))
        {
            return true;
        }
    }

    /* Hud */
    else if (strcmp(attribName, "overlay_path", false) == 0)
    {
        if (VolClassEditSetOverlayPath(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "nvgs", false) == 0)
    {
        if (VolClassEditSetNvgs(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "fov", false) == 0)
    {
        if (VolClassEditSetFov(dataIndex, attribValue))
        {
            return true;
        }
    }

    /* Effects */
    else if (strcmp(attribName, "has_napalm", false) == 0)
    {
        if (VolClassEditSetHasNapalm(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "napalm_time", false) == 0)
    {
        if (VolClassEditSetNapalmTime(dataIndex, attribValue))
        {
            return true;
        }
    }

    /* Player behavior */
    else if (strcmp(attribName, "immunity_mode", false) == 0)
    {
        if (VolClassEditSetImmunityMode(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "immunity_amount", false) == 0)
    {
        if (VolClassEditSetImmunityAmount(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "immunity_cooldown", false) == 0)
    {
        if (VolClassEditSetImmunityCooldown(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "no_fall_damage", false) == 0)
    {
        if (VolClassEditSetNoFallDamage(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "health_regen_interval", false) == 0)
    {
        if (VolClassEditSetRegenInterval(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "health_regen_amount", false) == 0)
    {
        if (VolClassEditSetRegenAmount(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "infect_gain", false) == 0)
    {
        if (VolClassEditSetInfectGain(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "kill_bonus", false) == 0)
    {
        if (VolClassEditSetKillBonus(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "speed", false) == 0)
    {
        if (VolClassEditSetSpeed(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "knockback", false) == 0)
    {
        if (VolClassEditSetKnockBack(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "jump_height", false) == 0)
    {
        if (VolClassEditSetJumpHeight(dataIndex, attribValue))
        {
            return true;
        }
    }
    else if (strcmp(attribName, "jump_distance", false) == 0)
    {
        if (VolClassEditSetJumpDistance(dataIndex, attribValue))
        {
            return true;
        }
    }

    // Invalid attribute name or empty value.
    return false;
}

/**
 * Dumps data to be used by zr_vol_list command.
 *
 * @param dataIndex     Index in anticamp data array.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 * @return              Number of cells written.
 */
int VolClassEditDumpData(int dataIndex, char[] buffer, int maxlen)
{
    #define CLASSEDIT_DUMP_FORMAT "%-19s %s\n"

    char linebuffer[128];
    char valuebuffer[256];
    VolTypeClassEdit cache;
    int cellswritten;

    // Validate index.
    if (!VolIsValidIndex(dataIndex))
    {
        return 0;
    }

    // Initialize and clear buffer.
    buffer[0] = 0;

    // Cache data.
    cache = VolClassEditData[dataIndex];

    VolClassEditModeToString(cache.VolClassEdit_Mode, valuebuffer, sizeof(valuebuffer));
    Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Mode:", valuebuffer);
    cellswritten += StrCat(buffer, maxlen, linebuffer);

    switch (cache.VolClassEdit_Mode)
    {
        case ClassEditMode_Name:
        {
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Class Name:", cache.VolClassEdit_ClassName);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
        }
        case ClassEditMode_Attributes:
        {
            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_AlphaInitial, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Alpha initial:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_AlphaDamaged, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Alpha damaged:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_AlphaDamage, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Alpha damage:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditStringToHumanStr(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_OverlayPath, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Overlay path:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_Nvgs, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "NVGs:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_Fov, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "FOV:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_HasNapalm, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Has napalm:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditFloatToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_NapalmTime, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Napalm time:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            ImmunityModeToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_ImmunityMode, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Immunity mode:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_ImmunityAmount, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Immunity amount:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_ImmunityCooldown, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Immunity cooldown:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_NoFallDamage, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "No fall damage:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditFloatToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_RegenInterval, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Regen interval:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_RegenAmount, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Regen amount:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_InfectGain, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Infect gain:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditIntToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_KillBonus, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Kill bonus:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditFloatToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_Speed, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Speed:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditFloatToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_KnockBack, valuebuffer, sizeof(valuebuffer), ZR_CLASS_KNOCKBACK_IGNORE);
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Knock back:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditFloatToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_JumpHeight, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Jump height:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);

            VolClassEditFloatToString(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_JumpDistance, valuebuffer, sizeof(valuebuffer));
            Format(linebuffer, sizeof(linebuffer), CLASSEDIT_DUMP_FORMAT, "Jump distance:", valuebuffer);
            cellswritten += StrCat(buffer, maxlen, linebuffer);
        }
    }

    return cellswritten;
}


/**************************************
 *                                    *
 *  EVENTS                            *
 *                                    *
 **************************************/

/**
 * Applies new class attributes to the player.
 *
 * @param client        The client index.
 * @param dataIndex     Local data index.
 */
void VolClassEditApply(int client, int dataIndex)
{
    switch (VolClassEditData[dataIndex].VolClassEdit_Mode)
    {
        case ClassEditMode_Name:
        {
            // Cache volume attributes.
            int classindex = ClassGetIndexByName(VolClassEditData[dataIndex].VolClassEdit_ClassName);

            // Save player's selected class.
            VolClassEditSelectedClass[client] = ClassGetActiveIndex(client);

            // Update cache with new attributes.
            ClassReloadPlayerCache(client, classindex);
        }
        case ClassEditMode_Attributes:
        {
            // Update player's entire cache with attributes from the specified
            // class attributes.
            VolClassEditUpdateAttributes(client, VolClassEditData[dataIndex].VolClassEdit_ClassData);
        }
    }

    LogEvent(_, LogType_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Applied class data on client %d.", client);

    // Apply the updated attributes.
    ClassApplyAttributes(client);
}

/**
 * Restores the player's regular class attributes (from modified cache).
 *
 * @param client        The client index.
 */
void VolClassEditRestore(int client, int dataIndex)
{
    int classindex = ClassGetActiveIndex(client);

    switch (VolClassEditData[dataIndex].VolClassEdit_Mode)
    {
        case ClassEditMode_Name:
        {
            // Restore player's entire cache with attributes from the selected
            // class.
            ClassReloadPlayerCache(client, VolClassEditSelectedClass[client]);
        }
        case ClassEditMode_Attributes:
        {
            // Restore individual attributes, if specified.
            VolClassEditRestoreAttributes(client, classindex, VolClassEditData[dataIndex].VolClassEdit_ClassData);
        }
    }

    // Apply the restored attributes.
    if (ClassApplyAttributes(client))
    {
        LogEvent(_, LogType_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Restored class data on client %d.", client);
    }
}

/**
 * Event callback. A player entered a class edit volume.
 *
 * @param client        The client index.
 * @param volumeIndex   The volume index.
 */
void VolClassEditOnPlayerEnter(int client, int volumeIndex)
{
    VolClassEditApply(client, Volumes[volumeIndex].Vol_DataIndex);
}

/**
 * Event callback. A player left a class edit volume.
 *
 * @param client        The client index.
 * @param volumeIndex   The volume index.
 */
void VolClassEditOnPlayerLeave(int client, int volumeIndex)
{
    VolClassEditRestore(client, Volumes[volumeIndex].Vol_DataIndex);
}

/**
 * Event callback. A class edit volume was enabled.
 *
 * @param volumeIndex   The volume index.
 */
/*VolClassEditOnEnabled(volumeIndex)
{

}*/

/**
 * Event callback. A class edit volume was enabled.
 *
 * @param volumeIndex   The volume index.
 */
void VolClassEditOnDisabled(int volumeIndex)
{
    for (int client = 1; client < MaxClients; client++)
    {
        if (IsClientConnected(client) && IsClientInGame(client))
        {
            // Only forward event if the player really is in the volume..
            if (VolPlayerInVolume[client][volumeIndex] && VolClassEditSelectedClass[client] >= 0)
            {
                VolClassEditOnPlayerLeave(client, volumeIndex);
            }
        }
    }
}


/**************************************
 *                                    *
 *  ATTRIBUTE FUNCTIONS               *
 *                                    *
 **************************************/

/**
 * Converts the specified mode to a string.
 *
 * @param mode      Mode to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written.
 */
int VolClassEditModeToString(VolClassEditMode mode, char[] buffer, int maxlen)
{
    switch (mode)
    {
        case ClassEditMode_Name:
        {
            return strcopy(buffer, maxlen, "Name");
        }
        case ClassEditMode_Attributes:
        {
            return strcopy(buffer, maxlen, "Attributes");
        }
    }

    return 0;
}

/**
 * Gets a integer attribute and converts it to a human readable string.
 *
 * Note: attribute is assumed to be a integer (cell) and is not type cheked!
 *
 * @param intVal        Value to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 */
int VolClassEditIntToString(int intVal, char[] buffer, int maxlen)
{
    char strVal[8];

    // Check if the attribute is marked as ignored.
    if (intVal == -1)
    {
        return strcopy(buffer, maxlen, "(no change)");
    }
    IntToString(intVal, strVal, sizeof(strVal));
    return strcopy(buffer, maxlen, strVal);
}

/**
 * Gets a float attribute and converts it to a human readable string.
 *
 * Note: attribute is assumed to be a float and is not type cheked!
 *
 * @param floatVal      Value to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 */
int VolClassEditFloatToString(float floatVal, char[] buffer, int maxlen, float minval = -1.0)
{
    char strVal[8];

    // Check if the attribute is marked as ignored.
    if (floatVal == minval)
    {
        return strcopy(buffer, maxlen, "(no change)");
    }
    FloatToString(floatVal, strVal, sizeof(strVal));
    return strcopy(buffer, maxlen, strVal);
}

/**
 * Gets a string attribute and converts it to a human readable string.
 *
 * Note: attribute is assumed to be a string and is not type cheked!
 *
 * @param strVal        Value to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of destination buffer.
 */
int VolClassEditStringToHumanStr(char strVal[PLATFORM_MAX_PATH], char[] buffer, int maxlen)
{
    // Check if the attribute is marked as ignored.
    if (strcmp(strVal, "nochange", false) == 0)
    {
        return strcopy(buffer, maxlen, "(no change)");
    }
    return strcopy(buffer, maxlen, strVal);
}

/**
 * Sets the mode attribute.
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetMode(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    if (strcmp(value, "name", false) == 0)
    {
        VolClassEditData[dataIndex].VolClassEdit_Mode = ClassEditMode_Name;
        return true;
    }
    else if (strcmp(value, "attributes", false) == 0)
    {
        VolClassEditData[dataIndex].VolClassEdit_Mode = ClassEditMode_Attributes;
        return true;
    }

    // No match.
    return false;
}

/**
 * Sets the class name attribute (name of the class to switch to).
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetName(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    strcopy(VolClassEditData[dataIndex].VolClassEdit_ClassName, VOL_CLASSNAME_SIZE, value);

    // TODO: Validate name.

    return true;
}

/**
 * Sets the alpha initial attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetAlphaInitial(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_AlphaInitial = StringToInt(value);
    return true;
}

/**
 * Sets the alpha damaged attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetAlphaDamaged(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_AlphaDamaged = StringToInt(value);
    return true;
}

/**
 * Sets the alpha damage attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetAlphaDamage(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_AlphaDamage = StringToInt(value);
    return true;
}

/**
 * Sets the overlay path attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetOverlayPath(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    strcopy(VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_OverlayPath, PLATFORM_MAX_PATH, value);
    return true;
}

/**
 * Sets the nvgs attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetNvgs(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_Nvgs = StringToInt(value);
    return true;
}

/**
 * Sets the fov attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetFov(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_Fov = StringToInt(value);
    return true;
}

/**
 * Sets the has napalm attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetHasNapalm(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_HasNapalm = StringToInt(value);
    return true;
}

/**
 * Sets the napalm time attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetNapalmTime(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_NapalmTime = StringToFloat(value);
    return true;
}

/**
 * Sets the immunity mode attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetImmunityMode(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    ImmunityMode mode = ImmunityStringToMode(value);
    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_ImmunityMode = mode;
    return true;
}

/**
 * Sets the immunity amount attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetImmunityAmount(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_ImmunityAmount = StringToInt(value);
    return true;
}

/**
 * Sets the immunity cooldown attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetImmunityCooldown(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_ImmunityCooldown = StringToInt(value);
    return true;
}

/**
 * Sets the no fall damage attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetNoFallDamage(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_NoFallDamage = StringToInt(value);
    return true;
}

/**
 * Sets the health regen interval attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetRegenInterval(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_RegenInterval = StringToFloat(value);
    return true;
}

/**
 * Sets the health regen amount attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetRegenAmount(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_RegenAmount = StringToInt(value);
    return true;
}

/**
 * Sets the infect gain attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetInfectGain(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_InfectGain = StringToInt(value);
    return true;
}

/**
 * Sets the kill bonus attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetKillBonus(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_KillBonus = StringToInt(value);
    return true;
}

/**
 * Sets the speed attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetSpeed(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_Speed = StringToFloat(value);
    return true;
}

/**
 * Sets the knock back attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetKnockBack(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_KnockBack = StringToFloat(value);
    return true;
}

/**
 * Sets the jump heigt attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetJumpHeight(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_JumpHeight = StringToFloat(value);
    return true;
}

/**
 * Sets the jump distance attribute.
 *
 * Note: The value is not validated!
 *
 * @param dataIndex     Local data index.
 * @param value         String value to set (converted to proper type by this
 *                      function).
 * @return              True if set, false if empty.
 */
bool VolClassEditSetJumpDistance(int dataIndex, const char[] value)
{
    if (strlen(value) == 0)
    {
        return false;
    }

    VolClassEditData[dataIndex].VolClassEdit_ClassData.ClassEdit_JumpDistance = StringToFloat(value);
    return true;
}

/**
 * Updates a player's cache with the specified attribute set. Only active
 * attributes are applied.
 *
 * Note: These attributes are not validated!
 *
 * @param client        The client index.
 * @param attributes    Attribute set to apply (ClassEditableAttributes)
 * @return              Number of attributes changed.
 */
int VolClassEditUpdateAttributes(int client, const ClassEditableAttributes attributes)
{
    int numChanges;

    // Alpha initial.
    if (attributes.ClassEdit_AlphaInitial > -1)
    {
        ClassPlayerCache[client].Class_AlphaInitial = attributes.ClassEdit_AlphaInitial;
        numChanges++;
    }

    // Alpha damaged.
    if (attributes.ClassEdit_AlphaDamaged > -1)
    {
        ClassPlayerCache[client].Class_AlphaDamaged = attributes.ClassEdit_AlphaDamaged;
        numChanges++;
    }

    // Alpha damage.
    if (attributes.ClassEdit_AlphaDamage > -1)
    {
        ClassPlayerCache[client].Class_AlphaDamage = attributes.ClassEdit_AlphaDamage;
        numChanges++;
    }

    // Overlay path.
    if (strcmp(attributes.ClassEdit_OverlayPath, "nochange", false) != 0)
    {
        strcopy(ClassPlayerCache[client].Class_OverlayPath, PLATFORM_MAX_PATH, attributes.ClassEdit_OverlayPath);
        numChanges++;
    }

    // Nvgs.
    if (attributes.ClassEdit_Nvgs > -1)
    {
        ClassPlayerCache[client].Class_Nvgs = view_as<bool>(attributes.ClassEdit_Nvgs);
        numChanges++;
    }

    // Napalm time.
    if (attributes.ClassEdit_NapalmTime > -1.0)
    {
        ClassPlayerCache[client].Class_NapalmTime = attributes.ClassEdit_NapalmTime;
        numChanges++;
    }

    // Immunity mode.
    if (attributes.ClassEdit_ImmunityMode != Immunity_Invalid)
    {
        ClassPlayerCache[client].Class_ImmunityMode = attributes.ClassEdit_ImmunityMode;
        numChanges++;
    }

    // Immunity amount.
    if (attributes.ClassEdit_ImmunityAmount > -1)
    {
        ClassPlayerCache[client].Class_ImmunityAmount = attributes.ClassEdit_ImmunityAmount;
        numChanges++;
    }

    // Immunity cooldown.
    if (attributes.ClassEdit_ImmunityCooldown > -1)
    {
        ClassPlayerCache[client].Class_ImmunityCooldown = attributes.ClassEdit_ImmunityCooldown;
        numChanges++;
    }

    // No fall damage.
    if (attributes.ClassEdit_NoFallDamage > -1)
    {
        ClassPlayerCache[client].Class_NoFallDamage = view_as<bool>(attributes.ClassEdit_NoFallDamage);
        numChanges++;
    }

    // Health regen interval.
    if (attributes.ClassEdit_RegenInterval > -1.0)
    {
        ClassPlayerCache[client].Class_HealthRegenInterval = attributes.ClassEdit_RegenInterval;
        numChanges++;
    }

    // Health regen amount.
    if (attributes.ClassEdit_RegenAmount > -1)
    {
        ClassPlayerCache[client].Class_HealthRegenAmount = attributes.ClassEdit_RegenAmount;
        numChanges++;
    }

    // Infect gain.
    if (attributes.ClassEdit_InfectGain > -1)
    {
        ClassPlayerCache[client].Class_HealthInfectGain = attributes.ClassEdit_InfectGain;
        numChanges++;
    }

    // Kill bonus.
    if (attributes.ClassEdit_KillBonus > -1)
    {
        ClassPlayerCache[client].Class_KillBonus = attributes.ClassEdit_KillBonus;
        numChanges++;
    }

    // Speed.
    if (attributes.ClassEdit_Speed > -1.0)
    {
        ClassPlayerCache[client].Class_Speed = attributes.ClassEdit_Speed;
        numChanges++;
    }

    // Knock back.
    if (attributes.ClassEdit_KnockBack > ZR_CLASS_KNOCKBACK_IGNORE)
    {
        ClassPlayerCache[client].Class_KnockBack = attributes.ClassEdit_KnockBack;
        numChanges++;
    }

    // Jump height.
    if (attributes.ClassEdit_JumpHeight > -1.0)
    {
        ClassPlayerCache[client].Class_JumpHeight = attributes.ClassEdit_JumpHeight;
        numChanges++;
    }

    // Jump distance.
    if (attributes.ClassEdit_JumpDistance > -1.0)
    {
        ClassPlayerCache[client].Class_JumpDistance = attributes.ClassEdit_JumpDistance;
        numChanges++;
    }

    LogEvent(_, LogType_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Applied %d attribute(s).", numChanges);
    return numChanges;
}

/**
 * Restores a player's cache to the original values (from modified cache). A
 * attribute set is used as a mask to determine what attributes to restore.
 *
 * @param client        The client index.
 * @param classindex    Index of class to restore.
 * @param attributes    Attribute mask (ClassEditableAttributes).
 * @return              Number of attributes changed.
 */
int VolClassEditRestoreAttributes(int client, int classindex, const ClassEditableAttributes attributes)
{
    int numChanges;

    // Alpha initial.
    if (attributes.ClassEdit_AlphaInitial > -1)
    {
        ClassPlayerCache[client].Class_AlphaInitial = ClassGetAlphaInitial(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Alpha damaged.
    if (attributes.ClassEdit_AlphaDamaged > -1)
    {
        ClassPlayerCache[client].Class_AlphaDamaged = ClassGetAlphaDamaged(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Alpha damage.
    if (attributes.ClassEdit_AlphaDamage > -1)
    {
        ClassPlayerCache[client].Class_AlphaDamage = ClassGetAlphaDamage(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Overlay path.
    if (strcmp(attributes.ClassEdit_OverlayPath, "nochange", false) != 0)
    {
        char path[PLATFORM_MAX_PATH];
        ClassGetOverlayPath(classindex, path, sizeof(path), ZR_CLASS_CACHE_MODIFIED);
        strcopy(ClassPlayerCache[client].Class_OverlayPath, PLATFORM_MAX_PATH, path);
        numChanges++;
    }

    // Nvgs.
    if (attributes.ClassEdit_Nvgs > -1)
    {
        ClassPlayerCache[client].Class_Nvgs = ClassGetNvgs(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Napalm time.
    if (attributes.ClassEdit_NapalmTime > -1.0)
    {
        ClassPlayerCache[client].Class_NapalmTime = ClassGetNapalmTime(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Immunity mode.
    if (attributes.ClassEdit_ImmunityMode != Immunity_Invalid)
    {
        ClassPlayerCache[client].Class_ImmunityMode = ClassGetImmunityMode(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Immunity amount.
    if (attributes.ClassEdit_ImmunityAmount > -1.0)
    {
        ClassPlayerCache[client].Class_ImmunityAmount = ClassGetImmunityAmount(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // No fall damage.
    if (attributes.ClassEdit_NoFallDamage > -1)
    {
        ClassPlayerCache[client].Class_NoFallDamage = ClassGetNoFallDamage(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Health regen interval.
    if (attributes.ClassEdit_RegenInterval > -1.0)
    {
        ClassPlayerCache[client].Class_HealthRegenInterval = ClassGetHealthRegenInterval(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Health regen amount.
    if (attributes.ClassEdit_RegenAmount > -1)
    {
        ClassPlayerCache[client].Class_HealthRegenAmount = ClassGetHealthRegenAmount(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Infect gain.
    if (attributes.ClassEdit_InfectGain > -1)
    {
        ClassPlayerCache[client].Class_HealthInfectGain = ClassGetHealthInfectGain(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Kill bonus.
    if (attributes.ClassEdit_KillBonus > -1)
    {
        ClassPlayerCache[client].Class_KillBonus = ClassGetKillBonus(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Speed.
    if (attributes.ClassEdit_Speed > -1.0)
    {
        ClassPlayerCache[client].Class_Speed = ClassGetSpeed(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Knock back.
    if (attributes.ClassEdit_KnockBack > ZR_CLASS_KNOCKBACK_IGNORE)
    {
        ClassPlayerCache[client].Class_KnockBack = ClassGetKnockback(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Jump height.
    if (attributes.ClassEdit_JumpHeight > -1.0)
    {
        ClassPlayerCache[client].Class_JumpHeight = ClassGetJumpHeight(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    // Jump distance.
    if (attributes.ClassEdit_JumpDistance > -1.0)
    {
        ClassPlayerCache[client].Class_JumpDistance = ClassGetJumpDistance(classindex, ZR_CLASS_CACHE_MODIFIED);
        numChanges++;
    }

    LogEvent(_, LogType_Normal, LOG_DEBUG, LogModule_Volfeatures, "ClassEdit", "Applied %d attribute(s).", numChanges);
    return numChanges;
}
