# Configuration

## Understanding Syntax

<p>In this manual commands or paths may be written in a certain style that tells how to use it.</p>

<p>Note that the style itself is not written, but it's used as an indicator to tell if a parameter is optional or not.</p>

<p>Example syntax of a command:</p>

<blockquote><p><code>zr_class_modify &lt;classname|"zombies"|"humans"|"admins"&gt; &lt;attribute&gt; &lt;value&gt; [is_multiplier]</code></p></blockquote>

<p>Some paths may look like:</p>

<blockquote><p><code>cfg/sourcemod/zombiereloaded/&lt;mapname&gt;.cfg</code></p></blockquote>

<p>How to read syntaxes like this is explained below.</p>

### Optional Parameters

<p>Optional parameters is not required and usually have a default value or action. They are marked with brackets.</p>

<p>Example:</p>

<blockquote><p><code>zr_somecommand [number]</code></p></blockquote>

<p>Usage examples:</p>

<blockquote><p><code>zr_somecommand<br/> zr_somecommand 10</code></p></blockquote>

### Required Parameters

<p>Required parameters must be specified for the command to function. Usually if no parameters are written the command replies with some info about the syntax.</p>

<p>Less than (&lt;) and greather than (&gt;) symbols marks required parameters.</p>

<p>Examples:</p>

<blockquote><p><code>zr_somecommand &lt;number&gt;<br/> zr_infect &lt;target&gt;</code></p></blockquote>

<p>Usage examples:</p>

<blockquote><p><code>zr_somecommand 100<br/> zr_infect "unnamed"</code></p></blockquote>

### Multiple Options In The Same Parameter

<p>Some commands have parameters that support multiple pre defined options. Usually it's specifying a name, a index or a predefined value. Each option is separated by a "|" symbol. Only one of the options listed is used in a command.</p>

<p>Example:</p>

<blockquote><p><code>zr_do_something &lt;index|name|"all"&gt;</code></p></blockquote>

<p>Usage examples:</p>

<blockquote><p><code>zr_do_something 1<br/> zr_do_something "all"<br/> zr_do_something "unnamed"</code></p></blockquote>

### Text As Parameters

<p>Since parameters are separated by spaces, text (strings) should be quoted. It's a good habit to do this on all string parameters to avoid parsing mistakes.</p>

<p>Example:</p>

<blockquote><p><code>zr_infect "unnamed" "player"<br/> zr_do_something "example text. test."</code></p></blockquote>

<p>Example of bad usage:</p>

<blockquote><p><code>zr_do_something example text. test.</code></p></blockquote>

<p>The last example actually have 3 parameters while it should be only one. This may cause unexpected results and is an example of why strings should be quoted.</p>

## Configuration Files

<p>These are the default configuration files. The paths are relative to the "cstrike" folder.</p>

<blockquote><table> <caption>Default Configuration Files</caption> <tr> <th class="mediumwidth">Type:</th> <th>File:</th> </tr> <tr> <td>Main conf.</td> <td class="code">cfg/sourcemod/zombiereloaded/zombiereloaded.cfg</td> </tr> <tr> <td><a href="#custom-map-configuration-files">Map config.</a></td> <td class="code">cfg/sourcemod/zombiereloaded/&lt;mapname&gt;.cfg</td> </tr> <tr> <td><a href="#custom-map-configuration-files">Post map conf.</a></td> <td class="code">cfg/sourcemod/zombiereloaded/&lt;mapname&gt;.post.cfg</td> </tr> <tr> <td><a href="../hitgroups/">Hitgroup conf.</a></td> <td class="code">addons/sourcemod/configs/zr/hitgroups.txt</td> </tr> <tr> <td><a href="../classes/">Class conf.</a></td> <td class="code">addons/sourcemod/configs/zr/playerclasses.txt</td> </tr> <tr> <td><a href="../weapons/">Weapon conf.</a></td> <td class="code">addons/sourcemod/configs/zr/weapons.txt</td> </tr> <tr> <td><a href="#download-list">Download list</a></td> <td class="code">addons/sourcemod/configs/zr/downloads.txt</td> </tr> <tr> <td><a href="#model-configuration">Model list</a></td> <td class="code">addons/sourcemod/configs/zr/models.txt</td> </tr> </table></blockquote>

<p>The post map configuration file is executed after all features of ZR is done loading. Commands that modify loaded data must be put in post configuration files. How to configure each file is explained in their own sections.</p>

<p>Configuration files also can be reloaded in-game when testing or tuning stuff.</p>

