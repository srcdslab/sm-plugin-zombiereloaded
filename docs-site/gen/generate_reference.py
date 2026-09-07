#!/usr/bin/env python3
"""
Generate the Zombie:Reloaded reference documentation straight from the repo:

* ConVars and commands  - parsed from the plugin source tree
* Default classes / weapons / hit groups - parsed from the shipped
  ``common/addons/sourcemod/configs/zr`` KeyValues files

The output is a set of Markdown files consumed by MkDocs. Running this on every
push keeps the online docs in sync with the code and configs with zero manual
work.

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
CONFIGS_SUBDIR = Path("common/addons/sourcemod/configs/zr")

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
# The command name is a string literal or a #define constant (SAYHOOKS_KEYWORD_*).
_REG_CMD_RE = re.compile(
    r"""Reg(?P<kind>Console|Admin)Cmd\(\s*
        (?P<name>"[^"]+"|[A-Za-z_][A-Za-z0-9_]*)\s*,\s*
        [A-Za-z0-9_]+\s*
        (?:,\s*(?P<flags>[A-Za-z0-9_|]+)\s*)?
        (?:,\s*"(?P<help>(?:[^"\\]|\\.)*)")?
    """,
    re.VERBOSE,
)

# #define SOME_NAME "literal" - used to resolve constant command names.
_DEFINE_STR_RE = re.compile(r'#define\s+(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s+"(?P<value>[^"]*)"')


def strip_comments(text: str) -> str:
    """Remove C/C++ comments while leaving string and char literals intact.

    The plugin has ``CreateConVar`` / ``Reg*Cmd`` calls sitting inside ``//`` and
    ``/* */`` comments (dead code); without this they leak into the reference.
    """
    out: list[str] = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c in ('"', "'"):
            quote = c
            out.append(c)
            i += 1
            while i < n:
                if text[i] == "\\" and i + 1 < n:
                    out.append(text[i])
                    out.append(text[i + 1])
                    i += 2
                    continue
                out.append(text[i])
                if text[i] == quote:
                    i += 1
                    break
                i += 1
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                i += 1
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            i += 2
            while i + 1 < n and not (text[i] == "*" and text[i + 1] == "/"):
                i += 1
            i += 2
            continue
        out.append(c)
        i += 1
    return "".join(out)


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
        text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
        for m in _CREATE_CONVAR_RE.finditer(text):
            name = m.group("name")
            if not name.startswith("zr_"):
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


def parse_defines(root: Path) -> dict[str, str]:
    """Collect ``#define NAME "literal"`` pairs so constant command names resolve."""
    scripting = root / SCRIPTING_SUBDIR
    defines: dict[str, str] = {}
    for path in sorted(scripting.rglob("*.inc")):
        text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
        for m in _DEFINE_STR_RE.finditer(text):
            defines.setdefault(m.group("name"), m.group("value"))
    return defines


def parse_commands(root: Path, defines: dict[str, str]) -> list[Command]:
    scripting = root / SCRIPTING_SUBDIR
    found: dict[str, Command] = {}
    for path in sorted(scripting.rglob("*.inc")):
        rel = path.relative_to(scripting).as_posix()
        text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
        for m in _REG_CMD_RE.finditer(text):
            raw = m.group("name")
            if raw.startswith('"'):
                name = raw.strip('"')
            else:
                name = defines.get(raw)
                if name is None:  # dynamic or unknown constant - can't document it
                    continue
            found[name] = Command(name, m.group("kind"), m.group("flags"), m.group("help"), rel)
    return sorted(found.values(), key=lambda c: c.name)


# ---------------------------------------------------------------------------
# Shipped config files (Valve KeyValues) -> class / weapon / hit group tables
# ---------------------------------------------------------------------------

_KV_TOKEN_RE = re.compile(r'"([^"\n]*)"|([{}])|([^\s{}"]+)')


def _strip_kv_comments(text: str) -> str:
    """Drop ``//`` line comments that sit outside a double-quoted string."""
    out: list[str] = []
    in_str = False
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            in_str = not in_str
            out.append(c)
        elif not in_str and c == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                i += 1
            continue
        else:
            out.append(c)
        i += 1
    return "".join(out)


def _kv_tokens(text: str):
    for m in _KV_TOKEN_RE.finditer(_strip_kv_comments(text)):
        if m.group(2):
            yield ("brace", m.group(2))
        else:
            yield ("str", m.group(1) if m.group(1) is not None else m.group(3))


