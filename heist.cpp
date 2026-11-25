#include "heist.h"

// ---------------- GameException ----------------

GameException::GameException(const string& msg)
{
    message = msg;
}

string GameException::getMessage() const
{
    return message;
}

// ---------------- Cottage ----------------

Cottage::Cottage() {}

Cottage::Cottage(const Cottage& other)
{
    LocCache = other.LocCache;
}

Cottage& Cottage::operator=(const Cottage& other)
{
    if (this != &other)
        LocCache = other.LocCache;
    return *this;
}

void Cottage::addLocation(const Location& location)
{
    LocCache.add(location);
}

int Cottage::getLocationCount() const
{
    return LocCache.size();
}

bool Cottage::isSearched(int index) const
{
    return LocCache[index].locSearchFlag;
}

Location& Cottage::getLocation(int index)
{
    return LocCache[index];
}

const Location& Cottage::getLocation(int index) const
{
    return LocCache[index];
}

void Cottage::displayLocations() const
{
    cout << "\n====== COTTAGE LOCATIONS ======\n";
    for (int i = 0; i < LocCache.size(); i++)
    {
        cout << "[" << i + 1 << "] " << LocCache[i].locName
             << " - " << (LocCache[i].locSearchFlag ? "Searched" : "Not searched")
             << "\n";
    }
}

// ---------------- Scrabble ----------------

Scrabble::Scrabble()
{
    // Utilize two independent dynamic rows for the two clue words.
    for (int i = 0; i < 7; i++)
        wordOne.add(Tile());

    for (int i = 0; i < 8; i++)
        wordTwo.add(Tile());
}

Scrabble::Scrabble(const Scrabble& other)
    : wordOne(other.wordOne), wordTwo(other.wordTwo),
      targetWordOne(other.targetWordOne), targetWordTwo(other.targetWordTwo)
{
    // Copy each row independently so both boards own their own tile arrays.
}

Scrabble& Scrabble::operator=(const Scrabble& other)
{
    // Replace the current board with an independent copy of the source board.
    if (this != &other)
    {
        wordOne = other.wordOne;
        wordTwo = other.wordTwo;
        targetWordOne = other.targetWordOne;
        targetWordTwo = other.targetWordTwo;
    }
    return *this;
}

void Scrabble::setTargetWords(const string& first, const string& second)
{
    if (first.length() != 7 || second.length() != 8)
        throw GameException("HEIST ERROR: Clue words must contain 7 and 8 letters.");

    targetWordOne = first;
    targetWordTwo = second;
}

void Scrabble::placeLetter(WordRow row, char letter, int position)
{
    DynamicArray<Tile>* selectedRow;

    if (row == Word1)
        selectedRow = &wordOne;
    else
        selectedRow = &wordTwo;

    if (position < 1 || position > selectedRow->size())
        throw GameException("HEIST ERROR: Invalid Scrabble board position.");

    if ((*selectedRow)[position - 1].filled)
        throw GameException("HEIST ERROR: This Scrabble position is already filled.");

    (*selectedRow)[position - 1].tileLetter = letter;
    (*selectedRow)[position - 1].filled = true;
}

bool Scrabble::isComplete(WordRow row) const
{
    const DynamicArray<Tile>& selectedRow = (row == Word1) ? wordOne : wordTwo;

    for (int i = 0; i < selectedRow.size(); i++)
        if (!selectedRow[i].filled)
            return false;

    return true;
}

bool Scrabble::bothRowsComplete() const
{
    return isComplete(Word1) && isComplete(Word2);
}

string Scrabble::revealWord(WordRow row) const
{
    const DynamicArray<Tile>& selectedRow = (row == Word1) ? wordOne : wordTwo;

    if (!isComplete(row))
        throw GameException("HEIST ERROR: Word is not complete yet.");

    string result = "";
    for (int i = 0; i < selectedRow.size(); i++)
        result += selectedRow[i].tileLetter;

    return result;
}

