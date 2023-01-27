//
//  main.cpp
//  RabbitsvsPlayer
//
//  Created by Antara Chugh on 1/26/23.
//

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
#include <cctype>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;               // max number of rows in the arena
const int MAXCOLS = 25;               // max number of columns in the arena
const int MAXRABBITS = 100;           // max number of rabbits allowed

const int NORTH = 0;
const int EAST  = 1;
const int SOUTH = 2;
const int WEST  = 3;
const int NUMDIRS = 4;

const int EMPTY      = 0;
const int HAS_POISON = 1;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
            // type name, since it's mentioned in the Rabbit declaration.

class Rabbit
{
public:
    // Constructor
  Rabbit(Arena* ap, int r, int c);

    // Accessors
  int  row() const;
  int  col() const;
  bool isDead() const;

    // Mutators
  void move();

private:
  Arena* m_arena;
  int    m_row;
  int    m_col;
  int    m_timesPoisoned=0;
  bool m_ableToMove=true;
    // coded: You'll probably find that a rabbit object needs additional
    // data members to support your implementation of the behavior affected
    // by poisoned carrots.
};

class Player
{
public:
    // Constructor
  Player(Arena* ap, int r, int c);

    // Accessors
  int  row() const;
  int  col() const;
  bool isDead() const;

    // Mutators
  string dropPoisonedCarrot();
  string move(int dir);
  void   setDead();

private:
  Arena* m_arena;
  int    m_row;
  int    m_col;
  bool   m_dead;
};

class Arena
{
public:
    // Constructor/destructor
  Arena(int nRows, int nCols);
  ~Arena();

    // Accessors
  int     rows() const;
  int     cols() const;
  Player* player() const;
  int     rabbitCount() const;
  int     getCellStatus(int r, int c) const;
  int     numberOfRabbitsAt(int r, int c) const;
  void    display(string msg) const;

    // Mutators
  void setCellStatus(int r, int c, int status);
  bool addRabbit(int r, int c);
  bool addPlayer(int r, int c);
  void moveRabbits();

private:
  int     m_grid[MAXROWS][MAXCOLS];
  int     m_rows;
  int     m_cols;
  Player* m_player;
  Rabbit* m_rabbits[MAXRABBITS];
  int     m_nRabbits;
  int     m_turns;

    // Helper functions
  void checkPos(int r, int c, string functionName) const;
  bool isPosInBounds(int r, int c) const;
};

class Game
{
public:
    // Constructor/destructor
  Game(int rows, int cols, int nRabbits);
  ~Game();

    // Mutators
  void play();

private:
  Arena* m_arena;

    // Helper functions
  string takePlayerTurn();
};

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int randInt(int lowest, int highest);
bool decodeDirection(char ch, int& dir);
bool attemptMove(const Arena& a, int dir, int& r, int& c);
bool recommendMove(const Arena& a, int r, int c, int& bestDir);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
//  Rabbit implementation
///////////////////////////////////////////////////////////////////////////

Rabbit::Rabbit(Arena* ap, int r, int c)
{
  if (ap == nullptr)
  {
      cout << "***** A rabbit must be created in some Arena!" << endl;
      exit(1);
  }
  if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
  {
      cout << "***** Rabbit created with invalid coordinates (" << r << ","
           << c << ")!" << endl;
      exit(1);
  }
  m_arena = ap;
  m_row = r;
  m_col = c;
}

int Rabbit::row() const
{
  return m_row;
}

int Rabbit::col() const
{
    return m_col;
}

bool Rabbit::isDead() const
{
  if(m_timesPoisoned==2){
      return true;
  }
  return false;
}

void Rabbit::move()
{
  if(m_ableToMove==false){
      if(!isDead()){
          m_ableToMove=true;
      }
  }else{
      
      int dir=randInt(0, 3);
      attemptMove(*m_arena, dir, m_row, m_col);
      if(m_arena->getCellStatus(m_row, m_col) == HAS_POISON){
          m_timesPoisoned++;
          m_arena->setCellStatus(m_row, m_col, EMPTY);
      }
      if(m_timesPoisoned>=1){
          m_ableToMove=false;
      }
      
      // coded:
      //   Return without moving if the rabbit has eaten one poisoned
      //   carrot (so is supposed to move only every other turn) and
      //   this is a turn it does not move.
      
      //   Otherwise, attempt to move in a random direction; if can't
      //   move, don't move.  If it lands on a poisoned carrot, eat the
      //   carrot and remove it from the game (so it is no longer on that
      //   grid point).
      
  }
}




