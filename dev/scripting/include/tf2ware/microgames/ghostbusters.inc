// Ghostbusters module

new bool:Ghostbusters_pp[MAXPLAYERS+1] = false;
new Ghostbusters_target[MAXPLAYERS+1] = 0;
new Ghostbusters_hurt[MAXPLAYERS+1] = 0;
new bool:Ghostbusters_ended = false;

public Ghostbusters_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Ghostbusters_OnMapStart);
}   

public Ghostbusters_OnMapStart() {
}

public Ghostbusters_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Ghostbusters_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Ghostbusters_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Ghostbusters_OnGameFrame);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Ghostbusters_PlayerDeath);

    new AllPlayers = GetClientCount(true);
    new target = 0;
    new attempts = 0;
    new medics = GetActivePlayers() / 3;
    if (medics <= 1) medics = 2;
    if (medics >= 5) medics = 4;
    new medicclients[MAXPLAYERS+1];
    new spyclients[MAXPLAYERS+1];
    new heavyclients[MAXPLAYERS+1];
    
    // Get the medics!
    for (new i = 1; i <= medics; i++) {
        attempts = 0;
        target = GetRandomInt(1, AllPlayers);
        while (((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || IsFakeClient(target) || g_Mission[target] > 0) && attempts <= 50) {
            target = GetRandomInt(1, AllPlayers);
            attempts += 1;
        }
        if (attempts <= 50) {
            g_Mission[target] = 1;
        }
    }
    
    // Get the heavy!
    attempts = 0;
    target = GetRandomInt(1, AllPlayers);
    while (((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || g_Mission[target] > 0) && attempts <= 50) {
        target = GetRandomInt(1, AllPlayers);
        attempts += 1;
    }
    if (attempts <= 50) {
        g_Mission[target] = 2;
    }
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            if (g_Mission[i] == 0) {
                SetStateClient(i, true);
                spyclients[i-1] = i;
            }
            if (g_Mission[i] == 1) {
                SetStateClient(i, false);
                medicclients[i-1] = i;
            }
            if (g_Mission[i] == 2) {
                SetStateClient(i, true);
                heavyclients[i-1] = i;
            }
        }
    }
    
    AddRandVocalization("ghostslaugh", spyclients, 2.0, 4.0, SNDLEVEL_AIRCRAFT);
    
    g_attack = true;
    g_AlwaysShowPoints = true;
    Ghostbusters_ended = false;
    CreateAllSprites();
}

public Ghostbusters_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            if (IsPlayerAlive(i)) {
                SetEntityRenderMode(i, RENDER_NORMAL);
                SetEntityRenderColor(i, 255, 255, 255, 255);
            }
            Ghostbusters_FixSlap(i);
        }
        //SDKUnhook(i, SDKHook_Touch, Ghostbusters_OnPlayerTouch);
    }
}

new Ghostbusters_Pos[][] = {
{-1924, 9034, 426, -138},
{127, 8604, 234, -180},
{-72, 9141, 234, -117},
{-619, 8552, 234, 90},
{-859, 8904, 298, 32}
};

public Ghostbusters_justEntered(client) {
    if (IsValidClient(client)) {   
        decl Float:pos[3];
        decl Float:ang[3];
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;

        if (g_Mission[client] == 0) {
            if (GetClientTeam(client) == 2) ChangeClientTeamAlive(client, 3);
            SetClientClass(client, "spy");
            DisableClientWeapons(client);
            
            new id = Ghostbusters_GetGroupId(client, g_Mission[client]) -1;
        
            pos[0] = float(Ghostbusters_Pos[id][0]);
            pos[1] = float(Ghostbusters_Pos[id][1]);
            pos[2] = float(Ghostbusters_Pos[id][2]);
            ang[0] = 0.0;
            ang[1] = float(Ghostbusters_Pos[id][3]);
            ang[2] = 0.0;
            
            SetEntityRenderMode(client, RENDER_TRANSCOLOR);
            SetEntityRenderColor(client, 255, 255, 255, 60);
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 280.0);
        }
        if (g_Mission[client] == 1) {
            if (GetClientTeam(client) == 3) ChangeClientTeamAlive(client, 3);
            SetClientClass(client, "medic");
            RemoveClientWeapons(client);
            GiveClientWeapon(client, "tf_weapon_medigun", 1, true);
            SetClientClass(client, "medic");
            pos[0] = -1787.0 + float(Ghostbusters_GetGroupId(client, g_Mission[client]));
            pos[1] = 8389.0;
            pos[2] = 10.0;
            ang[0] = 0.0;
            ang[1] = 90.0;
            ang[2] = 0.0;
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 300.0);
        }
        if (g_Mission[client] == 2) {
            if (GetClientTeam(client) == 3) ChangeClientTeamAlive(client, 2);
            SetClientClass(client, "heavy");
            RemoveClientWeapons(client);
            GiveClientWeapon(client, "tf_weapon_fists", 2, true);
            SetClientClass(client, "heavy");
            pos[0] = -1713.0;
            pos[1] = 8738.0;
            pos[2] = 666.0;
            ang[0] = 0.0;
            ang[1] = 180.0;
            ang[2] = 0.0;
            SetEntityRenderColor(client, 0, 255, 0, 255);
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 150.0);
        }
        TeleportEntity(client, pos, ang, vel);
        Ghostbusters_pp[client] = false;
        Ghostbusters_target[client] = 0;
    }
}

