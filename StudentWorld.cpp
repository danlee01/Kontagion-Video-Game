#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    this->socrates = nullptr;
    vector<Actor*> a;
    this->actors = a;
}

StudentWorld::~StudentWorld()
{
    //cleanUp();
}

int StudentWorld::init()
{
    this->socrates = new Socrates(this);
    addPits();
    addFood();
    addDirt();
    
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
        
    bool levelDone = true;
    
    addGoodies();
    
    socrates->doSomething();
    
    for (int i = 0; i < actors.size(); i++ )
    {
         actors[i]->doSomething();
         if (actors[i]->preventsLevelCompletion()) levelDone = false;
    }
    
    // Delete dead Actors
    vector<Actor*>::iterator it = actors.begin();
    for  (; it != actors.end(); )
    {
        if (!(*it)->isAlive())
        {
            delete (*it);
            it = actors.erase(it);
        }
        else
            it++;
    }
    
    sstream();
    
    if (!socrates->isAlive())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    
    if (levelDone) return GWSTATUS_FINISHED_LEVEL;
     
    // For Part 1, we can assume that Socrates cannot die.
    return GWSTATUS_CONTINUE_GAME;
    
}

void StudentWorld::cleanUp()
{
    // Delete Socrates. good night sweet prince 😔✊✊
    delete socrates;
    
    // Delete all the other actors 🙄
    vector<Actor*>::iterator it = actors.begin();
    for ( ; it != actors.end(); )
    {
        delete *it;
        it = actors.erase(it);
    }
}

void StudentWorld::addActor(Actor* actor)
{
    actors.push_back(actor);
}

bool StudentWorld::isBacteriumMovementBlockedAt(double x, double y) const
{
    double distFromCenter = sqrt(pow(VIEW_WIDTH/2 - x, 2) + pow(VIEW_HEIGHT/2 - y, 2));
    if (distFromCenter >= VIEW_RADIUS) return true;
    
    //std::vector<Actor*>::const_iterator it = actors.begin();
    //for ( ; it != actors.end(); it++)
    for (int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->blocks())
        {
            double blockX = actors[i]->getX();
            double blockY = actors[i]->getY();
            double distance = sqrt(pow(blockX - x, 2) + pow(blockY - y, 2));
            if (distance <= SPRITE_WIDTH/2)
                return true;
        }
            
    }
    
    return false;
}

Actor* StudentWorld::getOverlappingEdible(Actor *overlappingActor) const
{
    std::vector<Actor*>::const_iterator it = actors.begin();
    for ( ; it != actors.end(); it++)
    {
        if (overlap(*it, overlappingActor) && (*it)->isEdible() && (*it)->isAlive())
            return *it;
    }
    
    return nullptr;
}

Socrates* StudentWorld::getOverlappingSocrates(Actor *overlappingActor) const
{
    if (overlap(socrates, overlappingActor))
        return socrates;
    return nullptr;
}

bool StudentWorld::damageOneActor(Actor *actor, int damage)
{
    std::vector<Actor*>::const_iterator it = actors.begin();
    for ( ; it != actors.end(); it++)
    {
        if (overlap(*it, actor) && (*it)->isDamageable() && (*it)->isAlive())
        {
            (*it)->takeDamage(damage);
            return true;
        }
    }
    
    return false;
}

bool StudentWorld::getAngleToNearbySocrates(Actor* a, int dist, int& angle) const
{
    double distance = sqrt(pow(socrates->getX() - a->getX(), 2) + pow(socrates->getY() - a->getY(), 2));
    if (distance <= dist)
    {
        const double PI = 4 * atan(1);
        angle = atan2(socrates->getY() / VIEW_RADIUS - a->getY()/ VIEW_RADIUS, socrates->getX()/VIEW_RADIUS - a->getX()/VIEW_RADIUS) * 180 / PI;
        return true;
    }
    return false;
}

