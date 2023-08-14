
#include    <math.h>


#define PORT 

#define PI   3.141592653589793

//  Bits set in the order word.
//
#define oSA     0x0000000A      //  Solution A.
#define oSB     0x0000000B      //  Solution B.

#define oLA     0x0000000A      //  Last pos, solution A was used.
#define oLB     0x0000000B      //            solution B was used.

#define o1st    0x000000F0      //  Current pos, first  solution, 0, A or B.
#define o2nd    0x00000F00      //               second solution, 0, B or A.

#define oSEQ    0x000F0000      //  Sequence.  When, relative to other joints,
                                //  the joint was solved.

//  o   The order word.
//
//  i   The index of the solution: 0 - 1st, 1 - 2nd.
//
//  n   The number indicating sequence in which solution was found: 1 - 7
//
//  s   oSA or oSB
//
#define O_SOLVED(o,i)   ((o & (0xF0 << (i * 4))) != 0)

#define O_AorB(o,i)     (((o & 0xFF0) >> (4 + (i * 4))) & 0xF)

#define O_GET_SEQ(o)    ((o & oSEQ) >> 16)

#define O_SET_SEQ(o,n)  (o = (o & ~oSEQ) | (n << 16))

#define O_GET_S1(o)     ((o & o1st) >> 4)
#define O_SET_S1(o,s)   (o = (o & ~o1st) | (s << 4))

#define O_GET_S2(o)     ((o & o2nd) >> 8)
#define O_SET_S2(o,s)   (o = (o & ~o2nd) | (s << 8))

#define O_LA(o)         ((o & oLA) == oSA)
#define O_LB(o)         ((o & oLB) == oSB)


extern "C"
{
PORT int Test_InvKin ( const double Tgt[], double JCur[], double JNew[] );

PORT int InvKin ( const double Tgt[], double JCur[], 
                                      double JNew[][2], double MaxDelta, 
                                      int     Odr[] );
}


static double   DeltaAngle ( double A1, double A2 )
{
    if ( A1 < 0 )  A1 += 2 * PI;

    if ( A2 < 0 )  A2 += 2 * PI;

    double	D = A1 - A2;

    if ( D >  PI )  D -= 2 * PI;

    if ( D < -PI )  D += 2 * PI;

    return D;
}

