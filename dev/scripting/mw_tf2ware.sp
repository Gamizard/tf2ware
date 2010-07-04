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

#define PLUGIN_VERSION "0.7.2-14"
#define WW_START "imgay/tf2ware/warioman_intro.mp3"
#define WW_WIN "imgay/tf2ware/warioman_win.mp3"
#define WW_FAIL "imgay/tf2ware/warioman_fail.mp3"
#define WW_COMPLETE "imgay/tf2ware/complete_me.mp3"
#define WW_COMPLETE_YOU "imgay/tf2ware/complete_you.mp3"
#define WW_SPEEDUP "imgay/tf2ware/warioman_speedup.mp3"
#define WW_BOSS "imgay/tf2ware/warioman_boss.mp3"
#define WW_GAMEOVER "imgay/tf2ware/warioman_gameover.mp3"
#define WW_MINISCORE "items/pumpkin_drop.wav"
#define WW_HEAVY_KISS "vo/heavy_generic01.wav"
#define WW_WAITING "imgay/tf2ware/waitingforplayers.mp3"

#define SND_CHANNEL_SPECIFIC 32

#define PARTICLE_WIN_BLUE "teleportedin_blue"
#define PARTICLE_WIN_RED "teleportedin_red"
#define PARTICLE_BOMB "cinefx_goldrush_embers"
#define PARTICLE_EXPLODE "cinefx_goldrush_initial_smoke"

#define TF2_PLAYER_TAUNTING        (1 << 7)    // 128        Taunting

#define WW_BOMB "pl_hoodoo/alarm_clock_ticking_3.wav"
#define WW_BOMB_MODEL "models/custom/dirty_bomb_cart.mdl"

new String:var_heavy_love[][] = {"imgay/tf2ware/heavy_ilu.wav", "vo/heavy_specialcompleted08.wav", "vo/heavy_award04.wav"};


// Main intro texts
new String:var_intro1[][] = {"Hit an enemy", "Avoid the kamikaze", "Break a barrel", "Get to the end", "Needlejump", "Reach the end", "", "Airblast", "Type answer in chat", "Don't stop moving", "Get on a platform", "", "Score 7 goals", "Avoid the Cuddly Heavy", "Simon says: Taunt", "Stand on the Green", "Do the Spycrab"};

// Alternative intro texts
new String:var_intro2[][] = {"", "Explode 2 players", "", "", "", "", "", "", "", "Don't move", "", "", "", "Hug all Scouts", "Someone says: Taunt", "Avoid the Red Floor", ""};

// Language strings
new String:var_lang[][] = {"", "it/"};

// Time each microgame lasts
new Float:var_time[sizeof(var_intro1)] = {4.0, 4.0, 4.0, 4.0, 4.0, 52.5, 32.3, 4.0, 4.0, 4.0, 4.0, 32.0, 31.4, 64.8, 4.0, 99.0, 4.0};
new bool:var_boss[sizeof(var_intro1)] = {false, false, false, false, false, true, false, false, false, false, false, true, true, true, false, true, false};
new bool:var_dynamic[sizeof(var_intro1)] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false};

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

// Bools
new bool:g_Complete[MAXPLAYERS+1];
new bool:g_Spawned[MAXPLAYERS+1];
new bool:g_attack = false;
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
new minigame;
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

new g_welcomedisplayed[MAXPLAYERS+1];

// Strings
new String:materialpath[512] = "imgay/";

