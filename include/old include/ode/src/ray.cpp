/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001-2003 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

/*

standard ODE geometry primitives: public API and pairwise collision functions.

the rule is that only the low level primitive collision functions should set
dContactGeom::g1 and dContactGeom::g2.

*/

#include <ode/common.h>
#include <ode/collision.h>
#include <ode/matrix.h>
#include <ode/rotation.h>
#include <ode/odemath.h>
#include "collision_kernel.h"
#include "collision_std.h"
#include "collision_util.h"

#ifdef _MSC_VER
#pragma warning(disable:4291)  // for VC++, no complaints about "no matching operator delete found"
#endif

//****************************************************************************
// ray public API

dxRay::dxRay (dSpaceID space, dReal _length) : dxGeom (space,1)
{
  type = dRayClass;
  length = _length;
}


void dxRay::computeAABB()
{
  dVector3 e;
  e[0] = final_posr->pos[0] + final_posr->R[0*4+2]*length;
  e[1] = final_posr->pos[1] + final_posr->R[1*4+2]*length;
  e[2] = final_posr->pos[2] + final_posr->R[2*4+2]*length;

  if (final_posr->pos[0] < e[0]){
    aabb[0] = final_posr->pos[0];
    aabb[1] = e[0];
  }
  else{
    aabb[0] = e[0];
    aabb[1] = final_posr->pos[0];
  }
  
  if (final_posr->pos[1] < e[1]){
    aabb[2] = final_posr->pos[1];
    aabb[3] = e[1];
  }
  else{
    aabb[2] = e[1];
    aabb[3] = final_posr->pos[1];
  }

  if (final_posr->pos[2] < e[2]){
    aabb[4] = final_posr->pos[2];
    aabb[5] = e[2];
  }
  else{
    aabb[4] = e[2];
    aabb[5] = final_posr->pos[2];
  }
}


dGeomID dCreateRay (dSpaceID space, dReal length)
{
  return new dxRay (space,length);
}


void dGeomRaySetLength (dGeomID g, dReal length)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");
  dxRay *r = (dxRay*) g;
  r->length = length;
  dGeomMoved (g);
}


dReal dGeomRayGetLength (dGeomID g)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");
  dxRay *r = (dxRay*) g;
  return r->length;
}


void dGeomRaySet (dGeomID g, dReal px, dReal py, dReal pz,
		  dReal dx, dReal dy, dReal dz)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");
  g->recomputePosr();
  dReal* rot = g->final_posr->R;
  dReal* pos = g->final_posr->pos;
  dVector3 n;
  pos[0] = px;
  pos[1] = py;
  pos[2] = pz;

  n[0] = dx;
  n[1] = dy;
  n[2] = dz;
  dNormalize3(n);
  rot[0*4+2] = n[0];
  rot[1*4+2] = n[1];
  rot[2*4+2] = n[2];
  dGeomMoved (g);
}


void dGeomRayGet (dGeomID g, dVector3 start, dVector3 dir)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");
  g->recomputePosr();
  start[0] = g->final_posr->pos[0];
  start[1] = g->final_posr->pos[1];
  start[2] = g->final_posr->pos[2];
  dir[0] = g->final_posr->R[0*4+2];
  dir[1] = g->final_posr->R[1*4+2];
  dir[2] = g->final_posr->R[2*4+2];
}


void dGeomRaySetParams (dxGeom *g, int FirstContact, int BackfaceCull)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");

  if (FirstContact){
    g->gflags |= RAY_FIRSTCONTACT;
  }
  else g->gflags &= ~RAY_FIRSTCONTACT;

  if (BackfaceCull){
    g->gflags |= RAY_BACKFACECULL;
  }
  else g->gflags &= ~RAY_BACKFACECULL;
}


void dGeomRayGetParams (dxGeom *g, int *FirstContact, int *BackfaceCull)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");

  (*FirstContact) = ((g->gflags & RAY_FIRSTCONTACT) != 0);
  (*BackfaceCull) = ((g->gflags & RAY_BACKFACECULL) != 0);
}


void dGeomRaySetClosestHit (dxGeom *g, int closestHit)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");
  if (closestHit){
    g->gflags |= RAY_CLOSEST_HIT;
  }
  else g->gflags &= ~RAY_CLOSEST_HIT;
}


int dGeomRayGetClosestHit (dxGeom *g)
{
  dUASSERT (g && g->type == dRayClass,"argument not a ray");
  return ((g->gflags & RAY_CLOSEST_HIT) != 0);
}



