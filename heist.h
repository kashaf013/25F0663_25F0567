#ifndef HEIST_H
#define HEIST_H

#include "heist.h"
#include <fstream>
#include <ctime>
#include <cstdlib>

// GameException handles errors throughout the game.
class GameException
{
private:
    string message;

public:
    GameException(const string& msg);
    string getMessage() const;
};

// DynamicArray utilizes dynamic memory for different data types.
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

    int size() const
    {
        return currentSize;
    }

    bool isEmpty() const
    {
        return currentSize == 0;
    }

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

// CharacterRecord stores information loaded from characters.txt.
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

// Cottage manages the locations used in the investigation.
class Cottage
{
private:
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

// Scrabble manages the two clue-word rows.
class Scrabble
{
private:
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

// LeaderBoard handles saving and displaying game results.
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

// GameEngine controls the complete game using the Singleton pattern.
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
    static void destroyInstance();

    void run();

    ~GameEngine();
};

#endif
