/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          soundeffects/cvars.inc
 *  Type:          Module Component
 *  Description:   Console variables for Sound Effects.
 *
 *  Copyright (C) 2009-2015  Greyscale, Richard Helgeby
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

Handle SEffectsCvar_Voice;
//Handle SEffectsCvar_VoiceZombiesMute;
Handle SEffectsCvar_Moan;
Handle SEffectsCvar_Groan;
Handle SEffectsCvar_Death;
Handle SEffectsCvar_CommandLimit;
Handle SEffectsCvar_CommandTimespan;
Handle SEffectsCvar_PathMoan;
Handle SEffectsCvar_PathPain;
Handle SEffectsCvar_PathDie;
Handle SEffectsCvar_PathScream;
Handle SEffectsCvar_PathCountdown;
Handle SEffectsCvar_MoanMax;
Handle SEffectsCvar_PainMax;
Handle SEffectsCvar_DieMax;
Handle SEffectsCvar_ScreamMax;
Handle SEffectsCvar_CountdownMax;
Handle SEffectsCvar_Volume;
Handle SEffectsCvar_AmbientSounds;
Handle SEffectsCvar_AmbientSoundsFile;
Handle SEffectsCvar_AmbientSoundsLength;
Handle SEffectsCvar_AmbientSoundsVolume;

void SEffects_OnCvarsCreate()
{
    SEffectsCvar_Voice                  = CreateConVar("zr_voice",                      "0",                                    "(Incomplete) Modify sv_alltalk to obey zombie/human teams instead of t/ct.");
    //SEffectsCvar_VoiceZombiesMute       = CreateConVar("zr_voice_zombies_mute",         "0",                                    "(Incomplete) Only allow humans to communicate, block verbal zombie communication.");

    SEffectsCvar_Moan                   = CreateConVar("zr_seffects_moan",              "30.0",                                 "Time between emission of a moan sound from a zombie.");
    SEffectsCvar_Groan                  = CreateConVar("zr_seffects_groan",             "5",                                    "The probability that a groan sound will be emitted from a zombie when shot. ['100' = 1% chance | '50' = 2% chance | '1' = 100% chance]");
    SEffectsCvar_Death                  = CreateConVar("zr_seffects_death",             "1",                                    "Emit a death sound when a zombie dies.");
    SEffectsCvar_CommandLimit           = CreateConVar("zr_seffects_command_limit",     "4",                                    "Number of sound commands allowed within the time span, or total limit if time span is disabled. ['0' = Disable sound command limit]");
    SEffectsCvar_CommandTimespan        = CreateConVar("zr_seffects_command_timespan",  "10",                                   "Time span for sound command limiter (in seconds). ['0' = Disable time span check | positive number = Time span]");
    SEffectsCvar_PathMoan               = CreateConVar("zr_seffects_path_moan",         "zr/moan/zombie_voice_idle%d.mp3",   "Sound, relative to \"sound\" folder, to play from player when moaning.");
    SEffectsCvar_PathPain               = CreateConVar("zr_seffects_path_pain",         "zr/pain/zombie_pain%d.mp3",         "Sound, relative to \"sound\" folder, to play from player when hurt.");
    SEffectsCvar_PathDie                = CreateConVar("zr_seffects_path_die",          "zr/die/zombie_die%d.mp3",          "Sound, relative to \"sound\" folder, to play from player when killed.");
    SEffectsCvar_PathScream             = CreateConVar("zr_seffects_path_scream",       "zr/scream/zombie_scream%d.mp3",       "Sound, relative to \"sound\" folder, to play from zombie when infects a human.");
    SEffectsCvar_PathCountdown          = CreateConVar("zr_seffects_path_countdown",    "zr/countdown/%i.mp3",                  "Sound, relative to \"sound\" folder, to play from player when mother zombie countdown starts.");

    SEffectsCvar_MoanMax                = CreateConVar("zr_seffects_moan_max",          "14",                                   "Max moaning sound file index.");
    SEffectsCvar_PainMax                = CreateConVar("zr_seffects_pain_max",          "6",                                    "Max groan sound file index.");
    SEffectsCvar_DieMax                 = CreateConVar("zr_seffects_die_max",           "3",                                    "Max death sound file index.");
    SEffectsCvar_ScreamMax              = CreateConVar("zr_seffects_scream_max",        "1",                                    "Max scream sound file index.");
    SEffectsCvar_CountdownMax           = CreateConVar("zr_seffects_countdown_max",     "10",                                   "Max countdown sound file index.");
    SEffectsCvar_Volume                 = CreateConVar("zr_seffects_volume",            "0.8",                                  "Default volume of the seffects sounds. [1.0 = Max volume | 0.0001 = Not audible]");

    SEffectsCvar_AmbientSounds          = CreateConVar("zr_ambientsounds",              "1",                                    "Play an ambient sound to all players during gameplay.");
    SEffectsCvar_AmbientSoundsFile      = CreateConVar("zr_ambientsounds_file",         "zr/ambient/zr_ambience.mp3",           "Sound file, relative to \"sound\" folder, to play as ambience.  This file is automatically downloaded to clients. [Dependency: zr_ambientsounds]");
    SEffectsCvar_AmbientSoundsLength    = CreateConVar("zr_ambientsounds_length",       "60.0",                                 "Length of the ambient sound. [Dependency: zr_ambientsounds]");
    SEffectsCvar_AmbientSoundsVolume    = CreateConVar("zr_ambientsounds_volume",       "0.8",                                  "Default volume of the ambient sound. [1.0 = Max volume | 0.0001 = Not audible | Dependency: zr_ambientsounds]");
}

