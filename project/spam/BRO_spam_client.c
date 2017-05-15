#include <stdio.h>
#include "common/headers/simul_config.h"
#include "common/headers/bt_comm.h"
#include "headers/BRO_spam_client.h"

#define DELAY 10
#define MOTOR_TASK_PERIOD 4
#define SAMP_RATE 0.004
/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(SysTimerCnt);
DeclareResource(Lcd);
DeclareResource(SimConfigGlobal);
DeclareTask(DisplayTask);
DeclareTask(SimulTask);
DeclareTask(ConfigTask);
DeclareEvent(SimTermEvt);
DeclareAlarm(SimulTaskAlarm);
DeclareAlarm(MotorTaskAlarm);

const uint32_t MOTOR_PORT_1 = NXT_PORT_B;
const uint32_t MOTOR_PORT_2 = NXT_PORT_C;

sim_status_t sim_status;
sim_config_t sim_config;

uint32_t init_time;

/** buffer to store the sampled data */
data_record_t data_msg[MAX_DATA_ITEMS];
uint32_t data_idx;


/* global vars*/



/*--------------------------------------------------------------------------*/
/* LEJOS OSEK hooks                                                         */
/*--------------------------------------------------------------------------*/
void ecrobot_device_initialize()
{

}


void ecrobot_device_terminate()
{
  stop_motor();
  ecrobot_term_bt_connection();
}

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(void)
{
  StatusType ercd;

  /*
   *  Increment OSEK Alarm System Timer Count
   */
  ercd = SignalCounter( SysTimerCnt );
  if ( ercd != E_OK ) {
    ShutdownOS( ercd );
  }
}


/** Task started automatically when the application starts. This task initializes the Bluetooth connection,
 *  receives the configuration messages to set all the parameters for the simulation, and starts the simulation
 *  when a SIM_START message is received from the PC.
 */
TASK(ConfigTask)
{
  uint8_t in_packet[MAX_CONF_SIZE];

  sim_status = STATUS_CONFIG;

  while (1)
    {
      while (ecrobot_get_bt_status() != BT_STREAM) // waits for a master device to establish a connection
	{
	  ecrobot_init_bt_slave("1234");
	  systick_wait_ms(50);
	}
      
      recv_packet(in_packet, MAX_CONF_SIZE);

      GetResource(SimConfigGlobal);
      sim_status = decode_config_msg(in_packet, &sim_config);
      ReleaseResource(SimConfigGlobal);

      if (sim_status == STATUS_SIMUL)
	{
	  start_sim(&sim_config);
	  wait_sim_termination();
	}
    }
}

/*CHANGE THIS
/** Task setting the motor speed at each step according to the input function specific for the current simulation,
 *  and the elapsed time
 	*/		/*


TASK(MotorTask)
{
  
  //int speed_val = get_speed(&sim_config, elapsed_time);
  //pow0 = (1.7857*power1-0.7875*power2+(1*error0-1.9411*error1+0.9419*error2)*K)
  /*
  uint32_t elapsed_time = systick_get_ms()-init_time;
  //float a=(float)get_speed(&sim_config, elapsed_time);
  static float p[3]={50,0,0};
  int target;
  static float e[3];
  p[0]=coeffp[1]*p[1]+coeffp[2]*p[2]+(e[0]+coeffe[1]*e[1]+coeffe[2]*e[2])*K;
  target=(int)roundf(p[0]);
  display_goto_xy(0,0);
  display_string("p[0]: ");
  display_int(target, 6);
  
  //nxt_motor_set_speed(MOTOR_PORT, 53, 0);
  TerminateTask();

}


/** Task responsible to sample the data regarding the motor during the simulation, and to send it to the PC.
 *  When the simulation is terminated, this task resets the status of the application and unlocks the Configuration
 *  Task. 
 */