///////////////////////////////////////////////////////////////////////////
//  Player implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(Arena* ap, int r, int c)
{
  if (ap == nullptr)
  {
      cout << "***** The player must be created in some Arena!" << endl;
      exit(1);
  }
  if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
  {
      cout << "**** Player created with invalid coordinates (" << r
           << "," << c << ")!" << endl;
      exit(1);
  }
  m_arena = ap;
  m_row = r;
  m_col = c;
  m_dead = false;
}

int Player::row() const
{
    
  return m_row;
    
}

int Player::col() const
{
  return m_col;
  
}

string Player::dropPoisonedCarrot()
{
  if (m_arena->getCellStatus(m_row, m_col) == HAS_POISON)
      return "There's already a poisoned carrot at this spot.";
  m_arena->setCellStatus(m_row, m_col, HAS_POISON);
  return "A poisoned carrot has been dropped.";
}

string Player::move(int dir)
{
 bool move= attemptMove(*m_arena, dir, m_row, m_col);
  if(move==false){
      return "Player couldn't move; player stands.";
  }
  if(m_arena->numberOfRabbitsAt(m_row, m_col)>0){
      setDead();
      return "Player walked into a rabbit and died.";
  }
  if(dir==NORTH){
      return "Player moved north.";
  }
  if(dir==SOUTH){
      return "Player moved south.";
  }
  if(dir==EAST){
      return "Player moved east.";
  }
  if(dir==WEST){
      return "Player moved west.";
  }
  
  
    // CODED: Attempt to move the player one step in the indicated
    //        direction.  If this fails,
    //        return "Player couldn't move; player stands."
    //        A player who moves onto a rabbit, and this
    //        returns "Player walked into a rabbit and died."
    //        Otherwise, return one of "Player moved north.",
    //        "Player moved east.", "Player moved south.", or
    //        "Player moved west."
  return "Player couldn't move; player stands.";  // This implementation compiles, but is incorrect.
}

bool Player::isDead() const
{
  if(m_dead==true){
      return true;
  }
    // coded: Return whether the Player is dead
    // Delete the following line and replace it with the correct code.
  return false;  // This implementation compiles, but is incorrect.
}

void Player::setDead()
{
  m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementation
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
  if (nRows <= 0  ||  nCols <= 0  ||  nRows > MAXROWS  ||  nCols > MAXCOLS)
  {
      cout << "***** Arena created with invalid size " << nRows << " by "
           << nCols << "!" << endl;
      exit(1);
  }
  m_rows = nRows;
  m_cols = nCols;
  m_player = nullptr;
  m_nRabbits = 0;
  m_turns = 0;
  for (int r = 1; r <= m_rows; r++)
      for (int c = 1; c <= m_cols; c++)
          setCellStatus(r, c, EMPTY);
}

Arena::~Arena()
{
  // coded:  Deallocate the player and all remaining dynamically allocated
  //        rabbits.
  delete m_player;
  for(int i=0; i<m_nRabbits; i++){
      delete m_rabbits[i];
  }
  
}

int Arena::rows() const
{
  return m_rows;
}

int Arena::cols() const
{
  return m_cols;
   
}

Player* Arena::player() const
{
  return m_player;
}

int Arena::rabbitCount() const
{
  return m_nRabbits;
    
}

int Arena::getCellStatus(int r, int c) const
{
  checkPos(r, c, "Arena::getCellStatus");
  return m_grid[r-1][c-1];
}

int Arena::numberOfRabbitsAt(int r, int c) const
{
  int count=0;
  for(int i=0; i<m_nRabbits; i++){
      if(m_rabbits[i]->row()==r && m_rabbits[i]->col()==c){
          count++;
      }
  }
  return count;
  
    // coded: Return the number of rabbits at row r, column c
    // Delete the following line and replace it with the correct code.
 
}

