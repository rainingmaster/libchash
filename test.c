#include "libchash.h"
#include <stdlib.h>
#include <string.h>


void main()
{
    libchash_ctx_t      *ctx;
    libchash_node_t     *nodes;
    u_char              *name[] = {"AZJTZ1B001545P", "BZJTZ1B001545P",
                                   "CZJTZ1B001545P", "DZJTZ1B001545P",
                                   "EZJTZ1B001545P"};
    uint32_t             weight[] = {10, 20, 10, 20, 30};
    u_char              *key = "/foo/bar";
    u_char              *ret_ip;
    uint32_t             num;
    uint32_t             count = 5;

    nodes = (libchash_node_t *) calloc(count, sizeof(libchash_node_t));

    for (num = 0; num < count; num++) {
        nodes[num].name = name[num];
        nodes[num].name_len = strlen(name[num]);
        nodes[num].weight = weight[num];
        nodes[num].ip[0] = 22;
        nodes[num].ip[1] = 0;
        nodes[num].ip[2] = 0;
        nodes[num].ip[3] = num;
    }

    ctx = libchash_init(nodes, count);

    ret_ip = libchash_find(ctx, key, strlen(key));

    printf("ip is %d.%d.%d.%d\n", ret_ip[0], ret_ip[1], ret_ip[2], ret_ip[3]);

    libchash_exit(ctx);
    free(nodes);
}
