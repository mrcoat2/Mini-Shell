#define _CRT_SECURE_NO_WARNINGS   /* for MSVC strncpy safety warning */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

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
    printf("Scanning..");
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