/*=========================================================================*//**
@file    spi.c

@author  Daniel Zorychta

@brief   SPI driver

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/driver.h"
#include "stm32f1/spi_cfg.h"
#include "stm32f1/spi_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MUTEX_TIMOUT             MAX_DELAY_MS
#define SEMAPHORE_TIMEOUT        MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
MODULE_NAME(SPI);

/* configuration of single CS line (port and pin) */
struct cs_pin_cfg {
        GPIO_t *const            port;
        u16_t                    pin_mask;
};

/* SPI peripheral configuration */
struct spi_config {
        SPI_t                   *SPI;
        __IO u32_t              *APBRSTR;
        __IO u32_t              *APBENR;
        u32_t                    APBRSTRENR_mask;
        IRQn_Type                IRQn;
        u32_t                    IRQ_priority;
        DMA_t                   *DMA;
    #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
        DMA_Channel_t           *DMA_Tx_channel;
        DMA_Channel_t           *DMA_Rx_channel;
        u8_t                     DMA_Rx_channel_number;
        IRQn_Type                DMA_Rx_IRQn;
        u32_t                    DMA_enable_mask;
    #endif
        u8_t                     number_of_slaves;
        const struct cs_pin_cfg *CS;
};

/* independent SPI instance */
struct spi_virtual {
        dev_lock_t               file_lock;
        u8_t                     major;
        u8_t                     minor;
        struct SPI_config        config;
};

