HOOK SCRIPTS
------------

Hook scripts are specially named scripts that are run by sfall at specific points to allow mods to override normally hardcoded behaviour in a more flexible way than sfall's normal ini configuration.

In addition to the bit of code it overrides, the script will be run once when first loaded and again at each player reload to allow for setup. Hook scripts have access to a set of arguments supplied to sfall, but aren't required to use them all. They also return one or more values, but again they're optional, and you only need to return a value if you want to override the default.

### Hooks compatibility

To aid in mods compatibility, avoid using the predefined `hs_<name>.int` scripts. Instead it is recommended to use a normal global script combined with `register_hook_proc` or `register_hook`.

Example setup for a hook-script based mod:
```js
procedure tohit_hook_handler begin
   display_msg("Modifying hit_hook " + get_sfall_arg);
   set_hit_chance_max(100);
   set_sfall_return(100);
end

procedure start begin
   if game_loaded then begin
      register_hook_proc(HOOK_TOHIT, tohit_hook_handler);
   end
end
```

-------------------------------------------

There are script functions available, specific to hook scripts:

#### `int init_hook()`
The hook script equivalent of `game_loaded`; it returns 1 when the script is loaded for the first time or when the player reloads the game, and 0 otherwise.

#### `mixed get_sfall_arg()`
Gets the next argument from sfall. Each time it's called it returns the next argument, or otherwise it returns 0 if there are no more arguments left.
You can arbitrarily get the value of any argument using the `sfall_func1("get_sfall_arg_at", argNum)` function.

#### `array get_sfall_args()`
Returns all hook arguments as a new temp array.

#### `void set_sfall_return(int value)`
Used to return the new values from the script. Each time it's called it sets the next value, or if you've already set all return values it does nothing.

#### `void set_sfall_arg(int argNum, int value)`
Changes argument value. The argument number (`argNum`) is 0-indexed. This is useful if you have several hook scripts attached to one hook point (see below).

#### `void register_hook(int hookID)`
Used from a normal global script if you want to run it at the same point a full hook script would normally run. In case of this function, `start` procedure will be executed in the current global script. You can use all above functions like normal.

#### `void register_hook_proc(int hookID, procedure proc)`
The same as `register_hook`, except that you specifically define which procedure in the current script should be called as a hook (instead of "start" by default). Pass the procedure the same as how you use dialog option functions.
This IS the recommended way to use hook scripts, as it gives both modularity (each mod logic in a separate global script with no conflicts) and flexibility. You can place all related hook scripts for a specific mod in one global script!

Use zero (0) as the second argument to unregister the hook from the current global script.

#### `void register_hook_proc_spec(int hookID, procedure proc)`
Works the same as `register_hook_proc`, except that it registers the current script at the end of the hook script execution chain (i.e. the script will be executed after all previously registered scripts for the same hook, including the `hs_<name>.int` script).
In addition, all scripts hooked to a single hook point with this function are executed in the exact order of how they were registered. In the case of using `register_hook` and `register_hook_proc` functions, scripts are executed in reverse order of how they were registered.

**The execution chain of script procedures for a hook is as follows:**
1. Procedures registered with `register_hook` and `register_hook_proc` functions (executed in reverse order of registration).
2. The `hs_<name>.int` script.
3. Procedures registered with the `register_hook_proc_spec` function (executed in the exact order of registration).

You can hook several scripts to a single hook point, for example, if it's different mods from different authors or just some different aspects of one larger mod.
When one of the scripts in a chain returns value with `set_sfall_return`, the next script may override this value if calls `set_sfall_return` again.

__Example:__ Sometimes you need to multiply certain value in a chain of hook scripts.
Let's say we have a **Mod A** which reduces all "to hit" chances by 50%. The code might look like this:
```js
    original_chance = get_sfall_arg;
    set_sfall_return(original_chance / 2);
```

**Mod B** also want to affect hit chances globally, by increasing them by 50%. Now in order for both mods to work well together, we need to add this line to **Mod A** hook script:
```js
    set_sfall_arg(0, (original_chance / 2));
```

This basically changes hook argument for the next script. **Mod B** code:
```js
    original_chance = get_sfall_arg;
    set_sfall_return(original_chance * 1.5);
    set_sfall_arg(0, (original_chance * 1.5));
```

