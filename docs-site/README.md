# docs-site

Documentation for Zombie:Reloaded, built with
[MkDocs Material](https://squidfunk.github.io/mkdocs-material/) and published to
GitHub Pages by [`.github/workflows/docs.yml`](../.github/workflows/docs.yml).

## Layout

```
docs-site/
├── mkdocs.yml                 # site config + navigation
├── requirements.txt           # pinned build deps
├── gen/generate_reference.py  # parses the plugin source -> Markdown
└── docs/
    ├── index.md
    ├── getting-started.md
    ├── stylesheets/manual.css # table styling for the guide pages
    ├── guide/                 # the hand-written manual, one page per topic - EDIT THESE
    │   └── *.md
    └── reference/
        ├── convars.md         # GENERATED - do not edit
        └── commands.md        # GENERATED - do not edit
```

## Two kinds of content

- **`docs/guide/*.md`** - the hand-written manual (formerly the single
  `docs/index.html`). Explains how each system works and how to configure it.
  Edit these directly as Markdown.
- **`docs/reference/{convars,commands}.md`** - generated from the plugin source
  on every build, git-ignored. Never edit by hand.

## Build locally

```bash
pip install -r docs-site/requirements.txt
python docs-site/gen/generate_reference.py
mkdocs serve -f docs-site/mkdocs.yml
```

## How the reference stays in sync

`generate_reference.py` scans `src/addons/sourcemod/scripting/**/*.inc` for
`CreateConVar(...)` and `RegConsoleCmd/RegAdminCmd(...)` calls and regenerates
`docs/reference/*.md`. Comments (`//` and `/* */`) are stripped first so dead
code does not leak in, and constant command names (`SAYHOOKS_KEYWORD_*`) are
resolved via their `#define`. The CI job runs it on every push to `master`, so
any cvar or command change ships with matching docs automatically. The job
fails if zero cvars or zero commands are parsed, which catches an accidental
change to the declaration style.
