﻿#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Windows.h"

#define NAME L"\\\\.\\pipe\\Tube"

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    HANDLE sH;
    DWORD lp;
    char buf[50];

    try {
        cout << "ServerNP\n\n";

        if ((sH = CreateNamedPipe(NAME,
                                    PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, //режимы 
                                    1, NULL, NULL, INFINITE, nullptr)) == INVALID_HANDLE_VALUE) { //max кол во экземпляров канала, размер входного/выходного буфера, время ожидания подключ
            throw SetPipeError("CreateNamedPipe: ", GetLastError());
        }

        cout << "Waiting for connect...\n";

        while (true) {
            if (!ConnectNamedPipe(sH, NULL)) { //в случае синхронной связи можно установить значение NULL      

                throw SetPipeError("ConnectNamedPipe: ", GetLastError());
            }
            while (true) {
                if (ReadFile(sH, buf, sizeof(buf), &lp, NULL)) {

                    if (strcmp(buf, "STOP") == 0) {
                        cout << endl;
                        break;
                    }
                    cout << buf << endl;
                    if (WriteFile(sH, buf, sizeof(buf), &lp, NULL)) {
                        if (strstr(buf, "ClientNPct")) {
                            break;
                        }
                    }
                    else {
                        throw SetPipeError("WriteFile: ", GetLastError());
                    }
                }
                else {
                    throw SetPipeError("ReadFile: ", GetLastError());
                }
            }
            if (!DisconnectNamedPipe(sH)) {
                throw SetPipeError("DisconnectNamedPipe: ", GetLastError());
            }
        }
    }
    catch (string ErrorPipeText) {
        cout << endl << ErrorPipeText;
    }
}