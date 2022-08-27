/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _FILE_UPDATER_H
#define _FILE_UPDATER_H

class Updater {
public:
    bool Connect(char *ip, int port);

    bool Download(char *filename);

    bool GetDirectoryList(char *directory, char *directoryList);

    void Close(void);

    float GetDownloadedSizeInBytes(void) {
        return (float) downloadedSize;
    }

    float GetDownloadedSizeInKiloBytes(void) {
        return (float) downloadedSize / 1024;
    }

    float GetDownloadedSizeInMegaBytes(void) {
        return (float) (downloadedSize / 1024) / 1024;
    }

    float GetDownloadedFileSizeInBytes(void) {
        return (float) fileSize;
    }

    float GetDownloadedFileSizeinKiloBytes(void) {
        return (float) fileSize / 1024;
    }

    float GetDownloadedFileSizeInMegaBytes(void) {
        return (float) (fileSize / 1024) / 1024;
    }

private:
    SOCKET sock;
    sockaddr_in server;
    int len;
    int downloadedSize;
    int fileSize;
    char buffer[1048576];
};



#endif
