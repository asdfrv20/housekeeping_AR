# Installing Python 3.7.6 on Raspbian

1. Install the required build-tools (some might already be installed on your system)
    If one of the packages cannot be found, try a newer version number (e.g. libdb5.4-dev instead of libdb5.3-dev)

sudo apt-get update -y
sudo apt-get install build-essential tk-dev libncurses5-dev libncursesw5-dev libreadline6-dev libdb5.3-dev libgdbm-dev libsqlite3-dev libssl-dev libbz2-dev libexpat1-dev liblzma-dev zlib1g-dev libffi-dev -y

2. Download and install Python 3.7.6. 
   When downloading the source code, select the most recent release of Python, available
   
wget https://www.python.org/ftp/python/3.7.6/Python-3.7.6.tar.xz
tar xf Python-3.7.6.tar.xz
cd Python-3.7.6
./configure
make -j 4
sudo make altinstall

3. (Option) Delete the source code

sudo rm -r Python-3.7.4
rm Python-3.7.4.tar.xz

# Installing Tensorflow 2.0 on Raspbian

1. Get whl Image file form a github or build it 

wget https://github.com/lhelontra/tensorflow-on-arm/releases/download/v2.0.0/tensorflow-2.0.0-cp37-none-linux_armv7l.whl
pip3 install tensorflow-2.0.0-cp37-none-linux_armv7l.whl
rm tensorflow-2.0.0-cp37-none-linux_armv7l.whl

2. Test installed version

python3 -c 'import tensorflow as tf;print(tf.__version__)'

# Installing OpenCV 4 on Raspbian
https://www.pyimagesearch.com/2019/09/16/install-opencv-4-on-raspberry-pi-4-and-raspbian-buster/

1. Extand filesystem and enlarge space

sudo raspi-config

select the “7 Advanced Options” menu
selecting “A1 Expand filesystem”:
After rebooting, check all available space

df -h

deleting both Wolfram Engine and LibreOffice to save ~1GB of space

sudo apt-get purge wolfram-engine
sudo apt-get purge libreoffice*
sudo apt-get clean
sudo apt-get autoremove

2. Install dependancies

sudo apt-get update && sudo apt-get upgrade
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libjpeg-dev libtiff5-dev libjasper-dev libpng-dev
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
sudo apt-get install libxvidcore-dev libx264-dev
sudo apt-get install libfontconfig1-dev libcairo2-dev
sudo apt-get install libgdk-pixbuf2.0-dev libpango1.0-dev
sudo apt-get install libgtk2.0-dev libgtk-3-dev
sudo apt-get install libatlas-base-dev gfortran
sudo apt-get install libhdf5-dev libhdf5-serial-dev libhdf5-103
sudo apt-get install libqtgui4 libqtwebkit4 libqt4-test python3-pyqt5
sudo apt-get install python3-dev
sudo apt-get install python-pip
pip install Pillow

3. Create Vitual environment

wget https://bootstrap.pypa.io/get-pip.py
sudo python get-pip.py
sudo python3 get-pip.py
sudo rm -rf ~/.cache/pip

sudo pip install virtualenv virtualenvwrapper

nano ~/.bashrc

export WORKON_HOME=$HOME/.virtualenvs
export VIRTUALENVWRAPPER_PYTHON=/usr/bin/python3
export VIRTUALENVWRAPPER_VIRTUALENV=/usr/local/bin/virtualenv
source /usr/local/bin/virtualenvwrapper.sh
export VIRTUALENVWRAPPER_ENV_BIN_DIR=bin 

source ~/.bashrc

mkvirtualenv cv -p python3
pip install "picamera[array]"

4. pip install openCV
pip install opencv-contrib-python==4.1.0.25

5. Test openCV
python3 -c 'import cv2;print(cv2.__version__)'
