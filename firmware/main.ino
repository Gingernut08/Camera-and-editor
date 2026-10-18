#include <SPI.h>
#include <SD.h>
#include <Arducam_Mega.h>


// HIGH QUALITY: CAM_IMAGE_MODE_QVGA
// LOW_QUALITY:  CAM_IMAGE_MODE_VGA
const CAM_IMAGE_MODE IMAGE SIZE = CAM_IMAGE_MODE_VGA;

const uint8_t CAMERA_COUNT = 5;
const uint8_t BUFFER_SIZE = 255;

// Pico GPIO
const uint8_t CAMERA_CS[CAMERA_COUNT] = {0, 1, 2, 3, 4};
const uint8_t SD_CS = 5;
const uint8_t SHUTTER_BUTTON = 14;

// Camera object per CS pin
Arducam_Mega cameras[CAMERA_COUNT] = {
    Arducam_Mega(CAMERA_CS[0]),
    Arducam_Mega(CAMERA_CS[1]),
    Arducam_Mega(CAMERA_CS[2]),
    Arducam_Mega(CAMERA_CS[3]),
    Arducam_Mega(CAMERA_CS[4])
};

bool cameraReady[CAMERA_COUNT] = {false};
uint32_t captureNumber = 0;

// DESELECT ALL CS
void deslectAll(){
    for (uint8_t i = 0; i < CAMERA_COUNT; i++){
        digitalWrite(CAMERA_CS[i], HIGH);
    }
    digitalWrite(SD_CS, HIGH);
}

// ACTIVATE CAMERA
bool initiliseCamera(uint8_t index){
    Serial.print("Initialising camera ");
    Serial.println(index + 1)

    deslectAll();

    if (cameras{index}.bbegin() != CAM_ERR_SUCCESS){
        Serial.println("Camera initialisation failed.");
        return false;
    }

    Serial.println("Camera ready.");
    return true;
}

bool saveCameraImage(uint8_t index){
    char filename[32];

    snprintf(
        filename,
        sizeof(filename),
        "/CAM%u_%04lu.jpg",
        index + 1,
        (unsigned long)captureNumber
    );

    Serial.print("Capturing camera ");
    Serial.println(index + 1);

    deslectAll();
}
