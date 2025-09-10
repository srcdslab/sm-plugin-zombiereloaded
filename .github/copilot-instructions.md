# Copilot Instructions for Zombie:Reloaded SourceMod Plugin

## Repository Overview

This repository contains **Zombie:Reloaded**, a comprehensive SourcePawn plugin for SourceMod that provides infection/survival gameplay for Source engine games (primarily Counter-Strike). The plugin transforms standard team-based gameplay into a zombie survival experience with extensive customization options.

### Key Features
- **Infection System**: Human vs zombie gameplay with mother zombie selection
- **Class System**: Customizable player classes for zombies and humans  
- **Weapon Management**: Advanced weapon restrictions and custom buy system
- **Visual/Audio Effects**: Comprehensive effects system for immersive gameplay
- **Admin Tools**: Extensive administrative commands and configuration
- **API**: Public API for other plugins to integrate with ZR

## Technical Environment

- **Language**: SourcePawn (SourceMod scripting language)
- **Platform**: SourceMod 1.11+ (requires latest stable release)
- **Compiler**: SourcePawn compiler (spcomp) via SourceKnight build system
- **Target Games**: Source engine games (CS:S, CS:GO, etc.)
- **Build System**: SourceKnight with YAML configuration

### Dependencies
- **SourceMod**: 1.11.0+ (core framework)
- **MultiColors**: Extended color support for chat messages
- **AFKManager**: AFK player management
- **TeamManager**: Team balancing functionality  
- **SourceTVManager**: SourceTV integration support

## Project Structure

```
src/addons/sourcemod/
├── scripting/
│   ├── zombiereloaded.sp          # Main plugin file
│   ├── include/
│   │   ├── zombiereloaded.inc     # Main API include
│   │   └── zr/                    # API module includes
│   ├── zr/                        # Plugin module includes
│   │   ├── global.inc             # Global definitions
│   │   ├── infect.inc             # Infection mechanics
│   │   ├── playerclasses/         # Class system
│   │   ├── weapons/               # Weapon management
│   │   └── [other modules]
│   └── testsuite/                 # Test plugins
├── configs/zr/                    # Configuration files
├── translations/                  # Language files
└── gamedata/                      # Game signatures/offsets
```

### Configuration Files
- `playerclasses.txt` - Player class definitions
- `weapons.txt` - Weapon configuration and restrictions
- `hitgroups.txt` - Damage/knockback hitgroup settings
- `models.txt` - Player model assignments
- `downloads.txt` - Required client downloads

## Code Style & Standards

### SourcePawn Conventions
```sourcepawn
#pragma semicolon 1              // Required
#pragma newdecls required        // Required for new syntax

// Indentation: 4 spaces (use tabs)
// Variable naming:
//   - camelCase for local variables and parameters
//   - PascalCase for function names and global variables  
//   - Prefix globals with "g_"

int g_GlobalVariable;             // Global variable
bool g_ClientData[MAXPLAYERS+1];  // Client arrays

public void FunctionName(int clientId, const char[] message)
{
    int localVariable = 0;
    char localBuffer[256];
}
```

### Memory Management Rules
```sourcepawn
// CRITICAL: Always use delete, never .Clear()
// .Clear() creates memory leaks with StringMap/ArrayList

StringMap playerData = new StringMap();
// Later...
delete playerData;  // Correct - no null check needed
playerData = new StringMap();  // Create new instance

// WRONG: playerData.Clear();  // Memory leak!
```

### Database/SQL Requirements
```sourcepawn
// ALL SQL queries MUST be asynchronous
Database db = SQL_Connect("mydb");

// Use methodmap syntax and prepared statements
char query[] = "SELECT * FROM players WHERE steamid = ?";
DBStatement stmt = SQL_PrepareQuery(db, query);
stmt.BindString(0, steamId);
stmt.ExecuteQuery(OnQueryComplete, clientId);

// Always use transactions for multiple operations
Transaction txn = new Transaction();
txn.AddQuery("INSERT INTO ...", data);
db.Execute(txn, OnTransactionSuccess, OnTransactionError);
```

### Performance Best Practices
```sourcepawn
// Minimize operations in frequently called functions
public void OnGameFrame()
{
    // Avoid heavy operations here
    // Cache expensive calculations
}

// Use timers sparingly
CreateTimer(1.0, Timer_CheckPlayers, _, TIMER_REPEAT);

// Optimize string operations  
char buffer[256];
FormatEx(buffer, sizeof(buffer), "Player: %N", client);  // Better than Format()
```

## Build System

### SourceKnight Configuration
The project uses SourceKnight for automated building:

```yaml
# sourceknight.yaml
project:
  name: zombiereloaded
  dependencies: [sourcemod, multicolors, etc...]
  targets: [zombiereloaded]
```

### Build Commands
```bash
# Build plugin (via GitHub Actions)
# Manual building requires SourceKnight installation
sourceknight build

# Output: src/addons/sourcemod/plugins/zombiereloaded.smx
```

### CI/CD Pipeline
- **GitHub Actions**: Automatic building on push/PR
- **Artifact Creation**: Compiled plugins uploaded as artifacts
- **Release Automation**: Tagged releases create downloadable packages

## API Development