bool SEffects_GetVoice()
{
    return GetConVarBool(SEffectsCvar_Voice);
}

/* INCOMPLETE
bool SEffects_GetVoiceZombiesMute()
{
    return GetConVarBool(SEffectsCvar_VoiceZombiesMute);
}
*/

float SEffects_GetMoanTime()
{
    return GetConVarFloat(SEffectsCvar_Moan);
}

int SEffects_GetGroanProbability()
{
    return GetConVarInt(SEffectsCvar_Groan);
}

bool SEffects_GetDeathEmission()
{
    return GetConVarBool(SEffectsCvar_Death);
}

int SEffects_GetCommandLimit()
{
    return GetConVarInt(SEffectsCvar_CommandLimit);
}

float SEffects_GetCommandTimespan()
{
    return GetConVarFloat(SEffectsCvar_CommandTimespan);
}

void SEffects_GetPath(char[] sound, ZombieSounds soundtype)
{
    switch(soundtype)
    {
        case Moan:
            GetConVarString(SEffectsCvar_PathMoan, sound, SOUND_MAX_PATH);
        case Groan:
            GetConVarString(SEffectsCvar_PathPain, sound, SOUND_MAX_PATH);
        case Death:
            GetConVarString(SEffectsCvar_PathDie, sound, SOUND_MAX_PATH);
        case Scream:
            GetConVarString(SEffectsCvar_PathScream, sound, SOUND_MAX_PATH);
        case Countdown:
            GetConVarString(SEffectsCvar_PathCountdown, sound, SOUND_MAX_PATH)
    }
}

int SEffects_GetMax(ZombieSounds soundtype)
{
    switch(soundtype)
    {
        case Moan:
            return GetConVarInt(SEffectsCvar_MoanMax);
        case Groan:
            return GetConVarInt(SEffectsCvar_PainMax);
        case Death:
            return GetConVarInt(SEffectsCvar_DieMax);
        case Scream:
            return GetConVarInt(SEffectsCvar_ScreamMax);
        case Countdown:
            return GetConVarInt(SEffectsCvar_CountdownMax)
    }

    return 1;
}

float SEffects_GetVolume()
{
    return GetConVarFloat(SEffectsCvar_Volume);
}

bool SEffects_GetAmbientSounds()
{
    return GetConVarBool(SEffectsCvar_AmbientSounds);
}

void SEffects_GetAmbientSoundsFile(char[] sound)
{
    GetConVarString(SEffectsCvar_AmbientSoundsFile, sound, SOUND_MAX_PATH);
}

float SEffects_GetAmbientSoundsLength()
{
    return GetConVarFloat(SEffectsCvar_AmbientSoundsLength);
}

float SEffects_GetAmbientSoundsVolume()
{
    return GetConVarFloat(SEffectsCvar_AmbientSoundsVolume);
}

