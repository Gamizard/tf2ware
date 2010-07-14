#if defined _mw_ach_included_
    #endinput
#endif
#define _mw_ach_included_

#define MWACHIEVEMENTSAPIVERSION 2

public SharedPlugin:__pl_mw_ach = {
    name = "mw_ach",
    file = "mw_achievements.smx",
    #if defined REQUIRE_PLUGIN
    required = 1,
    #else
    required = 0,
    #endif
};
 
public __pl_mw_ach_SetNTVOptional() {
    MarkNativeAsOptional("mw_AchievementEvent");
}

/**
 * Triggers an achievement for a client. 
 *
 * @param event: The name of the event in the database which is associated with a custom achievement
 * @param target0: target0
 * @param target1: target1
 * @param target2: target2
 * @param add: add
 */
native mw_AchievementEvent(String:event[128], target0, target1, target2, add);