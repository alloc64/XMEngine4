/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"

extern Gui gui;
extern Camera camera;
extern Characters characters;
extern xmWeapon weapon;
extern HUD hud;


SOCKET Network::CreateChatSocket(const char *server, int port) {
    hostent *host;
    SOCKET sock;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("error: cannot create socket!\n");
        return NULL;
    }
    if ((host = gethostbyname(server)) == NULL) {
        printf("error: cannot identify hostname!\n");
        return NULL;
    }

    chatLen = sizeof(chatServ);
    chatServ.sin_family = AF_INET;
    chatServ.sin_port = htons(port);
    memcpy(&(chatServ.sin_addr), host->h_addr, host->h_length);

    return sock;
}

SOCKET Network::Create(const char *server, int port) {
    if (!InitWinsock()) {
        printf("error: cannot load winsock2!\n");
        return NULL;
    }

    len = sizeof(serv);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = inet_addr(server);

    SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    SetConnectionLag(sock, 10);

    bind(sock, (struct sockaddr *) &serv, len);

    if (sock == INVALID_SOCKET) {
        printf("error: bind() - invalid socket!\n");
        return NULL;
    }

    return sock;
}

bool Network::SendAx(SOCKET sock, char *tosend, char *output) {
    startPing = (float) timer.GetSystemTime();
    WSABUF recvBuf;
    WSABUF sendBuf;
    DWORD BytesSent = 0;
    DWORD Flags = 0;

    sendBuf.buf = tosend;
    sendBuf.len = strlen(tosend) + 1;
    //printf("%s\n", tosend);
    int nSendRel = WSASendTo(sock, &sendBuf, 1, &BytesSent, Flags, (sockaddr *) &serv, len, NULL, NULL);

    if (SOCKET_ERROR != nSendRel) {
        memset(&buffer, 0, sizeof(buffer));
        recvBuf.buf = buffer;
        recvBuf.len = sizeof(buffer);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = 0;
        int nRecvRel = WSARecvFrom(sock,
                                   &recvBuf,
                                   1,
                                   &NumberOfBytesRecvd,
                                   &Flags,
                                   (sockaddr *) &serv,
                                   &len,
                                   NULL,
                                   NULL);

        if (SOCKET_ERROR != nRecvRel) {
            ping = (float) timer.GetSystemTime() - startPing;
            strcpy(output, recvBuf.buf);
            //printf("%s\n", output);
            strcpy(buffer, "");
            return true;
        }
    }

    lostPacketCount++;
    iConnectionFailsCount += timer.GetDT();
    printf("error: send failed - %d!\n", WSAGetLastError());
    ping = -1;
    return false;
}

bool Network::ConnectToServer(char *server, int port) {
    if (!(sock = Create(server, port))) {
        gui.MessageBox("error: server not responding!\n");
        return false;
    }
    if (!(chatSock = CreateChatSocket(server, port + 1))) {
        gui.MessageBox("error: server not responding ::Chatroom!\n");
        return false;
    }

    sprintf(buffer, "first_connect %s %s", gui.cUserNick, gui.cSessionID);

    if (!SendAx(sock, buffer, output)) {
        printf("error: data retrieve error!\n");
        return false;
    }

    if (strstr(output, "unlogged_hacker")) {
        gui.bLogoutUser = true;
        return false;
    }

    if (strstr(output, "full_server")) {
        gui.MessageBox("Sorry, server is full! Buy premium account!\n");
        return false;
    }

    int uid, maxPlayers, maxRoundTime;
    char imp[16], mapName[1024];

    if (sscanf(output, "cid %d %d %s %s", &uid, &maxPlayers, &mapName, &imp) == 4) {
        gui.SetPlayerID(uid);
        gui.SetMaxPlayersOnServer(maxPlayers);
        gui.SetPlayerImprint(imp);
        SetMap(mapName);
    } else {
        return false;
    }
    //printf("%s\n", output);

    return true;
}


