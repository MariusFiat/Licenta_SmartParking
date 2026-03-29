import cv2
import time
from CameraWrapper import CameraWrapper

def test_roi_split():
    print("Camera v3 init...")
    cap = CameraWrapper()
    
    # Wait one second, let the sensor to fix the image
    time.sleep(1)
    
    ret, frame = cap.read()
    
    if ret:
        H, W, _ = frame.shape
        print(f"Captured resolution: {W}x{H}")
        
        mid = W // 2
        roi_left = frame[:, :mid, :]
        roi_right = frame[:, mid:, :]
        
        # Save the images
        cv2.imwrite("test_full_frame.jpg", frame)
        cv2.imwrite("test_roi_left.jpg", roi_left)
        cv2.imwrite("test_roi_right.jpg", roi_right)
    else:
        print("Error: Unable to read the frame sent by the camera.")

    cap.release()

if __name__ == "__main__":
    test_roi_split()