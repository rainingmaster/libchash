#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libchash.h"



#define CONSISTENT_POINTS 160
#define POW32             pow(2, 32)


static uint32_t  crc32_table16[] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};


static inline uint32_t
crc32_short(u_char *p, size_t len)
{
    u_char    c;
    uint32_t  crc;

    crc = 0xffffffff;

    while (len--) {
        c = *p++;
        crc = crc32_table16[(crc ^ (c & 0xf)) & 0xf] ^ (crc >> 4);
        crc = crc32_table16[(crc ^ (c >> 4)) & 0xf] ^ (crc >> 4);
    }

    return crc ^ 0xffffffff;
}


libchash_ctx_t*
libchash_init(libchash_node_t *nodes, uint32_t nnodes)
{
    chash_point_t    *points;
    libchash_ctx_t   *ctx;
    uint32_t          i;
    uint32_t          base_hash;
    uint32_t          num;
    uint32_t          npoints;
    uint32_t          start = 0;
    uint32_t          total_weight = 0;

    ctx = (libchash_ctx_t *) calloc(1, sizeof(libchash_ctx_t));
    if (! ctx) {
        return NULL;
    }

    for (i = 0; i < nnodes; i++) {
        total_weight += nodes[i].weight;
    }

    npoints = total_weight * CONSISTENT_POINTS;

    points = (chash_point_t *) calloc(npoints, sizeof(chash_point_t));
    if (! points) {
        free(ctx);
        return NULL;
    }

    for (i = 0; i < nnodes; i++) {
        num = nodes[i].weight * CONSISTENT_POINTS;
        base_hash = crc32_short(nodes[i].name, nodes[i].name_len) ^ 0xffffffff;
        /* index = 0 use in chash to sort */
        chash_point_init(points, base_hash, start, num, i + 1);

        start = start + num;
    }

    chash_point_sort(points, npoints);

    ctx->points  = points;
    ctx->npoints = npoints;
    ctx->nodes   = nodes;
    ctx->nnodes  = nnodes;

    return ctx;
}


u_char*
libchash_find(libchash_ctx_t *ctx, u_char *key, uint32_t key_len)
{
    uint32_t          i;
    uint32_t          step;
    uint32_t          hash;
    uint32_t          index;
    uint32_t          nodeindex;
    uint32_t          max_index;
    chash_point_t    *points;
    uint32_t          npoints;
    libchash_node_t  *nodes;

    hash = crc32_short(key, key_len);
    step = POW32 / npoints;
    index = (uint32_t)(hash / step);

    max_index = npoints - 1;

    /* it seems safer to do this */
    if (index > max_index) {
        index = max_index;
    }

    points    = ctx->points;
    npoints   = ctx->npoints;
    nodes     = ctx->nodes;

    nodeindex = points[0].id;

    /* find the first points >= hash */
    if (points[index].hash >= hash) {

        for (i = index; i > 0; i--) {

            if (points[i - 1].hash < hash) {
                nodeindex = points[i].id;
                break;
            }
        }

        /* index begin from 1 */
        return nodes[nodeindex - 1].ip;
    }

    for (i = index + 1; i <= max_index; i++) {
        if (hash <= points[i].hash) {
            nodeindex = points[i].id;
            break;
        }
    }

    /* index begin from 1 */
    return nodes[nodeindex - 1].ip;
}


void libchash_exit(libchash_ctx_t *ctx)
{
    chash_point_t    *points;

    points = ctx->points;
    free(points);
    free(ctx);
}
