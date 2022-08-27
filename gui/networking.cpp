/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"

extern Gui gui;

int Networking::Connect() {

    WSADATA data;
    hostent *host;
    sockaddr_in serverSock;
    int retSocket;

    if (WSAStartup(MAKEWORD(1, 1), &data) != 0) return false;
    if ((host = gethostbyname(ServerName)) == NULL) {
        printf("error: bad adress!\n");
        gui.SetMessage("Cannot connect to server! Try it later!\n");
        WSACleanup();
        return -1;
    }

    if ((retSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("error: cannot create socket!\n");
        WSACleanup();
        return -1;
    }

    serverSock.sin_family = AF_INET;
    serverSock.sin_port = htons(80);
    memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);

    if (connect(retSocket, (sockaddr *) &serverSock, sizeof(serverSock)) == -1) {
        gui.SetMessage("error: cannot connect to server!\n");
        WSACleanup();
        return -1;
    }

    return retSocket;
}

char *Networking::Send(char *header, bool bGetSid) {
    char buff[16384];
    char *ret = new char[8388608];

    int connSocket;

    if (!header) return NULL;
    if (!(connSocket = Connect())) return NULL;

    int size;
    if ((size = send(connSocket, header, strlen(header) + 1, 0)) == -1) {
        gui.SetMessage("error: cannot send data to server!\n");
        return NULL;
    }

    int totalSize = 0;
    while (((size = recv(connSocket, buff, sizeof(buff), 0)) != 0) && (size != -1)) {
        totalSize += size;
        buff[size] = '\0';
        strcat(ret, buff);
    }
    ret[totalSize + 1] = '\0';

    if (!ret) return NULL;

    if (bGetSid) {
        char *sid = GetSESSIONID(ret);
        if (sid) gui.SetSESSIONID(sid);
    }

    if (size == -1) {
        printf("error: cannot receive data from server!\n");
        return NULL;
    }

    char *final = new char[strlen(ret) + 1];
    strcpy(final, ret);
    if (ret) delete[] ret;

    Close(connSocket);
    return IsValidResponse(final);
}


bool Networking::SendB(char *header, char *fPath) {
    char buff[16384];
    char ret[2048000]; //2MB? xD
    int connSocket;

    if (!header) return NULL;
    if (!(connSocket = Connect())) return NULL;

    int size;
    if ((size = send(connSocket, header, strlen(header) + 1, 0)) == -1) {
        printf("error: cannot send data to server!\n");
        return false;
    }

    FILE *pFile = fopen(fPath, "wb");
    if (!pFile) return false;

    int totalSize = 0;


    float time1 = timeGetTime();
    while (((size = recv(connSocket, buff, sizeof(buff), 0)) != 0) && (size != -1)) {
        totalSize += size;
        memcpy(ret, buff, size);
    }
    printf("%.1f ms\n", timeGetTime() - time1);

    char *find = "Content-Length: ";
    if (!strstr(ret, find)) return false;
    char *pt = strstr(ret, find) + strlen(find);
    pt = strtok(pt, "\n");
    int contentLenght = atoi(pt);

    for (int i = totalSize - contentLenght; i < totalSize; i++) fwrite(&ret[i], 1, 1, pFile);

    fclose(pFile);
    Close(connSocket);

    return true;

}

void Networking::Close(int socket) {
    closesocket(socket);
    WSACleanup();
    return;
}
