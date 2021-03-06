/*=========================================================================*//**
@file    loop_ioctl.h

@author  Daniel Zorychta

@brief   Universal virtual device that allow to connect applications together

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/**
@defgroup drv-loop LOOP Driver

\section drv-loop-desc Description
Driver handles virtual loop device. Driver creates special node (file) that can
be handled by application (user space) instead of driver (kernel space). Driver
can be used as protocol translator or adjust layers e.g. translating access to
driver file to special network protocol. There is many driver appliances.

\section drv-loop-sup-arch Supported architectures
\li Any (noarch)

\section drv-loop-ddesc Details
\subsection drv-loop-ddesc-num Meaning of major and minor numbers
The major number determines ID of device. User can create up to 256 driver
nodes or up to free operating memory. There is no meaning of minor number.

\subsubsection drv-loop-ddesc-numres Numeration restrictions
The major number can be set up to 255. The minor number should be set always to 0.

\subsection drv-loop-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("LOOP", 0, 0, "/dev/loop0");
driver_init("LOOP", 1, 0, "/dev/loop1");
driver_init("LOOP", 2, 0, "/dev/loop2");
@endcode

\subsection drv-loop-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("LOOP", 0, 0);
driver_release("LOOP", 1, 0);
@endcode

\subsection drv-loop-ddesc-cfg Driver configuration
Driver does not support any configuration. Driver is ready to use after
initialization.

\subsection drv-loop-ddesc-write Data write
Data to the loop device can be written as regular file.

\subsection drv-loop-ddesc-read Data read
Data from the loop device can be read as regular file.

\subsection drv-loop-ddesc-fileop Other file operations
All file operations are allowed and handling is transparent for client. Only
host operations requires special handling.

\subsection drv-loop-ddesc-host Host mode
To handle incoming requests from loop device the host application should be
started. Application is responsible for handling entire traffic from and to
loop node. If request is not handled then client application is waiting for
operation to be done.

\subsubsection drv-loop-ddesc-host-open Registering host application
When host application is registered then is not possible to register additional
host application. Only one host can handle loop device. Application registration
example:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...

        // opening loop device file
        FILE *loop_dev = fopen(loop, "r+");
        if (!loop_dev) {
                perror(loop);
                exit(EXIT_FAILURE);
        }

        // ...

        // registering this application as host
        if (ioctl(loop_dev, IOCTL_LOOP__HOST_OPEN) != 0) {
                perror(loop);
                fclose(loop_dev);
                exit(EXIT_FAILURE);
        }

        // at this point the loop device is handling by this application
        // ...
\endcode

\subsubsection drv-loop-ddesc-host-close Unregistering host application
Only host application can unregister loop device. There is no possibility to
unregister loop device by other application. Unregister example code:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE *loop_dev;         // already registered device

        // ...

        // unregistering this application
        if (ioctl(loop_dev, IOCTL_LOOP__HOST_CLOSE) != 0) {
                perror(loop);
                fclose(loop_dev);
                exit(EXIT_FAILURE);
        }

        // at this point loop device is not handled by this application
        // ...
\endcode

\subsubsection drv-loop-ddesc-host-req Handling device requests
All incoming requests are generated by client applications accessing to the
loop device. All requests should be handled by host application. In this case
host application should check incoming request and then perform required action.
Example code:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>
        #include <stdbool.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE *loop_dev;         // already registered device

        // ...

        while (true) {
                LOOP_request_t client_req;
                if (ioctl(loop_dev, IOCTL_LOOP__HOST_WAIT_FOR_REQUEST, &client_req) != 0) {
                        perror(loop);
                        continue;
                }

                switch (client_req.cmd) {
                default:
                case LOOP_CMD__IDLE:
                        break;

                case LOOP_CMD__TRANSMISSION_CLIENT2HOST:
                        // ...
                        break;

                case LOOP_CMD__TRANSMISSION_HOST2CLIENT:
                        // ...
                        break;

                case LOOP_CMD__IOCTL_REQUEST:
                        // ...
                        break;

                case LOOP_CMD__FLUSH_BUFFERS:
                        // ...
                        break;

                case LOOP_CMD__DEVICE_STAT:
                        // ...
                        break;
                }
        }

        // ...
\endcode

\subsubsection drv-loop-ddesc-host-c2h Handling Client to Host request
This request is used when client application is sending data block to host
application. The host application can pass this data forward e.g. by using
network protocol or can redirect to other device, or even can handle by itself.
Example code of handling Client to Host request:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>
        #include <stdbool.h>
        #include <dnx/misc.h>
        #include <errno.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE   *loop_dev;         // already registered device
        uint8_t buffer[64];

        // ...

        void client2host_transaction(LOOP_request_t *rq)
        {
                size_t rd = rq.arg.rw.size;     // block size
                // The file position (rq.arg.rw.seek) can be used as well

                while (rd) {
                        size_t part = min(rd, sizeof(buffer));

                        // object used to transfer data from client
                        LOOP_buffer_t buf;
                        buf.data = buffer;
                        buf.size = part;
                        buf.err  = ESUCC;

                        int s = ioctl(loop_dev, IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT, &buf);
                        if (s == 0) {
                                // buffer is already filled, host can utilize buffer
                                // ...

                                rd -= part;
                        } else {
                                perror(loop);
                                break;
                        }
                }
        }

        // ...
\endcode

\subsubsection drv-loop-ddesc-host-h2c Handling Host to Client request
This request is used when host application is sending data block to client
application. Example code of handling Host to Client request:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>
        #include <stdbool.h>
        #include <dnx/misc.h>
        #include <errno.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE   *loop_dev;         // already registered device
        uint8_t buffer[64];

        // ...

        void host2client_transaction(LOOP_request_t *rq)
        {
                size_t wr = global->client_req.arg.rw.size;
                // The file position (rq.arg.rw.seek) can be used as well

                while (wr) {
                        // host should fill buffer
                        // ...
                        size_t n = ...;

                        LOOP_buffer_t buf;
                        buf.data = buffer;
                        buf.size = n;
                        buf.err  = ESUCC;

                        int s = ioctl(loop_dev, IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT, &buf);
                        if (s != 0) {
                                break;
                        } else {
                                wr -= n;
                        }
                }

                // if host has not more bytes then can finish transaction
                if (wr) {
                        LOOP_buffer_t buf;
                        buf.data = NULL;
                        buf.size = 0;
                        buf.err  = errno;
                        ioctl(loop_dev, IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT, &buf);
                }
        }

        // ...
\endcode

\subsubsection drv-loop-ddesc-host-ioctl Handling ioctl request
This request is used when client use ioctl() function. When ioctl() request is
specified for other device (not for loop driver) then is not necessary to use
@ref IOCTL_LOOP__CLIENT_REQUEST() macro. Only loop driver specified ioctl()
request should be send by using mentioned macro. Example code of handling ioctl
request:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>
        #include <stdbool.h>
        #include <dnx/misc.h>
        #include <errno.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE   *loop_dev;         // already registered device
        uint8_t buffer[64];

        // ...

        void ioctl_request(LOOP_request_t *rq)
        {
                LOOP_ioctl_response_t ioctl_resp;

                // handling request
                switch (rq.ioctl.request) {
                case ...:
                        // ...
                        // The rq.ioctl.arg can be used as well
                        ioctl_resp.err = ESUCC;
                        break;
                }

                if (ioctl(loop_dev, IOCTL_LOOP__HOST_SET_IOCTL_STATUS, &ioctl_resp) != 0) {
                        perror(loop);
                }
        }

        // ...
\endcode

\subsubsection drv-loop-ddesc-host-stat Handling device statistics request
The client program or operating system can check some device statistics. In this
the host application should handle statistics request. Example code:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>
        #include <stdbool.h>
        #include <dnx/misc.h>
        #include <errno.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE   *loop_dev;         // already registered device
        uint8_t buffer[64];

        // ...

        void stat_request(LOOP_request_t *rq)
        {
                LOOP_stat_response_t stat_resp;
                stat_resp.size = 100;         // example size
                stat_resp.err  = ESUCC;       // operation status

                if (ioctl(loop_dev, IOCTL_LOOP__HOST_SET_DEVICE_STATS, &stat_resp) != 0) {
                        perror(loop);
                }
        }

        // ...
\endcode

\subsubsection drv-loop-ddesc-host-flush Handling flush request
There are situations that output buffer should be flushed to e.g. solid state
disc. In this case the client application or operating system can send flush
request. Example code of handling flush request:
\code
        #include <stdio.h>
        #include <sys/ioctl.h>
        #include <stdbool.h>
        #include <dnx/misc.h>
        #include <errno.h>

        // ...
        const char *loop = "/dev/loop0";

        // ...
        FILE   *loop_dev;         // already registered device
        uint8_t buffer[64];

        // ...

        void flush_request(LOOP_request_t *rq)
        {
                // flush operation
                // ...

                int err = ESUCC;
                if (ioctl(loop_dev, IOCTL_LOOP__HOST_FLUSH_DONE, &err) != 0) {
                        perror(loop);
                }
        }

        // ...
\endcode

@{
*/

