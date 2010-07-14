// Hopscotch module

public Hopscotch_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Hopscotch_justEntered);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Hopscotch_OnGameFrame);
    
    SetStateAll(false);
}

public Hopscotch_justEntered(client) {
    if (IsValidClient(client)) {
        SetClientClass(client, "heavy");
        DisableClientWeapons(client);
        
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
        pos[0] = -2453.0  - float(column*55);
        pos[1] = -1833.0 - float(row*55);
        pos[2] = 17.6;

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
            if (pos[1] > 2235.0 ) SetStateClient(i, true, true);
        }
    }
}
