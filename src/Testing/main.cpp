#include "Collision/CollisionWorld.hpp"
#include "Collision/CollisionHull.hpp"
#include "Collision/CollisionMath.hpp"

scalar springMassAcc(scalar x) {
    scalar k = 1.0;
    scalar m = 1.0;

    return (-k * x) / m;
}

int main(int argc, char* argv[]) {
    double t = 0.0;
    double dt = .02;

    VerletIntegrator position(4, 0, springMassAcc(4));

    double tEnd = 10.0;
    do {
        position.PrintState(t);

        position.Step(dt, springMassAcc);

        t += dt;
    } while (t <= tEnd+dt);

    system("pause");

    return 0;
}