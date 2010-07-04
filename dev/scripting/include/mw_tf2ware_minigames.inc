//  1: Hit enemy
//  2: Kamikaze
//  3: Barrel break
//  4: Saw run
//  5: Needlejump
//  6: Heavy Hopscotch (Boss)
//  7: Heavy Skydiving (Boss) _[Scrapped]_
//  8: Airblast
//  9: Maths
// 10: Movement
// 11: Flood
// 12: Demo DodgeBall (Boss) _[Scrapped]_
// 13: Demo BBall (Boss)
// 14: Hugging Heavy (Boss)
// 15: Simon Says
// 16: Least Wins (Boss)
// 17: Spycrab

// Added these for code clarity

#define GAME_HIT_ENEMY 1
#define GAME_KAMIKAZE 2
#define GAME_BARREL_BREAK 3
#define GAME_SAW_RUN 4
#define GAME_NEEDLEJUMP 5
#define GAME_BOSS_HOPSCOTCH 6
#define GAME_AIRBLAST 8
#define GAME_MATHS 9
#define GAME_MOVEMENT 10
#define GAME_FLOOD 11
#define GAME_BOSS_DEMO_DODGEBALL 12
#define GAME_BOSS_DEMO_BBALL 13
#define GAME_BOSS_HUGGING_HEAVY 14
#define GAME_SIMON_SAYS 15
#define GAME_BOSS_LEAST_WINS 16
#define GAME_SPYCRAB 17