### Public API Structure
```sourcepawn
// Main API include
#include <zombiereloaded>

// Individual modules
#include <zr/infect.zr>      // Infection functions
#include <zr/class.zr>       // Class management  
#include <zr/weapons.zr>     // Weapon functions
#include <zr/ztele.zr>       // Teleport system
```

### Creating New API Functions
```sourcepawn
// In appropriate .inc file:

/**
 * Description of the function.
 * 
 * @param client    Client index.
 * @param value     Value to set.
 * @return          True on success, false otherwise.
 */
native bool ZR_SetPlayerValue(int client, int value);

// In main plugin:
public int Native_SetPlayerValue(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    int value = GetNativeCell(2);
    
    // Validate parameters
    if (!IsValidClient(client))
        return false;
        
    // Implementation
    return true;
}
```

## Configuration Management

### CVars (ConVars)
When adding/modifying/removing CVars:

1. **Update Plugin Code**: Add/modify the ConVar definition
2. **Update Web Docs**: **MANDATORY** - Must update web documentation
3. **Test Functionality**: Verify the CVar works as expected
4. **Document Changes**: Include in commit message and release notes

```sourcepawn
ConVar g_CvarNewFeature;

public void OnPluginStart()
{
    g_CvarNewFeature = CreateConVar("zr_new_feature", "1", 
        "Enable new feature. [0 = Disabled | 1 = Enabled]");
}
```

## Translation System

### Message Localization
```sourcepawn
// Use translation files for all user-facing messages
LoadTranslations("zombiereloaded.phrases");

// In code:
MC_PrintToChat(client, "%t", "Infection Started", playerName);

// In translations/zombiereloaded.phrases.txt:
"Infection Started"
{
    "en"    "Infection has started! {1} is the mother zombie!"
    "de"    "Infektion hat begonnen! {1} ist der Mutter-Zombie!"
}
```

## Testing

### Test Suite Structure
```
testsuite/zr/
├── classapitest.sp      # Class system tests
├── infectapitest.sp     # Infection tests  
├── weaponsapitest.sp    # Weapon tests
└── [other test modules]
```

### Running Tests
```sourcepawn
// Test plugins verify API functionality
// Load alongside main plugin for testing
// Use console commands to trigger tests
```

## Common Development Patterns

### Client Data Management
```sourcepawn
// Global client arrays
bool g_ClientInfected[MAXPLAYERS+1];
int g_ClientClass[MAXPLAYERS+1];

public void OnClientConnected(int client)
{
    // Reset client data
    g_ClientInfected[client] = false;
    g_ClientClass[client] = -1;
}
```

### Event Handling
```sourcepawn
public void OnPluginStart()
{
    // Hook game events
    HookEvent("player_spawn", Event_PlayerSpawn);
    HookEvent("player_death", Event_PlayerDeath);
}

public void Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    // Handle spawn logic
}
```

### Command Registration
```sourcepawn
public void OnPluginStart()
{
    RegConsoleCmd("sm_zmenu", Command_ZMenu, "Opens ZR main menu");
    RegAdminCmd("sm_zr_infect", Command_Infect, ADMFLAG_GENERIC, 
                "Infect a player");
}
```

## Debugging & Development Tools

### Debug Commands
- `zr_log_*` - Logging configuration  
- `zr_class_dump` - Class data inspection
- `zr_vol_*` - Volume debugging
- `zrtest_*` - Test commands for development

### Performance Monitoring
```sourcepawn
// Use SourceMod's built-in profiler
// Monitor timer usage and frequency
// Check for memory leaks with delete operations
// Validate SQL query performance
```

## Documentation Requirements

### Code Documentation
```sourcepawn
/**
 * Function description.
 *
 * @param param1    Parameter description.
 * @param param2    Another parameter.
 * @return          Return value description.
 * @error           Error conditions if any.
 */
public bool SomeFunction(int param1, const char[] param2)
{
    // Implementation with comments for complex logic
}
```

### No Unnecessary Headers
- **DO NOT** create header comments for simple plugins
- **DO** document complex logic sections
- **DO** document all native functions with full parameter/return info
- **DO** update web docs for any CVar changes

## Version Control

### Commit Standards
- Use clear, descriptive commit messages
- Reference issue numbers when applicable
- Keep commits focused on single changes
- Update version numbers in plugin info

### Release Process
- Use semantic versioning (MAJOR.MINOR.PATCH)
- Tag releases in repository
- Automated release creation via GitHub Actions
- Include changelog in release notes

## Common Pitfalls to Avoid

1. **Memory Leaks**: Never use `.Clear()` on StringMap/ArrayList
2. **Synchronous SQL**: All database operations must be async
3. **Missing Validation**: Always validate client indices and parameters
4. **Hardcoded Values**: Use configuration files instead
5. **Missing Translations**: All user messages must be translatable
6. **Performance Issues**: Avoid heavy operations in frequently called functions
7. **Missing Web Docs**: CVar changes require documentation updates

## Integration Points

### External Plugin Integration
- Other plugins can include `<zombiereloaded>` for API access
- Use forwards for event notifications
- Provide natives for external plugins to query ZR state
- Maintain backwards compatibility in API changes

This plugin is a critical component for zombie-themed game servers and requires careful attention to performance, stability, and user experience.