void Scrabble::displayBoard() const
{
    cout << "\n========== SCRABBLE BOARD ==========\n";
    cout << "Word 1: ";
    for (int i = 0; i < wordOne.size(); i++)
    {
        if (wordOne[i].filled)
            cout << "[" << wordOne[i].tileLetter << "] ";
        else
            cout << "[ ] ";
    }

    cout << "\nWord 2: ";
    for (int i = 0; i < wordTwo.size(); i++)
    {
        if (wordTwo[i].filled)
            cout << "[" << wordTwo[i].tileLetter << "] ";
        else
            cout << "[ ] ";
    }
    cout << "\n====================================\n";
}

// ---------------- LeaderBoard ----------------

LeaderBoard::LeaderBoard(const string& file)
{
    fileName = file;
}

void LeaderBoard::displayScores() const
{
    ifstream input(fileName.c_str());

    if (!input)
        throw GameException("HEIST ERROR: Leader/scoreBoard.txt could not be opened.");

    string line;
    cout << "\n========== LEADER / SCORE BOARD ==========\n";

    while (getline(input, line))
        cout << line << "\n";

    cout << "==========================================\n";
    input.close();
}

void LeaderBoard::saveScore(const string& detective, const string& result,
                            int credibility, int turnsLeft, const string& culprit) const
{
    ofstream output(fileName.c_str(), ios::app);

    if (!output)
        throw GameException("HEIST ERROR: Score file could not be opened for saving.");

    time_t now = time(0);
    tm* localTime = localtime(&now);
    char dateText[20];
    strftime(dateText, sizeof(dateText), "%d-%m-%y", localTime);

    output << detective << "|" << result << "|" << credibility << "|"
           << turnsLeft << "|" << culprit << "|" << dateText << "\n";

    output.close();
}

// ---------------- GameEngine Singleton ----------------

GameEngine* GameEngine::instance = 0;

GameEngine::GameEngine()
    : leaderBoard("leader/scoreBoard.txt")
{
    maxTurns = 25;
    startingCredibility = 100;
    wrongTurnPenalty = 5;
    wrongWordPenalty = 10;
    turnsUsed = 0;
    credibility = 100;
}

GameEngine& GameEngine::getInstance()
{
    if (instance == 0)
        instance = new GameEngine();

    return *instance;
}

GameEngine::~GameEngine()
{
    // The Singleton instance is the central controller for the complete game.
}

DynamicArray<string> GameEngine::splitLine(const string& line, char delimiter) const
{
    DynamicArray<string> parts;
    string current = "";

    for (int i = 0; i < (int)line.length(); i++)
    {
        if (line[i] == delimiter)
        {
            parts.add(current);
            current = "";
        }
        else
            current += line[i];
    }

    parts.add(current);
    return parts;
}

int GameEngine::roleColumn(const string& role) const
{
    if (role == "Cook") return 2;
    if (role == "Guard") return 3;
    if (role == "Servant") return 4;
    if (role == "Partner 1" || role == "Partner_1") return 5;
    if (role == "Partner 2" || role == "Partner_2") return 6;
    if (role == "PA") return 7;
    if (role == "Child") return 8;

    throw GameException("HEIST ERROR: Culprit role does not exist in the data files.");
}

bool GameEngine::loadClues()
{
    ifstream input("clue.txt");
    if (!input)
        throw GameException("HEIST ERROR: clue.txt file was not found.");

    string line;
    getline(input, line); // Skip column headings.

    while (getline(input, line))
    {
        if (line.empty()) continue;

        DynamicArray<string> fields = splitLine(line, '|');
        if (fields.size() < 3)
            throw GameException("HEIST ERROR: A clue record is incomplete.");

        culpritRoles.add(fields[0]);

        // The selected culprit's words are assigned during game initialization.
        if (fields[0] == selectedCulprit)
        {
            targetWord1 = fields[1];
            targetWord2 = fields[2];
        }
    }

    input.close();
    return culpritRoles.size() == 7;
}

bool GameEngine::loadLocations()
{
    ifstream input("locations.txt");
    if (!input)
        throw GameException("HEIST ERROR: locations.txt file was not found.");

    string line;
    getline(input, line); // Skip headings.

    int selectedColumn = roleColumn(selectedCulprit);

    while (getline(input, line))
    {
        if (line.empty()) continue;

        DynamicArray<string> fields = splitLine(line, '|');
        if (fields.size() < 9)
            throw GameException("HEIST ERROR: A location record is incomplete.");

        Location location;
        location.locName = fields[0];
        location.boardPosLoc = atoi(fields[1].c_str());
        location.letterClueLoc = fields[selectedColumn][0];
        location.locRow = Word1;
        location.locSearchFlag = false;

        cottage.addLocation(location);
    }

    input.close();

    if (cottage.getLocationCount() != 7)
        throw GameException("HEIST ERROR: Exactly seven locations are required.");

    return true;
}

