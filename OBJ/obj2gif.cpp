// version 1.0 : initial version of OBJ to GIF
// version 1.1 : fix GIF black background of the 1st image
// version 1.11 : edited using ply2gif
// version 1.12 : edited to update bouding box and use -s default value to max of z bouding box.
// VCG LIB

#include<vcg/complex/complex.h>
//#include<vcg/complex/append.h>
// VCG LIB IO
#include <wrap/io_trimesh/import.h>
#include "wrap/ply/plylib.cpp"
// topology computation VCG LIB
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>

// this file header
#include "obj_header.h"
#include "obj_extraction.h"
// own library header
#include "filePath.h"
#include "clOptions.h"
//opencv using c opencv function
#include <opencv2/opencv.hpp>
//opengl header
#include <GL/glut.h>
//standard headers
#include <stdio.h>
#include <math.h>


// Global variables
std::vector<vertex2> vertices;    // for OBJ access, vertices
std::vector<Vid3> facets;         // for OBJ access, facet indices
std::vector<texture> textures;    // for OBJ access, texture coordinates
std::vector<v_color> rgb;
std::vector<v_color> v_rgb;
std::vector<v_normal> v_norm;
vertex2 meanMass;
bool HasPNG = false;
double rotateAngle = 0;
double incrementAngle = 20;
string folderPath;
string deleteCommand = "";
string makingMP4 = "";
string gif_option = "";
string mass_rotation = "";
string delayTime = "";
float angX = 0, angY = 0, angZ = 0;
float rx, ry, rz;
double bbox_max, fovy, sMul;
float sizeOBJ;
string lightOption = "";
string lightMode = "";
string shininess = "";
vertex2 lightAngle;
vector<v_color> rgbPerVertex;

// necessary functions
void getTextureCoodinates(std::vector<texture> &textureCoord, cv::Mat img);
void getTextureColor_perFace(std::vector<texture> textureCoord, cv::Mat img, std::vector<v_color> &rgb);
void getTextureColorV(std::vector<Vid3> f, std::vector<v_color> v_rgb, std::vector<v_color> &rgb);
void wt_facet_txt(std::string f, std::vector<Vid3> facets);
void wt_color_txt(std::string f, std::vector<v_color> rgb);
void wt_vert_txt(std::string f, std::vector<vertex2> vertices);
void CaptureViewPort(int no, string folder_path);
void myPNG(void);
void display(void);
void lightRotation(vertex2 &vert, float angle, float axisX, float axisY, float axisZ);
void help(void);

