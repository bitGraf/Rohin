#include "rhpch.hpp"
#include "CollisionMath.hpp"

/* Initialisors */
VerletIntegrator::VerletIntegrator() :
    x(0),
    x_dot(0),
    x_ddot(0)
{}
VerletIntegrator::VerletIntegrator(scalar X0) :
    x(X0),
    x_dot(0),
    x_ddot(0)
{}
VerletIntegrator::VerletIntegrator(scalar X0, scalar Xdot0) :
    x(X0),
    x_dot(Xdot0),
    x_ddot(0)
{}
VerletIntegrator::VerletIntegrator(scalar X0, scalar Xdot0, scalar Xddot0) :
    x(X0),
    x_dot(Xdot0),
    x_ddot(Xddot0)
{}

void VerletIntegrator::Step(double dt, scalar a2) {
    // Using Velocity Verlet method

    // 1. Calc r(t+dt)
    scalar x_ = x + x_dot * dt + .5*x_ddot*dt*dt;
    // 2. Calc v(t+.5dt)
    scalar v_2 = x_dot + .5*x_ddot*dt;
    // 3. Calc a(t+dt)
    scalar a_ = a2;
    // 4. calc v(1+dt)
    scalar v_ = v_2 + .5*a_*dt;

    x = x_;
    x_dot = v_;
    x_ddot = a_;
}

void VerletIntegrator::Step(double dt, forcingFunction A) {
    // Using Velocity Verlet method

    // 1. Calc r(t+dt)
    scalar x_ = x + x_dot * dt + .5*x_ddot*dt*dt;
    // 2. Calc v(t+.5dt)
    scalar v_2 = x_dot + .5*x_ddot*dt;
    // 3. Calc a(t+dt)
    scalar a_ = A(x_);
    // 4. calc v(1+dt)
    scalar v_ = v_2 + .5*a_*dt;

    x = x_;
    x_dot = v_;
    x_ddot = a_;
}

void VerletIntegrator::PrintState(double t) {
    printf("[%-6.3f] X=%-5.3f  V=%-5.3f  A=%-5.3f\n", t, x, x_dot, x_ddot);
}

void VerletIntegrator::PrintState() {
    printf("X=%-5.3f  V=%-5.3f  A=%-5.3f\n", x, x_dot, x_ddot);
}



/* Initialisors */
VerletVec3::VerletVec3() :
    x(),
    x_dot(),
    x_ddot()
{}
VerletVec3::VerletVec3(vec3 X0) :
    x(X0),
    x_dot(),
    x_ddot()
{}
VerletVec3::VerletVec3(vec3 X0, vec3 Xdot0) :
    x(X0),
    x_dot(Xdot0),
    x_ddot()
{}
VerletVec3::VerletVec3(vec3 X0, vec3 Xdot0, vec3 Xddot0) :
    x(X0),
    x_dot(Xdot0),
    x_ddot(Xddot0)
{}

void VerletVec3::Step(double dt, vec3 a2) {
    // Using Velocity Verlet method

    // 1. Calc r(t+dt)
    vec3 x_ = x + x_dot * dt + .5*x_ddot*dt*dt;
    // 2. Calc v(t+.5dt)
    vec3 v_2 = x_dot + .5*x_ddot*dt;
    // 3. Calc a(t+dt)
    vec3 a_ = a2;
    // 4. calc v(1+dt)
    vec3 v_ = v_2 + .5*a_*dt;

    x = x_;
    x_dot = v_;
    x_ddot = a_;
}

void VerletVec3::Step(double dt, forcingFunctionVec3 A) {
    // Using Velocity Verlet method

    // 1. Calc r(t+dt)
    vec3 x_ = x + x_dot * dt + .5*x_ddot*dt*dt;
    // 2. Calc v(t+.5dt)
    vec3 v_2 = x_dot + .5*x_ddot*dt;
    // 3. Calc a(t+dt)
    vec3 a_ = A(x_);
    // 4. calc v(1+dt)
    vec3 v_ = v_2 + .5*a_*dt;

    x = x_;
    x_dot = v_;
    x_ddot = a_;
}

void VerletVec3::PrintState(double t) {
    printf("[%-6.3f] X=(%-.2f,%-.2f,%-.2f)\n", t, x.x, x.y, x.z);
}

void VerletVec3::PrintState() {
    printf("X=(%-.2f,%-.2f,%-.2f)\n", x.x, x.y, x.z);
}