OnMinigame(id) {
    new bool:g_respawn = false;

    GiveId();
    
    if (id == GAME_SPYCRAB) {
        SetStateAll(true);
        SetAllClass("spy");
        g_respawn = true;
        RespawnAll(true, true);
    }

    if (id == GAME_SAW_RUN) {
        SetAllClass("scout");
        g_respawn = true;
        RespawnAll(true, false);
    }

    if (id == GAME_AIRBLAST) {
        SetAllClass("pyro");
        SetStateAll(true);
        g_respawn = true;
        g_attack = true;
        RespawnAll(true, false);
    }
    
    if (id == GAME_BOSS_HOPSCOTCH) {
        SetAllClass("heavy");
        g_respawn = true;
        SetStateAll(true);
        RespawnAll(true, false);
    }
    
    if (id == GAME_BOSS_LEAST_WINS) {
        SetAllClass("spy");
        g_respawn = true;
        SetStateAll(true);
        for (new i = 1; i <= MaxClients; i++) {
            g_Mission[i] = 1;
        }
        CreateTimer(22.0, PosPlayer_Minigame16_Timer);
        RespawnAll(true, false);
    }
    
    if ((id == GAME_BOSS_DEMO_DODGEBALL)) {
        SetAllClass("demoman");
        SetStateAll(true);
        g_attack = true;
        g_respawn = true;
        RespawnAll(true, false);
    }
    
    if ((id == GAME_BOSS_DEMO_BBALL)) {
        SetAllClass("demoman");
        g_attack = true;
        g_respawn = true;
        RespawnAll(true, false);
    }
    
    if ((id == GAME_MATHS)) {
        new random1 = GetRandomInt(3, 15);
        new random2 = GetRandomInt(3, 15);
        new form = GetRandomInt(1,3);
        new String:formString[12];
        
        if (form == 1) {
            // A guy one day kept saying 9001 to the math problems and was sad that it never was it. Take this as a 1/50 chance joke :P
            if (GetRandomInt(1, 50) == 1) {
                random1 = 9000;
                random2 = 1;
            }
            g_result = random1 + random2;
            Format(formString, sizeof(formString), "+");
        }
        if (form == 2) {
            g_result = random1 - random2;
            Format(formString, sizeof(formString), "-");
        }
        if (form == 3) {
            Format(formString, sizeof(formString), "*");
            random1 = GetRandomInt(2, 10);
            random2 = GetRandomInt(2, 10);
            g_result = random1 * random2;
        }
        
        // Save the question string so we can replace it in when the player answers
        Format(g_mathquestion, sizeof(g_mathquestion), "%d %s %d", random1, formString, random2);
        for (new i = 1; i<= MaxClients; i++) {
            if (IsValidClient(i)) {
                SetHudTextParams(-1.0, 0.4, 4.0, 255,255,255, 255, 0, 6.0, 0.2, 0.5);
                ShowHudText(i, 5, "%s = ?", g_mathquestion);
            }
        }
       // PrintCenterTextAll("%d %s %d", random1, formString, random2);
    }
    
    if ((id == GAME_MOVEMENT) || (id == GAME_SIMON_SAYS)) {
        SetStateAll(true);
        new mission = GetRandomInt(0,1);
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                g_Mission[i] = mission;
            }
        }
    }
    
    if (id == GAME_SIMON_SAYS) {
        SetStateAll(false);
        g_respawn = true;
        RespawnAll(true, true);
    }

    if (id == GAME_KAMIKAZE) {
        new AllPlayers = GetClientCount(true);
        new target = GetRandomInt(1, AllPlayers);
        while ((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || !IsPlayerAlive(target)) {
            target = GetRandomInt(1, AllPlayers);
        }
        
        EmitSoundToClient(target, WW_BOMB, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
        ClientParticle(target, PARTICLE_BOMB, 2.0);
        
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                if (i == target) SetClientClass(i, "scout");
                else SetClientClass(i, "heavy");
            }
        }
        
        g_Mission[target] = 1;
        g_result = 0;
        SetStateAll(true);
        SetStateClient(target, false);
    }
    
    if (id == GAME_BOSS_HUGGING_HEAVY) {
        new AllPlayers = GetClientCount(true);
        new target = 0;
        new attempts = 0;
        new heavies = GetActivePlayers() / 4;
        if (heavies <= 1) heavies = 2;
        if (heavies >= 4) heavies = 3;
        for (new i = 1; i <= heavies; i++) {
            attempts = 0;
            target = GetRandomInt(1, AllPlayers);
            while (((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || IsFakeClient(target) || g_Mission[target] > 0) && attempts <= 50) {
                target = GetRandomInt(1, AllPlayers);
                attempts += 1;
            }
            if (attempts <= 50) {
                g_Mission[target] = 1;
            }
        }
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                if (g_Mission[i] == 0) {
                    SetClientClass(i, "scout");
                    SetStateClient(i, true);
                }
                else {
                    SetClientClass(i, "heavy");
                    SetStateClient(i, false);
                }
            }
        }
        g_respawn = true;
        RespawnAll(true, false);
    }
    
    if (id == GAME_HIT_ENEMY) {
        randommini = GetRandomInt(0, 2);
        if (randommini <= 0) {
            SetAllClass("spy");
        }
        if (randommini == 1) {
            SetAllClass("demoman");
        }
        if (randommini >= 2) {
            SetAllClass("engineer");
        }
        g_attack = true;
    }
    
    if (id == GAME_NEEDLEJUMP) {
        SetAllClass("medic");
        SetStateAll(true);
        g_attack = true;
    }
    
    if (id == GAME_BARREL_BREAK) {
        SetAllClass("scout");
        g_attack = true;
        
        decl Float:pos[3];
        decl Float:vel[3];
    
        
        DestroyAllBarrels();
        
        for (new i = 1; i <= MaxClients; i++) {
            
            if (IsValidClient(i)) {
                
                GetClientAbsOrigin(i, pos);
                
                g_Barrels[i] = CreateEntityByName("prop_physics");
                
                if (IsValidEdict(g_Barrels[i])) {
                    DispatchKeyValue(g_Barrels[i], "model", "models/props_farm/wooden_barrel.mdl");
                    DispatchSpawn(g_Barrels[i]);
                    vel[0]=GetRandomFloat(-300.0, 300.0);
                    vel[1]= GetRandomFloat(-300.0, 300.0);
                    vel[2] = GetRandomFloat(100.0, 300.0);
                    pos[2] += 150;
                    TeleportEntity(g_Barrels[i], pos, NULL_VECTOR, vel);
                    SDKHook(g_Barrels[i], SDKHook_OnTakeDamage, OnTakeDamageBarrel);
                }    
            }
        }
    }
    
    if (id == GAME_FLOOD) {
        SetStateAll(true);
        new respawn = false;
		
        // Change the clients that are Scout into Demomen
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                new TFClassType:class = TF2_GetPlayerClass(i);
                if (class == TFClass_Scout) {
                    SetClientClass(i, "demoman");
                    respawn = true;
                }
            }
        }
        
        if (respawn) {
            RespawnAll(true, true);
            g_respawn = true;
        }
    
        decl Float:pos[3];
        new prop = CreateEntityByName("prop_physics");
        
        if (IsValidEdict(prop)) {
            DispatchKeyValue(prop, "model", "models/props_farm/wooden_barrel.mdl");
            DispatchSpawn(prop);
            pos[0] = -1237.405029;
            pos[1] = 125.300606;
            pos[2] = -58.932495;
            TeleportEntity(prop, pos, NULL_VECTOR, NULL_VECTOR);
            CreateTimer(0.1, killprop_timer, prop);
        }    
    }
    
    if ((id == GAME_HIT_ENEMY) || (id == GAME_BARREL_BREAK) || (id == GAME_NEEDLEJUMP) || (id == GAME_KAMIKAZE)) {
        g_respawn = true;
        RespawnAll(true);
    }
    
    if (g_respawn == false) {
        for (new i = 1; i <= MaxClients; i++) {
            justEnteredMinigame(i);
        }
    }
}

