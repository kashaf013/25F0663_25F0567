#include "heist.h"
#include "game.h"
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
       try
    {
        Tile tileA('A', true);
        Tile tileB('B', true);

        cout << "Tile A: " << tileA << endl;
        cout << "A equals B: " << (tileA == tileB ? "Yes" : "No") << endl;
        cout << "A is less than B: " << (tileA < tileB ? "Yes" : "No") << endl;

        Location study("Study Room", 'S', 3, Word1, false);
        cout << "\n" << study << endl;

        TileRack rack1(8);
        rack1.addLetter('H');
        rack1.addLetter('E');
        rack1.addLetter('I');

        TileRack rack2(rack1);
        rack2.addLetter('S');
        rack2.addLetter('T');

        cout << "\nOriginal rack: " << rack1 << endl;
        cout << "Copied rack: " << rack2 << endl;

        TileRack merged = rack1 + rack2;
        cout << "Merged rack: " << merged << endl;

        rack2.removeLetter(0);
        cout << "Copied rack after removal: " << rack2 << endl;
        cout << "Original rack remains: " << rack1 << endl;

        Victim victim;
        Partner partner1("Ms.Rulana", "Partner 1", "Ukrainian");
        Partner partner2("Mr.Dmitry", "Partner 2", "Russian");
        PA assistant;
        Cook cook;
        Guard guard;
        Servant servant;
        Child child("Ms.Celine", "French", 17);
        Detective detective("Agent_Halim");

        Character* people[9] =
        {
            &victim, &partner1, &partner2, &assistant,
            &cook, &guard, &servant, &child, &detective
        };

        cout << "\n===== POLYMORPHISM TEST =====" << endl;

        for (int i = 0; i < 9; i++)
        {
            people[i]->display();
        }

        cout << "\n===== INTERVIEW CONDITIONS =====" << endl;
        cout << "Victim at 80: "
            << (victim.canBeInterviewed(80) ? "Allowed" : "Not allowed") << endl;
        cout << "Partner at 50: "
            << (partner1.canBeInterviewed(50) ? "Allowed" : "Not allowed") << endl;
        cout << "PA at 35: "
            << (assistant.canBeInterviewed(35) ? "Allowed" : "Not allowed") << endl;
        cout << "Cook at 0: "
            << (cook.canBeInterviewed(0) ? "Allowed" : "Not allowed") << endl;
        cout << "Child at 75: "
            << (child.canBeInterviewed(75) ? "Allowed" : "Not allowed") << endl;

        detective.getRack().addLetter('X');
        detective.setTurns(2);
        detective.changeCredibility(-10);
        detective.display();
    }
    catch (const exception& error)
    {
        cout << "Error: " << error.what() << endl;
    }
    system("pause");
    return 0;
}