// if mode==1 then use the sphere exit contact, not the entry contact

static int ray_sphere_helper (dxRay *ray, dVector3 sphere_pos, dReal radius,
			      dContactGeom *contact, int mode)
{
  dVector3 q;
  q[0] = ray->final_posr->pos[0] - sphere_pos[0];
  q[1] = ray->final_posr->pos[1] - sphere_pos[1];
  q[2] = ray->final_posr->pos[2] - sphere_pos[2];
  dReal B = dDOT14(q,ray->final_posr->R+2);
  dReal C = dDOT(q,q) - radius*radius;
  // note: if C <= 0 then the start of the ray is inside the sphere
  dReal k = B*B - C;
  if (k < 0) return 0;
  k = dSqrt(k);
  dReal alpha;
  if (mode && C >= 0) {
    alpha = -B + k;
    if (alpha < 0) return 0;
  }
  else {
    alpha = -B - k;
    if (alpha < 0) {
      alpha = -B + k;
      if (alpha < 0) return 0;
    }
  }
  if (alpha > ray->length) return 0;
  contact->pos[0] = ray->final_posr->pos[0] + alpha*ray->final_posr->R[0*4+2];
  contact->pos[1] = ray->final_posr->pos[1] + alpha*ray->final_posr->R[1*4+2];
  contact->pos[2] = ray->final_posr->pos[2] + alpha*ray->final_posr->R[2*4+2];
  dReal nsign = (C < 0 || mode) ? REAL(-1.0) : REAL(1.0);
  contact->normal[0] = nsign*(contact->pos[0] - sphere_pos[0]);
  contact->normal[1] = nsign*(contact->pos[1] - sphere_pos[1]);
  contact->normal[2] = nsign*(contact->pos[2] - sphere_pos[2]);
  dNormalize3 (contact->normal);
  contact->depth = alpha;
  return 1;
}


int dCollideRaySphere (dxGeom *o1, dxGeom *o2, int flags,
		       dContactGeom *contact, int skip)
{
  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->type == dRayClass);
  dIASSERT (o2->type == dSphereClass);
  dxRay *ray = (dxRay*) o1;
  dxSphere *sphere = (dxSphere*) o2;
  contact->g1 = ray;
  contact->g2 = sphere;
  return ray_sphere_helper (ray,sphere->final_posr->pos,sphere->radius,contact,0);
}


int dCollideRayBox (dxGeom *o1, dxGeom *o2, int flags,
		    dContactGeom *contact, int skip)
{
  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->type == dRayClass);
  dIASSERT (o2->type == dBoxClass);
  dxRay *ray = (dxRay*) o1;
  dxBox *box = (dxBox*) o2;

  contact->g1 = ray;
  contact->g2 = box;

  int i;

  // compute the start and delta of the ray relative to the box.
  // we will do all subsequent computations in this box-relative coordinate
  // system. we have to do a translation and rotation for each point.
  dVector3 tmp,s,v;
  tmp[0] = ray->final_posr->pos[0] - box->final_posr->pos[0];
  tmp[1] = ray->final_posr->pos[1] - box->final_posr->pos[1];
  tmp[2] = ray->final_posr->pos[2] - box->final_posr->pos[2];
  dMULTIPLY1_331 (s,box->final_posr->R,tmp);
  tmp[0] = ray->final_posr->R[0*4+2];
  tmp[1] = ray->final_posr->R[1*4+2];
  tmp[2] = ray->final_posr->R[2*4+2];
  dMULTIPLY1_331 (v,box->final_posr->R,tmp);

  // mirror the line so that v has all components >= 0
  dVector3 sign;
  for (i=0; i<3; i++) {
    if (v[i] < 0) {
      s[i] = -s[i];
      v[i] = -v[i];
      sign[i] = 1;
    }
    else sign[i] = -1;
  }

  // compute the half-sides of the box
  dReal h[3];
  h[0] = REAL(0.5) * box->side[0];
  h[1] = REAL(0.5) * box->side[1];
  h[2] = REAL(0.5) * box->side[2];

  // do a few early exit tests
  if ((s[0] < -h[0] && v[0] <= 0) || s[0] >  h[0] ||
      (s[1] < -h[1] && v[1] <= 0) || s[1] >  h[1] ||
      (s[2] < -h[2] && v[2] <= 0) || s[2] >  h[2] ||
      (v[0] == 0 && v[1] == 0 && v[2] == 0)) {
    return 0;
  }

  // compute the t=[lo..hi] range for where s+v*t intersects the box
  dReal lo = -dInfinity;
  dReal hi = dInfinity;
  int nlo = 0, nhi = 0;
  for (i=0; i<3; i++) {
    if (v[i] != 0) {
      dReal k = (-h[i] - s[i])/v[i];
      if (k > lo) {
	lo = k;
	nlo = i;
      }
      k = (h[i] - s[i])/v[i];
      if (k < hi) {
	hi = k;
	nhi = i;
      }
    }
  }

  // check if the ray intersects
  if (lo > hi) return 0;
  dReal alpha;
  int n;
  if (lo >= 0) {
    alpha = lo;
    n = nlo;
  }
  else {
    alpha = hi;
    n = nhi;
  }
  if (alpha < 0 || alpha > ray->length) return 0;
  contact->pos[0] = ray->final_posr->pos[0] + alpha*ray->final_posr->R[0*4+2];
  contact->pos[1] = ray->final_posr->pos[1] + alpha*ray->final_posr->R[1*4+2];
  contact->pos[2] = ray->final_posr->pos[2] + alpha*ray->final_posr->R[2*4+2];
  contact->normal[0] = box->final_posr->R[0*4+n] * sign[n];
  contact->normal[1] = box->final_posr->R[1*4+n] * sign[n];
  contact->normal[2] = box->final_posr->R[2*4+n] * sign[n];
  contact->depth = alpha;
  return 1;
}