So if you combine both mods together, they will run in chain and the end result will be a 75% from original hit chance (hook register order doesn't matter in this case, if you use `set_sfall_arg` in both hooks).


The defines to use for the hookID are in **sfall.h**.

-------------------------------------------

## HOOK SCRIPT TYPES

#### `HOOK_TOHIT (hs_tohit.int)`

Runs when Fallout is calculating the chances of an attack striking a target.\
Runs after the hit chance is fully calculated normally, including applying the 95% cap.

```
int     arg0 - The hit chance (capped)
Critter arg1 - The attacker
Critter arg2 - The target of the attack
int     arg3 - The targeted bodypart
int     arg4 - Source tile (may differ from attacker's tile, when AI is considering potential fire position)
int     arg5 - Attack Type (see ATKTYPE_* constants)
int     arg6 - Ranged flag. 1 if the hit chance calculation takes into account the distance to the target. This does not mean the attack is a ranged attack
int     arg7 - The raw hit chance before applying the cap

int     ret0 - the new hit chance
```

-------------------------------------------

#### `HOOK_AFTERHITROLL (hs_afterhitroll.int)`

Runs after Fallout has decided if an attack will hit or miss.

```
int     arg0 - If the attack will hit: 0 - critical miss, 1 - miss, 2 - hit, 3 - critical hit
Critter arg1 - The attacker
Critter arg2 - The target of the attack
int     arg3 - The bodypart
int     arg4 - The hit chance

int     ret0 - Override the hit/miss
int     ret1 - Override the targeted bodypart
Critter ret2 - Override the target of the attack
```

-------------------------------------------

#### `HOOK_CALCAPCOST (hs_calcapcost.int)`

Runs whenever Fallout is calculating the AP cost of using the weapon (or unarmed attack). Doesn't run for using other item types or moving.\
Note that the first time a game is loaded, this script doesn't run before the initial interface is drawn, so if the script effects the AP cost of whatever is in the player's hands at the time the wrong AP cost will be shown. It will be fixed the next time the interface is redrawn.\
You can get the weapon object by checking item slot based on attack type (`ATKTYPE_LWEP1`, `ATKTYPE_LWEP2`, etc) and then calling `critter_inven_obj`.

```
Critter arg0 - The critter performing the action
int     arg1 - Attack Type (see ATKTYPE_* constants)
int     arg2 - Is aimed attack (1 or 0)
int     arg3 - The default AP cost
Item    arg4 - The weapon for which the cost is calculated. If it is 0, the pointer to the weapon can still be obtained by the aforementioned method

int     ret0 - The new AP cost
```

-------------------------------------------

#### `HOOK_DEATHANIM1 (hs_deathanim1.int)`

Runs before Fallout tries to calculate the death animation. Lets you switch out which weapon Fallout sees.

Does not run for critters in the knockdown/out state.

```
int     arg0 - The pid of the weapon performing the attack. (May be -1 if the attack is unarmed)
Critter arg1 - The attacker
Critter arg2 - The target
int     arg3 - The amount of damage
int     arg4 - Unused, always -1. Use this if you are using the same procedure for HOOK_DEATHANIM1 and HOOK_DEATHANIM2 (since sfall 4.1/3.8.24)

int     ret0 - The pid of an object to override the attacking weapon with
```

-------------------------------------------

#### `HOOK_DEATHANIM2 (hs_deathanim2.int)`

Runs after Fallout has calculated the death animation. Lets you set your own custom frame id, so more powerful than `HOOK_DEATHANIM1`, but performs no validation.\
When using `critter_dmg` function, this script will also run. In that case weapon pid will be -1 and attacker will point to an object with `obj_art_fid == 0x20001F5`.

Does not run for critters in the knockdown/out state.

```
int     arg0 - The pid of the weapon performing the attack. (May be -1 if the attack is unarmed)
Critter arg1 - The attacker
Critter arg2 - The target
int     arg3 - The amount of damage
int     arg4 - The death anim id calculated by Fallout

int     ret0 - The death anim id to override with
```

-------------------------------------------

#### `HOOK_COMBATDAMAGE (hs_combatdamage.int)`

Runs when:
1) Game calculates how much damage each target will get. This includes primary target as well as all extras (explosions and bursts). This happens BEFORE the actual attack animation.
2) AI decides whether it is safe to use area attack (burst, grenades), if he might hit friendlies.

Does not run for misses, or non-combat damage like dynamite explosions.

```
Critter arg0  - The target
Critter arg1  - The attacker
int     arg2  - The amount of damage to the target
int     arg3  - The amount of damage to the attacker
int     arg4  - The special effect flags for the target (use bwand DAM_* to check specific flags)
int     arg5  - The special effect flags for the attacker (use bwand DAM_* to check specific flags)
Item    arg6  - The weapon used in the attack
int     arg7  - The bodypart that was struck
int     arg8  - Damage Multiplier (this is divided by 2, so a value of 3 does 1.5x damage, and 8 does 4x damage. Usually it's 2; for critical hits, the value is taken from the critical table; with Silent Death perk and the corresponding attack conditions, the value will be doubled)
int     arg9  - Number of bullets actually hit the target (1 for melee attacks)
int     arg10 - The amount of knockback to the target
int     arg11 - Attack Type (see ATKTYPE_* constants)
mixed   arg12 - computed attack data (see C_ATTACK_* for offsets and use get/set_object_data functions to get/set the data)

int     ret0 - The damage to the target
int     ret1 - The damage to the attacker
int     ret2 - The special effect flags for the target
int     ret3 - The special effect flags for the attacker
int     ret4 - The amount of knockback to the target
```