bool Network::AdjustPositions(void) {
    if (!sock) {
        printf("error: invalid socket %d!", sock);
        return false;
    }

    vec3 pos = camera.GetPosition();
    vec3 angle = camera.GetAngle();

    sprintf(buffer, "pos %f %f %f %f %f %d %d %d %d %d %d %d %d %d %d %d %s",
            pos.x, pos.y, pos.z,
            angle.x, angle.y,
            gui.GetPlayerID(),
            camera.IsCameraWalking(),
            camera.IsCameraMoving(),
            camera.IsCameraStrafingLeft(),
            camera.IsCameraStrafingRight(),
            camera.IsJumping(),
            camera.IsCrouched(),
            weapon.GetCurrentWeaponID(),
            weapon.GetCurrentWeaponAnimID(),
            weapon.GetCurrentWeaponFrame(),
            hud.IsBuyMenuShown(),
            gui.GetPlayerImprint());

    //printf(buffer);

    float start = timer.GetSystemTime();
    if (!SendAx(sock, buffer, output)) {
        printf("error: data retrieve error!\n");
        camera.SetCameraFreezed();
        return false;
    }

    if (!output || !*output || !strcmp(output, " ")) {
        printf("error: invalid input data - \"%s\"!\n", output);
        return false;
    }

    if (strstr(output, "timeout")) {
        gui.MessageBox("Connection timeout!\n");
        gui.bLoadLevel = false;
        gui.bLevelLoaded = false;
        gui.bShowGUI = true;
        return false;
    }

    if (strstr(output, "err")) {
        gui.MessageBox("Unexpected error occured!\n");
        gui.bLoadLevel = false;
        gui.bLevelLoaded = false;
        gui.bShowGUI = true;
        return false;
    }


    int id, walking, moving, isStrafingLeft, isStrafingRight, isJumping, isCrouched, dead, k = 0, roundRestart, damage, health, cframe, roundTime, wid;
    if (!output || !strcmp(output, "")) return false;
    char *line = strtok(output, "\n");
    if (!line) {
        printf("error: invalid data ::line!\n");
        return false;
    }

    while (line) {
        if (sscanf(line, "%f %f %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                   &pos.x, &pos.y, &pos.z,
                   &angle.x, &angle.y,
                   &id,
                   &walking,
                   &moving,
                   &isStrafingLeft,
                   &isStrafingRight,
                   &isJumping,
                   &isCrouched,
                   &dead,
                   &damage,
                   &health,
                   &roundRestart,
                   &cframe,
                   &wid,
                   &roundTime) == 19) {
            playerPositions[id].slotUsed = true;
            playerPositions[id].angle = angle;
            playerPositions[id].lastPos = playerPositions[id].pos;
            playerPositions[id].pos = pos;
            playerPositions[id].id = id;

            playerPositions[id].isWalking[OLD_STATE] = playerPositions[id].isWalking[REAL_STATE];
            playerPositions[id].isWalking[REAL_STATE] = (bool) walking;

            playerPositions[id].isMoving[OLD_STATE] = playerPositions[id].isMoving[REAL_STATE];
            playerPositions[id].isMoving[REAL_STATE] = (bool) moving;

            playerPositions[id].isStrafingLeft[OLD_STATE] = playerPositions[id].isStrafingLeft[REAL_STATE];
            playerPositions[id].isStrafingLeft[REAL_STATE] = (bool) isStrafingLeft;

            playerPositions[id].isStrafingRight[OLD_STATE] = playerPositions[id].isStrafingRight[REAL_STATE];
            playerPositions[id].isStrafingRight[REAL_STATE] = (bool) isStrafingRight;

            playerPositions[id].isJumping[OLD_STATE] = playerPositions[id].isJumping[REAL_STATE];
            playerPositions[id].isJumping[REAL_STATE] = (bool) isJumping;

            playerPositions[id].isCrouched[OLD_STATE] = playerPositions[id].isCrouched[REAL_STATE];
            playerPositions[id].isCrouched[REAL_STATE] = (bool) isCrouched;

            playerPositions[id].dead = (bool) dead;
            playerPositions[id].damage = damage;
            playerPositions[id].currFrame = cframe;
            playerPositions[id].wid = wid;
            playerPositions[id].packetPing = timer.GetSystemTime() - start;
            if (health <= 100 && health >= 0) playerPositions[id].health = health;

            usedPlayerSlotID[k++] = id;
            hud.SetRoundTime(roundTime);

            if (roundRestart && id == gui.GetPlayerID()) {
                if (camera.GetType() != FIRST_PERSON) camera.SetType(FIRST_PERSON);
                //camera.bJump = true;
                weapon.bWeaponChanged = true;
                characters.SetAlive();
                camera.SetPosition(pos);
            }

            bRespawnPlayers = (bool) roundRestart;// && !playerPositions[id].dead;
        }
        line = strtok(NULL, "\n");
    }


    iNumUsedPlayerSlots = k;
    k = 0;
    camera.SetCameraMovable();
    return true;
}


