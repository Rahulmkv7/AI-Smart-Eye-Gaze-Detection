import cv2
import mediapipe as mp
import time
import requests

ESP_IP = "192.168.1.105"

cap = cv2.VideoCapture(f"http://{ESP_IP}/stream")

mp_face = mp.solutions.face_detection
face = mp_face.FaceDetection()

current = None
start = time.time()
fan = False
light = False

def send(cmd):
    requests.get(f"http://{ESP_IP}/cmd?val={cmd}")

while True:
    ret, frame = cap.read()
    h,w,_ = frame.shape

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = face.process(rgb)

    direction = None

    if result.detections:
        box = result.detections[0].location_data.relative_bounding_box
        cx = int((box.xmin + box.width/2)*w)
        cy = int((box.ymin + box.height/2)*h)

        if cx < w//3: direction="LEFT"
        elif cx > 2*w//3: direction="RIGHT"
        elif cy < h//3: direction="UP"
        elif cy > 2*h//3: direction="DOWN"
        else: direction="CENTER"

    if direction == current and time.time()-start > 3:

        if direction=="LEFT":
            light = not light
            send("LIGHT_ON" if light else "LIGHT_OFF")

        if direction=="RIGHT":
            fan = not fan
            send("FAN_ON" if fan else "FAN_OFF")

        start = time.time()

    elif direction != current:
        current = direction
        start = time.time()

    if direction:
        send(direction)

    cv2.imshow("Tracking", frame)
    if cv2.waitKey(1)==27:
        break