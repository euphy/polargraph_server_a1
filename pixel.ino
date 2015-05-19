/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Pixel.

This is one of the core files for the polargraph server program.  

This is a biggie, and has the routines necessary for generating and drawing
the squarewave and scribble pixel styles.

*/
#ifdef PIXEL_DRAWING
void pixel_changeDrawingDirection() 
{
  globalDrawDirectionMode = atoi(inParam1);
  globalDrawDirection = atoi(inParam2);
//  Serial.print(F("Changed draw direction mode to be "));
//  Serial.print(globalDrawDirectionMode);
//  Serial.print(F(" and direction is "));
//  Serial.println(globalDrawDirection);
}

void pixel_drawSquarePixel() 
{
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);
    
    /*  Here density is accepted as a recording of BRIGHTNESS, where 0 is black and 255 is white.
        Later on, density gets scaled to the range that is available for this particular 
        pixel+pentip combination, and also inverted so that it becomes a recording of DARKNESS,
        where 0 is white and the higher values are darker.
        
        (Using the same variable to save on space, really.)
        
        This is because paper is white, and ink is black, and this density value is used to 
        control how many waves are drawn. 
        
        O waves means no ink, so a very light pixel.
        50 waves means lots of ink, so a much darker pixel.
    */

    int halfSize = size / 2;
    
    long startPointA;
    long startPointB;
    long endPointA;
    long endPointB;

    int calcFullSize = halfSize * 2; // see if there's any rounding errors
    int offsetStart = size - calcFullSize;
    
    if (globalDrawDirectionMode == DIR_MODE_AUTO)
      globalDrawDirection = pixel_getAutoDrawDirection(originA, originB, motorA.currentPosition(), motorB.currentPosition());
      

    if (globalDrawDirection == DIR_SE) 
    {
//      Serial.println(F("d: SE"));
      startPointA = originA - halfSize;
      startPointA += offsetStart;
      startPointB = originB;
      endPointA = originA + halfSize;
      endPointB = originB;
    }
    else if (globalDrawDirection == DIR_SW)
    {
//      Serial.println(F("d: SW"));
      startPointA = originA;
      startPointB = originB - halfSize;
      startPointB += offsetStart;
      endPointA = originA;
      endPointB = originB + halfSize;
    }
    else if (globalDrawDirection == DIR_NW)
    {
//      Serial.println(F("d: NW"));
      startPointA = originA + halfSize;
      startPointA -= offsetStart;
      startPointB = originB;
      endPointA = originA - halfSize;
      endPointB = originB;
    }
    else //(drawDirection == DIR_NE)
    {
//      Serial.println(F("d: NE"));
      startPointA = originA;
      startPointB = originB + halfSize;
      startPointB -= offsetStart;
      endPointA = originA;
      endPointB = originB - halfSize;
    }

    /* pixel_scaleDensity takes it's input value as a BRIGHTNESS value (ie 255 = white),
       but returns a DARKNESS value (ie 0 = white). 
       Here I'm using the same variable to hold both, save space in memory. */
    density = pixel_scaleDensity(density, 255, pixel_maxDensity(penWidth, size));
//    Serial.print(F("Start point: "));
//    Serial.print(startPointA);
//    Serial.print(COMMA);
//    Serial.print(startPointB);
//    Serial.print(F(". end point: "));
//    Serial.print(endPointA);
//    Serial.print(COMMA);
//    Serial.print(endPointB);
//    Serial.println(F("."));
    
    changeLength(startPointA, startPointB);
    if (density > 1)
    {
      pixel_drawSquarePixel(size, size, density, globalDrawDirection);
    }
    changeLength(endPointA, endPointB);
    
    //outputAvailableMemory(); 
}

byte pixel_getRandomDrawDirection()
{
  return random(1, 5);
}