// Main programs
int main(int argc, char **argv) {
    int err;
    if ( argc == 1) {
        help();
    } else {

        // command line options
        char * fileName = getCmdOption(argv, argv + argc, "-i");   // * input option, required
        char *delCmd_ = getCmdOption(argv, argv + argc, "-q");     //   delete temp option, default is no - by not provide this option
        char *gifOpt_ = getCmdOption(argv, argv + argc, "-g");     //   compress GIF option, default is no - by not provide this option
        char *delayOpt_ = getCmdOption(argv, argv + argc, "-d");   //   delay of image transition in GIF, default is 120
        char *makingMP4_ = getCmdOption(argv, argv + argc, "-m");  //   create mp4, default is no - by not provide this option
        char *angInc_ = getCmdOption(argv, argv + argc, "-a");     //   angle increment of rotation image, default is 20 degree
        char *ww = getCmdOption(argv, argv + argc, "-w");          //   image width,  default is 320 pixel
        char *hh = getCmdOption(argv, argv + argc, "-h");          //   image height, default is 320 pixel
        char *sizeOBJ_ = getCmdOption(argv, argv + argc, "-s");    //   object-over-image portion, default is maximum bounding box by not provide this option
        char *sMul_ = getCmdOption(argv, argv + argc, "-sm");      //   scaling object-over-image, default is 1.7 by not provide this option
        char *fovy_ = getCmdOption(argv, argv + argc, "-f");       //   field-of-image (fovy) in Y-axis of gluperspective function, default is 30 degree
        char *angleX = getCmdOption(argv, argv + argc, "-x");      //   orientation of X-axis adjustment (default is   0 degree)
        char *angleY = getCmdOption(argv, argv + argc, "-y");      //   orientation of Y-axis adjustment (default is 180 degree)
        char *angleZ = getCmdOption(argv, argv + argc, "-z");      //   orientation of Z-axis adjustment (default is 180 degree)
        char *rx_ = getCmdOption(argv, argv + argc, "-rx");        //   rotation around X-axis, 0 is disable, 1 is enable (default is 0)
        char *ry_ = getCmdOption(argv, argv + argc, "-ry");        //   rotation around Y-axis, 0 is disable, 1 is enable (default is 1)
        char *rz_ = getCmdOption(argv, argv + argc, "-rz");        //   rotation around Z-axis, 0 is disable, 1 is enable (default is 0)
        char *massRot_ = getCmdOption(argv, argv + argc, "-mass"); //   rotation around center of mass, default is set to mass, give -mass y
        char *lightOption_ = getCmdOption(argv, argv + argc, "-l");//   enable light option when demostrating GIF
        char *lightMode_ = getCmdOption(argv, argv + argc, "--lightmode");//  light mode: Diffuse, Ambient and Diffuce, Ambient and Specular
        char *shininess_ = getCmdOption(argv, argv + argc, "--shininess");//  shininess of object with white light

        (!delCmd_) ? deleteCommand = "" : deleteCommand = delCmd_;
        (!gifOpt_) ? gif_option = "" : gif_option = gifOpt_;
        (!massRot_) ? mass_rotation = "y" : mass_rotation = massRot_;
        (!delayOpt_) ? delayTime = "120" : delayTime = delayOpt_;
        (!makingMP4_) ? makingMP4 = "no" : makingMP4 = "yes";
        (!lightOption_) ? lightOption = "on" : lightOption = lightOption_;
        (!lightMode_) ? lightMode = "Diffuse" : lightMode = lightMode_;
        (!shininess_) ? shininess = "no" : shininess = shininess_;
        if (angInc_) incrementAngle = stoi(angInc_);
        if (!ww) ww = "500";
        if (!hh) hh = "500";

        // (!sizeOBJ_) ? sizeOBJ = 4.0 : sizeOBJ = stod(sizeOBJ_);
        (!angleX) ? angX =   90.0 : angX = stoi(angleX);
        (!angleY) ? angY = 90.0 : angY = stoi(angleY);
        (!angleZ) ? angZ = -90.0 : angZ = stoi(angleZ);
        (!rx_) ? rx = 0.0 : rx = stod(rx_);
        (!ry_) ? ry = 1.0 : ry = stod(ry_);
        (!rz_) ? rz = 0.0 : rz = stod(rz_);

        MyMesh mesh;
        string objName, pngName;
        filePath inputFile(fileName);
        folderPath = inputFile.path;

        objName = inputFile.path + inputFile.name;
        err = vcg::tri::io::Importer<MyMesh>::Open(mesh,objName.c_str()); // read OBJ
	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);// update_waqar
	std::cout<<"Input mesh vn: "<<mesh.VN()<<" fn: "<<mesh.FN()<<std::endl;//waqar_debug

        cout<<"mesh.texture size = "<<mesh.textures.size()<<endl;//waqar_debug
        if (mesh.textures.size()>0)
                HasPNG = true;

        vcg::tri::io::ExporterOBJ<MyMesh>::getObjInfo(mesh);            // extract OBJ information, faces, vertices, texcoord

        if(HasPNG)
                pngName = inputFile.path + mesh.textures[0];

        lightAngle.x = 0;
        lightAngle.y = 0;
        lightAngle.z = 1;

        cv::Mat img = cv::imread(pngName, cv::IMREAD_COLOR);   // Read the image file

        std::vector<texture> textureCoord;
        v_color rgb_;

        if (HasPNG) {
                getTextureCoodinates(textureCoord, img);
                getTextureColor_perFace(textureCoord, img, rgb);
        }
        else
                getTextureColorV(facets, v_rgb, rgb);

	vcg::Point3<float> center = mesh.bbox.Center();
	std::cout<< "mesh center x= "<<center[0]<<std::endl;// waqar_debug
	std::cout<< "mesh center y= "<<center[1]<<std::endl;// waqar_debug
	std::cout<< "mesh center z= "<<center[2]<<std::endl;// waqar_debug

        double min_[3] = {mesh.bbox.min[0], mesh.bbox.min[1], mesh.bbox.min[2]};
        double max_[3] = {mesh.bbox.max[0], mesh.bbox.max[1], mesh.bbox.max[2]};
        vertex2 ct = vertex2(0.5*(mesh.bbox.max[0] + mesh.bbox.min[0]),
                             0.5*(mesh.bbox.max[1] + mesh.bbox.min[1]),
                             0.5*(mesh.bbox.max[2] + mesh.bbox.min[2]));

        // get max bounding box, for znear and zfar in openGL perspective
        double bbox[3] = {std::max(fabs(mesh.bbox.max[0]),fabs(mesh.bbox.min[0])),
                           std::max(fabs(mesh.bbox.max[1]),fabs(mesh.bbox.min[1])),
                           std::max(fabs(mesh.bbox.max[2]),fabs(mesh.bbox.min[2]))};
        bbox_max = std::max(bbox[0], bbox[1]);
        bbox_max = std::max(bbox_max, bbox[2]);

	std::cout<< "mesh center z= "<<bbox_max<<std::endl;// waqar_debug
	
        (!fovy_) ? fovy = 30 : fovy = stod(fovy_);   // Field-Of_Image in Y axis, the larger the bigger view
        (!sMul_) ? sMul = (1.5) : sMul = stod(sMul_);  // default of -s is approximately 1.7 * max-z
        (!sizeOBJ_) ? sizeOBJ = sMul*bbox_max : sizeOBJ = stod(sizeOBJ_);

        if (mass_rotation.compare("y")==0) // rotation around mass-center on object
                ct = meanMass;// I dont know what is this -- waqar

        for (int i=0; i<vertices.size(); i++) {
                vertices[i].x = vertices[i].x - ct.x;
                vertices[i].y = vertices[i].y - ct.y;
                vertices[i].z = vertices[i].z - ct.z;
        }

        string tempName = folderPath + "temp";
        string tempCmd = "mkdir " + tempName;
        std::system(tempCmd.c_str());
/* // No need to save this files, they are tmp files
        string name_ = folderPath + "/faces.txt";
        wt_facet_txt(name_, facets);
        name_ = folderPath + "/color.txt";
        wt_color_txt(name_, rgb);
        name_ = folderPath + "/vertices.txt";
        wt_vert_txt(name_, vertices);
*/
        // Display rotating object and capture its to JPG
        glutInit( &argc, argv );
        glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
        glutInitWindowSize( stoi(ww), stoi(hh) );
        glutCreateWindow( "GLUT" );
        glutDisplayFunc( display );
        glutIdleFunc( display );


        glEnable( GL_DEPTH_TEST );
        glutMainLoop();

    }

        return err;
}

