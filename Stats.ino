// ** Compiles the Stats and saves to SD card (FAT 32) **

const char* STATS_FILENAME = "STATS.DAT"; // here we log the energy (power * time) diverted to accumulator, as well as the Insolation statistics from the reference PV panel

  struct { // we log consumedPower and potentialPower
  time_t timestamp = 0; 
  Stats day; // regulated energy
  Stats month;
  Stats year;
  Stats dayAccumulate; // night accumulation energy
  Stats monthAccumulate;
  Stats yearAccumulate;
  Stats dayManualRun; // manual energy stored
  Stats monthManualRun; 
  Stats yearManualRun;
  } statsData;

unsigned long statsMilliwats = 0;
unsigned long statsMilliwatMilis = 0;

bool statsAccumulateFlag;
bool statsManualRunFlag;
bool statsRegulateFlag;

unsigned long statsSaveTimer = 0;

void statsSetup() {
  File file = FS.open(STATS_FILENAME, FILE_READ); // dataset on energy diverted
  if (file) {
    file.readBytes((byte*) &statsData, sizeof(statsData));
    file.close();
  }
}

void statsLoop() {

  const unsigned long STATS_MILLIWATS_INTERVAL = 5 * 60000; // 5 minutes
  const unsigned long STATS_SAVE_INTERVAL = 30 * 60000; // 30 minutes
  static unsigned long lastPowerChangeMillis = 0;
  static int lastPower = 0;

  if (lastPowerChangeMillis == 0) { // not counting
    lastPowerChangeMillis = loopStartMillis; // start counting
    statsSaveTimer = loopStartMillis;
  }
  
  time_t t = statsData.timestamp;

  if (day(now()) != day(t)) {

// data written to CSV file; strucure: date , consumed power (regulated, accumulated, manual), insolation Wh per hourly timeslot, span = 04.00 - 19.00 hour, total insolation (Wh/d)
    char buff[150]; // we use 138 + termination byte = 139/150
    sprintf(buff, "DATA%d.CSV", year(t)); // we use 12 char
    File file = FS.open(buff, FILE_WRITE);
    if (file) {
      sprintf(buff, "%d-%02d-%02d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%04d;%05d", // we use 121 char
          year(t), month(t), day(t),
          statsData.day.consumedPower,
          statsData.dayAccumulate.consumedPower,
          statsData.dayManualRun.consumedPower,
          insolData.insolHour03.insolationPower,
          insolData.insolHour04.insolationPower,
          insolData.insolHour05.insolationPower,
          insolData.insolHour06.insolationPower,
          insolData.insolHour07.insolationPower,
          insolData.insolHour08.insolationPower,
          insolData.insolHour09.insolationPower,
          insolData.insolHour10.insolationPower,
          insolData.insolHour11.insolationPower,
          insolData.insolHour12.insolationPower,
          insolData.insolHour13.insolationPower,
          insolData.insolHour14.insolationPower,
          insolData.insolHour15.insolationPower,
          insolData.insolHour16.insolationPower,
          insolData.insolHour17.insolationPower,
          insolData.insolHour18.insolationPower,
          insolData.insolHour19.insolationPower,
          insolData.insolHour20.insolationPower,
          insolData.day.insolationPower);
      file.println(buff);
      file.close();
    }

    if (month(now()) != month(statsData.timestamp)) {
      statsData.month.consumedPower = 0;
      statsData.monthAccumulate.consumedPower = 0;
      statsData.monthManualRun.consumedPower = 0;
    }
    if (year(now()) != year(statsData.timestamp)) { // this section is new
      memset(&statsData, 0, sizeof(statsData));   
    }
    else {
      statsData.day.consumedPower = 0;
      statsData.dayAccumulate.consumedPower = 0;
      statsData.dayManualRun.consumedPower = 0;
    }
    statsData.timestamp = now(); // reset
  }


// accumulated power diverted to heat storage (kWh/day)
  unsigned long interval = loopStartMillis - lastPowerChangeMillis;
  int power = statsEvalCurrentPower();
 
  if (power != lastPower || interval > STATS_MILLIWATS_INTERVAL) {
    if (lastPower > 0) {
      statsMilliwats += (float) lastPower/3.6 * (float) interval/1000; // W to mWh (power * time * 1000, hence 3.6 factor)
      statsMilliwatMilis += interval;
      if (power == 0 || statsMilliwatMilis > STATS_MILLIWATS_INTERVAL) {
        statsAddMilliwats();  
      }
    }
    lastPower = power;
    lastPowerChangeMillis = loopStartMillis;
  }

  if (loopStartMillis - statsSaveTimer > STATS_SAVE_INTERVAL) {
    statsSave();
  }

}

