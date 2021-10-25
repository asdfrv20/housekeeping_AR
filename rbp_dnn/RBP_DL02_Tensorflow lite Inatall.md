# Installing Tensorflow Lite Interpreter

1. Install just the TensorFlow Lite interpreter
 
 pip3 install https://dl.google.com/coral/python/tflite_runtime-2.1.0-cp37-cp37m-linux_armv7l.whl


# Using Google's sample TFLite model

1. Get TF Lite model and labels 

wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.zip
unzip coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.zip -d Sample_TFLite_model
rm coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.zip

2. Get a labels file with corrected indices, delete the other one

wget https://dl.google.com/coral/canned_models/coco_labels.txt
