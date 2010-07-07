#pragma semicolon 1

//Includes:
#include <sourcemod>
#include <sdktools>
#include <sdktools_sound>
#include <colors>
#include <tf2>
#include <tf2_stocks>
#include <sdkhooks>
#include <geoip>
#include <attachments>

#undef REQUIRE_PLUGIN
#include <mw_achievements_natives>
#define REQUIRE_PLUGIN

#define MAX_MINIGAMES 20

#define PLUGIN_VERSION "0.8.0-15"
#define MUSIC_START "imgay/tf2ware/tf2ware_intro.mp3"
#define MUSIC_START_LEN 2.18
#define MUSIC_WIN "imgay/tf2ware/tf2ware_win.mp3"
#define MUSIC_FAIL "imgay/tf2ware/tf2ware_fail.mp3"
#define MUSIC_END_LEN 2.2
#define SOUND_COMPLETE "imgay/tf2ware/complete_me.mp3"
#define SOUND_COMPLETE_YOU "imgay/tf2ware/complete_you.mp3"
#define MUSIC_SPEEDUP "imgay/tf2ware/tf2ware_speedup.mp3"
#define MUSIC_SPEEDUP_LEN 3.29
#define MUSIC_BOSS "imgay/tf2ware/boss.mp3"
#define MUSIC_BOSS_LEN 3.9
#define MUSIC_GAMEOVER "imgay/tf2ware/warioman_gameover.mp3"
#define MUSIC_GAMEOVER_LEN 8.17
#define SOUND_MINISCORE "items/pumpkin_drop.wav"
#define SOUND_HEAVY_KISS "vo/heavy_generic01.wav"
#define MUSIC_WAITING "imgay/tf2ware/waitingforplayers.mp3"

#define SND_CHANNEL_SPECIFIC 32

#define PARTICLE_WIN_BLUE "teleportedin_blue"
#define PARTICLE_WIN_RED "teleportedin_red"
#define PARTICLE_BOMB "cinefx_goldrush_embers"
#define PARTICLE_EXPLODE "cinefx_goldrush_initial_smoke"

#define TF2_PLAYER_TAUNTING        (1 << 7)    // 128        Taunting

#define WW_BOMB "pl_hoodoo/alarm_clock_ticking_3.wav"
#define WW_BOMB_MODEL "models/custom/dirty_bomb_cart.mdl"

new String:var_heavy_love[][] = {"imgay/tf2ware/heavy_ilu.wav", "vo/heavy_specialcompleted08.wav", "vo/heavy_award04.wav"};

new String:g_name[MAX_MINIGAMES][12];
new Function:g_initFuncs[MAX_MINIGAMES];

// Language strings
new String:var_lang[][] = {"", "it/"};

// Handles
new Handle:ww_enable;
new Handle:ww_speed;
new Handle:ww_music;
new Handle:ww_force;
new Handle:ww_log;
new Handle:heavy_love[MAXPLAYERS+1] = INVALID_HANDLE;
new Handle:ww_allowedCommands;
new Handle:hudScore;
// REPLACE WEAPON
new Handle:GameConf = INVALID_HANDLE;
new Handle:hGiveNamedItem = INVALID_HANDLE;
new Handle:hWeaponEquip = INVALID_HANDLE;
new Handle:microgametimer = INVALID_HANDLE;

// Keyvalues configuration handle
new Handle:MinigameConf = INVALID_HANDLE;

// Bools
new bool:g_Complete[MAXPLAYERS+1];
new bool:g_Spawned[MAXPLAYERS+1];
new bool:g_attack = false;
new bool:g_respawn = false;
new bool:bossBattle = false;
new bool:g_enabled = false;
new bool:g_first = false;
new bool:g_waiting = true;
new bool:g_Achievements = false;

// Ints
new g_Mission[MAXPLAYERS+1];
new g_Barrels[MAXPLAYERS+1];
new g_NeedleDelay[MAXPLAYERS+1];
new g_Points[MAXPLAYERS+1];
new g_Id[MAXPLAYERS+1];
new g_Winner[MAXPLAYERS+1];
new g_Minipoints[MAXPLAYERS+1];
new g_Country[MAXPLAYERS+1];
new g_Sprites[MAXPLAYERS+1];
new currentSpeed;
new iMinigame;
new status;
new randommini;
new g_offsCollisionGroup;
new timeleft = 8;
new white;
new g_HaloSprite;
new g_ExplosionSprite;
new g_result = 0;
new String:g_mathquestion[24];
new g_bomb = 0;
new Roundstarts = 0;
new g_lastminigame = 0;
new g_lastboss = 0;

new g_welcomedisplayed[MAXPLAYERS+1];

// Strings
new String:materialpath[512] = "imgay/";
// Name of current minigame being played
new String:minigame[12];

// VALID iMinigame FORWARD HANDLERS //////////////
new Handle:g_justEnteredMinigame;
new Handle:g_OnAlmostEndMinigame;
new Handle:g_OnTimerMinigame;
new Handle:g_OnEndMinigame;
new Handle:g_OnGameFrame_Minigames;
new Handle:g_PlayerDeath;
/////////////////////////////////////////

#include tf2ware\microgames\hitenemy.inc
#include tf2ware\microgames\spycrab.inc
#include tf2ware\microgames\kamikaze.inc
#include tf2ware\microgames\math.inc
#include tf2ware\microgames\sawrun.inc
#include tf2ware\microgames\barrel.inc
#include tf2ware\microgames\needlejump.inc
#include tf2ware\microgames\hopscotch.inc
#include tf2ware\microgames\airblast.inc
#include tf2ware\microgames\movement.inc
#include tf2ware\microgames\flood.inc
#include tf2ware\microgames\simonsays.inc
#include tf2ware\microgames\bball.inc
#include tf2ware\microgames\hugging.inc
#include tf2ware\microgames\redfloor.inc

#include tf2ware\mw_tf2ware_features.inc

public Plugin:myinfo = {
    name = "TF2 Ware",
    author = "Mecha the Slag",
    description = "Wario Ware in Team Fortress 2!",
    version = PLUGIN_VERSION,
    url = "http://mechaware.net/"
};

