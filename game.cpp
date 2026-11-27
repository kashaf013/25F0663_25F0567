#include "game.h"

// Tile initializes one letter tile and its filled state.
Tile::Tile(char letter, bool isFilled)
{
    tileLetter = letter;
    filled = isFilled;
}

// Equality compares both the letter and filled state of two tiles.
bool Tile::operator==(const Tile& other) const
{
    return tileLetter == other.tileLetter && filled == other.filled;
}

// Less-than compares letters and is useful for ordering clue tiles.
bool Tile::operator<(const Tile& other) const
{
    return tileLetter < other.tileLetter;
}

// Insertion displays a tile's letter and whether its position is filled.
ostream& operator<<(ostream& out, const Tile& tile)
{
    out << tile.tileLetter << " (" << (tile.filled ? "filled" : "empty") << ")";
    return out;
}

// Extraction reads the tile letter and its filled state.
istream& operator>>(istream& in, Tile& tile)
{
    cout << "Enter tile letter: ";
    in >> tile.tileLetter;
    cout << "Is the tile filled? (1/0): ";
    in >> tile.filled;
    return in;
}

// Clue initializes the data carried from a location or character to Scrabble.
Clue::Clue(char letter, int position, WordRow row)
{
    clueLetter = letter;
    cluePosition = position;
    clueRow = row;
}

// Location initializes the location name, clue, position, row, and search flag.
Location::Location(string name, char letter, int position, WordRow row, bool searched)
{
    locName = name;
    letterClueLoc = letter;
    boardPosLoc = position;
    locRow = row;
    locSearchFlag = searched;
}

// Insertion displays all information stored in a location.
ostream& operator<<(ostream& out, const Location& location)
{
    out << "Location: " << location.locName
        << "\nLetter: " << location.letterClueLoc
        << "\nPosition: " << location.boardPosLoc
        << "\nRow: " << (location.locRow == Word1 ? "Word 1" : "Word 2")
        << "\nSearched: " << (location.locSearchFlag ? "Yes" : "No");
    return out;
}

// Extraction reads the information needed to create or update a location.
istream& operator>>(istream& in, Location& location)
{
    cout << "Enter location name: ";
    in >> ws;
    getline(in, location.locName);
    cout << "Enter clue letter: ";
    in >> location.letterClueLoc;
    cout << "Enter board position: ";
    in >> location.boardPosLoc;
    int row;
    cout << "Enter row (1 for Word 1, 2 for Word 2): ";
    in >> row;
    location.locRow = (row == 2) ? Word2 : Word1;
    cout << "Has this location been searched? (1/0): ";
    in >> location.locSearchFlag;
    return in;
}

// TileRack allocates its own array and starts with no stored tiles.
TileRack::TileRack(int maxCapacity)
{
    if (maxCapacity <= 0)
        throw invalid_argument("TileRack capacity must be greater than zero.");

    capacity = maxCapacity;
    tileCount = 0;
    tiles = new Tile[capacity];
}

// copyFrom allocates a separate array and copies each occupied tile.
void TileRack::copyFrom(const TileRack& other)
{
    capacity = other.capacity;
    tileCount = other.tileCount;
    tiles = new Tile[capacity];

    for (int i = 0; i < tileCount; i++)
        tiles[i] = other.tiles[i];
}

// The copy constructor utilizes copyFrom to make a deep copy of another rack.
TileRack::TileRack(const TileRack& other)
{
    copyFrom(other);
}

// Assignment releases old memory before utilizing copyFrom for a deep copy.
TileRack& TileRack::operator=(const TileRack& other)
{
    if (this != &other)
    {
        delete[] tiles;
        copyFrom(other);
    }
    return *this;
}

// The destructor releases the dynamic tile array owned by this rack.
TileRack::~TileRack()
{
    delete[] tiles;
}

// addTile stores a tile at the next free position and rejects a full rack.
void TileRack::addTile(const Tile& tile)
{
    if (isFull())
        throw overflow_error("TileRack is full.");

    tiles[tileCount] = tile;
    tiles[tileCount].filled = true;
    tileCount++;
}

// addLetter creates a filled tile from a character and stores it in the rack.
void TileRack::addLetter(char letter)
{
    addTile(Tile(letter, true));
}

