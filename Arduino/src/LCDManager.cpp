#include "LCDManager.h"

LCDManager::LCDManager() {
    lcd = new LiquidCrystal_I2C(0x27,20,4);
    lcd->init();
    lcd->backlight();
}