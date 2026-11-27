#include "heist.h"
// Main is the driver file. It starts the single GameEngine object
// and catches any exception that was not handled inside the game.
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

    system("pause");
    return 0;
}
