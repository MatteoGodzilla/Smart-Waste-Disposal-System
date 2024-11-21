#include <Arduino.h>
#include <Wire.h>

#include "pins.h"

#include "Scheduler.h"
#include "TemperatureTask.h"
#include "DashboardTask.h"
#include "WasteTask.h"
#include "SleepTask.h"
#include "LCDManager.h"

Scheduler scheduler;

void setup() {
    //Initialize pins
    pinMode(L1, OUTPUT);
    pinMode(L2, OUTPUT);
    pinMode(MOTOR, OUTPUT);
    pinMode(SONAR_ECHO, OUTPUT);

    pinMode(OPEN_BTN, INPUT);
    pinMode(CLOSE_BTN, INPUT);
    pinMode(PIR, INPUT);
    pinMode(TEMP, INPUT);
    pinMode(SONAR_TRIG, INPUT);

    //Initialize scheduler and tasks
	scheduler.init();

    LCDManager* lcdManager = new LCDManager();

    TemperatureTask* temperatureTask = new TemperatureTask();
    scheduler.addTask(temperatureTask);

    DashboardTask* dashboardTask = new DashboardTask();
    scheduler.addTask(dashboardTask);

    WasteTask* wasteTask = new WasteTask();
    scheduler.addTask(wasteTask);

    SleepTask* sleepTask = new SleepTask();
    scheduler.addTask(sleepTask);
}

void loop() {
	scheduler.schedule();
}
