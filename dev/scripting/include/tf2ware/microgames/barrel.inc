// Barrel module

public Barrel_OnMinigame() {
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Barrel_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Barrel_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Barrel_OnGameFrame);
    
    SetAllClass("scout");
    g_attack = true;
    g_respawn = true;
    RespawnAll(true);
    
    decl Float:pos[3];
    decl Float:vel[3];

    DestroyAllBarrels();
    
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            GetClientAbsOrigin(i, pos);
            g_Barrels[i] = CreateEntityByName("prop_physics");
            
            if (IsValidEdict(g_Barrels[i])) {
                DispatchKeyValue(g_Barrels[i], "model", "models/props_farm/wooden_barrel.mdl");
                DispatchSpawn(g_Barrels[i]);
                vel[0]=GetRandomFloat(-300.0, 300.0);
                vel[1]= GetRandomFloat(-300.0, 300.0);
                vel[2] = GetRandomFloat(100.0, 300.0);
                pos[2] += 150;
                TeleportEntity(g_Barrels[i], pos, NULL_VECTOR, vel);
                SDKHook(g_Barrels[i], SDKHook_OnTakeDamage, Barrel_OnTakeDamage);
            }    
        }
    }
}

public Barrel_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            if (pos[1] > 1000.0) SetStateClient(i, true, true);
        }
    }
}

public Barrel_justEnteredMinigame(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "scout");
        RemoveClientSlot(client,0);
        RemoveClientSlot(client,1);
        SetClientSlot(client, 2);
    }
}

public Barrel_End() {
    DestroyAllBarrels();
}

DestroyAllBarrels() {
        for (new i = 1; i <= MaxClients; i++) {
            if(g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i])) {
                DestroyBarrel(g_Barrels[i]);
            }
            g_Barrels[i] = -1;
        }    
}

public Action:Barrel_OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
    for (new i = 1; i <= MaxClients; i++) {
        if(g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i]) && (g_Barrels[i] == victim)) {
            DestroyBarrel(victim);
            g_Barrels[i] = -1;
            
            if (IsValidClient(attacker) && minigame == 3) SetStateClient(attacker, true, true);
        }    
    }
    
    return Plugin_Continue;
}

DestroyBarrel(ent) {
    SDKUnhook(ent, SDKHook_OnTakeDamage, Barrel_OnTakeDamage);
    CDetach(ent);
    AcceptEntityInput(ent, "Kill" );
}