OnAlmostEndMinigame() {
    if (minigame == GAME_BOSS_HOPSCOTCH) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsPlayerAlive(i)))) SetStateClient(i, false);
        }
    }
    if (minigame == GAME_AIRBLAST) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (!(IsPlayerAlive(i)))) SetStateClient(i, false);
        }
    }
    if (minigame == GAME_KAMIKAZE) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (g_Mission[i] == 1) && (g_result >= 2)) SetStateClient(i, true);
        }
    }
    if (minigame == GAME_MATHS) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i)) {
                ShowHudText(i, 5, "");
            }
        }
    }
}

OnTimerMinigame(timer) {
    if ((minigame == GAME_NEEDLEJUMP) && timer == 3) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i)) {
                decl Float:pos[3];
                GetClientAbsOrigin(i, pos);
                decl Float:forigin[3];
                forigin[0] = pos[0];
                forigin[1] = pos[1];
                forigin[2] = -140.0;
                        
                PyroExplode(forigin);
                
                if (pos[2] <= -55.0) {
                    ForcePlayerSuicide(i);
                    SetStateClient(i, false);
                }
            }
        }
    }
    
    if ((minigame == GAME_KAMIKAZE)) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i) && (g_Mission[i] == 1)) {
                if (timer > 2) {
                    ClientParticle(i, PARTICLE_BOMB, 2.0);
                }
                if (timer == 2) {
                    ClientParticle(i, PARTICLE_EXPLODE, 8.0);
                    decl Float:pos[3];
                    GetClientAbsOrigin(i, pos);
                            
                    PyroExplode(pos);
                    ExplosionDamage(pos);
                    if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
                        CDetach(g_bomb);
                        AcceptEntityInput(g_bomb, "Kill" );
                        g_bomb = 0;
                    }
                }
            }
        }
    }
    
    if (minigame == GAME_SPYCRAB && (timer == 3)) {
        new Float:min;
        new Float:ang[3];
        new duck;
        
        for (new i = 1; i <= MaxClients; i++) {
            min = -35.0;
            if (IsValidClient(i) && IsPlayerAlive(i)) {
                if (GetEntProp(i, Prop_Send, "m_nPlayerCond") & 16) SetEntProp(i, Prop_Send, "m_nPlayerCond", 0);
                SetEntPropFloat(i, Prop_Send, "m_flCloakMeter", 0.0);
                TF2_RemovePlayerDisguise(i);
                
                duck = (GetEntData(i, FindSendPropOffs("CTFPlayer", "m_fFlags")) & 2);
              
                GetClientEyeAngles(i, ang);
                if (ang[0] > min || duck == 0) {
                    ForcePlayerSuicide(i);
                    SetStateClient(i, false);
                    // Explain to the client what he should've done so he isn't left clueless.
                    // We should probably add a translation file but I have no clue how those work D:
                    if (g_Country[i] == 1) {
                    if (ang[0] > min && duck == 0) CPrintToChat(i, "{default}Le spycrab {red}si abbassano {default}e {red}guardano su{default}!");
                    else if (ang[0] > min) CPrintToChat(i, "{default}Le spycrab {default}si abbassano e {red}guardano su{default}!");
                    else CPrintToChat(i, "{default}Le spycrab {red}si abbassano{default} e guardano su!");
                    }
                    else {
                        if (ang[0] > min && duck == 0) CPrintToChat(i, "{default}Spycrabs {red}crouch {default}and {red}look up{default}!");
                        else if (ang[0] > min) CPrintToChat(i, "{default}Spycrabs {default}crouch and {red}look up{default}!");
                        else CPrintToChat(i, "{default}Spycrabs {red}crouch{default} and look up!");
                    }
                }
            }
        }                
    }
}


