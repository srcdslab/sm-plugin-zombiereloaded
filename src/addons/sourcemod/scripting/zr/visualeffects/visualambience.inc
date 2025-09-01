/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          visualambience.inc
 *  Type:          Module
 *  Description:   Fog, light style, sky, sun rendering, etc
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
 * Default sky of current map.
 */
char g_VAmbienceDefaultSky[PLATFORM_MAX_PATH];

/**
 * The map is ending.
 */
void VAmbienceOnMapEnd()
{
    // Reset the default sky cache to empty, to be re-cached for the next map.
    g_VAmbienceDefaultSky[0] = 0;
}

/**
 * Validate cvar data.
 */
void VAmbienceLoad()
{
    // Apply all visual effects now.
    VAmbienceApplyAll();

    // If sky is disabled, then stop.
    bool sky = g_hCvarsList.CVAR_VEFFECTS_SKY.BoolValue;
    if (!sky)
    {
        return;
    }

    char downloadpath[PLATFORM_MAX_PATH];
    char skypath[PLATFORM_MAX_PATH];

    // Get sky path.
    g_hCvarsList.CVAR_VEFFECTS_SKY_PATH.GetString(skypath, sizeof(skypath));

    // Prepend materials/skybox to the path.
    Format(downloadpath, sizeof(downloadpath), "materials/skybox/%s", skypath);

    // Add skybox file to downloads table.
    AddFileToDownloadsTable(downloadpath);
}

/**
 * Hook zr_veffects_* cvar changes.
 *
 * @param unhook    If true, cvars will be unhooked, false to hook cvars.
 */
void VAmbienceCvarsHook(bool unhook = false)
{
    // If unhook is true, then continue.
    if (unhook)
    {
        // Unhook lightstyle cvars.
        g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE.RemoveChangeHook(VAmbienceCvarsHookLightStyle);
        g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE_VALUE.RemoveChangeHook(VAmbienceCvarsHookLightStyle);

        // Unhook sky cvars.
        g_hCvarsList.CVAR_VEFFECTS_SKY.RemoveChangeHook(VAmbienceCvarsHookSky);
        g_hCvarsList.CVAR_VEFFECTS_SKY_PATH.RemoveChangeHook(VAmbienceCvarsHookSky);

        // Unhook sun cvars.
        g_hCvarsList.CVAR_VEFFECTS_SUN_DISABLE.RemoveChangeHook(VAmbienceCvarsHookSunDisable);

        // Unhook fog cvars.
        g_hCvarsList.CVAR_VEFFECTS_FOG.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_OVERRIDE.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_PCOLOR.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_SCOLOR.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_DENSITY.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_STARTDIST.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_ENDDIST.RemoveChangeHook(VAmbienceCvarsHookFog);
        g_hCvarsList.CVAR_VEFFECTS_FOG_FARZ.RemoveChangeHook(VAmbienceCvarsHookFog);

        // Stop after unhooking cvars.
        return;
    }

    // Hook lightstyle cvars.
    g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE.AddChangeHook(VAmbienceCvarsHookLightStyle);
    g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE_VALUE.AddChangeHook(VAmbienceCvarsHookLightStyle);

    // Hook sky cvars.
    g_hCvarsList.CVAR_VEFFECTS_SKY.AddChangeHook(VAmbienceCvarsHookSky);
    g_hCvarsList.CVAR_VEFFECTS_SKY_PATH.AddChangeHook(VAmbienceCvarsHookSky);

    // Hook sun cvars.
    g_hCvarsList.CVAR_VEFFECTS_SUN_DISABLE.AddChangeHook(VAmbienceCvarsHookSunDisable);

    // Hook fog cvars.
    g_hCvarsList.CVAR_VEFFECTS_FOG.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_OVERRIDE.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_PCOLOR.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_SCOLOR.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_DENSITY.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_STARTDIST.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_ENDDIST.AddChangeHook(VAmbienceCvarsHookFog);
    g_hCvarsList.CVAR_VEFFECTS_FOG_FARZ.AddChangeHook(VAmbienceCvarsHookFog);
}

