# Zombie:Reloaded

Infection / survival style gameplay for SourceMod (Counter-Strike: Source).

Humans must survive against a growing horde of zombies. One or more *mother
zombies* are picked shortly after the round starts; every human they knife
becomes a zombie. Humans win by surviving the round, zombies win by infecting
everyone.

## Documentation map

| Page | What's in it |
| --- | --- |
| [Getting started](getting-started.md) | Install, requirements, first run |
| [ConVars](reference/convars.md) | **Every** `zr_*` console variable, its default and range - generated from the source on every push |
| [Commands](reference/commands.md) | Player and admin commands - generated from the source |
| [Legacy manual](manual/index.html) | The full hand-written user manual (models, classes, volumes, logging...) |

## Why the reference pages are trustworthy

The **ConVars** and **Commands** pages are not maintained by hand. A CI job runs
[`docs-site/gen/generate_reference.py`](https://github.com/srcdslab/sm-plugin-zombiereloaded/blob/master/docs-site/gen/generate_reference.py)
which reads every `CreateConVar(...)` / `Reg*Cmd(...)` call in the plugin and
rebuilds the tables. If a cvar is added, removed, or its default/description
changes, the docs update automatically on the next merge to `master`.
