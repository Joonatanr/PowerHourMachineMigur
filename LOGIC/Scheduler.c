/*
 * Scheduler.c
 *
 *  Created on: 16. m�rts 2018
 *      Author: JRE
 */

#include "Scheduler.h"
#include "buzzer.h"
#include "buttons.h"
#include <driverlib.h>
#include "LcdWriter.h"
#include "display.h"
#include "BitmapHandler.h"
#include "MessageBox.h"
#include "PowerHourMain.h"

/* NB! Current implementation assumes that only 1 task is active at any time, but this can be changed ofcourse. */
/* NB! All lo prio interrupt tasks should come here. I think there is no point to create a separate scheduler for the
 *     hi prio interrupt. */




/*************  Private function prototypes.  **************/


Private void dedication_start(void);
Private void dedication_cyclic50ms(void);
Private void dedicationExitListener(void);


/* NB! Currently period has to be divisible by 50. Might want to change this. */

/* Ok : Idea is this that this array contains the tasks, of which only one can be active at a time. */
Private const Scheduler_LogicTask priv_application_modules[NUMBER_OF_APPLICATIONS] =
{
     { .period = 20u, .init_fptr = powerHour_init, .  start_fptr = powerHour_start,   .cyclic_fptr = powerHour_cyclic1000msec,    .stop_fptr = powerHour_stop },
     //{ .period = 50u,   .init_fptr = snake_init,        .start_fptr = snake_start,        .cyclic_fptr = snake_cyclic50ms,            .stop_fptr = snake_stop        },
     { .period = 1u,  .init_fptr = NULL,              .start_fptr = dedication_start,   .cyclic_fptr = dedication_cyclic50ms,      .stop_fptr = NULL          },
};



/* This array contains the tasks that run all the time. */
/* Small incremental changes :) - So lets enable the modules part first and then look at this part. */
Private const Scheduler_LogicTask priv_tasks[NUMBER_OF_SCHEDULER_TASKS] =
{
     { .period = 2u,  .init_fptr = buzzer_init,       .start_fptr = NULL,               .cyclic_fptr = buzzer_cyclic100msec,     .stop_fptr = NULL  }, /* Buzzer task.       */
     { .period = 2u,  .init_fptr = buttons_init,      .start_fptr = NULL,               .cyclic_fptr = buttons_cyclic100msec,    .stop_fptr = NULL  }, /* Buttons task       */
     { .period = 2u,  .init_fptr = BitmapHandler_init,.start_fptr = BitmapHandler_start,.cyclic_fptr = BitmapLoaderCyclic100ms,  .stop_fptr = NULL  }, /* Bitmap loader task */
     { .period = 2u,  .init_fptr = MessageBox_init,   .start_fptr = NULL,               .cyclic_fptr = MessageBox_cyclic100msec, .stop_fptr = NULL  }, /* Messagebox task   */
};


/*************  Private variable declarations.  **************/
Private const Scheduler_LogicTask * priv_curr_app_ptr = NULL;
Private U32 priv_app_task_timer = 0u;
Private U32 priv_task_timer = 0u;
Private Boolean priv_isInitComplete = FALSE;
Private Boolean priv_isAppPaused = FALSE;





/* Should be called once at startup. */
void Scheduler_initTasks(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_application_modules); ix++)
    {
        if (priv_application_modules[ix].init_fptr != NULL)
        {
            priv_application_modules[ix].init_fptr();
        }
    }

    for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_tasks); ix++)
    {
        if (priv_tasks[ix].init_fptr != NULL)
        {
            priv_tasks[ix].init_fptr();
        }
    }

    priv_isInitComplete = TRUE;
}

/* Should be called once at startup.. */
void Scheduler_StartTasks(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_SCHEDULER_TASKS; ix++)
    {
        if (priv_tasks[ix].start_fptr != NULL)
        {
            priv_tasks[ix].start_fptr();
        }
    }
}



void Scheduler_SetActiveApplication(Scheduler_LogicModuleEnum task)
{
    Interrupt_disableMaster();
    if (priv_curr_app_ptr != NULL)
    {
        priv_curr_app_ptr->stop_fptr();
    }
    priv_curr_app_ptr = &priv_application_modules[task];
    priv_isAppPaused = FALSE;
    //MessageBox_SetResponseHandler(NULL); //We make sure that the previous handler does not remain and cause any problems...
    if (priv_curr_app_ptr->init_fptr != NULL)
    {
        priv_curr_app_ptr->init_fptr();
    }
    Interrupt_enableMaster();

    if (priv_curr_app_ptr->start_fptr != NULL)
    {
        priv_curr_app_ptr->start_fptr();
    }
}



