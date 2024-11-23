#include "DashboardTask.h"

DashboardTask::DashboardTask(){
    active = true;
}

void DashboardTask::bindTemp(TemperatureTask* tTask){
    temperatureTask = tTask;
}

void DashboardTask::bindWaste(WasteTask* wTask){
    wasteTask = wTask;
}

void DashboardTask::execute(){

}