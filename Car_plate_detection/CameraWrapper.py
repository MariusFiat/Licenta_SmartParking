import cv2
from picamera2 import Picamera2
from libcamera import controls

# 1. Definim un "pseudo-cap" care are metoda .read()
class CameraWrapper:
    def __init__(self):
        self.picam2 = Picamera2()
        config = self.picam2.create_preview_configuration(main={"format": 'RGB888', "size": (640, 480)})
        self.picam2.configure(config)
        
        focus_30cm = 5.5
        
        self.picam2.set_controls({
            "AfMode": controls.AfModeEnum.Manual,
            "LensPosition": focus_30cm
        })

        self.picam2.start()

    def read(self):
        # Picamera2 capturează frame-ul
        frame = self.picam2.capture_array()
        # Îl transformăm în BGR pentru OpenCV
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        return True, frame

    def release(self):
        self.picam2.stop()