OnEndMinigame() {
    if (minigame == GAME_BARREL_BREAK) {
        DestroyAllBarrels();
    }
    if (minigame == GAME_SAW_RUN) {
        RespawnAll(true, false);
    }
    if (minigame == GAME_BOSS_HOPSCOTCH) {
        RespawnAll(true, false);
    }
    if (minigame == GAME_AIRBLAST) {
        RespawnAll(true, false);
    }
    if (minigame == GAME_KAMIKAZE) {
        if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
            CDetach(g_bomb);
            AcceptEntityInput(g_bomb, "Kill" );
            g_bomb = 0;
        }
    }
    if (minigame == GAME_BOSS_DEMO_DODGEBALL) {
        RespawnAll(true, false);
        
        decl Float:pos[3];
        new prop = CreateEntityByName("prop_physics");
        
        if (IsValidEdict(prop)) {
            DispatchKeyValue(prop, "model", "models/props_farm/wooden_barrel.mdl");
            DispatchSpawn(prop);
            pos[0] = -1237.405029;
            pos[1] = 400.300606;
            pos[2] = -58.932495;
            TeleportEntity(prop, pos, NULL_VECTOR, NULL_VECTOR);
            CreateTimer(0.1, killprop_timer, prop);
        }
    }
    if (minigame == GAME_BOSS_DEMO_BBALL) {
        RespawnAll(true, false);
    }
    if (minigame == GAME_BOSS_HUGGING_HEAVY) {
        RespawnAll(true, false);
    }
    if (minigame == GAME_BOSS_LEAST_WINS) {
        RespawnAll(true, false);
    }
}

