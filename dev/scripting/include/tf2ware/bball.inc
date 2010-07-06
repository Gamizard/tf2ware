// BBall module

public BBall_OnMinigame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsClientConnected(i)) {
            SDKHook(i, SDKHook_OnTakeDamage, BBall_OnTakeDamageClient);
        }
    }

    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, BBall_justEnteredMinigame);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, BBall_End);

    SetAllClass("demoman");
    g_attack = true;
    g_respawn = true;
    RespawnAll(true, false);

}

public Action:BBall_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (status == 2) {
        if (damage > 0) damage = 1.0;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public BBall_End() {
    RespawnAll(true, false);
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_OnTakeDamage, BBall_OnTakeDamageClient);
    }
}

public OnEntityCreated(entity, const String:classname[]) {
    if (!StrEqual(g_name[minigame-1], "BBall")) return;
    if ((status == 2) && (StrEqual(classname, "tf_projectile_pipe"))) {
        SDKHook(entity, SDKHook_StartTouch, Hook_StartTouch_Pipe);
    }
}

public Action:Hook_StartTouch_Pipe(entity, other) {
    new String:classname[64];
    GetEdictClassname(other, classname, sizeof(classname));
    
    if (StrEqual(classname, "func_button") && status == 2) {
        new owner = GetEntPropEnt(entity, Prop_Send, "m_hThrower");
        if (g_Complete[owner] == false) {
            EmitSoundToClient(owner, SOUND_MINISCORE, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            g_Minipoints[owner] += 1;
            if (g_Minipoints[owner] >= 7) SetStateClient(owner, true, true);
            CDetach(entity);
            AcceptEntityInput(entity, "Kill" );
        }
    }
}

public BBall_justEnteredMinigame(client) {
    SetClientClass(client, "demoman");
    RemoveClientSlot(client,1);
    RemoveClientSlot(client,2);
    RemoveClientSlot(client,3);
    RemoveClientSlot(client,4);
    RemoveClientSlot(client,5);
    
    decl Float:vel[3];
    vel[0] = 0.0;
    vel[1] = 0.0;
    vel[2] = 0.0;
    
    new column = client;
    new row = 0;
    while (column > 17) {
        column = column - 17;
        row = row + 1;
    }
    
    decl Float:pos[3];
    pos[0] = 3076.0  + float(column*55);
    pos[1] = -1930.0 + float(row*55);
    pos[2] = -260.0;

    decl Float:ang[3];
    ang[0] = 0.0;
    ang[1] = 90.0;
    ang[2] = 0.0;
    
    TeleportEntity(client, pos, ang, vel);
}