bool StudentWorld::getAngleToNearestNearbyEdible(Actor* a, int dist, int& angle) const
{
    double minDist = dist;
    double minX = 0, minY = 0;
    bool found = false;
    
    for (int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->isEdible())
        {
            double distance = sqrt(pow(a->getX() - actors[i]->getX(), 2) + pow(a->getY() - actors[i]->getY(), 2));
            if (distance < minDist)
            {
                minDist = distance;
                minX = actors[i]->getX()/VIEW_RADIUS - a->getX()/VIEW_RADIUS;
                minY = actors[i]->getY()/VIEW_RADIUS - a->getY()/VIEW_RADIUS;
                found = true;
            }
        }
    }
    
    if (found)
    {
        const double PI = 4 * atan(1);
        angle = atan2(minY, minX) * 180 / PI;
        return true;
    }

    return false;
    
}

void StudentWorld::sstream()
{
    
    ostringstream stream, stream2, stream3, stream4, stream5, stream6;
    stream << right;
    string toScreen;
    
    toScreen = toScreen + "Score: ";
    stream << setw(6);
    stream << setfill('0');
    stream << getScore();
    toScreen = toScreen + stream.str();
    toScreen = toScreen + "  Level: ";
    stream2 << setw(2) << setfill(' ') << getLevel();
    toScreen = toScreen + stream2.str() + "  Lives: ";
    stream3 << setw(2) << setfill(' ') << getLives();
    toScreen = toScreen + stream3.str() + "  Health: ";
    stream4 << setw(3) << setfill(' ') << socrates->getHP();
    toScreen = toScreen + stream4.str() + "  Sprays: ";
    stream5 << setw(2) << setfill(' ') << socrates->getSprayCharges();
    toScreen = toScreen + stream5.str() + "  Flames: ";
    stream6 << setw(2) << setfill(' ') << socrates->getFlameCharges();
    toScreen = toScreen + stream6.str();
    
    setGameStatText(toScreen);
    
}

void StudentWorld::addGoodies()
{
    // Spawn fungus
    int L = getLevel();
    int goodieLifeTime = max(randInt(0, 300-10*L), 50);
    int chanceFungus = max(510-L*10,200);
    int random = randInt(0, chanceFungus - 1);
    double randomX = randInt(1, 100), randomY = randInt(0, 100);
    if (random == 0)
    {
        double x = randomX/VIEW_RADIUS;
        double y = randomY/VIEW_RADIUS ;
        double angle = 0;
        const double PI = 4 * atan(1);
        
        // atan returns values (-180, 180), so we gotta catch the case when its Socrates is in 2nd, 3rd quadrants
        if (x < 0)
            angle = atan(y/x) + PI;
        else
            angle = atan(y/x);
        
        x = VIEW_RADIUS*cos(angle) + VIEW_RADIUS;
        y = VIEW_RADIUS*sin(angle) + VIEW_RADIUS;
        addActor(new Fungus(goodieLifeTime, this, x, y));
    }
    
    //Spawn goodies
    int chanceGoodies = max(510-L*10, 250);
    random = randInt(0, chanceGoodies - 1);
    if (random == 0)
    {
        double x = randomX/VIEW_RADIUS;
        double y = randomY/VIEW_RADIUS ;
        double angle = 0;
        const double PI = 4 * atan(1);
        
        // atan returns values (-180, 180), so we gotta catch the case when its Socrates is in 2nd, 3rd quadrants
        if (x < 0)
            angle = atan(y/x) + PI;
        else
            angle = atan(y/x);
        
        x = VIEW_RADIUS*cos(angle) + VIEW_RADIUS;
        y = VIEW_RADIUS*sin(angle) + VIEW_RADIUS;
        
        random = randInt(1, 10);
        if (random >= 1 && random <= 6) addActor(new RestoreHealthGoodie(goodieLifeTime, this, x, y));
        if (random == 10 )              addActor(new ExtraLifeGoodie(goodieLifeTime, this, x, y));
        else                            addActor(new FlamethrowerGoodie(goodieLifeTime, this, x, y));
        
        
    }
    
    
}

