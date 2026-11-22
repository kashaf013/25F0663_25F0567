#ifndef HEIST_H
#define HEIST_H

#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

// Class Begin: These shared structures and enumerations utilize simple data types
// to transfer clues and represent the different states and choices of the game.
enum GameState
{
    Menu,
    Playing,
    Won,
    Lost,
    Exit
};

enum MenuChoice
{
    NewGame = 1,
    LeaderScoreBoard,
    About,
    Rules,
    ExitGame
};

enum WordRow
{
    Word1,
    Word2
};

// Tile utilizes a letter and a filled flag to represent one board/rack position.
struct Tile
{
    char tileLetter;
    bool filled;

    Tile(char letter = ' ', bool isFilled = false);
    bool operator==(const Tile& other) const;
    bool operator<(const Tile& other) const;

    friend ostream& operator<<(ostream& out, const Tile& tile);
    friend istream& operator>>(istream& in, Tile& tile);
};

// Clue utilizes one structure to carry a letter, its position, and its target row.
struct Clue
{
    char clueLetter;
    int cluePosition;
    WordRow clueRow;

    Clue(char letter = ' ', int position = 0, WordRow row = Word1);
};

// Location stores the information that the Cottage module will utilize for each place.
struct Location
{
    string locName;
    char letterClueLoc;
    int boardPosLoc;
    WordRow locRow;
    bool locSearchFlag;

    Location(string name = "", char letter = ' ', int position = 0,
             WordRow row = Word1, bool searched = false);

    friend ostream& operator<<(ostream& out, const Location& location);
    friend istream& operator>>(istream& in, Location& location);
};

// Class Begin: TileRack utilizes dynamic memory to own and manage collected letter tiles.
// Its copy operations utilize deep copying so separate racks never share the same array.
class TileRack
{
private:
    Tile* tiles;
    int capacity;
    int tileCount;

    void copyFrom(const TileRack& other);

public:
    explicit TileRack(int maxCapacity = 8);
    TileRack(const TileRack& other);
    TileRack& operator=(const TileRack& other);
    ~TileRack();

    void addTile(const Tile& tile);
    void addLetter(char letter);
    Tile removeTile(int index);
    void removeLetter(int index);
    void clear();

    int getCount() const;
    int getCapacity() const;
    bool isEmpty() const;
    bool isFull() const;
    char getLetter(int index) const;
    void display() const;

    TileRack operator+(const TileRack& other) const;

    friend ostream& operator<<(ostream& out, const TileRack& rack);
};

// Class Begin: Character utilizes inheritance to store information common to every person.
// Its pure virtual display function ensures every concrete character provides its own display.
class Character
{
private:
    string characterName;
    string characterRole;
    string nationality;
    bool isCulprit;

protected:
    void displayBasic(ostream& out) const;

public:
    Character(const string& name, const string& role,
              const string& nation, bool culprit = false);
    virtual ~Character();

    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;

    void setName(const string& name);
    void setRole(const string& role);
    void setNationality(const string& nation);
    void setIsCulprit(bool value);

    virtual void display() const = 0;
    virtual bool canBeInterviewed(int credibility) const;

    friend ostream& operator<<(ostream& out, const Character& character);
    friend istream& operator>>(istream& in, Character& character);
    friend class GameEngine;
};

// Class Begin: Victim utilizes Character but is not a suspect in the investigation.
// The interview rule utilizes the required minimum credibility for the business owner.
class Victim : public Character
{
public:
    Victim(const string& name = "Mr.Leonardo",
           const string& nation = "French");
    void display() const override;
    bool canBeInterviewed(int credibility) const override;
};

// Class Begin: Suspect utilizes Character as the common base for all possible culprits.
// It stores the character clue once so every suspect role can reuse the same clue data.
class Suspect : public Character
{
private:
    Clue characterClue;

public:
    Suspect(const string& name, const string& role, const string& nation,
            bool culprit = false, const Clue& clue = Clue());
    Clue getCharacterClue() const;
    void setCharacterClue(const Clue& clue);
    void display() const override;
};