// removeTile removes and returns a tile at the requested zero-based index.
Tile TileRack::removeTile(int index)
{
    if (isEmpty())
        throw underflow_error("TileRack is empty.");

    if (index < 0 || index >= tileCount)
        throw out_of_range("Invalid TileRack position.");

    Tile removed = tiles[index];

    for (int i = index; i < tileCount - 1; i++)
        tiles[i] = tiles[i + 1];

    tileCount--;
    tiles[tileCount] = Tile();
    return removed;
}

// removeLetter removes a tile at the requested position without returning it.
void TileRack::removeLetter(int index)
{
    removeTile(index);
}

// clear removes all occupied tiles while retaining the allocated capacity.
void TileRack::clear()
{
    for (int i = 0; i < tileCount; i++)
        tiles[i] = Tile();

    tileCount = 0;
}

// getCount returns the number of tiles currently stored in the rack.
int TileRack::getCount() const
{
    return tileCount;
}

// getCapacity returns the maximum number of tiles the rack can store.
int TileRack::getCapacity() const
{
    return capacity;
}

// isEmpty checks whether the rack currently contains no tiles.
bool TileRack::isEmpty() const
{
    return tileCount == 0;
}

// isFull checks whether all allocated rack positions are occupied.
bool TileRack::isFull() const
{
    return tileCount == capacity;
}

// getLetter returns the letter at a valid zero-based rack position.
char TileRack::getLetter(int index) const
{
    if (index < 0 || index >= tileCount)
        throw out_of_range("Invalid TileRack position.");

    return tiles[index].tileLetter;
}

// display prints each stored letter with its zero-based position.
void TileRack::display() const
{
    if (isEmpty())
    {
        cout << "(empty)" << endl;
        return;
    }

    for (int i = 0; i < tileCount; i++)
        cout << "[" << i << "] " << tiles[i].tileLetter << endl;
}

// operator+ creates a new rack containing both racks with a space separator tile.
TileRack TileRack::operator+(const TileRack& other) const
{
    TileRack merged(tileCount + other.tileCount + 1);

    for (int i = 0; i < tileCount; i++)
        merged.addTile(tiles[i]);

    merged.addTile(Tile(' ', true));

    for (int i = 0; i < other.tileCount; i++)
        merged.addTile(other.tiles[i]);

    return merged;
}

// Insertion displays all rack letters and their positions.
ostream& operator<<(ostream& out, const TileRack& rack)
{
    if (rack.isEmpty())
    {
        out << "(empty)";
        return out;
    }

    for (int i = 0; i < rack.tileCount; i++)
        out << "[" << i << "] " << rack.tiles[i].tileLetter << " ";

    return out;
}

// Character initializes the common information shared by all characters.
Character::Character(const string& name, const string& role,
                     const string& nation, bool culprit)
{
    characterName = name;
    characterRole = role;
    nationality = nation;
    isCulprit = culprit;
}

// The virtual destructor safely destroys objects through Character pointers.
Character::~Character()
{
}

// getName returns the character's name without exposing the private data member.
string Character::getName() const
{
    return characterName;
}

// getRole returns the character's role.
string Character::getRole() const
{
    return characterRole;
}

// getNationality returns the character's nationality.
string Character::getNationality() const
{
    return nationality;
}

// getIsCulprit returns whether this character is the selected culprit.
bool Character::getIsCulprit() const
{
    return isCulprit;
}

// setName updates the character's name through a public setter.
void Character::setName(const string& name)
{
    characterName = name;
}

// setRole updates the character's role through a public setter.
void Character::setRole(const string& role)
{
    characterRole = role;
}

// setNationality updates the character's nationality through a public setter.
void Character::setNationality(const string& nation)
{
    nationality = nation;
}

// setIsCulprit updates the culprit flag selected by the GameEngine.
void Character::setIsCulprit(bool value)
{
    isCulprit = value;
}

// displayBasic writes the common Character fields to a selected output stream.
void Character::displayBasic(ostream& out) const
{
    out << "Name: " << characterName
        << "\nRole: " << characterRole
        << "\nNationality: " << nationality
        << "\nCulprit: " << (isCulprit ? "Yes" : "No");
}

