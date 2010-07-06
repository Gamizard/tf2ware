// RedFloor module

// BROKEN, LIKELY GOING TO BE SCRAPPED

public RedFloor_OnMinigame() {
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, RedFloor_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, RedFloor_End);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, RedFloor_PlayerDeath);

    new mission = GetRandomInt(0,1);
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            g_Mission[i] = mission;
        }
    }
    
    SetAllClass("spy");
    g_respawn = true;
    SetStateAll(true);
    for (new i = 1; i <= MaxClients; i++) {
        g_Mission[i] = 1;
    }
    CreateTimer(22.0, PosPlayer_Minigame16_Timer);
    RespawnAll(true, false);
}

public RedFloor_justEnteredMinigame(client) {
    SetClientClass(client, "spy");
    RemoveClientWeapons(client);
    PosPlayer_Minigame16(client);
}

public RedFloor_End() {
}

public Action:PosPlayer_Minigame16_Timer(Handle:hTimer) {
    if (!StrEqual(minigame, "RedFloor")) return Plugin_Stop;
    
    if (status == 2) {
        new activeplayers = 0;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && g_Complete[i] && IsPlayerAlive(i) && g_Spawned[i] && GetClientTeam(i) >= 2) {
                activeplayers += 1;
            }
        }
            
        if (activeplayers > 3) {
            for (new i = 1; i <= MaxClients; i++) {
                if(IsValidClient(i)) {
                    PosPlayer_Minigame16(i);
                }
            }
            CreateTimer(22.0, PosPlayer_Minigame16_Timer);
        }
        else {
            TriggerTimer(microgametimer);
        }
    }
    return Plugin_Stop;
}


PosPlayer_Minigame16(client) {
    decl Float:vel[3];
    vel[0] = 0.0;
    vel[1] = 0.0;
    vel[2] = 0.0;
    
    new column = client;
    new row = 0;
    while (column > 10) {
        column = column - 10;
        row = row + 1;
    }   
    
    decl Float:pos[3];
    pos[0] = -4954.0  + float(column*55);
    pos[1] = -2186.0 - float(row*55);
    pos[2] = -344.0;

    decl Float:ang[3];
    ang[0] = 0.0;
    ang[1] = 90.0;
    ang[2] = 0.0;
    TeleportEntity(client, pos, ang, vel);
}

public RedFloor_PlayerDeath(client) {
    SetStateClient(client, false);
}