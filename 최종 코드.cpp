#include <SPI.h>
#include "mcp_can.h"

#define SPI_CS_PIN  9

MCP_CAN CAN(SPI_CS_PIN);  // CAN 객체 생성

#define PID_ENGIN_PRM       0x0C
#define PID_VEHICLE_SPEED   0x0D
#define PID_ACCEL_PEDAL_POS 0x49

#define CAN_ID_PID          0x7DF

void set_mask_filt()
{
    CAN.init_Mask(0, 0, 0x7FC);
    CAN.init_Mask(1, 0, 0x7FC);

    CAN.init_Filt(0, 0, 0x7E8);                 
    CAN.init_Filt(1, 0, 0x7E8);
    CAN.init_Filt(2, 0, 0x7E8);
    CAN.init_Filt(3, 0, 0x7E8);
    CAN.init_Filt(4, 0, 0x7E8); 
    CAN.init_Filt(5, 0, 0x7E8);
}

void sendPid(unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}

bool getSpeed(int *s)
{
    sendPid(PID_VEHICLE_SPEED);
    unsigned long __timeout = millis();

    while(millis() - __timeout < 1000)
    {
        unsigned char len = 0;
        unsigned char buf[8];

        if (CAN_MSGAVAIL == CAN.checkReceive())
        {
            CAN.readMsgBuf(&len, buf);

            if(buf[1] == 0x41 && buf[2] == PID_VEHICLE_SPEED)
            {
                *s = buf[3];
                return true;
            }
        }
    }
    return false;
}

bool getRPM(int *rpm)
{
    sendPid(PID_ENGIN_PRM);
    unsigned long __timeout = millis();

    while(millis() - __timeout < 1000)
    {
        unsigned char len = 0;
        unsigned char buf[8];

        if (CAN_MSGAVAIL == CAN.checkReceive())
        {
            CAN.readMsgBuf(&len, buf);

            if(buf[1] == 0x41 && buf[2] == PID_ENGIN_PRM)
            {
                int A = buf[3];
                int B = buf[4];
                *rpm = ((A * 256) + B) / 4;
                return true;
            }
        }
    }
    return false;
}

bool getAccelPedalPos(float *pos)
{
    sendPid(PID_ACCEL_PEDAL_POS);
    unsigned long __timeout = millis();

    while(millis() - __timeout < 1000)
    {
        unsigned char len = 0;
        unsigned char buf[8];

        if (CAN_MSGAVAIL == CAN.checkReceive())
        {
            CAN.readMsgBuf(&len, buf);

            if (buf[1] == 0x41 && buf[2] == PID_ACCEL_PEDAL_POS)
            {
                int A = buf[3];
                *pos = (A * 100.0) / 255.0;
                return true;
            }
        }
    }
    return false;
}

const int pinPwrCtrl = 12;

void setup() {
    Serial.begin(115200);
    while(!Serial);

    pinMode(pinPwrCtrl, OUTPUT);
    digitalWrite(pinPwrCtrl, HIGH);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");
    set_mask_filt();
}

void loop() {
    int __speed = 0;
    int __rpm = 0;
    float __pedal = 0.0;

    if(getSpeed(&__speed))
    {
        Serial.print("Vehicle Speed: ");
        Serial.print(__speed);
        Serial.println(" km/h");
    }

    if(getRPM(&__rpm))
    {
        Serial.print("Engine RPM: ");
        Serial.println(__rpm);
    }

    if(getAccelPedalPos(&__pedal))
    {
        Serial.print("Accelerator Pedal Position: ");
        Serial.print(__pedal, 1);
        Serial.println(" %");
    }

    delay(500);
}
