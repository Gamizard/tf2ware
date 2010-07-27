// Goomba module

public Goomba_OnMinigame() {
    // These are forwards if you need a specific function!
    AddToForward(g_justEntered, INVALID_HANDLE, Goomba_justEntered);
}


public Goomba_justEntered(client) {
    if (IsValidClient(client)) {
    
        SDKHook(client, SDKHook_Touch, Goomba_Touch);

        // Change the client's class?
        SetClientClass(client, "scout");
    }
}

public Goomba_Touch(client, other) {
    decl String:edictName[32];
    GetEdictClassname(other, edictName, sizeof(edictName));

    if(StrEqual(edictName, "player") && IsValidClient(client) && IsValidClient(other) && IsPlayerAlive(client) && IsPlayerAlive(other)) {
        decl Float:ClientPos[3];
        decl Float:VictimPos[3];
        GetClientAbsOrigin(client, ClientPos);
        GetClientAbsOrigin(other, VictimPos);

        new Float:HeightDiff = ClientPos[2] - VictimPos[2];
        
        if(HeightDiff >= 81 && HeightDiff <= 84) {
            decl Float:vec[3];
            GetEntPropVector(client, Prop_Data, "m_vecAbsVelocity", vec);

            if(vec[2] < (-360.0)) {
                ForcePlayerSuicide(other);
                SetStateClient(client, true, true);
                
                decl Float:vecAng[3], Float:vecVel[3];
                GetClientEyeAngles(client, vecAng);
                GetEntPropVector(client, Prop_Data, "m_vecVelocity", vecVel);
                vecAng[0] = DegToRad(vecAng[0]);
                vecAng[1] = DegToRad(vecAng[1]);
                vecVel[0] = 300.0*Cosine(vecAng[0])*Cosine(vecAng[1]);
                vecVel[1] = 300.0*Cosine(vecAng[0])*Sine(vecAng[1]);
                vecVel[2] = 300.0+100.0;
                
                TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecVel);
            }
        }
    } 
}