byte pixel_getAutoDrawDirection(long targetA, long targetB, long sourceA, long sourceB)
{
  byte dir = DIR_SE;
  
  if (targetA<sourceA && targetB<sourceA)
    dir = DIR_NW;
  else if (targetA>sourceA && targetB>sourceB)
    dir = DIR_SE;
  else if (targetA<sourceA && targetB>sourceB)
    dir = DIR_SW;
  else if (targetA>sourceA && targetB<sourceB)
    dir = DIR_NE;
  else if (targetA==sourceA && targetB<sourceB)
    dir = DIR_NE;
  else if (targetA==sourceA && targetB>sourceB)
    dir = DIR_SW;
  else if (targetA<sourceA && targetB==sourceB)
    dir = DIR_NW;
  else if (targetA>sourceA && targetB==sourceB)
    dir = DIR_SE;
  else
  {
//    Serial.println("Not calculated - default SE");
  }

  return dir;
}

void pixel_drawScribblePixel() 
{
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);
    
    int maxDens = pixel_maxDensity(penWidth, size);

    density = pixel_scaleDensity(density, 255, maxDens);
    pixel_drawScribblePixel(originA, originB, size*1.1, density);
    
//    outputAvailableMemory(); 
}

void pixel_drawScribblePixel(long originA, long originB, int size, int density) 
{

//  int originA = motorA.currentPosition();
//  int originB = motorB.currentPosition();
  
  long lowLimitA = originA-(size/2);
  long highLimitA = lowLimitA+size;
  long lowLimitB = originB-(size/2);
//  long highLimitB = lowLimitB+size;
  int randA;
  int randB;
  
  int inc = 0;
  int currSize = size;
  
  for (int i = 0; i <= density; i++)
  {
    randA = random(0, currSize);
    randB = random(0, currSize);
    changeLength(lowLimitA+randA, lowLimitB+randB);
    
    lowLimitA-=inc;
    highLimitA+=inc;
    currSize+=inc*2;
  }
}

int pixel_minSegmentSizeForPen(float penSize)
{
  float penSizeInSteps = penSize * stepsPerMM;

  int minSegSize = 1;
  if (penSizeInSteps >= 2.0)
    minSegSize = int(penSizeInSteps);
    
//  Serial.print(F("Min segment size for penSize "));
//  Serial.print(penSize);
//  Serial.print(F(": "));
//  Serial.print(minSegSize);
//  Serial.print(F(" steps."));
//  Serial.println();
  
  return minSegSize;
}

int pixel_maxDensity(float penSize, int rowSize)
{
  float rowSizeInMM = mmPerStep * rowSize;
#ifdef DEBUG_PIXEL
  Serial.print(F("MSG,D,rowsize in mm: "));
  Serial.print(rowSizeInMM);
  Serial.print(F(", mmPerStep: "));
  Serial.print(mmPerStep);
  Serial.print(F(", rowsize: "));
  Serial.println(rowSize);
#endif

  float numberOfSegments = rowSizeInMM / penSize;
  int maxDens = 1;
  if (numberOfSegments >= 2.0)
    maxDens = int(numberOfSegments);
    
  if (maxDens <= 2)
  {
    Serial.print("num of segments float:");
    Serial.println(numberOfSegments);
    Serial.print(F("MSG,I,Max density for penSize: "));
    Serial.print(penSize);
    Serial.print(F(", rowSize: "));
    Serial.print(rowSize);
    Serial.print(F(" is "));
    Serial.println(maxDens);
    Serial.println(F("MSG,I,You probably won't get any detail in this."));
  }
    

//
//    
  
  return maxDens;
}

int pixel_scaleDensity(int inDens, int inMax, int outMax)
{
  float reducedDens = (float(inDens) / float(inMax)) * float(outMax);
  reducedDens = outMax-reducedDens;
//  Serial.print(F("inDens:"));
//  Serial.print(inDens);
//  Serial.print(F(", inMax:"));
//  Serial.print(inMax);
//  Serial.print(F(", outMax:"));
//  Serial.print(outMax);
//  Serial.print(F(", reduced:"));
//  Serial.println(reducedDens);
  
  // round up if bigger than .5
  int result = int(reducedDens);
  if (reducedDens - (result) > 0.5)
    result ++;
  
  return result;
}

