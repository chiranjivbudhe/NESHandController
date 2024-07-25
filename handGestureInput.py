import cv2
import mediapipe as mp
import serial

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_hands = mp.solutions.hands

# button bit order
# in LSB format

aBitPosition      : int = 0
bBitPosition      : int = 1
selectBitPosition : int = 2
startBitPosition  : int = 3
upBitPosition     : int = 4
downBitPosition   : int = 5
leftBitPosition   : int = 6
rightBitPosition  : int = 7

# for serial communcation from pythong to arduino, comport depends on the port the arduino is connect to, for me it is com3, can check in device manager
comPort = 'COM3'
baudRate = 115200 # lowers baudrate causing other leds to flicker or it doesn't work
timeOut = .1
arduino = serial.Serial(port=comPort, baudrate=baudRate, timeout=timeOut)

transmissionByte : int = 255 # defaul it all active

def HandleInput(key, hand_landmarks,tipLandmark, dipLandmark, bitPosition: int, currentByte: int):
  # if (hand_landmarks.landmark[tipLandmark].y < hand_landmarks.landmark[dipLandmark].y):
  if (tipLandmark < dipLandmark):
      # print("pressed.",key )
      return ((~(1 << bitPosition)) & currentByte) # 0 when pressed
  else:
      return 1 << bitPosition | currentByte # 1 when released
  
  # print("released.",key )


# For webcam input:
cap = cv2.VideoCapture(0)

with mp_hands.Hands(
    model_complexity=0,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as hands:
  while cap.isOpened():
    success, image = cap.read()
       
    transmissionByte = 255
    
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      continue

    # Read an image, flip it around y-axis for correct handedness output.
    #if flip: # selfie view
    #image = cv2.flip(image, 1)

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image.flags.writeable = False
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = hands.process(image)
    #print('Handedness:', results.multi_handedness)
    # Draw the hand annotations on the image.
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    if results.multi_hand_landmarks:
      for hand_landmarks in results.multi_hand_landmarks:
        mp_drawing.draw_landmarks(
            image,
            hand_landmarks,
            mp_hands.HAND_CONNECTIONS,
            mp_drawing_styles.get_default_hand_landmarks_style(),
            mp_drawing_styles.get_default_hand_connections_style())   
         
      transmissionByte = HandleInput(key='A', hand_landmarks=hand_landmarks, tipLandmark= hand_landmarks.landmark[mp_hands.HandLandmark.PINKY_TIP].y, dipLandmark= hand_landmarks.landmark[mp_hands.HandLandmark.PINKY_DIP].y, bitPosition= aBitPosition, currentByte= transmissionByte)
      transmissionByte = HandleInput(key='L', hand_landmarks=hand_landmarks, tipLandmark= hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_IP].x, dipLandmark= hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_TIP].x, bitPosition= leftBitPosition, currentByte= transmissionByte)
      transmissionByte = HandleInput(key='R', hand_landmarks=hand_landmarks, tipLandmark= hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].y, dipLandmark= hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_DIP].y, bitPosition= rightBitPosition, currentByte= transmissionByte)
    else:
        print('No input')
     
  
    print('final byte value', transmissionByte)
    arduino.write(transmissionByte.to_bytes(1,byteorder="little"))

    # Flip the image horizontally for a selfie-view display.
    image = cv2.resize(image,(1048,936))
    show = cv2.flip(image, 1)
    cv2.imshow('MediaPipe Hands', show)

    do_stop = False
    while True:
        key = cv2.waitKey(5)
        if key == -1:
          break
        if cv2.waitKey(5) & 0xFF == 27:  
           do_stop = True  

    if do_stop:
        break
           
    # if cv2.waitKey(5) & 0xFF == 27:
    #   break

cap.release()