Ghostbusters_GetGroupId(client, id) {
    new out = 0;
    for (new i = 1; i <= MaxClients; i++) {
        if (g_Mission[i] == id) out += 1;
        if (i == client) return out;
    }
    return 1;
}

public Ghostbusters_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) {
            if (g_Mission[i] == 1) {
                new target = GetHealtarget(i);
                if (IsValidClient(target) && IsPlayerAlive(target)) {
                    Ghostbusters_SetPP(i, true);
                    Ghostbusters_SetPP(target, true);
                    Ghostbusters_target[i] = target;
                    
                    if (Ghostbusters_hurt[target] <= 0) {
                        Ghostbusters_Slap(i);
                        Ghostbusters_Slap(target);
                    
                        new target_health = GetClientHealth(target);
                        new target_newhealth = target_health - 10;
                        
                        if (target_newhealth > 0) SetEntityHealth(target, target_newhealth);
                        else ForcePlayerSuicide(target);
                        Ghostbusters_hurt[target] = 6;
                    }
                    else Ghostbusters_hurt[target] -= 1;
                }
                else {
                    Ghostbusters_SetPP(i, false);
                    new target2 = Ghostbusters_target[i];
                    if (target2 != 0) {
                        if (IsPlayerAlive(target2)) Ghostbusters_SetPP(target2, false);
                        Ghostbusters_target[i] = 0;
                    }
                }
            }
        }
    }
}

public Ghostbusters_PlayerDeath(client) {
    SetStateClient(client, false, true);
    
    CreateTimer(0.1, Ghostbusters_Check);
}

public Action:Ghostbusters_Check(Handle:timer) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Ghostbusters")) return;
    if (Ghostbusters_ended) return;

    // Did the medics win?
    new bool:complete_medic = true;
    new bool:complete_spy = true;
    new winner = 0;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (g_Mission[i] == 0) && g_Spawned[i] && IsPlayerAlive(i)) complete_medic = false;
        if (IsValidClient(i) && (g_Mission[i] == 1) && g_Spawned[i] && IsPlayerAlive(i)) complete_spy = false;
    }
    
    if (complete_spy) winner == 2;
    if (complete_medic) winner == 1;
    if (winner > 0) {
        for (new i = 1; i <= MaxClients; i++) {
            if (winner == 1) {
                if (IsValidClient(i) && (g_Mission[i] == 1) && (IsPlayerAlive(i)) SetStateClient(i, true, true);
                if (IsValidClient(i) && (g_Mission[i] != 1)) {
                    SetStateClient(i, false, true);
                    if (IsPlayerAlive(i)) ForcePlayerSuicide(i);
                }
            }
            if (winner == 2) {
                if (IsValidClient(i) && (g_Mission[i] != 1) && (IsPlayerAlive(i)) SetStateClient(i, true, true);
                if (IsValidClient(i) && (g_Mission[i] == 1)) {
                    SetStateClient(i, false, true);
                    if (IsPlayerAlive(i)) ForcePlayerSuicide(i);
                }
            }
        }
        CreateTimer(2.0, Ghostbusters_TEnd);
    }
}

public Action:Ghostbusters_TEnd(Handle:timer) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Ghostbusters")) return;
    
    TriggerTimer(microgametimer);
}

Ghostbusters_Slap(client) {
    decl Float:fEye[3];
    GetClientEyeAngles(client, fEye);
    fEye[0] += GetRandomFloat(-10.0,10.0) * 0.15;
    fEye[1] += GetRandomFloat(-10.0,10.0) * 0.15;
    fEye[2] += GetRandomFloat(-10.0,10.0) * 0.15;
    TeleportEntity(client, NULL_VECTOR, fEye, NULL_VECTOR);
}

Ghostbusters_FixSlap(client) {
    decl Float:fEye[3];
    GetClientEyeAngles(client, fEye);
    fEye[2] = 0.0;
    TeleportEntity(client, NULL_VECTOR, fEye, NULL_VECTOR);
}

Ghostbusters_SetPP(client, bool:input) {
    if (IsValidClient(client)) {
        if (input == true && Ghostbusters_pp[client] == false) {
            TF2_SetPlayerPowerPlay(client, true);
            Ghostbusters_pp[client] = true;
            SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 400.0);
        }
        if (input == false && Ghostbusters_pp[client] == true) {
            TF2_SetPlayerPowerPlay(client, false);
            Ghostbusters_pp[client] = false;
            if (g_Mission[client] == 0) SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 280.0);
            else SetEntPropFloat(client, Prop_Send, "m_flMaxspeed", 300.0);
            Ghostbusters_FixSlap(client);
        }
    }
}