/* general module data */
struct module {
        sem_t                   *wait_irq_sem[_NUMBER_OF_SPI_PERIPHERALS];
        mutex_t                 *device_protect_mtx[_NUMBER_OF_SPI_PERIPHERALS];
        const u8_t              *tx_buffer[_NUMBER_OF_SPI_PERIPHERALS];
        u8_t                    *rx_buffer[_NUMBER_OF_SPI_PERIPHERALS];
        size_t                   count[_NUMBER_OF_SPI_PERIPHERALS];
        bool                     RAW[_NUMBER_OF_SPI_PERIPHERALS];
        u8_t                     dummy_byte[_NUMBER_OF_SPI_PERIPHERALS];
        u8_t                     number_of_virtual_spi[_NUMBER_OF_SPI_PERIPHERALS];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int      turn_on_SPI          (u8_t major);
static void     turn_off_SPI         (u8_t major);
static void     apply_SPI_config     (struct spi_virtual *vspi);
static void     apply_SPI_safe_config(u8_t major);
static void     select_slave         (u8_t major, u8_t minor);
static void     deselect_slave       (u8_t major);
static int      transceive           (struct spi_virtual *hdl, const u8_t *tx, u8_t *rx, size_t count);

/*==============================================================================
  Local objects
==============================================================================*/
/* SPI1 CS slaves configuration */
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
static const struct cs_pin_cfg SPI1_CS_cfg[_SPI1_NUMBER_OF_SLAVES] = {
        #if (_SPI1_NUMBER_OF_SLAVES >= 1)
        {.port = (GPIO_t *)_SPI1_CS0_PORT, .pin_mask = _SPI1_CS0_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 2)
        {.port = (GPIO_t *)_SPI1_CS1_PORT, .pin_mask = _SPI1_CS1_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 3)
        {.port = (GPIO_t *)_SPI1_CS2_PORT, .pin_mask = _SPI1_CS2_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 4)
        {.port = (GPIO_t *)_SPI1_CS3_PORT, .pin_mask = _SPI1_CS3_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 5)
        {.port = (GPIO_t *)_SPI1_CS4_PORT, .pin_mask = _SPI1_CS4_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 6)
        {.port = (GPIO_t *)_SPI1_CS5_PORT, .pin_mask = _SPI1_CS5_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 7)
        {.port = (GPIO_t *)_SPI1_CS6_PORT, .pin_mask = _SPI1_CS6_PIN_BM},
        #endif
        #if (_SPI1_NUMBER_OF_SLAVES >= 8)
        {.port = (GPIO_t *)_SPI1_CS7_PORT, .pin_mask = _SPI1_CS7_PIN_BM}
        #endif
};
#endif

/* SPI2 CS slaves configuration */
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
static const struct cs_pin_cfg SPI2_CS_cfg[_SPI2_NUMBER_OF_SLAVES] = {
        #if (_SPI2_NUMBER_OF_SLAVES >= 1)
        {.port = (GPIO_t *)_SPI2_CS0_PORT, .pin_mask = _SPI2_CS0_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 2)
        {.port = (GPIO_t *)_SPI2_CS1_PORT, .pin_mask = _SPI2_CS1_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 3)
        {.port = (GPIO_t *)_SPI2_CS2_PORT, .pin_mask = _SPI2_CS2_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 4)
        {.port = (GPIO_t *)_SPI2_CS3_PORT, .pin_mask = _SPI2_CS3_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 5)
        {.port = (GPIO_t *)_SPI2_CS4_PORT, .pin_mask = _SPI2_CS4_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 6)
        {.port = (GPIO_t *)_SPI2_CS5_PORT, .pin_mask = _SPI2_CS5_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 7)
        {.port = (GPIO_t *)_SPI2_CS6_PORT, .pin_mask = _SPI2_CS6_PIN_BM},
        #endif
        #if (_SPI2_NUMBER_OF_SLAVES >= 8)
        {.port = (GPIO_t *)_SPI2_CS7_PORT, .pin_mask = _SPI2_CS7_PIN_BM}
        #endif
};
#endif

/* SPI3 CS slaves configuration */
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
static const struct cs_pin_cfg SPI3_CS_cfg[_SPI3_NUMBER_OF_SLAVES] = {
        #if (_SPI3_NUMBER_OF_SLAVES >= 1)
        {.port = (GPIO_t *)_SPI3_CS0_PORT, .pin_mask = _SPI3_CS0_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 2)
        {.port = (GPIO_t *)_SPI3_CS1_PORT, .pin_mask = _SPI3_CS1_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 3)
        {.port = (GPIO_t *)_SPI3_CS2_PORT, .pin_mask = _SPI3_CS2_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 4)
        {.port = (GPIO_t *)_SPI3_CS3_PORT, .pin_mask = _SPI3_CS3_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 5)
        {.port = (GPIO_t *)_SPI3_CS4_PORT, .pin_mask = _SPI3_CS4_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 6)
        {.port = (GPIO_t *)_SPI3_CS5_PORT, .pin_mask = _SPI3_CS5_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 7)
        {.port = (GPIO_t *)_SPI3_CS6_PORT, .pin_mask = _SPI3_CS6_PIN_BM},
        #endif
        #if (_SPI3_NUMBER_OF_SLAVES >= 8)
        {.port = (GPIO_t *)_SPI3_CS7_PORT, .pin_mask = _SPI3_CS7_PIN_BM}
        #endif
};
#endif


/* SPI peripherals basic parameters */
static const struct spi_config SPI_cfg[_NUMBER_OF_SPI_PERIPHERALS] = {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        {
                .APBENR                = &RCC->APB2ENR,
                .APBRSTR               = &RCC->APB2RSTR,
                .APBRSTRENR_mask       = RCC_APB2ENR_SPI1EN,
                .IRQn                  = SPI1_IRQn,
                .IRQ_priority          = _SPI1_IRQ_PRIORITY,
                .SPI                   = SPI1,
                .DMA                   = _SPI1_USE_DMA ? DMA1 : NULL,
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                .DMA_Tx_channel        = DMA1_Channel3,
                .DMA_Rx_channel        = DMA1_Channel2,
                .DMA_Rx_IRQn           = DMA1_Channel2_IRQn,
                .DMA_Rx_channel_number = 2,
                .DMA_enable_mask       = RCC_AHBENR_DMA1EN,
                #endif
                .number_of_slaves      = _SPI1_NUMBER_OF_SLAVES,
                .CS                    = SPI1_CS_cfg
        },
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        {
                .APBENR                = &RCC->APB1ENR,
                .APBRSTR               = &RCC->APB1RSTR,
                .APBRSTRENR_mask       = RCC_APB1ENR_SPI2EN,
                .IRQn                  = SPI2_IRQn,
                .IRQ_priority          = _SPI2_IRQ_PRIORITY,
                .SPI                   = SPI2,
                .DMA                   = _SPI2_USE_DMA ? DMA1 : NULL,
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                .DMA_Tx_channel        = DMA1_Channel5,
                .DMA_Rx_channel        = DMA1_Channel4,
                .DMA_Rx_IRQn           = DMA1_Channel4_IRQn,
                .DMA_Rx_channel_number = 4,
                .DMA_enable_mask       = RCC_AHBENR_DMA1EN,
                #endif
                .number_of_slaves      = _SPI2_NUMBER_OF_SLAVES,
                .CS                    = SPI2_CS_cfg
        },
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        {
                .APBENR                = &RCC->APB1ENR,
                .APBRSTR               = &RCC->APB1RSTR,
                .APBRSTRENR_mask       = RCC_APB1ENR_SPI3EN,
                .IRQn                  = SPI3_IRQn,
                .IRQ_priority          = _SPI3_IRQ_PRIORITY,
                .SPI                   = SPI3,
                .DMA                   = _SPI3_USE_DMA ? DMA2 : NULL,
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                .DMA_Tx_channel        = DMA2_Channel2,
                .DMA_Rx_channel        = DMA2_Channel1,
                .DMA_Rx_IRQn           = DMA2_Channel1_IRQn,
                .DMA_Rx_channel_number = 1,
                .DMA_enable_mask       = RCC_AHBENR_DMA2EN,
                #endif
                .number_of_slaves      = _SPI3_NUMBER_OF_SLAVES,
                .CS                    = SPI3_CS_cfg
        }
        #endif
};

/* default SPI config */
static const struct SPI_config spi_default_cfg = {
        .dummy_byte  = _SPI_DEFAULT_CFG_DUMMY_BYTE,
        .clk_divider = _SPI_DEFAULT_CFG_CLK_DIVIDER,
        .mode        = _SPI_DEFAULT_CFG_MODE,
        .msb_first   = _SPI_DEFAULT_CFG_MSB_FIRST
};

/* pointers to memory of specified device */
static struct module *SPIM;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(SPI, void **device_handle, u8_t major, u8_t minor)
{
        int result = ENXIO;

