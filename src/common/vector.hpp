#ifndef H_VECTOR_HPP
#define H_VECTOR_HPP

#include <cmath>


namespace mn {

namespace gl {


template<class T = float>
struct Vector {
	typedef T value_type;

	T x, y, z;

	Vector(T theX = 0, T theY = 0, T theZ = 0)
		: x(theX), y(theY), z(theZ) { }
	template<class T2>
	Vector(const Vector<T2> &v) : x(v.x), y(v.y), z(v.z) { }

	T length   () const { return std::sqrt(length2()); }
	T length2  () const { return x*x + y*y + z*z; }
	T distance (const Vector &v) const { return (*this - v).length (); }
	T distance2(const Vector &v) const { return (*this - v).length2(); }
	void normalize () { *this *= 1 / length(); }


	Vector &operator+=(const Vector &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector &operator-=(const Vector &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector &operator*=(T r) {
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}

	Vector &operator/=(T r) {
		return *this *= 1 / r;
	}

	Vector  operator- () const {
		return Vector(-x, -y, -z);
	}

	Vector  operator+ () const {
		return *this;
	}
};

template<class T>
inline Vector<T> operator+(Vector<T> a, const Vector<T> &b) { return a += b; }
template<class T>
inline Vector<T> operator-(Vector<T> a, const Vector<T> &b) { return a -= b; }
template<class T>
inline Vector<T> operator*(Vector<T> a, T r) { return a *= r; }
template<class T>
inline Vector<T> operator*(T r, Vector<T> a) { return a *= r; }
template<class T>
inline Vector<T> operator/(Vector<T> a, T r) { return a /= r; }


}

}

#endif
