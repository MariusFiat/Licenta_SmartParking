import numpy as np
from picamera2 import Picamera2
import cv2

class CameraWrapper:
    def __init__(self):
        self.picam2 = Picamera2()
        config = self.picam2.create_preview_configuration(main={
            "format": "RGB888", 
            "size": (2304, 1296) #Just half of the max FOV
        })
        
        self.picam2.configure(config)

        #Focus ~15 cm 
        self.picam2.set_controls({
            "LensPosition": 6.67, 
            "AfMode": 0
        })

        self.picam2.start()
        print("[CameraWrapper] Full FOV Mode activated (2304x1296)")

    def read(self):
        try:
            # Capturează frame-ul
            frame = self.picam2.capture_array()
            
            if frame is not None:
                # Picamera2 images are RGB, covert to BGR
                frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
                return True, frame_bgr
            else:
                return False, None
        except Exception as e:
            print(f"Capture error: {e}")
            return False, None

    def release(self):
        try:
            self.picam2.stop()
            self.picam2.close()
        except:
            pass

    def __del__(self):
        self.release()