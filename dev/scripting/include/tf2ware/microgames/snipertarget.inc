new String:var_target_models[][] = {"models/props_training/target_sniper.mdl"};
new var_targets[MAXPLAYERS+1];

public SniperTarget_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, SniperTarget_OnMapStart);
}   

public SniperTarget_OnMapStart() {
    for (new i = 0; i < sizeof(var_target_models); i++) {
        PrecacheModel(var_target_models[i], true);
    }
}

public SniperTarget_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, SniperTarget_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, SniperTarget_End);

    SetAllClass("sniper");
    g_attack = true;
    g_respawn = true;
    RespawnAll(true);
    
    new targets = GetActivePlayers() / 3;
    if (targets <= 0) targets = 1;
    
    decl Float:pos[3];
    decl Float:vel[3];
    GetClientAbsOrigin(1, pos);
    
    for (new i = 0; i < targets; i++) {
        var_targets[i] = CreateEntityByName("prop_physics");
        
        if (IsValidEdict(var_targets[i])) {
            decl String:model[128];
            Format(model, sizeof(model), var_target_models[GetRandomInt(0, sizeof(var_target_models)-1)]);
            DispatchKeyValue(var_targets[i], "model", model);
            DispatchSpawn(var_targets[i]);
            vel[0]=GetRandomFloat(-300.0, 300.0);
            vel[1]= GetRandomFloat(-300.0, 300.0);
            vel[2] = GetRandomFloat(-300.0, 300.0);
            pos[2] += 150;
            TeleportEntity(var_targets[i], pos, NULL_VECTOR, vel);
            SetEntityMoveType(var_targets[i], MOVETYPE_NOCLIP);
            //SDKHook(var_targets[i], SDKHook_OnTakeDamage, Barrel_OnTakeDamage);
        }
    }
}

public SniperTarget_End() {
    for (new i = 0; i < sizeof(var_targets); i++) {
        if (var_targets[i] > 0 && IsValidEntity(var_targets[i])) AcceptEntityInput(var_targets[i], "Kill" );
        var_targets[i] = -1;
    }
}

public SniperTarget_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "sniper");
        RemoveClientSlot(client,1);
        RemoveClientSlot(client,2);
        RemoveClientSlot(client,3);
    }
}