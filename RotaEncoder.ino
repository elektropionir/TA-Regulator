// ** Rotary Encoder enabling manual power setting if no MQTT is used **

// this module is not yety implemented //

// sample libraries for samd https://github.com/mathertel/RotaryEncoder, https://github.com/Fattoresaimon/ArduinoDuPPaLib, https://github.com/mprograms/SimpleRotary, https://github.com/Seeed-Studio/Seeed_Arduino_RotaryEncoder, https://github.com/enjoyneering/RotaryEncoder 

// this is for the encoder board: https://learn.adafruit.com/adafruit-i2c-qt-rotary-encoder?view=all

// https://github.com/adafruit/Adafruit_Seesaw/blob/master/examples/encoder/encoder_basic/encoder_basic.ino

/*

#include <Encoder.h> // https://wiki.seeedstudio.com/Grove-Encoder/
#include <TimerOne.h>
 
unsigned int ROT[24];
int index_ROT;

void rotarySetup()
{
  encoder.Timer_init();
}
void rotaryLoop()
{
    if (encoder.rotate_flag ==1)
  {
    if (encoder.direct==1)
    {
      index_ROT++;
      if (index_ROT>23)
      index_ROT=24;
      setManualPower(index_ROT);
    }
     else
     {
      index_ROT--;
      if(index_ROT<0)
      index_ROT=0;
      setManualPower(index_ROT);
     }
    encoder.rotate_flag =0;
  }
}

void setManualPower(int index) // here should come the translation to setPower (see MQTT)
{

  
  for (int i=0;i<24;i++)
  {
    if (i<index)
    {
      ROT[i]=0xff;
    }
    else
    ROT[i]=0;
  }
//   set the power here

}

*/
