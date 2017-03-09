#ifndef YUNFAN_LIBCHASH_H
#define YUNFAN_LIBCHASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chash.h"
#include <stdint.h>
#include <stdlib.h>


typedef struct {
    u_char    ip[4];
    u_char   *name;
    uint32_t  name_len;
    uint32_t  weight;
} libchash_node_t;

typedef struct {
    chash_point_t    *points;
    uint32_t          npoints;
    libchash_node_t  *nodes;
    uint32_t          nnodes;
}libchash_ctx_t;


libchash_ctx_t*
libchash_init(libchash_node_t *nodes, uint32_t nnodes);

u_char*
libchash_find(libchash_ctx_t *ctx, u_char *key, uint32_t key_len);

void libchash_exit(libchash_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
