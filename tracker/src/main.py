from collections import deque
from imutils.video import VideoStream
from enum import Enum
import numpy as np
import argparse
import cv2
import imutils
import time

class TrackerStatus(Enum):
    TRACKING = 1
    LOST = 2
    STOPPED = 3

def openCVSetup() -> [VideoStream, dict]:
    ap = argparse.ArgumentParser()
    ap.add_argument("-v", "--video",
        help="path to the (optional) video file")
    ap.add_argument("-b", "--buffer", type=int, default=64,
        help="max buffer size")
    args = vars(ap.parse_args())

    # if a video path was not supplied, grab the reference
    if not args.get("video", False):
        vs = VideoStream(src=0).start()
    else:
        vs = cv2.VideoCapture(args["video"])

    time.sleep(2.0)

    cv2.namedWindow('Frame', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('Frame', 600, 600)

    return [vs, args]


def getPosition(vs:VideoStream, args:dict) -> [TrackerStatus, [float, float]]:
    # define the lower and upper boundaries of the "green"
    colorLower = (19, 101, 0)
    colorUpper = (176, 213, 255)

    frame = vs.read()
    frame = frame[1] if args.get("video", False) else frame

    if frame is None:
        return [TrackerStatus.STOPPED, None]

    frame = imutils.resize(frame, width=600)
    blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

    mask = cv2.inRange(hsv, colorLower, colorUpper)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)

    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    center = None

    if len(cnts) > 0:
        c = max(cnts, key=cv2.contourArea)
        ((x, y), radius) = cv2.minEnclosingCircle(c)
        M = cv2.moments(c)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

        if radius > 2:
            cv2.circle(frame, (int(x), int(y)), int(radius),
                (0, 255, 255), 2)
            cv2.circle(frame, center, 5, (0, 0, 255), -1)

    cv2.imshow("Frame", frame)
    key = cv2.waitKey(1) & 0xFF

    if key == ord("q"):
        return [TrackerStatus.STOPPED, None]
    elif center is not None:
        return [TrackerStatus.TRACKING, center]
    
    return [TrackerStatus.LOST, None]
    

setup = openCVSetup()
vs = setup[0]
args = setup[1]

while cv2.getWindowProperty('Frame', 0) >= 0:
    
    status, position = getPosition(vs, args)

    if status == TrackerStatus.STOPPED:
        break
    elif status == TrackerStatus.TRACKING:
        print(position)
    elif status == TrackerStatus.LOST:
        print("Lost")

    time.sleep(0.1)

cv2.destroyAllWindows()