void pixel_drawSquarePixel(int length, int width, int density, byte drawDirection) 
{
  // work out how wide each segment should be
  int segmentLength = 0;

  if (density > 0)
  {
    // work out some segment widths
    int basicSegLength = length / density;
    int basicSegRemainder = length % density;
    float remainderPerSegment = float(basicSegRemainder) / float(density);
    float totalRemainder = 0.0;
    int lengthSoFar = 0;
    
//    Serial.print("Basic seg length:");
//    Serial.print(basicSegLength);
//    Serial.print(", basic seg remainder:");
//    Serial.print(basicSegRemainder);
//    Serial.print(", remainder per seg");
//    Serial.println(remainderPerSegment);
    
    for (int i = 0; i <= density; i++) 
    {
      totalRemainder += remainderPerSegment;

      if (totalRemainder >= 1.0)
      {
        totalRemainder -= 1.0;
        segmentLength = basicSegLength+1;
      }
      else
      {
        segmentLength = basicSegLength;
      }

      if (drawDirection == DIR_SE) {
        pixel_drawSquareWaveAlongAxis(motorA, motorB, width, segmentLength, density, i);
      }
      else if (drawDirection == DIR_SW) {
        pixel_drawSquareWaveAlongAxis(motorB, motorA, width, segmentLength, density, i);
      }
      else if (drawDirection == DIR_NW) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawSquareWaveAlongAxis(motorA, motorB, width, segmentLength, density, i);
      }
      else if (drawDirection == DIR_NE) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawSquareWaveAlongAxis(motorB, motorA, width, segmentLength, density, i);
      }
      lengthSoFar += segmentLength;
    //      Serial.print("distance so far:");
    //      Serial.print(distanceSoFar);
      
      
      reportPosition();
    } // end of loop
  }
}

void pixel_drawSquareWaveAlongAxis(AccelStepper &longAxis, AccelStepper &wideAxis, int waveAmplitude, int waveLength, int totalWaves, int waveNo)
{
  if (waveNo == 0) 
  { 
    // first one, half a line and an along
    Serial.println(F("First wave half"));
    if (lastWaveWasTop) {
      moveAxis(wideAxis, waveAmplitude/2);
      moveAxis(longAxis, waveLength);
    }
    else {
      moveAxis(wideAxis, 0-(waveAmplitude/2));
      moveAxis(longAxis, waveLength);
    }
    pixel_flipWaveDirection();
  }
  else if (waveNo == totalWaves) 
  { 
    // last one, half a line with no along
    if (lastWaveWasTop) {
      moveAxis(wideAxis, waveAmplitude/2);
    }
    else {
      moveAxis(wideAxis, 0-(waveAmplitude/2));
    }
  }
  else 
  { 
    // intervening lines - full lines, and an along
    if (lastWaveWasTop) {
      moveAxis(wideAxis, waveAmplitude);
      moveAxis(longAxis, waveLength);
    }
    else {
      moveAxis(wideAxis, 0-waveAmplitude);
      moveAxis(longAxis, waveLength);
    }
    pixel_flipWaveDirection();
  }
}

void pixel_flipWaveDirection()
{
  if (lastWaveWasTop)
    lastWaveWasTop = false;
  else
    lastWaveWasTop = true;
}

  void pixel_testPenWidth()
  {
    int rowWidth = multiplier(atoi(inParam1));
    float startWidth = atof(inParam2);
    float endWidth = atof(inParam3); 
    float incSize = atof(inParam4);

    int tempDirectionMode = globalDrawDirectionMode;
    globalDrawDirectionMode = DIR_MODE_PRESET;
    
    float oldPenWidth = penWidth;
    int iterations = 0;
    
    for (float pw = startWidth; pw <= endWidth; pw+=incSize)
    {
      iterations++;
      penWidth = pw;
      int maxDens = pixel_maxDensity(penWidth, rowWidth);
//      Serial.print(F("Penwidth test "));
//      Serial.print(iterations);
//      Serial.print(F(", pen width: "));
//      Serial.print(penWidth);
//      Serial.print(F(", max density: "));
//      Serial.println(maxDens);
      pixel_drawSquarePixel(rowWidth, rowWidth, maxDens, DIR_SE);
    }

    penWidth = oldPenWidth;
    
    moveAxis(motorB, 0-rowWidth);
    for (int i = 1; i <= iterations; i++)
    {
      moveAxis(motorB, 0-(rowWidth/2));
      moveAxis(motorA, 0-rowWidth);
      moveAxis(motorB, rowWidth/2);
    }
    
    penWidth = oldPenWidth;
    globalDrawDirectionMode = tempDirectionMode;
  }
#endif

