# Server Administration Guide

## Adding Custom Content

### Managing Resources

<p>Resources like models, materials and sounds must be made available for download to players on the server. Zombie:Reloaded need a list of what files do be downloaded on the players' client. Currently it's only two lists; models and downloads.</p>

<p><strong>Note:</strong> The ambiecne sound file doesn't need to be listed.</p>

### Adding Models

<p>When adding models, update following modules:</p>

<ul> <li>Place model files on the server in "cstrike/models/player/&lt;...&gt;" and model materials in "cstrike/materials/player/&lt;...&gt;".</li> <li>Add model in <a href="../configuration/#model-configuration">model configuration</a>.</li> <li>Add model material files to <a href="../configuration/#download-list">download list</a>.</li> <li>Update model_path attribute on one or more classes in <a href="../classes/">class configuration</a> to use new models. Either by using a predefined setting for random selection, or by directly specifying a model file.</li> </ul>

## Tuning Knock Back

<p>A guide on tuning knock back. It will help finding the default knock back that is used as base value for all classes.</p>

<ol> <li>Make a new zombie class with normal Counter-Strike: Source settings: <blockquote><table> <tr><td class="parameter">fov</td><td class="code">90</td></tr> <tr><td class="parameter">speed</td><td class="code">300</td></tr> <tr><td class="parameter">knockback</td><td class="code">1.0</td></tr> <tr><td class="parameter">jump_height</td><td class="code">1.0</td></tr> <tr><td class="parameter">jump_distance</td><td class="code">1.0</td></tr> </table></blockquote> </li> <li>Disable hit groups module.</li> <li>Set all weapon knock back multipliers to 1.0 in weapon configuration.</li> <li>Join the game and get someone to help. Use that new zombie class.</li> <li>Open knock back multiplier menu and leave it open: !zadmin &gt; Class Multipliers &gt; Zombies &gt; Knock Back</li> <li>Experiment by increasing or decreasing multiplier to get a good feeling on its sensitivity. Then start testing in an open area to make a good balance between zombies and humans. Also test it while climbing on boxes.</li> <li>Once the balance is good, note the multiplier value. This is the actual value since all other multipliers are 1.0. This value will be used as base knock back and can be set on all zombies. Currently there's no in-game multiplier menu for hit groups, but it can be reloaded after changes are made, whithout restarting the server.</li> <li>Set knock back on other zombie classes in class configuration based on this vlaue, whether they should be stronger or weaker.</li> <li>Enable hit groups and adjust the knock back multiplier per hit group. Note that it's a multiplier and there should be only tiny changes from 1.0.</li> <li>Adjust weapon knock back in weapon configuration. The best is to keep these as close to 1.0 as possible. Knock back also depends on how much damage that's done, so pistols could have increased values, and shot guns can be decreased.</li> </ol>