// The default interview rule allows a character to be interviewed.
bool Character::canBeInterviewed(int credibility) const
{
    return true;
}

// Insertion invokes the derived display function through polymorphism.
ostream& operator<<(ostream& out, const Character& character)
{
    character.display();
    return out;
}

// Extraction updates the common private fields of a Character object.
istream& operator>>(istream& in, Character& character)
{
    cout << "Enter name: ";
    in >> ws;
    getline(in, character.characterName);
    cout << "Enter role: ";
    getline(in, character.characterRole);
    cout << "Enter nationality: ";
    getline(in, character.nationality);
    cout << "Is culprit? (1/0): ";
    in >> character.isCulprit;
    return in;
}

// Victim initializes the French business owner as a non-suspect character.
Victim::Victim(const string& name, const string& nation)
    : Character(name, "Business Owner (Victim)", nation, false)
{
}

// display shows the victim's common information.
void Victim::display() const
{
    cout << "\n--- Victim ---" << endl;
    displayBasic(cout);
    cout << endl;
}

// The victim can be interviewed only when credibility is at least 80.
bool Victim::canBeInterviewed(int credibility) const
{
    return credibility >= 80;
}

// Suspect initializes the common suspect information and its character clue.
Suspect::Suspect(const string& name, const string& role, const string& nation,
                 bool culprit, const Clue& clue)
    : Character(name, role, nation, culprit)
{
    characterClue = clue;
}

// getCharacterClue returns the clue assigned to this suspect.
Clue Suspect::getCharacterClue() const
{
    return characterClue;
}

// setCharacterClue replaces the suspect's clue data.
void Suspect::setCharacterClue(const Clue& clue)
{
    characterClue = clue;
}

// display shows common suspect information.
void Suspect::display() const
{
    cout << "\n--- Suspect ---" << endl;
    displayBasic(cout);
    cout << "\nClue: " << characterClue.clueLetter
         << "\nPosition: " << characterClue.cluePosition << endl;
}

// BusinessRelations initializes the shared virtual Suspect base.
BusinessRelations::BusinessRelations(const string& name, const string& role,
                                     const string& nation, bool culprit,
                                     const Clue& clue)
    : Suspect(name, role, nation, culprit, clue)
{
}

// display identifies the business-relations branch.
void BusinessRelations::display() const
{
    Suspect::display();
    cout << "Category: Business Relations" << endl;
}

// HouseHelp initializes a staff member as a Suspect.
HouseHelp::HouseHelp(const string& name, const string& role,
                     const string& nation, bool culprit, const Clue& clue)
    : Suspect(name, role, nation, culprit, clue)
{
}

// display identifies the house-help branch.
void HouseHelp::display() const
{
    Suspect::display();
    cout << "Category: House Help" << endl;
}

// HouseHelp members can be interviewed regardless of detective credibility.
bool HouseHelp::canBeInterviewed(int credibility) const
{
    return true;
}

// CloseRelations initializes the shared virtual Suspect base.
CloseRelations::CloseRelations(const string& name, const string& role,
                               const string& nation, bool culprit,
                               const Clue& clue)
    : Suspect(name, role, nation, culprit, clue)
{
}

// display identifies the close-relations branch.
void CloseRelations::display() const
{
    Suspect::display();
    cout << "Category: Close Relations" << endl;
}

// Partner initializes the virtual Suspect base and business-relations branch.
Partner::Partner(const string& name, const string& role, const string& nation,
                 bool culprit, const Clue& clue)
    : Suspect(name, role, nation, culprit, clue),
      BusinessRelations(name, role, nation, culprit, clue)
{
}

// display shows the partner's inherited suspect information.
void Partner::display() const
{
    cout << "\n--- Partner ---" << endl;
    displayBasic(cout);
    cout << endl;
}

// Partners require credibility of at least 50 for an interview.
bool Partner::canBeInterviewed(int credibility) const
{
    return credibility >= 50;
}

// PA initializes Suspect once through virtual inheritance and both branches.
PA::PA(const string& name, const string& nation, bool culprit, const Clue& clue)
    : Suspect(name, "PA", nation, culprit, clue),
      BusinessRelations(name, "PA", nation, culprit, clue),
      CloseRelations(name, "PA", nation, culprit, clue)
{
}

