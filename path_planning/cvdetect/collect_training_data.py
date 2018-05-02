import numpy as np
import cv2
import time
import os


class CollectTrainingData(object):
    
    def __init__(self):
        # create labels
        self.k = np.zeros((5, 5), 'float')

        # labels: [left, right, sharp left, sharp right, straight]
        for i in range(5):
            self.k[i,i] = 1
        self.temp_label = np.zeros((1, 5), 'float')
        self.collect_image()

    def collect_image(self):
        # source = 720 x 1280 x 3
        videos = ['source/videos/bin_4.wmv'] # ... add more

        saved_frame = 0
        total_frame = 0

        # collect images for training
        print 'Start collecting images...'
        e1 = cv2.getTickCount()

        # 320x240 resolution
        image_array = np.zeros((1, 76800))
        label_array = np.zeros((1, 5), 'float')

        resize_h = (720/6)
        resize_w = (1280/4)

        # stream video frames one by one
        try:
            frame_num = 1
            cap = cv2.VideoCapture(videos[0])
            while cap.isOpened():
                ret, image = cap.read()

                # resize into 320 x 240
                image = cv2.resize(image, (resize_w, resize_h))

                # grayscale
                image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
                
                # save streamed images
                # cv2.imwrite('training_images/frame{:>05}.jpg'.format(frame_num), image)
                cv2.imshow('image', image)
                
                # reshape into one row
                temp_array = image.reshape(1, 76800).astype(np.float32)
                
                frame_num += 1
                total_frame += 1

                # go frame by frame and provide labels
                k = cv2.waitKey(0) & 0xFF
            
                # commands
                if k == ord('Q'):
                    print('quit')
                    break

                elif k == ord('a'):
                    print("Left")
                    image_array = np.vstack((image_array, temp_array))
                    label_array = np.vstack((label_array, self.k[0]))
                    saved_frame += 1

                elif k == ord('d'):
                    print("Right")
                    image_array = np.vstack((image_array, temp_array))
                    label_array = np.vstack((label_array, self.k[1]))
                    saved_frame += 1

                elif k == ord('w'):
                    print("Straight")
                    image_array = np.vstack((image_array, temp_array))
                    label_array = np.vstack((label_array, self.k[4]))
                    saved_frame += 1

                # sharps
                elif k == ord('q'):
                    print("Sharp Left")
                    image_array = np.vstack((image_array, temp_array))
                    label_array = np.vstack((label_array, self.k[2]))
                    saved_frame += 1

                elif k == ord('e'):
                    print("Sharp Right")
                    image_array = np.vstack((image_array, temp_array))
                    label_array = np.vstack((label_array, self.k[3]))
                    saved_frame += 1

                elif k == ord('n'):
                    print('Saved')
                    cv2.imwrite('./source/bg/frame{:>05}.jpg'.format(frame_num), image)

                elif k == ord('s'):
                    print('Skip')
    

            # save training images and labels
            train = image_array[1:, :]
            train_labels = label_array[1:, :]

            # save training data as a numpy file
            file_name = str(int(time.time()))
            directory = "training_data"
            if not os.path.exists(directory):
                os.makedirs(directory)
            try:    
                np.savez(directory + '/' + file_name + '.npz', train=train, train_labels=train_labels)
            except IOError as e:
                print(e)

            e2 = cv2.getTickCount()
            # calculate streaming duration
            time0 = (e2 - e1) / cv2.getTickFrequency()
            print 'Streaming duration:', time0

            print(train.shape)
            print(train_labels.shape)
            print 'Total frame:', total_frame
            print 'Saved frame:', saved_frame
            print 'Dropped frame', total_frame - saved_frame

        finally:
            cap.release()
            cv2.destroyAllWindows()

if __name__ == '__main__':
    CollectTrainingData()