int dCollideRayCapsule (dxGeom *o1, dxGeom *o2,
			  int flags, dContactGeom *contact, int skip)
{
  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->type == dRayClass);
  dIASSERT (o2->type == dCapsuleClass);
  dxRay *ray = (dxRay*) o1;
  dxCapsule *ccyl = (dxCapsule*) o2;

  contact->g1 = ray;
  contact->g2 = ccyl;
  dReal lz2 = ccyl->lz * REAL(0.5);

  // compute some useful info
  dVector3 cs,q,r;
  dReal C,k;
  cs[0] = ray->final_posr->pos[0] - ccyl->final_posr->pos[0];
  cs[1] = ray->final_posr->pos[1] - ccyl->final_posr->pos[1];
  cs[2] = ray->final_posr->pos[2] - ccyl->final_posr->pos[2];
  k = dDOT41(ccyl->final_posr->R+2,cs);	// position of ray start along ccyl axis
  q[0] = k*ccyl->final_posr->R[0*4+2] - cs[0];
  q[1] = k*ccyl->final_posr->R[1*4+2] - cs[1];
  q[2] = k*ccyl->final_posr->R[2*4+2] - cs[2];
  C = dDOT(q,q) - ccyl->radius*ccyl->radius;
  // if C < 0 then ray start position within infinite extension of cylinder

  // see if ray start position is inside the capped cylinder
  int inside_ccyl = 0;
  if (C < 0) {
    if (k < -lz2) k = -lz2;
    else if (k > lz2) k = lz2;
    r[0] = ccyl->final_posr->pos[0] + k*ccyl->final_posr->R[0*4+2];
    r[1] = ccyl->final_posr->pos[1] + k*ccyl->final_posr->R[1*4+2];
    r[2] = ccyl->final_posr->pos[2] + k*ccyl->final_posr->R[2*4+2];
    if ((ray->final_posr->pos[0]-r[0])*(ray->final_posr->pos[0]-r[0]) +
	(ray->final_posr->pos[1]-r[1])*(ray->final_posr->pos[1]-r[1]) +
	(ray->final_posr->pos[2]-r[2])*(ray->final_posr->pos[2]-r[2]) < ccyl->radius*ccyl->radius) {
      inside_ccyl = 1;
    }
  }

  // compute ray collision with infinite cylinder, except for the case where
  // the ray is outside the capped cylinder but within the infinite cylinder
  // (it that case the ray can only hit endcaps)
  if (!inside_ccyl && C < 0) {
    // set k to cap position to check
    if (k < 0) k = -lz2; else k = lz2;
  }
  else {
    dReal uv = dDOT44(ccyl->final_posr->R+2,ray->final_posr->R+2);
    r[0] = uv*ccyl->final_posr->R[0*4+2] - ray->final_posr->R[0*4+2];
    r[1] = uv*ccyl->final_posr->R[1*4+2] - ray->final_posr->R[1*4+2];
    r[2] = uv*ccyl->final_posr->R[2*4+2] - ray->final_posr->R[2*4+2];
    dReal A = dDOT(r,r);
    dReal B = 2*dDOT(q,r);
    k = B*B-4*A*C;
    if (k < 0) {
      // the ray does not intersect the infinite cylinder, but if the ray is
      // inside and parallel to the cylinder axis it may intersect the end
      // caps. set k to cap position to check.
      if (!inside_ccyl) return 0;
      if (uv < 0) k = -lz2; else k = lz2;
    }
    else {
      k = dSqrt(k);
      A = dRecip (2*A);
      dReal alpha = (-B-k)*A;
      if (alpha < 0) {
	alpha = (-B+k)*A;
	if (alpha < 0) return 0;
      }
      if (alpha > ray->length) return 0;

      // the ray intersects the infinite cylinder. check to see if the
      // intersection point is between the caps
      contact->pos[0] = ray->final_posr->pos[0] + alpha*ray->final_posr->R[0*4+2];
      contact->pos[1] = ray->final_posr->pos[1] + alpha*ray->final_posr->R[1*4+2];
      contact->pos[2] = ray->final_posr->pos[2] + alpha*ray->final_posr->R[2*4+2];
      q[0] = contact->pos[0] - ccyl->final_posr->pos[0];
      q[1] = contact->pos[1] - ccyl->final_posr->pos[1];
      q[2] = contact->pos[2] - ccyl->final_posr->pos[2];
      k = dDOT14(q,ccyl->final_posr->R+2);
      dReal nsign = inside_ccyl ? REAL(-1.0) : REAL(1.0);
      if (k >= -lz2 && k <= lz2) {
	contact->normal[0] = nsign * (contact->pos[0] -
				      (ccyl->final_posr->pos[0] + k*ccyl->final_posr->R[0*4+2]));
	contact->normal[1] = nsign * (contact->pos[1] -
				      (ccyl->final_posr->pos[1] + k*ccyl->final_posr->R[1*4+2]));
	contact->normal[2] = nsign * (contact->pos[2] -
				      (ccyl->final_posr->pos[2] + k*ccyl->final_posr->R[2*4+2]));
	dNormalize3 (contact->normal);
	contact->depth = alpha;
	return 1;
      }

      // the infinite cylinder intersection point is not between the caps.
      // set k to cap position to check.
      if (k < 0) k = -lz2; else k = lz2;
    }
  }

  // check for ray intersection with the caps. k must indicate the cap
  // position to check
  q[0] = ccyl->final_posr->pos[0] + k*ccyl->final_posr->R[0*4+2];
  q[1] = ccyl->final_posr->pos[1] + k*ccyl->final_posr->R[1*4+2];
  q[2] = ccyl->final_posr->pos[2] + k*ccyl->final_posr->R[2*4+2];
  return ray_sphere_helper (ray,q,ccyl->radius,contact, inside_ccyl);
}