void StudentWorld::addPits()
{
    bool overlappingPits = false;
    if (actors.size() != 0) return;
    int L = getLevel();
    double x = 0, y = 0;
    
    // If actors is empty, add any random Pit
    // Else, generate a Pit and check for overlaps.
    // If it overlaps, delete the Pit and redo the
    // current iteration of the loop by decrementing the counter
    for (int i = 0; i < L; i++)
    {
        getPositionInViewRadius(x, y);
        Pit* pitricia = new Pit(this, x, y);
        if (actors.size() == 0)
            actors.push_back(pitricia);
        else
        {
            vector<Actor*>::iterator it = actors.begin();
            for ( ; it != actors.end(); it++)
            {
                if (overlap((*it), pitricia))
                    overlappingPits = true;
            }
        }
        
        if (overlappingPits)
        {
            delete pitricia;
            i--;
        }
        else if (actors.size() != 1)
            actors.push_back(pitricia);
    }
    
}

void StudentWorld::addFood()
{
    int L = getLevel();
    
    // Spawn Food.
    int numFood = min(5*L,25);
    for (int i = 0; i < numFood; i++)
    {
        bool overlapsPitOrFood = false;
        double x = 0, y = 0;
        getPositionInViewRadius(x, y);
        
        Food* fredTheFoodie = new Food(this, x, y);
        
        vector<Actor*>::iterator it = actors.begin();
        for ( ;it != actors.end(); it++)
        {
            if (overlap((*it), fredTheFoodie))
                overlapsPitOrFood = true;
        }
        
        if (overlapsPitOrFood)
        {
            delete fredTheFoodie; // sad.
            numFood++;
        }
        else
            actors.push_back(fredTheFoodie);
        
    }
}

void StudentWorld::addDirt()
{
    
    int L = getLevel();
    // Spawn Dirt piles.
    int numPiles = max(180-20*L, 20);
    
    for (int i = 0; i < numPiles; i++)
    {
        bool overlapsPitsOrFood = false;
        double x = 0, y = 0;
        getPositionInViewRadius(x, y);
        
        Dirt* dirtyDan = new Dirt(this, x, y);
        
        // Check if our dirt pile overlaps with any food or pits.
        vector<Actor*>::iterator it = actors.begin();
        for( ; it != actors.end(); it++)
        {
            if (overlap((*it), dirtyDan) && !(*it)->blocks())
                overlapsPitsOrFood = true;
        }
        // PROBLEM we dont check for overlap
        
        if (overlapsPitsOrFood)
        {
            delete dirtyDan;  // sad.
            numPiles++;
        }
        //Add dirt to our Actor* container 💩 ➕ 📦[🎭]
        else
            actors.push_back(dirtyDan);
    }
}

bool StudentWorld::overlap(Actor *actor1, Actor *actor2) const
{
    bool overlap = false;
    
    // The distance formula is used to check for an overlap.
    // Two objects are said to overlap if their (x,y) centers
    // are equal to or less than 8 pixels apart (SPRITE_WIDTH)
    double x1 = actor1->getX();
    double y1 = actor1->getY();
    double x2 = actor2->getX();
    double y2 = actor2->getY();
    double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    overlap = (distance <= SPRITE_WIDTH) ? true : false;
    
    return overlap;
}

void StudentWorld::getPositionInViewRadius(double &x, double &y) const
{
    bool valid = false;
    // Generate (x,y) coordinates until they are valid.
    // By valid, we mean that it is within 120 pixels
    // of our circle. The distance formula is used to check.
    while (!valid)
    {
        x = randInt(0 + SPRITE_WIDTH, VIEW_WIDTH - 1);
        y = randInt(0 + SPRITE_HEIGHT, VIEW_HEIGHT - 1);
        double distance = sqrt(pow(x - VIEW_RADIUS, 2) + pow(y - VIEW_RADIUS, 2));
        valid = (distance <= 120) ? true : false;
    }
}