<blockquote><table> <caption>Configuration File Console Commands</caption> <tr> <th>Syntax:</th> </tr> <tr> <td class="commandheader">zr_config_reload &lt;file alias&gt;</td> </tr> <tr> <td class="indent"> <p>Reloads a config file.</p> <p>Parameters:</p> <blockquote><table> <tr> <td class="parameter">file alias</td> <td>The module to reload. Available options: <ul> <li><code>models</code></li> <li><code>downloads</code></li> <li><code>classes</code></li> <li><code>weapons</code></li> <li><code>hitgroups</code></li> </ul> </td> </tr> </table></blockquote> </td> </tr> <tr> <td class="commandheader">zr_config_reloadall</td> </tr> <tr> <td class="indent"><p>Reloads all config files.</p></td> </tr> </table></blockquote>

## Logging

<p>The log system used in Zombie:Reloaded is pretty powerful and customizable. It's based on logging flags and a module filter.</p>

<p>The flags tell what kind of events to log. Those flags are stored as a numeric value in a bit field where each bit tell wether to log a certain event or not. See <a href="http://en.wikipedia.org/wiki/Bit_field">Bit field</a> for technical details.</p>

<p>The module filter is a list of modules to enable log events from. This filter gives extra control of what stuff to log. Use console commands below in the main configuration file to add or remove modules to the filter.</p>

<p>These commands might not work properly until <a href="http://bugs.alliedmods.net/show_bug.cgi?id=3828">bug 3828</a> in SourceMod is fixed. It's a bug where console commands in plugin configuration files are executed late so a command like zr_log_add_module is too late.</p>

<p>There are console variables for different log settings and exceptions. Place them in the main configuration file.</p>

<blockquote><table> <caption>Log Console variables</caption> <tr> <th class="namewidth">Console variable:</th> <th>Default:</th> </tr> <tr> <td class="commandheader">zr_log</td> <td class="commandheader">"1"</td> </tr> <tr> <td class="indent" colspan="2"> <p>Enable logging of events in the plugin. Fatal errors or errors are independendt on this setting and always logged.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> <tr> <td class="commandheader">zr_log_flags</td> <td class="commandheader">"2"</td> </tr> <tr> <td class="indent" colspan="2"> <p>A bit field that specify what event types to log.</p> <p>Options:<br/> Number - See <a href="#log-flags">Log Flags (3.3.1)</a></p> </td> </tr> <tr> <td class="commandheader">zr_log_module_filter</td> <td class="commandheader">"0"</td> </tr> <tr> <td class="indent" colspan="2"> <p>Enable module filtering. Only log events from listed modules will be logged. Use console commands below to add or remove modules from the filter.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> <tr> <td class="commandheader">zr_log_ignore_console</td> <td class="commandheader">"1"</td> </tr> <tr> <td class="indent" colspan="2"> <p>Don't log events triggered by console that are executed by the console itself, like commands in configs. Enable this command to avoid spamming logs with events like weapon restrictions.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> <tr> <td class="commandheader">zr_log_error_override </td> <td class="commandheader">"1"</td> </tr> <tr> <td class="indent" colspan="2"> <p>Always log error messages no matter what logging flags or modules filters that are enabled.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> <tr> <td class="commandheader">zr_log_print_admins</td> <td class="commandheader">"0"</td> </tr> <tr> <td class="indent" colspan="2"> <p>Print log events to admin chat in addition to the log file.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> <tr> <td class="commandheader">zr_log_print_chat</td> <td class="commandheader">"0"</td> </tr> <tr> <td class="indent" colspan="2"> <p>Print log events to public chat in addition to the log file.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> </table></blockquote>

<blockquote><table> <caption>Log Console commands</caption> <tr> <th>Syntax:</th> </tr> <tr> <td class="commandheader">zr_log_add_module &lt;module&gt; [modules...]</td> </tr> <tr> <td class="indent"> <p>Adds one or more modules to the module filter. Use short module names, see <a href="#list-of-modules">List Of Modules (3.3.2)</a>.</p> <p>Parameters:</p> <blockquote><table> <tr><td class="parameter">module</td><td>Name of the module to add.</td></tr> <tr><td class="parameter">modules</td><td>Additional modules to add.</td></tr> </table></blockquote> </td> </tr> <tr> <td class="commandheader">zr_log_remove_module &lt;module&gt; [modules...]</td> </tr> <tr> <td class="indent"> <p>Removes one or more modules from the module filter. Use short module names, see <a href="#list-of-modules">List Of Modules (3.3.2)</a>.</p> <p>Parameters:</p> <blockquote><table> <tr><td class="parameter">module</td><td>Name of the module to remove.</td></tr> <tr><td class="parameter">modules</td><td>Additional modules to remove.</td></tr> </table></blockquote> </td> </tr> <tr> <td class="commandheader">zr_log_list</td> </tr> <tr> <td class="indent"> <p>Lists current log flag settings and module filtering settings.</p> </td> </tr> </table></blockquote>