-------------------------------------------

#### `HOOK_ONDEATH (hs_ondeath.int)`

Runs immediately after a critter dies for any reason. No return values; this is just a convenience for when you need to do something after death for a large number of different critters and don't want to have to script each and every one of them.

```
Critter arg0 - The critter that just died
```

-------------------------------------------

#### `HOOK_FINDTARGET (hs_findtarget.int)`

Runs when the AI is trying to pick a target in combat. Fallout first chooses a list of 4 likely suspects, then normally sorts them in order of weakness/distance/etc depending on the AI caps of the attacker.\
This hook replaces that sorting function, allowing you to sort the targets in some arbitrary way.

The return values can include critters that weren't in the list of possible targets, but the additional targets may still be discarded later on in the combat turn if they are out of the attackers perception or the chance of a successful hit is too low. The list of possible targets often includes duplicated entries, but this is fixed in sfall 4.2.3/3.8.23.
Use `set_sfall_return` to give the 4 targets, in order of preference. If you want to specify less than 4 targets, fill in the extra spaces with 0's or pass -1 to skip the return value.

__NOTE:__ The engine can choose targets by the following criteria:
1) The nearest enemy to the attacker.
2) The enemy that attacked the attacker.
3) The enemy that attacked an NPC from the same team as the attacker.
4) The enemy that is attacked by an NPC from the same team as the attacker.

```
Critter arg0 - The attacker
Critter arg1 - A possible target
Critter arg2 - A possible target
Critter arg3 - A possible target
Critter arg4 - A possible target

Critter ret0 - The first choice of target
Critter ret1 - The second choice of target
Critter ret2 - The third choice of target
Critter ret3 - The fourth choice of target
```

-------------------------------------------

#### `HOOK_USEOBJON (hs_useobjon.int)`

Runs when:
1) a critter uses an object on another critter (or themselves).
2) a critter uses an object from inventory screen AND this object does not have "Use" action flag set and it's not active flare or explosive.
3) player or AI uses any drug

This is fired before the object is used, and the relevant `use_obj_on` script procedures are run. You can disable default item behavior.

__NOTE:__ You can't remove and/or destroy this object during the hookscript (game will crash otherwise). To remove it, return 1.

```
Critter arg0 - The target
Critter arg1 - The user
int     arg2 - The object used

int     ret0 - overrides hard-coded handler and selects what should happen with the item (0 - place it back, 1 - remove it, -1 - use engine handler)
```

-------------------------------------------

#### `HOOK_USEOBJ (hs_useobj.int)`

Runs when:
1) a critter uses an object from inventory which have "Use" action flag set or it's an active flare or dynamite.
2) player uses an object from main interface

This is fired before the object is used, and the relevant `use_obj` script procedures are run. You can disable default item behavior.

__NOTE:__ You can't remove and/or destroy this object during the hookscript (game will crash otherwise). To remove it, return 1.

```
Critter arg0 - The user
Obj     arg1 - The object used

int     ret0 - overrides hard-coded handler and selects what should happen with the item (0 - place it back, 1 - remove it, -1 - use engine handler)
```

-------------------------------------------

#### `HOOK_REMOVEINVENOBJ (hs_removeinvenobj.int)`

Runs when an object is removed from a container or critter's inventory for any reason.

```
Obj     arg0 - the owner that the object is being removed from
Item    arg1 - the item that is being removed
int     arg2 - the number of items to remove
int     arg3 - The reason the object is being removed (see RMOBJ_* constants)
Obj     arg4 - The destination object when the item is moved to another object, 0 otherwise
```

-------------------------------------------

#### `HOOK_BARTERPRICE (hs_barterprice.int)`

Runs whenever the value of goods being purchased is calculated.

__NOTE:__ The hook is executed twice when entering the barter screen or after transaction: the first time is for the player and the second time is for NPC.

```
Critter arg0 - the critter doing the bartering (either dude_obj or inven_dude)
Critter arg1 - the critter being bartered with
int     arg2 - the default value of the goods
Critter arg3 - table of requested goods (being bought from NPC)
int     arg4 - the amount of actual caps in the barter stack (as opposed to goods)
int     arg5 - the value of all goods being traded before skill modifications
Critter arg6 - table of offered goods (being sold to NPC)
int     arg7 - the total cost of the goods offered by the player
int     arg8 - 1 if the "offers" button was pressed (not for a party member), 0 otherwise
int     arg9 - 1 if trading with a party member, 0 otherwise

int     ret0 - the modified value of all of the goods (pass -1 if you just want to modify offered goods)
int     ret1 - the modified value of all offered goods
```

