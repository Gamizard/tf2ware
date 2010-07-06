// SimonSays module

public SimonSays_OnMinigame() {
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, SimonSays_justEnteredMinigame);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, SimonSays_OnGameFrame);

    new mission = GetRandomInt(0,1);
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            g_Mission[i] = mission;
        }
    }
    
    SetStateAll(false);
    g_respawn = true;
    RespawnAll(true, true);
}

public SimonSays_justEnteredMinigame(client) {
    SetClientSlot(client, 0);
    RemoveClientWeapons(client);
}

public SimonSays_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if((IsValidClient(i) && IsPlayerAlive(i))) {
            new failed = 0;
            if((g_Mission[i] == 0) && IsPlayerTaunting(i))
            {
                SetStateClient(i, true, true);
            }
            if((g_Mission[i] >= 1))
            {
                if(IsPlayerTaunting(i))
                {
                    SetOverlay(i,"");
                    SetOverlay(i,"simon_fail");
                    SetStateClient(i, false);
                    failed = 1;
                }
                else if(failed == 0)
                {
                    SetStateClient(i, true);
                }
            }
        }
    }
}