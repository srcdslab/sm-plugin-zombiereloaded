# docs-site

Documentation for Zombie:Reloaded, built with
[MkDocs Material](https://squidfunk.github.io/mkdocs-material/) and published to
GitHub Pages by [`.github/workflows/docs.yml`](../.github/workflows/docs.yml).

## Layout

```
docs-site/
├── mkdocs.yml                 # site config + navigation
├── requirements.txt           # pinned build deps
├── gen/generate_reference.py  # parses the plugin source + shipped configs -> Markdown
└── docs/
    ├── index.md
    ├── getting-started.md
    ├── stylesheets/manual.css # table styling for the guide pages
    ├── guide/                 # the hand-written manual, one page per topic - EDIT THESE
    │   └── *.md
    └── reference/             # GENERATED - do not edit, git-ignored
        ├── convars.md         # from src/**/*.inc CreateConVar(...)
        ├── commands.md        # from src/**/*.inc Reg*Cmd(...)
        ├── classes.md         # from configs/zr/playerclasses*.txt
        ├── weapons.md         # from configs/zr/weapons.txt
        └── hitgroups.md       # from configs/zr/hitgroups.txt
```

## Two kinds of content

- **`docs/guide/*.md`** - the hand-written manual (formerly the single
  `docs/index.html`). Explains how each system works and how to configure it.
  Edit these directly as Markdown.
- **`docs/reference/*.md`** - generated on every build from the plugin source
  (`convars.md`, `commands.md`) and the shipped example configs (`classes.md`,
  `weapons.md`, `hitgroups.md`), git-ignored. Never edit by hand - change the
  code or the config instead.

## Build locally

```bash
pip install -r docs-site/requirements.txt
python docs-site/gen/generate_reference.py
mkdocs serve -f docs-site/mkdocs.yml
```

## How the reference stays in sync

`generate_reference.py` regenerates `docs/reference/*.md` from two sources:

- **Plugin source** - scans `src/addons/sourcemod/scripting/**/*.inc` for
  `CreateConVar(...)` and `RegConsoleCmd/RegAdminCmd(...)` calls. Comments (`//`
  and `/* */`) are stripped first so dead code does not leak in, and constant
  command names (`SAYHOOKS_KEYWORD_*`) are resolved via their `#define`.
- **Shipped configs** - parses the Valve KeyValues files in
  `common/addons/sourcemod/configs/zr/` (`playerclasses*.txt`, `weapons.txt`,
  `hitgroups.txt`) into the "Default …" reference tables.

The CI job runs it on every push to `master`, so any cvar, command or shipped
config change ships with matching docs automatically. The job fails if zero
cvars, zero commands, or an empty config set is parsed, which catches an
accidental change to the declaration or config layout.

On a pull request the `reference-diff` job regenerates the reference for the PR
and its base branch and posts (or updates) a single comment with a `diff` of
what changed, so a cvar/command rename or default change is visible in review
without checking out the branch.
