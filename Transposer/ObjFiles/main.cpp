#include <iostream>
#include <memory>
#include <vector>

#include "Array.h"
#include "Object.h"
#include "Primitive.h"
#include "String.h"

#define CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "SafePtr.h"
#include "Utils.h"

void pascal(Primitive<unsigned int> size);

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

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    try
    {
        start();
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
    Array<Array<Primitive<unsigned int>>> arr
        = Array<Array<Primitive<unsigned int>>>(size, Array<Primitive<unsigned int>>(size, Primitive<unsigned int>(0)));
    arr[0][0] = Primitive<unsigned int>(1);
    for (Primitive<int> i(1); i < size; i++)
    {
        for (Primitive<int> j; j < size; j++)
        {
            if (j == Primitive<int>(0))
            {
                arr[i][j] = arr[i - 1][j];
            }
            else
            {
                arr[i][j] = arr[i - 1][j - 1] + arr[i - 1][j];
            }
        }
    }
    for (Primitive<int> i; i < size; i++)
    {
        std::cout << "[";
        for (Primitive<int> j; j < size; j++)
        {
            std::cout << "[" << arr[i][j] << "]";
            if (j != size - 1)
            {
                std::cout << ", ";
            }
        }
        std::string b = "]";
        if (i != size - 1)
        {
            b += ", ";
        }
        std::cout << b << std::endl;
    }
}

