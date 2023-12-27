/*
 * Configuration.c
 *
 *  Created on: 27 Dec 2023
 *      Author: Joonatan
 */


#include "Configuration.h"
#include "driverlib.h"

/***************************** Private definitions*************************************************/


#define CONFIG_START 0x0003F000
#define CONFIG_SIZE 32u /* Should be more than enough. */

typedef struct
{
    U8 task_frequency;
    U8 selected_color_scheme;
    U8 brightness;

    /* TODO : Add more items, at the very least custom color schemes etc. */
    U8 unused[CONFIG_SIZE - 3u];
} Configuration_T;

typedef union
{
    Configuration_T conf;
    U8 data[CONFIG_SIZE];
} Configuration_Data_T;

/***************************** Private function forward declarations ******************************/

Private Boolean verifyConfig(void);
Private void storeConfig(void);


/***************************** Private variables **************************************************/

Private Configuration_Data_T priv_conf;

/***************************** Public functions  **************************************************/
Public void configuration_start(void)
{
    /* First we load the configuration data from flash memory. */
    memcpy(priv_conf.data, (U8*)CONFIG_START, CONFIG_SIZE);

    /* Then lets perform sanity checks on it. */
    if (verifyConfig() == TRUE)
    {
        storeConfig();
    }
}

Public U32 configuration_getItem(Configuration_Item item)
{
    switch(item)
    {
        case CONFIG_ITEM_TASK_FREQ:
            return priv_conf.conf.task_frequency;
        case CONFIG_ITEM_COLOR_SCHEME:
            return priv_conf.conf.selected_color_scheme;
        case CONFIG_ITEM_BRIGHTNESS:
            return priv_conf.conf.brightness;
        default:
            return 0u;
    }
}

Public void configuration_setItem(U32 value, Configuration_Item item)
{
    Boolean isDirty = FALSE;

    switch(item)
    {
        case CONFIG_ITEM_TASK_FREQ:
            if (priv_conf.conf.task_frequency != value)
            {
                priv_conf.conf.task_frequency = value;
                isDirty = TRUE;
            }
            break;
        case CONFIG_ITEM_COLOR_SCHEME:
            if (priv_conf.conf.selected_color_scheme != value)
            {
                priv_conf.conf.selected_color_scheme = value;
                isDirty = TRUE;
            }
            break;
        case CONFIG_ITEM_BRIGHTNESS:
            if (priv_conf.conf.brightness != value)
            {
                priv_conf.conf.brightness = value;
                isDirty = TRUE;
            }
            break;
        default:
            break;
    }

    if (isDirty)
    {
        storeConfig();
    }
}


/***************************** Private functions  **************************************************/

Private Boolean verifyConfig(void)
{
    /* Returns true if we need to modify the config (because of incorrect values etc. ) */
    Boolean res = FALSE;

    /* TODO : Somehow we should limit these values with defines and connect them to the menu settings... */
    if (priv_conf.conf.task_frequency <= 1u || priv_conf.conf.task_frequency > 6u)
    {
        /* Revert to default. */
        priv_conf.conf.task_frequency = 3u;
        res = TRUE;
    }

    if (priv_conf.conf.selected_color_scheme > 3u)
    {
        priv_conf.conf.selected_color_scheme = 0u;
        res = TRUE;
    }

    if (priv_conf.conf.brightness > 100u)
    {
        priv_conf.conf.brightness = 60u;
        res = TRUE;
    }

    return res;
}


Private void storeConfig(void)
{
    /* Unprotecting Info Bank 0, Sector 0  */
    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR31);

    /* Trying to erase the sector. Within this function, the API will
        automatically try to erase the maximum number of tries. If it fails,
         trap in an infinite loop */
    if(!MAP_FlashCtl_eraseSector(CONFIG_START))
        while(1);

    /* Trying to program the memory. Within this function, the API will
        automatically try to program the maximum number of tries. If it fails,
        trap inside an infinite loop */
    if(!MAP_FlashCtl_programMemory(priv_conf.data,
            (void*) CONFIG_START, 32u))
                while(1);

    /* Setting the sector back to protected  */
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR31);
}

