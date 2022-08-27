/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"

bool Updater::Connect(char *ip, int port) {
    WSADATA WsaData;
    WSAStartup(MAKEWORD(2, 2), &WsaData);

    len = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("error: cannot create socket ::FUP!\n");
        return false;
    }

    if (connect(sock, (sockaddr *) &server, len) == -1) {
        printf("error: cannot estabilish connection ::FUP!\n");
        return false;
    }

    return true;
}

bool Updater::Download(char *filename) {
    char originalChecksum[32];

    if (strstr(filename, "files/")) {
        sprintf(buffer, "file %s %s", filename, gui.GetPlayerImprint());
    } else {
        sprintf(buffer, "file files/%s %s", filename, gui.GetPlayerImprint());
//        filename += 6;
    }

    if (sendto(sock, buffer, strlen(buffer) + 1, 0, (sockaddr *) &server, len) != SOCKET_ERROR) {
        char header[1024], filepath[1024];

        if ((recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr *) &server, &len) > 0)) {
            printf("%s\n", buffer);
            if (strstr(buffer, "invalid_file")) {
                printf("error: file %s doesn't exists, or u r trying download files from bad directory!\n", filename);
                return false;
            }

            if (sscanf(buffer, "file %s %d %s", &filepath, &fileSize, &originalChecksum) != 3) {
                printf("error: invalid header !\n");
                return false;
            }

        }
        gui.mainProgressBar.SetPercents(1);

        FILE *fp = fopen(filename, "wb");
        if (!fp) {
            printf("error: cannot create file %s!\n", filename);
            return false;
        }

        float startTime = timeGetTime();
        int size = 0;
        while (((size = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr *) &server, &len)) != 0) && (size != -1)) {
            fwrite(buffer, 1, size, fp);
            downloadedSize += size;
            //printf("ok: downloaded %.1fMB from %.1fMB\n", GetDownloadedSizeInMegaBytes(), GetDownloadedFileSizeInMegaBytes());

            sprintf(buffer, "Downloading file \"%s\" (%.1fKB z %dKB)", filename, GetDownloadedSizeInKiloBytes(),
                    (int) fileSize / 1024);

            gui.mainProgressBar.SetPercents(round((GetDownloadedSizeInKiloBytes() / (fileSize / 1024)) * 100));
            gui.mainProgressBar.SetMessage(buffer);
            if (downloadedSize >= fileSize) break;
        }
        fclose(fp);

        fp = fopen(filename, "rb");
        if (!fp) {
            printf("error: cannot open file %s!\n", filename);
            return false;
        }

        unsigned char *fileContent = new unsigned char[fileSize];
        if (!fileContent) return false;

        fread(fileContent, 1, fileSize, fp);
        fclose(fp);

        fp = fopen(filename, "wb");
        if (!fp) {
            printf("error: cannot create file %s!\n", filename);
            return false;
        }

        fwrite(fileContent, 1, fileSize, fp);
        fclose(fp);

        char acctualChecksum[32], oChecksum[32];
        strcpy(oChecksum, originalChecksum);
        //strcpy(acctualChecksum, md5(fileContent, fileSize));
        if (fileContent) delete[] fileContent;

        /*
        if (!memcmp(oChecksum, acctualChecksum, 32)) {
         */
            printf("ok: file %s (%.4fMB) was sucessfully downloaded in %.4fs!\n", filename,
                   GetDownloadedFileSizeInMegaBytes(), (timeGetTime() - startTime) * 0.001);
            return true;
            /*
        } else {
            remove(filename);
            printf("error: file %s was not sucessfully downloaded, try it again!\n", filename);
            return false;
        }
             */
    }

    return false;
}

bool Updater::GetDirectoryList(char *directory, char *directoryList) {
    char header[1024];
    if (!directoryList) return false;

    sprintf(header, "dirlist %s", directory);//gui.GetPlayerImprint());

    if (sendto(sock, header, strlen(header) + 1, 0, (sockaddr *) &server, len) != SOCKET_ERROR) {
        if ((recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr *) &server, &len) > 0)) {
            if (strstr(buffer, "invalid_directory")) {
                printf("error: %s is not valid directory!\n", directory);
                return false;
            } else {
                strcpy(directoryList, buffer);
            }
        }
    }

    return true;
}

void Updater::Close(void) {
    closesocket(sock);
}