// Class Begin: BusinessRelations virtually inherits Suspect to prevent duplicate
// Suspect/Character objects when the PA also inherits CloseRelations.
class BusinessRelations : virtual public Suspect
{
public:
    BusinessRelations(const string& name, const string& role,
                      const string& nation, bool culprit = false,
                      const Clue& clue = Clue());
    void display() const override;
};

// Class Begin: HouseHelp utilizes Suspect for staff members who can be interviewed
// at any credibility level according to the project requirements.
class HouseHelp : public Suspect
{
public:
    HouseHelp(const string& name, const string& role,
              const string& nation, bool culprit = false,
              const Clue& clue = Clue());
    void display() const override;
    bool canBeInterviewed(int credibility) const override;
};

// Class Begin: CloseRelations virtually inherits Suspect to solve the PA diamond
// together with BusinessRelations while retaining the owner's close relations.
class CloseRelations : virtual public Suspect
{
public:
    CloseRelations(const string& name, const string& role,
                   const string& nation, bool culprit = false,
                   const Clue& clue = Clue());
    void display() const override;
};

// Class Begin: Partner utilizes BusinessRelations for either of the two business partners.
// Partners can be interviewed only when detective credibility is at least 50.
class Partner : public BusinessRelations
{
public:
    Partner(const string& name, const string& role, const string& nation,
            bool culprit = false, const Clue& clue = Clue());
    void display() const override;
    bool canBeInterviewed(int credibility) const override;
};

// Class Begin: PA utilizes both business and close relations, demonstrating a diamond.
// Virtual inheritance ensures that PA contains only one shared Suspect base.
class PA : public BusinessRelations, public CloseRelations
{
public:
    PA(const string& name = "Mr.Jad", const string& nation = "Egyptian",
       bool culprit = false, const Clue& clue = Clue());
    void display() const override;
    bool canBeInterviewed(int credibility) const override;
};

// Class Begin: Cook utilizes HouseHelp and keeps the staff role as Cook.
// HouseHelp supplies the rule that staff can always be interviewed.
class Cook : public HouseHelp
{
public:
    Cook(const string& name = "Mr.Enzo", const string& nation = "Italian",
         bool culprit = false, const Clue& clue = Clue());
    void display() const override;
};

// Class Begin: Guard utilizes HouseHelp and keeps the staff role as Guard.
// The inherited interview rule allows the detective to interview the Guard at any time.
class Guard : public HouseHelp
{
public:
    Guard(const string& name = "Mr.Lukas", const string& nation = "German",
          bool culprit = false, const Clue& clue = Clue());
    void display() const override;
};

// Class Begin: Servant utilizes HouseHelp and keeps the staff role as Servant.
// The inherited interview rule allows the detective to interview the Servant at any time.
class Servant : public HouseHelp
{
public:
    Servant(const string& name = "Mr.Toby", const string& nation = "British",
            bool culprit = false, const Clue& clue = Clue());
    void display() const override;
};

// Class Begin: Child utilizes CloseRelations for the owner's child.
// The child can be interviewed only for age 15-19 and credibility of at least 75.
class Child : public CloseRelations
{
private:
    int age;

public:
    Child(const string& name = "Ms.Celine", const string& nation = "French",
          int childAge = 17, bool culprit = false, const Clue& clue = Clue());
    int getAge() const;
    void setAge(int childAge);
    void display() const override;
    bool canBeInterviewed(int credibility) const override;
};

// Class Begin: Detective utilizes Character as the player and owns a TileRack.
// GameEngine will update the stored turn count as required by the project.
class Detective : public Character
{
private:
    int credibility;
    int turns;
    TileRack rack;

public:
    Detective(const string& name = "Agent_Halim", int startingCredibility = 100,
              int startingTurns = 0, int rackCapacity = 8);

    int getCredibility() const;
    int getTurns() const;
    void setCredibility(int value);
    void setTurns(int value);
    void changeCredibility(int amount);
    void incrementTurns();

    TileRack& getRack();
    const TileRack& getRack() const;

    void display() const override;
};

#endif