#include mw_tf2ware_features.inc
#include mw_tf2ware_minigames.inc

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
    if(GetExtensionFileStatus("sdkhooks.ext") < 1)
        SetFailState("SDK Hooks is not loaded.");
    
    g_offsCollisionGroup = FindSendPropOffs("CBaseEntity", "m_CollisionGroup");
    if (g_offsCollisionGroup == -1) {
        PrintToServer("* FATAL ERROR: Failed to get offset for CBaseEntity::m_CollisionGroup");
    }
    
    AddServerTag("TF2Ware");
    
    GameConf = LoadGameConfigFile("tf2ware.games");

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
    
    ww_enable = CreateConVar("ww_enable", "0", "Enables/Disables TF2 Ware.", FCVAR_PLUGIN);
    ww_force = CreateConVar("ww_force", "0", "Force a certain minigame (0 to not force).", FCVAR_PLUGIN);
    ww_speed = CreateConVar("ww_speed", "1", "Speed level.", FCVAR_PLUGIN);
    ww_music = CreateConVar("ww_music", "1", "Play music?", FCVAR_PLUGIN);
    ww_log = CreateConVar("ww_log", "0", "Log server events?", FCVAR_PLUGIN);
    HookConVarChange(ww_enable,StartMinigame_cvar);
    HookEvent("post_inventory_application", EventInventoryApplication,  EventHookMode_Post);
    HookEvent("player_death", Player_Death);
    HookEvent("player_team", Player_Team);
    HookEvent("teamplay_round_start", Event_Roundstart, EventHookMode_PostNoCopy);
    HookEvent("teamplay_game_over", Event_Roundend, EventHookMode_PostNoCopy);
    HookEvent("teamplay_round_stalemate", Event_Roundend, EventHookMode_PostNoCopy);
    HookEvent("teamplay_round_win", Event_Roundend, EventHookMode_PostNoCopy);
    
    HookEvent("player_changeclass",Event_ChangeClass);
    
    RegConsoleCmd("say", Player_Say);
    RegConsoleCmd("say_team", Player_Say);
    RegAdminCmd("ww_give", Command_points, ADMFLAG_GENERIC, "Gives you 20 points - You're a winner! (testing feature)");
    
    currentSpeed = GetConVarInt(ww_speed);
    
    minigame = 1;
    status = 0;
    randommini = 0;
    Roundstarts = 0;
    
    SetStateAll(false);
    ResetWinners();
    SetMissionAll(0);
    
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
    
    RemoveNotifyFlag("sv_tags");
    RemoveNotifyFlag("mp_respawnwavetime");
    RemoveNotifyFlag("mp_friendlyfire");
    RemoveNotifyFlag("tf_tournament_hide_domination_icons");
    SetConVarInt(FindConVar("tf_tournament_hide_domination_icons"), 0, true);
    
    if (LibraryExists("mw_ach")) g_Achievements = true;
    
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

    precacheSound(WW_START);
    precacheSound(WW_WIN);
    precacheSound(WW_FAIL);
    precacheSound(WW_COMPLETE);
    precacheSound(WW_COMPLETE_YOU);
    precacheSound(WW_SPEEDUP);
    precacheSound(WW_BOSS);
    precacheSound(WW_GAMEOVER);
    precacheSound(WW_BOMB);
    precacheSound(WW_MINISCORE);
    precacheSound(WW_WAITING);
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
    
    for (new i = 1; i <= sizeof(var_intro1); i++) {
        Format(input, sizeof(input), "imgay/tf2ware/minigame_%d.mp3", i);
        precacheSound(input);
        for (new i2 = 1; i2 <= 2; i2++) {
            if (((i2 == 1) && (!(StrEqual(var_intro1[i-1], "")))) || ((i2 == 2) && (!(StrEqual(var_intro2[i-1], ""))))) {
                for (new i3 = 0; i3 < sizeof(var_lang); i3++) {
                    Format(input, sizeof(input), "materials/%s%stf2ware_minigame_%d_%d.vmt", materialpath, var_lang[i3], i, i2);
                    AddFileToDownloadsTable(input);
                    Format(input, sizeof(input), "materials/%s%stf2ware_minigame_%d_%d.vtf", materialpath, var_lang[i3], i, i2);
                    AddFileToDownloadsTable(input);
                }
            }
        }
    }
    
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
                if (IsValidClient(i) && !IsFakeClient(i) && g_Spawned[i]) StopSound(i, SND_CHANNEL_SPECIFIC, WW_WAITING);
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
    SDKHook(client, SDKHook_Touch, OnPlayerTouch);
    
}

public OnClientDisconnect(client) {
    if (GetConVarBool(ww_log)) LogMessage("Client disconnected");
    SDKUnhook(client, SDKHook_PreThink, OnPreThink);
    SDKUnhook(client, SDKHook_OnTakeDamage, OnTakeDamageClient);
    SDKUnhook(client, SDKHook_Touch, OnPlayerTouch);
    g_Spawned[client] = false;
}