TASK(SimulTask)
{
  static boolean initialized = false;
  //notice the use of array indexes: 0 for current 1 for t-1 2 for t-2
  uint32_t elapsed_time = systick_get_ms()-init_time;
  static uint32_t prev_time=0;
  static int rev_count_1[2]={0,0};
  static int rev_count_2[2]={0,0};

  /*static const float coeffp[3]={1,1,0};
  static const float coeffe[3]={1,-0.8564,0};
  static const float K=0.24839;*/

  /*static const float coeffp[3]={1,1,0};
  static const float coeffe[3]={1,-0.8712,0};
  static const float K=0.41462;*/

  static const float coeffp[3]={1,1,0};
  static const float coeffe[3]={1,-0.8657,0};
  static const float K=0.2333;
  
 /* static const float coeffp[3]={1,1.6051,-0.6051};
  static const float coeffe[3]={1,-1.876,0.8798};
  static const float K=14.028; */

  /*
   static const float coeffp[3]={1,1.7319,0.7487};
 static const float coeffe[3]={1,0.0165,0.9835};
 static const float K=15.36;*/
  
  static const float alpha=0.1; //forgetting factor 0.9 (high = better filtering but higher overshoot)
  static float avg_speed_1=0;
  static float avg_speed_2=0;
  static float p_1[3]={0,0,0};
  static float p_2[3]={0,0,0};
  static float e_1[3]={0,0,0};
  static float e_2[3]={0,0,0};
  static float current_speed_1=0;
  static float current_speed_2=0;
  static float target_speed_1=0;
  static float target_speed_2=0;
  static int power_1=0;
  static int power_2=0;
  
  
  if(!initialized){
	  //input: deg/sec (-900 to +900)
      //
      //target_speed=get_speed(&sim_config,elapsed_time)/1000; 
      target_speed_1 = 0.6; 
      target_speed_2 = 0.6; 
  	  //speed in deg/msec (range goes from ca -0.9 to 0.9)
	  initialized==true;
	}
  
  if (elapsed_time >= get_duration(&sim_config))
    {
      terminate_sim();
      TerminateTask();
    }

  else
    {

	  //calculating current speed
      rev_count_1[0]=nxt_motor_get_count(MOTOR_PORT_1);
      rev_count_2[0]=nxt_motor_get_count(MOTOR_PORT_2);
	  current_speed_1=((float)(rev_count_1[0]-rev_count_1[1]))/(elapsed_time - prev_time);
	  current_speed_2=((float)(rev_count_2[0]-rev_count_2[1]))/(elapsed_time - prev_time);
      rev_count_1[1]=rev_count_1[0];
      rev_count_2[1]=rev_count_2[0];
	  
	  //moving average (no butterworth, introduces delay)
      avg_speed_1=alpha*current_speed_1+(1-alpha)*avg_speed_1;
      avg_speed_2=alpha*current_speed_2+(1-alpha)*avg_speed_2;
      //current_speed=avg_speed;

	  //calculating current error
      e_1[0]=target_speed_1-avg_speed_1;
      e_2[0]=target_speed_2-avg_speed_2;
      
	  //calculating current power
      p_1[0]=coeffp[1]*p_1[1]+coeffp[2]*p_1[2]+(e_1[0]+coeffe[1]*e_1[1]+coeffe[2]*e_1[2])*K;
      p_2[0]=coeffp[1]*p_2[1]+coeffp[2]*p_2[2]+(e_2[0]+coeffe[1]*e_2[1]+coeffe[2]*e_2[2])*K;
      
      if (p_1[0]>1) {p_1[0]=1;} //saturation
      if (p_2[0]>1) {p_2[0]=1;} //saturation
      
	  if (p_1[0]<-1){p_1[0]=-1;} 
	  if (p_2[0]<-1){p_2[0]=-1;} 
	  power_1=(int)(p_1[0]*100);
	  power_2=(int)(p_2[0]*100);
	  //set current power to motor
      nxt_motor_set_speed(MOTOR_PORT_1, power_1	, 0);
      nxt_motor_set_speed(MOTOR_PORT_2, power_2	, 0);
      
	  //debugging

      //speed_val = get_speed(&sim_config, elapsed_time);
	  
      //shifting to old samples
    e_1[2]=e_1[1];
    e_2[2]=e_2[1];
	  e_1[1]=e_1[0];
	  e_2[1]=e_2[0];

    p_1[2]=p_1[1];
    p_2[2]=p_2[1];
	  p_1[1]=p_1[0];
	  p_2[1]=p_2[0];

	prev_time = elapsed_time;

	  
	  //send data to pc
      //send_buffered_data((int)(elapsed_time), (int)(avg_speed_2*1000));
		send_buffered_data((int)(elapsed_time), (int)(rev_count_1[0]));
      TerminateTask();
    }
}

