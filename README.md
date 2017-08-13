# 3d2gif
This program converts a 3D format file, OBJ and PLY to a gif file and mp4 file. Currently the program support PLY and OBJ format. The libraries required are:

1 - GLUT
2 - GL
3 - GLU
4 - OpenCV
5 - VCGLIB

The ply2gif is written by Samart Moodleah, Lecturer, Faculty of Information Technology, King Mongkut's Institute of Technology Ladkrabang. 

The obj2gif is edited from the ply2gif version by Waqar Shahid
## How to compile
gifPLY: ply2gif.cpp
	g++ -Wall -g ply2gif.cpp -o gifPLY -std=c++11 -I /home/xyz/vcglib/vcglib -I ../../libs/sm_lib/ `pkg-config opencv --cflags --libs` -lGL -lGLU -lglut
