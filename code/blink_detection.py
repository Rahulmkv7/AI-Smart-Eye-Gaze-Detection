import cv2
import mediapipe as mp
import numpy as np
import time

# MediaPipe setup
mp_face = mp.solutions.face_mesh
face_mesh = mp_face.FaceMesh(refine_landmarks=True)

# Eye landmark indices (LEFT EYE)
LEFT_EYE = [33, 160, 158, 133, 153, 144]

# EAR threshold and blink timing
EAR_THRESHOLD = 0.25
LONG_BLINK_TIME = 2   # seconds

blink_start = None
blink_detected = False

cap = cv2.VideoCapture(0)

def eye_aspect_ratio(landmarks, eye_indices, w, h):
    p = []
    for i in eye_indices:
        x = int(landmarks[i].x * w)
        y = int(landmarks[i].y * h)
        p.append((x, y))

    vertical1 = np.linalg.norm(np.array(p[1]) - np.array(p[5]))
    vertical2 = np.linalg.norm(np.array(p[2]) - np.array(p[4]))
    horizontal = np.linalg.norm(np.array(p[0]) - np.array(p[3]))

    ear = (vertical1 + vertical2) / (2.0 * horizontal)
    return ear

while True:
    ret, frame = cap.read()
    if not ret:
        break

    h, w, _ = frame.shape
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = face_mesh.process(rgb)

    if result.multi_face_landmarks:
        landmarks = result.multi_face_landmarks[0].landmark
        ear = eye_aspect_ratio(landmarks, LEFT_EYE, w, h)

        cv2.putText(frame, f"EAR: {ear:.2f}", (30, 50),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        if ear < EAR_THRESHOLD:
            if blink_start is None:
                blink_start = time.time()
        else:
            if blink_start is not None:
                blink_duration = time.time() - blink_start
                blink_start = None

                if blink_duration > LONG_BLINK_TIME:
                    cv2.putText(frame, "LONG BLINK DETECTED", (30, 100),
                                cv2.FONT_HERSHEY_SIMPLEX, 1,
                                (0, 0, 255), 3)
                    print("LONG BLINK")

    cv2.imshow("Blink Detection", frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
