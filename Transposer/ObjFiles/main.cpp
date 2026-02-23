#include <iostream>
#include <memory>
#include <vector>

#include "Array.h"
#include "Object.h"
#include "Primitive.h"
#include "SafePtr.h"
#include "String.h"

#define CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

void pascal(Primitive<unsigned int> size);
/*
void start()
{
    Array<Object> objects(Primitive<unsigned int>(3));
    Array<Object> subObjects(Primitive<unsigned int>(2));
    Array<Primitive<unsigned int>> t(Primitive<unsigned int>(2), SafePtr(Primitive<unsigned int>(0)));
    std::cout << t << std::endl;

    subObjects[0] = Primitive<int>(3);
    subObjects[1] = Primitive<bool>(false);

    objects[0] = String("Hello World!");
    objects[1] = String("Goodbye World!");
    objects[2] = subObjects;

    Utils::cast<Array<Object>>(objects[2])[0] = Primitive<int>(1);

    //Utils::cast<Primitive<int>>(objects[0]);

    std::cout << objects << std::endl;

    subObjects[0] = Primitive<int>(1);

    std::cout << (subObjects[0] == Primitive<int>(1) ? "true" : "false") << std::endl;

    SafePtr<Primitive<int>> primitive(Primitive<int>(4));
    std::cout << primitive << std::endl;

    if (Primitive<bool>(false) || Primitive<bool>(true)) std::cout << "true" << std::endl;
    else std::cout << "false" << std::endl;

    std::cout << objects[2][0] << std::endl;
}
*/

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    try
    {
        //start();
        pascal(Primitive<unsigned int>(10));
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    if constexpr (_CrtDumpMemoryLeaks())
    {
        std::cout << "LEAK!! :(" << std::endl;
    }
    else
    {
        std::cout << "no leaks :)" << std::endl;
    }

    return 0;
}


void pascal(Primitive<unsigned int> size)
{
    Array<Array<Primitive<unsigned int>>> arr = Array<Array<Primitive<unsigned int>>>(size, Array<Primitive<unsigned int>>(size, Primitive<unsigned int>()));
    arr[Primitive<int>(0)][Primitive<int>(0)] = Primitive<int>(1);
    for (Primitive<int> i = Primitive<int>(1); i < size; i += Primitive<int>(1))
    {
        for (Primitive<int> j = Primitive<int>(0); j < size; j += Primitive<int>(1))
        {
            if (j == Primitive<int>(0))
            {
                arr[i][j] = arr[i - Primitive<int>(1)][j];
            }
            else
            {
                arr[i][j] = arr[i - Primitive<int>(1)][j - Primitive<int>(1)] + arr[i - Primitive<int>(1)][j];
            }
        }
    }
    for (Primitive<int> i = Primitive<int>(0); i < size; i += Primitive<int>(1))
    {
        std::cout << String("[");
        for (Primitive<int> j = Primitive<int>(0); j < size; j += Primitive<int>(1))
        {
            std::cout << String("[") << arr[i][j] << String("]");
            if (j != size - Primitive<int>(1))
            {
                std::cout << String(", ");
            }
        }
        String b = String(String("]"));
        if (i != size - Primitive<int>(1))
        {
            b += String(", ");
        }
        std::cout << b << std::endl;
    }
}

