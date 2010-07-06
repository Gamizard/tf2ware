// Airblast module

public Airblast_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKHook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
        }
    }

    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Airblast_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Airblast_End);
    // Hopscotch game has the same exact check
    AddToForward(g_OnAlmostEndMinigame, INVALID_HANDLE, Hopscotch_CheckDeadPlayers);
    
    SetAllClass("pyro");
    SetStateAll(true);
    g_respawn = true;
    g_attack = true;
    RespawnAll(true, false);
}

public Airblast_justEnteredMinigame(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "pyro");
        ReplaceClientWeapon(client, 40, "tf_weapon_flamethrower");
        RemoveClientSlot(client,1);
        RemoveClientSlot(client,2);
        RemoveClientSlot(client,3);
        RemoveClientSlot(client,4);
        RemoveClientSlot(client,5);
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new total = GetClientCount(true);
        new posa = 360 / total * (g_Id[client]-1);
        
        decl Float:pos[3];
        pos[0] = 3544.661133   + (Cosine(DegToRad(float(posa)))*200.0);
        pos[1] = -160.203690  - (Sine(DegToRad(float(posa)))*200.0);
        pos[2] = -91.027405;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = float(180-posa);
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}

public Airblast_RemoveLeftClick(client) {
    new iButtons = GetClientButtons(client);
    if ((status == 2) && GetConVarBool(ww_enable) && g_enabled) {
        if ((iButtons & IN_ATTACK)) {
            iButtons &= ~IN_ATTACK;
            SetEntProp(client, Prop_Data, "m_nButtons", iButtons);
        }
    }
}

public Airblast_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
    }
}