### Log Flags

<blockquote><table> <caption>Log Flags</caption> <tr> <th class="namewidth">Flag:</th> <th class="tinywidth">Bit No.:</th> <th class="tinywidth">Value:</th> <th>Description:</th> </tr> <tr> <td class="code">LOG_CORE_EVENTS</td> <td>1</td> <td>1</td> <td>Log events from the plugin core like config validation and other messages.</td> </tr> <tr> <td class="code">LOG_GAME_EVENTS</td> <td>2</td> <td>2</td> <td>Log admin commands, console commands, and game related events from modules like, suicide attempts and weapon restrictions.</td> </tr> <tr> <td class="code">LOG_PLAYER_COMMANDS</td> <td>3</td> <td>4</td> <td>Log events that are triggered by players, like chat triggers, teleporting and class changes.</td> </tr> <tr> <td class="code">LOG_DEBUG</td> <td>4</td> <td>8</td> <td>Log debug messages, if any. Usually only developers need to enable this log flag.</td> </tr> <tr> <td class="code">LOG_DEBUG_DETAIL</td> <td>5</td> <td>16</td> <td>Log additional debug messages with more details. May cause spam depending on module filter settings. Usually only developers need to enable this log flag.</td> </tr> </table></blockquote>

<p>To combine several logging flags use the sum of their values. A combination could be "3", which is these log flags:</p>

<blockquote><p><code>LOG_CORE_EVENTS + LOG_GAME_EVENTS<br/> 1 + 2</code></p></blockquote>

<p>Most server setups donesn't need different flag settings. Default is fine.</p>

<p>To decode the value you must convert it from decimals to binary, and count from right to left what bits that are 1. Look up the bit number (not value) in the table above.</p>

<p>As an example on using the number 11 it's 1011 in binary. Counting from right we see that the following bit numbers are on: 1, 2, and 4. That is the flags:</p>

<blockquote><p><code>LOG_CORE_EVENTS + LOG_GAME_EVENTS + LOG_DEBUG<br/> 1 + 2 + 8</code></p></blockquote>

<p>Most operating systems or distributions have a calculator that can convert between binary and decimal numbers with scientific mode enabled. An online unit converter like below can also be used.</p>

<blockquote><p><a href="http://www.unitconversion.org/numbers/decimals-to-binary-conversion.html"> Decimals to binary conversion</a></p></blockquote>

### List Of Modules

<blockquote><table> <caption>List Of Modules</caption> <tr> <th>Short name:</th> <th>Description:</th> </tr> <tr> <td class="valueoption">account</td> <td>Money manager</td> </tr> <tr> <td class="valueoption">antistick</td> <td>Anti-Stick feature</td> </tr> <tr> <td class="valueoption">config</td> <td>Configuration file manager</td> </tr> <tr> <td class="valueoption">cvars</td> <td>Console variables</td> </tr> <tr> <td class="valueoption">damage</td> <td>Damage manager</td> </tr> <tr> <td class="valueoption">downloads</td> <td>File download manager</td> </tr> <tr> <td class="valueoption">hitgroups</td> <td>Hit group feature</td> </tr> <tr> <td class="valueoption">infect</td> <td>Infection manager</td> </tr> <tr> <td class="valueoption">models</td> <td>Model list file manager</td> </tr> <tr> <td class="valueoption">playerclasses</td> <td>Class manager</td> </tr> <tr> <td class="valueoption">veffects</td> <td>Visual effect manager</td> </tr> <tr> <td class="valueoption">seffects</td> <td>Sound effect manager</td> </tr> <tr> <td class="valueoption">tools</td> <td>Helper functions (offsets)</td> </tr> <tr> <td class="valueoption">volfeatures</td> <td>Volumetric features</td> </tr> <tr> <td class="valueoption">weapons</td> <td>Weapon manager</td> </tr> <tr> <td class="valueoption">weaponrestrict</td> <td>Weapon restriction manager</td> </tr> <tr> <td class="valueoption">zspawn</td> <td>Spawn command manager</td> </tr> <tr> <td class="valueoption">ztele</td> <td>Teleport manager</td> </tr> </table></blockquote>

## Custom Map Configuration Files

<p>Configuration files for each map is supported. They're executed after the main configuration files are executed, and are ideal for customizing map settings. These files are just regular configuration files and can also have standard console commands like setting map time. Map configuration files are optional.</p>

