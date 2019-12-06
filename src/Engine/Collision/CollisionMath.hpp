#ifndef COLLISION_MATH_H
#define COLLISION_MATH_H

#include "GameMath.hpp"

using namespace math;

// acceleration at time t+dt in terms of r(t+dt)
typedef scalar(*forcingFunction) (scalar);
typedef scalar(*forcingFunctionVec3) (vec3);

class VerletIntegrator {
public:
    VerletIntegrator();
    VerletIntegrator(scalar X0);
    VerletIntegrator(scalar X0, scalar Xdot0);
    VerletIntegrator(scalar X0, scalar Xdot0, scalar Xddot0);

    void Step(double dt, scalar a2 = 0.0f);
    void Step(double dt, forcingFunction A);

    void PrintState(double t);
    void PrintState();

private:
    scalar x;
    scalar x_dot;
    scalar x_ddot;
};

class VerletVec3 {
public:
    VerletVec3();
    VerletVec3(vec3 X0);
    VerletVec3(vec3 X0, vec3 Xdot0);
    VerletVec3(vec3 X0, vec3 Xdot0, vec3 Xddot0);

    void Step(double dt, vec3 a2 = vec3());
    void Step(double dt, forcingFunctionVec3 A);

    void PrintState(double t);
    void PrintState();

private:
    vec3 x;
    vec3 x_dot;
    vec3 x_ddot;
};

#endif
