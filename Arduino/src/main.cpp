#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
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

    //Initialize libraries

    //Initialize scheduler and tasks
	scheduler.init();

    SMDSFiniteStateMachine* fsm = new SMDSFiniteStateMachine();

    TemperatureTask* temperatureTask = new TemperatureTask();
    scheduler.addTask(temperatureTask);

    DashboardTask* dashboardTask = new DashboardTask();
    scheduler.addTask(dashboardTask);

    WasteTask* wasteTask = new WasteTask();
    scheduler.addTask(wasteTask);

    SleepTask* sleepTask = new SleepTask();
    scheduler.addTask(sleepTask);

    LCDManager* lcd = new LCDManager();
    scheduler.addTask(lcd);

    //Bind tasks each other if necessary
    temperatureTask->bindFSM(fsm);
    temperatureTask->bindDashboard(dashboardTask);
    dashboardTask->bindTemp(temperatureTask);
    dashboardTask->bindWaste(wasteTask);
    wasteTask->bindFSM(fsm);
    wasteTask->bindDashboard(dashboardTask);
    sleepTask->bindFSM(fsm);
    lcd->bindFSM(fsm);
}

void loop() {
	scheduler.schedule();
}