        if (major >= _NUMBER_OF_SPI_PERIPHERALS) {
                return result;
        }

#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        if (major == _SPI1 && minor >= _SPI1_NUMBER_OF_SLAVES) {
                return result;
        }
#endif

#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        if (major == _SPI2 && minor >= _SPI2_NUMBER_OF_SLAVES) {
                return result;
        }
#endif

#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        if (major == _SPI3 && minor >= _SPI3_NUMBER_OF_SLAVES) {
                return result;
        }
#endif

        /* allocate module memory */
        if (!SPIM) {
                result = _sys_zalloc(sizeof(struct module), static_cast(void**, &SPIM));
                if (result != ESUCC)
                        return result;
        }

        /* create SPI peripheral instance */
        if (SPIM->wait_irq_sem[major] == NULL) {
                result = _sys_semaphore_create(1, 0, &SPIM->wait_irq_sem[major]);
                if (result != ESUCC)
                        goto finish;

                result = _sys_mutex_create(MUTEX_TYPE_RECURSIVE, &SPIM->device_protect_mtx[major]);
                if (result != ESUCC)
                        goto finish;

                result = turn_on_SPI(major);
                if (result == ESUCC) {
                        if (SPI_cfg[major].DMA) {
                                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                                RCC->AHBENR |= SPI_cfg[major].DMA_enable_mask;

                                u32_t DMA_IRQ_mask = (DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_TEIF1);
                                u8_t  ch_position  = (SPI_cfg[major].DMA_Rx_channel_number - 1) * 4;
                                SPI_cfg[major].DMA->IFCR = (DMA_IRQ_mask << ch_position);
                                SPI_cfg[major].DMA->ISR |= (DMA_IRQ_mask << ch_position);

                                NVIC_EnableIRQ(SPI_cfg[major].DMA_Rx_IRQn);
                                NVIC_SetPriority(SPI_cfg[major].DMA_Rx_IRQn, SPI_cfg[major].IRQ_priority);
                                #endif
                        } else {
                                #if (_SPI1_USE_DMA == 0 && _SPI1_ENABLE) || (_SPI2_USE_DMA == 0 && _SPI2_ENABLE) || (_SPI3_USE_DMA == 0 && _SPI3_ENABLE)
                                NVIC_EnableIRQ(SPI_cfg[major].IRQn);
                                NVIC_SetPriority(SPI_cfg[major].IRQn, SPI_cfg[major].IRQ_priority);
                                #endif
                        }
                }

                finish:
                if (result != ESUCC) {
                        if (SPIM->wait_irq_sem[major]) {
                                _sys_semaphore_destroy(SPIM->wait_irq_sem[major]);
                                SPIM->wait_irq_sem[major] = NULL;
                        }

                        if (SPIM->device_protect_mtx[major]) {
                                _sys_mutex_destroy(SPIM->device_protect_mtx[major]);
                                SPIM->device_protect_mtx[major] = NULL;
                        }
                }
        }

        /* create new instance for specified major-minor number (virtual SPI) */
        if (SPIM->wait_irq_sem[major]) {
                result = _sys_zalloc(sizeof(struct spi_virtual), device_handle);
                if (result == ESUCC) {
                        struct spi_virtual *hdl = *device_handle;

                        hdl->config = spi_default_cfg;
                        hdl->major  = major;
                        hdl->minor  = minor;

                        SPIM->number_of_virtual_spi[major]++;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(SPI, void *device_handle)
{
        struct spi_virtual *hdl = device_handle;

        if (_sys_device_is_unlocked(hdl->file_lock)) {

                _sys_critical_section_begin();

                SPIM->number_of_virtual_spi[hdl->major]--;

                /* deinitialize major device if all minor devices are deinitialized */
                if (SPIM->number_of_virtual_spi[hdl->major] == 0) {
                        _sys_mutex_destroy(SPIM->device_protect_mtx[hdl->major]);
                        SPIM->device_protect_mtx[hdl->major] = NULL;

                        _sys_semaphore_destroy(SPIM->wait_irq_sem[hdl->major]);
                        SPIM->wait_irq_sem[hdl->major] = NULL;

                        turn_off_SPI(hdl->major);

                        if (SPI_cfg[hdl->major].DMA) {
                                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                                NVIC_DisableIRQ(SPI_cfg[hdl->major].DMA_Rx_IRQn);
                                #endif
                        } else {
                                #if (_SPI1_USE_DMA == 0 && _SPI1_ENABLE) || (_SPI2_USE_DMA == 0 && _SPI2_ENABLE) || (_SPI3_USE_DMA == 0 && _SPI3_ENABLE)
                                NVIC_DisableIRQ(SPI_cfg[hdl->major].IRQn);
                                #endif
                        }
                }

                /* free module memory if all devices are deinitialized */
                bool free_module_mem = true;
                for (int i = 0; i < _NUMBER_OF_SPI_PERIPHERALS && free_module_mem; i++) {
                        free_module_mem = SPIM->device_protect_mtx[i] == NULL;
                }

                if (free_module_mem) {
                        _sys_free(reinterpret_cast(void**, &SPIM));
                }

                _sys_critical_section_end();

                /* free virtual spi memory */
                _sys_free(device_handle);

                return ESUCC;
        } else {
                return EBUSY;
        }
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(SPI, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        struct spi_virtual *hdl = device_handle;

        return _sys_device_lock(&hdl->file_lock) ? ESUCC : EBUSY;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(SPI, void *device_handle, bool force)
{
        struct spi_virtual *hdl = device_handle;

        if (_sys_device_is_access_granted(&hdl->file_lock) || force) {
                _sys_device_unlock(&hdl->file_lock, force);
                return ESUCC;
        } else {
                return EBUSY;
        }
}

//==============================================================================
/**
 * @brief Write data to device. Function does not check that current task is
 *        allowed to write. This is filtered by the open() function. This design
 *        helps to handle other modules that uses directly files to transfer
 *        data. This allows modules to access file from other tasks
 *        (application tasks that not opened SPI file).
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(SPI,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fpos);
        UNUSED_ARG1(fattr);

        struct spi_virtual *hdl    = device_handle;

        int status = _sys_mutex_lock(SPIM->device_protect_mtx[hdl->major], MUTEX_TIMOUT);
        if (status == ESUCC) {
                if (SPIM->RAW[hdl->major] == false) {
                        deselect_slave(hdl->major);
                        apply_SPI_config(hdl);
                        select_slave(hdl->major, hdl->minor);
                }

                status = transceive(hdl, src, NULL, count);
                if (status == ESUCC) {
                        *wrcnt = count;
                }

                _sys_mutex_unlock(SPIM->device_protect_mtx[hdl->major]);
        }

        return status;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(SPI,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fpos);
        UNUSED_ARG1(fattr);

        struct spi_virtual *hdl    = device_handle;

        int status = _sys_mutex_lock(SPIM->device_protect_mtx[hdl->major], MUTEX_TIMOUT);
        if (status == ESUCC) {
                if (SPIM->RAW[hdl->major] == false) {
                        deselect_slave(hdl->major);
                        apply_SPI_config(hdl);
                        select_slave(hdl->major, hdl->minor);
                }

                status = transceive(hdl, NULL, dst, count);
                if (status == ESUCC) {
                        *rdcnt = count;
                }

                _sys_mutex_unlock(SPIM->device_protect_mtx[hdl->major]);
        }

        return status;
}

//==============================================================================
/**
 * @brief IO control.  Function does not check that current task is
 *        allowed to write. This is filtered by the open() function. This design
 *        helps to handle other modules that uses directly files to transfer
 *        data. This allows modules to access file from other tasks
 *        (application tasks that not opened SPI file).
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(SPI, void *device_handle, int request, void *arg)
{
        struct spi_virtual *hdl    = device_handle;
        int                 status = EIO;

        switch (request) {
        case IOCTL_SPI__SET_CONFIGURATION:
                if (arg) {
                        hdl->config = *reinterpret_cast(struct SPI_config*, arg);
                        status      = ESUCC;
                } else {
                        status = EINVAL;
                }
                break;

        case IOCTL_SPI__GET_CONFIGURATION:
                if (arg) {
                        *reinterpret_cast(struct SPI_config*, arg) = hdl->config;
                        status = ESUCC;
                } else {
                        status = EINVAL;
                }
                break;

        case IOCTL_SPI__SELECT:
                if (_sys_mutex_trylock(SPIM->device_protect_mtx[hdl->major]) == ESUCC) {
                        SPIM->RAW[hdl->major] = true;
                        apply_SPI_config(hdl);
                        deselect_slave(hdl->major);
                        select_slave(hdl->major, hdl->minor);
                        status = ESUCC;
                } else {
                        status = EBUSY;
                }
                break;

        case IOCTL_SPI__DESELECT:
                if (_sys_mutex_trylock(SPIM->device_protect_mtx[hdl->major]) == ESUCC) {
                        deselect_slave(hdl->major);
                        apply_SPI_safe_config(hdl->major);
                        SPIM->RAW[hdl->major] = false;
                        _sys_mutex_unlock(SPIM->device_protect_mtx[hdl->major]);
                        _sys_mutex_unlock(SPIM->device_protect_mtx[hdl->major]);
                        status = ESUCC;
                } else {
                        status = EBUSY;
                }
                break;

        case IOCTL_SPI__TRANSCEIVE:
                if (arg) {
                        struct SPI_transceive *tr = reinterpret_cast(struct SPI_transceive*, arg);

                        if (tr->count) {
                                if (_sys_mutex_trylock(SPIM->device_protect_mtx[hdl->major]) == ESUCC) {
                                        if (SPIM->RAW[hdl->major] == false) {
                                                deselect_slave(hdl->major);
                                                apply_SPI_config(hdl);
                                                select_slave(hdl->major, hdl->minor);
                                        }

                                        status = transceive(hdl, tr->tx_buffer, tr->rx_buffer, tr->count);

                                        _sys_mutex_unlock(SPIM->device_protect_mtx[hdl->major]);
                                }
                        } else {
                                status = EINVAL;
                        }
                } else {
                        status = EINVAL;
                }
                break;

        case IOCTL_SPI__TRANSMIT_NO_SELECT:
                if (arg) {
                        int byte = reinterpret_cast(int, arg);

                        if (_sys_mutex_trylock(SPIM->device_protect_mtx[hdl->major]) == ESUCC) {
                                deselect_slave(hdl->major);
                                apply_SPI_config(hdl);

                                if (transceive(hdl, static_cast(u8_t*, &byte), NULL, 1)) {
                                        status = ESUCC;
                                }

                                _sys_mutex_unlock(SPIM->device_protect_mtx[hdl->major]);
                        }
                } else {
                        status = EINVAL;
                }
                break;

        default:
                status = EBADRQC;
                break;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(SPI, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(SPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        struct spi_virtual *hdl = device_handle;

        device_stat->st_major = hdl->major;
        device_stat->st_minor = hdl->minor;
        device_stat->st_size  = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function enable SPI interface
 * @param[in] major     SPI major number
 * @return One of errno value.
 */
//==============================================================================
static int turn_on_SPI(u8_t major)
{
        if (!(*SPI_cfg[major].APBENR & SPI_cfg[major].APBRSTRENR_mask)) {
                *SPI_cfg[major].APBRSTR |=  SPI_cfg[major].APBRSTRENR_mask;
                *SPI_cfg[major].APBRSTR &= ~SPI_cfg[major].APBRSTRENR_mask;
                *SPI_cfg[major].APBENR  |=  SPI_cfg[major].APBRSTRENR_mask;
                return ESUCC;
        } else {
                return EADDRINUSE;
        }
}

//==============================================================================
/**
 * @brief Function disable SPI interface
 * @param[in] major     SPI major number
 * @return None
 */
//==============================================================================
static void turn_off_SPI(u8_t major)
{
        *SPI_cfg[major].APBRSTR |=  SPI_cfg[major].APBRSTRENR_mask;
        *SPI_cfg[major].APBRSTR &= ~SPI_cfg[major].APBRSTRENR_mask;
        *SPI_cfg[major].APBENR  &= ~SPI_cfg[major].APBRSTRENR_mask;
}

//==============================================================================
/**
 * @brief Function apply new configuration for selected SPI
 *
 * @param vspi          virtual spi handler
 */
//==============================================================================
static void apply_SPI_config(struct spi_virtual *vspi)
{
        const u16_t divider_mask[SPI_CLK_DIV_256 + 1] = {
                [SPI_CLK_DIV_2  ] = 0x00,
                [SPI_CLK_DIV_4  ] = SPI_CR1_BR_0,
                [SPI_CLK_DIV_8  ] = SPI_CR1_BR_1,
                [SPI_CLK_DIV_16 ] = SPI_CR1_BR_1 | SPI_CR1_BR_0,
                [SPI_CLK_DIV_32 ] = SPI_CR1_BR_2,
                [SPI_CLK_DIV_64 ] = SPI_CR1_BR_2 | SPI_CR1_BR_0,
                [SPI_CLK_DIV_128] = SPI_CR1_BR_2 | SPI_CR1_BR_1,
                [SPI_CLK_DIV_256] = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
        };

        const u16_t spi_mode_mask[SPI_MODE_3 + 1] = {
                [SPI_MODE_0] = 0x00,
                [SPI_MODE_1] = SPI_CR1_CPHA,
                [SPI_MODE_2] = SPI_CR1_CPOL,
                [SPI_MODE_3] = SPI_CR1_CPOL | SPI_CR1_CPHA
        };

        SPI_t *SPI = SPI_cfg[vspi->major].SPI;

        /* configure SPI divider */
        CLEAR_BIT(SPI->CR1, SPI_CR1_BR);
        SET_BIT(SPI->CR1, divider_mask[vspi->config.clk_divider]);

        /* configure SPI mode */
        CLEAR_BIT(SPI->CR1, SPI_CR1_CPOL | SPI_CR1_CPHA);
        SET_BIT(SPI->CR1, spi_mode_mask[vspi->config.mode]);

        /* 8-bit mode */
        CLEAR_BIT(SPI->CR1, SPI_CR1_DFF);

        /* set MSB/LSB */
        if (vspi->config.msb_first)
                CLEAR_BIT(SPI->CR1, SPI_CR1_LSBFIRST);
        else
                SET_BIT(SPI->CR1, SPI_CR1_LSBFIRST);

        /* NSS software mode */
        SET_BIT(SPI->CR1, SPI_CR1_SSM | SPI_CR1_SSI);

        /* set SPI as master */
        SET_BIT(SPI->CR1, SPI_CR1_MSTR);

        /* enable peripheral */
        SET_BIT(SPI->CR1, SPI_CR1_SPE);
}

//==============================================================================
/**
 * @brief Function apply SPI device safe configuration
 *
 * @param major         SPI major number
 */
//==============================================================================
static void apply_SPI_safe_config(u8_t major)
{
        SPI_t *SPI = SPI_cfg[major].SPI;

        while (SPI->SR & SPI_SR_BSY)
        CLEAR_BIT(SPI->CR1, SPI_CR1_SPE);
        SET_BIT(SPI->CR1, SPI_CR1_MSTR);
}

//==============================================================================
/**
 * @brief Function select slave device
 *
 * @param major         major device number (SPI device)
 * @param minor         minor device number (CS number)
 */
//==============================================================================
static void select_slave(u8_t major, u8_t minor)
{
        GPIO_t *GPIO = SPI_cfg[major].CS[minor].port;
        u16_t   mask = SPI_cfg[major].CS[minor].pin_mask;

        if (GPIO) {
                GPIO->BRR = mask;
        }
}

//==============================================================================
/**
 * @brief Function deselect current slave device
 *
 * @param major         major device number (SPI device)
 * @param minor         minor device number (CS number)
 */
//==============================================================================
static void deselect_slave(u8_t major)
{
        for (int minor = 0; minor < SPI_cfg[major].number_of_slaves; minor++) {
                GPIO_t *GPIO = SPI_cfg[major].CS[minor].port;
                u16_t   mask = SPI_cfg[major].CS[minor].pin_mask;

                if (GPIO) {
                        GPIO->BSRR = mask;
                }
        }
}

//==============================================================================
/**
 * @brief  Transceive data by using IRQs or DMA
 * @param  hdl          virtual SPI handler
 * @param  tx           source buffer
 * @param  rx           destination buffer
 * @param  count        number of bytes to transfer
 * @return One of errno value
 */
//==============================================================================
static int transceive(struct spi_virtual *hdl, const u8_t *tx, u8_t *rx, size_t count)
{
        if (SPI_cfg[hdl->major].DMA) {
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                SPIM->dummy_byte[hdl->major] = hdl->config.dummy_byte;
                SPIM->count[hdl->major]      = count;

                SPI_cfg[hdl->major].DMA_Tx_channel->CPAR  = reinterpret_cast(u32_t, &SPI_cfg[hdl->major].SPI->DR);
                SPI_cfg[hdl->major].DMA_Tx_channel->CMAR  = tx ? reinterpret_cast(u32_t, tx) : reinterpret_cast(u32_t, &SPIM->dummy_byte[hdl->major]);
                SPI_cfg[hdl->major].DMA_Tx_channel->CNDTR = count;
                SPI_cfg[hdl->major].DMA_Tx_channel->CCR   = (tx ? DMA_CCR1_MINC : 0) | DMA_CCR1_DIR;

                SPI_cfg[hdl->major].DMA_Rx_channel->CPAR  = reinterpret_cast(u32_t, &SPI_cfg[hdl->major].SPI->DR);
                SPI_cfg[hdl->major].DMA_Rx_channel->CMAR  = rx ? reinterpret_cast(u32_t, rx) : reinterpret_cast(u32_t, &SPIM->dummy_byte[hdl->major]);
                SPI_cfg[hdl->major].DMA_Rx_channel->CNDTR = count;
                SPI_cfg[hdl->major].DMA_Rx_channel->CCR   = (rx ? DMA_CCR1_MINC : 0) | DMA_CCR1_TEIE | DMA_CCR1_TCIE;

                SET_BIT(SPI_cfg[hdl->major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
                SET_BIT(SPI_cfg[hdl->major].DMA_Rx_channel->CCR, DMA_CCR1_EN);
                SET_BIT(SPI_cfg[hdl->major].DMA_Tx_channel->CCR, DMA_CCR1_EN);
                #endif
        } else {
                #if (_SPI1_USE_DMA == 0 && _SPI1_ENABLE) || (_SPI2_USE_DMA == 0 && _SPI2_ENABLE) || (_SPI3_USE_DMA == 0 && _SPI3_ENABLE)
                SPIM->tx_buffer[hdl->major]  = tx;
                SPIM->rx_buffer[hdl->major]  = rx;
                SPIM->count[hdl->major]      = count;
                SPIM->dummy_byte[hdl->major] = hdl->config.dummy_byte;

                SET_BIT(SPI_cfg[hdl->major].SPI->CR2, SPI_CR2_TXEIE);
                #endif
        }

        return _sys_semaphore_wait(SPIM->wait_irq_sem[hdl->major], SEMAPHORE_TIMEOUT);
}

//==============================================================================
/**
 * @brief  Function handle SPI IRQ
 * @param  major        SPI device number
 * @return If task was woken then true is returned, otherwise false
 */
//==============================================================================
#if (_SPI1_USE_DMA == 0 && _SPI1_ENABLE) || (_SPI2_USE_DMA == 0 && _SPI2_ENABLE) || (_SPI3_USE_DMA == 0 && _SPI3_ENABLE)
static bool handle_SPI_IRQ(u8_t major)
{
        bool woken = false;
        SPI_t *SPI = SPI_cfg[major].SPI;

        /* receive data from RX register */
        void receive()
        {
                if ((SPI->SR & SPI_SR_RXNE) && (SPI->CR2 & SPI_CR2_RXNEIE)) {
                        u8_t byte = SPI->DR;

                        if (SPIM->count[major] > 0) {
                                if (SPIM->rx_buffer[major]) {
                                        *(SPIM->rx_buffer[major]++) = byte;
                                }

                                SPIM->count[major]--;
                        }

                        SET_BIT(SPI->CR2, SPI_CR2_TXEIE);
                }
        }

        /* transmit data by using Tx register */
        void transmit()
        {
                if ((SPI->SR & SPI_SR_TXE) && (SPI->CR2 & SPI_CR2_TXEIE)) {

                        if (SPIM->count[major] > 0) {
                                if (SPIM->tx_buffer[major]) {
                                        SPI->DR = *(SPIM->tx_buffer[major]++);
                                } else {
                                        SPI->DR = SPIM->dummy_byte[major];
                                }
                        }

                        SET_BIT(SPI->CR2, SPI_CR2_RXNEIE);
                        CLEAR_BIT(SPI->CR2, SPI_CR2_TXEIE);
                }
        }

        /* finish transmission if all frames are received and transmitted */
        void check_finish()
        {
                if (SPIM->count[major] == 0) {
                        if (SPIM->RAW[major] == false) {
                                deselect_slave(major);
                        }

                        CLEAR_BIT(SPI->CR2, SPI_CR2_RXNEIE);
                        CLEAR_BIT(SPI->CR2, SPI_CR2_TXEIE);
                        _sys_semaphore_signal_from_ISR(SPIM->wait_irq_sem[major], &woken);
                }
        }

        /* IRQ operations */
        transmit();
        receive();
        check_finish();

        return woken;
}
#endif

//==============================================================================
/**
 * @brief  DMA IRQ handler
 * @param  major        SPI major number
 * @return If task was woken then true is returned, otherwise false
 */
//==============================================================================
#if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
static bool handle_DMA_IRQ(u8_t major)
{
        u32_t DMA_IRQ_mask = (DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_TEIF1);
        u8_t  ch_position  = (SPI_cfg[major].DMA_Rx_channel_number - 1) * 4;
        SPI_cfg[major].DMA->IFCR = (DMA_IRQ_mask << ch_position);

        CLEAR_BIT(SPI_cfg[major].DMA_Rx_channel->CCR, DMA_CCR1_EN);
        CLEAR_BIT(SPI_cfg[major].DMA_Tx_channel->CCR, DMA_CCR1_EN);
        CLEAR_BIT(SPI_cfg[major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);

        if (SPIM->RAW[major] == false) {
                deselect_slave(major);
        }

        bool woken = false;
        _sys_semaphore_signal_from_ISR(SPIM->wait_irq_sem[major], &woken);
        woken = woken || SPIM->count[major] > 10;

        return woken;
}
#endif

//==============================================================================
/**
 * @brief SPI1 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0) && (_SPI1_USE_DMA == 0)
void SPI1_IRQHandler(void)
{
        if (handle_SPI_IRQ(_SPI1)) {
                _sys_task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief SPI2 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0) && (_SPI2_USE_DMA == 0)
void SPI2_IRQHandler(void)
{
        if (handle_SPI_IRQ(_SPI2)) {
                _sys_task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief SPI3 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0) && (_SPI3_USE_DMA == 0)
void SPI3_IRQHandler(void)
{
        if (handle_SPI_IRQ(_SPI3)) {
                _sys_task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief DMA Rx transfer complete IRQ for SPI1
 */
//==============================================================================
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0) && (_SPI1_USE_DMA > 0)
void DMA1_Channel2_IRQHandler(void)
{
        if (handle_DMA_IRQ(_SPI1)) {
                _sys_task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief DMA Rx transfer complete IRQ for SPI2
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0) && (_SPI2_USE_DMA > 0)
void DMA1_Channel4_IRQHandler(void)
{
        if (handle_DMA_IRQ(_SPI2)) {
                _sys_task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief DMA Rx transfer complete IRQ for SPI3
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0) && (_SPI3_USE_DMA > 0)
void DMA2_Channel1_IRQHandler(void)
{
        if (handle_DMA_IRQ(_SPI3)) {
                _sys_task_yield_from_ISR();
        }
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