justEnteredMinigame(client) {
    if (IsValidClient(client)) {
        if (minigame == GAME_SPYCRAB) {
            SetClientClass(client, "spy");
            RemoveClientSlot(client,0);
            RemoveClientSlot(client,1);
            RemoveClientSlot(client,2);
            RemoveClientSlot(client,4);
            RemoveClientSlot(client,5);
            SetClientSlot(client, 3);
        }
        
        if (minigame == GAME_HIT_ENEMY) {
            if (randommini <= 0) {
                SetClientClass(client, "spy");
                RemoveClientSlot(client,1);
                RemoveClientSlot(client,2);
                RemoveClientSlot(client,3);
                RemoveClientSlot(client,4);
                RemoveClientSlot(client,5);
            }
            if (randommini == 1) {
                SetClientClass(client, "demoman");
                // Blocks anything but bottle to avoid giving certain players an advantage
                ReplaceClientWeapon(client, 132, "tf_weapon_bottle");
                ReplaceClientWeapon(client, 172, "tf_weapon_bottle");
                ReplaceClientWeapon(client, 154, "tf_weapon_bottle");
                RemoveClientSlot(client,0);
                RemoveClientSlot(client,1);
                RemoveClientSlot(client,3);
                RemoveClientSlot(client,4);
                RemoveClientSlot(client,5);
                SetClientSlot(client, 2);
            }
            if (randommini >= 2) {
                SetClientClass(client, "engineer");
                RemoveClientSlot(client,1);
                RemoveClientSlot(client,2);
                RemoveClientSlot(client,3);
                RemoveClientSlot(client,4);
                RemoveClientSlot(client,5);
            }
        }
        if (minigame == GAME_KAMIKAZE) {
            if (g_Mission[client] == 0) {
                SetClientClass(client, "heavy");
                RemoveClientWeapons(client);
            }
            if (g_Mission[client] == 1) {
                SetClientClass(client, "scout");
                RemoveClientWeapons(client);
                
                if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
                    AcceptEntityInput(g_bomb, "Kill" );
                    g_bomb = 0;
                }
                
                g_bomb = CreateEntityByName("prop_dynamic");
                
                if (IsValidEdict(g_bomb)) {                        
                    DispatchKeyValue(g_bomb, "model", WW_BOMB_MODEL);
                    DispatchSpawn(g_bomb);
                    SetVariantString("!activator");
                    CAttach(g_bomb, client, "flag");
                    ActivateEntity(g_bomb);
                }    
            }
        }
        if (minigame == GAME_BOSS_HUGGING_HEAVY) {
            decl Float:pos[3];
            decl Float:ang[3];
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
            if (g_Mission[client] == 0) {
                SetClientClass(client, "scout");
                if (GetClientTeam(client) == 2) {
                    ChangeClientTeam(client, 3);
                    RespawnClient(client, true, false);
                    return;
                }
            
                RemoveClientWeapons(client);
                pos[0] = 142.0 - float(column*80);
                pos[1] = -2040.0 + float(row*80);
                pos[2] = -700.0;
                ang[0] = 0.0;
                ang[1] = -90.0;
                ang[2] = 0.0;
            }
            if (g_Mission[client] == 1) {
                SetClientClass(client, "heavy");
                if (GetClientTeam(client) == 3) {
                    ChangeClientTeam(client, 2);
                    RespawnClient(client, true, false);
                    return;
                }
                RemoveClientWeapons(client);
                pos[0] = 142.0 - float(column*80);
                pos[1] = -2965.0 - float(row*80);
                pos[2] = -700.0;
                ang[0] = 0.0;
                ang[1] = 90.0;
                ang[2] = 0.0;
            }
            TeleportEntity(client, pos, ang, vel);
            if (heavy_love[client] == INVALID_HANDLE) heavy_love[client] = CreateTimer(5.0, HeavyLoveTaunt, client);
        }
        
        if (minigame == GAME_BARREL_BREAK) {
            SetClientClass(client, "scout");
            RemoveClientSlot(client,0);
            RemoveClientSlot(client,1);
            SetClientSlot(client, 2);
        }
        
        if (minigame == GAME_SAW_RUN) {
            SetClientClass(client, "scout");
            RemoveClientWeapons(client);
            
            decl Float:vel[3];
            vel[0] = 0.0;
            vel[1] = 0.0;
            vel[2] = 0.0;
            
            new column = client;
            new row = 0;
            while (column > 9) {
                column = column - 9;
                row = row + 1;
            }
            
            
            decl Float:pos[3];
            pos[0] = 1780.0 + float(column*55);
            pos[1] = 290.0 - float(row*55);
            pos[2] = 130.0;

            decl Float:ang[3];
            ang[0] = 27.0;
            ang[1] = 90.0;
            ang[2] = 0.0;
            TeleportEntity(client, pos, ang, vel);
            NoCollision(true);
        }
        if (minigame == GAME_NEEDLEJUMP) {
            SetClientClass(client, "medic");
            RemoveClientSlot(client,1);
            RemoveClientSlot(client,2);
            RemoveClientSlot(client,3);
            RemoveClientSlot(client,4);
            RemoveClientSlot(client,5);
            g_NeedleDelay[client] = 50;
        }
        
        if (minigame == GAME_BOSS_HOPSCOTCH) {
            SetClientClass(client, "heavy");
            RemoveClientWeapons(client);
            
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
        if (minigame == GAME_AIRBLAST) {
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
        if (minigame == GAME_BOSS_DEMO_DODGEBALL) {
            SetClientClass(client, "demoman");
            SetClientSlot(client, 2);
            RemoveClientSlot(client,0);
            RemoveClientSlot(client,1);
            RemoveClientSlot(client,3);
            RemoveClientSlot(client,4);
            RemoveClientSlot(client,5);
            
            decl Float:vel[3];
            vel[0] = 0.0;
            vel[1] = 0.0;
            vel[2] = 0.0;
            
            new column = client;
            
            decl Float:pos[3];
            pos[0] = 2351.0  - float(column*55);
            pos[1] = -546.0;
            pos[2] = -260.0;

            decl Float:ang[3];
            ang[0] = 0.0;
            ang[1] = -90.0;
            ang[2] = 0.0;
            
            if (GetClientTeam(client) == 3) {
                pos[1] = -2217.0;
                ang[1] = 90.0;
            }
            
            TeleportEntity(client, pos, ang, vel);
            NoCollision(true);
        }
        
        if (minigame == GAME_BOSS_DEMO_BBALL) {
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
        
        if (minigame == GAME_SIMON_SAYS) {
            SetClientSlot(client, 0);
            RemoveClientWeapons(client);
        }
        
        if (minigame == GAME_BOSS_LEAST_WINS) {
            SetClientClass(client, "spy");
            RemoveClientWeapons(client);
            PosPlayer_Minigame16(client);
        }
    }
}

public Action:PosPlayer_Minigame16_Timer(Handle:hTimer) {
    if (status == 2 && minigame == GAME_BOSS_LEAST_WINS) {
        new activeplayers = 0;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && g_Complete[i] && IsPlayerAlive(i) && g_Spawned[i] && GetClientTeam(i) >= 2) {
                activeplayers += 1;
            }
        }
            
        if (activeplayers > 3) {
            for (new i = 1; i <= MaxClients; i++) {
                if(IsValidClient(i)) {
                    PosPlayer_Minigame16(i);
                }
            }
            CreateTimer(22.0, PosPlayer_Minigame16_Timer);
        }
        else {
            TriggerTimer(microgametimer);
        }
    }
    return Plugin_Stop;
}