public OnPluginStart() {
    // G A M E  C H E C K //
    decl String:game[32];
    GetGameFolderName(game, sizeof(game));
    if(!(StrEqual(game, "tf")))
    {
        SetFailState("This plugin is only for Team Fortress 2, not %s", game);
    }
    
    // Check for SDKHooks
    if(GetExtensionFileStatus("sdkhooks.ext") < 1)
        SetFailState("SDK Hooks is not loaded.");
    
    // Find collision group offsets
    g_offsCollisionGroup = FindSendPropOffs("CBaseEntity", "m_CollisionGroup");
    if (g_offsCollisionGroup == -1) {
        PrintToServer("* FATAL ERROR: Failed to get offset for CBaseEntity::m_CollisionGroup");
    }
    
    // Add server tag
    AddServerTag("TF2Ware");
    
    // Load game config
    GameConf = LoadGameConfigFile("tf2ware.games");
    
    // Load minigames
    decl String:imFile[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, imFile, sizeof(imFile), "configs/minigames.cfg");
    
    MinigameConf = CreateKeyValues("Minigames");
    if (FileToKeyValues(MinigameConf, imFile)) {
        PrintToServer("Loaded minigames from minigames.cfg");
        
        KvGotoFirstSubKey(MinigameConf);
        new i=0;
        do {
            KvGetSectionName(MinigameConf, g_name[KvGetNum(MinigameConf, "id")-1], 32);
            i++;
          } while (KvGotoNextKey(MinigameConf)); 
          
        KvRewind(MinigameConf);
    }
    else {
        PrintToServer("Failed to load minigames.cfg!");
    }
    
    // SDK
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(GameConf, SDKConf_Virtual, "GiveNamedItem");
    PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    PrepSDKCall_SetReturnInfo(SDKType_CBaseEntity, SDKPass_Plain);
    hGiveNamedItem = EndPrepSDKCall();

    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(GameConf, SDKConf_Virtual, "WeaponEquip");
    PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
    hWeaponEquip = EndPrepSDKCall();
    
    // ConVars
    ww_enable = CreateConVar("ww_enable", "0", "Enables/Disables TF2 Ware.", FCVAR_PLUGIN);
    ww_force = CreateConVar("ww_force", "0", "Force a certain iMinigame (0 to not force).", FCVAR_PLUGIN);
    ww_speed = CreateConVar("ww_speed", "1", "Speed level.", FCVAR_PLUGIN);
    ww_music = CreateConVar("ww_music_fix", "0", "Apply music fix? Should only be on for localhosts during testing", FCVAR_PLUGIN);
    ww_log = CreateConVar("ww_log", "0", "Log server events?", FCVAR_PLUGIN);
    
    // Hooks
    HookConVarChange(ww_enable,StartMinigame_cvar);
    HookEvent("post_inventory_application", EventInventoryApplication,  EventHookMode_Post);
    HookEvent("player_death", Player_Death);
    HookEvent("player_team", Player_Team);
    HookEvent("teamplay_round_start", Event_Roundstart, EventHookMode_PostNoCopy);
    HookEvent("teamplay_game_over", Event_Roundend, EventHookMode_PostNoCopy);
    HookEvent("teamplay_round_stalemate", Event_Roundend, EventHookMode_PostNoCopy);
    HookEvent("teamplay_round_win", Event_Roundend, EventHookMode_PostNoCopy);
    HookEvent("player_changeclass",Event_ChangeClass);
    RegAdminCmd("ww_give", Command_points, ADMFLAG_GENERIC, "Gives you 20 points - You're a winner! (testing feature)");
    
    // Vars
    currentSpeed = GetConVarInt(ww_speed);
    iMinigame = 1;
    status = 0;
    randommini = 0;
    Roundstarts = 0;
    SetStateAll(false);
    ResetWinners();
    SetMissionAll(0);
    
    // FORWARDS FOR MINIGAMES
    g_justEnteredMinigame = CreateForward(ET_Ignore, Param_Cell);
    g_OnAlmostEndMinigame = CreateForward(ET_Ignore);
    g_OnTimerMinigame = CreateForward(ET_Ignore, Param_Cell);
    g_OnEndMinigame = CreateForward(ET_Ignore);
    g_OnGameFrame_Minigames = CreateForward(ET_Ignore);
    g_PlayerDeath = CreateForward(ET_Ignore, Param_Cell);
    
    
    // MINIGAME REGISTRATION
    RegMinigame("HitEnemy", HitEnemy_OnMinigame);
    RegMinigame("Spycrab", Spycrab_OnMinigame);
    RegMinigame("Kamikaze", Kamikaze_OnMinigame);
    RegMinigame("Math", Math_OnMinigame);
    RegMinigame("SawRun", SawRun_OnMinigame);
    RegMinigame("Barrel", Barrel_OnMinigame);
    RegMinigame("Needlejump", Needlejump_OnMinigame);
    RegMinigame("Hopscotch", Hopscotch_OnMinigame);
    RegMinigame("Airblast", Airblast_OnMinigame);
    RegMinigame("Movement", Movement_OnMinigame);
    RegMinigame("Flood", Flood_OnMinigame);
    RegMinigame("SimonSays", SimonSays_OnMinigame);
    RegMinigame("BBall", BBall_OnMinigame);
    RegMinigame("Hugging", Hugging_OnMinigame);
    RegMinigame("RedFloor", RedFloor_OnMinigame);

    // CHEATS
    HookConVarChange(FindConVar("sv_cheats"), OnConVarChanged_SvCheats);
    ww_allowedCommands = CreateArray(64);
    PushArrayString(ww_allowedCommands, "host_timescale");
    PushArrayString(ww_allowedCommands, "r_screenoverlay");
    PushArrayString(ww_allowedCommands, "thirdperson");
    PushArrayString(ww_allowedCommands, "firstperson");
    PushArrayString(ww_allowedCommands, "sv_cheats");
    UpdateClientCheatValue();
    HookAllCheatCommands();
    
    DestroyAllBarrels();
    
    // HUD
    hudScore = CreateHudSynchronizer();
    ResetScores();
    
    // Remove Notification Flags
    RemoveNotifyFlag("sv_tags");
    RemoveNotifyFlag("mp_respawnwavetime");
    RemoveNotifyFlag("mp_friendlyfire");
    RemoveNotifyFlag("tf_tournament_hide_domination_icons");
    SetConVarInt(FindConVar("tf_tournament_hide_domination_icons"), 0, true);
    
    // Include optional achievements
    if (LibraryExists("mw_ach")) g_Achievements = true;
    
    // Add logging
    if (GetConVarBool(ww_log)) {
    LogMessage("//////////////////////////////////////////////////////");
    LogMessage("//                     TF2WARE LOG                  //");
    LogMessage("//////////////////////////////////////////////////////");
    }
}