#ifndef _LOOP_IOCTL_H_
#define _LOOP_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Host request. Set this program as Host.
 *
 * After this operation this program has only possibility to gets requests from
 * Client program.
 *
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_OPEN                   _IO(LOOP, 0x00)

/**
 * @brief  Host request. Disconnect this program as host function.
 *
 * After this operation any program can be a host if request OPEN function.
 *
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_CLOSE                  _IO(LOOP, 0x01)

/**
 * @brief  Host request. Wait for request from Client.
 *
 * Client request and arguments are set in the loop_client_rq_t-type variable.
 * Host application should read this variable and perform requested actions.
 *
 * @param  [RD] @ref LOOP_request_t*             request details
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_WAIT_FOR_REQUEST       _IOR(LOOP, 0x02, LOOP_request_t*)

/**
 * @brief  Host request. Read data from buffers shared by the Client.
 *
 * When host send all bytes or send 0-length buffer then read operation is
 * finished. If operation is not finished, then timeout was generated.
 * If host wants to finish operation earlier the ZLB (Zero-Length Buffer)
 * should be send (buffer size = 0).
 *
 * @param  [RD] @ref LOOP_buffer_t*                host buffer descriptor
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT  _IOR(LOOP, 0x03, LOOP_buffer_t*)

/**
 * @brief  Host request. Write data to the client.
 *
 *  Write operation is finished and Client is resumed when buffer size is set
 *  to 0 (this means that Host written all data to the buffer) or all bytes was
 *  written. Write operation can be done by sending small buffers (is not
 *  required to send entire requested buffer in one part).
 *
 * @param  [WR] @ref LOOP_buffer_t*                host buffer descriptor
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT   _IOW(LOOP, 0x04, LOOP_buffer_t*)

/**
 * @brief  Host request. Response to the captured ioctl request.
 *
 * This request is send as response when Client requested IOCTL action in the command
 * capture request.
 *
 * @param  [WR] @ref LOOP_ioctl_response_t*        ioctl response data
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_SET_IOCTL_STATUS       _IOW(LOOP, 0x05, LOOP_ioctl_response_t*)

/**
 * @brief  Host request. Response to the captured stat request.
 * @param  [WR] @ref LOOP_stat_response_t*         device statistics response
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_SET_DEVICE_STATS       _IOW(LOOP, 0x06, LOOP_ioctl_response_t*)

/**
 * @brief  Host request. Response to the captured flush request
 * @param  [WR] int*                          errno value
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LOOP__HOST_FLUSH_DONE             _IOW(LOOP, 0x07, int*)

/**
 * @brief  Client request. General purpose RAW request. Depends on host protocol.
 *
 * By this request Client can send request from another device type.
 * In this case is not required to use @ref IOCTL_LOOP__CLIENT_REQUEST() macro.
 *
 * @param  n                            request number (macro's argument)
 * @return Depends on host program protocol.
 */
