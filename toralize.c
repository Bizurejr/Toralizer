#include "toralize.h"

Req *request(struct sockaddr_in *sock2) {
    Req *req = malloc(reqsize);

    req->vn = 4;
    req->cd = 1;
    req->dstport = sock2->sin_port;
    req->dstip = sock2->sin_addr.s_addr;
    strncpy(req->userid, USERNAME, 7);
    req->userid[7] = '\0'; // Ensure null-terminated string

    return req;
}

/* Intercept and redirect connections */
int connect(int s2, const struct sockaddr *sock2, socklen_t addrlen) {
    int s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;

    char buf[ressize];
    int success; /* Boolean value used as predicate */
    int (*p)(int, const struct sockaddr *, socklen_t);

    p = dlsym(RTLD_NEXT, "connect");

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    sock.sin_family = AF_INET; /* Connect to proxy server */
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (p(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("connect");
        return -1;
    }

    printf("Connected to proxy\n");
    req = request((struct sockaddr_in *)sock2);
    write(s, req, reqsize);

    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1) {
        perror("read");
        free(req);
        close(s);
        return -1;
    }

    res = (Res *)buf;
    success = (res->cd == 90); /* SOCKS response code 90 means request granted */

    if (!success) {
        fprintf(stderr, "Unable to traverse the proxy, error code: %d\n", res->cd);
        close(s);
        free(req);
        return -1;
    }

    printf("Connected through the proxy.\n");

    /* Man-in-the-middle: redirect communication */
    dup2(s, s2);
    free(req);
    return 0;
}


