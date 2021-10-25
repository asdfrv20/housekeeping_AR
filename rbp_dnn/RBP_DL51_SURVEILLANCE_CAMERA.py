from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2
import numpy as np
import face_recognition
import pickle
import time
import pygame

import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

import dropbox
import os

dropbox_token = 'VIMxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
dbx = dropbox.Dropbox(dropbox_token)

pygame.mixer.init()
pygame.mixer.music.load('fire-truck.wav')

face_cascade_name = './haarcascades/haarcascade_frontalface_alt.xml'
face_cascade = cv2.CascadeClassifier()
#-- 1. Load the cascades
if not face_cascade.load(cv2.samples.findFile(face_cascade_name)):
    print('--(!)Error loading face cascade')
    exit(0)

encoding_file = 'encodings.pickle'
# load the known faces and embeddings
data = pickle.loads(open(encoding_file, "rb").read())

# Fetch the service account key JSON file contents
cred = credentials.Certificate('Your-certificate.json')
# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://Your-project.firebaseio.com/'
})

unknown_name = 'Unknown'
recognized_name = None
frame_count = 0
frame_interval = 8

frame_width = 640
frame_height = 480
frame_resolution = [frame_width, frame_height]
frame_rate = 16

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = frame_resolution
camera.framerate = frame_rate
rawCapture = PiRGBArray(camera, size=(frame_resolution))
# allow the camera to warmup
time.sleep(0.1)

catured_image = './image/captured_image.jpg'
#catured = open(catured_image,'wb')

# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    start_time = time.time()
    # grab the raw NumPy array representing the image
    image = frame.array
    # store temporary catured image
    camera.capture(catured_image)
    # transform gray image
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    #-- Detect faces
    faces = face_cascade.detectMultiScale(gray)

    rois = [(y, x + w, y + h, x) for (x, y, w, h) in faces]

    encodings = face_recognition.face_encodings(rgb, rois)

    # initialize the list of names for each face detected
    names = []

    # loop over the facial embeddings
    for encoding in encodings:
        # attempt to match each face in the input image to our known
        # encodings
        matches = face_recognition.compare_faces(data["encodings"],
            encoding)
        name = unknown_name

        # check to see if we have found a match
        if True in matches:
            # find the indexes of all matched faces then initialize a
            # dictionary to count the total number of times each face
            # was matched
            matchedIdxs = [i for (i, b) in enumerate(matches) if b]
            counts = {}

            # loop over the matched indexes and maintain a count for
            # each recognized face face
            for i in matchedIdxs:
                name = data["names"][i]
                counts[name] = counts.get(name, 0) + 1

            # determine the recognized face with the largest number of
            # votes (note: in the event of an unlikely tie Python will
            # select first entry in the dictionary)
            name = max(counts, key=counts.get)
        
        # update the list of names
        names.append(name)

    # loop over the recognized faces
    for ((top, right, bottom, left), name) in zip(rois, names):
        # draw the predicted face name on the image
        y = top - 15 if top - 15 > 15 else top + 15
        color = (0, 255, 0)
        line = 2
        if(name == unknown_name):
            color = (0, 0, 255)
            line = 1
            # play SOUND
            if(pygame.mixer.music.get_busy()==False):
                pygame.mixer.music.play()
            
        if(name != recognized_name):
            recognized_name = name
            # Send Notice
            print("Send Notice")
            current = str(time.time())
            path = '/' + current[:10] + name + '.jpg'
            print(path)
            #cv2.imwrite(path, image)
            
            ref = db.reference('surveillance')
            box_ref = ref.child(name)
            box_ref.update({
                'name': name,
                'time': time.time(),
                'path': path
            })
            dbx.files_upload(open(catured_image, "rb").read(), path)
            print(dbx.files_get_metadata(path))
            #os.remove(catured_image)

        cv2.rectangle(image, (left, top), (right, bottom), color, line)
        y = top - 15 if top - 15 > 15 else top + 15
        cv2.putText(image, name, (left, y), cv2.FONT_HERSHEY_SIMPLEX,
            0.75, color, line)
                
    end_time = time.time()
    process_time = end_time - start_time
    print("=== A frame took {:.3f} seconds".format(process_time))
    # show the output image
    cv2.imshow("Recognition", image)

    key = cv2.waitKey(1) & 0xFF
    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)
    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break
