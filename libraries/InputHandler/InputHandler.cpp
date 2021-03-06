#include "InputHandler.h"

#define CHUNK 5

typedef struct encoderCDT {
  uint8_t pin1;
  uint8_t pin2;
  uint8_t code;
  uint32_t lastRead;
  uint8_t lastValue;
  int8_t count;
  uint8_t limit;
  int8_t actualDirection;
} encoderCDT;

InputHandler::InputHandler() {
  activeTypes = (uint8_t*) malloc(CHUNK * (sizeof(*activeTypes)));
  activePins = (uint8_t*) malloc(CHUNK * (sizeof(*activePins)));
  pins = (uint8_t*) malloc(CHUNK * (sizeof(*pins)));
  currentLenght = CHUNK;
  count = activePinsCount = 0;

  activeEncoders = (encoderADT*) malloc(CHUNK * (sizeof(*activeEncoders)));
  encoders = (encoderADT*) malloc(CHUNK * (sizeof(*encoders)));
  currentEncoderLength = CHUNK;
  encoderCount = activeEncoderCount = 0;
}

void InputHandler::registerPin(uint8_t pin, uint8_t activeType, bool internalPullup) {
  pinMode(pin, INPUT);
  
  if (activeType == 1 && internalPullup) {
    digitalWrite(pin, 1);
  }
  
  count++;
  if (count == currentLenght) {
    currentLenght += CHUNK;
    activeTypes = (uint8_t*) realloc(activeTypes, sizeof(*activeTypes) * currentLenght);
    activePins = (uint8_t*) realloc(activePins, sizeof(*activePins) * currentLenght);
    pins = (uint8_t*) realloc(pins, sizeof(*pins) * currentLenght);
  }
  activeTypes[count - 1] = activeType;
  pins[count - 1] = pin;
}

void InputHandler::registerPin(uint8_t pin, uint8_t activeType) {
  registerPin(pin, activeType, false);
}

void InputHandler::registerEncoder(uint8_t pin1, uint8_t pin2, uint8_t code, uint8_t limit, bool internalPullup1, bool internalPullup2) {
  pinMode(pin1, INPUT);
  if (internalPullup1) {
    digitalWrite(pin1, 1);
  }

  pinMode(pin2, INPUT);
  if (internalPullup2) {
    digitalWrite(pin2, 1);
  }
  
  encoderCount++;
  if (encoderCount == currentEncoderLength) {
    currentEncoderLength += CHUNK;
    activeEncoders = (encoderADT*) realloc(activeEncoders, sizeof(*activeEncoders) * currentEncoderLength);
    encoders = (encoderADT*) realloc(encoders, sizeof(*encoders) * currentEncoderLength);
  }
  encoders[encoderCount - 1] = {
    pin1,
    pin2,
    code,
    0,
    0,
    limit,
    0
  };
}

void InputHandler::registerEncoder(uint8_t pin1, uint8_t pin2, uint8_t code, uint8_t limit) {
  registerEncoder(pin1, pin2, code, limit, false, false);
}

uint8_t InputHandler::getActivePinsCount() {
  return activePinsCount;
}

const uint8_t * InputHandler::readInputs() {
  activePinsCount = 0;
  
  for (uint8_t ix = 0; ix < count; ix++) {
    if(digitalRead(pins[ix]) == activeTypes[ix]){
      activePins[activePinsCount++] = pins[ix];
    }
  }
  
  return activePins;
}

uint8_t InputHandler::getActiveEncoderCount() {
  return activeEncoderCount;
}

const encoderADT * InputHandler::readEncoders() {
  activeEncoderCount = 0;
  
  for (uint8_t ix = 0; ix < encoderCount; ix++) {
    if (updateEncoder(encoders[ix])) {
      activeEncoders[activeEncoderCount++] = encoders[ix];
    }
  }
  
  return activeEncoders;
}

uint8_t InputHandler::getEncoderCode(encoderADT encoder) {
  if (encoder == NULL) return 0;
  return encoder->code;
}

int8_t InputHandler::getEncoderDirection(encoderADT encoder) {
  if (encoder == NULL) return 0;
  return encoder->actualDirection;
}

// Private
uint8_t InputHandler::updateEncoder(encoderADT encoder) {
    if (millis() - encoder->lastRead > CLEAR_ENCODERS) {
      encoder->lastValue = 0;
      encoder->count = 0;
      encoder->actualDirection = 0;
    }

    uint8_t MSB = digitalRead(encoder->pin1);
    uint8_t LSB = digitalRead(encoder->pin2);
    uint8_t encoded = (MSB << 1) | LSB;               //Converting the 2 pin value to single number
    uint8_t sum  = (encoder->lastValue << 2) | encoded;      //Adding it to the previous encoded value
    encoder->lastValue = encoded;           //Store this value for next time
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        if (encoder->count < 0) encoder->count = 0;
        if (encoder->count == encoder->limit) {        //Used to increment the number of turns needed to make a change.
            encoder->count = 0;
            encoder->actualDirection = 1;
        } else {
            encoder->count++;
        }
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        if (encoder->count > 0) encoder->count = 0;
        if (encoder->count == encoder->limit) {
            encoder->count = 0;
            encoder->actualDirection = -1;
        } else {
            encoder->count--;
        }
    } else {
          encoder->actualDirection = 0;
    }
    
    encoder->lastRead = millis();
}