PORT int Test_InvKin ( const double Tgt[], double JCur[], double JNew[] )
{
#define	J1Set	bJSet[0]
#define	J2Set	bJSet[1]
#define	J3Set	bJSet[2]
#define	J4Set	bJSet[3]
#define	J5Set	bJSet[4]
#define	J6Set	bJSet[5]
#define	J7Set	bJSet[6]

    //  Arm's configuration parameters.
    //
    const double a3 =   200.000000;
    const double d5 =   200.000000;

    double  J1 = JCur[0];
    double  J2 = JCur[1];
    double  J3 = JCur[2];
    double  J4 = JCur[3];
    double  J5 = JCur[4];
    double  J6 = JCur[5];
    double  J7 = JCur[6];

    //  Where the arm is moving to.
    //
    double  nx = Tgt[ 0];
    double  ox = Tgt[ 1];
    double  ax = Tgt[ 2];
    double  px = Tgt[ 3];
    double  ny = Tgt[ 4];
    double  oy = Tgt[ 5];
    double  ay = Tgt[ 6];
    double  py = Tgt[ 7];
    double  nz = Tgt[ 8];
    double  oz = Tgt[ 9];
    double  az = Tgt[10];
    double  pz = Tgt[11];

    //  These, sines and cosines, are set as each joint is solved.
    //
    double      S1, C1, S2, C2, S3, C3, S4, C4, S5, C5, S6, C6, S7, C7;

    bool    bJSet [7], bError = false;
    int     i, itr, iJ;

    for ( i = 0; i < 7; i++ )  bJSet[i] = false;

    for ( itr = 0; (itr < 2) && ! bError; itr++ )
    {
        iJ = 1;

        //  Joint 2, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  In practice, set to JCur.  But for testing ...
            //
            double  Ja = JNew[iJ];
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J2 = JNew[iJ];    bJSet[iJ] = true;

                S2 = sin ( J2 );
                C2 = cos ( J2 );
            }
        }

        if ( bError )  break;

        iJ = 0;

        //  Joint 1, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = S2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = (-(-C2 * -pz) * (1 / S2));
            double  x = py;
            double  y = px;
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = ((-1 * atan2(y, x)) + atan2(k, rt));
            double  Jb = ((-1 * atan2(y, x)) + atan2(k, (-1 * rt)));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J1 = JNew[iJ];    bJSet[iJ] = true;

                S1 = sin ( J1 );
                C1 = cos ( J1 );
            }
        }

        if ( bError )  break;

        //  Joint 1, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Intermediate terms.
            //
            double  x = py;
            double  y = px;

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            //  In practice, set to JCur.  But for testing ...
            //
            double  Ja = JNew[iJ];
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J1 = JNew[iJ];    bJSet[iJ] = true;

                S1 = sin ( J1 );
                C1 = cos ( J1 );
            }
        }

        if ( bError )  break;

        iJ = 3;

        //  Joint 4, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = C2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k1 = (((C1 * px) + (S1 * py)) * (1 / C2));
            double  k2 = ((-S1 * px) + (C1 * py));
            double  d = d5;
            double  a = a3;
            double  SJ = (-1 * (((((k1 * k1) + (k2 * k2)) + ((-1 * d) * d)) + ((-1 * a) * a)) * (1 / ((2 * d) * a))));
            double  qa = (1 + ((-1 * SJ) * SJ));

            if ( fabs ( qa ) < 0.0000001 )  qa = 0;

            if ( qa < 0 )  break;

            double  CJ = sqrt(qa);

            double  Ja = atan2(SJ, CJ);
            double  Jb = atan2(SJ, (-1 * CJ));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J4 = JNew[iJ];    bJSet[iJ] = true;

                S4 = sin ( J4 );
                C4 = cos ( J4 );
            }
        }

        if ( bError )  break;

        //  Joint 4, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = S2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k1 = (-pz * (1 / S2));
            double  k2 = ((-S1 * px) + (C1 * py));
            double  d = d5;
            double  a = a3;
            double  SJ = (-1 * (((((k1 * k1) + (k2 * k2)) + ((-1 * d) * d)) + ((-1 * a) * a)) * (1 / ((2 * d) * a))));
            double  qa = (1 + ((-1 * SJ) * SJ));

            if ( fabs ( qa ) < 0.0000001 )  qa = 0;

            if ( qa < 0 )  break;

            double  CJ = sqrt(qa);

            double  Ja = atan2(SJ, CJ);
            double  Jb = atan2(SJ, (-1 * CJ));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J4 = JNew[iJ];    bJSet[iJ] = true;

                S4 = sin ( J4 );
                C4 = cos ( J4 );
            }
        }

        if ( bError )  break;

        iJ = 2;

        //  Joint 3, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[3] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = C2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = (((C1 * px) + (S1 * py)) * (1 / C2));
            double  x = (d5 * C4);
            double  y = ((-d5 * S4) + a3);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J3 = JNew[iJ];    bJSet[iJ] = true;

                S3 = sin ( J3 );
                C3 = cos ( J3 );
            }
        }

        if ( bError )  break;

        //  Joint 3, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[3] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = S2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = (-pz * (1 / S2));
            double  x = (d5 * C4);
            double  y = ((-d5 * S4) + a3);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J3 = JNew[iJ];    bJSet[iJ] = true;

                S3 = sin ( J3 );
                C3 = cos ( J3 );
            }
        }

        if ( bError )  break;

        iJ = 4;

        //  Joint 5, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = -((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));
            double  x = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = (-1 * -((S2 * ((C1 * px) + (S1 * py))) + (-C2 * -pz)));
            double  x = (-1 * ((C4 * (((C3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (S3 * ((-S1 * px) + (C1 * py)))) + -a3)) + (S4 * ((-S3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (C3 * ((-S1 * px) + (C1 * py)))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 3 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = (-1 * ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay)))))));
            double  x = -((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 4 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = (-1 * ((C4 * (((C3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (S3 * ((-S1 * px) + (C1 * py)))) + -a3)) + (S4 * ((-S3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (C3 * ((-S1 * px) + (C1 * py)))))));
            double  x = -((S2 * ((C1 * px) + (S1 * py))) + (-C2 * -pz));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 5 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[5] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Requirements
            //
            double  aq, bq;

            aq = -S6;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = ((((C1 * ax) + (S1 * ay)) + -((C2 * -S34) * C6)) * (1 / -S6));
            double  x = S2;
            double  y = (C2 * C34);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 6 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Intermediate terms.
            //
            double  x = S2;
            double  y = (C2 * C34);

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            //  In practice, set to JCur.  But for testing ...
            //
            double  Ja = JNew[iJ];
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 7 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[5] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Requirements
            //
            double  aq, bq;

            aq = -S6;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = ((-az + -((S2 * -S34) * C6)) * (1 / -S6));
            double  x = C2;
            double  y = (S2 * C34);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = ((-1 * atan2(y, x)) + atan2(k, rt));
            double  Jb = ((-1 * atan2(y, x)) + atan2(k, (-1 * rt)));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 8 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Intermediate terms.
            //
            double  x = C2;
            double  y = (S2 * C34);

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            //  In practice, set to JCur.  But for testing ...
            //
            double  Ja = JNew[iJ];
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 9 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[5] )  break;

            //  Intermediate terms.
            //
            double  k = -S6;
            double  x = ((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));
            double  y = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        //  Joint 5, solution 10 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  x = ((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));
            double  y = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            //  In practice, set to JCur.  But for testing ...
            //
            double  Ja = JNew[iJ];
            double  Jb = 0;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J5 = JNew[iJ];    bJSet[iJ] = true;

                S5 = sin ( J5 );
                C5 = cos ( J5 );
            }
        }

        if ( bError )  break;

        iJ = 5;

        //  Joint 6, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));
            double  x = -((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));

            //  Requirements
            //
            double  aq, bq;

            aq = y;
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            aq = x;
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            double  Ja = 0;
            double  Jb = PI;

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J6 = JNew[iJ];    bJSet[iJ] = true;

                S6 = sin ( J6 );
                C6 = cos ( J6 );
            }
        }

        if ( bError )  break;

        //  Joint 6, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[4] )  break;

            //  Intermediate terms.
            //
            double  y = ((-S4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (C4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))));
            double  x = ((C5 * ((C4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (S4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))))) + (S5 * -((S2 * ((C1 * nx) + (S1 * ny))) + (-C2 * -nz))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J6 = JNew[iJ];    bJSet[iJ] = true;

                S6 = sin ( J6 );
                C6 = cos ( J6 );
            }
        }

        if ( bError )  break;

        //  Joint 6, solution 3 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[4] )  break;

            //  Intermediate terms.
            //
            double  y = ((-S4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (C4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))));
            double  x = ((C5 * ((C4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (S4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))))) + (S5 * -((S2 * ((C1 * ox) + (S1 * oy))) + (-C2 * -oz))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J6 = JNew[iJ];    bJSet[iJ] = true;

                S6 = sin ( J6 );
                C6 = cos ( J6 );
            }
        }

        if ( bError )  break;

        iJ = 6;

        //  Joint 7, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = -((-S4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (C4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))));
            double  x = ((-S4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (C4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J7 = JNew[iJ];    bJSet[iJ] = true;

                S7 = sin ( J7 );
                C7 = cos ( J7 );
            }
        }

        if ( bError )  break;

        //  Joint 7, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[4] )  break;

            //  Intermediate terms.
            //
            double  y = -((C5 * ((C4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (S4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))))) + (S5 * -((S2 * ((C1 * ox) + (S1 * oy))) + (-C2 * -oz))));
            double  x = ((C5 * ((C4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (S4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))))) + (S5 * -((S2 * ((C1 * nx) + (S1 * ny))) + (-C2 * -nz))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            if (    (fabs ( DeltaAngle ( JNew[iJ], Ja ) ) > 0.000001)  
                 && (fabs ( DeltaAngle ( JNew[iJ], Jb ) ) > 0.000001) )
            {
                bError = true;    break;
            }
            else
            {
                J7 = JNew[iJ];    bJSet[iJ] = true;

                S7 = sin ( J7 );
                C7 = cos ( J7 );
            }
        }

        if ( bError )  break;

    }

    if ( bError )  return -1;

    return 0;

