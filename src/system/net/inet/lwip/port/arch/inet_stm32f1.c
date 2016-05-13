/*=========================================================================*//**
@file    netman_stm32f1.c

@author  Daniel Zorychta

@brief   Network manager. Interface handle.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "inet_types.h"
#include "kernel/sysfunc.h"
#include "drivers/ioctl_requests.h"
#include "netif/etharp.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Function allocate memory for network interface.
 *         The function is called after that interface file was opened. Allocated
 *         memory pointer should be written to the inet->if_mem variable. If
 *         interface does not need additional memory then this value can be set
 *         to NULL and status should be returned as true.
 *
 * @param  inet         inet container
 *
 * @return One of @ref errno value.
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
int _inet_port_alloc(inet_t *inet)
{
        /*
         * The memory is not allocated because this interface does not need additional
         * memory (interface specific).
         */
        inet->if_mem = NULL;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function is called when network manager is closed
 *         The functin shall free memory allocated in the _netman_ifmem_alloc()
 *         function.
 *
 * @param  inet         inet container
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
void _inet_port_free(inet_t *inet)
{
        /*
         * Nothing to free because memory was not allocated for this interface.
         */
        inet->if_mem = NULL;
}

//==============================================================================
/**
 * @brief  Function initializes hardware interface.
 *         The function should be used to initialize interface parameters like
 *         MAC address, etc. By using this function the network interface should
 *         be started.
 *
 * @param  inet         inet container
 *
 * @return One of @ref errno value.
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
int _inet_port_hardware_init(inet_t *inet)
{
        /* set MAC address */
        int err = sys_ioctl(inet->if_file, IOCTL_ETHMAC__SET_MAC_ADDR, inet->netif.hwaddr);
        if (err) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_inet_port_hardware_init: MAC set fail\n"));
                return err;
        }

        /* start Ethernet interface */
        err = sys_ioctl(inet->if_file, IOCTL_ETHMAC__ETHERNET_START);
        if (err) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_inet_port_hardware_init: start fail\n"));
                return err;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function de-initialize configured network interface.
 *
 * @param  inet         inet container
 *
 * @return One of @ref errno value.
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
int _inet_port_hardware_deinit(inet_t *inet)
{
        int err = sys_ioctl(inet->if_file, IOCTL_ETHMAC__ETHERNET_STOP);
        if (err) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_inet_port_hardware_deinit: stop fail\n"));
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function receive packet from the network interface.
 *         This function should receive incoming packet from network interface.
 *         Function should try receive packet by specified time passed by
 *         'input_timeout'. If function does not receive any packet due to
 *         specified time then function should exit.
 *         If packet was received then the function should allocate buffer for
 *         transfer by using pbuf_alloc() function and put this buffer to then
 *         TCPIP stack by using inet->netif.input() function. If packets are
 *         received every loop then the function should not exit.
 *
 * @param  inet                 inet container
 * @param  input_timeout        packet receive timeout
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
void _inet_port_handle_input(inet_t *inet, u32_t timeout)
{
        ETHMAC_packet_wait_t pw = {.timeout = timeout};
        int r = sys_ioctl(inet->if_file, IOCTL_ETHMAC__WAIT_FOR_PACKET, &pw);

        while (r == 0 && pw.pkt_size > 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_netman_handle_input: packet size = %d\n", pw.pkt_size));

                // NOTE: subtract packet size by 4 to discard CRC32
                struct pbuf *p = pbuf_alloc(PBUF_RAW, pw.pkt_size - 4, PBUF_RAM);
                if (p) {
                        r = sys_ioctl(inet->if_file, IOCTL_ETHMAC__RECEIVE_PACKET_TO_CHAIN, p);

                        LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_netman_handle_input: received = %d\n", p->tot_len));

                        if (r == 0) {
                                if (inet->netif.input(p, &inet->netif) != ERR_OK) {
                                        pbuf_free(p);
                                } else {
                                        inet->rx_packets++;
                                        inet->rx_bytes += p->tot_len;
                                }
                        } else {
                                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_netman_handle_input: receive error\n"));
                                pbuf_free(p);
                        }

                        r = sys_ioctl(inet->if_file, IOCTL_ETHMAC__WAIT_FOR_PACKET, &pw);
                } else {
                        LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_netman_handle_input: not enough free memory\n"));
                        sys_sleep_ms(10);
                }
        }

        LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_netman_handle_input: packet receive timeout\n"));
}

//==============================================================================
/**
 * @brief  This function should do the actual transmission of the packet.
 *         The packet is contained in the pbuf that is passed to the function.
 *         This pbuf might be chained. To get access to the network manager
 *         object, use netif->state pointer.
 *
 * @param  netif        the lwip network interface structure
 * @param  p            the MAC packet to send (e.g. IP packet including MAC addresses and type)
 *
 * @return ERR_OK if the packet could be sent an err_t value if the packet couldn't be sent.
 *
 * @note   Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *         strange results. You might consider waiting for space in the DMA queue
 *         to become available since the stack doesn't retry to send a packet
 *         dropped because of memory failure (except for the TCP timers).
 *
 * @note   Called from TCPIP thread.
 */
//==============================================================================
err_t _inet_port_handle_output(struct netif *netif, struct pbuf *p)
{
      inet_t *inet = netif->state;

      if (sys_ioctl(inet->if_file, IOCTL_ETHMAC__SEND_PACKET_FROM_CHAIN, p) == 0) {
              inet->tx_packets++;
              inet->tx_bytes += p->tot_len;
              return ERR_OK;
      } else {
              LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("_netman_handle_output: packet send error\n"));
              return ERR_IF;
      }
}

