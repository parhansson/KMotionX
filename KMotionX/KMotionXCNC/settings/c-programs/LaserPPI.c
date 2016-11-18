#include "KMotionDef.h"

#define CLOCK 16.6666e6
#define ON 1
#define OFF 0
#define PULSE_LENGTH 2 // 2ms
#define LASER_PWM_BIT 26
#define PPI_IO_BIT 14
#define LASER_ARMED_BIT 46
#define SET_LASER_STATE(STATE) laserState = STATE;SetStateBit(PPI_IO_BIT, laserState);
#define STEPS_PER_MM 100 // 4000 / 40; //40mm per rev 4000 steps per rev

double LastPrintT;
int laserState = OFF;
int periods = 0, lastPeriod = 0, onPeriods = 0;


//http://www.buildlog.net/blog/2011/12/getting-more-power-and-cutting-accuracy-out-of-your-home-built-laser-system/
int printTime(){
  double T=Time_sec();
  if(T-LastPrintT > 1.0){

    //printf("Speed=%f steps/sec, %fmm/s pps=%f\n",speed, mmps,pps);
    LastPrintT = T;
    return 1;
  }
  return 0;
}

double pulsesPerStep;
int pulseLengthPeriods = (int) (PULSE_LENGTH / (1000*TIMEBASE)) / 2; // divide by two for some strange reason

void servicePPI() {
  double speed; //steps per second
  double vx, vy;
  double pps = 0; //pulses per second
  //double feedRate FS= F/60 mm i sekunden


  if (laserState == ON) {
        onPeriods++;
      }
      periods++;
      //Timebase should not be used. The actual time between each execution should be used
      //If many threads are running on KFlop result may differ.
      vx = (ch0->Dest - ch0->last_dest) * (1.0f / TIMEBASE);
      vy = (ch1->Dest - ch1->last_dest) * (1.0f / TIMEBASE);
      // compute xy speed in counts/sec
      //Do this instead, velocity is already computed
      //speed = sqrtf(ch0->last_vel*ch0->last_vel + ch1->last_vel*ch1->last_vel);
      speed = sqrtf(vx * vx + vy * vy);
      //speed is steps per second
      //speed=step/sec
      //step
      if (speed < 0.9) {
        //head is not moving turn laser off
        if(laserState == ON){
          //This almost never happens because laser is turned of by pulse length setting.
          //We just need this to be sure
          SET_LASER_STATE(OFF);
          //printf("Movement Stopped %d\n", laserState);
        }
        onPeriods = 0;
        lastPeriod = periods = 0;

      } else if (laserState == ON) {
        // head is moving an laser is on
        // turn off if on period is exceeded
        if (onPeriods > pulseLengthPeriods) {
          onPeriods = 0;
          SET_LASER_STATE(OFF);
          //printf("LaserState %d\n", laserState);
        }
      } else {
        // head is moving an laser is off
        // check if it is time to turn laser on
        pps = pulsesPerStep * speed;
        double t = (1.0f / TIMEBASE / pps);
        //if(printTime()){printf("Debug %f - %f\n", pps, t);}
        //if(printTime()){printf("Debug %d - %d > %f\n", periods, lastPeriod, t);}
        if (periods - lastPeriod > t) {

          SET_LASER_STATE(ON);
          onPeriods = 0;

          if (periods > 2000000000) periods = 0;
          lastPeriod = periods;
        }
      }
      /*
      if(printTime()){
         printf("Speed=%f steps/sec, %fmm/s pps=%f\n",speed, mmps,pps);
      }
      */
      //if(printTime()){printf("Channel 0 dest %lf\n", ch0->Dest);}

}
/*
int checkArmed(){
  if(ReadBit(LASER_ARMED_BIT)){
    return 1;
  } else {
    return 0;
  }
}
*/
main() {
  //M100 P500 Q50
  double ppi;
  ppi = *(float *) &persist.UserData[0]; //1500;//8000; //mm per min P Parameter
  double percent = *(float *) &persist.UserData[1]; //50%; //Q parameter
  
  double dutycycle = 255 * ((100 - percent) / 100);


  SetBitDirection(LASER_PWM_BIT, 1);    // Set bit 26 (PWM 0 as an output)
  FPGA(IO_PWMS_PRESCALE) = 2;   // set pwm frequency 21.7KHz KHz 32,5?
  FPGA(IO_PWMS+1) = 1;   // enable the PWM0
  FPGA(IO_PWMS+0) = dutycycle;   //12;//4.7%  // Set duty cycle to 25%

  
  ppi = ppi * 2; // double to just to make it work for some reason.

  SetBitDirection(PPI_IO_BIT, 1);
  ClearBit(PPI_IO_BIT);
  SetBitDirection(LASER_ARMED_BIT, 1);
  ClearBit(LASER_ARMED_BIT);



  //SetBitDirection(4, 1);
  /*
   https://groups.yahoo.com/neo/groups/DynoMotion/conversations/messages/8717
   If all you need is a square wave for the charge pump.
   Then you could use a Step/Dir Generator set to quadrature output mode.
   That would generate an accurate square wave anywhere from 0 to 500KHz.
   Command an axis to Jog at 4X the desired frequency.
   */
  /*
   TIME_BASE = 0,00009
   1/TIME_BASE times per second program is executed
   PMAX = 1/(2TIME_BASE) max pulser per sekund i KFLOP

   F=1500 mm i minuten
   PPI= 5000
   PPMM= PPI/25,4 pulser per mm
   FS= F/60 mm i sekunden
   P= PPMM * FS pulser i sekunden


   CF= 984 Pulser i sekunden vid 300 feedrate
   Hur manga timebase ar det
   (1/TIME_BASE/cf)
   */

  //printf("Frequency=%d, DutyCycle=%f, Pulse=%d clocks, Period=%f clocks\n",Frequency,DutyCycle,Pulse,Period);


  int pulseLengthPeriods = (int) ((PULSE_LENGTH) / (1000*TIMEBASE)) / 2; // divide by two for some strange reason
  printf("Pulse length=%dms, Periods=%d\n",PULSE_LENGTH, pulseLengthPeriods);

  //double T;
  LastPrintT = Time_sec();

  SET_LASER_STATE(OFF);

  if(ppi == 0){
      printf("Mode Constant\n");
      for (;;) {
        WaitNextTimeSlice(); // No need to update more often
        if(ReadBit(LASER_ARMED_BIT)){
          SET_LASER_STATE(ON);
        } else {
          SET_LASER_STATE(OFF);
        }  
      }
  } else {
      printf("Mode PPI\n");
      printf("LaserPPI.c M100 P = %f Q = %f\n", ppi, percent);
      printf("LaserPPI.c adjusted PPI = %f DutyCycle = %f\n", ppi, dutycycle);
      double ppmm; //pulses per mm
      ppmm = ppi / 25.4; //Pulses per mm
      pulsesPerStep = ppmm/STEPS_PER_MM;
      for (;;) {
    
        //Delay_sec(0.1);
        WaitNextTimeSlice();  // ensure we don't get interrupted
        if(ReadBit(LASER_ARMED_BIT)){
          servicePPI();
        } else {
          SET_LASER_STATE(OFF);
        }
    
      }
  }
}
