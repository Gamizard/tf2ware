// Kamikaze module

public Kamikaze_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsClientConnected(i)) {
            SDKHook(i, SDKHook_OnTakeDamage, Kamikaze_OnTakeDamageClient);
        }
    }
    
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Kamikaze_justEnteredMinigame);
    AddToForward(g_OnAlmostEndMinigame, INVALID_HANDLE, Kamikaze_OnAlmostEndMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Kamikaze_End);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, Kamikaze_TimerMinigame);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Kamikaze_PlayerDeath);

    randommini = GetRandomInt(0, 2);
    
    new AllPlayers = GetClientCount(true);
    new target = GetRandomInt(1, AllPlayers);
    while ((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || !IsPlayerAlive(target)) {
        target = GetRandomInt(1, AllPlayers);
    }
    
    EmitSoundToClient(target, WW_BOMB, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
    ClientParticle(target, PARTICLE_BOMB, 2.0);
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            if (i == target) SetClientClass(i, "scout");
            else SetClientClass(i, "heavy");
        }
    }
    
    g_Mission[target] = 1;
    g_result = 0;
    SetStateAll(true);
    SetStateClient(target, false);

    g_respawn = true;
    RespawnAll(true);
    
}

public Kamikaze_End() {
    if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
        CDetach(g_bomb);
        AcceptEntityInput(g_bomb, "Kill" );
        g_bomb = 0;
    }
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, Kamikaze_OnTakeDamageClient);
    }
}

public Action:Kamikaze_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if ((IsValidClient(victim)) && (victim != attacker) && (status == 2) && IsValidClient(attacker)&& g_Mission[victim] == 1) {
        SetStateClient(attacker, true, true);
    }
    return Plugin_Continue;
}

public Kamikaze_OnAlmostEndMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (g_Mission[i] == 1) && (g_result >= 2)) SetStateClient(i, true);
    }
}

public Kamikaze_PlayerDeath(client) {
    if (g_Mission[client] == 0) {
        SetStateClient(client, false);
        g_result += 1;
    }
}

public Kamikaze_TimerMinigame(timer) {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && (g_Mission[i] == 1)) {
            if (timer > 2) {
                ClientParticle(i, PARTICLE_BOMB, 2.0);
            }
            if (timer == 2) {
                ClientParticle(i, PARTICLE_EXPLODE, 8.0);
                decl Float:pos[3];
                GetClientAbsOrigin(i, pos);
                        
                PyroExplode(pos);
                ExplosionDamage(pos);
                if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
                    CDetach(g_bomb);
                    AcceptEntityInput(g_bomb, "Kill" );
                    g_bomb = 0;
                }
            }
        }
    }
}

public Kamikaze_justEnteredMinigame(client) {
    if (g_Mission[client] == 0) {
        SetClientClass(client, "heavy");
        RemoveClientWeapons(client);
    }
    if (g_Mission[client] == 1) {
        SetClientClass(client, "scout");
        RemoveClientWeapons(client);
        
        if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
            AcceptEntityInput(g_bomb, "Kill" );
            g_bomb = 0;
        }
        
        g_bomb = CreateEntityByName("prop_dynamic");
        
        if (IsValidEdict(g_bomb)) {                        
            DispatchKeyValue(g_bomb, "model", WW_BOMB_MODEL);
            DispatchSpawn(g_bomb);
            SetVariantString("!activator");
            CAttach(g_bomb, client, "flag");
            ActivateEntity(g_bomb);
        }    
    }
}