# Getting started

## Requirements

- SourceMod 1.11 or newer
- [MultiColors](https://github.com/srcdslab/sm-plugin-MultiColors)
- Optional: [AFKManager](https://github.com/srcdslab/sm-plugin-AFKManager),
  [TeamManager](https://github.com/srcdslab/sm-plugin-TeamManager),
  SourceTVManager

## Install

1. Grab the latest build from the
   [Releases](https://github.com/srcdslab/sm-plugin-zombiereloaded/releases) page.
2. Extract the archive into your server's `csgo/` (or `cstrike/`) folder so that
   `addons/`, `cfg/`, `sound/` and `materials/` line up.
3. Restart the map or the server.

## Configuration

ZR reads its ConVars from `cfg/sourcemod/zombiereloaded/zombiereloaded.cfg`
(created on first load) and its data files from `addons/sourcemod/configs/zr/`:

| File | Purpose |
| --- | --- |
| `playerclasses.txt` | Zombie / human / admin class definitions |
| `weapons.txt` | Weapon restrictions, knockback, ZMarket prices |
| `hitgroups.txt` | Per-hitgroup damage and knockback |
| `models.txt` | Player model pool and access |
| `downloads.txt` | Files forced to clients |

Per-map overrides go in `addons/sourcemod/configs/zr/maps/<mapname>.cfg`
(see `zr_config_sm_path`).

For the full list of every setting, see the [ConVars reference](reference/convars.md).