bool GameEngine::loadCharacters()
{
    ifstream input("characters.txt");
    if (!input)
        throw GameException("HEIST ERROR: characters.txt file was not found.");

    string line;
    getline(input, line); // Skip headings.

    // Character file has three identity columns before its role-letter columns.
    int selectedColumn = roleColumn(selectedCulprit) + 2;

    while (getline(input, line))
    {
        if (line.empty()) continue;

        DynamicArray<string> fields = splitLine(line, '|');
        if (fields.size() < 12)
            throw GameException("HEIST ERROR: A character record is incomplete.");

        CharacterRecord person;
        person.name = fields[0];
        person.role = fields[1];
        person.nationality = fields[2];
        person.word2Position = atoi(fields[3].c_str());
        person.clueLetter = fields[selectedColumn][0];
        person.isCulprit = (person.role == selectedCulprit);
        person.interviewed = false;

        characters.add(person);
    }

    input.close();

    if (characters.size() != 8)
        throw GameException("HEIST ERROR: Exactly eight characters are required.");

    return true;
}

void GameEngine::loadRules()
{
    ifstream input("rules.txt");

    // Defaults are used if the rules file is missing or incomplete.
    maxTurns = 25;
    startingCredibility = 100;
    wrongTurnPenalty = 5;
    wrongWordPenalty = 10;

    if (!input)
    {
        cout << "\nRules file missing. Default rules will be used.\n";
        return;
    }

    string line;
    while (getline(input, line))
    {
        DynamicArray<string> fields = splitLine(line, '=');
        if (fields.size() != 2) continue;

        if (fields[0] == "Max_Turns")
            maxTurns = atoi(fields[1].c_str());
        else if (fields[0] == "Starting_Credibility")
            startingCredibility = atoi(fields[1].c_str());
        else if (fields[0] == "Every_Wrong_Turn")
            wrongTurnPenalty = abs(atoi(fields[1].c_str()));
        else if (fields[0] == "Every_Wrong_Word")
            wrongWordPenalty = abs(atoi(fields[1].c_str()));
    }

    input.close();
}

void GameEngine::loadAbout()
{
    ifstream input("about.txt");
    if (!input)
        throw GameException("HEIST ERROR: about.txt file was not found.");

    aboutText = "";
    string line;

    while (getline(input, line))
        aboutText += line + "\n";

    input.close();
}

void GameEngine::showRules() const
{
    cout << "\n========== GAME RULES ==========\n";
    cout << "Maximum turns: " << maxTurns << "\n";
    cout << "Starting credibility: " << startingCredibility << "\n";
    cout << "Wrong action penalty: -" << wrongTurnPenalty << "\n";
    cout << "Wrong word penalty: -" << wrongWordPenalty << "\n";
    cout << "Win: Accuse the correct culprit.\n";
    cout << "Lose: Credibility reaches zero or turns run out.\n";
    cout << "================================\n";
}

void GameEngine::showAbout() const
{
    cout << "\n========== ABOUT THE GAME ==========\n";
    cout << aboutText << "\n";
}

