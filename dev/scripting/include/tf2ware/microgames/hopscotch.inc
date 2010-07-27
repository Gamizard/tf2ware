// Hopscotch module

new g_Hopscotch_arena = 0;

public Hopscotch_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Hopscotch_justEntered);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Hopscotch_OnGameFrame);
    
    g_Hopscotch_arena = GetRandomInt(1,1);
}

public Hopscotch_justEntered(client) {
    if (IsValidClient(client)) {
        if (g_Hopscotch_arena == 1) SetClientClass(client, "soldier");
        else SetClientClass(client, "heavy");
        DisableClientWeapons(client);
        if (g_Hopscotch_arena == 1) GiveClientWeapon(client, "tf_weapon_rocketlauncher", 0, true);
        
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
        
        decl Float:pos[3];
        
        if (g_Hopscotch_arena == 1) {
            pos[0] = 2460.0  + float(column*55);
            pos[1] = 3395.0 - float(row*55);
            pos[2] = -2688.0;
        }
        else {
        pos[0] = -2453.0  - float(column*55);
        pos[1] = -1833.0 - float(row*55);
        pos[2] = 17.6;
        }

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 270.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
        NoCollision(true);
    }
}

public Hopscotch_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            if (g_Hopscotch_arena == 0 && pos[1] > 2235.0 ) SetStateClient(i, true, true);
            if (g_Hopscotch_arena == 1) SetEntityHealth(i, 200);
        }
    }
}
