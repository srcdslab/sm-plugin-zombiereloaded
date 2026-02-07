#if defined _zr_weapons_included
  #endinput
#endif
#define _zr_weapons_included

/**
 * Gets a weapon's entity name.
 *
 * @param weapon        Weapon name to get entity name from.
 * @param entity        String to store entity name.
 * @param maxlen        Maximum length of string buffer.
 * @return              True if weapon exists, false otherwise.
 */
native bool ZR_GetWeaponEntity(const char[] weapon, char[] entity, int maxlen);

/**
 * Gets a weapon's type.
 *
 * @param weapon        Weapon name to get type from.
 * @param type          String to store weapon type.
 * @param maxlen        Maximum length of string buffer.
 * @return              True if weapon exists, false otherwise.
 */
native bool ZR_GetWeaponType(const char[] weapon, char[] type, int maxlen);

/**
 * Gets a weapon's slot.
 *
 * @param weapon        Weapon name to get slot from.
 * @return              Weapon slot, or -1 if weapon doesn't exist.
 */
native int ZR_GetWeaponSlot(const char[] weapon);

/**
 * Gets a weapon's default restriction state.
 *
 * @param weapon        Weapon name to get restriction state from.
 * @return              True if weapon is restricted by default, false otherwise.
 */
native bool ZR_GetWeaponRestrictDefault(const char[] weapon);

/**
 * Gets a weapon's toggleable state.
 *
 * @param weapon        Weapon name to get toggleable state from.
 * @return              True if weapon is toggleable, false otherwise.
 */
native bool ZR_GetWeaponToggleable(const char[] weapon);

/**
 * Gets a weapon's ammo type.
 *
 * @param weapon        Weapon name to get ammo type from.
 * @param ammotype      String to store ammo type.
 * @param maxlen        Maximum length of string buffer.
 * @return              True if weapon exists, false otherwise.
 */
native bool ZR_GetWeaponAmmoType(const char[] weapon, char[] ammotype, int maxlen);

/**
 * Gets a weapon's ammo price.
 *
 * @param weapon        Weapon name to get ammo price from.
 * @return              Ammo price, or -1 if weapon doesn't exist.
 */
native int ZR_GetWeaponAmmoPrice(const char[] weapon);

/**
 * Gets a weapon's knockback value.
 *
 * @param weapon        Weapon name to get knockback from.
 * @return              Knockback value, or 0.0 if weapon doesn't exist.
 */
native float ZR_GetWeaponKnockback(const char[] weapon);

/**
 * Gets a weapon's ZMarket name.
 *
 * @param weapon        Weapon name to get ZMarket name from.
 * @param name          String to store ZMarket name.
 * @param maxlen        Maximum length of string buffer.
 * @return              True if weapon exists, false otherwise.
 */
native bool ZR_GetWeaponZMarketName(const char[] weapon, char[] name, int maxlen);

/**
 * Gets a weapon's ZMarket price.
 *
 * @param weapon        Weapon name to get ZMarket price from.
 * @return              ZMarket price, or -1 if weapon doesn't exist.
 */
native int ZR_GetWeaponZMarketPrice(const char[] weapon);

/**
 * Gets a weapon's ZMarket purchase maximum.
 *
 * @param weapon        Weapon name to get ZMarket purchase maximum from.
 * @return              ZMarket purchase maximum, or -1 if weapon doesn't exist.
 */
native int ZR_GetWeaponZMarketPurchaseMax(const char[] weapon);

/**
 * Gets a weapon's ZMarket command.
 *
 * @param weapon        Weapon name to get ZMarket command from.
 * @param command       String to store ZMarket command.
 * @param maxlen        Maximum length of string buffer.
 * @return              True if weapon exists, false otherwise.
 */
native bool ZR_GetWeaponZMarketCommand(const char[] weapon, char[] command, int maxlen);

/**
 * Sets a weapon's knockback value.
 *
 * @param weapon        Weapon name to set knockback for.
 * @param value         New knockback value.
 * @return              True if weapon exists and value is valid, false otherwise.
 */
native bool ZR_SetWeaponKnockback(const char[] weapon, float value);

/**
 * Set all wepaon restrict array for the specified client.
 * @param client    The client index.
 * @param value	   The restrict value
 * @noreturn        
 */
native void ZR_SetClientWeaponRestrictAll(int client, bool value);

/**
 * Set all wepaon restrict array for all clients.
 * @param client    The client index.
 * @param value	   The restrict value
 * @noreturn        
 */
native void ZR_SetAllRestrictAll(bool value);

/**
 * Set weapon restrict value to a specific client.
 * @param client    		 The client index.
 * @param returntarget    The weapon or weapon type name.
 * @param value	   		 The restrict value
 * @return				 True on success, false otherwise            
 */
native bool ZR_SetClientWeaponRestrict(int client, const char[] returntarget, bool value);

/**
 * Set weapon restrict value to all clients.
 * @param returntarget    The weapon or weapon type name.
 * @param value	   		 The restrict value
 * @return				 True on success, false otherwise        
 */
native bool ZR_SetAllWeaponRestrict(const char[] returntarget, bool value);