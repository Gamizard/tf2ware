//new String:var_target_models[][] = {"models/props_training/target_sniper.mdl"};

//setpos 3224.501465 2305.397461 339.531616;setang -19.439056 -0.855116 0.000000

new var_sniper_target = -1;

public SniperTarget_Init() {
}   

public SniperTarget_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, SniperTarget_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, SniperTarget_End);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, SniperTarget_TimerMinigame);

    g_attack = true;
    g_attack = true;
    RemoveAllWeapons();
}

public SniperTarget_TimerMinigame(timer) {
    if (timer == 7) {
        var_sniper_target = CreateEntityByName("prop_physics");
        
        if (IsValidEdict(var_sniper_target)) {
            decl Float:pos[3];
            decl Float:vel[3];
            
            pos[0] = 3224.0;
            pos[1] = 2305.0;
            pos[2] = 339.0;
            
            vel[0] = -9000.0;
            vel[1] = 0.0;
            vel[2] = 9000.0;
        
            decl String:model[128];
            Format(model, sizeof(model), "models/props_farm/wooden_barrel.mdl");
            DispatchKeyValue(var_sniper_target, "model", model);
            TeleportEntity(var_sniper_target, pos, NULL_VECTOR, NULL_VECTOR);
            DispatchSpawn(var_sniper_target);
            TeleportEntity(var_sniper_target, pos, NULL_VECTOR, vel);
            SDKHook(var_sniper_target, SDKHook_OnTakeDamage, Target_OnTakeDamage);
        }
    }
}

public Action:Target_OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
    if (!StrEqual(minigame, "SniperTarget")) return Plugin_Continue;
    if (IsValidClient(attacker)) SetStateClient(attacker, true, true);
    
    damage = 0.0;
    
    return Plugin_Changed;
}

public SniperTarget_End() {
    if (var_sniper_target >= 0 && IsValidEntity(var_sniper_target)) {
        AcceptEntityInput(var_sniper_target, "Kill" );
        var_sniper_target = -1;
    }
}

public SniperTarget_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "sniper");
        GiveClientWeapon(client, "tf_weapon_sniperrifle", 0, true);
        SetClientClass(client, "sniper");
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new column = client;
        new row = 0;
        
        decl Float:pos[3];
        pos[0] = 3239.0  + float(column*55);
        pos[1] = 755.0 - float(row*55);
        pos[2] = 324.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}