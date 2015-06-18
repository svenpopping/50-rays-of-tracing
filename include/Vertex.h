#ifndef VERTEX_H
#define VERTEX_H

#include "Vec3D.h"

/************************************************************
 * Vertex of the mesh (containing normal n and vertetx v)
 ************************************************************/
class Vertex {
public:
    inline Vertex () {}
    inline Vertex (const Vec3Dd & p) : p (p) {}
    inline Vertex (const Vec3Dd & p, const Vec3Dd & n) : p (p), n (n){}
    inline Vertex (const Vertex & v) : p (v.p), n (v.n){}
    inline virtual ~Vertex () {}
    inline Vertex & operator= (const Vertex & v) {
        p = v.p;
        n = v.n;
		return (*this);
    }
    Vec3Dd p;
    Vec3Dd n;
};


#endif // VERTEX_H
