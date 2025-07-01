/*
 * Compile on Linux/macOS:
 *   gcc server.c -o server -lpthread
 *
 * Compile on Windows (MSVC):
 *   cl /W4 server.c ws2_32.lib
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>          /* _beginthreadex */
typedef SOCKET sock_t;
#define close_socket closesocket
#define THREAD_RETURN unsigned __stdcall

#define _CRT_SECURE_NO_WARNINGS   /* for MSVC strncpy safety warning */
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define LISTEN_PORT 52423
#define BACKLOG     8           /* pending connections */

/* -------------- helpers -------------- */

/* Caller must free the returned string with free() */
static char *dup_str(const char *src)
{
    size_t len = strlen(src) + 1;
    char *dst = (char *)malloc(len);
    if (dst) memcpy(dst, src, len);
    return dst;
}

/* Returns the first non-empty IPv4 address it finds, or NULL on failure */
char *get_ip_address(void)
{
    DWORD bufLen = 0;
    if (GetAdaptersInfo(NULL, &bufLen) != ERROR_BUFFER_OVERFLOW)
        return NULL;

    IP_ADAPTER_INFO *adapters = (IP_ADAPTER_INFO *)malloc(bufLen);
    if (!adapters) return NULL;

    char *result = NULL;
    if (GetAdaptersInfo(adapters, &bufLen) == NO_ERROR) {
        for (IP_ADAPTER_INFO *cur = adapters; cur; cur = cur->Next) {
            const char *ip = cur->IpAddressList.IpAddress.String;
            if (ip && ip[0] != '\0' && strcmp(ip, "0.0.0.0") != 0) {
                result = dup_str(ip);
                break;
            }
        }
    }
    free(adapters);
    return result;           /* NULL means “couldn’t find one” */
}

/* Returns the subnet mask for the same adapter as above, or NULL */
char *get_subnet_mask(void)
{
    DWORD bufLen = 0;
    if (GetAdaptersInfo(NULL, &bufLen) != ERROR_BUFFER_OVERFLOW)
        return NULL;

    IP_ADAPTER_INFO *adapters = (IP_ADAPTER_INFO *)malloc(bufLen);
    if (!adapters) return NULL;

    char *result = NULL;
    if (GetAdaptersInfo(adapters, &bufLen) == NO_ERROR) {
        for (IP_ADAPTER_INFO *cur = adapters; cur; cur = cur->Next) {
            const char *ip = cur->IpAddressList.IpAddress.String;
            if (ip && ip[0] != '\0' && strcmp(ip, "0.0.0.0") != 0) {
                result = dup_str(cur->IpAddressList.IpMask.String);
                break;
            }
        }
    }
    free(adapters);
    return result;
}

/* -------------- demo -------------- */

int scan_network() {
    printf("Scanning...");
    return 0;
}

int printInfo(void)
{
    char *ip   = get_ip_address();
    char *mask = get_subnet_mask();

    if (ip && mask) {
        printf("IP Address : %s\n", ip);
        printf("Subnet Mask: %s\n", mask);
    } else {
        fprintf(stderr, "Unable to retrieve adapter information.\n");
    }

    free(ip);
    free(mask);
    return 0;
}

static unsigned __stdcall listener(void *arg)
{
    (void)arg;                   /* nothing passed in */

    SOCKET srv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (srv == INVALID_SOCKET) { perror("socket"); _endthreadex(1); }

    struct sockaddr_in sa = {0};
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port        = htons(LISTEN_PORT);

    if (bind(srv, (SOCKADDR*)&sa, sizeof sa) == SOCKET_ERROR) {
        perror("bind"); closesocket(srv); _endthreadex(1);
    }

    /* ---------- receive/send loop ---------- */
    for (;;) {
        struct sockaddr_in cli;
        int clen = sizeof(cli);
        char buffer[1024];

        int bytes_received = recvfrom(srv, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&cli, &clen);
        if (bytes_received == SOCKET_ERROR) {
            perror("recvfrom"); break;
        }

        buffer[bytes_received] = '\0';  // Null-terminate for safe printing
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
        // printf("Received from %s:%u -> %s\n", ip, ntohs(cli.sin_port), buffer);

        const char *msg = "👍";
        sendto(srv, msg, (int)strlen(msg), 0, (SOCKADDR*)&cli, clen);
    }

    closesocket(srv);
    _endthreadex(0);
    return 0; /* never reached */
}

uintptr_t startListener(void)
{
    /* ---- Winsock start-up ---- */
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa)) {
        fputs("WSAStartup failed\n", stderr); return 1;
    }

    /* ---- spawn listener thread ---- */
    uintptr_t th = _beginthreadex(NULL, 0, listener, NULL, 0, NULL);
    if (!th) { fputs("cannot start listener thread\n", stderr); return 1; }

    /* ---- main thread could do other work here ---- */
    // puts("Starting listener, disable with stoplistener");

    return th;
}

int stopListener(uintptr_t th) {
    /* ---- tidy up ---- */
    WaitForSingleObject((HANDLE)th, INFINITE);
    CloseHandle((HANDLE)th);
    WSACleanup();
    return 0;
}