int dCollideRayPlane (dxGeom *o1, dxGeom *o2, int flags,
		      dContactGeom *contact, int skip)
{
  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->type == dRayClass);
  dIASSERT (o2->type == dPlaneClass);
  dxRay *ray = (dxRay*) o1;
  dxPlane *plane = (dxPlane*) o2;

  dReal alpha = plane->p[3] - dDOT (plane->p,ray->final_posr->pos);
  // note: if alpha > 0 the starting point is below the plane
  dReal nsign = (alpha > 0) ? REAL(-1.0) : REAL(1.0);
  dReal k = dDOT14(plane->p,ray->final_posr->R+2);
  if (k==0) return 0;		// ray parallel to plane
  alpha /= k;
  if (alpha < 0 || alpha > ray->length) return 0;
  contact->pos[0] = ray->final_posr->pos[0] + alpha*ray->final_posr->R[0*4+2];
  contact->pos[1] = ray->final_posr->pos[1] + alpha*ray->final_posr->R[1*4+2];
  contact->pos[2] = ray->final_posr->pos[2] + alpha*ray->final_posr->R[2*4+2];
  contact->normal[0] = nsign*plane->p[0];
  contact->normal[1] = nsign*plane->p[1];
  contact->normal[2] = nsign*plane->p[2];
  contact->depth = alpha;
  contact->g1 = ray;
  contact->g2 = plane;
  return 1;
}
