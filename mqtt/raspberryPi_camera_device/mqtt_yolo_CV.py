import paho.mqtt.client as mqtt
import cv2
import numpy as np
import matplotlib.pyplot as plt
from picamera import PiCamera
from picamera.array import PiRGBArray
import time

# mqtt 변수 설정 
mqtt_server = "192.168.0.102"
topic = "raspberryPanda/sensor/camera"
msg = "camera test"    
clientID = "Rapsberry Pi Camera"

# PiCamera 변수
frame_width = 640
frame_height = 480
frame_resolution = [frame_width, frame_height]

# 경로 설정(yolo, image)
weights_path = 'pretrained/yolov3.weights'
config_path = 'pretrained/yolov3.cfg'
origImage_path = "data/orig_img.jpg"
saveImage_path = "data/save_img.jpg" 

# yolo 판별 객체들의 index:name sequence
labels_to_names_seq = {0: 'person', 1: 'bicycle', 2: 'car', 3: 'motorbike', 4: 'aeroplane', 5: 'bus', 6: 'train',
						7: 'truck', 8: 'boat', 9: 'traffic light', 10: 'fire hydrant',
						11: 'stop sign', 12: 'parking meter', 13: 'bench', 14: 'bird', 15: 'cat', 16: 'dog',
						17: 'horse', 18: 'sheep', 19: 'cow', 20: 'elephant',
						21: 'bear', 22: 'zebra', 23: 'giraffe', 24: 'backpack', 25: 'umbrella', 26: 'handbag',
						27: 'tie', 28: 'suitcase', 29: 'frisbee', 30: 'skis',
						31: 'snowboard', 32: 'sports ball', 33: 'kite', 34: 'baseball bat', 35: 'baseball glove',
						36: 'skateboard', 37: 'surfboard', 38: 'tennis racket', 39: 'bottle', 40: 'wine glass',
						41: 'cup', 42: 'fork', 43: 'knife', 44: 'spoon', 45: 'bowl', 46: 'banana', 47: 'apple',
						48: 'sandwich', 49: 'orange', 50: 'broccoli',
						51: 'carrot', 52: 'hot dog', 53: 'pizza', 54: 'donut', 55: 'cake', 56: 'chair', 57: 'sofa',
						58: 'pottedplant', 59: 'bed', 60: 'diningtable',
						61: 'toilet', 62: 'tvmonitor', 63: 'laptop', 64: 'mouse', 65: 'remote', 66: 'keyboard',
						67: 'cell phone', 68: 'microwave', 69: 'oven', 70: 'toaster',
						71: 'sink', 72: 'refrigerator', 73: 'book', 74: 'clock', 75: 'vase', 76: 'scissors',
						77: 'teddy bear', 78: 'hair drier', 79: 'toothbrush'}