-------------------------------------------

#### `HOOK_MOVECOST (hs_movecost.int)`

Runs when calculating the AP cost of movement.

```
Critter arg0 - the critter doing the moving
int     arg1 - the number of hexes being moved
int     arg2 - the original AP cost

int     ret0 - the new AP cost
```

-------------------------------------------

#### DEPRECATED HOOKS:
##### `HOOK_HEXMOVEBLOCKING  (hs_hexmoveblocking.int)`
##### `HOOK_HEXAIBLOCKING    (hs_hexaiblocking.int)`
##### `HOOK_HEXSHOOTBLOCKING (hs_hexshootblocking.int)`
##### `HOOK_HEXSIGHTBLOCKING (hs_hexsightblocking.int)`

Run when checking to see if a hex blocks movement or shooting. (or ai-ing, presumably...)

__NOTE:__ These hook scripts can become very CPU-intensive and you should avoid using them.
For this reason, these hooks are not thoroughly supported in sfall, and may be removed in future versions.\
If you want to check if some tile or path is blocked, use functions: `obj_blocking_tile`, `obj_blocking_line`, `path_find_to`.\
If you want script to be called every time NPC moves by hex in combat, use `HOOK_MOVECOST` hook.

```
Critter arg0 - the critter doing the moving
int     arg1 - the tile number being checked
int     arg2 - the elevation being checked
int     arg3 - 1 if the hex would normally be blocking

Obj     ret0 - 0 if the hex doesn't block, or any sort of object pointer if it does
```

-------------------------------------------

#### `HOOK_ITEMDAMAGE (hs_itemdamage.int)`

Runs when retrieving the damage rating of the player's used weapon. (Which may be their fists.)

```
int     arg0 - The default min damage
int     arg1 - The default max damage
Item    arg2 - The weapon used (0 if unarmed)
Critter arg3 - The critter doing the attacking
int     arg4 - The type of attack
int     arg5 - non-zero if this is an attack using a melee weapon

int     ret0 - Either the damage to be used, if ret1 isn't given, or the new minimum damage if it is
int     ret1 - The new maximum damage
```

-------------------------------------------

#### `HOOK_AMMOCOST (hs_ammocost.int)`

Runs when calculating ammo cost for a weapon. Doesn't affect damage, only how much ammo is spent.\
By default, weapons can make attacks when at least 1 ammo is left, regardless of ammo cost calculations.\
To add proper check for ammo before attacking and proper calculation of the number of burst rounds (hook type 1 and 2 in `arg3`), set **CheckWeaponAmmoCost=1** in **Misc** section of ddraw.ini.

```
Item    arg0 - The weapon
int     arg1 - Number of bullets in burst or 1 for single shots
int     arg2 - The amount of ammo that will be consumed, calculated by the original ammo cost function (this is basically 2 for Super Cattle Prod and Mega Power Fist)
               NOTE: for hook type 2, this value is the ammo cost per round (default is always 1)
int     arg3 - Type of hook:
               0 - when subtracting ammo after single shot attack
               1 - when checking for "out of ammo" before attack
               2 - when calculating number of burst rounds
               3 - when subtracting ammo after burst attack

int     ret0 - The new amount of ammo to be consumed, or ammo cost per round for hook type 2 (set to 0 for unlimited ammo)
```

-------------------------------------------

#### `HOOK_KEYPRESS (hs_keypress.int)`

Runs once every time when any key was pressed or released.
- DX codes: see **dik.h** header or https://kippykip.com/b3ddocs/commands/scancodes.htm
- VK codes: http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx

__NOTE:__ If you want to override a key, the new key DX scancode should be the same for both pressed and released events.

```
int     arg0 - event type: 1 - pressed, 0 - released
int     arg1 - key DX scancode
int     arg2 - key VK code (very similar to ASCII codes)

int     ret0 - overrides the pressed key (a new key DX scancode or 0 for no override)
```

-------------------------------------------

#### `HOOK_MOUSECLICK (hs_mouseclick.int)`

Runs once every time when a mouse button was pressed or released.

```
int     arg0 - event type: 1 - pressed, 0 - released
int     arg1 - button number (0 - left, 1 - right, up to 7)
```

-------------------------------------------

#### `HOOK_USESKILL (hs_useskill.int)`

Runs when using any skill on any object.

This is fired before the default handlers are called, which you can override. In this case you should write your own skill use handler entirely, or otherwise nothing will happen (this includes fade in/fade out, time lapsing and messages - all of this can be scripted; to get vanilla text messages - use `message_str_game` along with `sprintf`).
Suggested use - override First Aid/Doctor skills to buff/nerf them, override Steal skill to disallow observing NPCs inventories in some cases.

