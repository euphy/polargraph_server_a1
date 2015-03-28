/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Specific features for Polarshield / arduino mega.
Curve geometry handling.

This file contains code mostly lifted from the AS220 drawbot.
The old links to the AS220 code don't work any more!

This stuff is about drawing curves, circles and spirals.  It contains the
implementation of the spiral pixel.

*/

#if MICROCONTROLLER == MC_MEGA
void curves_pixel_drawCircularPixel() 
{
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);

    int radius = size / 2;

    Serial.print("Density before: ");
    Serial.println(density);
    
    int maxDensity = pixel_maxDensity(penWidth, radius);
    Serial.print("Max density: " );
    Serial.print(maxDensity);
    Serial.print(", penWidth: ");
    Serial.print(penWidth);
    Serial.print(", radius: ");
    Serial.println(radius);
     
    density = pixel_scaleDensity(density, 255, maxDensity);
    Serial.print("Density scaled");
    Serial.println(density);
    
    int increment = radius / maxDensity;

    if (density > 0)
    {
      Serial.print("Density:");
      Serial.println(density);
      curves_drawSpiral(originA, originB, radius, increment, density);
    }
}


/* AS220 Drawbot */
float rads(int n) {
  // Return an angle in radians
  return (n/180.0 * PI);
}    

void curves_drawCurve(long x, long y, long fx, long fy, long cx, long cy) {
  curves_drawCurve(x,y,fx,fy,cx,cy,0);
}

void curves_drawCurve(long x, long y, long fx, long fy, long cx, long cy, int speedOfSegment) {
  // Draw a Quadratic Bezier curve from (x, y) to (fx, fy) using control pt
  // (cx, cy)
  float xt=0;
  float yt=0;

  for (float t=0; t<=1; t+=.01) {
    xt = pow((1-t),2) *x + 2*t*(1-t)*cx+ pow(t,2)*fx;
    yt = pow((1-t),2) *y + 2*t*(1-t)*cy+ pow(t,2)*fy;
    
    if (speedOfSegment != 0)
    {
      motorA.setSpeed(speedOfSegment);
      motorB.setSpeed(speedOfSegment);
      usingAcceleration = false;
    }
    reportingPosition = false;
    changeLength(xt, yt);
    reportingPosition = true;
    usingAcceleration = true;
  }  
}
                                                     

void curves_drawCircle(long centerx, long centery, int radius) {
  // Estimate a circle using 20 arc Bezier curve segments
  int segments =20;
  int angle1 = 0;
  int midpoint=0;
   
//  changeLength(centerx+radius, centery);

  for (float angle2=360/segments; angle2<=360; angle2+=360/segments) {

    midpoint = angle1+(angle2-angle1)/2;

    float startx=centerx+radius*cos(rads(angle1));
    float starty=centery+radius*sin(rads(angle1));
    float endx=centerx+radius*cos(rads(angle2));
    float endy=centery+radius*sin(rads(angle2));
    
    int t1 = rads(angle1)*1000 ;
    int t2 = rads(angle2)*1000;
    int t3 = angle1;
    int t4 = angle2;

    curves_drawCurve(startx,starty,endx,endy,
              centerx+2*(radius*cos(rads(midpoint))-.25*(radius*cos(rads(angle1)))-.25*(radius*cos(rads(angle2)))),
              centery+2*(radius*sin(rads(midpoint))-.25*(radius*sin(rads(angle1)))-.25*(radius*sin(rads(angle2))))
    );
    
    angle1=angle2;
  }
}

void curves_drawSpiral(long centerx, long centery, int maxRadius, int increment, int density) 
{
  Serial.println("Draw spiral.");
  Serial.print("Max radius: ");
  Serial.println(maxRadius);
  Serial.print("Increment:");
  Serial.println(increment);
  Serial.print("density:");
  Serial.println(density);
  // Estimate a circle using 20 arc Bezier curve segments
  int segments = 20;
  int totalSegments = segments * density;

  
  float radius = float(increment);
  // work out how many shells to draw
  int runSpeed = 0;
  for (int turns = 0; turns < density; turns++)
  {
//    if (increment < 80)
//      segments = radius / 4;
//    if (segments < 5)
//      segments = 5;
      

    float anglePerSegment = 360/segments;
    
    float radiusIncrementPerSegment = float(increment) / float(segments);
  
    int angle1 = 0;
    int midpoint=0;
    boolean firstMove = true;
    for (float angle2=anglePerSegment; angle2<=360; angle2+=anglePerSegment) 
    {

      midpoint = angle1+(angle2-angle1)/2;
  
      float startx=centerx+radius*cos(rads(angle1));
      float starty=centery+radius*sin(rads(angle1));
      float endx=centerx+radius*cos(rads(angle2));
      float endy=centery+radius*sin(rads(angle2));
      
      int t1 = rads(angle1)*1000 ;
      int t2 = rads(angle2)*1000;
      int t3 = angle1;
      int t4 = angle2;
  
      if (firstMove)
      {
        changeLength(startx, starty);
        firstMove = false;
      }
      // do the move
      runSpeed = desiredSpeed(totalSegments, runSpeed, currentAcceleration*2);
//      Serial.print("segment: ");
//      Serial.print(totalSegments);
//      Serial.print(", runSpeed: ");
//      Serial.println(runSpeed);
      usingAcceleration = false;
      curves_drawCurve(startx,starty,endx,endy
                ,centerx+2*(radius*cos(rads(midpoint))-.25*(radius*cos(rads(angle1)))-.25*(radius*cos(rads(angle2))))
                ,centery+2*(radius*sin(rads(midpoint))-.25*(radius*sin(rads(angle1)))-.25*(radius*sin(rads(angle2))))
                ,runSpeed
      );
      totalSegments--;
      usingAcceleration = true;
      angle1=angle2;
      radius += radiusIncrementPerSegment;

    }
//    Serial.println("Finished drawing turn.");
  }
  Serial.println("Finished spiral pixel.");
}
#endif


