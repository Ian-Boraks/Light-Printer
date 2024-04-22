from collections import deque
from enum import Enum
import numpy as np
import cv2
import imutils
import time
import serial
from picamera2 import Picamera2, Preview

# Set up a serial port connection to communicate with external devices.
serialPort = serial.Serial(port="/dev/ttyUSB0", baudrate=460800)


class TrackerStatus(Enum):
    """Enumeration for tracking status."""

    TRACKING = 1  # Object is currently being tracked.
    LOST = 2  # Object has been lost from tracking.
    STOPPED = 3  # Tracking has been manually stopped.


def open_camera() -> Picamera2:
    """
    Initialize the Raspberry Pi Camera and start capturing.

    Returns:
        Picamera2: The initialized and configured camera object.
    """
    camera = Picamera2()
    camera_config = camera.create_preview_configuration(
        main={"format": "XRGB8888", "size": (640, 480)}
    )
    camera.configure(camera_config)
    camera.start()
    return camera


def get_frame(camera: Picamera2) -> np.ndarray:
    """
    Capture a single frame from the Picamera2.

    Args:
        camera (Picamera2): The camera from which to capture the frame.

    Returns:
        np.ndarray: The captured image frame as a numpy array.
    """
    frame = camera.capture_array()
    return frame


def get_position(frame: np.ndarray) -> [TrackerStatus, [float, float]]:
    """
    Process the captured frame to find and track an object based on its color.

    Args:
        frame (np.ndarray): The image frame to process for object tracking.

    Returns:
        A tuple with the tracking status and the object's central coordinates (x, y).
    """
    colorLower = (0, 0, 82)
    colorUpper = (255, 255, 255)

    if frame is None:
        time.sleep(0.5)
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
        center = (
            (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"])) if M["m00"] else None
        )

    if center:
        return [TrackerStatus.TRACKING, center]
    return [TrackerStatus.LOST, None]


def main():
    """
    Main function that initializes the camera, starts the tracking process,
    and communicates the tracked position over a serial connection.
    """
    camera = open_camera()

    print("Starting the tracking process...")

    while True:
        frame = get_frame(camera)
        status, position = get_position(frame)
        if status == TrackerStatus.STOPPED:
            break
        elif status == TrackerStatus.TRACKING:
            position_str = "X{:0=4}Y{:0=4}".format(position[0], position[1])
            print(position_str)
            serialPort.write(position_str.encode())
        elif status == TrackerStatus.LOST:
            pass

    camera.stop()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    print("Starting the tracker...")
    main()
