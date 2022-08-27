/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _NETWORKING_H
#define _NETWORKING_H

#define ServerName "www.masterserver.php5.cz"
#define userLogInHeader "POST /index.php HTTP/1.0\nUser-Agent: ETlogin\nHost: www.masterserver.php5.cz\nContent-Length: %d\nContent-Type: application/x-www-form-urlencoded\n\nlogMail=%s&logPass=%s\n\n"

#define registerHeader "GET /index.php?registerParams=%s HTTP/1.0\nUser-Agent: East WTF\nhost: www.masterserver.php5.cz\n\n"
#define registerHeaderUserExists "GET /index.php?userExists=%s HTTP/1.0\nUser-Agent: East WTF\nhost: www.masterserver.php5.cz\n\n"

#define loginHeader "GET /index.php?loginParams=%s HTTP/1.0\nUser-Agent: East WTF\nhost: www.masterserver.php5.cz\n\n"
#define logoutHeader "GET /index.php?unlogUser= HTTP/1.0\nhost: www.masterserver.php5.cz\nUser-Agent: East WTF\nCookie: PHPSESSID=%s\n\n"
#define userAvatarHeader "GET /%s HTTP/1.0\nhost: www.masterserver.php5.cz\nUser-Agent: East WTF\nCookie: PHPSESSID=%s\n\n"
#define GetAvatarPathHeader "GET /index.php?getAvatarPath HTTP/1.0\nhost: www.masterserver.php5.cz\nKeep-Alive: 115\nConnection: keep-alive\nUser-Agent: East WTF\nCookie: PHPSESSID=%s\n\n"
#define GetUserInfoHeader "GET /index.php?getUserInfo HTTP/1.0\nhost: www.masterserver.php5.cz\nKeep-Alive: 115\nConnection: keep-alive\nUser-Agent: East WTF\nCookie: PHPSESSID=%s\n\n"
#define updatePlayerNickHeader "GET /index.php?updatePlayerNick=%s HTTP/1.0\nhost: www.masterserver.php5.cz\nUser-Agent: East WTF\nCookie: PHPSESSID=%s\n\n"
#define deleteServerByIDHeader "GET /index.php?deleteServerByID=%s HTTP/1.0\nhost: www.masterserver.php5.cz\nUser-Agent: East WTF\nCookie: PHPSESSID=%s\n\n"
#define uploadAvatarHeader "POST /index.php HTTP/1.0\nUser-Agent: ETUfo\nCookie: PHPSESSID=%s\nHost: www.masterserver.php5.cz\nContent-Length: %d\nContent-Type: application/x-www-form-urlencoded\n\navatarContent=%s\n\n"

class Networking {
public:
    int Connect();

    char *Send(char *header, bool bGetSid = true);

    bool SendB(char *header, char *fPath);

    void Close(int socket);

private:
};

inline char *IsValidResponse(char *in) {
    if ((strstr(in, "HTTP/1.0 200 OK") || strstr(in, "HTTP/1.1 200 OK"))) {
        char *mes = strstr(in, "\r\n\r\n");
        while (strstr(mes, "\r\n\r\n")) {
            mes = strstr(mes, "\r\n\r\n");
            mes += 4;
        }

        return mes;
    }

    return NULL;
}

inline char *GetSESSIONID(char *in) {
    if (!in) return NULL;

    if ((strstr(in, "HTTP/1.0 200 OK") || strstr(in, "HTTP/1.1 200 OK")) && strstr(in, "Set-Cookie:")) {
        char *find = "PHPSESSID=";
        char ret[255];
        int i = 0;
        char *ptr = strstr(in, find) + strlen(find);
        for (i = 0; i < strcspn(ptr, ";"); i++) ret[i] = ptr[i];
        ret[i] = '\0';
        return ret;
    }

    return NULL;
}

#endif