void wt_facet_txt(std::string f, std::vector<Vid3> facets) {
        FILE* fout = fopen(f.c_str(), "w");
        for (int j = 0; j < facets.size(); j++) {
                Vid3 &facet = facets[j];
                fprintf(fout, "%i\t", facet.id1);
                fprintf(fout, "%i\t", facet.id2);
                fprintf(fout, "%i\t", facet.id3);
                fprintf(fout, "\n");
        }
        fclose(fout);
}

void wt_color_txt(std::string f, std::vector<v_color> rgb) {
        FILE* fout = fopen(f.c_str(), "w");
        v_color rgb_;
        for (int j = 0; j < rgb.size(); j++) {
                rgb_ = rgb[j];
                fprintf(fout, "%f\t", rgb_.red);
                fprintf(fout, "%f\t", rgb_.green);
                fprintf(fout, "%f\t", rgb_.blue);
                fprintf(fout, "\n");
        }
        fclose(fout);
}

void wt_vert_txt(std::string f, std::vector<vertex2> vertices) {
        FILE* fout = fopen(f.c_str(), "w");
        for (int j = 0; j < vertices.size(); j++) {
                vertex2 &vert = vertices[j];
                fprintf(fout, "%f\t", vert.x);
                fprintf(fout, "%f\t", vert.y);
                fprintf(fout, "%f\t", vert.z);
                fprintf(fout, "\n");
        }
        fclose(fout);
}

