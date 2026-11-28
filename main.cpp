
#include "heist.h"

int main()
{
    try
    {
        GameEngine::getInstance().run();
    }
    catch (const GameException& error)
    {
        cout << error.getMessage() << endl;
    }
    catch (...)
    {
        cout << "HEIST ERROR: An unexpected error occurred." << endl;
    }

    GameEngine::destroyInstance();

    system("pause");
    return 0;
}