public Action:OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {    
    if ((status == 2) && (minigame == 13)) {
        if (damage > 0) damage = 1.0;
        return Plugin_Changed;
    }
    
    if ((g_Winner[victim] >= 1) && (status != 2)) {
        damage = 0.0;
        return Plugin_Changed;
    }
    
    if (IsValidClient(attacker) && (g_Winner[attacker] == 1) && (g_Winner[victim] == 0) && IsValidClient(victim) && IsPlayerAlive(victim)) {
        damage = 450.0;
        return Plugin_Changed;
    }
    
    if (GetConVarBool(ww_enable) && (IsValidClient(victim)) && (victim != attacker) && (status == 2)) {
        if ((minigame == 1) && IsValidClient(attacker)) {
            SetStateClient(attacker, true, true);
            damage = 450.0;
            return Plugin_Changed;
        }
        if ((minigame == 2) && IsValidClient(attacker) && (g_Mission[victim] == 1)) {
            SetStateClient(attacker, true, true);
        }
        if (minigame == 5 && IsValidClient(victim)) {
            decl Float:fVelocity[3];
            GetEntPropVector(victim, Prop_Data, "m_vecVelocity", fVelocity);
            fVelocity[2] -= 70.0;
            TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, fVelocity);
        }
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
    
    if ((status == 2) && (minigame == 8) && GetConVarBool(ww_enable) && g_enabled) {
        if ((iButtons & IN_ATTACK)) {
        iButtons &= ~IN_ATTACK;
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
        EmitSoundToClient(client, WW_WAITING, SOUND_FROM_PLAYER, SND_CHANNEL_SPECIFIC);
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
            justEnteredMinigame(client);
            CreateSprite(client);
        }
        if (status == 5 && g_Winner[client] > 0) CreateSprite(client);
        SetOverlay(client, "");
    }
    if ((status == 2 && g_attack) || (g_Winner[client] > 0)) SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 1);
    else SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 0);
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
    if (GetConVarBool(ww_enable) && g_enabled && (status == 2)) OnGameFrame_Minigames();
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
    if (GetConVarBool(ww_log) && bossBattle == false) LogMessage("Rolling normal microgame...");
    if (GetConVarBool(ww_log) && bossBattle) LogMessage("Rolling boss microgame...");
    new Handle:roll = CreateArray();
    new bool:accept = true;
    new out = 1;
    
    for (new i = 1; i <= sizeof(var_intro1); i++) {
        accept = true;
        if ((i == 2) && ((GetActivePlayers(2) <= 1) || (GetActivePlayers(3) <= 1))) accept = false;
        if ((bossBattle) && (var_boss[i-1] == false)) accept = false;
        if ((bossBattle == false) && (var_boss[i-1])) accept = false;
        if ((i == 12) && (GetActivePlayers() < 4)) accept = false;
        if ((i == 14) && (GetActivePlayers() < 6)) accept = false;
        if ((i == 16) && (GetActivePlayers() < 6)) accept = false;
        if (StrEqual(var_intro1[i-1], "")) accept = false;
        if (accept) PushArrayCell(roll, i);
        if (GetConVarBool(ww_log) && (accept)) LogMessage("-- Microgame %d allowed", i);
        if (GetConVarBool(ww_log) && (accept == false)) LogMessage("-- Microgame %d NOT allowed", i);
    }
        
    if (GetArraySize(roll) > 0) out = GetArrayCell(roll, GetRandomInt(0, GetArraySize(roll)-1));
    if (GetConVarBool(ww_log)) LogMessage("Rolled microgame was: %d", out);
    CloseHandle(roll);
    
    if (GetConVarInt(ww_force) > 0) out = GetConVarInt(ww_force);
    
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
        if (GetConVarBool(ww_log)) LogMessage("Starting microgame! Status = 0");
        DestroyAllBarrels();
        RespawnAll();
        RemoveAllWeapons();
        SetConVarInt(FindConVar("mp_respawnwavetime"), 9999);
        
        HandOutPoints();

        currentSpeed = GetConVarInt(ww_speed);
        ServerCommand("host_timescale %f", GetHostMultiplier(1.0));
        ServerCommand("phys_timescale %f", GetHostMultiplier(1.0));
        
        if (GetConVarBool(ww_music)) EmitSoundToAll(WW_START, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                SetOverlay(i,"");
                g_Minipoints[i] = 0;
            }
        }
        
        status = 1;
        minigame = RollMinigame();
        CreateTimer(GetSpeedMultiplier(2.1), Game_Start);
        g_attack = false;
        CreateAllSprites();
        UpdateHud(GetSpeedMultiplier(2.0));
    }
}