def _kv_parse(tokens) -> list[tuple[str, object]]:
    """Parse a Valve KeyValues token stream into ordered ``(key, value)`` pairs.

    ``value`` is a string, or a nested list of pairs for a ``{ }`` block.
    Duplicate keys are kept in order.
    """
    result: list[tuple[str, object]] = []
    for kind, val in tokens:
        if kind == "brace":
            if val == "}":
                break
            continue  # stray '{'
        try:
            nkind, nval = next(tokens)
        except StopIteration:
            break
        if nkind == "brace" and nval == "{":
            result.append((val, _kv_parse(tokens)))
        elif nkind == "str":
            result.append((val, nval))
    return result


def load_kv(path: Path) -> list[tuple[str, object]]:
    return _kv_parse(_kv_tokens(path.read_text(encoding="utf-8", errors="replace")))


def _kv_section(pairs, name: str) -> list:
    for key, value in pairs:
        if key.lower() == name.lower() and isinstance(value, list):
            return value
    return []


def _kv_attrs(pairs) -> dict:
    """Flatten a leaf block's string pairs into a dict (last key wins)."""
    return {key: value for key, value in pairs if isinstance(value, str)}


def parse_hitgroups(config_dir: Path) -> list[dict]:
    path = config_dir / "hitgroups.txt"
    if not path.exists():
        return []
    groups = []
    for name, body in _kv_section(load_kv(path), "hitgroups"):
        if not isinstance(body, list):
            continue
        a = _kv_attrs(body)
        groups.append(
            {
                "name": name,
                "index": a.get("index", "-"),
                "damage": a.get("damage", "-"),
                "knockback": a.get("knockback", "-"),
            }
        )
    groups.sort(key=lambda g: int(g["index"]) if g["index"].lstrip("-").isdigit() else 999)
    return groups


def parse_weapons(config_dir: Path) -> list[dict]:
    path = config_dir / "weapons.txt"
    if not path.exists():
        return []
    weapons = []
    for name, body in _kv_section(load_kv(path), "weapons"):
        if not isinstance(body, list):
            continue
        a = _kv_attrs(body)
        cats = [t.strip() for t in a.get("weapontype", "").split(",") if t.strip().lower() not in ("", "all")]
        weapons.append(
            {
                "name": name,
                "category": cats[-1] if cats else "Other",
                "slot": a.get("weaponslot", "-"),
                "knockback": a.get("knockback", "-"),
                "ammoprice": a.get("ammoprice", "-"),
                "restrictdefault": a.get("restrictdefault", "no"),
                "zmarketprice": a.get("zmarketprice", "-"),
                "zmarketpurchasemax": a.get("zmarketpurchasemax", "-"),
                "zmarketcommand": a.get("zmarketcommand", "-"),
            }
        )
    return weapons


def parse_classes(config_dir: Path) -> list[tuple[str, list[dict]]]:
    """Return ``[(filename, [class attrs, ...]), ...]`` for every shipped class file."""
    class_sets: list[tuple[str, list[dict]]] = []
    for filename in ("playerclasses.txt", "playerclasses-nemesis.txt"):
        path = config_dir / filename
        if not path.exists():
            continue
        classes = [
            {"key": key, **_kv_attrs(body)}
            for key, body in _kv_section(load_kv(path), "classes")
            if isinstance(body, list)
        ]
        if classes:
            class_sets.append((filename, classes))
    return class_sets


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
    # ZR registers everything with RegConsoleCmd and checks privileges at runtime,
    # so there is no reliable Console/Admin split to parse. Only add the flags
    # column if some command is actually registered with RegAdminCmd.
    with_flags = any(c.kind == "Admin" for c in commands)

    def esc(text: str) -> str:
        return text.replace("|", "\\|")

    out: list[str] = []
    out.append("# Commands\n")
    out.append(
        "!!! info\n"
        f"    Auto-generated from the plugin source for **v{version}** on {stamp}.\n"
        "    Names without an `sm_` prefix also work as chat triggers (`!name` / `/name`).\n"
    )
    out.append(
        f"\nThe plugin registers **{len(commands)}** commands. Admin-only commands "
        "(infect, class editing, weapon restrictions, ...) enforce their access at "
        "runtime rather than through an admin flag.\n"
    )

    if with_flags:
        out.append("\n| Command | Admin flags | Description |")
        out.append("| --- | --- | --- |")
        for c in commands:
            out.append(f"| `{c.name}` | `{c.flags or '-'}` | {esc(c.help)} |")
    else:
        out.append("\n| Command | Description |")
        out.append("| --- | --- |")
        for c in commands:
            out.append(f"| `{c.name}` | {esc(c.help)} |")
    out.append("")
    return "\n".join(out) + "\n"


