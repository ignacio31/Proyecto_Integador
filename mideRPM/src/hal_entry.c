/* HAL-only entry function */
#include "hal_data.h"

#define SYST_CLOCK_MS   (24)
uint32_t ui32actual_time = 0;
uint32_t ui32past_time = 0;
uint32_t ui32elapsed_time = 0;
uint32_t ui32vel_rpm = 0;
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
float a=2.373;
//float b=8.56;
//float c=154.61;
float b=0.279;
float c=5.043;
float iT,iT0;
float dT;
float error0=0;
float uT=0; //output  PID
uint32_t u8DutyCycle=25000;
int32_t delay_us;
ioport_level_t level = IOPORT_LEVEL_HIGH;
const uint32_t delay =19117;
float ganancia;

float freq=0;
const bsp_delay_units_t bsp_delay_units =BSP_DELAY_UNITS_MICROSECONDS;
// este programa es para medir la velocidad en RPM , usando la interrupción externa
void Call_Back_IRQ0_RPM(external_irq_callback_args_t *p_args)
   {
       (void) p_args;

       ui32actual_time = DWT->CYCCNT;
       if(ui32past_time > ui32actual_time)
       {
           ui32elapsed_time = ~(ui32past_time - ui32actual_time);
       }
       else
       {
           ui32elapsed_time = ui32actual_time - ui32past_time;
       }
       ui32elapsed_time = ui32elapsed_time / SYST_CLOCK_MS;
       ui32past_time = ui32actual_time;

       ui32vel_rpm = 75090900 / ui32elapsed_time;
       u32RPM_FilteredData = (ui32vel_rpm + (C_FILTER_ORDER - 1)*u32RPM_FilteredData) / RPM_FILTER_ORDER;

   }
//******************************************************************************************


void hal_entry(void)
{
    /* TODO: add your own code here */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MICROSECONDS;
    g_external_irq0.p_api->open(g_external_irq0.p_ctrl, g_external_irq0.p_cfg);
    /* ********************************INPUT ADC to read SETPOINT *************************************/
       g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
       g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
       g_adc0.p_api->scanStart(g_adc0.p_ctrl);
   //***************************OUTPUT PWM Timer ******************************************************/
     g_timer0.p_api->open (g_timer0.p_ctrl, g_timer0.p_cfg);
     g_timer0.p_api->start (g_timer0.p_ctrl);
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
   {g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05,&p05_state);
   g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &u16ADC_Data);
   SETPOINT=(float)u16ADC_FilteredData*3300/4095;
   }
   PWM=100000;
   g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl,PWM,TIMER_PWM_UNIT_PERCENT_X_1000,0);
   g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_13,IOPORT_LEVEL_HIGH); //enable=1
   delay_us=500000;
   R_BSP_SoftwareDelay(delay_us, bsp_delay_units);
   //**************************************************************************************************/
   while(1)
   {
   toggle_pin();
  // read set point
   g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &u16ADC_Data);
   u16ADC_FilteredData = (u16ADC_Data + (C_FILTER_ORDER - 1)*u16ADC_FilteredData) / C_FILTER_ORDER;

   SETPOINT=(float)u16ADC_FilteredData*950/4095;
   RPM=u32RPM_FilteredData;
   intRPM=RPM;
   error_vel=SETPOINT-RPM;  // error calculus
   //**************************************************
   iT=b*error_vel+iT0;        //integral term calculus
   dT=c*(error_vel-error0);   //derivative term calculus
   uT=a*error_vel+iT+dT;      //PID output calculus
   iT0=iT;                  //update iT0
   error0=error_vel;        //update error0
   //***************************************************
   if(uT<0)
       uT=0;
   if(uT>90000)
       uT=90000;
   PWM=uT;
   g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl,PWM,TIMER_PWM_UNIT_PERCENT_X_1000,0);
   R_BSP_SoftwareDelay(delay, bsp_delay_units);

   }//end of while


}// end of hal entry

