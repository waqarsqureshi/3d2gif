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
## How to run the code

// The instruction are same as ply. The only important thing is the obj. its png and mtl file should be in the same executable forlder and not in another folder.

usage example:

 ./gifOBJ -i ./scene_dense_mesh_texture.obj -a 10 -x 90 -y 90  -z 0  -rx 0  -ry 0  -rz 1 -d 10  -f 60

 ./gifOBJ -i ./milinda_bin.obj -a 10 -x 0 -y 180  -z 90  -rx 1  -ry 0  -rz 0 -d 10  -f 60

(to set to default by not giving any options)
 
-w 500, -h 500 : image size (default is 500x500)
-a 5 : increasing rotation by 5 degree (default is 5)
-g y: compress size of GIF (default is NO)
-x 180: image rotate around x-axis by 180 degree (default is 180 -> set for output of Waqar reconstruction algorithm)
-y    0: image rotate around x-axis by 180 degree (default is      0 -> set for output of Waqar reconstruction algorithm)
-z -90: image rotate around x-axis by 180 degree (default is   -90 -> set for output of Waqar reconstruction algorithm) 

additional options are:
-x, -y, -z : re-orientation of PLY to proper position before rotation
-rx, -ry, -rz : rotation axis (1 enable, 0 disable: depends on what is the combination os -x, -y, -z)
-d 0 :delay 0 second
-s 4 : this is set the size of object ( if using manually try to set the max of bounding box)

