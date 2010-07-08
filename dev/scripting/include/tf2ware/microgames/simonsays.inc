// SimonSays module

#define A_TAUNT 0
#define A_JUMP 1
#define A_CROUCH 2
#define A_LOOKUP 3
#define A_LOOKDOWN 4

new String:simon_actions[5][24] = { "TAUNT!", "JUMP!", "CROUCH!", "LOOK UP!", "LOOK DOWN!" };

new simon_action;
new Handle:h_simonhud;

public SimonSays_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, SimonSays_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, SimonSays_End);

    simon_action = GetRandomInt(0,2);
    new simon = GetRandomInt(0,1);
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            g_Mission[i] = simon;
        }
    }
    
    h_simonhud = CreateHudSynchronizer();
    
    // If simon says it (simon == 0) set state to false until proven true
    // Otherwise, set to true until proven false
    SetStateAll(simon == 1);
    
    g_respawn = true;
    RespawnAll(true, true);
    
    CreateTimer(0.50, SimonSays_DisplayInstruct);
}

public Action:SimonSays_DisplayInstruct(Handle:h_timer) {
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, SimonSays_OnGameFrame);
    for (new i=1; i<=MaxClients; i++) {
        if (IsValidClient(i)) {
            SetHudTextParams(-1.0,0.30,5.0,255,255,255,0,1,3.0);
            // Needs localization (translation file would be very good).
            ShowSyncHudText(i,h_simonhud, simon_actions[simon_action]);
        }
    }
}

public SimonSays_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientSlot(client, 0);
        RemoveClientWeapons(client);
    }
}

public SimonSays_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if(IsValidClient(i)) {
            ClearSyncHud(i, h_simonhud);
        }
    }
    CloseHandle(h_simonhud);
}

public SimonSays_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        // If the player is valid and taunting
        if(IsValidClient(i) && IsPlayerAlive(i)) {
            if (IsPlayerDoing(i)) {
                if (!g_Mission[i]) {
                // If simon says it
                    SetStateClient(i, true, true);
                }
                else if (g_Complete[i]) {
                // Otherwise, simon didn't say it and if we still haven't lost
                    new String:input[512];
                    // Display "Simon didn't say it!"
                    Format(input, sizeof(input), "tf2ware_minigame_%d_3", GetMinigameConfNum("SimonSays", "id"));
                    SetOverlay(i, input);
                    ClearSyncHud(i, h_simonhud);
                    SetStateClient(i, false);
                }
            }
        }
    }
}

IsPlayerDoing(client) {
    switch (simon_action) {
        case A_TAUNT: {
            if (TF2_GetPlayerConditionFlags(client) & TF_CONDFLAG_TAUNTING) {
                return true;
            }
        }
        case A_JUMP: {
            if (GetClientButtons(client) & IN_JUMP) {
                return true;
            }
        }
        case A_CROUCH: {
            if (GetClientButtons(client) & IN_DUCK) {
                return true;
            }
        }
    }
    return false;
}