void CaptureViewPort(int no, string folder_path) {

        GLubyte * bits; //RGB bits
        GLint viewport[4]; //current viewport

        //get current viewport
        glGetIntegerv(GL_VIEWPORT, viewport);

        int w = viewport[2];
        int h = viewport[3];

        bits = new GLubyte[w*3*h];

        //read pixel from frame buffer
        glFinish(); //finish all commands of OpenGL
        glPixelStorei(GL_PACK_ALIGNMENT,1); //or glPixelStorei(GL_PACK_ALIGNMENT,4);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glReadPixels(0, 0, w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE, bits);

        cv::Mat capImg = cv::Mat(cv::Size(w, h), CV_8UC3);
        for(int i=0; i < h; ++i)
        {
                for(int j=0; j < w; ++j)
                {
                        capImg.at<cv::Vec3b>(i, j)[0] = static_cast<unsigned char>(bits[(h - i - 1) * 3 * w + j * 3 + 0]); // Blue
                        capImg.at<cv::Vec3b>(i, j)[1] = static_cast<unsigned char>(bits[(h - i - 1) * 3 * w + j * 3 + 1]); // Green
                        capImg.at<cv::Vec3b>(i, j)[2] = static_cast<unsigned char>(bits[(h - i - 1) * 3 * w + j * 3 + 2]); // Red
                }
        }

        string imgName, tempName, tempCmd;
        tempName = folderPath + "temp";
        // tempCmd = "mkdir " + tempName;
        // std::system(tempCmd.c_str());
        // string imgName;
        if (no <10)
                imgName = tempName + "/results_00" + std::to_string(no) + ".jpg";
        else if (no <100)
                imgName = tempName + "/results_0" + std::to_string(no) + ".jpg";
        else
                imgName = tempName + "/results_" + std::to_string(no) + ".jpg";

        // imgName = "result" + std::to_string(no) + ".jpg";
        cv::imwrite(imgName, capImg);
        // cvSaveImage("result.jpg",capImg);
        delete[] bits;

}

void myPNG(void) {

        if (lightOption.compare("on") == 0) {
                glEnable(GL_LIGHTING);  // Enable lighting
                glEnable(GL_LIGHT0);    // Enable lighting
                // GLfloat lightpos[] = {lightAngle.x, lightAngle.y, lightAngle.z, 1.};
                // GLfloat lightpos[] = {0., 0., 1., 1.};        // equally light in all directions
                // glLightfv(GL_LIGHT0, GL_POSITION, lightpos);  // equally light in all directions

                if (shininess.compare("yes") == 0) {
                        GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
                        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                        GLfloat shininess_[] = {199};
                        glMaterialfv(GL_FRONT, GL_SHININESS, shininess_);
                }
        }

        glClear(GL_COLOR_BUFFER_BIT);     // clear the screen
        glBegin(GL_TRIANGLES);

        v_color rgbVert1, rgbVert2, rgbVert3;
        for (int i=0; i<facets.size(); i++) {
                Vid3 &facet = facets[i];
                vertex2 &v0 = vertices[facet.id1];
                vertex2 &v1 = vertices[facet.id2];
                vertex2 &v2 = vertices[facet.id3];
                rgbVert1 = rgbPerVertex[facet.id1];
                rgbVert2 = rgbPerVertex[facet.id2];
                rgbVert3 = rgbPerVertex[facet.id3];

                if (lightOption.compare("on") == 0) {
                        { // vertex #1
                                GLfloat gl_vertex_color1[] = {rgbVert1.red, rgbVert1.green, rgbVert1.blue, 1.f}; // prepare vertex-color-for-lighting
                                if (lightMode.compare("AmbientAndDiffuse") == 0)
                                        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gl_vertex_color1);  // color by Ambient and Diffuse option, 2nd option
                                else if (lightMode.compare("Ambient") == 0)
                                        glMaterialfv(GL_FRONT, GL_AMBIENT, gl_vertex_color1);  // color by Ambient option
                                else if (lightMode.compare("Specular") == 0)
                                        glMaterialfv(GL_FRONT, GL_SPECULAR, gl_vertex_color1);  // color by Diffuse option
                                else
                                        glMaterialfv(GL_FRONT, GL_DIFFUSE, gl_vertex_color1);  // color by Diffuse option, 1st option

                                glNormal3d(v_norm[facet.id1].nx, v_norm[facet.id1].ny, v_norm[facet.id1].nz);
                                glVertex3d(v0.x, v0.y, v0.z);
                        }

                        { // vertex #2
                                GLfloat gl_vertex_color2[] = {rgbVert2.red, rgbVert2.green, rgbVert2.blue, 1.f}; // prepare vertex-color-for-lighting
                                if (lightMode.compare("AmbientAndDiffuse") == 0)
                                        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gl_vertex_color2);  // color by Ambient and Diffuse option, 2nd option
                                else if (lightMode.compare("Ambient") == 0)
                                        glMaterialfv(GL_FRONT, GL_AMBIENT, gl_vertex_color2);  // color by Ambient option
                                else if (lightMode.compare("Specular") == 0)
                                        glMaterialfv(GL_FRONT, GL_SPECULAR, gl_vertex_color2);  // color by Diffuse option
                                else
                                        glMaterialfv(GL_FRONT, GL_DIFFUSE, gl_vertex_color2);  // color by Diffuse option, 1st option

                                glNormal3d(v_norm[facet.id2].nx, v_norm[facet.id2].ny, v_norm[facet.id2].nz);
                                glVertex3d(v1.x, v1.y, v1.z);
                        }

                        { // vertex #3
                                GLfloat gl_vertex_color3[] = {rgbVert3.red, rgbVert3.green, rgbVert3.blue, 1.f}; // prepare vertex-color-for-lighting
                                if (lightMode.compare("AmbientAndDiffuse") == 0)
                                        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gl_vertex_color3);  // color by Ambient and Diffuse option, 2nd option
                                else if (lightMode.compare("Ambient") == 0)
                                        glMaterialfv(GL_FRONT, GL_AMBIENT, gl_vertex_color3);  // color by Ambient option
                                else if (lightMode.compare("Specular") == 0)
                                        glMaterialfv(GL_FRONT, GL_SPECULAR, gl_vertex_color3);  // color by Diffuse option
                                else
                                        glMaterialfv(GL_FRONT, GL_DIFFUSE, gl_vertex_color3);  // color by Diffuse option, 1st option

                                glNormal3d(v_norm[facet.id3].nx, v_norm[facet.id3].ny, v_norm[facet.id2].nz);
                                glVertex3d(v2.x, v2.y, v2.z);
                        }
                }
                else {
                        /* 1st vertex */
                        glColor3f(rgbVert1.red, rgbVert1.green, rgbVert1.blue);
                        glVertex3d(v0.x, v0.y, v0.z);

                        /* 2nd vertex */
                        glColor3f(rgbVert2.red, rgbVert2.green, rgbVert2.blue);
                        glVertex3d(v1.x, v1.y, v1.z);

                        /* 3rd vertex */
                        glColor3f(rgbVert3.red, rgbVert3.green, rgbVert3.blue);
                        glVertex3d(v2.x, v2.y, v2.z);
                }
        }
        glEnd();
        glFlush();		                 // send all output to display
}

