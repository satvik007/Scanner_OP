#!/bin/bash

echo "This is intended to be a universal OpenCV installation script, which supports installing on Anaconda Python too"
echo "Usually, if this script doesn't work, it's because the library versions may have been updated (of libvtk, libtiff, libjpeg and other dependencies) or OpenCV might have added new modules to contrib which require you to install additional dependencies. In such a scenario, kindly create in issue in the github repository."
echo
echo "NOTE: Enable Cannonical Partners in your Software Sources for ffmpeg installation. If you have, press [Enter]. Otherwise, [Ctrl+C], enable it, and re-run this script"
read -r temp

if ! echo "$PATH" | grep -q 'conda' ; then
    sudo apt-get install python3 python3-dev python3-numpy python3-pip python3-scipy python3-matplotlib python-dev python-matplotlib python-numpy python-scipy python-pip python-tk -y
else
    pip install numpy scipy matplotlib
fi


spatialPrint() {
    echo ""
    echo ""
    echo "$1"
    echo ""
    echo ""
}


spatialPrint "Finally download and install opencv"
git config --global http.postBuffer 1048576000
if [ ! -d "opencv" ]; then
	git clone https://github.com/Itseez/opencv
else
# Putting the git pull commands in paranthesis runs it in a subshell and avoids having to do cd ..
    (
        cd opencv || exit
        git pull
    )
fi
if [ ! -d "opencv_contrib" ]; then
	git clone https://github.com/Itseez/opencv_contrib
else
    (
        cd opencv_contrib || exit
        git pull
    )
fi

cd opencv || exit
mkdir build
cd build || exit


py2Ex=$(which python2)
py2In=$(python2 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())")
py2Pack=$(python2 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")
py3Ex=$(which python3)
py3In=$(python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())")
py3Pack=$(python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")

# This removes both Anaconda from the path, if it's there.
# Don't worry, your OpenCV WILL STILL BE INSTALLED FOR ANACONDA PYTHON if it is the default python
# This is important as anaconda has a malformed MKL library, and has different versions for a lot of other libraries too, and you'd like to use the full version instead
# Do note: If you are using anaconda, I would recommend creating a python 2.x environment in that, and adding that too to the env variable before running this script
export TEMP=$PATH
if echo "$PATH" | grep -q 'conda' ; then
    echo "Your PATH variable will be changed for the installation. Anaconda will be removed from the PATH because it messes the linkings and dependencies"
    export PATH=$(echo "$PATH" | tr ':' '\n' | grep -v "conda[2-9]\?" | uniq | tr '\n' ':')
fi


# Build tiff on as opencv supports tiff4, which is older version, which ubuntu has dropped
#  If you get an error, try disabling freetype by adding the following line in between the cmake command
#  -DBUILD_opencv_freetype=OFF \
cmake -DCMAKE_BUILD_TYPE=RELEASE \
 -DCMAKE_INSTALL_PREFIX=/usr/local \
 -DBUILD_opencv_cvv=OFF \
 -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
 -DBUILD_NEW_PYTHON_SUPPORT=ON \
 -DPYTHON2_EXECUTABLE="$py2Ex" \
 -DPYTHON2_INCLUDE_DIR="$py2In" \
 -DPYTHON2_PACKAGES_PATH="$py2Pack" \
 -DPYTHON3_EXECUTABLE="$py3Ex" \
 -DPYTHON3_INCLUDE_DIR="$py3In" \
 -DPYTHON3_PACKAGES_PATH="$py3Pack" \
 -DWITH_TBB=ON \
 -DWITH_V4L=ON \
 -DWITH_QT=ON \
 -DWITH_OPENGL=ON \
 -DWITH_VTK=ON \
 -DWITH_IPP=OFF \
 -DWITH_CUDA=OFF \
 -DBUILD_TESTS=OFF \
 -DBUILD_TIFF=ON \
 -DBUILD_opencv_java=ON \
 -DENABLE_AVX=ON ..

read -p -r "Press [Enter] to continue" temp

# De-comment the next line if you would like an interactive cmake menu to check if everything is alright and make some tweaks
# ccmake ..

spatialPrint "Making and installing"
make -j8
sudo make install

spatialPrint "Finishing off installation"
sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig

export PATH=$TEMP

echo "The installation just completed. If it shows an error in the end, kindly post an issue on the git repo"

