#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

//#include "Actor.h" cant do this or help we get stuck in circular includes
#include "GameWorld.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Socrates;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // addActor(Actor* actor)
    // Adds an actor to our StudentWorld
    // Primarily used by bacterium to multiply after consuming enough Food.
    void addActor(Actor* actor);
    
    // isBacteriumMovementBlockedAt(Actor* actor, double x, double y)
    // Returns true if Bacterium will be blocked at (x,y)
    bool isBacteriumMovementBlockedAt(double x, double y) const;
    
    // getOverlappingEdible(Actor* overlappingActor)
    // Returns an edible Actor* if it exists, otherwise nullptr.
    Actor* getOverlappingEdible(Actor* overlappingActor) const;
    
    // getOverlappingSocrates(Actor* overlappingActor)
    // Returns Socrates if some dunderhead collides wid da BOI
    Socrates* getOverlappingSocrates(Actor* overlappingActor) const;
    
    // damageOneActor(Actor* actor, int damage)
    // Damages one living, damageable actor and return true.
    bool damageOneActor(Actor* actor, int damage);
    
    // Return true if this world's socrates is within the indicated distance
    // of actor a; otherwise false.  If true, angle will be set to the
    // direction from actor a to the socrates.
    bool getAngleToNearbySocrates(Actor* a, int dist, int& angle) const;
    
    // getAngleToNearestNearbyEdible(Actor* a, int dist, int& angle)
    // Return true if there is a living edible object within the indicated
    // distance from actor a; otherwise false.  If true, angle will be set
    // to the direction from actor a to the edible object nearest to it
    bool getAngleToNearestNearbyEdible(Actor* a, int dist, int& angle) const;
    
private:
    Socrates* socrates;
    std::vector<Actor*> actors;
    
    // sstream()
    // Prints text to screen
    void sstream();
    
    void addGoodies();
    // NOTE: THE FOLLOWING FUNCTIONS SHOULD BE CALLED IN THEIR RESPECTIVE ORDERS BELOW
    
    // addPits()
    // Populates the StudentWorld with Pits as long as
    // there are no existing actors in our actors container.
    // Therefore, this MUST be called before our other populator functions.
    void addPits();
    
    // addFood()
    // Populates the StudentWorld with Food that
    // does not overlap with existing Pits or each other
    void addFood();
    
    // addDirt()
    // Populates the StudentWorld with Dirt that
    // does not overlap with existing Pits or edible Actors
    void addDirt();
    
    // overlap(Actor* actor1, Actor* actor2)
    // Returns true if actor1 and actor2 are equal to or less than 8 pixels apart
    bool overlap(Actor* actor1, Actor* actor2) const;
    
    // getPositionInViewRadius(double& x, double&y)
    // Returns a position whose distance is <= 120px from the
    // center of the circle using the x and y REFERENCES
    void getPositionInViewRadius(double& x, double&y) const;
};

#endif // STUDENTWORLD_H_
