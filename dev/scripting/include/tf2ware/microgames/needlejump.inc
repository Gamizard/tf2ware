// Needlejump module

public Needlejump_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKHook(i, SDKHook_OnTakeDamage, Needlejump_OnTakeDamageClient);
        }
    }

    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Needlejump_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Needlejump_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Needlejump_OnGameFrame);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, Needlejump_TimerMinigame);
    
    SetAllClass("medic");
    SetStateAll(true);
    g_attack = true;
    g_respawn = true;
    RespawnAll(true);
}

public Needlejump_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if((IsValidClient(i) && IsPlayerAlive(i))) {
            PerformNeedleJump(i);
        }
    }
}

public Action:Needlejump_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if ((IsValidClient(victim)) && (victim != attacker) && (status == 2)) {
        if (IsValidClient(victim)) {
            decl Float:fVelocity[3];
            GetEntPropVector(victim, Prop_Data, "m_vecVelocity", fVelocity);
            fVelocity[2] -= 70.0;
            TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, fVelocity);
        }
    }
    return Plugin_Continue;
}

public Needlejump_TimerMinigame(timer) {
    if (timer == 3) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i)) {
                decl Float:pos[3];
                GetClientAbsOrigin(i, pos);
                decl Float:forigin[3];
                forigin[0] = pos[0];
                forigin[1] = pos[1];
                forigin[2] = -140.0;
                        
                PyroExplode(forigin);
                
                if (pos[2] <= -55.0) {
                    ForcePlayerSuicide(i);
                    SetStateClient(i, false);
                }
            }
        }
    }
}

public Needlejump_justEnteredMinigame(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "medic");
        RemoveClientSlot(client,1);
        RemoveClientSlot(client,2);
        RemoveClientSlot(client,3);
        RemoveClientSlot(client,4);
        RemoveClientSlot(client,5);
        g_NeedleDelay[client] = 50;
    }
}

public Needlejump_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, Needlejump_OnTakeDamageClient);
    }
}


PerformNeedleJump(i) {
    decl Float:fEyeAngle[3];
    decl Float:fVelocity[3];
    if (g_NeedleDelay[i] > 0) g_NeedleDelay[i] -= 1;
    if ((GetClientButtons(i) & IN_ATTACK) && (g_NeedleDelay[i] <= 0)){
        if(GetEntData(GetPlayerWeaponSlot(i, 0), FindSendPropInfo("CTFWeaponBase", "m_iClip1")) != 0){
            GetClientEyeAngles(i, fEyeAngle);
            GetEntPropVector(i, Prop_Data, "m_vecVelocity", fVelocity);
            fVelocity[0] += 10 * Cosine(DegToRad(fEyeAngle[1])) * -1;
            fVelocity[1] += 10 * Sine(DegToRad(fEyeAngle[1])) * -1;
            fVelocity[2] -= 40 * Sine(DegToRad(fEyeAngle[0])) * -1;
            if(FloatAbs(fVelocity[0]) > 400.0){
                if(fVelocity[0] > 0.0)
                    fVelocity[0] = 400.0;
                else
                    fVelocity[0] = -400.0;
            }
            if(FloatAbs(fVelocity[1]) > 400.0){
                if(fVelocity[1] > 0.0)
                    fVelocity[1] = 400.0;
                else
                    fVelocity[1] = -400.0;
            }
            if(fVelocity[2] > 400.0)
                fVelocity[2] = 400.0;
            TeleportEntity(i, NULL_VECTOR, NULL_VECTOR, fVelocity);
            g_NeedleDelay[i] = 3;
        }
    }
}