#!/usr/bin/env python3
"""
Generate the Zombie:Reloaded reference documentation (ConVars and commands)
straight from the plugin source tree.

The output is a set of Markdown files consumed by MkDocs. Running this on every
push keeps the online docs in sync with the code with zero manual work.

Usage:
    python docs-site/gen/generate_reference.py [--repo-root PATH] [--out PATH]
"""

from __future__ import annotations

import argparse
import datetime as _dt
import re
import sys
from pathlib import Path

SCRIPTING_SUBDIR = Path("src/addons/sourcemod/scripting")

# CreateConVar("name", "default", "description" [, flags [, hasMin, min [, hasMax, max]]])
# Every call in the code base currently fits on a single line.
_CREATE_CONVAR_RE = re.compile(
    r"""CreateConVar\(\s*
        "(?P<name>[^"]+)"\s*,\s*
        "(?P<default>[^"]*)"\s*,\s*
        "(?P<description>(?:[^"\\]|\\.)*)"
        (?P<rest>[^;]*)
        \)\s*;""",
    re.VERBOSE,
)

_MINMAX_RE = re.compile(
    r",\s*(?P<hasmin>true|false)\s*,\s*(?P<min>-?[0-9.]+)"
    r"(?:\s*,\s*(?P<hasmax>true|false)\s*,\s*(?P<max>-?[0-9.]+))?"
)

# RegConsoleCmd("name", Handler, "help") / RegAdminCmd("name", Handler, ADMFLAG_x, "help")
_REG_CMD_RE = re.compile(
    r"""Reg(?P<kind>Console|Admin)Cmd\(\s*
        "(?P<name>[^"]+)"\s*,\s*
        [A-Za-z0-9_]+\s*
        (?:,\s*(?P<flags>[A-Za-z0-9_|]+)\s*)?
        (?:,\s*"(?P<help>(?:[^"\\]|\\.)*)")?
    """,
    re.VERBOSE,
)


class ConVar:
    __slots__ = ("name", "default", "description", "has_min", "min", "has_max", "max", "source")

    def __init__(self, name, default, description, source):
        self.name = name
        self.default = default
        self.description = description.replace('\\"', '"').strip()
        self.has_min = self.has_max = False
        self.min = self.max = None
        self.source = source


def _unescape(text: str) -> str:
    return text.replace('\\"', '"').replace("\\n", " ").strip()


def parse_convars(root: Path) -> list[ConVar]:
    scripting = root / SCRIPTING_SUBDIR
    found: dict[str, ConVar] = {}
    for path in sorted(scripting.rglob("*.inc")):
        rel = path.relative_to(scripting).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        for m in _CREATE_CONVAR_RE.finditer(text):
            name = m.group("name")
            if not name.startswith(("zr_", "gs_")):
                continue
            cv = ConVar(name, m.group("default"), m.group("description"), rel)
            mm = _MINMAX_RE.search(m.group("rest") or "")
            if mm:
                cv.has_min = mm.group("hasmin") == "true"
                cv.min = mm.group("min")
                if mm.group("hasmax"):
                    cv.has_max = mm.group("hasmax") == "true"
                    cv.max = mm.group("max")
            found[name] = cv
    return sorted(found.values(), key=lambda c: c.name)


class Command:
    __slots__ = ("name", "kind", "flags", "help", "source")

    def __init__(self, name, kind, flags, help_text, source):
        self.name = name
        self.kind = kind
        self.flags = flags or ""
        self.help = _unescape(help_text or "")
        self.source = source


def parse_commands(root: Path) -> list[Command]:
    scripting = root / SCRIPTING_SUBDIR
    found: dict[str, Command] = {}
    for path in sorted(scripting.rglob("*.inc")):
        rel = path.relative_to(scripting).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        for m in _REG_CMD_RE.finditer(text):
            name = m.group("name")
            found[name] = Command(name, m.group("kind"), m.group("flags"), m.group("help"), rel)
    return sorted(found.values(), key=lambda c: c.name)