void Scheduler_StopActiveApplication(void)
{
    if (priv_curr_app_ptr != NULL)
    {
        if (priv_curr_app_ptr->stop_fptr != NULL)
        {
            priv_curr_app_ptr->stop_fptr();
        }
    }
    priv_curr_app_ptr = NULL;
}


void Scheduler_SetActiveApplicationPause(Boolean pause)
{
    if (priv_curr_app_ptr != NULL)
    {
        priv_isAppPaused = pause;
    }
}


/* Lets assume this gets called every 50 ms, by the main.c lo prio interrupt. */
void Scheduler_cyclic(void)
{
    U8 ix;
    U32 task_timer = priv_task_timer;
    U32 app_task_timer = priv_app_task_timer;

    if (priv_isInitComplete == FALSE)
    {
        return;
    }

    /* Deal with the current active logical module. */
    if (priv_curr_app_ptr != NULL)
    {
        if ((app_task_timer % priv_curr_app_ptr->period) == 0u)
        {
            if (priv_curr_app_ptr->cyclic_fptr != NULL)
            {
                /* Pause can happen for example because we are waiting for user input... */
                if (!priv_isAppPaused)
                {
                    priv_curr_app_ptr->cyclic_fptr();
                }
            }
        }
    }

    /* Deal with other constantly firing modules. */
    for (ix = 0u; ix < NUMBER_OF_ITEMS(priv_tasks); ix++)
    {
        /* No point to check this, if there is no corresponding function... */
        if (priv_tasks[ix].cyclic_fptr != NULL)
        {
            if ((task_timer % priv_tasks[ix].period) == 0u)
            {
                priv_tasks[ix].cyclic_fptr();
            }
        }
    }
}

Public void Scheduler_50msec_timer(void)
{
    priv_app_task_timer++;
    priv_task_timer++;
}

/**************************  Private functions ***********************/


Private Boolean priv_is_dedication_screen_exit = FALSE;

/** Idea : Maybe make a text display application for other such instances in the future. */

/* Dedication screen functions*/
Private void dedication_start(void)
{
    U8 yCoord = 0u;
    const U8 line_height = 16u;

    priv_is_dedication_screen_exit = FALSE;

    display_fill(COLOR_RED);

    display_drawString("This Power Hour Machine",           0u, yCoord,  FONT_ARIAL_12, FALSE);
    yCoord += line_height;
    display_drawString("was built for Diana, the SOC",      0u, yCoord,  FONT_ARIAL_12, FALSE);
    yCoord += line_height;
    display_drawString("responsible for GA2024 and",      0u, yCoord,  FONT_ARIAL_12, FALSE);
    yCoord += line_height;
    display_drawString("Chairwoman of KT, and also", 0u, yCoord,  FONT_ARIAL_12, FALSE);
    yCoord += line_height;
    display_drawString("for her clone duplicate Mirtel.",          0u, yCoord,  FONT_ARIAL_12, FALSE);
    yCoord += line_height;
    display_drawString(" ",          0u, yCoord,  FONT_ARIAL_12, FALSE);

    yCoord += line_height;
    display_drawString("May you have an EPIC GA!",          0u, yCoord,  FONT_ARIAL_12, FALSE);
    yCoord += line_height;
    display_drawString("-Urmet,Jorx,Kristel,Joonatan",   0u, yCoord,  FONT_ARIAL_12, FALSE);



    /* Basically we wait for ANY key to be pressed. */
    buttons_subscribeListener(UP_BUTTON,    dedicationExitListener);
    buttons_subscribeListener(DOWN_BUTTON,  dedicationExitListener);
    buttons_subscribeListener(RIGHT_BUTTON, dedicationExitListener);
    buttons_subscribeListener(LEFT_BUTTON,  dedicationExitListener);
}


Private void dedicationExitListener(void)
{
    priv_is_dedication_screen_exit = TRUE;
}


Private void dedication_cyclic50ms(void)
{
    if (priv_is_dedication_screen_exit)
    {
        priv_is_dedication_screen_exit = FALSE;
        returnToMain();
    }
}