public OnMapStart() {
    // Check if the map has tf2ware at the beginning, otherwise tf2ware should be disabled
    // (A bit hacky I suppose)
    decl String:map[128];
    GetCurrentMap(map, 8);
    if(StrEqual(map, "tf2ware")) {
        g_enabled = true;
    }
    else {
        g_enabled = false;
    }

    precacheSound(MUSIC_START);
    precacheSound(MUSIC_WIN);
    precacheSound(MUSIC_FAIL);
    precacheSound(SOUND_COMPLETE);
    precacheSound(SOUND_COMPLETE_YOU);
    precacheSound(MUSIC_SPEEDUP);
    precacheSound(MUSIC_BOSS);
    precacheSound(MUSIC_GAMEOVER);
    precacheSound(WW_BOMB);
    precacheSound(SOUND_MINISCORE);
    precacheSound(MUSIC_WAITING);
    PrecacheModel("models/props_farm/wooden_barrel.mdl", true);
    PrecacheModel("models/props_farm/gibs/wooden_barrel_break02.mdl", true);
    PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk02.mdl", true);
    PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk04.mdl", true);
    PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk03.mdl", true);
    PrecacheModel("models/props_farm/gibs/wooden_barrel_chunk01.mdl", true);
    PrecacheModel(WW_BOMB_MODEL, true);
    
    decl String:input[512];
    
    for (new i = 0; i < sizeof(var_lang); i++) {
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_win.vmt", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_win.vtf", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_fail.vmt", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_fail.vtf", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_speed.vmt", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_speed.vtf", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_boss.vmt", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
        Format(input, sizeof(input), "materials/%s%stf2ware_minigame_boss.vtf", materialpath, var_lang[i]);
        AddFileToDownloadsTable(input);
    }
    
    for (new i = 1; i <= 19; i++) {
        Format(input, sizeof(input), "materials/imgay/tf2ware_points%d.vmt", i);
        AddFileToDownloadsTable(input);
        PrecacheGeneric(input, true);
        Format(input, sizeof(input), "materials/imgay/tf2ware_points%d.vtf", i);
        AddFileToDownloadsTable(input);
        PrecacheGeneric(input, true);
    }
    
    Format(input, sizeof(input), "materials/imgay/tf2ware_points99.vmt");
    AddFileToDownloadsTable(input);
    Format(input, sizeof(input), "materials/imgay/tf2ware_points99.vtf");
    AddFileToDownloadsTable(input);
    Format(input, sizeof(input), "materials/imgay/simon_fail.vmt");
    AddFileToDownloadsTable(input);
    Format(input, sizeof(input), "materials/imgay/simon_fail.vtf");
    AddFileToDownloadsTable(input);
    Format(input, sizeof(input), "materials/imgay/it/simon_fail.vmt");
    AddFileToDownloadsTable(input);
    Format(input, sizeof(input), "materials/imgay/it/simon_fail.vtf");
    AddFileToDownloadsTable(input);
    
    for (new i = 1; i <= sizeof(var_heavy_love); i++) {
        Format(input, sizeof(input), "sound/%s", var_heavy_love[i-1]);
        AddFileToDownloadsTable(input);
        precacheSound(var_heavy_love[i-1]);
    }
    
    KvGotoFirstSubKey(MinigameConf);
    decl id;
    new i=1;
    do {
        id = KvGetNum(MinigameConf, "id");
        new j=1, String:intros[8];
        Format(input, sizeof(input), "imgay/tf2ware/minigame_%d.mp3", id);
        precacheSound(input);
        
        Format(intros, sizeof(intros), "intro%d", j);
        while (KvJumpToKey(MinigameConf, intros)) {
            for (new k = 0; k < sizeof(var_lang); k++) {
                Format(input, sizeof(input), "materials/%s%stf2ware_minigame_%d_%d.vmt", materialpath, var_lang[k], id, j);
                AddFileToDownloadsTable(input);
                Format(input, sizeof(input), "materials/%s%stf2ware_minigame_%d_%d.vtf", materialpath, var_lang[k], id, j);
                AddFileToDownloadsTable(input);
            }
            j++;
            Format(intros, sizeof(intros), "intro%d", j);
            KvGoBack(MinigameConf);
        }
        i++;
      } while (KvGotoNextKey(MinigameConf)); 
    KvRewind(MinigameConf);
    
    white = PrecacheModel("materials/sprites/white.vmt");
    g_HaloSprite = PrecacheModel("materials/sprites/halo01.vmt");
    g_ExplosionSprite = PrecacheModel("sprites/sprite_fire01.vmt");

    PrecacheSound( "ambient/explosions/explode_8.wav", true);
    SetConVarInt(ww_speed, 1);
    ResetScores();
    bossBattle = false;
    Roundstarts = 0;
    
    if (GetConVarBool(ww_log)) LogMessage("Map started");
}

public Action:OnGetGameDescription(String:gameDesc[64]) {
    if (g_enabled) {
        Format(gameDesc, sizeof(gameDesc), "TF2Ware %s", PLUGIN_VERSION);
    }
    else
    {
        Format(gameDesc, sizeof(gameDesc), "Team Fortress");
    }
    return Plugin_Changed;
}

public OnClientConnected(client)
{
    g_welcomedisplayed[client] = false;
}

public Action:Event_ChangeClass(Handle:event, const String:name[], bool:dontBroadcast)
{
// We want the player to see the message as soon as he sees the hud (when he chooses classes for the first time).
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    if (!g_welcomedisplayed[client])
    {
        CreateTimer(2.0, Timer_DisplayWelcome, client);
        g_welcomedisplayed[client] = true;
    }
    return Plugin_Continue;
}

public Action:Timer_DisplayWelcome(Handle:timer, any:client)
{
    if (IsValidClient(client))
    {
        SetHudTextParams(-1.0,0.30,5.0,0,255,0,255,1,3.0,1.0,3.0);
        ShowHudText(client,1,"Welcome to TF2Ware %s!", PLUGIN_VERSION);
        SetHudTextParams(-1.0,0.35,5.5,255,255,0,255,1,3.0,1.5,3.0);
        ShowHudText(client, 2, "Have fun!");
    }
    return Plugin_Handled;
}