def read_version(root: Path) -> str:
    header = root / SCRIPTING_SUBDIR / "zr" / "hgversion.h.inc"
    if not header.exists():
        return "unknown"
    text = header.read_text(encoding="utf-8", errors="replace")
    parts = {}
    for key in ("ZR_VER_MAJOR", "ZR_VER_MINOR", "ZR_VER_PATCH"):
        mm = re.search(rf'#define\s+{key}\s+"?(\d+)"?', text)
        if mm:
            parts[key] = mm.group(1)
    if len(parts) == 3:
        return f'{parts["ZR_VER_MAJOR"]}.{parts["ZR_VER_MINOR"]}.{parts["ZR_VER_PATCH"]}'
    return "unknown"


def _prefix(name: str) -> str:
    # zr_infect_mzombie_ratio -> "infect"
    bits = name.split("_")
    return bits[1] if len(bits) > 1 else "misc"


def render_convars(convars: list[ConVar], version: str) -> str:
    stamp = _dt.datetime.now(_dt.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    groups: dict[str, list[ConVar]] = {}
    for cv in convars:
        groups.setdefault(_prefix(cv.name), []).append(cv)

    out: list[str] = []
    out.append("# ConVars\n")
    out.append(
        "!!! info\n"
        f"    Auto-generated from the plugin source for **v{version}** on {stamp}.\n"
        "    Do not edit by hand - update `CreateConVar` in the code instead.\n"
    )
    out.append(f"\nThe plugin registers **{len(convars)}** console variables, grouped below by feature.\n")

    out.append("\n## Index\n")
    for group in sorted(groups):
        out.append(f"- [`zr_{group}_*`](#zr_{group}) ({len(groups[group])})")
    out.append("")

    for group in sorted(groups):
        out.append(f'\n## zr_{group} {{: #zr_{group} }}\n')
        out.append("| ConVar | Default | Min / Max | Description |")
        out.append("| --- | --- | --- | --- |")
        for cv in groups[group]:
            rng = ""
            if cv.has_min or cv.has_max:
                lo = cv.min if cv.has_min else "-"
                hi = cv.max if cv.has_max else "-"
                rng = f"{lo} / {hi}"
            desc = cv.description.replace("|", "\\|")
            out.append(f"| `{cv.name}` | `{cv.default}` | {rng} | {desc} |")
        out.append("")
    return "\n".join(out) + "\n"


def render_commands(commands: list[Command], version: str) -> str:
    stamp = _dt.datetime.now(_dt.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    console = [c for c in commands if c.kind == "Console"]
    admin = [c for c in commands if c.kind == "Admin"]
    out: list[str] = []
    out.append("# Commands\n")
    out.append(
        "!!! info\n"
        f"    Auto-generated from the plugin source for **v{version}** on {stamp}.\n"
    )

    def table(rows: list[Command], with_flags: bool) -> None:
        if with_flags:
            out.append("\n| Command | Admin flags | Description |")
            out.append("| --- | --- | --- |")
            for c in rows:
                out.append(f"| `{c.name}` | `{c.flags or '-'}` | {c.help.replace('|', chr(92)+'|')} |")
        else:
            out.append("\n| Command | Description |")
            out.append("| --- | --- |")
            for c in rows:
                out.append(f"| `{c.name}` | {c.help.replace('|', chr(92)+'|')} |")
        out.append("")

    out.append(f"\n## Player commands ({len(console)})\n")
    table(console, with_flags=False)
    out.append(f"\n## Admin commands ({len(admin)})\n")
    table(admin, with_flags=True)
    return "\n".join(out) + "\n"


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--repo-root", type=Path, default=Path(__file__).resolve().parents[2])
    ap.add_argument("--out", type=Path, default=None, help="output docs directory")
    args = ap.parse_args(argv)

    root: Path = args.repo_root
    out: Path = args.out or (root / "docs-site" / "docs" / "reference")
    out.mkdir(parents=True, exist_ok=True)

    version = read_version(root)
    convars = parse_convars(root)
    commands = parse_commands(root)

    if not convars:
        print("error: no ConVars parsed - regex or layout changed", file=sys.stderr)
        return 1

    (out / "convars.md").write_text(render_convars(convars, version), encoding="utf-8")
    (out / "commands.md").write_text(render_commands(commands, version), encoding="utf-8")

    print(f"generated {len(convars)} convars and {len(commands)} commands for v{version} -> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