/** Task responsible to show on the display some informations regarding the current status of the application. */
TASK(DisplayTask)
{
  GetResource(SimConfigGlobal);
  display_clear(0);
  switch (sim_status)
    {
    case STATUS_CONFIG:
      ecrobot_status_monitor("Configuration Mode");   
      break;
    case STATUS_SIMUL:
      display_goto_xy(0,0);
      display_string("Type: ");
      display_string(sim_config.sim_type==SIM_STEP?"  Step":"   Sin");
      display_goto_xy(0,1);
      display_string("Deg/msec:  ");
      display_int((int)sim_config.data[AMP_IDX], 6);
      display_goto_xy(0,2);
      display_string("Off:  ");
      display_int((int)sim_config.data[OFF_IDX], 6);
      display_goto_xy(0,3);
      display_string("Dur:  ");
      display_int(get_duration(&sim_config), 6);
      display_goto_xy(0,4);
      display_string("Samp: ");
      display_int(get_sampling_time(&sim_config), 6);      
      break;
    }
  ReleaseResource(SimConfigGlobal);
  display_update();

  TerminateTask();
}


void stop_motor()
{
  nxt_motor_set_speed(MOTOR_PORT_1, 0, BRAKE);
  nxt_motor_set_speed(MOTOR_PORT_2, 0, BRAKE);
}

void start_sim(sim_config_t *config)
{
  init_time = systick_get_ms();
  nxt_motor_set_count(MOTOR_PORT_1, 0);
  nxt_motor_set_count(MOTOR_PORT_2, 0);
  clear_buffer();
  //SetRelAlarm(MotorTaskAlarm, DELAY, MOTOR_TASK_PERIOD);
  SetRelAlarm(SimulTaskAlarm, DELAY, get_sampling_time(config));
}

void wait_sim_termination()
{
  WaitEvent(SimTermEvt); 
  ClearEvent(SimTermEvt);
}

void terminate_sim()
{
  flush_buffer();
  send_sim_end();
  sim_status = STATUS_CONFIG;
  ecrobot_term_bt_connection();
  CancelAlarm(SimulTaskAlarm);
  //CancelAlarm(MotorTaskAlarm);
  stop_motor();
  SetEvent(ConfigTask, SimTermEvt);
}

void send_buffered_data(uint32_t elapsed_time, int rev_count)
{
  data_msg[data_idx].time = elapsed_time;
  data_msg[data_idx++].value = rev_count;
  if (data_idx == MAX_DATA_ITEMS)
    {
      flush_buffer();
    }
}

void flush_buffer()
{
  uint8_t data_packet[MAX_DATA_SIZE];

  if (data_idx > 0)
    {
      memset(data_packet, 0, MAX_DATA_SIZE);
      encode_sim_data_msg(data_packet, data_msg);
      send_packet(data_packet, MAX_DATA_SIZE);
      clear_buffer();
    }
}

void clear_buffer()
{
  data_idx = 0;
  memset(data_msg, 0, sizeof(data_record_t)*MAX_DATA_ITEMS);
}

void send_packet(uint8_t data[], int size)
{
  uint32_t sent_bytes = 0;
  while (sent_bytes < size)
    {
      sent_bytes += ecrobot_send_bt(data, sent_bytes, size-sent_bytes);
    }
}

void recv_packet(uint8_t data[], int size)
{
  int recv_bytes = 0;

  do
    {
      recv_bytes += ecrobot_read_bt(data+recv_bytes, 0, size-recv_bytes);
    }
  while (recv_bytes < size);
}

void send_sim_end()
{
  data_msg[data_idx++] = END_SIMULATION;
  flush_buffer();
}
