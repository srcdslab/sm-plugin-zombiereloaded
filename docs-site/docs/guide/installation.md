# Installation

## Requirements

<p>Zombie: Reloaded requires that the following stuff is installed on the server:</p>

<ol> <li><p>Metamod:Source (version 1.8 or newer) - A simplified API for server plugins. It makes it easier for developers to make plugins like SourceMod:</p> <ul><li><a href="http://wiki.alliedmods.net/Installing_Metamod:Source">Installing Metamod:Source</a></li></ul></li> <li><p>SourceMod (version 1.3 or newer) - A scripting platform:</p> <ul> <li><a href="http://wiki.alliedmods.net/Installing_SourceMod">Installing SourceMod</a></li> <li><a href="http://wiki.alliedmods.net/index.php/Category:SourceMod_Documentation">SourceMod Documentation</a></li> </ul></li> <li><p><a href="http://forums.alliedmods.net/showthread.php?t=106748">SDK Hooks Extension</a> (version 1.3 or newer).</p></li> </ol>

## Plugin Installation

<p>Extract the content of the zip file into "cstrike" on the server. The folder and file structure is already set up correctly and ready to be extracted into "cstrike".</p>

## Test Run

<p>The plugin should work with default configuration. Start the server and join a team. Once the round starts there sould be some messages at the chat with "[ZR]". Or type "!zmenu" in the chat to bring up the zombie menu to confirm that the plugin is running.</p>

<p>Next check error logs from SourceMod and look if there are any entries from "zombiereloaded.smx". If the plugin doesn't work at all or there are errors logged, see <a href="../troubleshooting/">Troubleshooting (5)</a>.</p>

## Upgrading

<p>When upgrading there might be changes to configuration files. They must either be reconfigured or merged with old settings.</p>

<p>Reloading Zombie:Reloaded with SourceMod is not recommended. Changing the map works in most cases, but some console variables might not be updated. Do a server restart for a complete refresh of console variables.</p>