#define IOCTL_LOOP__CLIENT_REQUEST(n)          _IOWR(LOOP, 0x08 + n, void*)


/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent the command of loop device.
 */
typedef enum {
        LOOP_CMD__IDLE,                         //!< Idle command, no action.
        LOOP_CMD__TRANSMISSION_CLIENT2HOST,     //!< Transmission request client to host (rw arguments are valid).
        LOOP_CMD__TRANSMISSION_HOST2CLIENT,     //!< Transmission request host to client (rw arguments are valid).
        LOOP_CMD__IOCTL_REQUEST,                //!< Ioctl operation request (ioctl arguments are valid).
        LOOP_CMD__DEVICE_STAT,                  //!< Request to response by device's statistics.
        LOOP_CMD__FLUSH_BUFFERS                 //!< Request to flush device buffers.
} LOOP_cmd_t;


/**
 * Type represent buffer descriptor for I/O operations.
 */
typedef struct {
        u8_t    *data;                          /*!< Write/read Host buffer address.*/
        size_t   size;                          /*!< Number of bytes to write/read.*/
        int      err;                           /*!< Errno value if error occurred (if no error must be set to ESUCC).*/
} LOOP_buffer_t;


/**
 * Type represent the response host->client for the ioctl request coming from client.
 */
typedef struct {
        int err;                               /*!< Errno value if error occurred (if no error must be set to ESUCC).*/
} LOOP_ioctl_response_t;


/**
 * Type represent the response host->client for the stat() request coming from client.
 */
typedef struct {
        u64_t size;                             /*!< Device capacity/file size.*/
        int   err;                              /*!< Errno value if error occurred (if no error must be set to ESUCC).*/
} LOOP_stat_response_t;


/**
 * Type represent the request.
 */
typedef struct {
        LOOP_cmd_t cmd;                         /*!< Requested action (command from Client).*/

        union {
                struct {
                        size_t size;            /*!< Requested size of read/write operation.*/
                        fpos_t seek;            /*!< Position in the device's file.*/
                } rw;                           /*!< Read/write transmission arguments group.*/

                struct {
                        int   request;          /*!< Ioctl's request number.*/
                        void *arg;              /*!< Ioctl's request argument.*/
                } ioctl;                        /*!< Ioctl argument group.*/
        } arg;                                  /*!< Command's arguments.*/
} LOOP_request_t;


/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LOOP_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
