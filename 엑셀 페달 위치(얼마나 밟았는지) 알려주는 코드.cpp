#define PID_ACCEL_PEDAL_POS 0x49

bool getAccelPedalPos(float *pos)
{
    sendPid(PID_ACCEL_PEDAL_POS);  // 0x49 요청 전송
    unsigned long __timeout = millis();

    while(millis() - __timeout < 1000)  // 1초 타임아웃
    {
        unsigned char len = 0;
        unsigned char buf[8];

        if (CAN_MSGAVAIL == CAN.checkReceive()) {
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