void GameEngine::startNewGame()
{
    cout << "\n========== NEW GAME ==========\n";
    cout << "Enter detective name (no spaces; use _ or . instead): ";
    cin >> detectiveName;

    if (detectiveName.empty() || detectiveName.find(' ') != string::npos)
        throw GameException("HEIST ERROR: Detective name must be non-empty and contain no spaces.");

    // Reset game objects so every new game starts with an empty board.
    cottage = Cottage();
    scrabble = Scrabble();
    characters.clear();
    culpritRoles.clear();
    targetWord1 = "";
    targetWord2 = "";
    turnsUsed = 0;

    loadRules();
    loadClues();

    if (culpritRoles.size() != 7)
        throw GameException("HEIST ERROR: Clue file must contain seven culprit profiles.");

    srand((unsigned int)time(0));
    int chosenIndex = rand() % culpritRoles.size();
    selectedCulprit = culpritRoles[chosenIndex];

    // Load both target words for the randomly selected culprit.
    targetWord1 = "";
    targetWord2 = "";
    ifstream clueInput("clue.txt");
    string clueLine;
    getline(clueInput, clueLine);

    while (getline(clueInput, clueLine))
    {
        DynamicArray<string> fields = splitLine(clueLine, '|');
        if (fields.size() >= 3 && fields[0] == selectedCulprit)
        {
            targetWord1 = fields[1];
            targetWord2 = fields[2];
            break;
        }
    }
    clueInput.close();

    if (targetWord1.empty() || targetWord2.empty())
        throw GameException("HEIST ERROR: Words for the selected culprit could not be loaded.");

    scrabble.setTargetWords(targetWord1, targetWord2);
    loadLocations();
    loadCharacters();

    credibility = startingCredibility;

    cout << "\nA culprit has been secretly selected. Your investigation begins!\n";
    gameLoop();
}

void GameEngine::addTurn()
{
    turnsUsed++;
}

bool GameEngine::gameIsOver() const
{
    return credibility <= 0 || turnsUsed >= maxTurns;
}

void GameEngine::visitLocation()
{
    cottage.displayLocations();

    int choice;
    cout << "Select location number (0 to cancel): ";
    cin >> choice;

    if (choice == 0) return;

    if (choice < 1 || choice > cottage.getLocationCount())
        throw GameException("HEIST ERROR: Invalid location selection.");

    Location& location = cottage.getLocation(choice - 1);

    if (location.locSearchFlag)
    {
        char again;
        cout << "You have already searched this location. Search it again? (Y/N): ";
        cin >> again;
        if (again == 'Y' || again == 'y')
            cout << "Previously collected letter: " << location.letterClueLoc
                 << " at position " << location.boardPosLoc << " of Word 1.\n";
        return;
    }

    char answer;
    cout << "Search " << location.locName << "? (Y/N): ";
    cin >> answer;

    if (answer == 'Y' || answer == 'y')
    {
        scrabble.placeLetter(Word1, location.letterClueLoc, location.boardPosLoc);
        location.locSearchFlag = true;

        cout << "Letter " << location.letterClueLoc
             << " collected at position " << location.boardPosLoc
             << " of Word 1.\n";
        addTurn();
    }
}

void GameEngine::interviewPerson()
{
    cout << "\n========== CHARACTER MENU ==========\n";

    for (int i = 0; i < characters.size(); i++)
    {
        cout << "[" << i + 1 << "] " << characters[i].name
             << " (" << characters[i].role << ") - "
             << (characters[i].interviewed ? "Interviewed" : "Not interviewed")
             << "\n";
    }

    int choice;
    cout << "Select character number (0 to cancel): ";
    cin >> choice;

    if (choice == 0) return;

    if (choice < 1 || choice > characters.size())
        throw GameException("HEIST ERROR: Invalid character selection.");

    CharacterRecord& person = characters[choice - 1];

    if (person.interviewed)
    {
        char again;
        cout << "This character has already been interviewed. Interview again? (Y/N): ";
        cin >> again;
        if (again == 'Y' || again == 'y')
            cout << "Previously collected letter: " << person.clueLetter
                 << " at position " << person.word2Position << " of Word 2.\n";
        return;
    }

    char answer;
    cout << "Interview " << person.name << "? (Y/N): ";
    cin >> answer;

    if (answer == 'Y' || answer == 'y')
    {
        scrabble.placeLetter(Word2, person.clueLetter, person.word2Position);
        person.interviewed = true;

        cout << "Letter " << person.clueLetter
             << " collected at position " << person.word2Position
             << " of Word 2.\n";
        addTurn();
    }
}

void GameEngine::formWords()
{
    if (!scrabble.bothRowsComplete())
    {
        cout << "\nYou have not collected all letters yet.\n";
        credibility -= wrongWordPenalty;
        addTurn();
        return;
    }

    cout << "\nWord 1: " << scrabble.revealWord(Word1) << "\n";
    cout << "Word 2: " << scrabble.revealWord(Word2) << "\n";
    addTurn();
}

