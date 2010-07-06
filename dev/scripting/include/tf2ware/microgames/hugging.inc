// Hugging module

public Hugging_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKHook(i, SDKHook_Touch, Hugging_OnPlayerTouch);
        }
    }

    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Hugging_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Hugging_End);

    new AllPlayers = GetClientCount(true);
    new target = 0;
    new attempts = 0;
    new heavies = GetActivePlayers() / 4;
    if (heavies <= 1) heavies = 2;
    if (heavies >= 4) heavies = 3;
    for (new i = 1; i <= heavies; i++) {
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
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            if (g_Mission[i] == 0) {
                SetClientClass(i, "scout");
                SetStateClient(i, true);
            }
            else {
                SetClientClass(i, "heavy");
                SetStateClient(i, false);
            }
        }
    }
    g_respawn = true;
    RespawnAll(true, false);
}

public Action:Hugging_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (status == 2) {
        if (damage > 0) damage = 1.0;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public Hugging_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_Touch, Hugging_OnPlayerTouch);
    }
}

public Hugging_justEnteredMinigame(client) {
    decl Float:pos[3];
    decl Float:ang[3];
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
    if (g_Mission[client] == 0) {
        SetClientClass(client, "scout");
        if (GetClientTeam(client) == 2) {
            ChangeClientTeam(client, 3);
            RespawnClient(client, true, false);
            return;
        }
    
        RemoveClientWeapons(client);
        pos[0] = 142.0 - float(column*80);
        pos[1] = -2040.0 + float(row*80);
        pos[2] = -700.0;
        ang[0] = 0.0;
        ang[1] = -90.0;
        ang[2] = 0.0;
    }
    if (g_Mission[client] == 1) {
        SetClientClass(client, "heavy");
        if (GetClientTeam(client) == 3) {
            ChangeClientTeam(client, 2);
            RespawnClient(client, true, false);
            return;
        }
        RemoveClientWeapons(client);
        pos[0] = 142.0 - float(column*80);
        pos[1] = -2965.0 - float(row*80);
        pos[2] = -700.0;
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
    }
    TeleportEntity(client, pos, ang, vel);
    if (heavy_love[client] == INVALID_HANDLE) heavy_love[client] = CreateTimer(5.0, HeavyLoveTaunt, client);
}

public Hugging_OnPlayerTouch(entity, other) {
    if (!StrEqual(g_name[minigame-1], "Hugging")) return;

    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && (g_Mission[other] > 0 || g_Mission[entity] > 0)) {
        if (g_Mission[entity] > 0 && g_Mission[other] > 0) return;
        new attacker = entity;
        new victim = other;
        if (g_Mission[other] > 0) {
            attacker = other;
            victim = entity;
        }
        SetStateClient(victim, false);
        ForcePlayerSuicide(victim);
        EmitSoundToAll(SOUND_HEAVY_KISS, attacker);
        EmitSoundToClient(victim, SOUND_HEAVY_KISS);
        new bool:complete = true;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (g_Mission[i] == 0) && g_Spawned[i] && IsPlayerAlive(i)) complete = false;
        }
        if (complete) {
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && (g_Mission[i] == 1)) SetStateClient(i, true, true);
            }
        }
    }
}

public Action:HeavyLoveTaunt(Handle:hTimer, any:client) {
    heavy_love[client] = INVALID_HANDLE;
    if (IsValidEntity(client) && status == 2 && minigame == 14 && g_Mission[client] > 0) {
        new random = GetRandomInt(0, sizeof(var_heavy_love)-1);
        EmitSoundToAll(var_heavy_love[random], client);
    }
    new Float:timerrandom = GetRandomFloat(8.0, 13.0);
    heavy_love[client] = CreateTimer(timerrandom, HeavyLoveTaunt, client);
    return Plugin_Stop;
}