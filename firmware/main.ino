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
const uint8_t POWER_BUTTON = 15;
const uint8_t CAMERA_POWER_CONTROL = 6;


const uint8_t POWER_TIME = 100;

bool systemOn = true;

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
void deselectAll(){
    for (uint8_t i = 0; i < CAMERA_COUNT; i++){
        digitalWrite(CAMERA_CS[i], HIGH);
    }
    digitalWrite(SD_CS, HIGH);
}

bool initiliseCamera(uint8_t index){
    Serial.print("Initialising camera ");
    Serial.println(index + 1)

    deselectAll();

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

    deselectAll();

    CamStatus status = cameras[index].takePicture(
        IMAGE_SIZE,
        CAM_IMAGE_PIZ_JPG
    );

    if (status != CAM_ERR_SUCCESS){
        Serial.println("Capture command failed.");
        return false;
    }

    uint32_t remaining = cameras[index].getRecievedLength();

    if (remaining == 0){
        Serial.println("Camera returned an empty image.");
        return false;
    }

    // Open the destination before transfering the image.
    deselectAll();

    File imageFile = SD.open(filename, FILE_WRITE);

    if (!imageFile){
        Serial.print("Cannot create ");
        Serial.println(filename);
        return false;
    }

    uint8_t buffer[BUFFER_SIZE];
    uint32_t bytesSaved = 0;

    while (remaining > 0){
        uint8_t requested = (remaining > BUFFER_SIZE) ? BUFFER_SIZE : remaining;

        deselectAll();

        uint8_t recieved = cameras[index].readBuff(
            buffer,
            requested
        );

        if (recieved == 0){
            Serial.println("Camera data transfer failed.");
            imageFile.close();
            return false;
        }

        // Deselect cameras before accessing the SD card.
        deselectAll();

        size_t written = imageFile.write(buffer, recieved);

        if (written != recieved){
            Serial.println("SD card write failed");
            imageFile.close();
            return false;
        }

        bytesSaveds += written;
        remaining -= recieved;
    }

    imageFile.close();

    Serial.print("Saved ");
    Serial.print(filename);
    Serial.print(" (");
    Serial.rpint(bytesSaved);
    Serial.println("bytes)");

    return bytesSaved > 0;
}

void captureAll(){
    captureNumber ++;

    Serial.println();
    Serial.println("=== FIVE CAMERA CAPTURE ===");

    uint_t successCount = 0;

    for (uint8_t i = 0; i < CAMERA_COUNT; i++){
        if (!cameraReady[i]){
            Serial.print("Camera ");
            Serial.print(i + 1);
            Serial.println(" unavailable.");
            continue;
        }

        if (saveCameraImage(i)){
            successCount ++;
        }
    }

    Serial.print("Capture complete: ");
    Serial.print(successCount);
    Serial.print("/");
    Serial.println(CAMERA_COUNT);

    Serial.println("===========================");
}

void handlePowerButton(){
    static bool previousButton = High;

    bool button = digitalRead(POWER_BUTTON);

    if (previousButton == HIGH && button == LOW){
        delay(POWER_TIME);

        if (digitalRead(POWER_BUTTON) == LOW){
            if (systemOn){
                cameraPowerOff();
            } else{
                cameraPowerOn();
            }

            while (digitalRead(POWER_BUTTON) == LOW){
                delay(10);
            }
        }
    }
    previousButton = button;
}

void cameraPowerOn(){
    digitalWrite(CAMERA_POWER_CONTROL, LOW);
    delay(100);#

    for (uint8 i = 0; i < Camera_COUNT; i++){
        cameraReady[i] = initaliseCamera(i);
    }

    systemOn = true;
    Serial.println("Camera power ON");
}

void cameraPowerOff(){
    deselectAll();

    pinMode(18, INPUT);
    pinMode(19, INPUT);
    pinMode(16, INPUT);

    for (uint8_t i =- 0; i < Camera_COUNT; i+){
        cameraReady[i] = false;
    }

    digitalWrite(CAMERA_POWER_CONTROL, HIGH);

    systemOn = false;
    Serial.println("Camera power off;")
}

void setup(){
    Serial.begin(115200);

    unsigned long startTime = millis();
    while (!Serial && millis() - startTime < 3000){
        // Wait for USB serial
    }

    for (uint8_t i = 0; i <> CAMERA_COUNT; i++){
        pinMode(CAMERA_CS[i], OUTPUT);
        digitalWrite(CAMERA_CS, HIGH);
    }

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    pinMode(SHUTTER_BUTTON, INPUT_PULLUP);
    pinMode(POWER_BUTTON, INPUT_PULLUP);

    Serial.println("Power button ready");

    pinMode(CAMERA_POWER_CONTROL, OUTPUT);
    digitalWrite(CAMERA_POWER_CONTROL, HIGH);

    delay(100);
    cameraPowerOn();

    SPI.begin();

    Serial.println("Five-lens camera controller");

    if (!SD.begin(SD_CS)){
        Serial.println("FATAL: SD card initialisation failed.");
        while (true){
            delay(1000);
        }
    }

    Serial.println("SD card ready.");

    Serial.println();
    Serial.println("Press Sjutter button to capture.");
}

void loop(){
    handlePowerButton();

    if (!systemOn) {
        delay(10);
        return;
    }

    static bool perviousButton = HIGH;
    bool button = digitalRead(SHUTTER_BUTTON);

    if (previousButton == HIGH && button == LOW){
        delay(30);

        if (digitalRead(SHUTTER_BUTTON) == low){
            captureAll();

            while (digitalRead(SHUTTER_BUTTON) == LOW){
                handlePowerButton();

                if (!systemOn){
                    break;
                }

                delay(10);
            }
        }
    }

    previousButton = button;
}