int statsEvalCurrentPower() {

  switch (state) {
    case RegulatorState::ACCUMULATE:
    case RegulatorState::MANUAL_RUN:
    case RegulatorState::REGULATING:
      return elsensPower; // TA power consumption
    default:
      return 0;
  }
}

void statsAddMilliwats() {
  int consumedPower = round((float) statsMilliwats / 1000); // watt
  statsMilliwatMilis = 0;
  statsMilliwats = 0;
  
  statsAccumulateFlag = (state == RegulatorState::ACCUMULATE);
  statsManualRunFlag = (state == RegulatorState::MANUAL_RUN);
  statsRegulateFlag = (state == RegulatorState::REGULATING || state == RegulatorState::MONITORING);
       
 if (statsAccumulateFlag) {
    statsData.dayAccumulate.consumedPower += consumedPower;
    statsData.monthAccumulate.consumedPower += consumedPower;
    statsData.yearAccumulate.consumedPower += consumedPower;
    }
  if (statsManualRunFlag) {
    statsData.dayManualRun.consumedPower += consumedPower;
    statsData.monthManualRun.consumedPower += consumedPower;
    statsData.yearManualRun.consumedPower += consumedPower;
    }
  if (statsRegulateFlag) {
    statsData.day.consumedPower += consumedPower;
    statsData.month.consumedPower += consumedPower;
    statsData.year.consumedPower += consumedPower;
    
  }
}

void statsSave() {
 if (!statsMilliwatMilis)
   return;
  statsAddMilliwats();

  File file = FS.open(STATS_FILENAME, FILE_NEW);
  if (file) {
    file.write((byte*) &statsData, sizeof(statsData));
    file.close();
  }

  eventsWrite(STATS_SAVE_EVENT, (loopStartMillis - statsSaveTimer) / 60000, 0);
  statsSaveTimer = loopStartMillis;
}

int statsRegulatedPowerToday() { // this is used by display and webserver
  if (day(now()) != day(statsData.timestamp))
    return 0;
  return statsData.day.consumedPower; // Wh
}

int statsRegulatedPowerYear() { // this is used by display and webserver
  if (day(now()) != day(statsData.timestamp))
    return 0;
  return statsData.year.consumedPower; // Wh
}

int statsAccumulatedPowerToday() { // this is used by display and webserver
  if (day(now()) != day(statsData.timestamp))
    return 0;
  return statsData.dayAccumulate.consumedPower; // Wh
}

void statsPrintJson(FormattedPrint& out) { // used by webserver
  out.printf(F("{\"timestamp\":%lu,"
      "\"emptyline\":%ld,"
      "\"dayConsumedPower\":%ld,"
      "\"monthConsumedPower\":%ld,"
      "\"yearConsumedPower\":%ld,"
      "\"dayAccumulatePower\":%ld,"
      "\"monthAccumulatePower\":%ld,"
      "\"yearAccumulatePower\":%ld,"    
      "\"dayManualRunPower\":%ld,"
      "\"monthManualRunPower\":%ld,"
      "\"yearManualRunPower\":%ld"),
      statsData.timestamp,
      statsData.day.consumedPower,
      statsData.month.consumedPower,
      statsData.year.consumedPower,
      statsData.dayAccumulate.consumedPower,
      statsData.monthAccumulate.consumedPower,
      statsData.yearAccumulate.consumedPower,    
      statsData.dayManualRun.consumedPower,
      statsData.monthManualRun.consumedPower,
      statsData.yearManualRun.consumedPower);
   out.printf(",\"fn\":\"DATA%d.CSV\"", year());
   out.print('}');
}