public Action:Event_Roundstart(Handle:event,const String:name[],bool:dontBroadcast) {
    if (g_enabled && GetConVarBool(ww_enable)) {
        if ( Roundstarts == 0 ) {
            g_waiting = true;
        }

        if ( Roundstarts == 1 ) {
            g_waiting = false;
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && !IsFakeClient(i) && g_Spawned[i]) StopSound(i, SND_CHANNEL_SPECIFIC, MUSIC_WAITING);
            }
            StartMinigame();
            if (GetConVarBool(ww_log)) LogMessage("Waiting-for-players period has ended");
        }
    }

    Roundstarts++;
}

public Action:Event_Roundend(Handle:event,const String:name[],bool:dontBroadcast) {
    if (g_enabled && GetConVarBool(ww_enable)) {
        g_enabled = false;
        if (GetConVarBool(ww_log)) LogMessage("== ROUND ENDED SUCCESSFULLY == ");
    }
}

public OnClientPostAdminCheck(client) {
    UpdateClientCheatValue();
    g_Points[client] = GetAverageScore();
    
    // Country
    decl String:ip[32];
    GetClientIP(client, ip, sizeof(ip));
    decl String:country[3];
    GeoipCode2(ip, country);
    g_Country[client] = 0;
    
    if (StrEqual(country, "IT")) g_Country[client] = 1;
    if (GetConVarBool(ww_log)) LogMessage("Client post admin check. Country: %d", g_Country[client]);
}

public OnClientPutInServer(client) {
   if (GetConVarBool(ww_log)) LogMessage("Client put in server and hooked");
   SDKHook(client, SDKHook_PreThink, OnPreThink);
   SDKHook(client, SDKHook_OnTakeDamage, OnTakeDamageClient);
    
}

public OnClientDisconnect(client) {
    if (GetConVarBool(ww_log)) LogMessage("Client disconnected");
    SDKUnhook(client, SDKHook_PreThink, OnPreThink);
    SDKUnhook(client, SDKHook_OnTakeDamage, OnTakeDamageClient);

    g_Spawned[client] = false;
}

public Action:OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    
    if ((g_Winner[victim] >= 1) && (status != 2)) {
        damage = 0.0;
        return Plugin_Changed;
    }
    
    if (IsValidClient(attacker) && (g_Winner[attacker] == 1) && (g_Winner[victim] == 0) && IsValidClient(victim) && IsPlayerAlive(victim)) {
        damage = 450.0;
        return Plugin_Changed;
    }
    
    return Plugin_Continue;
}

public OnPreThink(client) {
    new iButtons = GetClientButtons(client);
    if ((status != 2) && GetConVarBool(ww_enable) && g_enabled && (g_Winner[client] == 0)) {
        if ((iButtons & IN_ATTACK2) || (iButtons & IN_ATTACK)) {
        iButtons &= ~IN_ATTACK;
        iButtons &= ~IN_ATTACK2;
        SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
        }
    }
    
    if ((status == 2) && (g_attack == false) && GetConVarBool(ww_enable) && g_enabled) {
        if ((iButtons & IN_ATTACK2) || (iButtons & IN_ATTACK)) {
        iButtons &= ~IN_ATTACK;
        iButtons &= ~IN_ATTACK2;
        SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
        }
    }
}


public EventInventoryApplication(Handle:event, const String:name[], bool:dontBroadcast) {
    if (GetConVarBool(ww_log)) LogMessage("Client post inventory");
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    if (g_Spawned[client] == false && g_waiting && GetConVarBool(ww_enable) && g_enabled && !IsFakeClient(client)) {
        EmitSoundToClient(client, MUSIC_WAITING, SOUND_FROM_PLAYER, SND_CHANNEL_SPECIFIC);
    }
    g_Spawned[client] = true;
    if (GetConVarBool(ww_enable) && g_enabled) {
        // Replace huntsman with sniper rifle to avoid taunt killers
        ReplaceClientWeapon(client, 56, "tf_weapon_sniperrifle");
        if ((status != 2) && (g_Winner[client] == 0)) {
            RemoveClientWeapons(client);
            if (status != 5) CreateSprite(client);
        }
        if (status == 2) {
            Call_StartForward(g_justEnteredMinigame);
            Call_PushCell(client);
            Call_Finish();
            CreateSprite(client);
        }
        if (status == 5 && g_Winner[client] > 0) CreateSprite(client);
        SetOverlay(client, "");
        if ((status == 2 && g_attack) || (g_Winner[client] > 0)) SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 1);
        else SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 0);
    }
}

precacheSound(String:var[]) {
    new String:buffer[128];
    PrecacheSound(var, true);
    Format(buffer, sizeof(buffer), "sound/%s", var);
    AddFileToDownloadsTable(buffer);
}

public StartMinigame_cvar(Handle:cvar, const String:oldVal[], const String:newVal[]){
    if (GetConVarBool(ww_enable) && g_enabled) {
    StartMinigame();
    SetConVarInt(FindConVar("mp_respawnwavetime"), 9999);
    SetConVarInt(FindConVar("mp_forcecamera"), 0);
    }
    else {
        ServerCommand("host_timescale %f", 1.0);
        ServerCommand("phys_timescale %f", 1.0);
        ResetConVar(FindConVar("mp_respawnwavetime")); 
        ResetConVar(FindConVar("mp_forcecamera")); 
        status = 0;
    }
}

public OnGameFrame() {
    if (GetConVarBool(ww_enable) && g_enabled && (status == 2) && (g_OnGameFrame_Minigames != INVALID_HANDLE)) {
        Call_StartForward(g_OnGameFrame_Minigames);
        Call_Finish();
    }
}

public Action:StartMinigame_timer(Handle:hTimer) {
    if (status == 0) {
        StartMinigame();
    }
    return Plugin_Stop;
}

public Action:StartMinigame_timer2(Handle:hTimer) {
    if (status == 10) {
        status = 0;
        StartMinigame();
    }
    return Plugin_Stop;
}