public Action:Game_Start(Handle:hTimer) {
    if (status == 1) {
        if (GetConVarBool(ww_log)) LogMessage("Microgame started! Status = 1");
        new String:sound[512];
        Format(sound, sizeof(sound), "imgay/tf2ware/minigame_%d.mp3", minigame);
        if (GetConVarBool(ww_music)) {
            new channel = SNDCHAN_AUTO;
            if (var_dynamic[minigame-1]) channel = SND_CHANNEL_SPECIFIC;
            EmitSoundToAll(sound, SOUND_FROM_PLAYER, channel, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        }
        SetStateAll(false);
        g_first = false;
        status = 2;
        if (GetConVarBool(ww_log)) LogMessage("- Pre-sprite creation");
        CreateAllSprites();
        if (GetConVarBool(ww_log)) LogMessage("- Pre part 1");
        SetMissionAll(0);
        g_attack = false;
        if (GetConVarBool(ww_log)) LogMessage("- Pre part 2");
        OnMinigame(minigame);
        if (GetConVarBool(ww_log)) LogMessage("- Pre-Mission");
        PrintMissionText();
        timeleft = 8;
        if (GetConVarBool(ww_log)) LogMessage("- Pre part 3");
        if (bossBattle) CreateTimer(GetSpeedMultiplier(3.0), CountDown_Timer);
        else CreateTimer(GetSpeedMultiplier(1.0), CountDown_Timer);
        microgametimer = CreateTimer(GetSpeedMultiplier(var_time[minigame-1]), EndGame);
        if (GetConVarBool(ww_log)) LogMessage("Microgame started post");
    }
    return Plugin_Stop;
}

PrintMissionText() {
    if (GetConVarBool(ww_log)) LogMessage("Printing mission text");
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            new String:input[512];
            Format(input, sizeof(input), "tf2ware_minigame_%d_%d", minigame, g_Mission[i]+1);
            SetOverlay(i,input);
        }
    }
}

public Action:CountDown_Timer(Handle:hTimer) {
    if ((status == 2) && (timeleft > 0)) {
        timeleft = timeleft - 1;
        CreateTimer(GetSpeedMultiplier(0.4), CountDown_Timer);
        if (bossBattle == false) OnTimerMinigame(timeleft);
        if (timeleft == 3) {
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i)) SetOverlay(i,"");
            }
        }
    }
}

