#instalador de QtRatWalk

sudo apt-get update
sudo apt-get upgrade

sudo apt-get install libqt5core5a libqt5widgets5 libqt5gui5
sudo apt-get install qt5-qmake
sudo apt-get install qt5-default
sudo apt-get install qtbase5-private-dev


sudo apt-get install build-essential libgtk2.0-dev libjpeg-dev libtiff5-dev libjasper-dev libopenexr-dev cmake python-dev python-numpy python-tk libtbb-dev libeigen3-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev sphinx-common texlive-latex-extra libv4l-dev libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev default-jdk ant

# cd opencv-3.1.0/
unzip opencv-2.4.11.zip
cd opencv-2.4.11/
mkdir build
cd build

# cmake -D WITH_OPENGL=ON -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.1.0/modules/ ..
cmake -D WITH_OPENGL=ON ..
make -j4 # cuatro procesos de compilación
sudo make install
sudo echo /usr/local/lib > /etc/ld.so.conf.d/opencv.conf
sudo ldconfig

DIR=/etc/bash.bashrc
sudo echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig" >> $DIR
sudo echo "export PKG_CONFIG_PATH" >> $DIR
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
export PKG_CONFIG_PATH

cd ../../Programa/
qmake QtRatWalk/QtRatWalk.pro -o build/Makefile 
cd build/
make
cp QtRatWalk ~/Desktop/QtRatWalk
mkdir ~/Desktop/resources
cp -r resources/* ~/Desktop/resources
cp QtRatWalk ~/Escritorio/QtRatWalk
mkdir ~/Escritorio/resources
cp -r resources/* ~/Escritorio/resources
