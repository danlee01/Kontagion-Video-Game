#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

#include "StudentWorld.h"
// We use include instead of forward-declaring the class bc we need to use its functions

class Actor : public GraphObject
{
public:
    // Constructor needs to initialize GraphObject thru initializer list in .cpp
    Actor(StudentWorld* world,int imageID, double startX, double startY,
          int startDirection = 0, int depth = 0);
    
    virtual ~Actor();
    
    // doSomething()
    // Holds logic for what our actor does.
    virtual void doSomething() = 0;
    
    // isAlive()
    // Returns the status of our actor.
    // (Alive/Dead) -> (true/false), respectively.
    // Will be used by StudentWorld -> must be public
    bool isAlive() const;
    
    // blocks()
    // Holds logic for whether or not an Actor blocks other Actors
    virtual bool blocks() const;
    
    // takeDamage()
    // Holds logic on how an Actor takes damage
    virtual void takeDamage(int damage);
    
    // die()
    // Sets status to false
    virtual void die();
    
    // move()
    // Holds logic for how Actor moves
    virtual bool move();

    // world() 
    // Returns a pointer to the StudentWorld
    virtual StudentWorld* world() const;
    
    // isEdible()
    // Returns true if Actor is a food
    virtual bool isEdible() const;
    
    // isDamageable()
    // Returns true if Actor is damageable (by Socrates)
    // i.e. bacterium, goodie, fungus, dirt pile
    virtual bool isDamageable() const;
    
    // isGoodie()
    // Returns true if Actor is a goodie
    virtual bool isGoodie() const;
    
    // preventsLevelCompletion()
    // Returns true if Actor prevents level completion
    virtual bool preventsLevelCompletion() const;

    
private:
    bool status;
    StudentWorld* m_world;
    
    virtual void playDeathSound() const;
};

class Dirt : public Actor
{
public:
    Dirt(StudentWorld* world, double startX, double startY);
    ~Dirt();
    
    virtual void doSomething();
    
    virtual bool blocks() const;
    
    virtual bool isDamageable() const;
private:
};

class Pit : public Actor
{
public:
    Pit(StudentWorld* world, double startX, double startY);
    virtual ~Pit();
    
    virtual void doSomething();
    
    virtual bool preventsLevelCompletion() const;
private:
    bool pitEmpty() const;
    int bacteriaArr[3] = {5, 3, 2};
};

class HealthyActor : public Actor
{
public:
    HealthyActor(int hp, StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth);
    virtual ~HealthyActor();
    
    // incHP(int inc)
    // Increases HealthyActors hp.
    // This function is virtual because Socrates can pick up health goodies and has a max hp.
    // Thus, we must compute it differently.
    virtual void incHP(int inc);
    
    // decHP(int dec)
    // Decrease HealthyActors hp.
    // If hp reaches zero, HealthyActor dies.
    void decHP(int dec);
    
    int getHP() const;
    
protected:
    void setHP(int hp);
    
private:
    int hp;
};



class Socrates : public HealthyActor
{
public:
    Socrates(StudentWorld* world, int startX=0, int startY=VIEW_RADIUS);
    virtual ~Socrates();
    
    virtual void doSomething();
            
    virtual void incHP(int inc);

    virtual void takeDamage(int damage);
    
    void addFlameCharges(int charges);
    
    int getFlameCharges() const;
    
    int getSprayCharges() const;
    
private:
    void move(const int KEY_PRESS);
    int sprayCharges;
    int flameCharges;
    virtual void playDeathSound() const;
};

class Bacterium : public HealthyActor
{
public:
    Bacterium(int nFood, int movementDistancePlan, int hp, StudentWorld* world, int imageID, double startX, double startY);
    virtual ~Bacterium();
    
    virtual void die();
    
    // tryMove()
    // Tests for possible actions a Bacterium can take.
    // Always returns true
    virtual bool tryMove();
    
    virtual bool isDamageable() const;
    
    virtual bool preventsLevelCompletion() const;
    
    // canMultiply(double& newX, double& newY)
    // Returns true if Bacterium has consumed enough food to generate another Bacterium
    bool canMultiply(double& newX, double& newY);
    
    // move()
    // Holds logic for how all Bacterium move.
    virtual bool move() = 0;
    
