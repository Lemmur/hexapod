#include <Adafruit_PWMServoDriver.h>

#include "Leg.h"
#include "Gait2.h"
#include "Mark2Config.h"
#include "SimpleMovements.h"

static Point zero(0,0,0);

static const int N = 6;
static Leg legs[N];
static Leg mandibles[2];

static Adafruit_PWMServoDriver leftBoard = Adafruit_PWMServoDriver(0x43);
static Adafruit_PWMServoDriver rightBoard = Adafruit_PWMServoDriver(0x40);

static SimpleMovements moveSimple(legs, N, mandibles, 2);
static Gait2 gait(legs);

char command = 0;
unsigned long lastCommandTime = 0;

SmoothFloat fpitch(0,0);
SmoothFloat froll(0,0);
SmoothFloat fyaw(0,0);
SmoothFloat fbodyX(0,0);
SmoothFloat fbodyY(0,0);
SmoothFloat fbodyZ(0,0);

SmoothFloat fRMandibleX(0,0);
SmoothFloat fRMandibleY(0,0);
SmoothFloat fRMandibleZ(0,0);
SmoothFloat fLMandibleX(0,0);
SmoothFloat fLMandibleY(0,0);
SmoothFloat fLMandibleZ(0,0);

void tickMovements()
{
    gait.tick();

#ifdef SMOOTH_ANGLES
    for (int i = 0; i < N; i++)
        legs[i].tick();
#endif

    static unsigned long _lastTickTime = 0;
    const unsigned long now = millis();
    const unsigned long deltaT = now - _lastTickTime;
    const float angleStepDelta = ((PI * 0.25) * 0.001) * (float) deltaT;
    const float shiftStepDelta = (100.0 * 0.001) * (float) deltaT;

    Point bodyShift(fbodyX.getCurrent(shiftStepDelta),
                    fbodyY.getCurrent(shiftStepDelta),
                    fbodyZ.getCurrent(shiftStepDelta));

    moveSimple.shiftAbsolute(
        bodyShift,
        fpitch.getCurrent(angleStepDelta),
        froll.getCurrent(angleStepDelta),
        fyaw.getCurrent(angleStepDelta));

    _lastTickTime = now;
}

bool tryMultibyte(char cmd)
{
    static unsigned long lastMoveCommandTime = 0;
    const unsigned long now = millis();

    if (cmd == 'b')
    {
        while (Serial.available() < 7)
            tickMovements();

        char x, y, z, pitch, roll, yaw;
        x = Serial.read();
        y = Serial.read();
        z = Serial.read();
        pitch = Serial.read();
        roll = Serial.read();
        yaw = Serial.read();

        // Confirm footer byte
        if (Serial.read() != 'B')
            return false;

        

        fbodyX.setTarget(normalizeByte(x, 40));
        fbodyY.setTarget(normalizeByte(y, 40));
        fbodyZ.setTarget(normalizeByte(z, 40));
        fpitch.setTarget(normalizeByte(pitch, PI / 6));
        froll.setTarget(normalizeByte(roll, PI / 6));
        fyaw.setTarget(normalizeByte(yaw, PI / 6));

        return true;
    }

    if (cmd == 'm')
    {
        while (Serial.available() < 5)
            tickMovements();

        char x,y,turn, speed;

        x = Serial.read();
        y = Serial.read();
        turn = Serial.read();
        speed = Serial.read();

        // Confirm footer byte
        if (Serial.read() != 'M')
            return false;

        gait.setStep(Point(normalizeByte(x, 40),
                           normalizeByte(y, 40),
                           0),
                     turn != 0,
                     normalizeByte(turn, 1.0));

        if (speed < 0)
            speed = 0;

        gait.setSpeed(normalizeByte(speed, 2.0));

        return true;
    }

    if (cmd == 'g')
    {
        while (Serial.available() < 2)
            tickMovements();

        char gaitId = Serial.read();

        // Confirm footer byte
        if (Serial.read() != 'G')
            return false;

        switch (gaitId)
        {
            case 1:
                gait.setGait2x3();
                break;
            case 2:
                gait.setGait6x1();
                break;
            case 3:
                gait.setGait3x2();
                break;
        }

        lastMoveCommandTime = now;
        return true;
    }

    if (cmd == 'p')
    {
        while (Serial.available() < 7)
            tickMovements();
            
        char buffer[6];
        for (int i = 0; i < sizeof(buffer); ++i)
          buffer[i] = Serial.read();

        // Confirm footer byte
        if (Serial.read() != 'P')
            return false;
            
        fRMandibleX.setTarget(normalizeByte(buffer[0], 50));
        fRMandibleY.setTarget(normalizeByte(buffer[1], 50));
        fRMandibleZ.setTarget(normalizeByte(buffer[2], 50));

        fLMandibleX.setTarget(normalizeByte(buffer[3], 50));
        fLMandibleY.setTarget(normalizeByte(buffer[4], 50));
        fLMandibleZ.setTarget(normalizeByte(buffer[5], 50));

        return true;
    }

    if (now - lastMoveCommandTime > 1000)
    {
        gait.setSpeed(0);
        gait.setStep(zero, false, 0);

        lastMoveCommandTime = now + 1000000;
    }

    return false;
}

void setup()
{
    Serial.begin(9600);
    rightBoard.begin();
    rightBoard.setOscillatorFrequency(27000000);
    rightBoard.setPWMFreq(SERVO_FREQ);
    
    leftBoard.begin();
    leftBoard.setOscillatorFrequency(27000000);
    leftBoard.setPWMFreq(SERVO_FREQ);

    LegConfiguration::apply(leftBoard, rightBoard, legs, N);

    moveSimple.rememberDefault();

    for (Leg* leg = legs; leg < legs + N; leg++)
        leg->reachRelativeToDefault(zero);

    for (Leg* mandible = mandibles; mandible < mandibles + 2; mandible++)
        mandible->reachRelativeToDefault(zero);

    gait.setGait2x3();

//    fbodyX.setTarget(normalizeByte(0, 40));
//        fbodyY.setTarget(normalizeByte(0, 40));
//        fbodyZ.setTarget(normalizeByte(0, 40));
//        fpitch.setTarget(normalizeByte(0, PI / 6));
//        froll.setTarget(normalizeByte(0, PI / 6));
//        fyaw.setTarget(normalizeByte(0, PI / 6));

    gait.setStep(Point(normalizeByte(100, 50),
                           normalizeByte(100, 50),
                           30),
                     false,
                     normalizeByte(0, 1.0));

        gait.setSpeed(normalizeByte(200, 2.0));
}

void loop()
{
    tickMovements();
    char incoming;

    if (Serial.available() > 0) {
        incoming = Serial.read();
        Serial.println(String(incoming));
    }
    else
        return;

    if (tryMultibyte(incoming))
        return;
}