RollMinigame() {
    // FIXME: Need to move a lot of this to the cfg file
    if (GetConVarBool(ww_log) && bossBattle == false) LogMessage("Rolling normal microgame...");
    if (GetConVarBool(ww_log) && bossBattle) LogMessage("Rolling boss microgame...");
    new Handle:roll = CreateArray();
    new bool:accept = false;
    new out = 1;
    new iplayers = GetActivePlayers();
    for (new i = 1; i <= sizeof(g_name); i++) {
        if (StrEqual(g_name[i-1], "")) continue;
        accept = true;
        new gameisboss = GetMinigameConfNum(g_name[i-1], "boss", 0);
        if (iplayers < GetMinigameConfNum(g_name[i-1], "minplayers", 2)) accept = false;
        if ((bossBattle) && (!gameisboss)) accept = false;
        if ((!bossBattle) && (gameisboss)) accept = false;
        if (i == g_lastminigame) accept = false;
        if (i == g_lastboss) accept = false;
        if (!GetMinigameConfNum(g_name[i-1], "enable", 1)) accept = false;
        if (accept) PushArrayCell(roll, i);
        if (GetConVarBool(ww_log) && (accept)) LogMessage("-- Microgame #%d allowed", i);
        if (GetConVarBool(ww_log) && (accept == false)) LogMessage("-- Microgame #%d NOT allowed", i);
    }
        
    if (GetArraySize(roll) > 0) out = GetArrayCell(roll, GetRandomInt(0, GetArraySize(roll)-1));
    CloseHandle(roll);
    
    new force = GetConVarInt(ww_force);
    if (force > 0) {
        if (force-1 < sizeof(g_name) && !StrEqual(g_name[force-1], "")) out = GetConVarInt(ww_force);
        else PrintToServer("Warning: Couldn't find a game with id %d, continuing with random roll.", GetConVarInt(ww_force));
    }
    
    if (GetConVarBool(ww_log)) LogMessage("Rolled microgame was: %s (id:%d)", g_name[out-1], out);
    
    if (GetConVarBool(ww_log)) LogMessage("Roll end");
    return out;
}

public Player_Team(Handle:event, const String:name[], bool:dontBroadcast) {
    if (GetConVarBool(ww_log)) LogMessage("Player changed team");
    if (GetConVarBool(ww_enable) && g_enabled) {
        CreateTimer(0.1, StartMinigame_timer);
    }
}

HandOutPoints() {
    if (GetConVarBool(ww_log)) LogMessage("Handing out points");
    for (new i = 1; i <= MaxClients; i++) {
        new points = 1;
        if (bossBattle) points = 5;
        if ((IsValidClient(i)) && (g_Complete[i]) && (GetClientTeam(i) >= 2) && (g_Spawned[i])) g_Points[i] += points;
        g_Complete[i] = false;
    }
}

StartMinigame() {
    if (GetConVarBool(ww_enable) && g_enabled && (status == 0) && (GetTeamClientCount(2) >= 1) && (GetTeamClientCount(3) >= 1) && g_waiting == false) {
        if (GetConVarBool(ww_log)) LogMessage("Starting microgame %s! Status = 0", minigame);
        RespawnAll();
        RemoveAllWeapons();
        SetConVarInt(FindConVar("mp_respawnwavetime"), 9999);
        
        HandOutPoints();

        currentSpeed = GetConVarInt(ww_speed);
        ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
        ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));
        
        if (GetConVarBool(ww_music)) EmitSoundToClient(1, MUSIC_START, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        else EmitSoundToAll(MUSIC_START, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                SetOverlay(i,"");
                g_Minipoints[i] = 0;
            }
        }
        
        status = 1;
        iMinigame = RollMinigame();
        minigame = g_name[iMinigame-1];
        if (bossBattle) g_lastboss = iMinigame;
        else g_lastminigame = iMinigame;
        CreateTimer(GetSpeedMultiplier(MUSIC_START_LEN), Game_Start);
        g_attack = false;
        CreateAllSprites();
        UpdateHud(GetSpeedMultiplier(MUSIC_START_LEN));
    }
}

public Action:Game_Start(Handle:hTimer) {
    if (status == 1) {
        if (GetConVarBool(ww_log)) LogMessage("Microgame %s started! Status = 1", minigame);
        new String:sound[512];
        Format(sound, sizeof(sound), "imgay/tf2ware/minigame_%d.mp3", iMinigame);
        new channel = SNDCHAN_AUTO;
        if (GetMinigameConfNum(minigame, "dynamic", 0)) channel = SND_CHANNEL_SPECIFIC;
        if (GetConVarBool(ww_music)) EmitSoundToClient(1, sound, SOUND_FROM_PLAYER, channel, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        else EmitSoundToAll(sound, SOUND_FROM_PLAYER, channel, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        SetStateAll(false);
        g_first = false;
        status = 2;
        if (GetConVarBool(ww_log)) LogMessage("- Pre part 1");
        SetMissionAll(0);
        g_attack = false;
        if (GetConVarBool(ww_log)) LogMessage("- Pre part 2");
        InitMinigame(iMinigame);
        if (GetConVarBool(ww_log)) LogMessage("- Pre-Mission");
        PrintMissionText();
        timeleft = 8;
        if (GetConVarBool(ww_log)) LogMessage("- Pre part 3");
        if (bossBattle) CreateTimer(GetSpeedMultiplier(3.0), CountDown_Timer);
        else CreateTimer(GetSpeedMultiplier(1.0), CountDown_Timer);

        microgametimer = CreateTimer(GetSpeedMultiplier(GetMinigameConfFloat(minigame, "duration")), EndGame);
        if (GetConVarBool(ww_log)) LogMessage("Microgame started post");
    }
    return Plugin_Stop;
}

PrintMissionText() {
    if (GetConVarBool(ww_log)) LogMessage("Printing mission text");
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            new String:input[512];
            Format(input, sizeof(input), "tf2ware_minigame_%d_%d", iMinigame, g_Mission[i]+1);
            SetOverlay(i,input);
        }
    }
}

public Action:CountDown_Timer(Handle:hTimer) {
    if ((status == 2) && (timeleft > 0)) {
        timeleft = timeleft - 1;
        CreateTimer(GetSpeedMultiplier(0.4), CountDown_Timer);
        if (bossBattle == false) {
            Call_StartForward(g_OnTimerMinigame);
            Call_PushCell(timeleft);
            Call_Finish();
            //OnTimerMinigame(timeleft);
        }
        if (timeleft == 3) {
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i)) SetOverlay(i,"");
            }
        }
    }
}

