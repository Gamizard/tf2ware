// Spycrab module

public Spycrab_OnMinigame() {
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Spycrab_justEnteredMinigame);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, Spycrab_TimerMinigame);

    SetStateAll(true);
    SetAllClass("spy");
    g_respawn = true;
    RespawnAll(true, true);

}

public Spycrab_TimerMinigame(timer) {
    if (timer == 3) {
        new Float:min;
        new Float:ang[3];
        new duck;
        
        for (new i = 1; i <= MaxClients; i++) {
            min = -35.0;
            if (IsValidClient(i) && IsPlayerAlive(i)) {
                if (GetEntProp(i, Prop_Send, "m_nPlayerCond") & 16) SetEntProp(i, Prop_Send, "m_nPlayerCond", 0);
                SetEntPropFloat(i, Prop_Send, "m_flCloakMeter", 0.0);
                TF2_RemovePlayerDisguise(i);
                
                duck = (GetEntData(i, FindSendPropOffs("CTFPlayer", "m_fFlags")) & 2);
              
                GetClientEyeAngles(i, ang);
                if (ang[0] > min || duck == 0) {
                    ForcePlayerSuicide(i);
                    SetStateClient(i, false);
                    // Explain to the client what he should've done so he isn't left clueless.
                    // We should probably add a translation file but I have no clue how those work D:
                    if (g_Country[i] == 1) {
                    if (ang[0] > min && duck == 0) CPrintToChat(i, "{default}Le spycrab {red}si abbassano {default}e {red}guardano su{default}!");
                    else if (ang[0] > min) CPrintToChat(i, "{default}Le spycrab {default}si abbassano e {red}guardano su{default}!");
                    else CPrintToChat(i, "{default}Le spycrab {red}si abbassano{default} e guardano su!");
                    }
                    else {
                        if (ang[0] > min && duck == 0) CPrintToChat(i, "{default}Spycrabs {red}crouch {default}and {red}look up{default}!");
                        else if (ang[0] > min) CPrintToChat(i, "{default}Spycrabs {default}crouch and {red}look up{default}!");
                        else CPrintToChat(i, "{default}Spycrabs {red}crouch{default} and look up!");
                    }
                }
            }
        }                
    }
}

public Spycrab_justEnteredMinigame(client) {
    SetClientClass(client, "spy");
    RemoveClientSlot(client,0);
    RemoveClientSlot(client,1);
    RemoveClientSlot(client,2);
    RemoveClientSlot(client,4);
    RemoveClientSlot(client,5);
    SetClientSlot(client, 3);
}