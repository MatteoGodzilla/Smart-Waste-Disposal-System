#include <Arduino.h>
#include <Wire.h>

#include "pins.h"
#include "Scheduler.h"
#include "TemperatureTask.h"
#include "DashboardTask.h"
#include "WasteTask.h"
#include "SleepTask.h"
#include "LCDManager.h"
#include "SMDSFiniteStateMachine.h"

Scheduler scheduler;

void setup() {
    //Initialize pins
    pinMode(L1, OUTPUT);
    pinMode(L2, OUTPUT);
    pinMode(MOTOR, OUTPUT);
    pinMode(SONAR_TRIG, OUTPUT);

    pinMode(OPEN_BTN, INPUT);
    pinMode(CLOSE_BTN, INPUT);
    pinMode(PIR, INPUT);
    pinMode(TEMP, INPUT);
    pinMode(SONAR_ECHO, INPUT);

    //Initialize scheduler and tasks
	scheduler.init();

    SMDSFiniteStateMachine* fsm = new SMDSFiniteStateMachine();

    LCDManager* lcdManager = new LCDManager();

    TemperatureTask* temperatureTask = new TemperatureTask(fsm);
    scheduler.addTask(temperatureTask);

    DashboardTask* dashboardTask = new DashboardTask();
    scheduler.addTask(dashboardTask);

    WasteTask* wasteTask = new WasteTask(fsm);
    scheduler.addTask(wasteTask);

    SleepTask* sleepTask = new SleepTask(fsm);
    scheduler.addTask(sleepTask);
}

void loop() {
	scheduler.schedule();
}