void display(void) {
        // glClearColor( 0, 0, 0, 1 ); // black background
        glClearColor( 1, 1, 1, 1 ); // white background
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        int w = glutGet( GLUT_WINDOW_WIDTH );
        int h = glutGet( GLUT_WINDOW_HEIGHT );
        float min_z = bbox_max/1000000; // min-z
        float max_z = bbox_max*3; // max-z
	
        gluPerspective( fovy, w / h, min_z, max_z );
        // gluPerspective( 30, w / h, 0.1, 100 );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        // gluLookAt( 0, 0, 4,
        //            0, 0, 0,
        //            0, 1, 0
        //            );
        gluLookAt( 0, 0, sizeOBJ,
                   0, 0, 0,
                   0, 1, 0
                   );
        glRotatef( angZ, 0.0, 0.0, 1.0 );   // rotate around z-axis
        glRotatef( angY, 0.0, 1.0, 0.0 );   // rotate around y-axis
        glRotatef( angX, 1.0, 0.0, 0.0 );   // rotate around x-axis

        // lightRotation(lightAngle, rotateAngle, rx, ry, rz);    // rotate light corresponding to object rotation EVAL?

        if (rx!=0)
                // lightRotation(lightAngle, rotateAngle, rx, 0, 0);
                lightRotation(lightAngle, incrementAngle, rx, 0, 0);
        if (ry!=0)
                // lightRotation(lightAngle, rotateAngle, 0, ry, 0);
                lightRotation(lightAngle, incrementAngle, 0, ry, 0);
        if (rz!=0)
                // lightRotation(lightAngle, rotateAngle, 0, 0, rz);
                lightRotation(lightAngle, incrementAngle, 0, 0, rz);

        // glRotatef( -90, 0.0, 0.0, 1.0 );   // rotate around z-axis
        // glRotatef(   0, 0.0, 1.0, 0.0 );   // rotate around y-axis
        // glRotatef( 180, 1.0, 0.0, 0.0 );   // rotate around x-axis
        // glRotatef( rotateAngle, 1.0, 0.0, 0.0 );
        glRotatef( rotateAngle, rx, ry, rz );

        myPNG();
        CaptureViewPort(rotateAngle, "ii");

        rotateAngle += incrementAngle;
        if (rotateAngle > 359) {
            std::string command, option, gif_file, png, delay, output_name;

            // below 3 lines are the fix of black-background for
            // the 1st image, possibly rendering problem,
            // so, remove 1st image and recapture it, temporary fixing
            command = "rm " + folderPath +"temp/results_000.jpg";
            std::system(command.c_str());
            CaptureViewPort(0, "ii");

            output_name = "output.gif";
            // delay = "0";
            png = folderPath + "temp/*.jpg";
            gif_file = folderPath + output_name;
            // command = "convert -delay " + delay + " -loop 0 " + png + " " + gif_file.c_str();
            option =  " -dither none";
            option += " -layers optimizePlus";
            option += " -matte -depth 8 ";
            option += " \\( -clone 0--1 -background none +append -quantize transparent  -colors 32  -unique-colors -write mpr:cmap +delete \\) ";
            option += "-map mpr:cmap ";
            if (gif_option.compare("y")==0) // compress-GIF
            command = "convert -delay " + delayTime + " " + png + option + gif_file.c_str();
            else // losses-GIF
            command = "convert -delay " + delayTime + " -loop 0 " + png + " " + gif_file.c_str();

            std::system(command.c_str()); // convert images to lossess-GIF, losses

            if (makingMP4.compare("yes") == 0) {
                std::string ll_option;
                ll_option =  " -i ";
                ll_option += gif_file.c_str();
                ll_option += " -movflags faststart -pix_fmt yuv420p";
                ll_option += " -vf";
                ll_option += " \"scale=trunc(iw/3)*2:trunc(ih/3)*2\" ";
                ll_option += folderPath + "output.mp4";
                command = "ffmpeg" + ll_option;
                std::system(command.c_str()); // convert GIF/lossess-GIF to MP4 -> much smaller
            }

            // keep the 1st JPG image, for displaying purpose
            command = "cp " + folderPath +"temp/results_000.jpg " + folderPath + "output.JPG";
            std::system(command.c_str());

            if (deleteCommand.compare("del")==0) {
                command = "rm -rf " + folderPath +"temp";
                std::system(command.c_str());
            }
            exit(0);
        }

        glutSwapBuffers();
}


