/*
 * Copyright (c) 2007, Laminar Research.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef TensorUtils_H
#define TensorUtils_H

#include "CompGeomDefs2.h"

/*

	A 2-d tensor T is a matrix of the form
	  [ cos 2T  sin2T ]
	R [ sin 2T -cos2T ]

	Where theta (T) represents the rotation of the vector (1,0) CCW to get the normalized major Eigenvector, and R is a magnitude.
	Whoa.  Okay...really what we're saying is:

	- A tensor is a 2-d matrix and therefore represents an affine transform operation.
	- The eigenvectors of that transform represent the "flow" of the distorted space.

	We will represent our tensors via two quantities which we can stash in a vector:

	v.dx = R * cos2T
	v.dy = R * sin2T

	Thus we can recover R and T if needed.  (If the tensor's weight R was 1 we could recover sin2T from cos2T but in practice
	the weighting matters a lot.0

	Tensors are ADDITIVE - that is, we can sum v = v1 + v2 to merge tensors.

	A tensor FIELD is essentially a plane where the tensor's value changes with XY.  How it changes depends on the field (maybe only the magnitude
	changes, or maybe the tensor changes).



*/

inline Vector2		Tensor2Eigen(const Vector2& t);
inline Vector2		Eigen2Tensor(const Vector2& e);	// E must be normalized!
inline Vector2		Gradient2Tensor(const Vector2& g);

inline Vector2		Linear_Tensor(
					const Point2&	origin,
					const Vector2&	direction,	// MUST BE NORMALIZED
					double			decay,
					const Point2&	p);

inline Vector2		Radial_Tensor(
					const Point2&	origin,
					double			decay,
					const Point2&	p);

#if 0
inline Vector2		Weighted_Tensor(
					const Vector2 *	tensors,		// Tensor value for each pt
					const double *	weights,		// Weight of each pt
					int				n);

inline void		Normalize_Weights(
					double *		weights,
					int				n);

inline void		Polygon_Weights(
					const Point2 *	polygon,
					double *		weights,
					int				n,
					const			Point2& p);
#endif


inline Vector2		Tensor2Eigen(const Vector2& t)
{
	// First, normalize the tensor field - the weighting doesn't affect the eigenvector,
	// and we need the quantities sin(2T) and cos(2T) directly to apply the half-angle formulas..
	Vector2	tt(t);
	tt.normalize();
	// Half-angle formulas reveal sin(T) and cos(T), which are the eigenvector!

	double cos_t = sqrt(0.5 + tt.dx*0.5);
	double sin_t = sqrt(0.5 - tt.dx*0.5);

	// Half-angle formula is sin-ambiguous.  But tensors are also bi-directional so
	// we don't terribly care if our angle is simply inverted (sin is negated) -
	// for the range of values we get back (-180 to 180) it doesn't matter.
	// We DO care if the cosine is inverted..fortunately the sine of the double
	// angle goes negative when the cosine is negative for us.  (Sine is negative
	// from 180..360 and cosine is negative from 90..180.)

	if(tt.dy < 0.0)				  cos_t = -cos_t;

	return Vector2(cos_t, sin_t);
}

inline Vector2		Eigen2Tensor(const Vector2& e)
{
	// Use the double-angle formula to find cos(2T), sin(2T) from cos(T) and sin(T)
	return Vector2(1.0 - 2.0 * e.dy * e.dy, 2.0 * e.dx * e.dy);
}

inline Vector2		Gradient2Tensor(const Vector2& g)
{
	// Normally R = length(g) and theta = atan(g.dy/g.dx) and then we can build our canonical tensor.
	// But in this case, we'll find the eigenvector directly because we can stay in trig-space.

	double slen = g.squared_length();
	if (slen <= 0.0) return Vector2(0.0,0.0);		// Degenerate!
	double	mag = sqrt(slen);	// Magnitude R of gradient is magnitude of tensor field!

	// Find sin and cos of theta ... directly from the gradient, don't bother with atan.  Avoid round-trip trig!

	double sin_theta = g.dx / mag;		// Note that we have rotated CCW by 90 degrees
	double cos_theta = -g.dy / mag;		// so that the major eigenvector is PERPENDICULAR to the gradient!  That way we run ALONG a contour line, not UP the slope!

	return Eigen2Tensor(Vector2(cos_theta, sin_theta)) * mag;
}

inline Vector2 Linear_Tensor(
					const Point2&	o,	// origin
					const Vector2&	v,	// dir
					double			d,	// decay
					const Point2&	p)
{
	// Theta of tensor is atan(d.dy / d.dx)
	// So .... actually we can just call Eigen2Tensor on V!
	// magnitude is e^(-d * ||o-p||^2), which is just a decay functionDEM_NO_DATA

//	return Eigen2Tensor(v) * exp(-d * Vector2(o,p).squared_length());
	return Eigen2Tensor(v) / (d * Vector2(o,p).squared_length());
}


inline Vector2		Radial_Tensor(
					const Point2&	o,	// origin
					double			d,	// decay
					const Point2&	p)
{
//	cos2T = y^2 - x^2
//	sin2T = -2XY
//	R = e^-d sqlen(O,P)

	Vector2	dv(o,p);

	return Vector2(dv.dy * dv.dy - dv.dx * dv.dx, -2.0 * dv.dx * dv.dy) * exp(-d * dv.squared_length());
}

#if 0
inline Vector2		Weighted_Tensor(
					const Vector2 *	tensors,		// Tensor value for each pt
					const double *	weights,		// Weight of each pt
					int				n)
{
	Vector2	t(0.0,0.0);
	while(n--)
	{
		t += ((*tensors) * (*weights));
		++tensors;
		++weights;
	}
	return t;

}

inline void		Normalize_Weights(
					double *		weights,
					int				n)
{
	double w = 0.0;
	double * p = weights;
	double c = n;
	while(c--)
		w += *p++;
	if (w == 0.0)	return;
	w = 1.0 / w;
	c = n;
	p = weights;
	while(c--)
		*p++ *= w;
}

inline double cotangent(const Point2& a, const Point2& b, const Point2& c)
{
	Vector2 ba(b,a);
	Vector2 bc(b,c);

	double	ba_x_bc = ba.dx * bc.dy - ba.dy * bc.dx;
	return ba.dot(bc) / (ba_x_bc * ba_x_bc);
}



inline void		Polygon_Weights(
					const Point2 *	pgon,
					double *		weights,
					int				n,
					const Point2&	p)
{
	// Compute the barycentric weights for a point p in an n-gon Q
	// Assumes p is strictly within Q and the vertices q
	for (int q = 0; q < n; ++q)
	{
		int prev = (q + n - 1) % n;
		int next = (q     + 1) % n;

		weights[q] = (cotangent(p,pgon[q],pgon[prev]) + cotangent(p,pgon[q],pgon[next])) / Vector2(p,pgon[q]).squared_length();
	}
	Normalize_Weights(weights,n);
}

#endif


#endif /* TensorUtils_H */
