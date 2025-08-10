bool getRPM(int *rpm)
{
    sendPid(PID_ENGIN_PRM);  // 0x0C 요청 전송
    unsigned long __timeout = millis();

    while(millis()-__timeout < 1000)  // 1초 타임아웃
    {
        unsigned char len = 0;
        unsigned char buf[8];

        if (CAN_MSGAVAIL == CAN.checkReceive()) {
            CAN.readMsgBuf(&len, buf);

            if(buf[1] == 0x41 && buf[2] == PID_ENGIN_PRM)
            {
                // RPM 값은 두 바이트로 전달되며, (A*256 + B)/4 공식 사용
                int A = buf[3];
                int B = buf[4];
                *rpm = ((A * 256) + B) / 4;
                return 1;
            }
        }
    }
    return 0;
}
