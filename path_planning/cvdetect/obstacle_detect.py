import threading
import cv2
import numpy as np
import math


class DistanceToCamera(object):

    def __init__(self):

        # camera params
        self.alpha = 8.0 * math.pi / 180 # camera angle
        self.u0 = 200
        self.ay = 500

    def calculate(self, v, h, x_shift, image):
        # compute and return the distance from the target point to the camera
        d = h / math.tan(self.alpha + math.atan((v - self.u0) / self.ay))
        if d > 0:
            cv2.putText(image, "%.1fin" % d,
                (image.shape[1] - x_shift, image.shape[0] - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        return d


class ObjectDetection(object):

    def __init__(self):
        self.obstacle_detected = False

    def detect(self, cascade_classifier, gray_image, image):

        # x camera coordinate of the target point 'P'
        v = 0   
        
        # detection
        cascade_obj = cascade_classifier.detectMultiScale(
            gray_image,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30),
        )

        # draw a rectangle around the objects
        for (x_pos, y_pos, width, height) in cascade_obj:
            cv2.rectangle(image, (x_pos+5, y_pos+5), (x_pos+width-5, y_pos+height-5), (255, 255, 255), 2)

            # v = y_pos + height - 5
            v = x_pos + width

            cv2.putText(image, 'Obstacle', (x_pos, y_pos-5), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            
            print('Obstacle detected at ({},{}) of width {}px.'.format(x_pos, y_pos, width))
            
        return v

class VideoStreamHandler(object):

    def __init__(self):
        # Width of Obstacle
        self.binw = 18 # inches

        self.obj_detection = ObjectDetection()

        # cascade classifiers
        self.bin_cascade = cv2.CascadeClassifier('cascade_xml/cascade.xml') #!!!

        self.d_to_camera = DistanceToCamera()
        self.d_bin = 25

        self.start("source/videos/bin_2.wmv")




    # to eventually handle streams but testing on a video
    def start(self, path):

        cap = cv2.VideoCapture(path)

        while cap.isOpened():

            ret, image = cap.read()

            # resize into 320 x 240
            image = cv2.resize(image, (320, 240))

            # grayscale
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

            # object detection
            v_param = self.obj_detection.detect(self.bin_cascade, gray, image)

            # distance measurement
            if v_param > 0:
                d1 = self.d_to_camera.calculate(v_param, self.binw, 300, image)
                self.d_bin = d1

            cv2.imshow('image', image)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                print('Quit')
                break

        cv2.destroyAllWindows()

if __name__ == '__main__':
    VideoStreamHandler()