#undef	J1Set
#undef	J2Set
#undef	J3Set
#undef	J4Set
#undef	J5Set
#undef	J6Set
#undef	J7Set
}


PORT int InvKin ( const double Tgt[], double JCur[], 
                                      double JNew[][2], double MaxDelta, 
                                      int     Odr[] )
{
#define	J1Set	bJSet[0]
#define	J2Set	bJSet[1]
#define	J3Set	bJSet[2]
#define	J4Set	bJSet[3]
#define	J5Set	bJSet[4]
#define	J6Set	bJSet[5]
#define	J7Set	bJSet[6]

    //  Arm's configuration parameters.
    //
    const double a3 =   200.000000;
    const double d5 =   200.000000;

    double  J1 = JCur[0];
    double  J2 = JCur[1];
    double  J3 = JCur[2];
    double  J4 = JCur[3];
    double  J5 = JCur[4];
    double  J6 = JCur[5];
    double  J7 = JCur[6];

    //  Where the arm is moving to.
    //
    double  nx = Tgt[ 0];
    double  ox = Tgt[ 1];
    double  ax = Tgt[ 2];
    double  px = Tgt[ 3];
    double  ny = Tgt[ 4];
    double  oy = Tgt[ 5];
    double  ay = Tgt[ 6];
    double  py = Tgt[ 7];
    double  nz = Tgt[ 8];
    double  oz = Tgt[ 9];
    double  az = Tgt[10];
    double  pz = Tgt[11];

    //  These, sines and cosines, are set as each joint is solved.
    //
    double      S1, C1, S2, C2, S3, C3, S4, C4, S5, C5, S6, C6, S7, C7;

    bool    bJSet [7];
    int     i, itr, iJ;

    int     nS = 0;     //  Number of joints solved.

    for ( i = 0; i < 7; i++ )
    {
        bJSet[i] = O_GET_SEQ(Odr[i]);

        if ( bJSet[i] )  nS++;
    }

    if ( bJSet[0] ) { S1 = sin ( J1 );  C1 = cos ( J1 ); }
    if ( bJSet[1] ) { S2 = sin ( J2 );  C2 = cos ( J2 ); }
    if ( bJSet[2] ) { S3 = sin ( J3 );  C3 = cos ( J3 ); }
    if ( bJSet[3] ) { S4 = sin ( J4 );  C4 = cos ( J4 ); }
    if ( bJSet[4] ) { S5 = sin ( J5 );  C5 = cos ( J5 ); }
    if ( bJSet[5] ) { S6 = sin ( J6 );  C6 = cos ( J6 ); }
    if ( bJSet[6] ) { S7 = sin ( J7 );  C7 = cos ( J7 ); }

    for ( itr = 0; itr < 2; itr++ )
    {
        iJ = 1;

        //  Joint 2, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            double  Ja = J2;

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J2 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S2 = sin ( Ja );
                C2 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        iJ = 0;

        //  Joint 1, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = S2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = (-(-C2 * -pz) * (1 / S2));
            double  x = py;
            double  y = px;
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = ((-1 * atan2(y, x)) + atan2(k, rt));
            double  Jb = ((-1 * atan2(y, x)) + atan2(k, (-1 * rt)));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J1 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S1 = sin ( Jb );
                    C1 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J1 = Ja;

                        S1 = sin ( Ja );
                        C1 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J1 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S1 = sin ( Ja );
                    C1 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J1 = Jb;

                        S1 = sin ( Jb );
                        C1 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 1, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Intermediate terms.
            //
            double  x = py;
            double  y = px;

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            double  Ja = J1;

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J1 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S1 = sin ( Ja );
                C1 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        iJ = 3;

        //  Joint 4, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = C2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k1 = (((C1 * px) + (S1 * py)) * (1 / C2));
            double  k2 = ((-S1 * px) + (C1 * py));
            double  d = d5;
            double  a = a3;
            double  SJ = (-1 * (((((k1 * k1) + (k2 * k2)) + ((-1 * d) * d)) + ((-1 * a) * a)) * (1 / ((2 * d) * a))));
            double  qa = (1 + ((-1 * SJ) * SJ));

            if ( fabs ( qa ) < 0.0000001 )  qa = 0;

            if ( qa < 0 )  break;

            double  CJ = sqrt(qa);

            double  Ja = atan2(SJ, CJ);
            double  Jb = atan2(SJ, (-1 * CJ));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J4 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S4 = sin ( Jb );
                    C4 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J4 = Ja;

                        S4 = sin ( Ja );
                        C4 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J4 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S4 = sin ( Ja );
                    C4 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J4 = Jb;

                        S4 = sin ( Jb );
                        C4 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 4, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = S2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k1 = (-pz * (1 / S2));
            double  k2 = ((-S1 * px) + (C1 * py));
            double  d = d5;
            double  a = a3;
            double  SJ = (-1 * (((((k1 * k1) + (k2 * k2)) + ((-1 * d) * d)) + ((-1 * a) * a)) * (1 / ((2 * d) * a))));
            double  qa = (1 + ((-1 * SJ) * SJ));

            if ( fabs ( qa ) < 0.0000001 )  qa = 0;

            if ( qa < 0 )  break;

            double  CJ = sqrt(qa);

            double  Ja = atan2(SJ, CJ);
            double  Jb = atan2(SJ, (-1 * CJ));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J4 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S4 = sin ( Jb );
                    C4 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J4 = Ja;

                        S4 = sin ( Ja );
                        C4 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J4 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S4 = sin ( Ja );
                    C4 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J4 = Jb;

                        S4 = sin ( Jb );
                        C4 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        iJ = 2;

        //  Joint 3, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[3] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = C2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = (((C1 * px) + (S1 * py)) * (1 / C2));
            double  x = (d5 * C4);
            double  y = ((-d5 * S4) + a3);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J3 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S3 = sin ( Jb );
                    C3 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J3 = Ja;

                        S3 = sin ( Ja );
                        C3 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J3 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S3 = sin ( Ja );
                    C3 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J3 = Jb;

                        S3 = sin ( Jb );
                        C3 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 3, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[3] )  break;

            //  Requirements
            //
            double  aq, bq;

            aq = S2;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = (-pz * (1 / S2));
            double  x = (d5 * C4);
            double  y = ((-d5 * S4) + a3);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J3 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S3 = sin ( Jb );
                    C3 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J3 = Ja;

                        S3 = sin ( Ja );
                        C3 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J3 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S3 = sin ( Ja );
                    C3 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J3 = Jb;

                        S3 = sin ( Jb );
                        C3 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        iJ = 4;

        //  Joint 5, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = -((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));
            double  x = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S5 = sin ( Jb );
                    C5 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J5 = Ja;

                        S5 = sin ( Ja );
                        C5 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S5 = sin ( Ja );
                    C5 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J5 = Jb;

                        S5 = sin ( Jb );
                        C5 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = (-1 * -((S2 * ((C1 * px) + (S1 * py))) + (-C2 * -pz)));
            double  x = (-1 * ((C4 * (((C3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (S3 * ((-S1 * px) + (C1 * py)))) + -a3)) + (S4 * ((-S3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (C3 * ((-S1 * px) + (C1 * py)))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S5 = sin ( Ja );
                C5 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 3 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = (-1 * ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay)))))));
            double  x = -((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S5 = sin ( Ja );
                C5 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 4 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = (-1 * ((C4 * (((C3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (S3 * ((-S1 * px) + (C1 * py)))) + -a3)) + (S4 * ((-S3 * ((C2 * ((C1 * px) + (S1 * py))) + (S2 * -pz))) + (C3 * ((-S1 * px) + (C1 * py)))))));
            double  x = -((S2 * ((C1 * px) + (S1 * py))) + (-C2 * -pz));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S5 = sin ( Ja );
                C5 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 5 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[5] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Requirements
            //
            double  aq, bq;

            aq = -S6;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = ((((C1 * ax) + (S1 * ay)) + -((C2 * -S34) * C6)) * (1 / -S6));
            double  x = S2;
            double  y = (C2 * C34);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S5 = sin ( Jb );
                    C5 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J5 = Ja;

                        S5 = sin ( Ja );
                        C5 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S5 = sin ( Ja );
                    C5 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J5 = Jb;

                        S5 = sin ( Jb );
                        C5 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 6 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Intermediate terms.
            //
            double  x = S2;
            double  y = (C2 * C34);

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            double  Ja = J5;

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S5 = sin ( Ja );
                C5 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 7 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[5] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Requirements
            //
            double  aq, bq;

            aq = -S6;
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            //  Intermediate terms.
            //
            double  k = ((-az + -((S2 * -S34) * C6)) * (1 / -S6));
            double  x = C2;
            double  y = (S2 * C34);
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Other requirements.
            //
            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = ((-1 * atan2(y, x)) + atan2(k, rt));
            double  Jb = ((-1 * atan2(y, x)) + atan2(k, (-1 * rt)));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S5 = sin ( Jb );
                    C5 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J5 = Ja;

                        S5 = sin ( Ja );
                        C5 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S5 = sin ( Ja );
                    C5 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J5 = Jb;

                        S5 = sin ( Jb );
                        C5 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 8 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            double  S34 = sin ( J3 + J4 );
            double  C34 = cos ( J3 + J4 );

            //  Intermediate terms.
            //
            double  x = C2;
            double  y = (S2 * C34);

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            double  Ja = J5;

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S5 = sin ( Ja );
                C5 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 9 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[5] )  break;

            //  Intermediate terms.
            //
            double  k = -S6;
            double  x = ((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));
            double  y = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));
            double  qa1 = ((x * x) + (y * y));

            if ( fabs ( qa1 ) < 0.0000001 )  qa1 = 0;

            if ( qa1 < 0 )  break;

            double  r = sqrt(qa1);
            double  qa2 = ((r * r) + (-1 * (k * k)));

            if ( fabs ( qa2 ) < 0.0000001 )  qa2 = 0;

            if ( qa2 < 0 )  break;

            double  rt = sqrt(qa2);

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = (atan2(y, x) + (-1 * atan2(k, rt)));
            double  Jb = (atan2(y, x) + (-1 * atan2(k, (-1 * rt))));

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S5 = sin ( Jb );
                    C5 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J5 = Ja;

                        S5 = sin ( Ja );
                        C5 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S5 = sin ( Ja );
                    C5 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J5 = Jb;

                        S5 = sin ( Jb );
                        C5 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 5, solution 10 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  x = ((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));
            double  y = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            double  Ja = J5;

            int ns = nS;

            if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
            {
                JNew[iJ][0] = J5 = Ja;    bJSet[iJ] = true;

                ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                S5 = sin ( Ja );
                C5 = cos ( Ja );
            }

            nS = ns;

            break;
        }

        iJ = 5;

        //  Joint 6, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = ((C4 * ((C3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (S3 * ((-S1 * ax) + (C1 * ay))))) + (S4 * ((-S3 * ((C2 * ((C1 * ax) + (S1 * ay))) + (S2 * -az))) + (C3 * ((-S1 * ax) + (C1 * ay))))));
            double  x = -((S2 * ((C1 * ax) + (S1 * ay))) + (-C2 * -az));

            //  Requirements
            //
            double  aq, bq;

            aq = y;
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            aq = x;
            bq = 0;
            if ( fabs ( aq - bq ) >= 0.000001 )  break;

            double  Ja = 0;
            double  Jb = PI;

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J6 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S6 = sin ( Jb );
                    C6 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J6 = Ja;

                        S6 = sin ( Ja );
                        C6 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J6 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S6 = sin ( Ja );
                    C6 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J6 = Jb;

                        S6 = sin ( Jb );
                        C6 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 6, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[4] )  break;

            //  Intermediate terms.
            //
            double  y = ((-S4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (C4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))));
            double  x = ((C5 * ((C4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (S4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))))) + (S5 * -((S2 * ((C1 * nx) + (S1 * ny))) + (-C2 * -nz))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J6 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S6 = sin ( Jb );
                    C6 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J6 = Ja;

                        S6 = sin ( Ja );
                        C6 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J6 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S6 = sin ( Ja );
                    C6 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J6 = Jb;

                        S6 = sin ( Jb );
                        C6 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 6, solution 3 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[4] )  break;

            //  Intermediate terms.
            //
            double  y = ((-S4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (C4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))));
            double  x = ((C5 * ((C4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (S4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))))) + (S5 * -((S2 * ((C1 * ox) + (S1 * oy))) + (-C2 * -oz))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J6 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S6 = sin ( Jb );
                    C6 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J6 = Ja;

                        S6 = sin ( Ja );
                        C6 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J6 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S6 = sin ( Ja );
                    C6 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J6 = Jb;

                        S6 = sin ( Jb );
                        C6 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        iJ = 6;

        //  Joint 7, solution 1 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;

            //  Intermediate terms.
            //
            double  y = -((-S4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (C4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))));
            double  x = ((-S4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (C4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J7 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S7 = sin ( Jb );
                    C7 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J7 = Ja;

                        S7 = sin ( Ja );
                        C7 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J7 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S7 = sin ( Ja );
                    C7 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J7 = Jb;

                        S7 = sin ( Jb );
                        C7 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }

        //  Joint 7, solution 2 -
        //
        while ( ! bJSet[iJ] )
        {
            //  Other joints set?
            //
            if ( ! bJSet[0] )  break;
            if ( ! bJSet[1] )  break;
            if ( ! bJSet[2] )  break;
            if ( ! bJSet[3] )  break;
            if ( ! bJSet[4] )  break;

            //  Intermediate terms.
            //
            double  y = -((C5 * ((C4 * ((C3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (S3 * ((-S1 * ox) + (C1 * oy))))) + (S4 * ((-S3 * ((C2 * ((C1 * ox) + (S1 * oy))) + (S2 * -oz))) + (C3 * ((-S1 * ox) + (C1 * oy))))))) + (S5 * -((S2 * ((C1 * ox) + (S1 * oy))) + (-C2 * -oz))));
            double  x = ((C5 * ((C4 * ((C3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (S3 * ((-S1 * nx) + (C1 * ny))))) + (S4 * ((-S3 * ((C2 * ((C1 * nx) + (S1 * ny))) + (S2 * -nz))) + (C3 * ((-S1 * nx) + (C1 * ny))))))) + (S5 * -((S2 * ((C1 * nx) + (S1 * ny))) + (-C2 * -nz))));

            //  Requirements
            //
            double  aq, bq;

            aq = (fabs(x) + fabs(y));
            bq = 0;
            if ( fabs ( aq - bq ) < 0.000001 )  break;

            double  Ja = atan2(y, x);
            double  Jb = (atan2(y, x) + PI);

            int ns = nS;

            if ( O_LB(Odr[iJ]) )
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J7 = Jb;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSB);

                    S7 = sin ( Jb );
                    C7 = cos ( Jb );
                }

                if (    (fabs ( Ja - Jb ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Ja;    O_SET_S1(Odr[iJ],oSA);

                        ++ns;

                        J7 = Ja;

                        S7 = sin ( Ja );
                        C7 = cos ( Ja );
                    }
                    else
                    {   JNew[iJ][1] = Ja;    O_SET_S2(Odr[iJ],oSA); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }
            else
            {
                if ( fabs ( DeltaAngle ( JCur[iJ], Ja ) ) <= MaxDelta )
                {
                    JNew[iJ][0] = J7 = Ja;    bJSet[iJ] = true;

                    ++ns;    O_SET_SEQ(Odr[iJ],ns);   O_SET_S1(Odr[iJ],oSA);

                    S7 = sin ( Ja );
                    C7 = cos ( Ja );
                }

                if (    (fabs ( Jb - Ja ) > 0.000001) 
                     && 
                        (fabs ( DeltaAngle ( JCur[iJ], Jb ) ) <= MaxDelta) )
                {
                    if ( ! O_SOLVED(Odr[iJ],0) )
                    {
                        JNew[iJ][0] = Jb;    O_SET_S1(Odr[iJ],oSB);

                        ++ns;

                        J7 = Jb;

                        S7 = sin ( Jb );
                        C7 = cos ( Jb );
                    }
                    else
                    {   JNew[iJ][1] = Jb;    O_SET_S2(Odr[iJ],oSB); }

                    bJSet[iJ] = true;

                    O_SET_SEQ(Odr[iJ],ns);
                }
            }

            nS = ns;

            break;
        }
    }

    for ( i = 0; i < 7; i++ )  if ( ! bJSet[i] )  break;

    if ( i < 7 )  return -1;

    return 0;

#undef	J1Set
#undef	J2Set
#undef	J3Set
#undef	J4Set
#undef	J5Set
#undef	J6Set
#undef	J7Set
}


