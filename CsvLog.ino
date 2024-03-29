
const char* CSV_DIR = "/CSV/";

void csvLogSetup() {

  time_t t = now() - SECS_PER_WEEK;
  char fn0[15];
  sprintf(fn0, "%02d-%02d-%02d.CSV", year(t) - 2000, month(t), day(t));

#ifdef __SD_H__
  if (!SD.exists(CSV_DIR)) {
    SD.mkdir(CSV_DIR);
  }
  File dir = SD.open(CSV_DIR);
  File file = dir.openNextFile();
  while (file) {
    const char* fn = file.name();
    const char* ext = strchr(fn, '.');
    if (strcmp(ext, ".CSV") == 0) {
      if (strcmp(fn, fn0) < 0) {
        char path[20];
        strcpy(path, CSV_DIR);
        strcat(path, fn);
        SD.remove(path);
      }
    }
    file.close();
    file = dir.openNextFile();
  }
#endif
}

void csvLogLoop() {

  static char buff[2000];
  static CStringBuilder lines(buff, sizeof(buff));

  unsigned long t = now();

  if (lines.length() > sizeof(buff) - 100 || (state == RegulatorState::MONITORING && lines.length())) { // OR expression now included
    char fn[20];
    sprintf(fn, "%s%02d-%02d-%02d.CSV", CSV_DIR, year(t) - 2000, month(t), day(t));
    File file = FS.open(fn, FILE_WRITE);
    if (file) {
      if (file.size() == 0) {
        file.println(F("t;HP;insolP;invAC;metAC;trhld;insolR;elsP;V;"));
      }
      file.print(buff);
      file.close();
    }
    lines.reset();
  }

if (state == RegulatorState::REGULATING) { 
    lines.printf(F("%02d:%02d:%02d;%03d;%04d;%04d;%05d;%04d;%04d;%03d;%03d\r\n"), 
    hour(t), minute(t), second(t),
    heatingPower, insolRefAvg, inverterAC, meterPower, insol, tresholdAvg, elsensPower, voltage);
  }
}


void csvLogPrintJson(FormattedPrint& out) {
#ifdef FS
  boolean first = true;
  out.print(F("{\"f\":["));
#ifdef __SD_H__
  File dir = SD.open(CSV_DIR);
  File file;
  while (file = dir.openNextFile()) { // @suppress("Assignment in condition")
    const char* fn = file.name();
#else
  Dir dir = SPIFFS.openDir(CSV_DIR);
  while (dir.next()) {
    File file = dir.openFile("r");
    const char* fn = file.name() + sizeof(CSV_DIR) + 1;
#endif
    const char* ext = strchr(fn, '.');
    if (strcmp(ext, ".CSV") == 0) {
      if (first) {
        first = false;
      } else {
        out.print(',');
      }
      out.printf(F("{\"fn\":\"%s\",\"size\":%ld}"), fn, file.size() / 1000);
    }
    file.close();
  }
  out.print(F("]}"));
#endif
}
