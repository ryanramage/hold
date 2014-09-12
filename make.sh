# INSTRUCTIONS: copy and past the following inside your drone.io project setting.

# Environnement variables - must be copied in the reserved space
SRC=src
#TEST=/home/ubuntu/src/github.com/WeAreLeka/Bare-Arduino-Project/test
#ARDMK=/home/ubuntu/src/github.com/WeAreLeka/Bare-Arduino-Project/Arduino-Makefile

echo "Test all sources..."
cd $SRC
for dir in *; do
	if [ -d "${dir}" ]; then
		cd $dir
		#cp $CWD/Makefile-Drone.mk Makefile
		make
		echo "Building ${dir} passed with success!"
		cd ..
	fi
done