public Action:EndGame(Handle:hTimer) {
    if (GetConVarBool(ww_log)) LogMessage("Microgame %s, (id:%d) ended!", minigame, iMinigame);
    microgametimer = INVALID_HANDLE;
    if (status == 2) {
        Call_StartForward(g_OnAlmostEndMinigame);
        Call_Finish();

        g_attack = false;
        status = 0;
        NoCollision(false);
        
        if (GetMinigameConfNum(minigame, "endrespawn", 0) > 0) RespawnAll(true, false);
        
        Call_StartForward(g_OnEndMinigame);
        Call_Finish();
        
        new String:sound[512];
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                if (GetClientTeam(i) >= 2 && (g_Spawned[i])) {
                    decl String:event[128];
                    if (g_Complete[i]) {
                        Format(sound, sizeof(sound), MUSIC_WIN);
                        Format(event, sizeof(event), "tf2ware_complete_%d", iMinigame);
                        if (g_Achievements) mw_AchievementEvent(event, i, 0, 0, 1);
                    }
                    if (g_Complete[i] == false) {
                        Format(sound, sizeof(sound), MUSIC_FAIL);
                        Format(event, sizeof(event), "tf2ware_fail_%d", iMinigame);
                        if (g_Achievements) mw_AchievementEvent(event, i, 0, 0, 1);
                    }
                    SetEntProp(i, Prop_Send, "m_bDrawViewmodel", 0);
                }
                else {
                    Format(sound, sizeof(sound), MUSIC_WIN);
                }
                new String:oldsound[512];
                Format(oldsound, sizeof(oldsound), "imgay/tf2ware/minigame_%d.mp3", iMinigame);
                if (GetMinigameConfNum(minigame, "dynamic", 0)) StopSound(i, SND_CHANNEL_SPECIFIC, oldsound);
                EmitSoundToClient(i, sound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            }
        }
        
        
        // Clear all functions from forwards
        RemoveAllFromForward(g_justEnteredMinigame, INVALID_HANDLE);
        RemoveAllFromForward(g_OnAlmostEndMinigame, INVALID_HANDLE);
        RemoveAllFromForward(g_OnTimerMinigame, INVALID_HANDLE);
        RemoveAllFromForward(g_OnEndMinigame, INVALID_HANDLE);
        RemoveAllFromForward(g_OnGameFrame_Minigames, INVALID_HANDLE);
        RemoveAllFromForward(g_PlayerDeath, INVALID_HANDLE);
        
        RespawnAll();
        RemoveAllWeapons();
        
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsFakeClient(i)))  && (GetClientTeam(i) >= 2) && (g_Spawned[i])) {
                if (g_Complete[i]) {
                    SetOverlay(i,"tf2ware_minigame_win");
                }
                if (g_Complete[i] == false) {
                    SetOverlay(i,"tf2ware_minigame_fail");

                }
            }
        }
        UpdateHud(GetSpeedMultiplier(MUSIC_END_LEN));
        HandOutPoints();

        new bool:speedup = false;
        
        if ((GetHighestScore() >= 5) && (GetConVarInt(ww_speed) < 2) && (bossBattle == false)) speedup = true;
        if ((GetHighestScore() >= 10) && (GetConVarInt(ww_speed) < 3) && (bossBattle == false)) speedup = true;
        if ((GetHighestScore() >= 14) && (GetConVarInt(ww_speed) < 4) && (bossBattle == false)) speedup = true;
        if ((GetHighestScore() >= 18) && (GetConVarInt(ww_speed) >= 4) && (bossBattle == false)) speedup = true;
        
        if (speedup == false) {
            status = 10;
            CreateTimer(GetSpeedMultiplier(MUSIC_END_LEN), StartMinigame_timer2);
        }
        if (speedup == true) {
            status = 3;
            CreateTimer(GetSpeedMultiplier(MUSIC_END_LEN), Speedup_timer);
        }
        if (bossBattle) {
            status = 4;
            CreateTimer(GetSpeedMultiplier(MUSIC_END_LEN), Victory_timer);
        }
    }
    return Plugin_Stop;
}

public Action:Speedup_timer(Handle:hTimer) {
    if (status == 3) {
        if ((GetConVarInt(ww_speed) >= 4)  && (bossBattle == false)) {
            bossBattle = true;
            SetConVarInt(ww_speed, 1);
            currentSpeed = GetConVarInt(ww_speed);
            ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
            ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));
            CreateTimer(GetSpeedMultiplier(MUSIC_BOSS_LEN), StartMinigame_timer2);
            
            if (GetConVarBool(ww_music)) EmitSoundToClient(1, MUSIC_BOSS, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            else EmitSoundToAll(MUSIC_BOSS, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                    SetOverlay(i,"tf2ware_minigame_boss");
                }
            }
            
            UpdateHud(GetSpeedMultiplier(MUSIC_BOSS_LEN));
        }
    
        if ((GetConVarInt(ww_speed) < 4) && (bossBattle == false)) {
            if (GetConVarBool(ww_music)) EmitSoundToClient(1, MUSIC_SPEEDUP, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            else EmitSoundToAll(MUSIC_SPEEDUP, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                    SetOverlay(i,"tf2ware_minigame_speed");
                }
            }
            UpdateHud(GetSpeedMultiplier(MUSIC_SPEEDUP_LEN));
            SetConVarInt(ww_speed, GetConVarInt(ww_speed) + 1);
            CreateTimer(GetSpeedMultiplier(MUSIC_SPEEDUP_LEN), StartMinigame_timer2);
        }
        CreateAllSprites();
        status = 10;
    }
    return Plugin_Stop;
}