# draw_plot: 이미지 검출 결과 확인용 함수 
def draw_plot(img, idxs, boxes, class_ids, confidences):
	green_color, red_color = (0, 255, 0), (0, 0, 255)
	draw_img = img.copy()

	if len(idxs) > 0:
		for i in idxs.flatten():
			box = boxes[i]
			left = box[0]
			top = box[1]
			width = box[2]
			height = box[3]
            
			# labels_to_names 딕셔너리로 class_id값을 클래스명으로 변경. opencv에서는 class_id + 1로 매핑해야함.
			caption = "{}: {:.4f}".format(labels_to_names_seq[class_ids[i]], confidences[i])
			
			# cv2.rectangle()은 인자로 들어온 draw_img에 사각형을 그림. 위치 인자는 반드시 정수형.
			cv2.rectangle(draw_img, (int(left), int(top)), (int(left + width), int(top + height)), color=green_color, thickness=2)
			cv2.putText(draw_img, caption, (int(left), int(top - 5)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, red_color, 1)

	img_bgr = draw_img.copy()
	img_rgb = cv2.cvtColor(draw_img, cv2.COLOR_BGR2RGB)

	# 처리한 이미지를 matplotlib로 띄운다. 
	'''
	plt.figure(figsize=(10, 8))
	plt.imshow(img_rgb)
	plt.show()
	'''

	# 처리한 이미지를 opencv로 띄운다. 
	'''
	cv2.imshow('yolo result', img_bgr)
	cv2.waitKey(0)
	'''

	# 처리한 이미지를 저장한다. 
	cv2.imwrite(saveImage_path, img_bgr)


# extract_info: yolo를 적용식킨 결과를 받아, 그 정보를 정리해주는 함수
'''
<검출 결과:return>
- idx: 검출한 객체들의 index 값(len(idx)=검출한 객체 수)
- boxes: 이미지에서 해당 객체의 위치([left, top, width, height]로 구성)
- class_ids: yolo에서 판단한 가장 높은 확률로 인식한 객체의 class id(labels_to_names_seq에서 key(번호)에 해당한다.)
- confidences: yolo가 판단한 해당 객체일 확률(정확도) 
''' 
def extract_info(img, cv_outs):
	rows = img.shape[0]
	cols = img.shape[1]
	
	conf_threshold = 0.5
	
	class_ids = []
	confidences = []
	boxes = []
	
	for ix, output in enumerate(cv_outs):
		for jx, detection in enumerate(output):
			class_scores = detection[5:]
			class_id = np.argmax(class_scores)
			confidence = class_scores[class_id]

			if confidence > conf_threshold:
				center_x = int(detection[0] * cols)
				center_y = int(detection[1] * rows)
				width = int(detection[2] * cols)
				height = int(detection[3] * rows)
				left = int(center_x - width / 2)
				top = int(center_y - height / 2)
				class_ids.append(class_id)
				confidences.append(float(confidence))
				boxes.append([left, top, width, height])

	conf_threshold = 0.5
	nms_threshold = 0.4
	idxs = cv2.dnn.NMSBoxes(boxes, confidences, conf_threshold, nms_threshold)
	
	return idxs, boxes, class_ids, confidences


# [main]
# mqtt 객체 생성 및 설정  
mqttc = mqtt.Client(clientID)
mqttc.connect(mqtt_server, 1883)

# PiCamera 객체 생성 및 원본이미지 저장 
camera = PiCamera()
camera.resolution = frame_resolution
time.sleep(0.1)

# yolo 설정
cv_net_yolo = cv2.dnn.readNetFromDarknet(config_path, weights_path) # 학습된 모델 불러옴
	
layer_names = cv_net_yolo.getLayerNames()
outlayer_names = [layer_names[i[0] - 1] for i in cv_net_yolo.getUnconnectedOutLayers()]

try:
	while mqttc.loop() == 0:
		# 원본 이미지 촬영&저장 + 불러오기 
		camera.capture(origImage_path)
		img = cv2.imread(origImage_path)

		# yolo 이미지 처리 수행
		cv_net_yolo.setInput(cv2.dnn.blobFromImage(img, scalefactor=1 / 255.0, size=(416, 416), swapRB=True, crop=False))
		cv_outs = cv_net_yolo.forward(outlayer_names)
 
		idxs, boxes, class_ids, confidences = extract_info(img, cv_outs)        # yolo를 통해 가져온 정보들을 extra_info로 정리 및 정보가져오기

		# 결과 화면 출력(확인 용도, 실제 구동 시 생략)
		draw_plot(img, idxs, boxes, class_ids, confidences)

		# msg 생성
		if len(idxs) == 0:
			msg = "{ }"
		else:
			msg = "{"
			class_id_set = set(class_ids)
			for id in class_id_set:
				id_num = class_ids.count(id)
				msg = msg + ' ' + labels_to_names_seq[id] + " : " + str(id_num) + ','
			msg = msg[:-1] + "}"
		print(msg)

		# mqtt 결과 전송
		mqttc.publish(topic, msg)
		time.sleep(1)

except InterruptedError:
    camera.close()
finally:
    camera.close()

