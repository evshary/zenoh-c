/*
 * Copyright (c) 2017, 2020 ADLINK Technology Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
 * which is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *
 * Contributors:
 *   ADLINK zenoh team, <zenoh@adlink-labs.tech>
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "zenoh.h"

int main(int argc, char **argv)
{
    z_init_logger();

    char *uri = "/demo/example/zenoh-c-pub";
    if (argc > 1)
    {
        uri = argv[1];
    }
    char *value = "Pub from C!";
    if (argc > 2)
    {
        value = argv[2];
    }
    z_owned_config_t config = z_config_default();
    if (argc > 3)
    {
        z_config_set(z_borrow(config), ZN_CONFIG_PEER_KEY, argv[3]);
    }

    printf("Openning session...\n");
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s))
    {
        printf("Unable to open session!\n");
        exit(-1);
    }
    z_session_t borrowed_session = z_borrow(s);

    printf("Declaring Resource '%s'", uri);
    z_owned_keyexpr_t urikey = z_expr(uri);
    z_keyexpr_t keyexpr = z_register_resource(borrowed_session, z_move(urikey));
    printf(" => RId %lu\n", keyexpr.id);

    printf("Declaring Publisher on %lu\n", keyexpr.id);
    z_owned_publisher_t pub = z_publishing(borrowed_session, keyexpr);
    if (!z_check(pub))
    {
        printf("Unable to declare publisher.\n");
        exit(-1);
    }

    char buf[256];
    for (int idx = 0; 1; ++idx)
    {
        sleep(1);
        sprintf(buf, "[%4d] %s", idx, value);
        printf("Writing Data ('%lu': '%s')...\n", keyexpr.id, buf);
        z_write(borrowed_session, keyexpr, (const uint8_t *)buf, strlen(buf));
    }
    z_unregister_publisher(z_move(pub));
    z_close(z_move(s));
    return 0;
}