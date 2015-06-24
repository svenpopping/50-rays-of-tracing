#pragma  once
#include <iostream>
#include <vector>
#include <cmath>
#include <cfloat>
#include "mesh.h"
#include "Vec3D.h"

template<typename F, int N> class bvh;
template<typename F> class bvh_node;
template<typename F> class bvh_triangle;

template<typename F, int N> class bvh
{
public:
	bvh() = default;

	void build(const Mesh &m, const Vec3D<F> lb, const Vec3D<F> rb)
	{
		nodes.push_back(bvh_node<F>(lb, rb));
		for (unsigned i = 0; i < m.triangles.size(); i++) {
			add_child(m, i, m.triangles[i]);
		}

		std::cout << *this << std::endl;
		split();
		std::cout << *this << std::endl;
	}

	void add_child(const Mesh &m, unsigned idx, const Triangle &t)
	{
		this->nodes[0].children.push_back(bvh_triangle<F>(m, idx, t));
	}

	void split(void)
	{
		bool did_split;
		do {
			did_split = false;
	
			for (auto ni = nodes.begin(); ni < nodes.end(); ni++) {
				bvh_node<F> &n = *ni;

				if (n.children.size() > N) {
					// Try halfway split in the X, Y and Z axes.
					bool didx = false, didy = false, didz = false;
					while (true) {
						std::cout << "Found possible split: n = " << n.children.size() << std::endl;

						// Find largest split by volume.
						F xd = n.bounds[1].p[0] - n.bounds[0].p[0], xxd = fabs(xd);
						F yd = n.bounds[1].p[1] - n.bounds[0].p[1], yyd = fabs(yd);
						F zd = n.bounds[1].p[2] - n.bounds[0].p[2], zzd = fabs(zd);
						std::cout << "Old bounds: " << n.bounds[0] << "/" << n.bounds[1] << std::endl;

						Vec3D<F> nl(n.bounds[0]), nu(n.bounds[1]);
						if (!didx && xxd >= yyd && xxd >= zzd) {
							didx = true;
							n.bounds[1].p[0] -= 0.5 * xd;
							nl.p[0] += 0.5 * xd;
						} else if (!didy && yyd >= xxd && yyd >= zzd) {
							didy = true;
							n.bounds[1].p[1] -= 0.5 * yd;
							nl.p[1] += 0.5 * yd;
						} else if (!didz) {
							didz = true;
							n.bounds[1].p[2] -= 0.5 * zd;
							nl.p[2] += 0.5 * zd;
						} else {
							break;
						}

						
						std::cout << "New bounds: " << n.bounds[0] << "/" << n.bounds[1] << std::endl;
						std::cout << "dx / dy / dz: " << xd << " / " << yd << " / " << zd << std::endl;

						// Move over triangles and re-calculate boundaries for both boxes.
						bvh_node<F> nn(nl, nu);
						Vec3D<F>  minb(DBL_MAX, DBL_MAX, DBL_MAX),  maxb(DBL_MIN, DBL_MIN, DBL_MIN);
						Vec3D<F> nminb(DBL_MAX, DBL_MAX, DBL_MAX), nmaxb(DBL_MIN, DBL_MIN, DBL_MIN);
						for (auto it = n.children.begin(); it < n.children.end(); it++) {
							const bvh_triangle<F> &t = *it;
							if (!t.in(n.bounds[0], n.bounds[1])) {
								nn.children.push_back(t);
								n.children.erase(it);
								for (int i = 0; i < 3; i++) {
									nminb.p[i] = fmin(t.lbound.p[i], nminb.p[i]);
									nmaxb.p[i] = fmax(t.rbound.p[i], nmaxb.p[i]);
								}
							} else
								for (int i = 0; i < 3; i++) {
									minb.p[i] = fmin(t.lbound.p[i], minb.p[i]);
									maxb.p[i] = fmax(t.rbound.p[i], maxb.p[i]);
								}
						}

						n.bounds[0] = minb;
						n.bounds[1] = maxb;
						nn.bounds[0] = nminb;
						nn.bounds[1] = nmaxb;

						// See if we got anything and move along if we have.
						if (nn.children.size() > 0) {
							nodes.push_back(nn);
							did_split = true;
							didx = didy = didz = false;
							break;
						}
					}
					if (did_split)
						break;
				}
			}
		} while (did_split);
	}

	std::vector<bvh_node<F> > nodes;
};

template<typename F> class bvh_node
{
public:
	bvh_node(const Vec3D<F> lower, const Vec3D<F> upper)
	{
		this->bounds[0] = lower;
		this->bounds[1] = upper;
	}

	Vec3D<F> bounds[2];
	std::vector<bvh_triangle<F> > children;
};


#define pm(f, v, i) (f(v[0].p[i], f(v[1].p[i], v[2].p[i])))

template<typename F> class bvh_triangle
{
public:
	Vec3D<F> vertices[3];
	Vec3D<F> lbound, rbound;
	Vec3D<F> center;
	unsigned index;

	bvh_triangle(const Mesh &m, unsigned idx, const Triangle &t)
	{
		vertices[0] = m.vertices.at(t.v[0]).p;
		vertices[1] = m.vertices.at(t.v[1]).p;
		vertices[2] = m.vertices.at(t.v[2]).p;
		lbound = Vec3D<F>(pm(fmin, vertices, 0), pm(fmin, vertices, 1), pm(fmin, vertices, 2));
		rbound = Vec3D<F>(pm(fmax, vertices, 0), pm(fmax, vertices, 1), pm(fmax, vertices, 2));
		center = (vertices[0] + vertices[1] + vertices[2]) / 3;
		index = idx;
	}

	inline bool in(const Vec3D<F> &lb, const Vec3D<F> &rb) const
	{
		if (lbound >= lb && rbound < rb)
			return true;
		return center >= lb && center < rb;
	}
};

template <typename F, int N> std::ostream &operator<<(std::ostream &output, const bvh<F, N> &b)
{
	output << "BVH<N: " << b.nodes.size() << ">" << std::endl;
	for (const bvh_node<F> &n : b.nodes) {
		output << n;
	}
	return output;
}

template <typename F> std::ostream &operator<<(std::ostream &output, const bvh_node<F> &n)
{
	output << " node<L: " << n.bounds[0] << ", R: " << n.bounds[1] << ", S: " << n.children.size() << ">" << std::endl;
	for (auto v : n.children) {
		output << v;
	}
	return output;
}

template <typename F> std::ostream &operator<<(std::ostream &output, const bvh_triangle<F> &v)
{
	output << "  - triangle<X: " << v.vertices[0] << ", Y: " << v.vertices[1] << ", Z: " << v.vertices[2] << ", LB: " << v.lbound << ", RB: " << v.rbound << ", C: " << v.center << ">" << std::endl;
	return output;
}
