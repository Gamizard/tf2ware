// Math module

public Math_OnMinigame() {
    AddToForward(g_justEnteredMinigame, INVALID_HANDLE, Math_justEnteredMinigame);
    
    RegConsoleCmd("say", Math_Player_Say);
    RegConsoleCmd("say_team", Math_Player_Say);

    new random1 = GetRandomInt(3, 15);
    new random2 = GetRandomInt(3, 15);
    new form = GetRandomInt(1,3);
    new String:formString[12];
    
    if (form == 1) {
        // A guy one day kept saying 9001 to the math problems and was sad that it never was it. Take this as a 1/50 chance joke :P
        if (GetRandomInt(1, 50) == 1) {
            random1 = GetRandomInt(1,9)*1000;
            random2 = 9001 - random1;
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

}

public Math_justEnteredMinigame(client) {
    if (IsValidClient(client)) {
        SetHudTextParams(-1.0, 0.4, 4.0, 255,255,255, 255, 0, 6.0, 0.2, 0.5);
        ShowHudText(client, 5, "%s = ?", g_mathquestion);
    }
}

public Action:Math_Player_Say(iClient, iArgs) {
    // Check to make sure this is happening during the math minigame
    if (!StrEqual(g_name[minigame-1], "Math")) return Plugin_Continue;
    if (!(IsValidClient(iClient))) return Plugin_Continue;
    if (iArgs < 1) return Plugin_Continue;
    
    if ((IsPlayerAlive(iClient)) && (status == 2) && (g_Complete[iClient] == false)) {
        // Retrieve the first argument and check it's a valid trigger
        decl String:strArgument[64]; GetCmdArg(1, strArgument, sizeof(strArgument));
        new String:strZero[2];
        new bool:isAnswerZero = false;
        strZero = "0";

        if(strcmp(strArgument,strZero) == 0)
        {
            isAnswerZero = true;
        }
        
        new guess = StringToInt(strArgument);
        
        if ((guess == g_result) || (g_result == 0 && isAnswerZero == true)) {
            SetHudTextParams(-1.0, 0.4, 3.0, 0,255,0, 255, 0, 6.0, 0.2, 0.5);
            // Replace the current display to include the guess instead of a question mark
            ShowHudText(iClient, 5, "%s = %d", g_mathquestion, guess);            
            SetStateClient(iClient, true, true);
            if (!(g_first)) {
                for (new i = 1; i <= MaxClients; i++) {
                    if (IsValidClient(i)) {
                        if (g_Country[i] == 1) CPrintToChatEx(i, iClient, "{teamcolor}%N{green} ha indovinato la risposta per primo!", iClient);
                        else CPrintToChatEx(i, iClient, "{teamcolor}%N{green} guessed the answer first!", iClient);
                    }
                }
                g_first = true;
            }
        }
        if (guess != g_result) {
            SetHudTextParams(-1.0, 0.4, 3.0, 255,0,0, 255, 0, 6.0, 0.2, 0.5);
            // Use a notequals sign!
            ShowHudText(iClient, 5, "%s = %d", g_mathquestion, guess);
            ForcePlayerSuicide(iClient);
        }        
        return Plugin_Handled;
    }
    
    // If no valid argument found, pass
    return Plugin_Continue;
}