Does not run if the script of the object calls `script_overrides` for using the skill.

```
Critter arg0 - The user critter
Obj     arg1 - The target object
int     arg2 - skill being used
int     arg3 - skill bonus from items such as first aid kits

int     ret0 - overrides hard-coded handler (-1 - use engine handler, any other value - override; if it is 0, there will be a 10% chance of removing the used medical item)
```

-------------------------------------------

#### `HOOK_STEAL (hs_steal.int)`

Runs when checking an attempt to steal or plant an item in other inventory using Steal skill.

This is fired before the default handlers are called, which you can override. In this case you MUST provide message of the result to player ("You steal the %s", "You are caught planting the %s", etc.).

Example message (vanilla behavior):\
`display_msg(sprintf(mstr_skill(570 + (isSuccess != false) + arg3 * 2), obj_name(arg2)));`

```
Critter arg0 - Thief
Obj     arg1 - The target
Item    arg2 - Item being stolen/planted
int     arg3 - 0 when stealing, 1 when planting

int     ret0 - overrides hard-coded handler (1 - force success, 0 - force fail, -1 - use engine handler)
```

-------------------------------------------

#### `HOOK_WITHINPERCEPTION (hs_withinperception.int)`

Runs when checking if one critter sees another critter. This is used in different situations like combat AI. You can override the result.

__NOTE:__ `obj_can_see_obj` calls this first when deciding if critter can possibly see another critter with regard to perception, lighting, sneak factors.\
If check fails, the end result is false. If check succeeds (e.g. critter is within perception range), another check is made if there is any blocking tile between two critters (which includes stuff like windows, large bushes, barrels, etc.) and if there is - check still fails.\
You can override "within perception" check by returning 0 or 1, OR, as a convenience, you can also override blocking check after the perception check by returning 2 instead. In this case you should add "line of sight" check inside your hook script, otherwise critters will detect you through walls.

This is fired after the default calculation is made.

```
Critter arg0 - Watcher object
Obj     arg1 - Target object
int     arg2 - Result of vanilla function: 1 - within perception range, 0 - otherwise
int     arg3 - Type of hook:
               1 - when being called from obj_can_see_obj script function
               2 - when being called from obj_can_hear_obj script function (need to set ObjCanHearObjFix=1 in ddraw.ini)
               3 - when AI determines whether it sees a potential target when selecting attack targets
               0 - all other cases

int     ret0 - overrides the returned result of the function:
               0 - not in range (can't see)
               1 - in range (will see if not blocked)
               2 - forced detection (will see regardless, only used in obj_can_see_obj script function which is called by every critter in the game)
```

-------------------------------------------

#### `HOOK_INVENTORYMOVE (hs_inventorymove.int)`

Runs before moving items between inventory slots in dude interface. You can override the action.

What you can NOT do with this hook:
- force moving items to inappropriate slots (like gun in armor slot)

What you can do:
- restrict player from using specific weapons or armors
- add AP costs for all inventory movement including reloading
- apply or remove some special scripted effects depending on PC's armor

```
int     arg0 - Target slot:
               0 - main backpack
               1 - left hand
               2 - right hand
               3 - armor slot
               4 - weapon, when reloading it by dropping ammo
               5 - container, like bag/backpack
               6 - dropping on the ground
               7 - picking up item
               8 - dropping item on the character portrait
Item    arg1 - Item being moved
Item    arg2 - Item being replaced, weapon being reloaded, or container being filled (can be 0)

int     ret0 - Override setting (-1 - use engine handler, any other value - prevent relocation of item/reloading weapon/picking up item)
```

Notes for the event of dropping items on the ground:
- the event is called for each item when dropping multiple items from the stack
- for ammo type items, the number of dropped ammo in a pack can be found by using the `get_weapon_ammo_count` function
- for the `PID_BOTTLE_CAPS` item, the event is called only once, and the number of dropped units can be found from the value of the `OBJ_DATA_CUR_CHARGES` object field (or with the `get_weapon_ammo_count` function)

-------------------------------------------

#### `HOOK_INVENWIELD (hs_invenwield.int)`

Runs before causing a critter or the player to wield/unwield an armor or a weapon (except when using the inventory by PC).\
An example usage would be to change critter art depending on armor being used or to dynamically customize weapon animations.

__NOTE:__ When replacing a previously wielded armor or weapon, the unwielding hook will not be executed.
If you need to rely on this, try checking if armor/weapon is already equipped when wielding hook is executed.

