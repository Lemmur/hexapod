#ifndef MARK1_CONFIG_H__
#define MARK1_CONFIG_H__

class LegConfiguration
{
public:

    // NOTE: legs are attached after calling this method but are not moved
    static void apply(Adafruit_PWMServoDriver leftBoard, Adafruit_PWMServoDriver rightBoard, Leg* legs, int N)
    {
        Leg* rightLegs = legs;
        Leg* leftLegs = legs + N / 2;

        rightLegs[0].setPins(rightBoard, 8, 9, 10);
        rightLegs[1].setPins(rightBoard, 4, 5, 6);
        rightLegs[2].setPins(rightBoard, 0, 1, 2);

        leftLegs[0].setPins(leftBoard, 7, 6, 5);
        leftLegs[1].setPins(leftBoard, 11, 10, 9);
        leftLegs[2].setPins(leftBoard, 15, 14, 13);

        /*
         * configureServoDirections(float cServoDirection,
         *                          float fServoDirection,
         *                          float tServoDirection,
         *                          bool thirdQuarterFix)
         */


//        rightLegs[0].configureServoDirections(-1, -1, -1, false);
//        leftLegs [0].configureServoDirections(-1,  1, 1, true);
//        
//        rightLegs[1].configureServoDirections(-1, -1,  -1, false);
//        leftLegs [1].configureServoDirections(-1,  1, 1, true);
//        
//        rightLegs[2].configureServoDirections(-1, -1,  -1, false);
//        leftLegs [2].configureServoDirections(-1,  1, 1, true);

        
        rightLegs[0].configureServoDirections(-1, -1, -1, false);
        leftLegs [0].configureServoDirections(-1, 1, 1, true);

        rightLegs[1].configureServoDirections(-1, -1, -1, false);
        leftLegs [1].configureServoDirections(-1, 1, 1, true);

        rightLegs[2].configureServoDirections(-1, -1, -1, false);
        leftLegs [2].configureServoDirections(-1, 1, 1, true);

        for (int i = 0; i < 3; i++)
        {
            /* configureFemur(float fStartZOffset,
             *                float fStartFarOffset,
             *                float fLength,
             *                float fStartAngle)
             */
            rightLegs[i].configureFemur(0, 54, 72, deg2rad(-90)); // OK
            leftLegs [i].configureFemur(0, 54, 72, deg2rad(-90)); // OK

            /* configureTibia(float tLenght,
             *                float tStartAngle)
             */
            rightLegs[i].configureTibia(60, deg2rad(-90)); // OK
            leftLegs [i].configureTibia(60, deg2rad(-90)); // OK
        }

        /*
         * configureCoxa(float cStartX,
         *               float cStartY,
         *               float cStartAngle,
         *               float cFemurOffset)
         */
        rightLegs[0].configureCoxa( 87,  100, deg2rad(         54),  0); // OK
        leftLegs [0].configureCoxa(-87,  100, deg2rad(- (180 + 54)), 0); // OK

        rightLegs[1].configureCoxa( 87,   0, deg2rad(-        0),  0);   // OK
        leftLegs [1].configureCoxa(-87,   0, deg2rad(- (180 - 0)), 0);   // OK

        rightLegs[2].configureCoxa( 87, -100, deg2rad(-        34), 0);  // OK
        leftLegs [2].configureCoxa(-87, -100, deg2rad(- (180 - 34)),0);  // OK

        /*
         * configureDefault(Point def, bool move)
         */
        rightLegs[0].configureDefault(Point( 110, 150, -110), true); // OK
        leftLegs [0].configureDefault(Point(-110, 150, -110), true); // OK

        rightLegs[1].configureDefault(Point( 190, 10, -110), true); // OK
        leftLegs [1].configureDefault(Point(-190, 10, -110), true); // OK

        rightLegs[2].configureDefault(Point( 150, -120, -110), true); // OK
        leftLegs [2].configureDefault(Point(-150, -120, -110), true); // OK

        /*
         * tuneRestAngles(float cServoRestAngle,
         *                float fServoRestAngle,
         *                float tServoRestAngle)
         * +/- deg2rad(10)
         */
        rightLegs[0].tuneRestAngles(PI / 2,
                                    PI / 2 - deg2rad(15),
                                    PI / 2);

        leftLegs[0].tuneRestAngles(PI / 2,
                                   PI / 2,
                                   PI / 2);

        rightLegs[1].tuneRestAngles(PI / 2,
                                    PI / 2 + deg2rad(10),
                                    PI / 2);

        leftLegs[1].tuneRestAngles(PI / 2,
                                   PI / 2 - deg2rad(5),
                                   PI / 2 );

        rightLegs[2].tuneRestAngles(PI / 2,
                                    PI / 2 - deg2rad(5),
                                    PI / 2);

        leftLegs[2].tuneRestAngles(PI / 2,
                                   PI / 2 + deg2rad(5),
                                   PI / 2);

    }  

};

#endif