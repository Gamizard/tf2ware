new var_sniper_target = -1;

// NEED TO MODIFY SO THAT IT IS A SHOOTING RANGE

public SniperTarget_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, SniperTarget_OnMapStart);
}   

public SniperTarget_OnMapStart() {

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
            
            pos[0] = 3406.0;
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
            SDKHook(var_sniper_target, SDKHook_OnTakeDamage, SniperTarget_Hurt);
        }
    }
}

public SniperTarget_End() {
    if (var_sniper_target >= 0 && IsValidEntity(var_sniper_target)) {
        SDKUnhook(var_sniper_target, SDKHook_OnTakeDamage, SniperTarget_Hurt);
        AcceptEntityInput(var_sniper_target, "Kill" );
        var_sniper_target = -1;
    }
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, SniperTarget_Hurt);
    }
}

public Action:SniperTarget_Hurt(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    damage = 0.0;
    return Plugin_Changed;
}

public Action:TF2_CalcIsAttackCritical(client, weapon, String:weaponname[], &bool:result) {
    if (!StrEqual(minigame, "SniperTarget")) return Plugin_Continue;
    if (IsValidClient(client) && var_sniper_target > -1 && IsValidEntity(var_sniper_target)) {
        new iButtons = GetClientButtons(client);
        if ((iButtons & IN_ATTACK)) {
            decl Float:vecClientEyePos[3];
            decl Float:vecClientEyeAng[3];
            decl Float:vecEnd[3];
            decl Float:vecEnd2[3];
            decl Float:vecBarrel[3];
            decl Float:distance;
            GetClientEyePosition(client, vecClientEyePos); // Get the position of the player's eyes
            GetClientEyeAngles(client, vecClientEyeAng); // Get the angle the player is looking

            //Check for colliding entities
            TR_TraceRayFilter(vecClientEyePos, vecClientEyeAng, MASK_SOLID, RayType_Infinite, SniperTarget_Trace, client);

            if (TR_DidHit(INVALID_HANDLE)) {
                new TRIndex = TR_GetEntityIndex(INVALID_HANDLE);
                TR_GetEndPosition(vecEnd, INVALID_HANDLE);
                TR_GetEndPosition(vecEnd2, INVALID_HANDLE);
                if (IsValidEntity(TRIndex)) {
                    new bool:accept = false;
                    // if you hit it, bravo
                    if (TRIndex == var_sniper_target) accept = true;
                    
                    // if not, let's see if you got close enough
                    GetEntPropVector(var_sniper_target, Prop_Send, "m_vecOrigin", vecBarrel);
                    vecEnd2[1] = 0.0;
                    vecBarrel[1] = 0.0;
                    distance = GetVectorDistance(vecEnd2, vecBarrel, true) / 1000.0;
                    if (vecEnd[1] >= 2337.0 && distance <= 10.0) accept = true;
                    //PrintToChatAll("End: %f, Distance: %f", vecEnd[1], distance);
                    
                    if (accept == true) {
                        SetStateClient(client, true, true);
                        //new color[4]={188,220,255,200};
                        //TE_SetupBeamPoints(vecClientEyePos, vecEnd, g_HaloSprite, 0, 0, 0, 20.0, 3.0, 3.0, 10, 0.0, color, 0);
                        //TE_SendToAll();
                    }
                }
            }
            
        }
    }
    return Plugin_Continue;
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
        while (column > 24) {
            column = column - 24;
            row = row + 1;
        }
        
        decl Float:pos[3];
        pos[0] = 2958.0  + float(column*55);
        pos[1] = 806.0 - float(row*55);
        pos[2] = 270.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
        
        // No damage
        SDKHook(client, SDKHook_OnTakeDamage, SniperTarget_Hurt);
    }
}

public bool:SniperTarget_Trace(entity, mask, any:data) {
    if(entity == data)   // Check if the TraceRay hit the itself.
        return false;    // Don't let the entity be hit
    return true;     // It didn't hit itself
}