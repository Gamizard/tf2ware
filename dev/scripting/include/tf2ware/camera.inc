// The angle at which the camera is looking
new Float:g_CameraAngle[3] = {90.0, 90.0, 0.0};  

// the distance the camera is from the player.
new Float:g_CameraDistance = 400.0;

new ClientCamera[MAXPLAYERS+1]; 

// are custom cameras enabled?
new bool:g_CameraState = false;

// if the camera is strict, it won't rotate with the player.
new bool:g_CameraStrict = true;

public Action:Camera_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast) { 
    if (GetConVarBool(ww_enable) && g_enabled && g_CameraState) {
        new client = GetClientOfUserId(GetEventInt(event, "userid")); 
        CreateCamera(client); 
    }
    return Plugin_Continue; 
} 

public Action:Camera_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast) { 
    new client = GetClientOfUserId(GetEventInt(event, "userid")); 
    DestroyCamera(client); 
     
    return Plugin_Continue; 
} 

public CreateCamera(client) {   
    if (ClientCamera[client] != 0) DestroyCamera(client); 
      
    SetEntProp(client, Prop_Send, "m_iObserverMode", 1);     
     
    /* Create the Camera Entity */ 
    new entCamera = CreateEntityByName("point_viewcontrol"); 
     
    decl String:sWatcher[64]; 
    Format(sWatcher, sizeof(sWatcher), "target%i", client); 
     
    /* Sets the clients Targetname to their Index */ 
    DispatchKeyValue(client, "targetname", sWatcher); 
     
    if(IsValidEntity(entCamera)) { 
        //Name of the Camera Entity 
        DispatchKeyValue(entCamera, "targetname", "playercam"); 
         
        //Amount of time to stay active 
        DispatchKeyValue(entCamera, "wait", "3600");     
        DispatchSpawn(entCamera);
        
        // Parent it to the client
        decl Float:pos[3];
        GetClientAbsOrigin(client, pos);
        pos[2] += g_CameraDistance;
         
        TeleportEntity(entCamera, pos, g_CameraAngle, NULL_VECTOR); 
        
        //DispatchKeyValue(entCamera, "parentname", sWatcher);
        //SetVariantString(sWatcher);
        //AcceptEntityInput(entCamera, "SetParent", entCamera, entCamera, 0);
         
        SetVariantString(sWatcher); 
        AcceptEntityInput(entCamera, "Enable", client, entCamera, 0); 
         
        /* Stores the Camera index to the client */ 
        ClientCamera[client] = entCamera; 
        SDKHook(client, SDKHook_PreThink, Camera_PreThink); 
    } 
} 

stock DestroyCamera(client) { 
    if (ClientCamera[client] != 0) { 
        SetEntProp(client, Prop_Send, "m_iObserverMode", 0); 
        SetClientViewEntity(client, client); 
         
        decl String:sWatcher[64]; 
        Format(sWatcher, sizeof(sWatcher), "target%i", client); 
         
        /* We Must Disable first before removing, or it completely bugs out */ 
        SetVariantString(sWatcher); 
        AcceptEntityInput(ClientCamera[client], "Disable", client, ClientCamera[client], 0); 
         
        RemoveEdict(ClientCamera[client]); 
        ClientCamera[client] = 0;
        SDKUnhook(client, SDKHook_PreThink, Camera_PreThink);         
    }     
}  

SetCameraState(bool:enabled) {
    g_CameraState = enabled;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            if (g_CameraState) CreateCamera(i);
            else DestroyCamera(i);
        }
    }
}

public Camera_PreThink(client) { 
    if(ClientCamera[client] != 0) { 
        new Float:pos[3]; 
        GetClientAbsOrigin(client, pos); 
        pos[2] += g_CameraDistance;
        
        new Float:ang[3]; 
        GetClientEyeAngles(client, ang);
        ang[0] = g_CameraAngle[0];
        if (g_CameraStrict) {
            ang[1] = g_CameraAngle[1];
            ang[2] = g_CameraAngle[2];
        }

        TeleportEntity(ClientCamera[client], pos, ang, NULL_VECTOR); 
    }
} 