    // eat(Actor* edible)
    // Holds logic for when a Bacterium overlaps with an edible Actor.
    void eat(Actor* edible);
    
protected:
    int getFood() const;
    int getMovementDistancePlan() const;
    void setFood(int num);
    void setMovementDistancePlan(int num);
    virtual int getDamage() const = 0;
    bool isOverlappingWithSocrates();
    
private:
    virtual void addBacterium(double newX, double newY) const = 0;
    int nFood;
    int movementDistancePlan;
    bool overlapsWithSocrates;
};

class Salmonella : public Bacterium
{
public:
    Salmonella(int hp, StudentWorld* world, double startX, double startY);
    ~Salmonella();
    
    // tryMove()
    // Holds logic for how a Salmonella Bacterium will move.
    virtual bool tryMove();
    
    virtual void takeDamage(int damage);
    
    virtual bool move();
    
private:
    virtual void playDeathSound() const;
    
};

class RegularSalmonella : public Salmonella
{
public:
    RegularSalmonella(StudentWorld* world, double startX, double startY);
    virtual ~RegularSalmonella();
    
    virtual void doSomething();
    
protected:
    virtual int getDamage() const;
    
private:
    virtual void addBacterium(double newX, double newY) const;
    
};

class AggressiveSalmonella : public Salmonella
{
public:
    AggressiveSalmonella(StudentWorld* world, double startX, double startY);
    ~AggressiveSalmonella();
    
    virtual void doSomething();
    
protected:
    virtual int getDamage() const;
    
private:
    virtual void addBacterium(double newX, double newY) const;
};

class EColi : public Bacterium
{
public:
    EColi(StudentWorld* world, double startX, double startY);
    ~EColi();
    
    virtual void takeDamage(int damage);
    
    virtual bool move();
    
    virtual void doSomething();
    
protected:
    int getDamage() const;
    virtual void playDeathSound() const;
    
private:
    virtual void addBacterium(double newX, double newY) const;
};

class Goodie: public Actor
{
public:
    Goodie(int remainingTicks, StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth);
    virtual ~Goodie();
    
    virtual void doSomething();
    
    virtual bool isDamageable() const;
    
    virtual bool isGoodie() const;
    
    int getRemainingTicks() const;
    
    virtual void activate(Actor* toThisGuy) {}
    
protected:
    // pickUp(Socrates* socrates)
    // Holds logic for what happens to Socrates and the Goodie when it is picked up.
    virtual void pickUp(Socrates* socrates) = 0;
    
private:
    int remainingTicks;
};

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(int remainingTicks, StudentWorld* world, double startX, double startY);
    virtual ~RestoreHealthGoodie();
    
protected:
    virtual void pickUp(Socrates* socrates);
};

class FlamethrowerGoodie : public Goodie
{
public:
    FlamethrowerGoodie(int remainingTicks, StudentWorld* world, double startX, double startY);
    virtual ~FlamethrowerGoodie();
        
protected:
    virtual void pickUp(Socrates* socrates);
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(int remainingTicks, StudentWorld* world, double startX, double startY);
    virtual ~ExtraLifeGoodie();
    
protected:
    virtual void pickUp(Socrates* socrates);
};

class Fungus : public Goodie
{
public:
    Fungus(int remainingTicks, StudentWorld* world, double startX, double startY);
    virtual ~Fungus();
    
protected:
    virtual void pickUp(Socrates* socrates);
};

class Food : public Actor
{
public:
    Food(StudentWorld* world, int startX, int startY);
    virtual ~Food();
    
    virtual void doSomething();
    
    virtual bool isEdible() const;
        
private:
    
};

class Projectile : public Actor
{
public:
    Projectile(int travelDistance, StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth);
    virtual ~Projectile();
    
    virtual void doSomething();
    
    virtual int getDamage() const = 0;
    
    virtual bool move();
    
    int getTravelDist() const;
    
    void setTravelDist(int dist);
    
private:
    int travelDistance;
};

class Flame : public Projectile
{
public:
    Flame(StudentWorld* world, int startX, int startY, int startDirection);
    ~Flame();
    
    virtual void doSomething();
    
    virtual int getDamage() const;
    
};

class DisinfectantSpray : public Projectile
{
public:
    DisinfectantSpray(StudentWorld* world, int startX, int startY, int startDirection);
    ~DisinfectantSpray();
    
    virtual void doSomething();
    
    virtual int getDamage() const;
    
    
};

#endif // ACTOR_H_