PosPlayer_Minigame16(client) {
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
    pos[0] = -4954.0  + float(column*55);
    pos[1] = -2186.0 - float(row*55);
    pos[2] = -344.0;

    decl Float:ang[3];
    ang[0] = 0.0;
    ang[1] = 90.0;
    ang[2] = 0.0;
    TeleportEntity(client, pos, ang, vel);
}

DestroyAllBarrels() {
        for (new i = 1; i <= MaxClients; i++) {
            if(g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i])) {
                DestroyBarrel(g_Barrels[i]);
            }
            g_Barrels[i] = -1;
        }    
}

DestroyBarrel(ent) {
    SDKUnhook(ent, SDKHook_OnTakeDamage, OnTakeDamageBarrel);
    CDetach(ent);
    AcceptEntityInput(ent, "Kill" );
}

public Action:OnTakeDamageBarrel(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
    for (new i = 1; i <= MaxClients; i++) {
        if(g_Barrels[i] > 0 && IsValidEntity(g_Barrels[i]) && (g_Barrels[i] == victim)) {
            DestroyBarrel(victim);
            g_Barrels[i] = -1;
            
            if (IsValidClient(attacker) && minigame == 3) SetStateClient(attacker, true, true);
        }    
    }
    
    return Plugin_Continue;
}

public Player_Death(Handle:event, const String:name[], bool:dontBroadcast) {
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    
    DestroySprite(client);
    
    if (GetConVarBool(ww_enable) && (status == 2)) {
        if ((minigame == GAME_KAMIKAZE) && IsValidClient(client) && (g_Mission[client] == 0)) {
            SetStateClient(client, false);
            g_result += 1;
        }
        
        if ((minigame == GAME_FLOOD) && IsValidClient(client)) {
            SetStateClient(client, false);
        }
        
        if ((minigame == GAME_BOSS_DEMO_DODGEBALL) && IsValidClient(client)) {
            SetStateClient(client, false);
        }
        if ((minigame == GAME_BOSS_LEAST_WINS) && IsValidClient(client)) {
            SetStateClient(client, false);
        }
    }
}

OnGameFrame_Minigames() {
    if (minigame == GAME_SAW_RUN) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i)) {
                if (minigame == GAME_SAW_RUN) {
                    decl Float:pos[3];
                    GetClientAbsOrigin(i, pos);
                    if (pos[1] > 1000.0) SetStateClient(i, true, true);
                }
            }
        }
    }
    
    if (minigame == GAME_NEEDLEJUMP) {
        for (new i = 1; i <= MaxClients; i++) {
            if((IsValidClient(i) && IsPlayerAlive(i))) {
                PerformNeedleJump(i);
            }
        }
    }
    
    if ((minigame == GAME_MOVEMENT) && (timeleft <= 5)) {
        decl Float:fVelocity[3];
        decl Float:speed;
        decl Float:limit;
        speed = 0.0;
        limit = 0.0;
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && IsPlayerAlive(i)) {
                limit = GetEntPropFloat(i, Prop_Send, "m_flMaxspeed") - 100.0;
                GetEntPropVector(i, Prop_Data, "m_vecVelocity", fVelocity);
                speed = (FloatAbs(fVelocity[0]) + FloatAbs(fVelocity[1]) + FloatAbs(fVelocity[2]));
                if ((g_Mission[i] == 0) && (speed < limit) && (g_Complete[i]) && (IsPlayerAlive(i))) {
                    SetStateClient(i, false);
                    ForcePlayerSuicide(i);
                }
                if ((g_Mission[i] >= 1) && (speed > 100.0) && (g_Complete[i]) && (IsPlayerAlive(i))) {
                    SetStateClient(i, false);
                    ForcePlayerSuicide(i);
                }
            }
        }
    }
    
    if (minigame == GAME_SIMON_SAYS) {
        new failed = 0;
        for (new i = 1; i <= MaxClients; i++) {
            if((IsValidClient(i) && IsPlayerAlive(i))) {
                if((g_Mission[i] == 0) && IsPlayerTaunting(i))
                {
                    SetStateClient(i, true, true);
                }
                if((g_Mission[i] >= 1))
                {
                    if(IsPlayerTaunting(i))
                    {
                        SetOverlay(i,"");
                        SetOverlay(i,"simon_fail");
                        SetStateClient(i, false);
                        failed = 1;
                    }
                    else if(failed == 0)
                    {
                        SetStateClient(i, true);
                    }
                }
            }
        }
    }
}

