class MyEdge;
class MyFace;
class MyVertex;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyEdge>::AsEdgeType,vcg::Use<MyFace> ::AsFaceType>{};

class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::BitFlags, vcg::vertex::Color4b ,vcg::vertex::Normal3f >{};
/* class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::BitFlags  >{}; */
class MyFace    : public vcg::Face  < MyUsedTypes, vcg::face::VertexRef, vcg::face::FFAdj, vcg::face::WedgeTexCoord2f, vcg::face::Mark, vcg::face::BitFlags > {};
class MyEdge    : public vcg::Edge  <MyUsedTypes,  vcg::edge::VertexRef, vcg::edge::BitFlags,vcg::edge::EEAdj>{};
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {};


struct uv {
    double p0, p1, p2;
};

class vertex
{
private:
public:
    double x;
    double y;
    double z;
    vertex(double d1 = 0.0, double d2 = 0.0, double d3 = 0.0){
        x = d1;
        y = d2;
        z = d3;
    }
    double& operator[](int i){
        assert(0 <= i && i < 3);
        if (i == 0)
            return x;
        else if (i == 1)
            return y;
        return z;
    };
};

class Vid2
{
public:
    int id1, id2;
    Vid2(int id1 = -1, int id2 = -1){
        this->id1 = id1;
        this->id2 = id2;
    }
    virtual int& operator[](int i){
        assert(0 <= i && i < 2);
        if (i == 0)
            return id1;
        return id2; };
    Vid2 Sort2() {
        if (id1 > id2)
            return Vid2(id2, id1);
        return *this; }
};

class Vid3 : public Vid2
{
public:
    int id3;
    Vid3(int id1 = -1, int id2 = -1, int id3 = -1) : Vid2(id1, id2){
        this->id3 = id3;
    }
    int& operator[](int i){
        assert(0 <= i && i < 3);
        if (i == 0)
            return id1;
        else if (i == 1)
            return id2;
        return id3; };
    Vid3 Sort3() {
        Vid2 vid2 = Sort2();
        if (id3 < vid2.id1)
            return Vid3(id3, vid2.id1, vid2.id2);
        else if (id3 < vid2.id2)
            return Vid3(vid2.id1, id3, vid2.id2);
        return Vid3(vid2.id1, vid2.id2, id3);
    }
};

class vertex2
{
public:
    double x;
    double y;
    double z;
    vertex2(double d1 = 0.0, double d2 = 0.0, double d3 = 0.0){
        x = d1;
        y = d2;
        z = d3;
    }
};

class texture
{
public:
    double u0;
    double v0;
    double u1;
    double v1;
    double u2;
    double v2;
    texture(double d0 = 0.0, double d1 = 0.0, double d2 = 0.0, double d3 = 0.0, double d4 = 0.0, double d5 = 0.0){
        u0 = d0;
        v0 = d1;
        u1 = d2;
        v1 = d3;
        u2 = d4;
        v2 = d5;
    }
};

class v_color
{
public:
    double red;
    double green;
    double blue;
    v_color(double d1 = 0.0, double d2 = 0.0, double d3 = 0.0){
        red = d1;
        green = d2;
        blue = d3;
    }
};

class v_normal
{
public:
    double nx;
    double ny;
    double nz;
    v_normal(double d1 = 0.0, double d2 = 0.0, double d3 = 0.0){
        nx = d1;
        ny = d2;
        nz = d3;
    }
};
