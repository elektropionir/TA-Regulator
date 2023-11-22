// ** The oled data screen **

#include "lcdgfx.h" // https://github.com/lexus2k/lcdgfx version 1.1.1 installed

// standard X/Y positions: 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128

DisplaySSD1327_128x128_I2C display(-1); // SSD1327 Oled 128 x 128

String s;
char str[16];

void displaySetup() {
    display.setFixedFont( ssd1306xled_font6x8 ); // font set https://github.com/Defragster/ssd1306xled/blob/master/font6x8.h
    display.begin();
    display.clear();
}

void displayClear() {
display.clear();
}

void displayLoop() {
  display.setColor(GRAY_COLOR4(200)); // 255 is max level
    if (state == RegulatorState::ALARM) {
    display.clear();
    
      switch (alarmCause) {
        case AlarmCause::MQTT:
        display.printFixed(40, 16, "!MQTT", STYLE_BOLD);             
          break;
        case AlarmCause::NETWORK:
        display.printFixed(16, 16, "!NETWORK", STYLE_BOLD);
          break;
          case AlarmCause::TRIAC:
        display.printFixed(32, 16, "!TRIAC", STYLE_BOLD);
          break;
        case AlarmCause::MODBUS:
        display.printFixed(24, 16, "!MODBUS", STYLE_BOLD);
          break;
        default:
          break;
      }
  return;
  } 
  else {     
     // switch off display at night to prevent burn-in
     if (hourNow >= 22 && hourNow <= 23 && !buttonPressed) {
     display.clear();
       return;
    }  
    if (hourNow >= 0 && hourNow < 7 && !buttonPressed) { 
     display.clear();
       return;
    }

// display nightCall status and power
    if (nightCall == true) {
    display.invertColors();
    display.printFixed(0, 6, "POWER:", STYLE_NORMAL); // nightCharge initiate
    display.invertColors();
    }
    if (nightCall == false) {
    display.printFixed(0, 6, "POWER:", STYLE_NORMAL); // nightCharge disabled
    }               
    sprintf(str,"%03d",elsensPower);
    display.setFixedFont( ssd1306xled_font8x16 );
    display.printFixed(44, 0, str, STYLE_BOLD); // ssd1327
    display.setFixedFont( ssd1306xled_font6x8 );
    display.printFixed(70, 8, "w", STYLE_NORMAL);

//  display state
    display.printFixed(0, 20, "STATE:", STYLE_NORMAL);
    if (state == RegulatorState::MONITORING) {
    display.printFixed(44, 20, "monitoring", STYLE_BOLD); 
    }
    if (state == RegulatorState::REGULATING) {
    display.printFixed(44, 20, "regulating", STYLE_NORMAL);
    }
    if (state == RegulatorState::MANUAL_RUN) {
    display.printFixed(44, 20, "manual    ", STYLE_NORMAL); 
    }    
    if (state == RegulatorState::ACCUMULATE) {
    display.printFixed(44, 20, "charging  ", STYLE_NORMAL); 
    }  

// display solar sensor average (W)
  display.printFixed(0, 32, "INsol:", STYLE_NORMAL);
  sprintf(str,"%04d",insolRefAvg); // insol AV
  display.printFixed(44, 32, str, STYLE_NORMAL);
  
// display inverter AC Power
  display.printFixed(0, 40, "PVpow:", STYLE_NORMAL);
  sprintf(str,"%04d",inverterAC);
  display.printFixed(44, 40, str, STYLE_NORMAL);

// display Meter Power 
  display.printFixed(0, 48, "Meter:", STYLE_NORMAL);
  sprintf(str,"%04d",abs(meterPower));
  display.printFixed(44, 48, str, STYLE_NORMAL);
  if (meterPower < 0) {
  display.printFixed(36, 48, "-", STYLE_NORMAL);
  }
  if (meterPower >= 0) {
  display.printFixed(36, 48, " ", STYLE_NORMAL);
  }

// display throttled status
  if (pilotThrottled == true) {
  display.printFixed(72, 48, "X", STYLE_NORMAL);
  }
  else {
  display.printFixed(72, 48, " ", STYLE_NORMAL);
  }

// display suspended status
if (pilotSuspended == true) {
  display.printFixed(92, 48, "T>!", STYLE_NORMAL);
  }
  else {
  display.printFixed(92, 48, "   ", STYLE_NORMAL);
  }

// display forecastIndex > sunny = 1, sunny/cloudy = 2, worsening = 3, cloudy = 4, rainy = 5
  display.printFixed(0, 60, "Sky", STYLE_NORMAL);
  sprintf(str,"%02d",Z);
  display.printFixed(18, 60, str, STYLE_NORMAL);
  display.printFixed(30, 60, ":", STYLE_NORMAL);

  if (forecastTrend == 0) {
    display.printFixed(44, 60, "-//- ", STYLE_NORMAL);
  }
  else {
  if (forecastIndex == 1) {
    display.printFixed(44, 60, "sunny     ", STYLE_NORMAL);
  }
  if (forecastIndex == 2) {
    display.printFixed(44, 60, "partly sun", STYLE_NORMAL); // mixed sun
  }
  if (forecastIndex == 3) {
    display.printFixed(44, 60, "worsening ", STYLE_NORMAL);
  }
  if (forecastIndex == 4) {
    display.printFixed(44, 60, "cloudy    ", STYLE_NORMAL); // overcast
  }
  if (forecastIndex == 5) {
    display.printFixed(44, 60, "rainy     ", STYLE_NORMAL);
  }
  }
  
// display trend and pressure, 1 = raising, 2 = falling, 3 = steady, 0 = no data
  if (forecastTrend == 1) {
    display.printFixed(108, 60, "++", STYLE_NORMAL);
  }
  if (forecastTrend == 2) {
    display.printFixed(108, 60, "--", STYLE_NORMAL);
  }  
  if (forecastTrend == 3) {
    display.printFixed(108, 60, "<>", STYLE_NORMAL);
  }

// display kWh accumulated per day
  display.printFixed(0, 72, "kWh/d:", STYLE_NORMAL);
  sprintf(str,"%.2f",((float) statsAccumulatedPowerToday()/1000.0)); // kWh on day
  display.printFixed(44, 72, str, STYLE_NORMAL);
  display.printFixed(72, 72, "ACC", STYLE_NORMAL); // charge
  if (nightCall == true) { 
  display.printFixed(92, 72, "/", STYLE_NORMAL);
  sprintf(str,"%.2f",((float) chargeSetLevel/1000.0)); // kWh charge level set
  display.printFixed(100, 72, str, STYLE_NORMAL);
  }
    
// display kWh regulated per day
  sprintf(str,"%.2f",((float) statsRegulatedPowerToday()/1000.0)); // kWh on day
  display.printFixed(44, 80, str, STYLE_NORMAL);
  display.printFixed(72, 80, "REG", STYLE_NORMAL); // regulating

// display kWh insolation per day
  sprintf(str,"%.2f",((float) insolStatsInsolationPowerToday()/1000.0)); // kWh on day
  display.printFixed(44, 88, str, STYLE_NORMAL);  
  if ((float) insolStatsInsolationPowerToday()/1000.0 < 10) {
   display.printFixed(72, 88, "SOL", STYLE_NORMAL);
  } else { // we move one segment on the display
  display.printFixed(80, 88, "SOL", STYLE_NORMAL);
  }

// display kWh regulated over year
  sprintf(str,"%.0f",((float) statsRegulatedPowerYear()/1000.0)); // kWh on day
  display.printFixed(44, 96, str, STYLE_NORMAL);
  display.printFixed(72, 96, "R/Y", STYLE_NORMAL); // regulating

  display.setColor(GRAY_COLOR4(20));

// display Mem
  sprintf(str,"%.2d", freeMemory()/1000);
  display.printFixed(0, 108, str, STYLE_NORMAL);
  display.printFixed(14, 108, "kB", STYLE_NORMAL);
  
// display IP
  display.printFixed(44, 108, "192.168.0.61", STYLE_NORMAL);

// display Vac smart meter
  sprintf(str,"%.3d",voltage); // 
  display.printFixed(0, 120, str, STYLE_NORMAL);
  display.printFixed(20, 120, "V", STYLE_NORMAL);

// display temp
  sprintf(str,"%.1f", temperature);
  display.printFixed(44, 120, str, STYLE_NORMAL);
  display.printFixed(70, 120, "'", STYLE_NORMAL);
  display.printFixed(74, 120, "C", STYLE_NORMAL);

// display current time 

  sprintf(str,"%02d",hourNow); // hour
  display.printFixed(96, 120, str, STYLE_NORMAL);
  display.printFixed(108, 120, ":", STYLE_NORMAL);
  sprintf(str,"%02d",minuteNow); // minute
  display.printFixed(114, 120, str, STYLE_NORMAL);

// debugging

//  display current sensor
//    display.printFixed(92, 0, "#", STYLE_NORMAL);
//    sprintf(str,"%04d",elsens);
//    display.printFixed(100, 0, str, STYLE_NORMAL);

//  display heatingPower setting
//    display.printFixed(92, 8, "P", STYLE_NORMAL);
//    sprintf(str,"%04d",heatingPower);
//    display.printFixed(100, 8, str, STYLE_NORMAL);

// display solar sensor (raw) 
  display.printFixed(96, 32, "#", STYLE_NORMAL);
  sprintf(str,"%04d",insol); // insol
  display.printFixed(104, 32, str, STYLE_NORMAL);

// display Throttle threshold
//  sprintf(str,"%.4d",abs(tresholdAvg));
//  display.printFixed(100, 48, str, STYLE_NORMAL);
//   if (tresholdAvg < 0) {
//  display.printFixed(92, 48, "-", STYLE_NORMAL);
//  }
//  if (tresholdAvg >= 0) {
//  display.printFixed(92, 48, " ", STYLE_NORMAL);
//  }
  
  }
}
