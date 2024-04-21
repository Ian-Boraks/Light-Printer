from collections import deque
from enum import Enum
import numpy as np
import cv2
import imutils
import time
import serial
from picamera2 import Picamera2, Preview

serialPort = serial.Serial(port="/dev/ttyUSB0", baudrate=460800)

class TrackerStatus(Enum):
    TRACKING = 1
    LOST = 2
    STOPPED = 3

def open_camera() -> Picamera2:
    camera = Picamera2()
    camera_config = camera.create_preview_configuration(main={"format": 'XRGB8888', "size": (640, 480)})
    camera.configure(camera_config)
    camera.start()
    return camera

def get_frame(camera: Picamera2) -> np.ndarray:
    # Get the frame from the camera
    frame = camera.capture_array()
    return frame

def get_position(frame: np.ndarray) -> [TrackerStatus, [float, float]]:
    colorLower = (0, 0, 225)
    colorUpper = (255, 14, 255)

    if frame is None:
        print("Failed to read frame")
        time.sleep(.5)
        return [TrackerStatus.LOST, None]

    frame = imutils.resize(frame, width=600)
    blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

    mask = cv2.inRange(hsv, colorLower, colorUpper)
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    center = None

    if len(cnts) > 0:
        c = max(cnts, key=cv2.contourArea)
        ((x, y), radius) = cv2.minEnclosingCircle(c)
        M = cv2.moments(c)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"])) if M["m00"] else None

        if radius > 2:
            cv2.circle(frame, (int(x), int(y)), int(radius),
                       (0, 255, 255), 2)
            cv2.circle(frame, center, 5, (0, 0, 255), -1)

    cv2.imshow("Frame", mask)

    if center:
        return [TrackerStatus.TRACKING, center]
    return [TrackerStatus.LOST, None]

def main():
    camera = open_camera()
    camera.start_preview(Preview.QTGL)

    cv2.namedWindow('Frame', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('Frame', 600, 600)
    
    time.sleep(1)

    while True:
        frame = get_frame(camera)
        status, position = get_position(frame)
        if status == TrackerStatus.STOPPED:
            break
        elif status == TrackerStatus.TRACKING:
            position_str = 'X{:0=4}Y{:0=4}'.format(position[0], position[1])
            print(position_str)
            serialPort.write(position_str.encode())
        elif status == TrackerStatus.LOST:
            print("Lost")
        time.sleep(0.1)

    camera.stop()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
