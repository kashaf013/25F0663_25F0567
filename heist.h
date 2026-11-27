#ifndef HEIST_H
#define HEIST_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

// Class Begin: GameException
// This class gives one common format for errors throughout the game.
// It utilizes a message so the user can understand what went wrong.
class GameException
{
private:
    string message;

public:
    GameException(const string& msg);
    string getMessage() const;
};

// Class Begin: DynamicArray
// This template utilizes dynamic memory to store any data type.
// It supports safe copying, insertion, indexing and automatic cleanup.
template <class T>
class DynamicArray
{
private:
    T* data;
    int currentSize;
    int capacity;

    void resize(int newCapacity)
    {
        T* newData = new T[newCapacity];
        for (int i = 0; i < currentSize; i++)
            newData[i] = data[i];

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    DynamicArray(int initialCapacity = 4)
    {
        capacity = (initialCapacity > 0) ? initialCapacity : 4;
        currentSize = 0;
        data = new T[capacity];
    }

    DynamicArray(const DynamicArray& other)
    {
        capacity = other.capacity;
        currentSize = other.currentSize;
        data = new T[capacity];
        for (int i = 0; i < currentSize; i++)
            data[i] = other.data[i];
    }

    DynamicArray& operator=(const DynamicArray& other)
    {
        if (this != &other)
        {
            T* newData = new T[other.capacity];
            for (int i = 0; i < other.currentSize; i++)
                newData[i] = other.data[i];

            delete[] data;
            data = newData;
            capacity = other.capacity;
            currentSize = other.currentSize;
        }
        return *this;
    }

    ~DynamicArray()
    {
        delete[] data;
    }

    void add(const T& value)
    {
        if (currentSize == capacity)
            resize(capacity * 2);

        data[currentSize++] = value;
    }

    int size() const { return currentSize; }

    bool isEmpty() const { return currentSize == 0; }

    T& operator[](int index)
    {
        if (index < 0 || index >= currentSize)
            throw GameException("HEIST ERROR: Array index is out of range.");
        return data[index];
    }

    const T& operator[](int index) const
    {
        if (index < 0 || index >= currentSize)
            throw GameException("HEIST ERROR: Array index is out of range.");
        return data[index];
    }

    void clear()
    {
        delete[] data;
        capacity = 4;
        currentSize = 0;
        data = new T[capacity];
    }
};

// The enum identifies which row of the Scrabble board receives a letter.
enum GameState { Menu, Playing, Won, Lost, Exit };
enum MenuChoice { NewGame = 1, ViewLeaderBoard, About, Rules, ExitGame };

enum WordRow
{
    Word1,
    Word2
};

// These structures carry information between files and game classes.
struct Tile
{
    char tileLetter;
    bool filled;

    Tile(char letter = ' ', bool isFilled = false)
    {
        tileLetter = letter;
        filled = isFilled;
    }
};

struct Clue
{
    char clueLetter;
    int cluePosition;
    WordRow clueRow;
};

struct Location
{
    string locName;
    char letterClueLoc;
    int boardPosLoc;
    WordRow locRow;
    bool locSearchFlag;

    Location()
    {
        locName = "";
        letterClueLoc = ' ';
        boardPosLoc = 0;
        locRow = Word1;
        locSearchFlag = false;
    }
};

struct CharacterRecord
{
    string name;
    string role;
    string nationality;
    int word2Position;
    char clueLetter;
    bool isCulprit;
    bool interviewed;

    CharacterRecord()
    {
        name = "";
        role = "";
        nationality = "";
        word2Position = 0;
        clueLetter = ' ';
        isCulprit = false;
        interviewed = false;
    }
};

// Class Begin: Cottage
// Cottage utilizes a DynamicArray to manage its seven Location objects.
// Locations are aggregated because they are stored and managed as a collection.
class Cottage
{
private:
    // Earlier raw DMA approach (kept as required by Module 8):
    // Location* LocCache; int locationCount; // allocated with new[] and released with delete[]
    DynamicArray<Location> LocCache;

public:
    Cottage();
    Cottage(const Cottage& other);
    Cottage& operator=(const Cottage& other);

    void addLocation(const Location& location);
    int getLocationCount() const;
    bool isSearched(int index) const;
    Location& getLocation(int index);
    const Location& getLocation(int index) const;
    void displayLocations() const;
};

// Class Begin: Scrabble
// Scrabble utilizes two dynamic rows: seven slots for Word 1 and eight for Word 2.
// Each tile is placed at the exact position read from the project data files.
class Scrabble
{
private:
    // Earlier jagged raw DMA approach (kept as required by Module 8):
    // Tile* wordOneRaw; Tile* wordTwoRaw; // allocate 7 and 8 Tile slots respectively
    DynamicArray<Tile> wordOne;
    DynamicArray<Tile> wordTwo;
    string targetWordOne;
    string targetWordTwo;

public:
    Scrabble();
    Scrabble(const Scrabble& other);
    Scrabble& operator=(const Scrabble& other);
    void setTargetWords(const string& first, const string& second);
    void placeLetter(WordRow row, char letter, int position);
    bool isComplete(WordRow row) const;
    bool bothRowsComplete() const;
    string revealWord(WordRow row) const;
    void displayBoard() const;
};

// Class Begin: LeaderBoard
// LeaderBoard utilizes file handling to preserve results between game sessions.
// New results are appended so previous detectives' scores remain available.
class LeaderBoard
{
private:
    string fileName;

public:
    LeaderBoard(const string& file = "leader/scoreBoard.txt");
    void displayScores() const;
    void saveScore(const string& detective, const string& result,
        int credibility, int turnsLeft, const string& culprit) const;
};

// Class Begin: GameEngine
// GameEngine is a Singleton so only one central game controller exists.
// It coordinates file loading, game initialization, the board and score management.
class GameEngine
{
private:
    static GameEngine* instance;

    Cottage cottage;
    Scrabble scrabble;
    LeaderBoard leaderBoard;

    DynamicArray<CharacterRecord> characters;
    DynamicArray<string> culpritRoles;

    string detectiveName;
    string selectedCulprit;
    string targetWord1;
    string targetWord2;
    string aboutText;

    int maxTurns;
    int startingCredibility;
    int wrongTurnPenalty;
    int wrongWordPenalty;
    int turnsUsed;
    int credibility;

    GameEngine();
    GameEngine(const GameEngine&);
    GameEngine& operator=(const GameEngine&);

    DynamicArray<string> splitLine(const string& line, char delimiter) const;
    int roleColumn(const string& role) const;
    bool loadClues();
    bool loadLocations();
    bool loadCharacters();
    void loadRules();
    void loadAbout();
    void showRules() const;
    void showAbout() const;
    void startNewGame();
    void gameLoop();
    void visitLocation();
    void interviewPerson();
    void formWords();
    void accuseSuspect();
    void finishGame(const string& result);
    void addTurn();
    bool gameIsOver() const;

public:
    static GameEngine& getInstance();
    void run();
    ~GameEngine();
};

#endif