// display shows the PA as a member of both business and close relations.
void PA::display() const
{
    cout << "\n--- Personal Assistant ---" << endl;
    displayBasic(cout);
    cout << "\nCategory: Business and Close Relations" << endl;
}

// PA can be interviewed when credibility is at least 35.
bool PA::canBeInterviewed(int credibility) const
{
    return credibility >= 35;
}

// Cook initializes HouseHelp as the Cook role.
Cook::Cook(const string& name, const string& nation, bool culprit, const Clue& clue)
    : HouseHelp(name, "Cook", nation, culprit, clue)
{
}

// display shows the Cook's information.
void Cook::display() const
{
    cout << "\n--- Cook ---" << endl;
    displayBasic(cout);
    cout << endl;
}

// Guard initializes HouseHelp as the Guard role.
Guard::Guard(const string& name, const string& nation, bool culprit, const Clue& clue)
    : HouseHelp(name, "Guard", nation, culprit, clue)
{
}

// display shows the Guard's information.
void Guard::display() const
{
    cout << "\n--- Guard ---" << endl;
    displayBasic(cout);
    cout << endl;
}

// Servant initializes HouseHelp as the Servant role.
Servant::Servant(const string& name, const string& nation, bool culprit, const Clue& clue)
    : HouseHelp(name, "Servant", nation, culprit, clue)
{
}

// display shows the Servant's information.
void Servant::display() const
{
    cout << "\n--- Servant ---" << endl;
    displayBasic(cout);
    cout << endl;
}

// Child initializes the virtual Suspect base through CloseRelations.
Child::Child(const string& name, const string& nation, int childAge,
             bool culprit, const Clue& clue)
    : Suspect(name, "Child", nation, culprit, clue),
      CloseRelations(name, "Child", nation, culprit, clue)
{
    age = childAge;
    if (age < 0)
        throw invalid_argument("Age cannot be negative.");
}

// getAge returns the child's age.
int Child::getAge() const
{
    return age;
}

// setAge validates and updates the child's age.
void Child::setAge(int childAge)
{
    if (childAge < 0)
        throw invalid_argument("Age cannot be negative.");

    age = childAge;
}

// display shows the child's information and age.
void Child::display() const
{
    cout << "\n--- Owner's Child ---" << endl;
    displayBasic(cout);
    cout << "\nAge: " << age << endl;
}

// Child requires age 15-19 and detective credibility of at least 75.
bool Child::canBeInterviewed(int credibility) const
{
    return age >= 15 && age <= 19 && credibility >= 75;
}

// Detective initializes the player and its owned rack.
Detective::Detective(const string& name, int startingCredibility,
                     int startingTurns, int rackCapacity)
    : Character(name, "Detective", "Malay", false),
      rack(rackCapacity)
{
    credibility = startingCredibility;
    turns = startingTurns;
}

// getCredibility returns the detective's current credibility.
int Detective::getCredibility() const
{
    return credibility;
}

// getTurns returns the number of turns recorded by the detective.
int Detective::getTurns() const
{
    return turns;
}

// setCredibility updates the detective's credibility.
void Detective::setCredibility(int value)
{
    credibility = value;
}

// setTurns stores the turn count updated by GameEngine.
void Detective::setTurns(int value)
{
    turns = value;
}

// changeCredibility adds or subtracts credibility points.
void Detective::changeCredibility(int amount)
{
    credibility += amount;
    if (credibility < 0)
        credibility = 0;
}

// incrementTurns increases the stored turn count by one.
void Detective::incrementTurns()
{
    turns++;
}

// getRack returns the detective's rack so GameEngine can add or remove tiles.
TileRack& Detective::getRack()
{
    return rack;
}

// The const getRack overload provides read-only access to the detective's rack.
const TileRack& Detective::getRack() const
{
    return rack;
}

// display shows the detective's player information and collected letters.
void Detective::display() const
{
    cout << "\n--- Detective ---" << endl;
    displayBasic(cout);
    cout << "\nCredibility: " << credibility
         << "\nTurns: " << turns
         << "\nRack: " << rack << endl;
}
