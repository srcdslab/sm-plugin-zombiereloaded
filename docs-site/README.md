# docs-site

Modern documentation for Zombie:Reloaded, built with
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
    ├── manual/                # copied from repo-root docs/ at build time
    └── reference/
        ├── convars.md         # GENERATED - do not edit
        └── commands.md        # GENERATED - do not edit
```

## Build locally

```bash
pip install -r docs-site/requirements.txt
python docs-site/gen/generate_reference.py
cp -r docs docs-site/docs/manual        # legacy manual
mkdocs serve -f docs-site/mkdocs.yml
```

## How the reference stays in sync

`generate_reference.py` scans `src/addons/sourcemod/scripting/**/*.inc` for
`CreateConVar(...)` and `RegConsoleCmd/RegAdminCmd(...)` calls and regenerates
`docs/reference/*.md`. The CI job runs it on every push to `master`, so any cvar
change ships with matching docs automatically. The job also fails if zero
cvars are parsed, which catches an accidental change to the cvar declaration
style.