void GameEngine::accuseSuspect()
{
    cout << "\n========== SUSPECT ROLES ==========\n";
    for (int i = 0; i < culpritRoles.size(); i++)
        cout << "[" << i + 1 << "] " << culpritRoles[i] << "\n";

    int choice;
    cout << "Choose the culprit role: ";
    cin >> choice;

    if (choice < 1 || choice > culpritRoles.size())
        throw GameException("HEIST ERROR: Invalid suspect selection.");

    addTurn();

    if (culpritRoles[choice - 1] == selectedCulprit)
    {
        cout << "\nCorrect accusation! You solved the Locker Heist.\n";
        finishGame("WIN");
        turnsUsed = maxTurns; // Ends the game loop after a successful accusation.
    }
    else
    {
        cout << "\nWrong accusation! Credibility reduced.\n";
        credibility -= wrongTurnPenalty;

        if (gameIsOver())
            finishGame("LOSE");
    }
}

void GameEngine::finishGame(const string& result)
{
    int turnsLeft = maxTurns - turnsUsed;
    if (turnsLeft < 0) turnsLeft = 0;

    cout << "\n========== GAME RESULT ==========\n";
    cout << "Detective: " << detectiveName << "\n";
    cout << "Result: " << result << "\n";
    cout << "Credibility left: " << credibility << "\n";
    cout << "Turns left: " << turnsLeft << "\n";

    leaderBoard.saveScore(detectiveName, result, credibility, turnsLeft, selectedCulprit);
}

void GameEngine::gameLoop()
{
    while (!gameIsOver())
    {
        cout << "\n----------------------------------\n";
        cout << "Detective: " << detectiveName
             << " | Credibility: " << credibility
             << " | Turns used: " << turnsUsed << "/" << maxTurns << "\n";

        scrabble.displayBoard();

        cout << "\n========== GAME MENU ==========\n";
        cout << "[1] Visit a Location\n";
        cout << "[2] Interview a Person\n";
        cout << "[3] View Scrabble Board (free)\n";
        cout << "[4] Form a Word\n";
        cout << "[5] Accuse a Suspect\n";
        cout << "[6] Quit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        try
        {
            switch (choice)
            {
            case 1:
                visitLocation();
                break;
            case 2:
                interviewPerson();
                break;
            case 3:
                scrabble.displayBoard();
                break;
            case 4:
                formWords();
                break;
            case 5:
                accuseSuspect();
                if (credibility <= 0 || turnsUsed >= maxTurns)
                    return;
                break;
            case 6:
                finishGame("LOSE");
                return;
            default:
                credibility -= wrongTurnPenalty;
                addTurn();
                throw GameException("HEIST ERROR: Invalid game menu choice.");
            }
        }
        catch (const GameException& error)
        {
            cout << error.getMessage() << "\n";
        }

        if (gameIsOver())
        {
            cout << "\nYour investigation has ended.\n";
            finishGame("LOSE");
            return;
        }
    }

    finishGame("LOSE");
}

void GameEngine::run()
{
    bool running = true;

    while (running)
    {
        cout << "\n====================================\n";
        cout << "          THE LOCKER HEIST\n";
        cout << "====================================\n";
        cout << "A detective mystery: collect letters from seven locations and eight people,\n";
        cout << "reveal two clue words, and identify the culprit before credibility runs out.\n\n";
        cout << "[1] New Game\n";
        cout << "[2] View Leader/Score Board\n";
        cout << "[3] About\n";
        cout << "[4] Rules\n";
        cout << "[5] Exit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        try
        {
            switch (choice)
            {
            case 1:
                startNewGame();
                break;
            case 2:
                leaderBoard.displayScores();
                break;
            case 3:
                loadAbout();
                showAbout();
                break;
            case 4:
                loadRules();
                showRules();
                break;
            case 5:
                running = false;
                cout << "Thank you for playing The Locker Heist!\n";
                break;
            default:
                throw GameException("HEIST ERROR: Please select a valid main menu option.");
            }
        }
        catch (const GameException& error)
        {
            cout << error.getMessage() << "\n";
        }
    }
}
