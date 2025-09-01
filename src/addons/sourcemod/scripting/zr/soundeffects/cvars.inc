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

ConVar SEffectsCvar_Voice;
//ConVar SEffectsCvar_VoiceZombiesMute;
ConVar SEffectsCvar_Moan;
ConVar SEffectsCvar_Groan;
ConVar SEffectsCvar_Death;
ConVar SEffectsCvar_CommandLimit;
ConVar SEffectsCvar_CommandTimespan;
ConVar SEffectsCvar_PathMoan;
ConVar SEffectsCvar_PathPain;
ConVar SEffectsCvar_PathDie;
ConVar SEffectsCvar_PathScream;
ConVar SEffectsCvar_PathCountdown;
ConVar SEffectsCvar_MoanMax;
ConVar SEffectsCvar_PainMax;
ConVar SEffectsCvar_DieMax;
ConVar SEffectsCvar_ScreamMax;
ConVar SEffectsCvar_CountdownMax;
ConVar SEffectsCvar_Volume;
ConVar SEffectsCvar_AmbientSounds;
ConVar SEffectsCvar_AmbientSoundsFile;
ConVar SEffectsCvar_AmbientSoundsLength;
ConVar SEffectsCvar_AmbientSoundsVolume;

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
    return SEffectsCvar_Voice.BoolValue;
}

/* INCOMPLETE
bool SEffects_GetVoiceZombiesMute()
{
    return SEffectsCvar_VoiceZombiesMute.BoolValue;
}
*/

float SEffects_GetMoanTime()
{
    return SEffectsCvar_Moan.FloatValue;
}

int SEffects_GetGroanProbability()
{
    return SEffectsCvar_Groan.IntValue;
}

bool SEffects_GetDeathEmission()
{
    return SEffectsCvar_Death.BoolValue;
}

int SEffects_GetCommandLimit()
{
    return SEffectsCvar_CommandLimit.IntValue;
}

float SEffects_GetCommandTimespan()
{
    return SEffectsCvar_CommandTimespan.FloatValue;
}

void SEffects_GetPath(char[] sound, ZombieSounds soundtype)
{
    switch(soundtype)
    {
        case Moan:
            SEffectsCvar_PathMoan.GetString(sound, SOUND_MAX_PATH);
        case Groan:
            SEffectsCvar_PathPain.GetString(sound, SOUND_MAX_PATH);
        case Death:
            SEffectsCvar_PathDie.GetString(sound, SOUND_MAX_PATH);
        case Scream:
            SEffectsCvar_PathScream.GetString(sound, SOUND_MAX_PATH);
        case Countdown:
            SEffectsCvar_PathCountdown.GetString(sound, SOUND_MAX_PATH);
    }
}

int SEffects_GetMax(ZombieSounds soundtype)
{
    switch(soundtype)
    {
        case Moan:
            return SEffectsCvar_MoanMax.IntValue;
        case Groan:
            return SEffectsCvar_PainMax.IntValue;
        case Death:
            return SEffectsCvar_DieMax.IntValue;
        case Scream:
            return SEffectsCvar_ScreamMax.IntValue;
        case Countdown:
            return SEffectsCvar_CountdownMax.IntValue;
    }

    return 1;
}

float SEffects_GetVolume()
{
    return SEffectsCvar_Volume.FloatValue;
}

bool SEffects_GetAmbientSounds()
{
    return SEffectsCvar_AmbientSounds.BoolValue;
}

void SEffects_GetAmbientSoundsFile(char[] sound)
{
    SEffectsCvar_AmbientSoundsFile.GetString(sound, SOUND_MAX_PATH);
}

float SEffects_GetAmbientSoundsLength()
{
    return SEffectsCvar_AmbientSoundsLength.FloatValue;
}

float SEffects_GetAmbientSoundsVolume()
{
    return SEffectsCvar_AmbientSoundsVolume.FloatValue;
}

