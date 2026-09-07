# Admin access

<p>Zombie:Reloaded use SourceMod to authenticate players. There are two ways to do this; flag based or group based. With flag authentication a certain admin flag is required to be set, and with group authentication the player must be member of a certain group to do certain types of operations.</p>

<p>The reason to use group authentication could be to let admins have full access in SourceMod so they get access to configure all plugins, but still limit access to commands in Zombie:Reloaded. When using flag authentication the permissions will be global across all plugins, including Zombie:Reloaded.</p>

## Admin Flags Used In Zombie:Reloaded

<p>Admins must have the appropriate <a href="http://wiki.alliedmods.net/Adding_Admins_(SourceMod)#Levels">SourceMod admin flags</a> to do certain types of operations. The required flag is configurable per operation type through permissions cvars:</p>

<blockquote><table> <caption>Admin Flags Used In Zombie:Reloaded</caption> <tr> <th>Flag cvar (default):</th> <th>Operation type:</th> </tr> <tr> <td class="parameter">zr_permissions_flag_generic (Admin_Ban)</td> <td>Access to generic operations like infecting, teleporting, and spawning players.</td> </tr> <tr> <td class="parameter">zr_permissions_flag_configuration (Admin_Config)</td> <td>Access to operations that change settings in Zombie:Reloaded.</td> </tr> </table></blockquote>

## Predefined Admin Groups

<p>If group authentication is used, admins must be member of one of these <a href="http://wiki.alliedmods.net/Adding_Groups_(SourceMod)">SourceMod groups</a> to do the following operations:</p>

<blockquote><table> <caption>Predefined Admin Groups</caption> <tr> <th>Group:</th> <th>Operation type:</th> </tr> <tr> <td class="valueoption">zr_admins</td> <td>Full access to all commands in Zombie:Reloaded.</td> </tr> <tr> <td class="valueoption">zr_moderators</td> <td>Access to generic operations like infecting, teleporting, and spawning players.</td> </tr> <tr> <td class="valueoption">zr_configurators</td> <td>Access to operations that change settings in Zombie:Reloaded.</td> </tr> </table></blockquote>

## Console Variables

<blockquote><table> <caption>Permission Console Variables</caption> <tr> <th>Console variable:</th> <th>Default:</th> </tr> <tr> <td class="commandheader">zr_permissions_use_groups</td> <td class="commandheader">0</td> </tr> <tr> <td class="indent" colspan="2"> <p>Use group authentication instead of flags to access admin features. Generic admin flag is still required on some features.</p> <p>Options:<br/> 0 or 1</p> </td> </tr> <tr> <td class="commandheader">zr_permissions_flag_generic</td> <td class="commandheader">d</td> </tr> <tr> <td class="indent" colspan="2"> <p>Admin flag used for generic admin operations when group authentication is disabled.</p> <p>Options:<br/> Any single SourceMod admin flag character (a-t or z). Recommended default: d (ban).</p> </td></tr> <tr> <td class="commandheader">zr_permissions_flag_configuration</td> <td class="commandheader">i</td> </tr> <tr> <td class="indent" colspan="2"> <p>Admin flag used for configuration operations when group authentication is disabled.</p> <p>Options:<br/> Any single SourceMod admin flag character (a-t or z). Recommended default: i (config).</p> <p>See SourceMod flag levels:<br/> <a href="https://wiki.alliedmods.net/Adding_Admins_(SourceMod)#Levels">https://wiki.alliedmods.net/Adding_Admins_(SourceMod)#Levels</a></p> </td> </tr> </table></blockquote>
