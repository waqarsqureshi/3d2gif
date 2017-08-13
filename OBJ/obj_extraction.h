// This files defines a function to read the info from the mesh.
//#include<wrap/io_trimesh/io_mask.h>
//#include<wrap/io_trimesh/io_ply.h>
#include<vcg/container/simple_temporary_data.h>
#include <stdio.h>

extern std::vector<vertex2> vertices;
extern std::vector<Vid3> facets;
extern std::vector<texture> textures;
extern std::vector<v_color> v_rgb;
extern std::vector<v_normal> v_norm;
extern bool HasPNG;
extern vertex2 meanMass;
bool check_normal = false;

namespace vcg {
  namespace tri {
    namespace io {

      template <class SaveMeshType>
          class ExporterOBJ {
      public:
        typedef typename SaveMeshType::VertexPointer VertexPointer;
        typedef typename SaveMeshType::VertexType VertexType;
        typedef typename SaveMeshType::FaceType FaceType;
        typedef typename SaveMeshType::FacePointer FacePointer;
        typedef typename SaveMeshType::VertexIterator VertexIterator;
        typedef typename SaveMeshType::FaceIterator FaceIterator;

        static int getObjInfo(SaveMeshType &m) {
          int j;
          VertexPointer  vp;
          VertexIterator vi;

          SimpleTempData<typename SaveMeshType::VertContainer,int> indices(m.vert);

          vertex2 sum = 0;
          double vn[3];

          for(j=0, vi=m.vert.begin(); vi!=m.vert.end(); ++vi) {
            vp = &(*vi);
            indices[vi] = j;
            if(!HasPerVertexFlags(m) || !vp->IsD()) {
              double t[3];
              t[0] = double(vp->P()[0]);
              t[1] = double(vp->P()[1]);
              t[2] = double(vp->P()[2]);
              vertices.push_back(vertex2(t[0],t[1],t[2]));
              sum.x += t[0];
              sum.y += t[1];
              sum.z += t[2];
              j++;
            }

            /* if(HasPerVertexColor(m) && !HasPNG) {   // temporary use because HasPervertexcolor always true, need second condition, but later */
            if(!HasPNG) {   // temporary use because HasPervertexcolor always true, need second condition, but later
              double c[4];
              c[0] = double(vp->C()[0])/255;
              c[1] = double(vp->C()[1])/255;
              c[2] = double(vp->C()[2])/255;
              c[3] = double(vp->C()[3])/255;
              v_rgb.push_back(v_color(c[0],c[1],c[2]));
            }

            if (vp->HasNormal()) {
              double norm_vp = std::sqrt(std::pow(vp->N()[0],2) + std::pow(vp->N()[1],2) + std::pow(vp->N()[2],2));

              if (!check_normal && norm_vp < 0.001) { // if mesh has normal_maginitude ~0, meaning no normal for that mesh, then calculate it using vcglib
                vcg::tri::UpdateNormal<MyMesh>::PerVertexClear(m); // clear normal, vcglib
                vcg::tri::UpdateNormal<MyMesh>::PerVertex(m); // update normal, vcglib
                vcg::tri::UpdateNormal<MyMesh>::PerVertexAngleWeighted(m);
                /* vcg::tri::UpdateNormal<MyMesh>::PerVertexNelsonMaxWeighted(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::PerFace(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::PerVertexFromCurrentFaceNormal(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::PerFaceFromCurrentVertexNormal(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::NormalizePerVertex(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::NormalizePerFace(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::NormalizePerFaceByArea(m); */
                /* vcg::tri::UpdateNormal<MyMesh>::PerBitQuadFaceNormalized(m); */

                check_normal = true;
              }

              if (norm_vp == 0)
                norm_vp = 0.0000000000000001;

              vn[0] = double(vp->N()[0])/norm_vp;
              vn[1] = double(vp->N()[1])/norm_vp;
              vn[2] = double(vp->N()[2])/norm_vp;
            }
            else {
              vn[0] = 0;
              vn[1] = 0;
              vn[2] = 1;
            }

            v_norm.push_back(v_normal(vn[0],vn[1],vn[2]));
          }
          meanMass = vertex2(sum.x/vertices.size(), sum.y/vertices.size(), sum.z/vertices.size());

          assert(j==m.vn);

          FacePointer fp;
          FaceIterator fi;
          int fcnt=0;
          Vid3 facet;
          for(j=0, fi=m.face.begin(); fi!=m.face.end(); ++fi) {
            // for#1
            fp=&(*fi);
            if(!fp->IsD()) { // if#1
              fcnt++;
              facet.id1 = indices[fp->cV(0)];   // face's vertex.0 index
              facet.id2 = indices[fp->cV(1)];   // face's vertex.1 index
              facet.id3 = indices[fp->cV(2)];   // face's vertex.2 index
              facets.push_back(facet);
              if(HasPerWedgeTexCoord(m) && HasPNG) { // if#2
              /* if(HasPerWedgeTexCoord(m)) { // if#2 */
                double t[6];
                for(int k=0;k<3;++k) { // for#2
                  t[k*2+0] = fp->WT(k).u();    // texture coordinate (u) color of each vertex
                  t[k*2+1] = fp->WT(k).v();    // texture coordinate (v)) color of each vertex
                }// end for#2
                textures.push_back(texture(t[0], t[1], t[2], t[3], t[4], t[5]));
              }// end if#2
            }// end if#1
          }// end for#1

          return 0;
        }

      }; // end class

    } // end namespace tri
  } // end namespace io
} // end namespace vcg
