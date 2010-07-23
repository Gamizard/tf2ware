new bool:Special_TwoBosses = false;

public Special_NoTouch(entity, other) {
    if (SpecialRound != 2) return;
    if (StrEqual(minigame, "Hugging")) return;

    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && GetClientTeam(entity) != GetClientTeam(other)) {
        SetStateClient(entity, false, true);
        SetStateClient(other, false, true);
        ForcePlayerSuicide(entity);
        ForcePlayerSuicide(other);
        CPrintToChatEx(entity, other, "You touched: {teamcolor}%N{default}", other);
        CPrintToChat(entity, "In this SPECIAL ROUND, you are {red}not allowed to touch anyone{default}!");
        CPrintToChatEx(other, entity, "You touched: {teamcolor}%N{default}", entity);
        CPrintToChat(other, "In this SPECIAL ROUND, you are {red}not allowed to touch anyone{default}!");
    }
}

ResetSpecialRoundEffect() {
    if (SpecialRound == 2) {
        for (new i = 1; i <= MaxClients; i++) {
            SDKUnhook(i, SDKHook_Touch, Special_NoTouch);
        }
    }
    if (SpecialRound == 4) {
        NoCollision(false);
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                SetEntityRenderColor(i, 255, 255, 255, 255);
                SetEntityRenderMode(i, RENDER_NORMAL);
            }
        }
    }
}

AddSpecialRoundEffect() {
    if (SpecialRound == 2) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) SDKHook(i, SDKHook_Touch, Special_NoTouch);
        }
    }
    if (SpecialRound == 3) Special_TwoBosses = false;
    if (SpecialRound == 4) {
        NoCollision(true);
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                SetEntityRenderColor(i, 255, 255, 255, 0);
                SetEntityRenderMode(i, RENDER_NONE);
            }
        }
    }
}