/**
 * Cvar hook callback (zr_veffects_lightstyle, zr_veffects_lightstyle_value)
 * Updated server to cvar values.
 *
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public void VAmbienceCvarsHookLightStyle(ConVar cvar, const char[] oldvalue, const char[] newvalue)
{
    // If lightstyle is disabled, then disable.
    bool lightstyle = g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE.BoolValue;

    // Apply light style.
    VAmbienceApplyLightStyle(!lightstyle);
}

/**
 * Cvar hook callback (zr_veffects_sky, zr_veffects_sky_path)
 * Updated server to cvar values.
 *
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public void VAmbienceCvarsHookSky(Handle cvar, const char[] oldvalue, const char[] newvalue)
{
    // If sky is disabled, then disable.
    bool sky = g_hCvarsList.CVAR_VEFFECTS_SKY.BoolValue;

    // Apply new sky.
    VAmbienceApplySky(!sky);
}

/**
 * Cvar hook callback (zr_veffects_sun_disable)
 * Updated server to cvar values.
 *
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public void VAmbienceCvarsHookSunDisable(Handle cvar, const char[] oldvalue, const char[] newvalue)
{
    // If fog is disabled, then disable.
    bool sun = g_hCvarsList.CVAR_VEFFECTS_SUN_DISABLE.BoolValue;

    // Apply fog.
    VAmbienceApplySunDisable(!sun);
}

/**
 * Cvar hook callback (zr_veffects_fog_*)
 * Updated server to cvar values.
 *
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public void VAmbienceCvarsHookFog(Handle cvar, const char[] oldvalue, const char[] newvalue)
{
    // If fog is disabled, then disable.
    bool fogoverride = g_hCvarsList.CVAR_VEFFECTS_FOG_OVERRIDE.BoolValue;

    // Apply fog.
    VAmbienceApplyFog(fogoverride);
}

/**
 * Apply all cvar values on server.
 */
void VAmbienceApplyAll()
{
    // If lightstyle is disabled, then disable.
    bool lightstyle = g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE.BoolValue;

    // Apply light style.
    VAmbienceApplyLightStyle(!lightstyle);

    // If sky is disabled, then disable.
    bool sky = g_hCvarsList.CVAR_VEFFECTS_SKY.BoolValue;

    // Apply new sky.
    VAmbienceApplySky(!sky);

    // If fog is disabled, then disable.
    bool fogoverride = g_hCvarsList.CVAR_VEFFECTS_FOG_OVERRIDE.BoolValue;

    // Apply fog.
    VAmbienceApplyFog(fogoverride);
}

void VAmbienceApplyLightStyle(bool disable = false)
{
    // If default, then set to normal light style.
    if (disable)
    {
        // Set light style.
        SetLightStyle(0, "n");

        return;
    }

    // Get light value.
    char lightstylevalue[4];
    g_hCvarsList.CVAR_VEFFECTS_LIGHTSTYLE_VALUE.GetString(lightstylevalue, sizeof(lightstylevalue));

    // Set light style.
    SetLightStyle(0, lightstylevalue);
}

void VAmbienceApplySky(bool disable = false)
{
    // If we can't find the sv_skyname cvar, then stop.
    ConVar hSkyname = FindConVar("sv_skyname");
    if (hSkyname == null)
    {
        return;
    }

    // Store map's default sky before applying new one.
    if (!g_VAmbienceDefaultSky[0])
    {
        hSkyname.GetString(g_VAmbienceDefaultSky, sizeof(g_VAmbienceDefaultSky));
    }

    // If default, then set to default sky.
    if (disable)
    {
        if (g_VAmbienceDefaultSky[0])
        {
            // Set default sky on all clients.
            hSkyname.SetString(g_VAmbienceDefaultSky, true);
        }

        return;
    }

    // Get sky path.
    char skypath[PLATFORM_MAX_PATH];
    g_hCvarsList.CVAR_VEFFECTS_SKY_PATH.GetString(skypath, sizeof(skypath));

    // Set new sky on all clients.
    hSkyname.SetString(skypath, true);
}

void VAmbienceApplySunDisable(bool disable = false)
{
    // Find sun entity.
    int sun = FindEntityByClassname(-1, "env_sun");

    // If sun is invalid, then stop.
    if (sun == -1)
    {
        return;
    }

    // If default, then re-enable sun rendering.
    if (disable)
    {
        // Turn on sun rendering.
        AcceptEntityInput(sun, "TurnOn");

        return;
    }

    // Turn off sun rendering.
    AcceptEntityInput(sun, "TurnOff");
}

