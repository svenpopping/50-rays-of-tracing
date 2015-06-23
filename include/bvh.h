#pragma  once
#include <iostream>
#include <vector>
#include <cmath>
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
		this->nodes.push_back(bvh_node<F>(lb, rb));
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
			for (bvh_node<F> &n : nodes) {
				if (n.children.size() > N) {
					did_split = true;

					F xd = fabs(n.bounds[0].p[0] - n.bounds[1].p[0]);
					F yd = fabs(n.bounds[0].p[1] - n.bounds[1].p[1]);
					F zd = fabs(n.bounds[0].p[2] - n.bounds[1].p[2]);
					Vec3D<F> nl(n.bounds[0]), nu(n.bounds[1]);
					if (xd >= yd && xd >= zd) {
						n.bounds[1].p[0] -= 0.5 * xd;
						nl.p[0] += 0.5 * xd;
					} else if (yd >= xd && yd >= zd) {
						n.bounds[1].p[1] -= 0.5 * yd;
						nl.p[1] += 0.5 * yd;
					} else {
						n.bounds[1].p[2] -= 0.5 * zd;
						nl.p[2] += 0.5 * zd;
					}

					bvh_node<F> nn(nl, nu);
					for (auto it = n.children.begin(); it < n.children.end(); it++) {
						bvh_triangle<F> &t = *it;
						if (!t.in(n.bounds[0], n.bounds[1])) {
							nn.children.push_back(t);
							n.children.erase(it);
						}
					}
					nodes.push_back(nn);
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

	inline bool in(const Vec3D<F> &lb, const Vec3D<F> &rb)
	{
		if (lbound >= lb && rbound >= rb)
			return true;
		return lb <= center && rb > center;
	}
};

template <typename F, int N> std::ostream &operator<<(std::ostream &output, const bvh<F, N> &b) {
	for (const bvh_node<F> &n : b.nodes) {
		output << " node<L: " << n.bounds[0] << ", R: " << n.bounds[1] << ", S: " << n.children.size() << ">" <<  std::endl;
		for (auto v : n.children) {
			output << "  - triangle<X: " << v.vertices[0] << ", Y: " << v.vertices[1] << ", Z: " << v.vertices[2] << ", LB: " << v.lbound << ", RB: " << v.rbound << ", C: " << v.center << ">" << std::endl;
		}
	}
	return output;
}

