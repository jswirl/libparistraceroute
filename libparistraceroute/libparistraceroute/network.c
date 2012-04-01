#include <stdlib.h>
#include <stdio.h>
#include "network.h"
#include "packet.h"
#include "queue.h"

/******************************************************************************
 * Network
 ******************************************************************************/

void network_sniffer_handler(network_t *network, packet_t *packet)
{
    queue_push_packet(network->recvq, packet);
}

network_t* network_create(void)
{
    network_t *network;

    network = malloc(sizeof(network_t));
    if (!network)
        goto err_network;

    /* create a socket pool */
    network->socketpool = socketpool_create();
    if (!network->socketpool)
        goto err_socketpool;

    /* create the send queue */
    network->sendq = queue_create();
    if (!network->sendq)
        goto err_sendq;

    /* create the receive queue */
    network->recvq = queue_create();
    if (!network->recvq)
        goto err_recvq;

    /* create the sniffer */
    network->sniffer = sniffer_create(network, network_sniffer_handler);
    if (!network->sniffer) 
        goto err_sniffer;

    return network;

err_sniffer:
    queue_free(network->recvq);
err_recvq:
    queue_free(network->sendq);
err_sendq:
    socketpool_free(network->socketpool);
err_socketpool:
    free(network);
    network = NULL;
err_network:
    return NULL;
}

void network_free(network_t *network)
{
    sniffer_free(network->sniffer);
    queue_free(network->sendq);
    queue_free(network->recvq);
    socketpool_free(network->socketpool);
    free(network);
    network = NULL;
}

int network_get_sendq_fd(network_t *network)
{
    return queue_get_fd(network->sendq);
}

int network_get_recvq_fd(network_t *network)
{
    return queue_get_fd(network->recvq);
}

packet_t *packet_create_from_probe(probe_t *probe)
{
    //unsigned char  * payload;
    //size_t           payload_size;
    layer_t        * layer;
    size_t           size;
    int              i;
    unsigned char  * dip;
    unsigned short   dport;
    packet_t *packet;
    field_t *field;
    
    // XXX assert ipv4/udp/no payload (encode tag into checksum) 
    probe_dump(probe);
    
    field = probe_get_field(probe, "dst_ip");
    if (!field)
        return NULL; // missing dst ip
    dip = field->string_value;

    field = probe_get_field(probe, "dst_port");
    if (!field)
        return NULL; // missing dst port
    dport = field->int16_value;

    packet = packet_create(dip, dport);
    packet_set_buffer(packet, probe_get_buffer(probe));
    
    /* probe->buffer contains an incomplete packet being contructed :
     * missing parts are checksums and payload
     * NOTE: user checksum is not supported yet, since it is used to set a given
     * checksum for tagging the packet.
     */

    // 1) Write payload (need to have enough buffer space)
    //payload      = probe_get_payload(probe);
    //payload_size = probe_get_payload_size(probe);

    // 2) Go though the layers of the probe in the reverse order to write
    // checksums
    // XXX layer_t will require parent layer, and probe_t bottom_layer
    size = dynarray_get_size(probe->layers);
    for (i = size - 1; i >= 0; i--) {
        layer = dynarray_get_ith_element(probe->layers, i);
        printf("LAYER %d %s\n", i, layer->protocol->name);
        /* Does the protocol require a pseudoheader ? */
        if (layer->protocol->need_ext_checksum) {
            //layer_t        * layer_prev;
            //protocol_t     * protocol_prev;
            //pseudoheader_t * psh; // variant of a buffer

            if (i == 0)
                return NULL;

            //layer_prev = dynarray_get_ith_element(probe->layers, i-1);
            //protocol_prev = layer_prev->protocol;

            // TODO not implemented
            // psh = protocol->create_pseudo_header(protocol_prev, offsets[i-1]);
            // if (!psh)
            //     return NULL;
            //    protocol->write_checksum(data, psh);
            //    free(psh); 
            //    psh = NULL;

        } else {
            // could be a function in layer ?
            layer->protocol->write_checksum(layer->buffer, NULL);
        }
    }

    // 3) swap payload and IPv4 checksum

    return packet;

}


// not the right callback here
int network_send_probe(network_t *network, probe_t *probe, void (*callback)) 
{
    packet_t *packet;
    
    packet = packet_create_from_probe(probe);
    queue_push_packet(network->sendq, packet);

    return 0;
}

int network_process_sendq(network_t *network)
{
    packet_t *packet = queue_pop_packet(network->sendq);
    if (packet)
        socketpool_send_packet(network->socketpool, packet);
    return 0;
}

/**
 * \brief Process received packets: match them with a probe, or discard them.
 * \param network Pointer to a network structure
 *
 * Received packets, typically handled by the snifferTypically, the receive queue stores all the packets received by the sniffer.
 */
int network_process_recvq(network_t *network)
{
    return 0;
}