```
Critter arg0 - critter
Item    arg1 - item being wielded or unwielded (weapon/armor)
int     arg2 - slot (INVEN_TYPE_*)
int     arg3 - 1 when wielding, 0 when unwielding
int     arg4 - 1 when removing an equipped item from inventory, 0 otherwise

int     ret0 - overrides hard-coded handler (-1 - use engine handler, any other value - override) - NOT RECOMMENDED
```

-------------------------------------------

#### `HOOK_ADJUSTFID (hs_adjustfid.int)`

Runs after calculating character figure FID on the inventory screen, whenever the game decides that character appearance might change.\
Also happens on other screens, like barter.

__NOTE:__ FID has following format: `0x0ABBCDDD`, where: `A` - object type, `BB` - animation code (always 0 in this case), `C` - weapon code, `DDD` - FRM index in LST file.

```
int     arg0 - the vanilla FID calculated by the engine according to critter base FID and armor/weapon being used
int     arg1 - the modified FID calculated by the internal sfall code (like Hero Appearance Mod)

int     ret0 - overrides the calculated FID with provided value
```

-------------------------------------------

#### `HOOK_COMBATTURN (hs_combatturn.int)`

Runs before and after each turn in combat (for both PC and NPC).

```
int     arg0 - event type:
                1 - start of turn
                0 - normal end of turn
               -1 - combat ends abruptly (by script or by pressing Enter during PC turn)
               -2 - combat ends normally (hook always runs at the end of combat)
Critter arg1 - critter doing the turn
bool    arg2 - 1 at the start/end of the player's turn after loading a game saved in combat mode, 0 otherwise

int     ret0 - pass 1 at the start of turn to skip the turn, pass -1 at the end of turn to force end of combat
```

-------------------------------------------

#### `HOOK_CARTRAVEL (hs_cartravel.int)`

Runs continuously during worldmap travel on car.

```
int     arg0 - vanilla car speed (between 3 and 8 "steps")
int     arg1 - vanilla fuel consumption (100 and below)

int     ret0 - car speed override (pass -1 if you just want to override fuel consumption)
int     ret1 - fuel consumption override
```

-------------------------------------------

#### `HOOK_SETGLOBALVAR (hs_setglobalvar.int)`

Runs when setting the value of a global variable.

```
int     arg0 - the index number of the global variable being set
int     arg1 - the set value of the global variable

int     ret0 - overrides the value of the global variable
```

-------------------------------------------

#### `HOOK_RESTTIMER (hs_resttimer.int)`

Runs continuously while the player is resting (using pipboy alarm clock).

```
int     arg0 - the game time in ticks
int     arg1 - event type: 1 - when the resting ends normally, -1 - when pressing ESC to cancel the timer, 0 - otherwise
int     arg2 - the hour part of the length of resting time
int     arg3 - the minute part of the length of resting time

int     ret0 - pass 1 to interrupt the resting, pass 0 to continue the rest if it was interrupted by pressing ESC key
```

-------------------------------------------

#### `HOOK_GAMEMODECHANGE (hs_gamemodechange.int)`

Runs once every time when the game mode was changed, like opening/closing the inventory, character screen, pipboy, etc.

```
int     arg0 - event type: 1 - when the player exits the game, 0 - otherwise
int     arg1 - the previous game mode
```

-------------------------------------------

#### `HOOK_USEANIMOBJ (hs_useanimobj.int)`

Runs before playing the "use" (usually "magic hands") animation when a critter uses a scenery/container object on the map, or before walking/running animation if the player is at a distance from the object.

```
Critter arg0 - the critter that uses an object (usually dude_obj)
Obj     arg1 - the object being used
int     arg2 - the animation code being used (see ANIM_* in Animcomd.h)

int     ret0 - overrides the animation code (pass -1 if you want to skip the animation)
```

-------------------------------------------

#### `HOOK_EXPLOSIVETIMER (hs_explosivetimer.int)`

Runs after setting the explosive timer. You can override the result.

```
int     arg0 - the time in ticks set in the timer
Obj     arg1 - the explosive object
int     arg2 - the result of engine calculation of whether the timer was set successfully: 1 - failure, 2 - success (similar to ROLL_* in Condtion.h)

int     ret0 - overrides the time of the timer (maximum 18000 ticks)
int     ret1 - overrides the result of engine calculation: 0/1 - failure, 2/3 - success (similar to ROLL_*), any other value - use engine handler
```

-------------------------------------------

#### `HOOK_DESCRIPTIONOBJ (hs_descriptionobj.int)`

Runs when using the examine action icon to display the description of an object. You can override the description text.
An example usage would be to add an additional description to the item based on player's stats/skills.

Does not run if the script of the object overrides the description.

```
Obj     arg0 - the object

int     ret0 - a pointer to the new text received by using the get_string_pointer function
```

-------------------------------------------

#### `HOOK_USESKILLON (hs_useskillon.int)`