def _config_note(source: str, guide: tuple[str, str], version: str, stamp: str) -> str:
    return (
        "!!! info\n"
        f"    Auto-generated from `{source}` on {stamp} (Zombie:Reloaded v{version}).\n"
        f"    These are the shipped defaults - server owners override them in their own"
        f" configs. See [{guide[0]}]({guide[1]}) for what every attribute means.\n"
    )


def _md_cell(value: str) -> str:
    return str(value).replace("|", "\\|")


def _truthy(value: str) -> bool:
    return str(value).strip().lower() in ("yes", "1", "on", "true")


def render_hitgroups(hitgroups: list[dict], version: str) -> str:
    stamp = _dt.datetime.now(_dt.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    out = ["# Default hit groups\n"]
    out.append(_config_note("configs/zr/hitgroups.txt", ("Hit groups & knockback", "../guide/hitgroups.md"), version, stamp))
    out.append(
        f"\nThe shipped `hitgroups.txt` defines **{len(hitgroups)}** hit groups. "
        "`knockback` is a multiplier (`1.0` = no change); turning `damage` off makes "
        "that body part ignore bullet damage while a player is a zombie.\n"
    )
    out.append("\n| Hit group | Index | Damage | Knockback |")
    out.append("| --- | --- | --- | --- |")
    for g in hitgroups:
        out.append(f"| `{g['name']}` | {g['index']} | {g['damage']} | {g['knockback']} |")
    out.append("")
    return "\n".join(out) + "\n"


_WEAPON_CATEGORY_ORDER = [
    "Pistol", "Shotgun", "SMG", "Rifle", "Sniper", "Machine Gun",
    "Melee", "Projectile", "Equipment", "Other",
]


def render_weapons(weapons: list[dict], version: str) -> str:
    stamp = _dt.datetime.now(_dt.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    groups: dict[str, list[dict]] = {}
    for w in weapons:
        groups.setdefault(w["category"], []).append(w)
    categories = [c for c in _WEAPON_CATEGORY_ORDER if c in groups]
    categories += sorted(c for c in groups if c not in _WEAPON_CATEGORY_ORDER)

    out = ["# Default weapons\n"]
    out.append(_config_note("configs/zr/weapons.txt", ("Weapons & ZMarket", "../guide/weapons.md"), version, stamp))
    out.append(
        f"\nThe shipped `weapons.txt` configures **{len(weapons)}** weapons. "
        "`Knockback` is the multiplier applied to a zombie hit by that weapon. "
        "The `ZMarket` columns are `-` for weapons that cannot be bought through ZMarket.\n"
    )
    for category in categories:
        out.append(f"\n## {category}\n")
        out.append("| Weapon | Slot | Knockback | Ammo $ | ZMarket $ | Max / spawn | Restricted by default | Buy command |")
        out.append("| --- | --- | --- | --- | --- | --- | --- | --- |")
        for w in sorted(groups[category], key=lambda x: x["name"].lower()):
            cmd = w["zmarketcommand"]
            cmd = f"`{cmd}`" if cmd and cmd != "-" else "-"
            out.append(
                f"| `{_md_cell(w['name'])}` | {w['slot']} | {w['knockback']} | {w['ammoprice']} | "
                f"{w['zmarketprice']} | {w['zmarketpurchasemax']} | "
                f"{'Yes' if _truthy(w['restrictdefault']) else 'No'} | {cmd} |"
            )
        out.append("")
    return "\n".join(out) + "\n"


_CLASS_TEAM_NAMES = {"0": "Zombie", "1": "Human", "2": "Admin-mode"}


def _class_regen(c: dict) -> str:
    interval = c.get("health_regen_interval", "0")
    try:
        if float(interval) <= 0:
            return "-"
    except ValueError:
        return "-"
    return f"+{c.get('health_regen_amount', '0')} / {interval}s"


def _class_notes(c: dict) -> str:
    notes: list[str] = []
    if not _truthy(c.get("enabled", "yes")):
        notes.append("**disabled**")
    if _truthy(c.get("team_default", "no")):
        notes.append("team default")
    try:
        flags = int(c.get("flags", "0") or "0")
    except ValueError:
        flags = 0
    if flags & 1:
        notes.append("admins only")
    if flags & 2:
        notes.append("mother zombie")
    mode = c.get("immunity_mode", "none").strip().lower()
    if mode and mode != "none":
        notes.append(f"immunity: {mode}")
    if _truthy(c.get("nvgs", "no")):
        notes.append("NVGs")
    if _truthy(c.get("has_napalm", "no")):
        notes.append("napalm")
    if c.get("group", "").strip():
        notes.append(f"group `{c['group'].strip()}`")
    return ", ".join(notes) or "-"


def render_classes(class_sets: list[tuple[str, list[dict]]], version: str) -> str:
    stamp = _dt.datetime.now(_dt.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    total = sum(len(cs) for _, cs in class_sets)
    out = ["# Default player classes\n"]
    out.append(_config_note("configs/zr/playerclasses*.txt", ("Player classes", "../guide/classes.md"), version, stamp))
    out.append(
        f"\nThe shipped class configs define **{total}** classes. `Speed` is the raw "
        "config value - with the default `prop` speed method it is an offset from 250. "
        "`Knockback` and `Jump` apply to zombie classes only.\n"
    )
    for filename, classes in class_sets:
        out.append(f"\n## `{filename}`\n")
        by_team: dict[str, list[dict]] = {}
        for c in classes:
            by_team.setdefault(c.get("team", "0"), []).append(c)
        for team_id in sorted(by_team):
            label = _CLASS_TEAM_NAMES.get(team_id, f"Team {team_id}")
            out.append(f"\n### {label} classes\n")
            out.append("| Class | Description | Health | Speed | Knockback | Jump h / d | HP regen | Notes |")
            out.append("| --- | --- | --- | --- | --- | --- | --- | --- |")
            for c in by_team[team_id]:
                zombie = team_id == "0"
                kb = c.get("knockback", "-") if zombie else "-"
                jump = f"{c.get('jump_height', '-')} / {c.get('jump_distance', '-')}" if zombie else "-"
                out.append(
                    f"| {_md_cell(c.get('name', c['key']))} | {_md_cell(c.get('description', ''))} | "
                    f"{c.get('health', '-')} | {c.get('speed', '-')} | {kb} | {jump} | "
                    f"{_class_regen(c)} | {_class_notes(c)} |"
                )
            out.append("")
    return "\n".join(out) + "\n"


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--repo-root", type=Path, default=Path(__file__).resolve().parents[2])
    ap.add_argument("--out", type=Path, default=None, help="output docs directory")
    args = ap.parse_args(argv)

    root: Path = args.repo_root
    out: Path = args.out or (root / "docs-site" / "docs" / "reference")
    config_dir: Path = root / CONFIGS_SUBDIR
    out.mkdir(parents=True, exist_ok=True)

    version = read_version(root)
    convars = parse_convars(root)
    commands = parse_commands(root, parse_defines(root))
    class_sets = parse_classes(config_dir)
    weapons = parse_weapons(config_dir)
    hitgroups = parse_hitgroups(config_dir)

    if not convars:
        print("error: no ConVars parsed - regex or layout changed", file=sys.stderr)
        return 1

    if not commands:
        print("error: no commands parsed - regex or layout changed", file=sys.stderr)
        return 1

    if not class_sets or not weapons or not hitgroups:
        print(
            f"error: config reference empty - check {CONFIGS_SUBDIR.as_posix()} "
            f"(classes={bool(class_sets)}, weapons={len(weapons)}, hitgroups={len(hitgroups)})",
            file=sys.stderr,
        )
        return 1

    (out / "convars.md").write_text(render_convars(convars, version), encoding="utf-8")
    (out / "commands.md").write_text(render_commands(commands, version), encoding="utf-8")
    (out / "classes.md").write_text(render_classes(class_sets, version), encoding="utf-8")
    (out / "weapons.md").write_text(render_weapons(weapons, version), encoding="utf-8")
    (out / "hitgroups.md").write_text(render_hitgroups(hitgroups, version), encoding="utf-8")

    class_count = sum(len(cs) for _, cs in class_sets)
    print(
        f"generated {len(convars)} convars, {len(commands)} commands, {class_count} classes, "
        f"{len(weapons)} weapons, {len(hitgroups)} hit groups for v{version} -> {out}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
