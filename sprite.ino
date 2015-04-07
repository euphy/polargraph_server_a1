/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Specific features for Polarshield / arduino mega.
Sprite.

Methods that handle drawing and processing vector sprites.

*/
#if MICROCONTROLLER == MC_MEGA

void sprite_drawSprite()
{
  int spriteScale = atoi(inParam1);
  String spriteFilename = inParam2;

  // flip just B axis to orientate for SE drawing  
  int spriteScaleX = spriteScale;
  int spriteScaleY = -spriteScale;
  int rotation = 0;
  
  // and work out rotation value
  if (globalDrawDirection == DIR_SE) // 2
  {
    rotation = 0;
  }
  else if (globalDrawDirection == DIR_NW) // 4
  {
    rotation = 180;
  }
  else if (globalDrawDirection == DIR_SW) //3
  {
    rotation = 90;
  }
  else //(drawDirection == DIR_NE) // default //1
  {
    rotation = 270;
  }
  
  boolean okToDraw = false;
  if (useRoveArea)
  {
    Serial.println("Must use rove area.");
    // if you're using the rove area, then make sure it's in it before drawing
    if (rove_inRoveArea(motorA.currentPosition(), motorB.currentPosition()))
    {
      Serial.println("You're already in the rove area!");
      okToDraw = true;
    }
    else
    {
      Serial.println("You're outside of the rove area!");
      // if drawing text then move to the next line
      if (textRowSize > 1)
      {
        Serial.println("Using text rows.");
        // drawing text
        // so work out the beginning of the next line
        boolean nextLine = rove_moveToBeginningOfNextTextLine();
        Serial.print("Next line calculated:");
        Serial.println(nextLine);
        
        if (nextLine)
          okToDraw = true;
      }
      else // not drawing text
      {
        Serial.println("Not drawing text, AND outside the rove area.");
      }
    }
  }
  else
  {
    Serial.println("Ok to draw - no roving.");
    // otherwise just draw it here
    okToDraw = true;
  }
  
  if (okToDraw)
    sprite_drawSprite(spriteScaleX, spriteScaleY, rotation, spriteFilename);
}

void sprite_drawSprite(int sX, int sY, int rotation, String filename)
{
  Serial.println("Draw sprite.");
  
  
  
  
  // save the old settings
  float oldTranslateX = translateX;
  float oldTranslateY = translateY;
  float oldScaleX = scaleX;
  float oldScaleY = scaleY;
  int oldRotation = rotateTransform;
  
//  Serial.print("1.ScaleX:");
//  Serial.println(scaleX);
//  Serial.print("1.translateX:");
//  Serial.println(translateX);

  // apply the transform parameters
  translateX = motorA.currentPosition();
  translateY = motorB.currentPosition();
  scaleX = sX;
  scaleY = sY;
  rotateTransform = rotation;

//  Serial.print("2.ScaleX:");
//  Serial.println(scaleX);
//  Serial.print("2.translateX:");
//  Serial.println(translateX);
  
  
  
  // open the file
  currentlyDrawingFromFile = true;
  impl_exec_execFromStore(filename);

  
  // undo the transformation
  translateX = oldTranslateX;
  translateY = oldTranslateY;
  scaleX = oldScaleX;
  scaleY = oldScaleY;
  rotateTransform = oldRotation;
}

void sprite_drawRandomPositionedSprite()
{
  Serial.println("This doesn't work yet.");
}

#endif