// TODO: need to find the way to map uv-texcoord and image-color
void getTextureCoodinates(std::vector<texture> &textureCoord, cv::Mat img) {
        for (int i=0; i<textures.size(); i++){
                texture &tt = textures[i];
                textureCoord.push_back(texture(round(tt.u0*(img.cols-1)),
                                               round(img.rows - 1 - tt.v0*(img.rows-1)),             // xy and pixel coordinates have different convention in y (row) axis
                                               round(tt.u1*(img.cols-1)),
                                               round(img.rows - 1 - tt.v1*(img.rows-1)),
                                               round(tt.u2*(img.cols-1)),
                                               round(img.rows - 1 - tt.v2*(img.rows-1))));
        }
}

void getTextureColor_perFace(std::vector<texture> textureCoord, cv::Mat img, std::vector<v_color> &rgb) {
        rgbPerVertex.resize(vertices.size());
        for (int i=0;i < textureCoord.size(); i++) {
                cv::Vec3b intensity = img.at<cv::Vec3b>(textureCoord[i].v0, textureCoord[i].u0);         // only RGB of 1st vertex in each facet
                rgb.push_back(v_color(intensity.val[2]/255.0,   // red
                                      intensity.val[1]/255.0,   // green
                                      intensity.val[0]/255.0)); // blue

                rgbPerVertex[facets[i].id1].red = intensity.val[2]/255.0;
                rgbPerVertex[facets[i].id1].green = intensity.val[1]/255.0;
                rgbPerVertex[facets[i].id1].blue = intensity.val[0]/255.0;

                intensity = img.at<cv::Vec3b>(textureCoord[i].v1, textureCoord[i].u1);         // 2nd vertex
                rgbPerVertex[facets[i].id2].red = intensity.val[2]/255.0;
                rgbPerVertex[facets[i].id2].green = intensity.val[1]/255.0;
                rgbPerVertex[facets[i].id2].blue = intensity.val[0]/255.0;

                intensity = img.at<cv::Vec3b>(textureCoord[i].v2, textureCoord[i].u2);         // 3rd vertex
                rgbPerVertex[facets[i].id3].red = intensity.val[2]/255.0;
                rgbPerVertex[facets[i].id3].green = intensity.val[1]/255.0;
                rgbPerVertex[facets[i].id3].blue = intensity.val[0]/255.0;
        }
}