public Action:Victory_timer(Handle:hTimer) {
    if ((status == 4) && (bossBattle)) {
        bossBattle = false;
        SetConVarInt(ww_speed, 1);
        currentSpeed = GetConVarInt(ww_speed);
        CreateTimer(GetSpeedMultiplier(8.17), Restartall_timer);
        
        if (GetConVarBool(ww_music)) EmitSoundToClient(1, MUSIC_GAMEOVER, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        else EmitSoundToAll(MUSIC_GAMEOVER, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        
        status = 5;
        
        DestroyAllSprites();
        ResetWinners();
        
        new top = GetHighestScore();
        new winnernumber = 0;
        new Handle:ArrayWinners = CreateArray();
        decl String:winnerstring_prefix[128];
        decl String:winnerstring_names[128];
        
        for (new i = 1; i <= MaxClients; i++) {
            SetOverlay(i, "");
            if (IsValidClient(i)) {
                if (g_Points[i] >= top) {
                    g_Winner[i] = 1;
                    CreateSprite(i);
                    RespawnClient(i, true, true);
                    winnernumber += 1;
                    PushArrayCell(ArrayWinners, i);
                    if (g_Achievements) mw_AchievementEvent("tf2ware_win", i, 0, 0, 1);
                }
                if (g_Achievements) mw_AchievementEvent("tf2ware_playround", i, 0, 0, 1);
            }
        }
        for (new i = 0; i < GetArraySize(ArrayWinners); i++) {
            new client = GetArrayCell(ArrayWinners, i);
            if (winnernumber > 1) {
                if (i >= (GetArraySize(ArrayWinners)-1)) Format(winnerstring_names, sizeof(winnerstring_names), "%s and {olive}%N{green}", winnerstring_names, client);
                else Format(winnerstring_names, sizeof(winnerstring_names), "%s, {olive}%N{green}", winnerstring_names, client);
            }
            else Format(winnerstring_names, sizeof(winnerstring_names), "{olive}%N{green}", client);
        }
        if (winnernumber > 1) ReplaceStringEx(winnerstring_names, sizeof(winnerstring_names), ", ", "");
        
        if (winnernumber == 1) Format(winnerstring_prefix, sizeof(winnerstring_prefix), "{green}The winner is");
        else Format(winnerstring_prefix, sizeof(winnerstring_prefix), "{green}The winners are");
        
        CPrintToChatAll("%s %s (%i points)!", winnerstring_prefix, winnerstring_names, top);
        CloseHandle(ArrayWinners);
        
        UpdateHud(GetSpeedMultiplier(MUSIC_GAMEOVER_LEN));
        SetConVarInt(FindConVar("mp_friendlyfire"), 1);
    }
    return Plugin_Stop;
}

public Action:Restartall_timer(Handle:hTimer) {
    if (status == 5) {
        bossBattle = false;
        SetConVarInt(ww_speed, 1);
        currentSpeed = GetConVarInt(ww_speed);
        ResetScores();
        SetStateAll(false);
        status = 0;
        ResetConVar(FindConVar("mp_friendlyfire"));
        ResetWinners();
        StartMinigame();
    }
    return Plugin_Stop;
}

SetStateAll(bool:value) {
    for (new i = 1; i <= MaxClients; i++) {
        g_Complete[i] = value;
    }
}

SetMissionAll(value) {
    for (new i = 1; i <= MaxClients; i++) {
        g_Mission[i] = value;
    }
}

SetClientSlot(client, slot) {
    if (GetConVarBool(ww_log)) LogMessage("Setting client slot");
    new weapon = GetPlayerWeaponSlot(client, slot);
    SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon", weapon);
}

RespawnAll(bool:force = false, bool:savepos = true) {
    if (GetConVarBool(ww_log)) LogMessage("Respawning everyone");
    for (new i = 1; i <= MaxClients; i++) {
        RespawnClient(i, force, savepos);
    }
}

RespawnClient(any:i, bool:force = false, bool:savepos = true) {
    decl Float:pos[3];
    decl Float:vel[3];
    decl Float:ang[3];
    new alive = false;
    if (IsValidClient(i) && (g_Spawned[i] == true)) {
        if ((!(IsPlayerAlive(i))) || force) {
            alive = false;
            if (savepos) {
                GetClientAbsOrigin(i, pos);
                GetClientEyeAngles(i, ang);
                GetEntPropVector(i, Prop_Data, "m_vecVelocity", vel);
                if (IsPlayerAlive(i)) alive = true;
            }
            
            TF2_RespawnPlayer(i);
            if ((savepos) && (alive)) TeleportEntity(i, pos, ang, vel);
        }
        
        TF2_RemovePlayerDisguise(i);
    }
}

RemoveAllWeapons() {
    if (GetConVarBool(ww_log)) LogMessage("Removing all weapons");
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (IsPlayerAlive(i))) {
            RemoveClientWeapons(i);
        }
    }
}

RemoveClientWeapons(i) {
    if (GetConVarBool(ww_log)) LogMessage("Removing all client weapons");
    if (IsValidClient(i) && (IsPlayerAlive(i)) && (g_Winner[i] == 0)) {
        CreateTimer(0.0, RemoveClientWeapons_timer, i, TIMER_FLAG_NO_MAPCHANGE);
    }
}

public Action:RemoveClientWeapons_timer(Handle:hTimer, any:i) {
    if (IsValidClient(i) && (IsPlayerAlive(i)) && (g_Winner[i] == 0)) {
        SetClientSlot(i, 0);
        for (new j=1; j<=5; j++) {
            TF2_RemoveWeaponSlot(i, j);
        }
        
        new ActiveWeapon = GetEntDataEnt2(i,FindSendPropOffs("CTFPlayer", "m_hActiveWeapon"));
        if(IsValidEntity(ActiveWeapon)) {
            SetEntData(ActiveWeapon,FindSendPropOffs("CBaseCombatWeapon", "m_iClip1"),0,4);
            SetEntData(i,FindSendPropOffs("CTFPlayer", "m_iAmmo")+4,0,4);
            SetEntData(i,FindSendPropOffs("CTFPlayer", "m_iAmmo")+8,0,4);
            SetEntityRenderColor(ActiveWeapon, 255, 255, 255, 0);
            SetEntityRenderMode(ActiveWeapon, RENDER_NONE);
        }
    }
}

RemoveClientSlot(i, slot) {
    if (IsValidClient(i) && (IsPlayerAlive(i))) {
		TF2_RemoveWeaponSlot(i, slot);
    }
}

SetAllClass(String:tfclass[128]) {
    if (GetConVarBool(ww_log)) LogMessage("Setting everyone's class to %s", tfclass);
    for (new i = 1; i <= MaxClients; i++) {
            SetClientClass(i, tfclass);
    }
}

SetStateClient(client, bool:value, bool:complete=false) {
    if (IsValidClient(client)) {
        if ((complete) && (g_Complete[client] == false)) {
            EmitSoundToClient(client, SOUND_COMPLETE);
            for(new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && !IsFakeClient(i)) EmitSoundToClient(i, SOUND_COMPLETE_YOU, client);
            }
            new String:effect[128] = PARTICLE_WIN_BLUE;
            if (GetClientTeam(client) == 2) effect = PARTICLE_WIN_RED;
            ClientParticle(client, effect, 8.0);
        }
        g_Complete[client] = value;
    }
}

stock Float:GetSpeedMultiplier(Float:count) {
    new Float:divide = ((float(currentSpeed-1)/7.5)+1.0);
    new Float:speed = count / divide;
    return speed;
}

stock Float:GetHostMultiplier(Float:count) {
    new Float:divide = ((float(currentSpeed-1)/7.5)+1.0);
    new Float:speed = count * divide;
    return speed;
}

GetSoundMultiplier() {
    new speed = SNDPITCH_NORMAL + (currentSpeed-1)*10;
    return speed;
}