void VAmbienceApplyFog(bool override = false)
{
    // If fog is disabled, then stop.
    bool fog = g_hCvarsList.CVAR_VEFFECTS_FOG.BoolValue;

    if (!fog)
    {
        return;
    }

    // Find current fog index
    int fogindex = FindEntityByClassname(-1, "env_fog_controller");

    // If override is enabled, then continue.
    if (override)
    {
        // If there is fog, then continue.
        if (fogindex != -1)
        {
            // Delete fog.
            AcceptEntityInput(fogindex, "Kill");
            fogindex = -1;
        }
    }

    // If there is no fog on the map, create new fog.
    if (fogindex == -1)
    {
        // Create and spawn fog.
        fogindex = CreateEntityByName("env_fog_controller");
        DispatchSpawn(fogindex);
    }

    char fogcolor[16];

    // Set primary fog color.
    g_hCvarsList.CVAR_VEFFECTS_FOG_PCOLOR.GetString(fogcolor, sizeof(fogcolor));
    VAmbienceSetFogColor(fogindex, fogcolor, true);

    // Set secondary fog color.
    g_hCvarsList.CVAR_VEFFECTS_FOG_SCOLOR.GetString(fogcolor, sizeof(fogcolor));
    VAmbienceSetFogColor(fogindex, fogcolor, false);

    // Set fog's density.
    float fogdensity = g_hCvarsList.CVAR_VEFFECTS_FOG_DENSITY.FloatValue;
    VAmbienceSetFogDensity(fogindex, fogdensity);

    // Set fog's start distance.
    int fogstart = g_hCvarsList.CVAR_VEFFECTS_FOG_STARTDIST.IntValue;
    VAmbienceSetFogStartDist(fogindex, fogstart);

    // Set fog's end distance.
    int fogend = g_hCvarsList.CVAR_VEFFECTS_FOG_ENDDIST.IntValue;
    VAmbienceSetFogEndDist(fogindex, fogend);

    // Set fog's far z distance.
    int fogfarz = g_hCvarsList.CVAR_VEFFECTS_FOG_FARZ.IntValue;
    VAmbienceSetFogFarZ(fogindex, fogfarz);
}

/**
 * Set fog's primary or secondary color.
 *
 * @param fogindex  Edict index of the fog to modify.
 * @param color     The rgb color of the fog.
 * @param primary   (Optional) True to set primary, false otherwise.
 */
void VAmbienceSetFogColor(int fogindex, const char[] color, bool primary = true)
{
    // Set primary color.
    if (primary)
    {
        // Set new color.
        SetVariantString(color);
        AcceptEntityInput(fogindex, "SetColor");
    }
    // Set secondary color.
    else
    {
        // Set new color.
        SetVariantString(color);
        AcceptEntityInput(fogindex, "SetColorSecondary");
    }
}

/**
 * Set fog's density.
 *
 * @param fogindex  Edict index of the fog to modify.
 * @param density   The density of the fog.
 */
void VAmbienceSetFogDensity(int fogindex, float density)
{
    // Set density.
    DispatchKeyValueFloat(fogindex, "fogmaxdensity", density);
}

/**
 * Set fog's start distance.
 *
 * @param fogindex      Edict index of the fog to modify.
 * @param startdist     The start distance of the fog.
 */
void VAmbienceSetFogStartDist(int fogindex, int startdist)
{
    // Set start distance.
    SetVariantInt(startdist);
    AcceptEntityInput(fogindex, "SetStartDist");
}

/**
 * Set fog's end distance.
 *
 * @param fogindex      Edict index of the fog to modify.
 * @param enddist       The end distance of the fog.
 */
void VAmbienceSetFogEndDist(int fogindex, int enddist)
{
    // Set end distance.
    SetVariantInt(enddist);
    AcceptEntityInput(fogindex, "SetEndDist");
}

/**
 * Set fog's far z distance.
 *
 * @param fogindex      Edict index of the fog to modify.
 * @param farz          The far z distance of the fog.
 */
void VAmbienceSetFogFarZ(int fogindex, int farz)
{
    // Set far z distance.
    SetVariantInt(farz);
    AcceptEntityInput(fogindex, "SetFarZ");
}
