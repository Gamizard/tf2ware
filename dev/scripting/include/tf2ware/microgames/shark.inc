// TEMPLATE module

public Shark_OnMinigame() {
    // These are forwards if you need a specific function!
    AddToForward(g_justEntered, INVALID_HANDLE, TEMPLATE_justEntered);
    //AddToForward(g_OnEndMinigame, INVALID_HANDLE, TEMPLATE_End);

    // Should the person be able to attack?
    //g_attack = true;
    
    //The below function needs to be there if you're planning on giving clients a weapon!
    //RemoveAllWeapons();
}

/*
public TEMPLATE_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            // do something?
        }
    }
}
*/


public Shark_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_Touch, Shark_OnPlayerTouch);

        // Change the client's class?
        if (g_Mission[client] == 0) {
            SetClientClass(client, "scout");
            ChangeClassAlive(victim, 2);
        }
        else {
            SetClientClass(client, "heavy");
            ChangeClassAlive(victim, 3);
        }
        
        // Give the client a weapon? GiveClientWeapon(client, weapon name, slot, visible);
        //GiveClientWeapon(client, "tf_weapon_revolver", 0, true);
    }
}

public Shark_OnPlayerTouch(entity, other) {
    if (!StrEqual(minigame, "shark")) return;

    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && (g_Mission[other] > 0 || g_Mission[entity] > 0)) {
        if (g_Mission[entity] > 0 && g_Mission[other] > 0) return;
        new attacker = entity;
        new victim = other;
        if (g_Mission[other] > 0) {
            attacker = other;
            victim = entity;
        }
        SetStateClient(victim, false, true);
        g_Mission[victim] = 1;
        Shark_justEntered(victim);
        new bool:complete = true;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (g_Mission[i] == 0) && g_Spawned[i] && IsPlayerAlive(i)) complete = false;
        }
        if (complete) {
            for (new i = 1; i <= MaxClients; i++) {
                if (IsValidClient(i) && (g_Mission[i] == 1)) SetStateClient(i, true, true);
            }
        }
    }
}