#include "AudioInputHandler.h"
#include "LinkedList.h"

typedef struct relayInputCDT {
    int8_t pinLeft;
    int8_t pinRight;
    int8_t pinLeftSW;
    int8_t pinRightSW;
    uint8_t pinLeftActive;
    uint8_t pinRightActive;
    uint8_t pinLeftSWActive;
    uint8_t pinRightSWActive;
} relayInputCDT;

typedef struct audioEntryCDT {
    uint8_t tdaInput;
    LinkedList relayInputs;
} audioEntryCDT;

audioEntryADT addAudioEntry(uint8_t tdaInput) {
    audioEntryADT audioEntry = (audioEntryADT) malloc(sizeof(*audioEntry));
    if (audioEntry == NULL) return NULL;

    audioEntry->tdaInput = tdaInput;
    audioEntry->relayInputs = LinkedList();
    return audioEntry;
}

relayInputADT registerRelay(
    audioEntryADT audioEntry, 
    int8_t pinLeft, 
    uint8_t pinLeftActive, 
    int8_t pinRight, 
    uint8_t pinRightActive) {
    if (audioEntry == NULL || (pinLeft < 0 && pinRight < 0)) return NULL;
    relayInputADT relayInput = (relayInputADT) malloc(sizeof(*relayInput));
    if (relayInput == NULL) return NULL;

    relayInput->pinLeft = pinLeft;
    relayInput->pinRight = pinRight;
    relayInput->pinLeftActive = pinLeftActive;
    relayInput->pinRightActive = pinRightActive;

    audioEntry->relayInputs.add(relayInput);
}

void setAudioInput(TDA7303 * amplifier, audioEntryADT audioEntry) {
    if (audioEntry == NULL) return NULL;
    audioEntry->relayInputs.setIterator();
    while (audioEntry->relayInputs.hasNext()) {
        relayInputADT relayInput = (relayInputADT) audioEntry->relayInputs.getElement(audioEntry->relayInputs.next());
        if (relayInput->pinLeft >= 0) {
            digitalWrite(relayInput->pinLeft, relayInput->pinLeftActive);
        }
        if (relayInput->pinRight >= 0) {
            digitalWrite(relayInput->pinRight, relayInput->pinRightActive);
        }
    }
    if (amplifier != NULL) amplifier->setInput(audioEntry->tdaInput);
}

void setAudioInputLeft(TDA7303 * amplifier, audioEntryADT audioEntry) {
    if (audioEntry == NULL) return NULL;
    audioEntry->relayInputs.setIterator();
    while (audioEntry->relayInputs.hasNext()) {
        relayInputADT relayInput = (relayInputADT) audioEntry->relayInputs.getElement(audioEntry->relayInputs.next());
        if (relayInput->pinLeft >= 0) {
            digitalWrite(relayInput->pinLeft, relayInput->pinLeftActive);
        }
    }
    if (amplifier != NULL) amplifier->setInput(audioEntry->tdaInput);
}

void setAudioInputRight(TDA7303 * amplifier, audioEntryADT audioEntry) {
    if (audioEntry == NULL) return NULL;
    audioEntry->relayInputs.setIterator();
    while (audioEntry->relayInputs.hasNext()) {
        relayInputADT relayInput = (relayInputADT) audioEntry->relayInputs.getElement(audioEntry->relayInputs.next());
        if (relayInput->pinRight >= 0) {
            digitalWrite(relayInput->pinRight, relayInput->pinRightActive);
        }
    }
    if (amplifier != NULL) amplifier->setInput(audioEntry->tdaInput);
}

uint8_t getTDAInput(audioEntryADT audioEntry) {
    if (audioEntry == NULL) return 0;
    return audioEntry->tdaInput;
}
