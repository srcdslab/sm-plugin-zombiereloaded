/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          tools.inc
 *  Type:          Core
 *  Description:   Find offsets and signatures.
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
 * Initialize global offset variables.
 */
int g_iToolsVelocity;
int g_iToolsLMV;
int g_iToolsHasNightVision;
int g_iToolsNightVisionOn;
int g_iToolsFOV;
int g_iToolsLastHitGroup;

/**
 * @endsection
 */

// Tools Functions (core)
#include "tools_functions"

/**
 * Tools module init function.
 */
void ToolsInit()
{
    // Find offsets.
    ToolsFindOffsets();
}

/**
 * Finds all offset values for the plugin.
 */
void ToolsFindOffsets()
{
    // If offset "m_vecAbsVelocity" can't be found, then stop the plugin.
    g_iToolsVelocity = FindDataMapInfo(0, "m_vecAbsVelocity");
    if (g_iToolsVelocity == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CBaseEntity::m_vecAbsVelocity\" was not found.");
    }

    // If offset "m_flLaggedMovementValue" can't be found, then stop the plugin.
    g_iToolsLMV = FindSendPropInfo("CCSPlayer", "m_flLaggedMovementValue");
    if (g_iToolsLMV == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CCSPlayer::m_flLaggedMovementValue\" was not found.");
    }

    // If offset "m_bHasNightVision" can't be found, then stop the plugin.
    g_iToolsHasNightVision = FindSendPropInfo("CCSPlayer", "m_bHasNightVision");
    if (g_iToolsHasNightVision == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CCSPlayer::m_bHasNightVision\" was not found.");
    }

    // If offset "m_bNightVisionOn" can't be found, then stop the plugin.
    g_iToolsNightVisionOn = FindSendPropInfo("CCSPlayer", "m_bNightVisionOn");
    if (g_iToolsNightVisionOn == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CCSPlayer::m_bNightVisionOn\" was not found.");
    }

    // If offset "m_iFOV" can't be found, then stop the plugin.
    g_iToolsFOV = FindSendPropInfo("CBasePlayer", "m_iFOV");
    if (g_iToolsFOV == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CBasePlayer::m_iFOV\" was not found.");
    }
	
    GameData hGameConf = new GameData("zombiereloaded");
    if (hGameConf != null)
    {
        // If offset "m_LastHitGroup" can't be found, then stop the plugin.
        g_iToolsLastHitGroup = hGameConf.GetOffset("m_LastHitGroup");
        if (g_iToolsLastHitGroup == -1)
        {
            LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Gamedata Offset \"CCSPlayer::m_LastHitGroup\" was not found.");
        }

        delete hGameConf;
    }
    else
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Couldn't load zombiereloaded game config!");

    // Forward event to modules.
    WeaponsOnOffsetsFound();
    AccountOnOffsetsFound();
    VEffectsOnOffsetsFound();
    NapalmOnOffsetsFound();
}
