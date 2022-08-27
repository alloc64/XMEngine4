/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H

#define CONECTION_FAILS 30
#define MAX_PLAYERS 128

#define MASTER_SERVER_IP "2.4.1.3"
#define MASTER_SERVER_PORT 3517

inline bool ValidHashes(char *hash1, char *hash2) {
    return !strcmp(hash1, hash2);
}

inline void SetConnectionLag(SOCKET sock, int timeVal) {
    timeval tv;
    tv.tv_sec = timeVal;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(timeval)) == SOCKET_ERROR) {
        printf("error: setsockopt() - error: %d\n", WSAGetLastError());
        return;
    }
}


extern Timer timer;
extern xmLevel level;
extern xmWeapon weapon;

enum {
    BLUE_TEAM = 0,
    RED_TEAM = 1
};

enum {
    REAL_STATE = 0,
    OLD_STATE = 1
};

struct player {
    player() {
        id = 0;
        pos = vec3(0);
        angle = vec3(0);
        lastPos = vec3(0);
        slotUsed = false;
        damage = 0;
        health = 0;
        team = BLUE_TEAM;
        dead = true;
        isWatched = false;
        currFrame = 0;
        wid = 0;
        packetPing = 1.0;
    }

    int GetDamage() {
        return damage;
    }

    vec3 pos;
    vec3 lastPos;
    vec3 angle;
    int id;
    bool slotUsed;
    // 0 real state
    // 1 old state
    bool isWalking[2];
    bool isMoving[2];
    bool isStrafingLeft[2];
    bool isStrafingRight[2];
    bool isJumping[2];
    bool isCrouched[2];

    char *name;
    int deaths;
    int kills;
    int team;
    int ping;

    float packetPing;

    bool dead;
    int damage;
    int health;
    int currFrame;
    int wid;

    bool isWatched;

    void SetTeam(int team) {
        this->team = team;
    }
};

class Network {
public:
    Network() {
        iConnectionFailsCount = 0;
        lostPacketCount = 0;
        bCanMove = false;
        bRespawnPlayers = false;
        strcpy(currMapName, "");
    }

    SOCKET Create(const char *server, int port);

    SOCKET CreateChatSocket(const char *server, int port);

    void SetSocketBlocking(u_long Block);

    bool SendAx(SOCKET sock, char *tosend, char *output); //ASCII
    bool SendBx(SOCKET sock, char *tosend, char *output); //BINARY
    bool ConnectToServer(char *server, int port);

    char *GetFileHash(char *path);


    inline bool ConnectToServerByUserList() {
        if (!serverIP || !serverPort) return false;
        iConnectionFailsCount = 0;
        return ConnectToServer(serverIP, serverPort);
    }

    inline char *GetCurrentMap(void) {
        return currMapName;
    }

    inline void SetMap(char *mapname) {
        if (!mapname) return;
        strcpy(currMapName, "");
        strcat(currMapName, "maps/");
        strcat(currMapName, mapname);
    }

    inline void RespawnPlayers(void);

    bool CanRespawnPlayers(void) {
        return bRespawnPlayers;
    }

    inline int GetNumberConnectionFails(void) {
        return iConnectionFailsCount;
    }

    bool AdjustPositions(void);

    bool GetPositionsAnotherPlayers(void);

    bool SetSlotLoaded(void);

    bool DisconnectPlayer(void);

    bool GetPlayerScores(void);

    bool SendPlayerHit(void);

    bool SendChatMessage(char *SESSIONID, char *nick, char *message);

    char *GetChatMessages(char *SESSIONID, char *nick);

    inline void GetServerInfo(char *server, int port, int *ping, int *numPlayers, bool *responding, char *info) {
        int start = (int) timer.GetSystemTime();
        if (!SendAx(Create(server, port), "info\0", output)) {
            printf("error: data retrieve error!\n");
            *responding = false;
            *ping = 0;
            *numPlayers = 0;
            strcpy(info, "not responding");
            return;
        }

        if (output) {
            *responding = true;
            //*numPlayers =
            *ping = (int) (timer.GetSystemTime() - start);
            strcpy(info, output);
        }
    }

    SOCKET GetSocket(void) {
        return sock;
    }

    float GetPing(void) {
        return ping;
    }

    int GetPort(void) {
        return serverPort;
    }

    int GetPacketLoss() {
        return lostPacketCount;
    }

    void SetPort(int port) {
        if (port < 1) return;
        serverPort = port;
    }

    char *GetServerIP(void) {
        return !serverIP ? (char *) "NULL" : serverIP;
    }

    void SetServerIP(char *ip) {
        if (!ip) return;
        strcpy(serverIP, ip);
    }

    bool CanUpdatePosition() {
        return bCanMove;
    }

    char serverIP[255];
    char serverName[255];
    char currMapName[1024];
    int serverPort;
    float ping;
    float startPing;
    player playerPositions[MAX_PLAYERS];
    int usedPlayerSlotID[MAX_PLAYERS];
    int iNumUsedPlayerSlots;


private:
    int len;
    int chatLen;
    float iConnectionFailsCount;
    int lostPacketCount;

    sockaddr_in serv;
    sockaddr_in chatServ;

    SOCKET sock;
    SOCKET chatSock;

    char buffer[4096];
    char output[4096];

    bool bCanMove;
    bool bRespawnPlayers;
};

extern Network netw;

struct Server {
    void Set(char *ip, int port) {
        strcpy(this->ip, ip);
        this->port = port;
        //ping a dalsi picoviny loadnu ze serveriqu ;)
        netw.GetServerInfo(this->ip, this->port, &this->ping, &this->numPlayers, &this->responding, this->mode);
        //printf("%s %d %d %s\n", this->ip, this->port, this->ping, this->mode);
    }

    void Refresh(void) {
        netw.GetServerInfo(this->ip, this->port, &this->ping, &this->numPlayers, &this->responding, this->mode);
    }

    char ip[255];
    char mode[255];
    int port;
    int ping;
    int numPlayers;
    bool responding;
};

inline bool InitWinsock() {
    WSADATA WsaData;
    int Error = WSAStartup(MAKEWORD(2, 2), &WsaData);
    if (Error != 0) {
        return false;
    } else {
        if (LOBYTE(WsaData.wVersion) != 2 || HIBYTE(WsaData.wHighVersion) != 2) {
            WSACleanup();
            return false;
        }
    }
    return true;
}

inline bool IsIPv4(char *ip) {
    int num;
    int flag = 1;
    int counter = 0;
    char *p = strtok(ip, ".");

    while (p && flag) {
        num = atoi(p);

        if (num >= 0 && num <= 255 && (counter++ < 4)) {
            flag = 1;
            p = strtok(NULL, ".");

        } else {
            flag = 0;
            break;
        }
    }

    return flag && (counter == 3);
}


#endif