PerformNeedleJump(i) {
    decl Float:fEyeAngle[3];
    decl Float:fVelocity[3];
    if (g_NeedleDelay[i] > 0) g_NeedleDelay[i] -= 1;
    if ((GetClientButtons(i) & IN_ATTACK) && (g_NeedleDelay[i] <= 0)){
        if(GetEntData(GetPlayerWeaponSlot(i, 0), FindSendPropInfo("CTFWeaponBase", "m_iClip1")) != 0){
            GetClientEyeAngles(i, fEyeAngle);
            GetEntPropVector(i, Prop_Data, "m_vecVelocity", fVelocity);
            fVelocity[0] += 10 * Cosine(DegToRad(fEyeAngle[1])) * -1;
            fVelocity[1] += 10 * Sine(DegToRad(fEyeAngle[1])) * -1;
            fVelocity[2] -= 40 * Sine(DegToRad(fEyeAngle[0])) * -1;
            if(FloatAbs(fVelocity[0]) > 400.0){
                if(fVelocity[0] > 0.0)
                    fVelocity[0] = 400.0;
                else
                    fVelocity[0] = -400.0;
            }
            if(FloatAbs(fVelocity[1]) > 400.0){
                if(fVelocity[1] > 0.0)
                    fVelocity[1] = 400.0;
                else
                    fVelocity[1] = -400.0;
            }
            if(fVelocity[2] > 400.0)
                fVelocity[2] = 400.0;
            TeleportEntity(i, NULL_VECTOR, NULL_VECTOR, fVelocity);
            g_NeedleDelay[i] = 3;
        }
    }
}

public OnEntityCreated(entity, const String:classname[]) {
    if ((minigame == GAME_BOSS_DEMO_BBALL) && (status == 2) && (StrEqual(classname, "tf_projectile_pipe"))) {
        SDKHook(entity, SDKHook_StartTouch, Hook_StartTouch_Pipe);
    }
}

public Action:Hook_StartTouch_Pipe(entity, other) {
    new String:classname[64];
    GetEdictClassname(other, classname, sizeof(classname));
    
    if (StrEqual(classname, "func_button") && status == 2) {
        new owner = GetEntPropEnt(entity, Prop_Send, "m_hThrower");
        if (g_Complete[owner] == false) {
            EmitSoundToClient(owner, WW_MINISCORE, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
            g_Minipoints[owner] += 1;
            if (g_Minipoints[owner] >= 7) SetStateClient(owner, true, true);
            CDetach(entity);
            AcceptEntityInput(entity, "Kill" );
        }
    }
}

public OnPlayerTouch(entity, other) {
    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && minigame == 14 && IsPlayerAlive(entity) && IsPlayerAlive(other) && (g_Mission[other] > 0 || g_Mission[entity] > 0)) {
        if (g_Mission[entity] > 0 && g_Mission[other] > 0) return;
        new attacker = entity;
        new victim = other;
        if (g_Mission[other] > 0) {
            attacker = other;
            victim = entity;
        }
        SetStateClient(victim, false);
        ForcePlayerSuicide(victim);
        EmitSoundToAll(WW_HEAVY_KISS, attacker);
        EmitSoundToClient(victim, WW_HEAVY_KISS);
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