<p>The main purpose of these files is to make it possible to change settings for Zombie:Reloaded on certain maps. That could be scaling knockback, restricting certain weapons, changing class attributes or changing ambience sound.</p>

<p>Other map configuration plugins should also work if certain features that doesn't exist in Zombie:Reloaded map configurations is needed.</p>

### Types

<p>There are two kinds of map configs; pre and post. Pre map configuration files are executed before the modules and data is loaded. They're useful for changing configuration sets for certain modules like classes. Post map configuration files are executed after the modules are loaded. Certain stuff have to be placed in this one to be effective, like changing class attributes.</p>

<blockquote><table> <caption>Map Configuration File Types</caption> <tr> <th class="tinywidth">Type:</th> <th class="mediumwidth">Executed:</th> <th>Path:</th> </tr> <tr> <td>Pre</td> <td>Before modules and data loading</td> <td class="code">cfg/sourcemod/zombiereloaded/&lt;mapname&gt;.cfg</td> </tr> <tr> <td>Post</td> <td>After modules</td> <td class="code">cfg/sourcemod/zombiereloaded/&lt;mapname&gt;.post.cfg</td> </tr> </table></blockquote>

<p>If not explicit specified in the module documentation, use pre configuration.</p>

## Model Configuration

<p>The model configuration file is a list of models used on the server stored in Valve's key/value format.</p>

<p>The models listed in this file are also precached when the server starts. Custom models used, but not listed in this file will cause a "model not precached" error on the server, so they must be listed in this file.</p>

<p>In addition models can be restricted to certain groups using the "access" attribute.</p>

<p>List of available model attributes:</p>

<blockquote><table> <caption>Model Attributes</caption> <tr> <th class="namewidth">Attribute:</th> <th class="mediumwidth">Value type:</th> </tr> <tr> <td class="commandheader">name</td> <td class="commandheader">text</td> </tr> <tr> <td class="indent" colspan="2"> <p>File name of model file, without extension.</p> </td> </tr> <tr> <td class="commandheader">path</td> <td class="commandheader">text</td> </tr> <tr> <td class="indent" colspan="2"> <p>Path to model files. <strong>Must</strong> end with "/". Windows servers can use "\" in paths, but they also work with "/".</p> <p>The path is relative to "cstrike".</p> </td> </tr> <tr> <td class="commandheader">team</td> <td class="commandheader">text</td> </tr> <tr> <td class="indent" colspan="2"> <p>What team the model belongs to.</p> <p>Options:</p> <blockquote><table> <tr><td class="valueoption">zombies</td><td>Zombie players. Includes mother zombies.</td></tr> <tr><td class="valueoption">humans</td><td>Human players.</td></tr> </table></blockquote> </td> </tr> <tr> <td class="commandheader">access</td> <td class="commandheader">text</td> </tr> <tr> <td class="indent" colspan="2"> <p>Access mode of the model.</p> <p>Options:</p> <blockquote><table> <tr><td class="valueoption">public</td><td>Everyone can use the model. Included in public random selections.</td></tr> <tr><td class="valueoption">admins</td><td>Model can only be used by admins. Included in public random selections but only applied to admins.</td></tr> <tr><td class="valueoption">hidden</td><td>Model is excluded from public random selections.</td></tr> <tr><td class="valueoption">motherzombies</td><td>Model can only be used by mother zombies.</td></tr> <tr><td class="valueoption">group</td><td>Use group authentication. See "group" attribute.</td></tr> </table></blockquote> </td> </tr> <tr> <td class="commandheader">group</td> <td class="commandheader">text</td> </tr> <tr> <td class="indent" colspan="2"> <p>Name of SourceMod group to use for model authentication if access is "group". If access is anything else than "group" this setting is ignored and can be blank ("").</p> <p>Only players that is a member of this group can use this model. Root admins can't use this model if they're not a member.</p> </td> </tr> </table></blockquote>

<p>For example usages see examples in default model configuration.</p>

<p>Put the list of models in:</p>

<blockquote><p><code>addons/sourcemod/configs/zr/models.txt</code></p></blockquote>

## Download List

<p>Custom materials and overlays must be listed in the download list so clients will download them. Use one line per file, with paths relative to the "cstrike" folder.</p>

<p>List files to be downloaded in the following file:</p>

<blockquote><p><code>addons/sourcemod/configs/zr/downloads.txt</code></p></blockquote>

<p>Look at the default downloads in that file for an example on how to list files to be downloaded.</p>

<p><strong>Note:</strong> The ambience sound file doesn't need to be listed.</p>
