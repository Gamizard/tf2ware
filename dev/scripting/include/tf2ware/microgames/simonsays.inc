// SimonSays module

public SimonSays_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, SimonSays_justEntered);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, SimonSays_OnGameFrame);

    new mission = GetRandomInt(0,1);
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            g_Mission[i] = mission;
        }
    }
    SetStateAll(mission == 1);
    
    g_respawn = true;
    RespawnAll(true, true);
}

public SimonSays_justEntered(client) {
    SetClientSlot(client, 0);
    RemoveClientWeapons(client);
}

public SimonSays_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        // If the player is valid and taunting
        if((IsValidClient(i) && IsPlayerAlive(i)) && IsPlayerTaunting(i)) {
            if(g_Mission[i] == 0) {
            // If simon says it
                SetStateClient(i, true, true);
            }
            else if (g_Complete[i]) {
            // Otherwise, simon didn't say it and if we still haven't lost
                new String:input[512];
                // Display "Simon didn't say it!"
                Format(input, sizeof(input), "tf2ware_minigame_%d_3", GetMinigameConfNum("SimonSays", "id"));
                SetOverlay(i, input);
                SetStateClient(i, false);
            }
        }
    }
}