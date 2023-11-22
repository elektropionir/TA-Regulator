// ** Compiles the InsolStats and saves to SD card (FAT 32) **
// take care: struct insolStats is defined in const.h

const char* INSOL_FILENAME = "INSOL.DAT"; // here we log the measured insolation energy (power * time)

// we catch longest daylight period june 04.00 - 19.00 (Belgrade) >> span = 04.00 - 19.00 hour, plus safety margin of 1 hour on both sides = 18 slots

struct { // we log insolationPower
  time_t timestamp = 0;
  insolStats hour;
  insolStats day;
  insolStats month;
  insolStats year;
  insolStats insolHour03;
  insolStats insolHour04;
  insolStats insolHour05;
  insolStats insolHour06;
  insolStats insolHour07;
  insolStats insolHour08;
  insolStats insolHour09;
  insolStats insolHour10;
  insolStats insolHour11;
  insolStats insolHour12;
  insolStats insolHour13;
  insolStats insolHour14;
  insolStats insolHour15;
  insolStats insolHour16;
  insolStats insolHour17;
  insolStats insolHour18;
  insolStats insolHour19;
  insolStats insolHour20;
  } insolData;

unsigned long insolStatsMilliwats = 0;
unsigned long insolStatsMilliwatMilis = 0;

unsigned long insolStatsSaveTimer = 0;

  void insolStatsSetup() {
  
  File file = FS.open(INSOL_FILENAME, FILE_READ);
  if (file) {
    file.readBytes((byte*) &insolData, sizeof(insolData));
    file.close();
  }
}

void insolStatsLoop() {

  const unsigned long INSOLSTATS_MILLIWATS_INTERVAL = 5 * 60000; // 5 minutes
  const unsigned long INSOLSTATS_SAVE_INTERVAL = 30 * 60000; // 30 minutes
  static unsigned long lastInsolPowerChangeMillis = 0;
  static int lastInsolPower = 0;
  

  if (lastInsolPowerChangeMillis == 0) { // not counting
    lastInsolPowerChangeMillis = loopStartMillis; // start counting
    insolStatsSaveTimer = loopStartMillis;
  }
 
if(hourLogged != hourNow){ // at the arrival of a new hour, we parse the data of the passed hour in struct and reset insolData.hour.insolationPower
     if (hourLogged == 3) { // 
    insolData.insolHour03.insolationPower = insolData.hour.insolationPower;
     }
    if (hourLogged == 4) { // 
    insolData.insolHour04.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 5) { // 
    insolData.insolHour05.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 6) { // 
    insolData.insolHour06.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 7) { // 
    insolData.insolHour07.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 8) { // 
    insolData.insolHour08.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 9) { // 
    insolData.insolHour09.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 10) { // 
    insolData.insolHour10.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 11) { // 
    insolData.insolHour11.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 12) { // 
    insolData.insolHour12.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 13) { // 
    insolData.insolHour13.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 14) { // 
    insolData.insolHour14.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 15) { // 
    insolData.insolHour15.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 16) { // 
    insolData.insolHour16.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 17) { // 
    insolData.insolHour17.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 18) { // 
    insolData.insolHour18.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 19) { // 
    insolData.insolHour19.insolationPower = insolData.hour.insolationPower;
     }
     if (hourLogged == 20) { // 
    insolData.insolHour20.insolationPower = insolData.hour.insolationPower;
     }    
      hourLogged = hourNow;
      insolData.hour.insolationPower = 0;
    }

  time_t tInsol = insolData.timestamp; 
  if (day(now()) != day(tInsol)) {

if (month(now()) != month(insolData.timestamp)) {
      insolData.month.insolationPower = 0;
    }
    if (year(now()) != year(insolData.timestamp)) { // this section is new
      memset(&insolData, 0, sizeof(insolData));   
    }
    else {
      insolData.day.insolationPower = 0;
    }
    
  insolData.timestamp = now(); // reset
}

  unsigned long insolInterval = loopStartMillis - lastInsolPowerChangeMillis;
  int insolPower = insolStatsEvalCurrentInsolPower();
 
  if (insolPower != lastInsolPower || insolInterval > INSOLSTATS_MILLIWATS_INTERVAL) { // > 5 min.
    if (lastInsolPower > 0) {
      insolStatsMilliwats += (float) lastInsolPower/3.6 * (float) insolInterval/1000; // W to mWh (power * time * 1000, hence 3.6 factor)
      insolStatsMilliwatMilis += insolInterval;
      if (insolPower == 0 || insolStatsMilliwatMilis > INSOLSTATS_MILLIWATS_INTERVAL) {
        insolStatsAddMilliwats();  
      }
    }
    lastInsolPower = insolPower;
    lastInsolPowerChangeMillis = loopStartMillis;
  }

if (loopStartMillis - insolStatsSaveTimer > INSOLSTATS_SAVE_INTERVAL) {
    insolStatsSave();
  }

}
int insolStatsEvalCurrentInsolPower() { 
      return insolCalibPower; // calibrated insolation power
  }

void insolStatsAddMilliwats() {
  int insolationPower = round((float) insolStatsMilliwats / 1000); // watt
  insolStatsMilliwatMilis = 0;
  insolStatsMilliwats = 0;
  insolData.hour.insolationPower += insolationPower;
  insolData.day.insolationPower += insolationPower;
  insolData.month.insolationPower += insolationPower;
  insolData.year.insolationPower += insolationPower;

}

void insolStatsSave() {
 if (!insolStatsMilliwatMilis)
    return;
  insolStatsAddMilliwats();
 
  File file = FS.open(INSOL_FILENAME, FILE_NEW);
  if (file) {
    file.write((byte*) &insolData, sizeof(insolData));
    file.close();
  }

  insolStatsSaveTimer = loopStartMillis;
}

int insolStatsInsolationPowerHour() { // this is used by display and webserver
  if (day(now()) != day(insolData.timestamp))
    return 0;
  return insolData.hour.insolationPower; // Wh
}

int insolStatsInsolationPowerToday() { // this is used by display and webserver
  if (day(now()) != day(insolData.timestamp))
    return 0;
  return insolData.day.insolationPower; // Wh
}

int insolStatsInsolationPowerYear() { // this is used by display and webserver
  if (day(now()) != day(insolData.timestamp))
    return 0;
  return insolData.year.insolationPower; // Wh
}