Runs before using any skill on any object. Lets you override the critter that uses the skill.

__NOTE:__ The user critter can't be overridden when using Steal skill.

```
Critter arg0 - the user critter (usually dude_obj)
Obj     arg1 - the target object/critter
int     arg2 - skill being used

int     ret0 - a new critter to override the user critter. Pass -1 to cancel the skill use, pass 0 to skip this return value
int     ret1 - pass 1 to allow the skill to be used in combat (only for dude_obj or critter being controlled by the player)
```

-------------------------------------------

#### `HOOK_ONEXPLOSION (hs_onexplosion.int)`

Runs when Fallout is checking all the tiles within the explosion radius for targets before an explosion occurs.\
The tile checking will be interrupted when 6 additional targets (critters) are received.

```
int     arg0 - event type: 1 - when checking objects within the explosion radius without causing damage (e.g. the player drops an active explosive), 0 - otherwise
Critter arg1 - the attacker
int     arg2 - the tile on which the explosion occurs
int     arg3 - checked tile within the explosion radius
Obj     arg4 - first found object on the checked tile as an additional target
Critter arg5 - the target critter, may be 0 or equal to the attacker
int     arg6 - 1 when using throwing weapons (e.g. grenades), 0 otherwise

int     ret0 - overrides the found object on the checked tile, pass 0 to skip the object
```

-------------------------------------------

#### `HOOK_SUBCOMBATDAMAGE (hs_subcombatdmg.int)`

This hook overrides the vanilla damage calculation formula.

Runs when:
1) Before the game calculates how much damage each target will get. This includes primary target as well as all extras (explosions and bursts).
2) AI decides whether it is safe to use area attack (burst, grenades), if he might hit friendlies.

Does not run for misses, non-combat damage like dynamite explosions, or if one of the damage formulas is selected in ddraw.ini.

```
Critter arg0  - the attacker
Critter arg1  - the target
Item    arg2  - the weapon used in the attack
int     arg3  - attack type (see ATKTYPE_* constants)
int     arg4  - number of bullets actually hit the target (1 for melee attacks)
int     arg5  - target's Damage Resistance (DR) value (affected by critical hit effects, perks, traits, and special unarmed attacks)
int     arg6  - target's Damage Threshold (DT) value (affected by critical hit effects, perks, traits, and special unarmed attacks)
int     arg7  - bonus ranged damage from the perk
int     arg8  - damage multiplier (this is divided by 2, so a value of 3 does 1.5x damage, and 8 does 4x damage. Usually it's 2; for critical hits, the value is taken from the critical table; with Silent Death perk and the corresponding attack conditions, the value will be doubled)
int     arg9  - combat difficulty multiplier (125 - rough, 100 - normal, 75 - wimpy; for player or party members it's always 100)
int     arg10 - the calculated amount of damage (usually 0, required when using multiple hook scripts to calculate damage and using the set_sfall_arg function)
mixed   arg11 - computed attack data (see C_ATTACK_* for offsets and use get/set_object_data functions to get/set the data)

int     ret0 - the returned amount of damage
```

-------------------------------------------

#### `HOOK_SETLIGHTING (hs_setlighting.int)`

Runs before setting the light level for an object or a map. You can override the result.

```
Obj     arg0 - the object being set, or -1 when setting the light level for a map
int     arg1 - the light intensity
int     arg2 - the light radius, or -1 when setting the light level for a map

int     ret0 - overrides the light intensity. Intensity range is from 0 to 65536
int     ret1 - overrides the light radius. Radius range is from 0 to 8 (works only for the object)
```

-------------------------------------------

#### `HOOK_SNEAK (hs_sneak.int)`

Runs when the Sneak skill is activated, or when the game rolls another Sneak check after the duration for the current one is over.\
You can override the result of a random Sneak check or the duration time for the current result.

```
int     arg0 - Sneak check result: 1 - success, 0 - failure
int     arg1 - the duration in ticks for the current Sneak check (time depends on Sneak skill level)
Critter arg2 - the critter (usually dude_obj)

int     ret0 - overrides the result of the Sneak check
int     ret1 - overrides the duration time for the current result
```

-------------------------------------------

#### `HOOK_STDPROCEDURE, HOOK_STDPROCEDURE_END (hs_stdprocedure.int)`

Runs before or after Fallout engine executes a standard procedure (handler) in any script of any object.

__NOTE:__ This hook will not be executed for `start`, `critter_p_proc`, `timed_event_p_proc`, and `map_update_p_proc` procedures.

```
int     arg0 - the number of the standard script handler (see define.h)
Obj     arg1 - the object that owns this handler (self_obj)
Obj     arg2 - the object that called this handler (source_obj, can be 0)
int     arg3 - 1 after procedure execution (for HOOK_STDPROCEDURE_END), 0 otherwise

int     ret0 - pass -1 to cancel the execution of the handler (only for HOOK_STDPROCEDURE)
```

