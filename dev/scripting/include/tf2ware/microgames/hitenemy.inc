// Hit Enemy module

public HitEnemy_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsClientConnected(i)) {
            SDKHook(i, SDKHook_OnTakeDamage, HitEnemy_OnTakeDamageClient);
        }
    }
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, HitEnemy_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, HitEnemy_End);

    randommini = GetRandomInt(0, 2);
    
    if (randommini <= 0) {
        SetAllClass("spy");
    }
    if (randommini == 1) {
        SetAllClass("demoman");
    }
    if (randommini >= 2) {
        SetAllClass("engineer");
    }
    g_attack = true;
    g_respawn = true;
    RespawnAll(true);

}

public HitEnemy_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsClientConnected(i)) {
            SDKUnhook(i, SDKHook_OnTakeDamage, HitEnemy_OnTakeDamageClient);
        }
    }
}

public Action:HitEnemy_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(attacker)) {
        SetStateClient(attacker, true, true);
        damage = 450.0;
    }
    return Plugin_Changed;
}


public HitEnemy_justEnteredMinigame(client) {
    if (randommini <= 0) {
        SetClientClass(client, "spy");
        RemoveClientSlot(client,1);
        RemoveClientSlot(client,2);
        RemoveClientSlot(client,3);
        RemoveClientSlot(client,4);
        RemoveClientSlot(client,5);
    }
    if (randommini == 1) {
        SetClientClass(client, "demoman");
        // Blocks anything but bottle to avoid giving certain players an advantage
        ReplaceClientWeapon(client, 132, "tf_weapon_bottle");
        ReplaceClientWeapon(client, 172, "tf_weapon_bottle");
        ReplaceClientWeapon(client, 154, "tf_weapon_bottle");
        RemoveClientSlot(client,0);
        RemoveClientSlot(client,1);
        RemoveClientSlot(client,3);
        RemoveClientSlot(client,4);
        RemoveClientSlot(client,5);
        SetClientSlot(client, 2);
    }
    if (randommini >= 2) {
        SetClientClass(client, "engineer");
        RemoveClientSlot(client,1);
        RemoveClientSlot(client,2);
        RemoveClientSlot(client,3);
        RemoveClientSlot(client,4);
        RemoveClientSlot(client,5);
    }
}