void Arena::display(string msg) const
{
  char displayGrid[MAXROWS][MAXCOLS];
  int r, c;
  
    // Fill displayGrid with dots (empty) and stars (poisoned carrots)
  for (r = 1; r <= rows(); r++)
      for (c = 1; c <= cols(); c++)
          displayGrid[r-1][c-1] = (getCellStatus(r,c) == EMPTY ? '.' : '*');

    // Indicate each rabbit's position
    // coded
    //        to 'R'.  If it's 2 though 8, set it to '2' through '8'.
    //        For 9 or more, set it to '9'.
  
  for(r=1; r<=rows(); r++){
      for(c=1; c<=cols(); c++){
          int numRabbits=numberOfRabbitsAt(r, c);
          if(numRabbits==0){
          
          } else if(numRabbits==1){
              displayGrid[r-1][c-1]='R';
          } else if(numRabbits>=9){
              displayGrid[r-1][c-1]='9';
          }else{
              displayGrid[r-1][c-1]=numRabbits+'0';
              
          }
          
          
      }
  }

    // Indicate the player's position
  if (m_player != nullptr)
      displayGrid[m_player->row()-1][m_player->col()-1] = (m_player->isDead() ? 'X' : '@');

    // Draw the grid
  clearScreen();
  for (r = 1; r <= rows(); r++)
  {
      for (c = 1; c <= cols(); c++)
          cout << displayGrid[r-1][c-1];
      cout << endl;
  }
  cout << endl;

    // Write message, rabbit, and player info
  if (msg != "")
      cout << msg << endl;
  cout << "There are " << rabbitCount() << " rabbits remaining." << endl;
  if (m_player == nullptr)
      cout << "There is no player!" << endl;
  else if (m_player->isDead())
      cout << "The player is dead." << endl;
  cout << m_turns << " turns have been taken." << endl;
}

void Arena::setCellStatus(int r, int c, int status)
{
  checkPos(r, c, "Arena::setCellStatus");
  m_grid[r-1][c-1] = status;
}

bool Arena::addRabbit(int r, int c)
{
  if (! isPosInBounds(r, c))
      return false;

    // Don't add a rabbit on a spot with a poisoned carrot
  if (getCellStatus(r, c) != EMPTY)
      return false;

    // Don't add a rabbit on a spot with a player
  if (m_player != nullptr  &&  m_player->row() == r  &&  m_player->col() == c)
      return false;

    // If there are MAXRABBITS existing rabbits, return false.  Otherwise,
    // dynamically allocate a new rabbit at coordinates (r,c).  Save the
    // pointer to newly allocated rabbit and return true.
  if(m_nRabbits==MAXRABBITS){
      return false;
  }else{
      m_rabbits[m_nRabbits]= new Rabbit(this, r,c);
      m_nRabbits++;
      return true;
      }

    // coded
  return false;  // This implementation compiles, but is incorrect.
}

bool Arena::addPlayer(int r, int c)
{
  if (! isPosInBounds(r, c))
      return false;

    // Don't add a player if one already exists
  if (m_player != nullptr)
      return false;

    // Don't add a player on a spot with a rabbit
  if (numberOfRabbitsAt(r, c) > 0)
      return false;

  m_player = new Player(this, r, c);
  return true;
}

void Arena::moveRabbits()
{
  for(int i=0; i<m_nRabbits; i++){
      m_rabbits[i]->move();
  }
  int rabbitsAtPlayerSpace=numberOfRabbitsAt(m_player->row(), m_player->col());
  if(rabbitsAtPlayerSpace>0){
      m_player->setDead();
  }
  //check if player is dead, delete player if dead
  //then check if rabbit is dead:
  for(int i=0; i<m_nRabbits; i++){
      if(m_rabbits[i]->isDead()==true){
          delete m_rabbits[i];
          for(int j=i; j<m_nRabbits-1; j++){
              m_rabbits[j]=m_rabbits[j+1];
          }
          m_nRabbits--;
      }
  }
    // Move all rabbits
    // coded:  Move each rabbit.  Mark the player as dead if necessary.
    //        Deallocate any dead dynamically allocated rabbit.

    // Another turn has been taken
  m_turns++;
}

bool Arena::isPosInBounds(int r, int c) const
{
  return (r >= 1  &&  r <= m_rows  &&  c >= 1  &&  c <= m_cols);
}