bool Network::SendPlayerHit(void) {
    if (!sock) return false;
    if (gui.GetMaxPlayersOnServer() <= 0) return true;

    if (characters.bHit) {
        vec3 direction = -camera.GetCameraDirection();
        sprintf(buffer, "hit %d %d %d %d %f %f %f %s",
                gui.GetPlayerID(),
                characters.iHitPlayerID,
                weapon.wid,
                characters.iCausedDamage,
                direction.x,
                direction.y,
                direction.z,
                gui.GetPlayerImprint());
        characters.bHit = false;

        if (!SendAx(sock, buffer, output)) {
            printf("error: data retrieve error ::GetPositionsAnotherPlayers() - %s!\n",
                   output ? output : (char *) "NULL");
            return false;
        }
        int health, id, dead;
        if (sscanf(output, "cdmg %d %d %d", &health, &id, &dead) == 3) {
            printf("%d %d %d\n", health, id, dead);
            playerPositions[id].health = health;
        } else {
            printf("ERROR: %s\n", output);
            return false;
        }
    }
    return true;
}

bool Network::GetPlayerScores(void) {
    if (!sock) return false;
    sprintf(buffer, "score %s", gui.GetPlayerImprint());
    if (!SendAx(sock, buffer, output)) {
        printf("error: data retrieve error!\n");
        return false;
    }

    int kills, deaths, ping, id;
    char name[1024];

    char *line = strtok(output, "\n");
    if (!line) return false;

    while (line != NULL) {
        if (sscanf(line, "%s %d %d %d %d\n", &name, &kills, &deaths, &ping, &id) == 5) {
            playerPositions[id].name = (char *) name;
            playerPositions[id].kills = kills;
            playerPositions[id].deaths = deaths;
            playerPositions[id].ping = ping;
        }
        line = strtok(NULL, "\n");
    }

    return true;
}

bool Network::SetSlotLoaded(void) {
    if (!sock) return false;
    sprintf(buffer, "loaded %d %s", gui.GetPlayerID(), gui.GetPlayerImprint());
    printf("%s\n", buffer);
    if (!SendAx(sock, buffer, output)) {
        printf("error: data retrieve error!\n");
        return false;
    }
    printf("%s\n", output);
    if (!strstr(output, "loaded")) return false;
    return true;
}

bool Network::DisconnectPlayer(void) {
    if (!sock) return false;
    sprintf(buffer, "disconnect %d %s", gui.GetPlayerID(), gui.GetPlayerImprint());
    if (!SendAx(sock, buffer, output)) {
        printf("error: data retrieve error!\n");
        return false;
    }

    if (strstr(output, "failed")) {
        printf("error: cannot disconnect player!\n");
    }

    return true;
}


bool Network::SendChatMessage(char *SESSIONID, char *nick, char *message) {
    char send[1024];
    char buff[4096];
    int ret = 0;

    if (!strcmp(message, "") || !strcmp(message, " ")) return false;
    sprintf(send, "add %s|%s: %s", SESSIONID, nick, message);

    if ((ret = sendto(chatSock, send, strlen(send) + 1, 0, (sockaddr *) &chatServ, chatLen)) == SOCKET_ERROR)
        return false;
    if ((ret = recvfrom(chatSock, buff, sizeof(buff), 0, (sockaddr *) &chatServ, &chatLen)) == SOCKET_ERROR)
        return false;
    buff[ret] = '\0';
    return strstr(buff, "added");
}

char *Network::GetChatMessages(char *SESSIONID, char *nick) {
    char send[1024];
    char buff[4096];
    int ret = 0;
    int final = 0;
    sprintf(send, "get %s|%s", SESSIONID, nick);

    if ((ret = sendto(chatSock, send, strlen(send) + 1, 0, (sockaddr *) &chatServ, chatLen)) == SOCKET_ERROR)
        return nullptr;
    if ((ret = recvfrom(chatSock, buff, sizeof(buff), 0, (sockaddr *) &chatServ, &chatLen)) == SOCKET_ERROR)
        return nullptr;
    final += ret;
    buff[final] = '\0';
    return buff ? buff : NULL;
}

char *Network::GetFileHash(char *path) {
    if (!sock) return NULL;
    sprintf(buffer, "ghash %s", path);
    if (!SendAx(sock, buffer, output)) {
        printf("error: data retrieve error!\n");
        return NULL;
    }
    printf("%s\n", output);

    if (!*output || strlen(output) != 32) return NULL;

    return output;

}