-------------------------------------------

#### `HOOK_TARGETOBJECT (hs_targetobject.int)`

Runs when the targeting cursor hovers over an object, or when the player tries to attack the target object.\
You can override the target object or prevent the player from attacking the chosen target.

```
int     arg0 - event type: 0 - when the targeting cursor hovers over the object, 1 - when trying to attack the target object
int     arg1 - 1 when the target object is valid to attack, 0 otherwise
Obj     arg2 - the target object

mixed   ret0 - overrides the target object, or pass -1 to prevent the player from attacking the object
```

-------------------------------------------

#### `HOOK_ENCOUNTER (hs_encounter.int)`

Runs whenever a random encounter occurs (except the Horrigan meeting and scripted encounters), or when the player enters a local map from the world map.\
You can override the map for loading or the encounter.

```
int     arg0 - event type: 0 - when a random encounter occurs, 1 - when the player enters from the world map
int     arg1 - the map ID that the encounter will load (see MAPS.h or Maps.txt)
int     arg2 - 1 when the encounter occurs is a special encounter, 0 otherwise

int     ret0 - overrides the map ID, or pass -1 for event type 0 to cancel the encounter and continue traveling
int     ret1 - pass 1 to cancel the encounter and load the specified map from the ret0 (only for event type 0)
```

-------------------------------------------

#### `HOOK_ADJUSTPOISON (hs_adjustpoison.int)`

Runs when a critter's poison level is changed, or when the player takes damage from the poison.

```
Critter arg0 - the critter
int     arg1 - the amount of poison being added/removed
int     arg2 - the damage value at the time of applying the poison effect
               (damage from the poison effect is implemented only for the player character; for other critters, this value will always be 0)

int     ret0 - the new amount of poison being added/removed
int     ret1 - the new damage value, only negative values are allowed (will only be valid at the time of taking damage from the poison)
```

-------------------------------------------

#### `HOOK_ADJUSTRADS (hs_adjustrads.int)`

Runs when a critter's radiation level is changed.

```
Critter arg0 - the critter (usually dude_obj)
int     arg1 - the amount of radiation being added/removed

int     ret0 - the new amount of radiation being added/removed
```

-------------------------------------------

#### `HOOK_ROLLCHECK (hs_rollcheck.int)`

Runs when a game event performs a random roll to check the chance of success or failure.

```
int     arg0 - event type:
               1 - checks the chance of an attack hitting the target
               2 - checks the chance of a bullet from a burst hitting the target (for burst attacks)
               3 - checks the chance when using skills (not listed below)
               4 - check the chance of using Repair skill
               5 - check the chance of using Doctor skill
               6 - check the chance of using Steal skill for the thief (usually the player)
               7 - the second Steal skill chance check for the target to catch the thief, in which the target's failure is the thief's success result
int     arg1 - the value of roll result (see ROLL_* constants), which is calculated as:
               for ROLL_CRITICAL_SUCCESS: random(1, 100) <= (random_chance / 10) + bonus
               for ROLL_CRITICAL_FAILURE: random(1, 100) <= -random_chance / 10
int     arg2 - the chance value
int     arg3 - the bonus value, used when checking critical success
int     arg4 - random chance, calculated as: (chance - random(1, 100)), where a negative value is a failure check (ROLL_FAILURE)

int     ret0 - overrides the roll result
```

-------------------------------------------

#### `HOOK_BESTWEAPON (hs_bestweapon.int)`

Runs when the AI decides which weapon is the best while searching the inventory for a weapon to equip in combat.\
This also runs when the player presses the "Use Best Weapon" button on the party member control panel.

```
Critter arg0 - the critter searching for a weapon
Item    arg1 - the best weapon chosen from two items
Item    arg2 - the first choice of weapon
Item    arg3 - the second choice of weapon
Critter arg4 - the target of the critter (can be 0)

Item    ret0 - overrides the chosen best weapon
```

-------------------------------------------

#### `HOOK_CANUSEWEAPON (hs_canuseweapon.int)`

Run when the AI checks whether it can use a weapon, or when the game checks whether the player can use an item (weapon) in hand slot.\
For AI, this mostly happens when NPCs try to find weapons in their inventory or on the map.\
For the player, this happens when the game updates the item data for active item slots on the interface bar.

```
Critter arg0 - the critter doing the check
Item    arg1 - the item being checked
int     arg2 - attack type (see ATKTYPE_* constants), or -1 for dude_obj
int     arg3 - original result of engine function: 1 - can use, 0 - can't use

int     ret0 - overrides the result of engine function. Any non-zero value allows using the weapon
```
