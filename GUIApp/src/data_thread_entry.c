#include "data_thread.h"


#define SYST_CLOCK_MS   (24)
uint32_t past_time = 0;
uint32_t current_time= 0;
uint32_t elapsed_time = 0;
uint32_t vel_rpm = 0;
uint32_t u32RPM_FilteredData=0;
#define RPM_FILTER_ORDER (uint32_t)10

#define C_FILTER_ORDER (uint16_t)10
uint16_t u16ADC_FilteredData;
uint16_t u16ADC_Data;
float SETPOINT;     //RPM adjust
uint32_t counts;  //
uint32_t intRPM=0;    //integer part of RPM
uint32_t counter;
uint32_t tempo2;
const double factor=3.1635e-7;
const float A=7.5;
float AF;
float RPM=0;  //motor current speed

uint32_t PWM=0;
float max=999950;   //max PWM value
float min=0;        //min PWM value
uint32_t uDutyCycle=25000;

float error_vel;
float a=500;
//float b=8.56;
//float c=154.61;
float b=0;
float c=0;
float iT,iT0;
float dT;
float error0=0;
float uT=0; //output  PID
uint32_t u8DutyCycle=25000;
int32_t delay_us;
ioport_level_t level = IOPORT_LEVEL_HIGH;
const uint32_t delay =19117;

float freq=0;
const bsp_delay_units_t bsp_delay_units =BSP_DELAY_UNITS_MICROSECONDS;
// este programa es para medir la velocidad en RPM , usando la interrupción externa
uint32_t data_message[4] = {0x00, 0x00, 0x00};

void Call_Back_IRQ0_RPM(external_irq_callback_args_t *p_args)
   {
       (void) p_args;

       current_time = DWT->CYCCNT;
       if(past_time > current_time)
       {
           elapsed_time = ~(past_time - current_time);
       }
       else
       {
           elapsed_time = current_time - past_time;
       }
       elapsed_time = elapsed_time / SYST_CLOCK_MS;
       past_time = current_time;

       vel_rpm = 75090900 / elapsed_time;
       u32RPM_FilteredData = (vel_rpm + (C_FILTER_ORDER - 1)*u32RPM_FilteredData) / RPM_FILTER_ORDER;
   }

/* Data Thread entry function */
void data_thread_entry(void)
{
    /* TODO: add your own code here */
     const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MICROSECONDS;
     g_external_irq0.p_api->open(g_external_irq0.p_ctrl, g_external_irq0.p_cfg);
     /* ********************************INPUT ADC to read SETPOINT *************************************/
        g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
        g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
        g_adc0.p_api->scanStart(g_adc0.p_ctrl);
    //***************************OUTPUT PWM Timer ******************************************************/
      g_timer9.p_api->open (g_timer9.p_ctrl, g_timer9.p_cfg);
      g_timer9.p_api->start (g_timer9.p_ctrl);
    //************************** RPM pin port **********************************************************/
      ioport_level_t p400_state; // tolerant pin to  5 volts pulse input
      AF=A/factor;
      ioport_level_t p05_state; //push button state
    //**************************************************************************************************/

      void toggle_pin()
         {
           if(level==IOPORT_LEVEL_HIGH)
               level=IOPORT_LEVEL_LOW;
           else
               level=IOPORT_LEVEL_HIGH;
           g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, level);
         }
        //**************************************************************************************************/
        //**************************************************************************************************/
        //Starting motor
        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05,&p05_state);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &u16ADC_Data);
        SETPOINT=(float)u16ADC_FilteredData*3300/4095;

        while(p05_state==1)              //wait for START push button pressed
        {
            g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05,&p05_state);
            g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &u16ADC_Data);
            SETPOINT=(float)u16ADC_FilteredData*3300/4095;
        }
        PWM=100000;
        g_timer9.p_api->dutyCycleSet(g_timer9.p_ctrl,PWM,TIMER_PWM_UNIT_PERCENT_X_1000,0);
        g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_13,IOPORT_LEVEL_HIGH); //enable=1
        delay_us=500000;
        R_BSP_SoftwareDelay(delay_us, bsp_delay_units);

    while (1)
    {
        toggle_pin();
         // read set point
          g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &u16ADC_Data);
          u16ADC_FilteredData = (u16ADC_Data + (C_FILTER_ORDER - 1)*u16ADC_FilteredData) / C_FILTER_ORDER;
          SETPOINT=(float)u16ADC_FilteredData*3300/4095;
          RPM=u32RPM_FilteredData;
          intRPM=RPM;
          error_vel=SETPOINT-RPM;  // error calculus
          //**************************************************
          iT=b*error_vel+iT0;        //integral term calculus
          dT=c*(error_vel-error0);   //derivative term calculus
          uT=a*error_vel;      //PID output calculus
       //   if(uT>max)                //adjust uT between max and min limits
       //     {uT=max;}
       //   if(uT<min)
       //     {uT=min;}
          iT0=iT;                  //update iT0
          error0=error_vel;        //update error0
          //***************************************************
          if(uT<0)
              uT=0;
          if(uT>90000)
              uT=90000;
          PWM=uT;
          g_timer9.p_api->dutyCycleSet(g_timer9.p_ctrl,PWM,TIMER_PWM_UNIT_PERCENT_X_1000,0);
          R_BSP_SoftwareDelay(delay, bsp_delay_units);

          data_message[0] = RPM;
          data_message[1] = SETPOINT;
          data_message[2] = u8DutyCycle;

       tx_queue_send(&g_val_queue0, data_message, TX_NO_WAIT);
       tx_thread_sleep(20);
    }
}

//******************************************************************************************
