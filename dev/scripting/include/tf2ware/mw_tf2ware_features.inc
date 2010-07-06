stock bool:IsValidClient(iClient) {
    if (iClient <= 0) return false;
    if (iClient > MaxClients) return false;
    if (!IsClientConnected(iClient)) return false;
    return IsClientInGame(iClient);
}

RegMinigame(String:game_name[], Function:initFunc) {
    for (new i=0; i<sizeof(g_name); i++) {
        if (StrEqual(game_name, g_name[i])) {
            PrintToServer("Registered minigame %s, id %d", game_name, i+1);
            g_initFuncs[i] = initFunc;
            break;
        }
    }
}


ReplaceClientWeapon(any:client, any:original, String:replacement[512]) {
    new edict;
    new defIdx;
    if (IsValidClient(client)) {
        for (new x=0; x<11; x++) {
            if((edict = GetPlayerWeaponSlot(client, x)) != -1) {
                defIdx = GetEntProp(edict, Prop_Send, "m_iItemDefinitionIndex");
                if (defIdx == original) {
                    RemovePlayerItem(client, edict);
                    RemoveEdict(edict);
                    new newWeapon = SDKCall(hGiveNamedItem, client, replacement, 0);
                    SDKCall(hWeaponEquip, client, newWeapon);
                }
            }
        }
    }
}

GiveId() {
    new id = 0;
    for (new client = 1; client <= MaxClients; client++) {
        if (IsValidClient(client)) {
            id += 1;
            g_Id[client] = id;
        }
    }
}

NoCollision(bool:input) {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidEntity(i) && IsValidClient(i)) {
            if (input) SetEntData(i, g_offsCollisionGroup, 2, 4, true); // NO COLLISION
            if (input == false) SetEntData(i, g_offsCollisionGroup, 5, 4, true); // BRING BACK COLLISION
        }
    }
}

PyroExplode(Float:vec1[3]) {
    new color[4]={188,220,255,200};
    EmitSoundFromOrigin("ambient/explosions/explode_8.wav", vec1);
    TE_SetupExplosion(vec1, g_ExplosionSprite, 10.0, 1, 0, 0, 750); // 600
    TE_SendToAll();
    TE_SetupBeamRingPoint(vec1, 10.0, 500.0, white, g_HaloSprite, 0, 10, 0.6, 10.0, 0.5, color, 10, 0);
    TE_SendToAll();
}

ExplosionDamage(Float:origin[3])
{
    new maxplayers = GetMaxClients();
    
    decl Float:PlayerVec[3];
    decl Float:distance;
    for (new i = 1; i <= maxplayers; i++) {
        if( !IsClientInGame(i) || !IsPlayerAlive(i) ) continue;
        GetClientAbsOrigin(i, PlayerVec);
        
        distance = GetVectorDistance(origin, PlayerVec, true);
        if(distance > 100000.0) continue;
        
        new dmg = RoundFloat(100000.0 - distance) / 50;
        new health = GetClientHealth(i) - dmg;
        
        if (health > 0) SetEntityHealth(i,health);
        if (health <= 0) ForcePlayerSuicide(i);
        
    }
}

EmitSoundFromOrigin(const String:sound[],const Float:orig[3]) {
    EmitSoundToAll(sound,SOUND_FROM_WORLD,SNDCHAN_AUTO,SNDLEVEL_NORMAL,SND_NOFLAGS,SNDVOL_NORMAL,SNDPITCH_NORMAL,-1,orig,NULL_VECTOR,true,0.0);
}

SetClientClass(i, String:tfclass[128]) {
    new TFClassType:input = TFClass_Scout;
    if (StrEqual(tfclass,"soldier")) input = TFClass_Soldier;
    if (StrEqual(tfclass,"spy")) input = TFClass_Spy;
    if (StrEqual(tfclass,"demoman")) input = TFClass_DemoMan;
    if (StrEqual(tfclass,"sniper")) input = TFClass_Sniper;
    if (StrEqual(tfclass,"medic")) input = TFClass_Medic;
    if (StrEqual(tfclass,"engineer")) input = TFClass_Engineer;
    if (StrEqual(tfclass,"heavy")) input = TFClass_Heavy;
    if (StrEqual(tfclass,"pyro")) input = TFClass_Pyro;

    if (IsValidClient(i)) {
        TF2_SetPlayerClass(i, input);
    }
}