//==============================================================================
/**
 * @brief  Function initializes network interface (TCPIP stack configuration).
 *         The function must not allocate any memory and block program flow.
 *
 * @param  netif        the lwip network interface structure for this interface
 *
 * @return ERR_OK       if the loopif is initialized
 *         ERR_MEM      if private data couldn't be allocated any other err_t on error
 *
 * @note   Called at system startup.
 */
//==============================================================================
err_t _inet_port_netif_init(struct netif *netif)
{
        netif->hostname   = const_cast(char*, __OS_HOSTNAME__);
        netif->name[0]    = 'E';
        netif->name[1]    = 'T';
        netif->output     = etharp_output;
        netif->linkoutput = _inet_port_handle_output;
        netif->mtu        = 1500;
        netif->hwaddr_len = ETHARP_HWADDR_LEN;
        netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
        netif->hwaddr[0]  = __NETWORK_TCPIP_MAC_ADDR0__;
        netif->hwaddr[1]  = __NETWORK_TCPIP_MAC_ADDR1__;
        netif->hwaddr[2]  = __NETWORK_TCPIP_MAC_ADDR2__;
        netif->hwaddr[3]  = __NETWORK_TCPIP_MAC_ADDR3__;
        netif->hwaddr[4]  = __NETWORK_TCPIP_MAC_ADDR4__;
        netif->hwaddr[5]  = __NETWORK_TCPIP_MAC_ADDR5__;

        return ERR_OK;
}

//==============================================================================
/**
 * @brief  Function check state of the network connection.
 *         The function is called when packet was not received (by timeout).
 *         The function should return a status of the physical network connection.
 *         If link is disconnected then function should return false, if link
 *         is connected then should return true.
 *
 * @param  inet                 inet container
 *
 * @return If link is connected then true is returned, otherwise false.
 *
 * @note   Called at system startup.
 */
//==============================================================================
bool _inet_port_is_link_connected(inet_t *inet)
{
        ETHMAC_link_status_t linkstat;
        if (sys_ioctl(inet->if_file, IOCTL_ETHMAC__GET_LINK_STATUS, &linkstat) == 0) {
                return linkstat == ETHMAC_LINK_STATUS__CONNECTED;
        } else {
                return false;
        }
}

/*==============================================================================
  End of file
==============================================================================*/