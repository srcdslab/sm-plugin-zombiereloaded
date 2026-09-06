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
| [Guide](guide/index.md) | The full hand-written manual: configuration file formats, classes, weapons, volumes, admin access, tactics and troubleshooting |
| [ConVars](reference/convars.md) | **Every** `zr_*` console variable, its default and range - generated from the source on every push |
| [Commands](reference/commands.md) | Player and admin commands - generated from the source |

## Two kinds of pages

The **Guide** is written and maintained by hand. It explains *how* each system
works and how to configure it - the depth the reference tables don't carry.

The **ConVars** and **Commands** pages are the opposite: never edited by hand. A
CI job runs
[`docs-site/gen/generate_reference.py`](https://github.com/srcdslab/sm-plugin-zombiereloaded/blob/master/docs-site/gen/generate_reference.py),
which reads every `CreateConVar(...)` / `Reg*Cmd(...)` call in the plugin and
rebuilds the tables, so an added or renamed cvar shows up on the next merge to
`master` with no manual step.
