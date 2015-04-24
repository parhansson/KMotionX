#include "KMotionDef.h"

#define CLOCK 16.6666e6
#define ON 1
#define OFF 0
#define PULSE_LENGTH 2 // was and should be 2ish
#define PPI_IO_BIT 14

//http://www.buildlog.net/blog/2011/12/getting-more-power-and-cutting-accuracy-out-of-your-home-built-laser-system/
main() {
  //M100 P500 Q50
  double ppi = *(float *) &persist.UserData[0]; //1500;//8000; //mm per min P Parameter
  double percent = *(float *) &persist.UserData[1]; //50%; //Q parameter
  printf("LaserPPI.c M100 P = %f Q = %f\n", ppi, percent);
  
  double dutycycle = 255 * ((100 - percent) / 100);
  printf("LaserPPI.c adjusted PPI = %f DutyCycle = %f\n", ppi, dutycycle);


  SetBitDirection(26, 1);    // Set bit 26 (PWM 0 as an output)
  FPGA(IO_PWMS_PRESCALE) = 2;   // set pwm frequency 21.7KHz KHz
  FPGA(IO_PWMS+1) = 1;   // enable the PWM0
  FPGA(IO_PWMS+0) = dutycycle;   //12;//4.7%  // Set duty cycle to 25%

  
  ppi = ppi * 2; // double to just to make it work for some reason.

  SetBitDirection(PPI_IO_BIT, 1);
  ClearBit(PPI_IO_BIT);
  //SetBitDirection(4, 1);
  /*
   https://groups.yahoo.com/neo/groups/DynoMotion/conversations/messages/8717
   If all you need is a square wave for the charge pump.
   Then you could use a Step/Dir Generator set to quadrature output mode.
   That would generate an accurate square wave anywhere from 0 to 500KHz.
   Command an axis to Jog at 4X the desired frequency.
   */
  double speed; //steps per second
  double stepsPerMM = 4000 / 40; //40mm per rev 4000 steps per rev

  //printf("Frequency=%d, DutyCycle=%f, Pulse=%d clocks, Period=%f clocks\n",Frequency,DutyCycle,Pulse,Period);
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
  double pulseLength = PULSE_LENGTH; //ms
  int pulseLengthPeriods = (int) (pulseLength / 1000 / TIMEBASE) / 2; // divide by two for some strange reason
  //printf("Pulse length=%fms, Periods=%d\n",pulseLentgh, pulseLengthPeriods);
  //ppi = 500;
  double ppmm = ppi / 25.4; //pulser per mm
  double pps = 0; //pulses per second
  //P= PPMM * FS pulser i sekunden
  double vx, vy;
  double mmps; //mm per second
  //double feedRate FS= F/60 mm i sekunden
  double T;
  double LastPrintT = Time_sec();
  int laserState = OFF;
  SetStateBit(PPI_IO_BIT, laserState);
  int periods = 0, lastPeriod = 0, onPeriods = 0;
  for (;;) {

    //Delay_sec(0.1);
    WaitNextTimeSlice();  // ensure we don't get interrupted
    if (laserState == ON) {
      onPeriods++;
    }
    periods++;
    vx = (ch0->Dest - ch0->last_dest) * (1.0f / TIMEBASE);
    vy = (ch1->Dest - ch1->last_dest) * (1.0f / TIMEBASE);
    speed = sqrtf(vx * vx + vy * vy);
    //speed is steps per second
    //speed=step/sec
    //step
    if (speed < 0.9) {
      //head is not moving turn laser off
      if(laserState == ON){
        laserState = OFF;
        SetStateBit(PPI_IO_BIT, laserState);
        printf("Movement Stopped %d\n", laserState);
      }
      onPeriods = 0;
      lastPeriod = periods = 0;

    } else if (laserState == ON) {
      // head is moving an laser is on
      // turn off if on period is exceeded
      if (onPeriods > pulseLengthPeriods) {
        onPeriods = 0;
        laserState = OFF;
        SetStateBit(PPI_IO_BIT, laserState);
        //printf("LaserState %d\n", laserState);
      }
    } else {
      // head is moving an laser is off
      // check if it is time to turn laser on
      mmps = speed / stepsPerMM;
      pps = ppmm * mmps;

      double t = (1.0f / TIMEBASE / pps);
      if (periods - lastPeriod > t) {

        laserState = ON;
        onPeriods = 0;
        SetStateBit(PPI_IO_BIT, laserState);
        //printf("LaserState %d\n", laserState);
        if (periods > 2000000000) periods = 0;
        lastPeriod = periods;
      }
    }


     T=Time_sec();
     if(T-LastPrintT > 0.5){
       printf("Speed=%f steps/sec, %fmm/s pps=%f\n",speed, mmps,pps);
       LastPrintT = T;
     }

  }
}