void Arena::checkPos(int r, int c, string functionName) const
{
  if (!isPosInBounds(r, c))
  {
      cout << "***** " << "Invalid arena position (" << r << ","
           << c << ") in call to " << functionName << endl;
      exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////
//  Game implementation
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRabbits)
{
  if (nRabbits < 0)
  {
      cout << "***** Cannot create Game with negative number of rabbits!" << endl;
      exit(1);
  }
  if (nRabbits > MAXRABBITS)
  {
      cout << "***** Trying to create Game with " << nRabbits
           << " rabbits; only " << MAXRABBITS << " are allowed!" << endl;
      exit(1);
  }
  int nEmpty = rows * cols - nRabbits - 1;  // 1 for Player
  if (nEmpty < 0)
  {
      cout << "***** Game created with a " << rows << " by "
           << cols << " arena, which is too small too hold a player and "
           << nRabbits << " rabbits!" << endl;
      exit(1);
  }

    // Create arena
  m_arena = new Arena(rows, cols);

    // Add player
  int rPlayer;
  int cPlayer;
  do
  {
      rPlayer = randInt(1, rows);
      cPlayer = randInt(1, cols);
  } while (m_arena->getCellStatus(rPlayer, cPlayer) != EMPTY);
  m_arena->addPlayer(rPlayer, cPlayer);

    // Populate with rabbits
  while (nRabbits > 0)
  {
      int r = randInt(1, rows);
      int c = randInt(1, cols);
      if (r == rPlayer && c == cPlayer)
          continue;
      m_arena->addRabbit(r, c);
      nRabbits--;
  }
}

Game::~Game()
{
  delete m_arena;
}

string Game::takePlayerTurn()
{
  for (;;)
  {
      cout << "Your move (n/e/s/w/c or nothing): ";
      string playerMove;
      getline(cin, playerMove);

      Player* player = m_arena->player();
      int dir;

      if (playerMove.size() == 0)
      {
          if (recommendMove(*m_arena, player->row(), player->col(), dir))
              return player->move(dir);
          else
              return player->dropPoisonedCarrot();
      }
      else if (playerMove.size() == 1)
      {
          if (tolower(playerMove[0]) == 'c')
              return player->dropPoisonedCarrot();
          else if (decodeDirection(playerMove[0], dir))
              return player->move(dir);
      }
      cout << "Player move must be nothing, or 1 character n/e/s/w/c." << endl;
  }
}

void Game::play()
{
  m_arena->display("");
  Player* player = m_arena->player();
  if (player == nullptr)
      return;
  while ( ! player->isDead()  &&  m_arena->rabbitCount() > 0)
  {
      string msg = takePlayerTurn();
      m_arena->display(msg);
      if (player->isDead())
          break;
      m_arena->moveRabbits();
      m_arena->display(msg);
  }
  if (player->isDead())
      cout << "You lose." << endl;
  else
      cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

// Return a uniformly distributed random int from lowest to highest, inclusive
int randInt(int lowest, int highest)
{
  if (highest < lowest)
      swap(highest, lowest);
  static random_device rd;
  static default_random_engine generator(rd());
  uniform_int_distribution<> distro(lowest, highest);
  return distro(generator);
}

bool decodeDirection(char ch, int& dir)
{
  switch (tolower(ch))
  {
    default:  return false;
    case 'n': dir = NORTH; break;
    case 'e': dir = EAST;  break;
    case 's': dir = SOUTH; break;
    case 'w': dir = WEST;  break;
  }
  return true;
}

// Return false without changing anything if moving one step from (r,c)
// in the indicated direction would run off the edge of the arena.
// Otherwise, update r and c to the position resulting from the move and
// return true.
bool attemptMove(const Arena& a, int dir, int& r, int& c)
{
  if(dir==NORTH){
      if((r-1)<=0){
          return false;
      }
      r--;
      return true;
  }
  if(dir==SOUTH){
      if(r+1>a.rows()){
          return false;
      }
      r++;
      return true;
      
  }
  if(dir==EAST){
      if(c+1>a.cols()){
          return false;
      }
      c++;
      return true;
  }
  if(dir==WEST){
      if(c-1<=0){
          return false;
      }
      c--;
      return true;
  }
  
  return false;
 
}

// Recommend a move for a player at (r,c):  A false return means the
// recommendation is that the player should drop a poisoned carrot and
// not move; otherwise, this function sets bestDir to the recommended
// direction to move and returns true.
bool recommendMove(const Arena& a, int r, int c, int& bestDir)
{
  //check where rabbits are in 4 coordinates, n, s, e, w,
  //if no rabbits around, return false (drop a carrot)
  int moveNorth=r-1;
  int moveSouth=r+1;
  int moveEast=c+1;
  int moveWest=c-1;
  int numRabbitsN=-1;
  int numRabbitsS=-1;
  int numRabbitsE=-1;
  int numRabbitsW=-1;
  int numRabbitsNW=-1;
  int numRabbitsNE=-1;
  int numRabbitsSW=-1;
  int numRabbitsSE=-1;
  int riskStayingPut=0;
 
  
  
  
  if(moveNorth>0){
      numRabbitsN=a.numberOfRabbitsAt(moveNorth, c);
      riskStayingPut+=numRabbitsN;
  }
  if(moveSouth<=a.rows()){
      numRabbitsS=a.numberOfRabbitsAt(moveSouth, c);
      riskStayingPut+=numRabbitsS;
  }
  if(moveEast<=a.cols()){
      numRabbitsE=a.numberOfRabbitsAt(r, moveEast);
      riskStayingPut+=numRabbitsE;
  }
  if(moveWest>0){
      numRabbitsW=a.numberOfRabbitsAt(r, moveWest);
      riskStayingPut+=numRabbitsW;
  }
  if(riskStayingPut==0 && a.getCellStatus(r, c)==EMPTY){
      return false;
  }
  if(numRabbitsN>0 && numRabbitsS>0 && numRabbitsE>0 && numRabbitsW>0){
      return false;
  }
  if(moveNorth>0 && moveWest>0){
      numRabbitsNW=a.numberOfRabbitsAt(moveNorth, moveWest);
  }
  if(moveNorth>0 && moveEast<=a.cols()){
      numRabbitsNE=a.numberOfRabbitsAt(moveNorth, moveEast);
  }
  if(moveSouth<=a.rows() && moveWest>0){
      numRabbitsSW=a.numberOfRabbitsAt(moveSouth, moveWest);
  }
  if(moveSouth<=a.rows() && moveEast<=a.cols()){
      numRabbitsSE=a.numberOfRabbitsAt(moveSouth, moveEast);
  }
  
  int riskN=-1;
  int riskS=-1;
  int riskE=-1;
  int riskW=-1;
  
  if(numRabbitsN==0){
      riskN=0;
      if(numRabbitsNE!=-1){
          riskN+=numRabbitsNE;
      }
      if(numRabbitsNW!=-1){
          riskN+=numRabbitsNW;
      }
      if(moveNorth-1>0){
          riskN+=a.numberOfRabbitsAt(moveNorth-1, c);
      }
  }
  if(numRabbitsS==0){
      riskS=0;
      if(numRabbitsSE!=-1){
          riskS+=numRabbitsSE;
      }
      if(numRabbitsSW!=-1){
          riskS+=numRabbitsSW;
      }
      if(moveSouth+1<=a.rows()){
          riskS+=a.numberOfRabbitsAt(moveSouth+1, c);
      }
  }
  if(numRabbitsW==0){
      riskW=0;
      if(numRabbitsNW!=-1){
          riskW+=numRabbitsNW;
      }
      if(numRabbitsSW!=-1){
          riskW+=numRabbitsSW;
      }
      if(moveWest-1>0){
          riskW+=a.numberOfRabbitsAt(r, moveWest-1);
      }
  }
  if(numRabbitsE==0){
      riskE=0;
      if(numRabbitsNE!=-1){
          riskE+=numRabbitsNE;
      }
      if(numRabbitsSE!=-1){
          riskE+=numRabbitsSE;
      }
      if(moveEast+1<=a.cols()){
          riskE+=a.numberOfRabbitsAt(r, moveEast+1);
      }
  
  }
  
  int riskFactors[4];
  riskFactors[0]=riskN;
  riskFactors[1]=riskE;
  riskFactors[2]=riskS;
  riskFactors[3]=riskW;
  int minRisk=riskStayingPut;
  for(int i=0; i<4; i++){
      if(riskFactors[i]!=-1 && riskFactors[i]<=minRisk){
          bestDir=i;
          minRisk=riskFactors[i];
      }
      
  }
  return true;
 
  

  
  
  
  
    
    // Delete the following line and replace it with your code.
 // This implementation compiles, but is incorrect.

    // Your replacement implementation should do something intelligent.
    // You don't have to be any smarter than the following, although
    // you can if you want to be:  If staying put runs the risk of a
    // rabbit possibly moving onto the player's location when the rabbits
    // move, yet moving in a particular direction puts the player in a
    // position that is safe when the rabbits move, then the chosen
    // action is to move to a safer location.  Similarly, if staying put
    // is safe, but moving in certain directions puts the player in
    // danger of dying when the rabbits move, then the chosen action should
    // not be to move in one of the dangerous directions; instead, the player
    // should stay put or move to another safe position.  In general, a
    // position that may be moved to by many rabbits is more dangerous than
    // one that may be moved to by few.
    //
    // Unless you want to, you do not have to take into account that a
    // rabbit might be poisoned and thus sometimes less dangerous than one
    // that is not.  That requires a more sophisticated analysis that
    // we're not asking you to do.
}

///////////////////////////////////////////////////////////////////////////
// main()
///////////////////////////////////////////////////////////////////////////


int main()
{
    // Create a game
    // Use this instead to create a mini-game:   Game g(3, 5, 2);
  
 
  Game g(10, 12, 40);
         
    // Play the game
g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANYTHING BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++/g31 UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _WIN32

#include <windows.h>

void clearScreen()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  COORD upperLeft = { 0, 0 };
  DWORD dwCharsWritten;
  FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                                                      &dwCharsWritten);
  SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not _WIN32

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
  static const char* term = getenv("TERM");
  if (term == nullptr  ||  strcmp(term, "dumb") == 0)
      cout << endl;
  else
  {
      static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
      cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
  }
}

#endif