stock CreateAllSprites() {
    for (new client = 1; client <= MaxClients; client++) {
        if (IsValidClient(client)) {
            CreateSprite(client);
        }
    }
}

stock CreateSprite(client) {
    if (IsValidClient(client)) {
        DestroySprite(client);
        if (IsPlayerAlive(client) && g_Points[client] > 0) {
            
            // Sprite name
            new points = g_Points[client];
            if (points > 19) points = 19;
            if (g_Winner[client] > 0) points = 99;
            decl String:sprite[256];
            Format(sprite, sizeof(sprite), "materials/imgay/tf2ware_points%d.vmt", points);

            new Float:vOrigin[3];
            GetClientAbsOrigin(client, vOrigin);
            
            vOrigin[2] += 100;
            
            new ent = CreateEntityByName("env_sprite");
            if (IsValidEntity(ent)) {            
                DispatchKeyValue(ent, "classname", "env_sprite");
                DispatchKeyValue(ent, "spawnflags", "1");
                DispatchKeyValue(ent, "scale", "0.2");
                DispatchKeyValue(ent, "rendermode", "1");
                DispatchKeyValue(ent, "rendercolor", "255 255 255");
                DispatchKeyValue(ent, "model", sprite);
                DispatchSpawn(ent);
                TeleportEntity(ent, vOrigin, NULL_VECTOR, NULL_VECTOR);
                CAttach(ent, client, "");
                
                g_Sprites[client] = ent;
            }
        }
    }
}

stock SetSpriteParent(ent, String:szTargetName[]) {
    SetVariantString(szTargetName);
    AcceptEntityInput(ent, "SetParent", ent, ent, 0);
    SetVariantString("flag");
    //AcceptEntityInput(ent, "SetParentAttachmentMaintainOffset", ent, ent, 0);
}

stock DestroyAllSprites() {
    for (new client = 1; client <= MaxClients; client++) {
        if (IsValidClient(client)) {
            DestroySprite(client);
        }
    }
}

stock DestroySprite(client) {
    if (IsValidClient(client)) {
        new ent = g_Sprites[client];
        if ((g_Sprites[client] > 0) && IsValidEntity(ent)) {
            CDetach(ent);
            AcceptEntityInput(ent, "kill");
            g_Sprites[client] = 0;
        }
    }
}

stock ClientParticle(client, String:effect[128], Float:time) {
    new Float:strflVec[3];
    GetClientEyePosition(client, strflVec);
    
    new strIParticle = CreateEntityByName("info_particle_system");
    new String:strName[128];
    if (IsValidEdict(strIParticle)) {
        new Float:strflPos[3];
        GetEntPropVector(client, Prop_Send, "m_vecOrigin", strflPos);
        TeleportEntity(strIParticle, strflPos, NULL_VECTOR, NULL_VECTOR);
        
        Format(strName, sizeof(strName), "target%i", client);
        DispatchKeyValue(client, "targetname", strName);
        
        DispatchKeyValue(strIParticle, "targetname", "tf2particle");
        DispatchKeyValue(strIParticle, "parentname", strName);
        DispatchKeyValue(strIParticle, "effect_name", effect);
        DispatchSpawn(strIParticle);
        SetVariantString(strName);
        AcceptEntityInput(strIParticle, "SetParent", strIParticle, strIParticle, 0);
        //SetVariantString("head");
        //AcceptEntityInput(strIParticle, "SetParentAttachment", strIParticle, strIParticle, 0);
        ActivateEntity(strIParticle);
        AcceptEntityInput(strIParticle, "start");
        
        CreateTimer(time, killprop_timer, strIParticle);
    }
}

public Action:killprop_timer(Handle:hTimer, any:prop) {
    if (IsValidEntity(prop)) {
        AcceptEntityInput(prop, "Kill" );
    }
    return Plugin_Stop;
}

stock GetActivePlayers(team = 0) {
    new output = 0;
    for (new client = 1; client <= MaxClients; client++) {
        if (IsValidClient(client) && g_Spawned[client]) {
            if ((team == 0 && GetClientTeam(client) >= 2) || (team > 0 && GetClientTeam(client) == team)) output += 1;
        }
    }
    return output;
}

stock bool:IsPlayerTaunting(client)
{
    new pcond = GetEntProp(client, Prop_Send, "m_nPlayerCond");
    if (pcond & TF2_PLAYER_TAUNTING) 
    {
        return true;
    }
    return false;
}  