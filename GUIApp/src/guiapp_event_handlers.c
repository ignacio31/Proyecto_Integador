

#include "gui/guiapp_resources.h"
#include "gui/guiapp_specifications.h"
#include "data_thread.h"
#include "main_thread.h"

#define TIMERPERIOD    (101)
#define TIMERID        (30)

uint32_t message[4] = {0x00, 0x00, 0x00};

static void update_data(GX_WIDGET *p_widget);

UINT window1_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT result = gx_window_event_process(widget, event_ptr);
    tx_queue_receive(&g_val_queue0, message, 20);

    switch (event_ptr->gx_event_type)
    {
        case GX_EVENT_SHOW:
                gx_system_timer_start(widget, TIMERID, TIMERPERIOD,
                                      TIMERPERIOD);
        break;
        case GX_EVENT_TIMER:
               if (TIMERID == event_ptr->gx_event_payload.gx_event_timer_id)
               {
                   update_data(widget->gx_widget_parent);
               }
           break;
    default:
        gx_window_event_process(widget, event_ptr);
        break;
    }

    return result;
}


static void update_data(GX_WIDGET *p_widget)
{
    static GX_CHAR dutyArray[5];
    static GX_CHAR speedArray[5];

    GX_PROMPT *p_prompt = NULL;
    ssp_err_t err = 0;

    err = gx_widget_find(p_widget, ID_SPEED, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_prompt);
    if (TX_SUCCESS == err)
    {
        gx_utility_ltoa((LONG) message[0], speedArray, 4);

        err = gx_prompt_text_set(p_prompt, speedArray);
        if (err)
            while (1);
    }
    err = gx_widget_find(p_widget, ID_DUTY_CICLE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_prompt);
        if (TX_SUCCESS == err)
        {
            gx_utility_ltoa((LONG) message[2], dutyArray, 4);

            err = gx_prompt_text_set(p_prompt, dutyArray);
            if (err)
                while (1);
        }
}