void getTextureColorV(std::vector<Vid3> f, std::vector<v_color> v_rgb, std::vector<v_color> &rgb) {
        for (int i=0;i < f.size(); i++) {
                int fn = f[i][0];
                rgb.push_back(v_color(v_rgb[fn].red,   // red
                                      v_rgb[fn].green,   // green
                                      v_rgb[fn].blue)); // blue
        }
}

void lightRotation(vertex2 &vert, float angle, float axisX, float axisY, float axisZ) {
        double x, y, z, zeta;
        zeta = angle * M_PI / 180;

        // rotate around WinX axis
        if (axisX!=0) {
                x = vert.x;
                y = vert.y*cos(zeta) - vert.z*sin(zeta);
                z = vert.y*sin(zeta) + vert.z*cos(zeta);
        }

        // rotate around WinY axis
        if (axisY!=0) {
                x = vert.x*cos(zeta) + vert.z*sin(zeta);
                y = vert.y;
                z =-vert.x*sin(zeta) + vert.z*cos(zeta);
        }

        // rotate around WinZ axis
        if (axisZ!=0) {
                x = vert.x*cos(zeta) - vert.y*sin(zeta);
                y = vert.x*sin(zeta) + vert.y*cos(zeta);
                z = vert.z;
        }
        vert.x = x;
        vert.y = y;
        vert.z = z;
}


void help() {
    std::cout << "usage exmample: <no need to provide options,unless you want to change the default.> \n" << std::endl;
    std::cout << "./gifOBJ -i /fullpath/to/OBJ\n" << std::endl;
    std::cout << "-i: *required fullpath of OBJ object" << std::endl;
    std::cout << "-q: delete temp image for GIF, default is no - by not provide this option" << std::endl;
    std::cout << "-g: GIF option, default is no - by not provide this option" << std::endl;
    std::cout << "-d: delay of image transition in GIF, default is 120" << std::endl;
    std::cout << "-m: mp4, default is no - by not provide this option" << std::endl;
    std::cout << "-a: increment of rotation image, default is 20 degree" << std::endl;
    std::cout << "-w: width,  default is 320 pixel" << std::endl;
    std::cout << "-h: height, default is 320 pixel" << std::endl;
    std::cout << "-s: over-image portion, default is maximum bounding box by not provide this option" << std::endl;
    std::cout << "-sm: object-over-image, default is 1.7 by not provide this option" << std::endl;
    std::cout << "-f: field-of-image (fovy) in Y-axis of gluperspective function, default is 30 degree" << std::endl;
    std::cout << "-x: orientation of X-axis adjustment (default is   0 degree)" << std::endl;
    std::cout << "-y: orientation of Y-axis adjustment (default is 180 degree)" << std::endl;
    std::cout << "-z: orientation of Z-axis adjustment (default is 180 degree)" << std::endl;
    std::cout << "-rx: rotation around X-axis, 0 is disable, 1 is enable (default is 0)" << std::endl;
    std::cout << "-ry: rotation around Y-axis, 0 is disable, 1 is enable (default is 1)" << std::endl;
    std::cout << "-rz: rotation around Z-axis, 0 is disable, 1 is enable (default is 0)" << std::endl;
    std::cout << "-mass: rotation around center of mass, default is set to mass, give -mass y" << std::endl;
    std::cout << "-l: enable light option when demostrating GIF" << std::endl;
    std::cout << "--lightmode: light mode: Diffuse, Ambient and Diffuce, Ambient and Specular" << std::endl;
    std::cout << "--shininess:  //  shininess of object with white light" << std::endl;
}
