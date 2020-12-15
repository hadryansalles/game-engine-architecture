#include "Tests.h"

void TestVec3() {

    Vec3 zero;
    Vec3 one(1);
    Vec3 oneTwoThree(1, 2, 3);
    
    Vec3 up(0, 1, 0);
    Vec3 right(1, 0, 0);
    Vec3 front(0, 0, 1);

    std::cout << oneTwoThree << " + " << 1 << " = " << oneTwoThree + 1 << '\n';

    std::cout << oneTwoThree << " - " << 1 << " = " << oneTwoThree - 1 << '\n';

    std::cout << oneTwoThree << " * " << 3 << " = " << oneTwoThree * 3 << '\n';

    std::cout << oneTwoThree << " / " << 3 << " = " << oneTwoThree / 3 << '\n';

    std::cout << oneTwoThree << ".Mag = " << oneTwoThree.Mag() << '\n';

    std::cout << oneTwoThree << ".Mag2 = " << oneTwoThree.Mag2() << '\n';

    std::cout << oneTwoThree << " Dot " << oneTwoThree << " = " << oneTwoThree.Dot(oneTwoThree) << '\n';

    std::cout << right << " Cross " << up << " = " << right.Cross(up) << '\n';

    std::cout << oneTwoThree << " Normalized = " << oneTwoThree/oneTwoThree.Mag() << '\n';

    std::cout << oneTwoThree << " at i = 1 is " << oneTwoThree.g << '\n';
}