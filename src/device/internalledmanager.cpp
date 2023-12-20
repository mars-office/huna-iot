#include <Arduino.h>

#include "internalledmanager.h"

InternalLedManager::InternalLedManager()
{
  
}

InternalLedManager::~InternalLedManager()
{
}

void InternalLedManager::init()
{
  pinMode(LED_BUILTIN, OUTPUT);
}

void InternalLedManager::on()
{
  digitalWrite(LED_BUILTIN, HIGH);
}

void InternalLedManager::off()
{
  digitalWrite(LED_BUILTIN, LOW);
}
