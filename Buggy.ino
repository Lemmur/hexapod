#include "Leg.h"

// TODO: Remove this temporary goo
void log(const char* x) { Serial.println(x); }
void log(float x) { Serial.println(x); }
void log(int x) { Serial.println(x); }

static Adafruit_PWMServoDriver leftBoard = Adafruit_PWMServoDriver(0x41);
static Adafruit_PWMServoDriver rightBoard = Adafruit_PWMServoDriver(0x43);

static Leg legs[6];
static int N = 6;//sizeof(legs) / sizeof(Leg);

static Leg* rightLegs = legs;
static Leg* leftLegs = legs + N / 2;

static Point zero(0,0,0);

static Point stateLinearMovement;

void processState()
{
    // TODO:
    for (int i = 0; i < N; ++i)
        legs[i].reachRelativeToCurrent(stateLinearMovement);
}

void walk(int steps, Point direction)
{
    for (int i = 0; i < steps; i++)
    {
        for(float p = 0; p <= 2; p += 0.06) // тут регулировать скорость макс 0.08
        {
            float progress; 
            float height1;
            float height2;
            if (p < 1) 
            {
                progress = p;
                height1 = 0;
                height2 = direction.z * (0.5 - fabs(0.5 - p));
            }
            else 
            {
                progress = 1 - (p - 1);
                height1 = direction.z * (0.5 - fabs(1.5 - p));
                height2 = 0;
            }
          
            Point group1(- direction.x / 2 + (direction.x - progress * direction.x),
                         - direction.y / 2 + (direction.y - progress * direction.y),
                         height1);
            Point group2(- direction.x / 2 + progress * direction.x,
                         - direction.y / 2 + progress * direction.y,
                         height2);


            if (legs[0].getCurrentRelative().maxDistance(group1) > 10)
                smoothTo(group1, 0);
            if (legs[1].getCurrentRelative().maxDistance(group2) > 5)
                smoothTo(group2, 1);
                    
            for (int li = 0; li < N; li+=2)
            {
                    
              
                legs[li].reachRelativeToDefault(group1);                  
                legs[li + 1].reachRelativeToDefault(group2);                  
            }

            delay(1);
        }        
    }
}

void smoothTo(Point& to)
{
    smoothTo(to, 0);
    smoothTo(to, 1);
}

// Relative to default!
void smoothTo(Point& to, int legGroup)
{
    Point relative[N];
    for (int i = legGroup; i < N; i += 2)
        relative[i].assign((legs[i].getDefaultPos() + to) - legs[i].getCurrentPos());
  
    for(float p = 0; p <= 1; p += 0.05)
    {
        for (int li = legGroup; li < N; li += 2)
        {
            Point currSubStep = relative[li] * p;
            Point currStep = legs[li].getCurrentPos() + currSubStep;
            currStep.z = legs[li].getDefaultPos().z + to.z + 50 * (0.5 - fabs(0.5 - p));
            legs[li].reach(currStep);
        }
        
        delay(5);
    }
}

void configureLegs()
{
    rightBoard.begin();
    rightBoard.setOscillatorFrequency(27000000);
    rightBoard.setPWMFreq(SERVO_FREQ);

    leftBoard.begin();
    leftBoard.setOscillatorFrequency(27000000);
    leftBoard.setPWMFreq(SERVO_FREQ);
    
    rightLegs[0].attach(rightBoard, 7, 6, 5);
    rightLegs[1].attach(rightBoard, 11, 10, 9);
    rightLegs[2].attach(rightBoard, 13, 14, 15);
    
    leftLegs[0].attach(leftBoard, 8, 9, 10);
    leftLegs[1].attach(leftBoard, 4, 5, 6);
    leftLegs[2].attach(leftBoard, 2, 1, 0);


    rightLegs[0].configureServoDirections(-1, -1, -1, false);
    leftLegs [0].configureServoDirections(-1,  1, 1, false);
    
    rightLegs[1].configureServoDirections(-1, -1,  -1, false);
    leftLegs [1].configureServoDirections(-1,  -1, 1, true);
    
    rightLegs[2].configureServoDirections(-1, -1,  -1, false);
    leftLegs [2].configureServoDirections(-1,  1, 1, true);

    for (int i = 0; i < 3; i++)
    {
        rightLegs[i].configureFemur(0.0, 52.5, 73.0, deg2rad(10)); // смещение по Z относительно нуля, смещение от кокса, длина фемура, 
        leftLegs [i].configureFemur(0.0, 52.5, 73.0, deg2rad(10));

        rightLegs[i].configureTibia(65, deg2rad(-70));
        leftLegs [i].configureTibia(65, deg2rad(-70));
    }
    
    rightLegs[0].configureCoxa( 72.5,  99, deg2rad(      20),  0);   // координата сервака Х, Y относительно центра паука, угол поворота ноги, смещение конца лапы от оси лапы               
    leftLegs [0].configureCoxa(-72.5,  99, deg2rad(180 - 20), 0);

    rightLegs[1].configureCoxa( 72.5,   0, deg2rad(        10),  0);                        
    leftLegs [1].configureCoxa(-72.5,   0, deg2rad(180 - 10), 0);

    rightLegs[2].configureCoxa( 72.5, -99, deg2rad(-         43), 0);                        
    leftLegs [2].configureCoxa(-72.5, -99, deg2rad(- (180 - 43)), 0);

    rightLegs[0].configureDefault(Point( 220, 120, -20), true);
    leftLegs [0].configureDefault(Point(-220, 120, -20), true);
  
    rightLegs[1].configureDefault(Point( 220, 00, -20), true);
    leftLegs [1].configureDefault(Point(-220, 00, -20), true);

    rightLegs[2].configureDefault(Point( 220, -110, -20), true);
    leftLegs [2].configureDefault(Point(-220, -110, -20), true);
    
    // Fine tuning
    rightLegs[0].tuneRestAngles(PI / 2,
                                PI / 2 + deg2rad(-2),
                                PI / 2);
    leftLegs[0].tuneRestAngles(PI / 2,
                                PI / 2 - deg2rad(5),
                                PI / 2);

    leftLegs[1].tuneRestAngles(PI / 2,
                                PI / 2,
                                PI / 2 - deg2rad(10));

    rightLegs[1].tuneRestAngles(PI / 2,
                                PI / 2,
                                PI / 2 + deg2rad(20));


    leftLegs[2].tuneRestAngles(PI / 2 - deg2rad(23),
                                PI / 2,
                                PI / 2);
    
    for (Leg* leg = legs; leg < legs + 6; leg++)
        leg->reachRelativeToDefault(zero);
      //leg->reset();
}

void setup() 
{
    Serial.begin(9600);
  
    Serial.println("Starting");
    
    //for (Leg* leg = legs; leg < legs + 6; leg++)
     //   leg->debug(true);
    

    configureLegs();

 /* stateLinearMovement.assign(0, -1, 0);
  for (int i = 0; i < 50; ++i)
  {
      processState();
      delay(20);
  }

  stateLinearMovement.assign(0, 1, 0);
  for (int i = 0; i < 50; ++i)
  {
      processState();
      delay(10);
  } */
  

  // walk(8, Point(0, 75, 40));
  
  /*walk(4, Point(-90, 0, 50));
  walk(4, Point(0, -130, 50));
  walk(4, Point(90, 0, 50));

  walk(2, Point(60, 60, 50));
  walk(2, Point(-60, 60, 50));
  walk(2, Point(-60, -60, 50));
  walk(2, Point(60, -60, 50));*/


  smoothTo(zero);
} 
 
 
void loop() 
{ 

} 
