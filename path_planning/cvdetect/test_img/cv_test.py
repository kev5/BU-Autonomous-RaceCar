import numpy as np
import cv2
from matplotlib import pyplot as plt

img = np.zeros((512,512,3), np.uint8)
# img = cv2.rectangle(img, (384,0), (510,128), (0,255,0), 3)
# img = cv2.circle(img, (447,63), 63, (0,0,255), -1)
pts = np.array([[10,5], [20,30], [70,20], [50,10]], np.int32)
pts = pts.reshape((-1,1,2))
print(pts.shape)
img = cv2.polylines(img, [pts], True, (0,255,255))
font = cv2.FONT_HERSHEY_SIMPLEX
cv2.putText(img, 'OpenCV', (10,500), font, 4, (255,255,255),2, cv2.LINE_AA)

cv2.imshow('frame', img)

cv2.waitKey(0)
cv2.destroyAllWindows()

# cap = cv2.VideoCapture(0)

# # Define the codec and create VideoWriter object
# fourcc = cv2.VideoWriter_fourcc(*'WMV1')
# out = cv2.VideoWriter('output.avi', fourcc, 20.0, (640, 480))

# while (cap.isOpened()):
# 	ret, frame = cap.read()

# 	if ret:
# 		frame = cv2.flip(frame, 0)

# 		out.write(frame)

# 		cv2.imshow('frame', frame)

# 		# resized = cv2.resize(frame, (300,240), fx=0.5, fy=0.5, interpolation = cv2.INTER_LINEAR)
# 		# cv2.imshow('capture', resized)
# 		# wind = cv2.namedWindow(...)

# 		if cv2.waitKey(1) & 0xFF == ord('q'):
# 			break
# 	else: 
# 		break

# cap.release()
# out.release()
# cv2.destroyAllWindows()