HookAllCheatCommands() {
    decl String:name[64];
    new Handle:cvar;
    new bool:isCommand;
    new flags;
    
    cvar = FindFirstConCommand(name, sizeof(name), isCommand, flags);
    if (cvar ==INVALID_HANDLE) {
        SetFailState("Could not load cvar list");
    }
    
    do {
        if (!isCommand || !(flags & FCVAR_CHEAT)) {
            continue;
        }
        
        RegConsoleCmd(name, OnCheatCommand);
        
    } while (FindNextConCommand(cvar, name, sizeof(name), isCommand, flags));
    
    CloseHandle(cvar);
}

UpdateClientCheatValue() {
        if (GetConVarBool(ww_log)) LogMessage("Updating client cheat value");
        for(new i = 1; i <= MaxClients; i++) {
            if(IsValidClient(i) && (!(IsFakeClient(i)))) {
                SendConVarValue(i, FindConVar("sv_cheats"), "1");
            }
        }
}

public OnConVarChanged_SvCheats(Handle:convar, const String:oldValue[], const String:newValue[]) {
    UpdateClientCheatValue();
}

public Action:OnCheatCommand(client, args) {
    if (GetConVarBool(ww_log)) LogMessage("on cheat command");
    if (GetConVarBool(ww_enable) && g_enabled) {
        decl String:command[32];
        GetCmdArg(0, command, sizeof(command));

        decl String:buf[64];
        new size = GetArraySize(ww_allowedCommands);
        for (new i=0; i<size; ++i) {
            GetArrayString(ww_allowedCommands,i, buf, sizeof(buf));
            
            if (StrEqual(buf, command, false) || GetConVarInt(FindConVar("sv_cheats")) == 1) {
                return Plugin_Continue;
            }
        }

        KickClient(client, "Attempted to use cheat command.");
        return Plugin_Handled;
    }

    return Plugin_Continue;
}

SetOverlay(i, String:overlay[512]) {
    if (IsValidClient(i) && (!(IsFakeClient(i)))) {
        new String:language[512];
        new String:input[512];
        // TRANSLATION
        Format(language, sizeof(language), "");
        
        if (g_Country[i] > 0) {
            Format(language, sizeof(language), "/%s",var_lang[g_Country[i]]);
        }
        
        if (StrEqual(overlay, "")) {
            Format(input, sizeof(input), "r_screenoverlay \"\"");
        }
        if (!(StrEqual(overlay, ""))) {
            Format(input, sizeof(input), "r_screenoverlay \"%s%s%s\"", materialpath,language,overlay);
        }

        ClientCommand(i,input);
    }
}

UpdateHud(Float:time) {
    decl String:output[512];
    decl String:add[5];
    for(new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            Format(add, sizeof(add), "");
            if (g_Complete[i] && bossBattle) Format(add, sizeof(add), "+5");
            if (g_Complete[i] && !bossBattle) Format(add, sizeof(add), "+1");
            Format(output, sizeof(output), "Points: %i %s", g_Points[i], add);
            SetHudTextParams(0.3, 0.70, time, 255, 255, 0, 0);
            ShowSyncHudText(i, hudScore, output);
        }
    }
}

public SortPlayerTimes(elem1[],elem2[],const array[][],Handle:hndl) {
    if(elem1[1] > elem2[1]) {
        return -1;
    }
    else if(elem1[1] < elem2[1]) {
        return 1;
    }

    return 0;
}  

ResetScores() {
    for(new i = 1; i <= MaxClients; i++) {
        g_Points[i] = 0;
    }
}

GetHighestScore() {
    new out = 0;
    
    for(new i = 1; i <= MaxClients; i++) {
        if (g_Points[i] > out) out = g_Points[i];
    }
    
    return out;
}

GetAverageScore() {
    new out = 0;
    new total = 0;
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (g_Points[i] > 0)) {
            out += g_Points[i];
            total += 1;
        }
    }
    
    if ((total > 0) && (out > 0)) out = out / total;
    
    return out;
}

ResetWinners() {
        for (new i = 1; i <= MaxClients; i++) {
            g_Winner[i] = 0;
        }
}

public Action:Command_points(client, args) {
    PrintToChatAll("Gave %N 20 points", client);
    g_Points[client] += 20;
    g_Points[0] += 20;
    g_Points[1] += 20;
    return Plugin_Handled;
}

RemoveNotifyFlag(String:name[128]) {
    new Handle:cv1 = FindConVar(name);
    new flags = GetConVarFlags(cv1);
    flags &= ~FCVAR_REPLICATED;
    flags &= ~FCVAR_NOTIFY;
    SetConVarFlags(cv1, flags);
}

InitMinigame(id) {
    g_respawn = false;

    GiveId();
    Call_StartFunction(INVALID_HANDLE, g_initFuncs[id-1]);
    Call_Finish();
    
    if (g_respawn == false) {
        for (new i = 1; i <= MaxClients; i++) {
            Call_StartForward(g_justEnteredMinigame);
            Call_PushCell(i);
            Call_Finish();
        }
    }
}

public Player_Death(Handle:event, const String:name[], bool:dontBroadcast) {
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    
    DestroySprite(client);
    
    if (GetConVarBool(ww_enable) && (status == 2)) {
    
        if (g_PlayerDeath != INVALID_HANDLE && IsValidClient(client)) {
            Call_StartForward(g_PlayerDeath);
            Call_PushCell(client);
            Call_Finish();
        }
    }
}

// Some convenience functions for parsing the configuration file more simply.
p_GotoGameConf(String:game[]) {
    if (!KvJumpToKey(MinigameConf, game)) {
        PrintToServer("ERROR: Couldn't find requested iMinigame %s in configuration file!", game);
        KvRewind(MinigameConf);
    }
}

GetMinigameConfStr(String:game[], String:key[], String:buffer, size) {
    p_GotoGameConf(game);
    KvGetString(MinigameConf, key, buffer, size);
    KvGoBack(MinigameConf);
}

Float:GetMinigameConfFloat(String:game[], String:key[], Float:def=4.0) {
    p_GotoGameConf(game);
    new Float:value = KvGetFloat(MinigameConf, key, def);
    KvGoBack(MinigameConf);
    return value;
}

GetMinigameConfNum(String:game[], String:key[], def=0) {
    p_GotoGameConf(game);
    new value = KvGetNum(MinigameConf, key, def);
    KvGoBack(MinigameConf);
    return value;
}