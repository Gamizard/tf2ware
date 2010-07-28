// TEMPLATE module

#define FROGGER_MODEL "models/props_2fort/frog.mdl"

public Frogger_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Frogger_OnMapStart);
}

public Frogger_OnMinigame() {
    // These are forwards if you need a specific function!
    AddToForward(g_justEntered, INVALID_HANDLE, Frogger_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Frogger_End);
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Frogger_OnGameFrame);
    
    SetCameraState(true);
}

public Frogger_OnMapStart() {
    PrecacheModel(FROGGER_MODEL, true);
}


public Frogger_End() { 
    // Let's turn them back
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SetVariantString("");
            AcceptEntityInput(i, "SetCustomModel");
            SetVariantInt(0);
            AcceptEntityInput(i, "SetCustomModelRotates");
        }
    }
}

public Frogger_justEntered(client) {
    if (IsValidClient(client)) {
    
        // Change the client's class?
        SetClientClass(client, "medic");
        
        // Turn him into a frog!
        SetVariantString(FROGGER_MODEL);
        AcceptEntityInput(client, "SetCustomModel");
        SetVariantInt(1);
        AcceptEntityInput(client, "SetCustomModelRotates");
    }
}

public Frogger_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            if (pos[1] > 2235.0 && g_Complete[i] == false ) SetStateClient(i, true, true);
        }
    }
}