public Action:EndGame(Handle:hTimer) {
    if (GetConVarBool(ww_log)) LogMessage("Microgame %d ended!", minigame);
    microgametimer = INVALID_HANDLE;
    if (status == 2) {
        OnAlmostEndMinigame();
        new String:sound[512];
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                if (GetClientTeam(i) >= 2 && (g_Spawned[i])) {
                    decl String:event[128];
                    if (g_Complete[i]) {
                        Format(sound, sizeof(sound), WW_WIN);
                        Format(event, sizeof(event), "tf2ware_complete_%d", minigame);
                        if (g_Achievements) mw_AchievementEvent(event, i, 0, 0, 1);
                    }
                    if (g_Complete[i] == false) {
                        Format(sound, sizeof(sound), WW_FAIL);
                        Format(event, sizeof(event), "tf2ware_fail_%d", minigame);
                        if (g_Achievements) mw_AchievementEvent(event, i, 0, 0, 1);
                    }
                    SetEntProp(i, Prop_Send, "m_bDrawViewmodel", 0);
                }
                else {
                    Format(sound, sizeof(sound), WW_WIN);
                }
                if (GetConVarBool(ww_music)) {
                    new String:oldsound[512];
                    Format(oldsound, sizeof(oldsound), "imgay/tf2ware/minigame_%d.mp3", minigame);
                    if (var_dynamic[minigame-1]) StopSound(i, SND_CHANNEL_SPECIFIC, oldsound);
                    EmitSoundToClient(i, sound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
                }
            }
        }
        g_attack = false;
        status = 0;
        CreateAllSprites();
        NoCollision(false);
        OnEndMinigame();
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
        UpdateHud(GetSpeedMultiplier(2.0));
        HandOutPoints();

        new bool:speedup = false;
        
        if ((GetHighestScore() >= 5) && (GetConVarInt(ww_speed) < 2) && (bossBattle == false)) speedup = true;
        if ((GetHighestScore() >= 10) && (GetConVarInt(ww_speed) < 3) && (bossBattle == false)) speedup = true;
        if ((GetHighestScore() >= 14) && (GetConVarInt(ww_speed) < 4) && (bossBattle == false)) speedup = true;
        if ((GetHighestScore() >= 18) && (GetConVarInt(ww_speed) >= 4) && (bossBattle == false)) speedup = true;
        
        if (speedup == false) {
            status = 10;
            CreateTimer(GetSpeedMultiplier(1.9), StartMinigame_timer2);
        }
        if (speedup == true) {
            status = 3;
            CreateTimer(GetSpeedMultiplier(1.9), Speedup_timer);
        }
        if (bossBattle) {
            status = 4;
            CreateTimer(GetSpeedMultiplier(1.9), Victory_timer);
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
            CreateTimer(GetSpeedMultiplier(4.1), StartMinigame_timer2);
            
            if (GetConVarBool(ww_music)) EmitSoundToAll(WW_BOSS, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                    SetOverlay(i,"tf2ware_minigame_boss");
                }
            }
            
            UpdateHud(GetSpeedMultiplier(4.0));
        }
    
        if ((GetConVarInt(ww_speed) < 4) && (bossBattle == false)) {
            if (GetConVarBool(ww_music)) EmitSoundToAll(WW_SPEEDUP, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && (!(IsFakeClient(i)))) {
                    SetOverlay(i,"tf2ware_minigame_speed");
                }
            }
            UpdateHud(GetSpeedMultiplier(3.7));
            SetConVarInt(ww_speed, GetConVarInt(ww_speed) + 1);
            CreateTimer(GetSpeedMultiplier(3.8), StartMinigame_timer2);
        }
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
        
        if (GetConVarBool(ww_music)) EmitSoundToAll(WW_GAMEOVER, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        
        status = 5;
        
        DestroyAllSprites();
        ResetWinners();
        
        new top = GetHighestScore();
        new winnernumber = 0;
        new Handle:ArrayWinners = CreateArray();
        decl String:winnerstring_prefix[128];
        decl String:winnerstring_names[128];
        
        for (new i = 1; i <= MaxClients; i++) {
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
        
        UpdateHud(GetSpeedMultiplier(8.17));
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
            EmitSoundToClient(client, WW_COMPLETE);
            for(new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && !IsFakeClient(i)) EmitSoundToClient(i, WW_COMPLETE_YOU, client);
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

public Action:Player_Say(iClient, iArgs)
{
    if (!(IsValidClient(iClient))) return Plugin_Continue;
    if (iArgs < 1) return Plugin_Continue;
    
    if ((IsPlayerAlive(iClient)) && (status == 2) && (minigame == 9) && (g_Complete[iClient] == false)) {
        // Retrieve the first argument and check it's a valid trigger
        decl String:strArgument[64]; GetCmdArg(1, strArgument, sizeof(strArgument));
        new String:strZero[2];
        new bool:isAnswerZero = false;
        strZero = "0";

        if(strcmp(strArgument,strZero) == 0)
        {
            isAnswerZero = true;
        }
        
        new guess = StringToInt(strArgument);
        
        if ((guess == g_result) || (g_result == 0 && isAnswerZero == true)) {
            SetHudTextParams(-1.0, 0.4, 3.0, 0,255,0, 255, 0, 6.0, 0.2, 0.5);
            // Replace the current display to include the guess instead of a question mark
            ShowHudText(iClient, 5, "%s = %d", g_mathquestion, guess);            
            SetStateClient(iClient, true, true);
            if (!(g_first)) {
                CPrintToChatAllEx(iClient, "{teamcolor}%N{green} guessed the answer first!", iClient);
                g_first = true;
            }
        }
        if (guess != g_result) {
            SetHudTextParams(-1.0, 0.4, 3.0, 255,0,0, 255, 0, 6.0, 0.2, 0.5);
            // Use a notequals sign!
            ShowHudText(iClient, 5, "%s ≠ %d", g_mathquestion, guess);
            ForcePlayerSuicide(iClient);
        }
        
        return Plugin_Handled;
    }
    
    // If no valid argument found, pass
    return Plugin_Continue;
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