from collections import deque
from enum import Enum
import numpy as np
import cv2
import imutils
import time
import serial

serialPort = serial.Serial(port="/dev/ttyUSB0", baudrate=460800)

class TrackerStatus(Enum):
    TRACKING = 1
    LOST = 2
    STOPPED = 3

def open_camera() -> cv2.VideoCapture:
    # Define the GStreamer pipeline
    gst_pipeline = (
        "nvarguscamerasrc ! "
        "video/x-raw(memory:NVMM), "
        "width=(int)1280, height=(int)720, "
        "format=(string)NV12, framerate=(fraction)60/1 ! "
        "nvvidconv flip-method=2 ! "
        "video/x-raw, width=(int)640, height=(int)480, format=(string)BGRx ! "
        "videoconvert ! "
        "video/x-raw, format=(string)BGR ! appsink"
    )

    return cv2.VideoCapture(gst_pipeline, cv2.CAP_GSTREAMER)

def get_position(vs: cv2.VideoCapture) -> [TrackerStatus, [float, float]]:
    colorLower = (0, 0, 225)
    colorUpper = (255, 14, 255)

    ret, frame = vs.read()
    if not ret:
        return [TrackerStatus.STOPPED, None]

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
    # key = cv2.waitKey(1) & 0xFF

    # if key == ord("q"):
    #     return [TrackerStatus.STOPPED, None]
    # elif 

    if center:
        return [TrackerStatus.TRACKING, center]
    return [TrackerStatus.LOST, None]

def main():
    vs = open_camera()

    cv2.namedWindow('Frame', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('Frame', 600, 600)

    time.sleep(2)

    while True:
        status, position = get_position(vs)
        if status == TrackerStatus.STOPPED:
            break
        elif status == TrackerStatus.TRACKING:
            position_str = 'X{:0=4}Y{:0=4}'.format(position[0], position[1])
            print(position_str)
            serialPort.write(position_str.encode())
        elif status == TrackerStatus.LOST:
            print("Lost")
        time.sleep(0.1)

    vs.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
