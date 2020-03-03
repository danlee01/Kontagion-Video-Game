#include "Actor.h"
#include "StudentWorld.h"

// ACTOR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth)
: GraphObject(imageID, startX, startY, startDirection, depth)
{
    this->status = true;
    this->m_world = world;
}

Actor::~Actor() {}

bool Actor::isAlive() const { return this->status; }

bool Actor::blocks() const { return false; }

void Actor::takeDamage(int damage)
{
    if (isDamageable())
        die();
}

void Actor::die()
{
    playDeathSound();
    this->status = false;
}

bool Actor::move() { return false; }

StudentWorld* Actor::world() const { return this->m_world; }

bool Actor::isEdible() const { return false; }

bool Actor::isDamageable() const { return false; }

bool Actor::isGoodie() const { return false; }

bool Actor::preventsLevelCompletion() const { return false; }

void Actor::playDeathSound() const {}

// DIRT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dirt::Dirt(StudentWorld* world, double startX, double startY)
: Actor(world, IID_DIRT, startX, startY, 0, 1)
{
    // Nothing needed here as Dirt does nothing.
}

Dirt::~Dirt()
{
    // ACTUALLY: Deleting the Dirt object is done by the StudentWorld class.
}

void Dirt::doSomething()
{
    // Dirt does nothing.
}

bool Dirt::blocks() const { return true; }

bool Dirt::isDamageable() const { return true; }

// PIT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Pit::Pit(StudentWorld* world, double startX, double startY)
: Actor(world, IID_PIT, startX, startY, 0, 1)
{}

Pit::~Pit() {}

void Pit::doSomething()
{
    if (randInt(1, 50) == 1)
    {
        bool spawned = false;
        
        while (!spawned)
        {
            int index = randInt(0,2);
            if (bacteriaArr[index] != 0)
            {
                     if (index == 0) world()->addActor(new RegularSalmonella(world(), getX(), getY()));
                else if (index == 1) world()->addActor(new AggressiveSalmonella(world(), getX(), getY()));
                else if (index == 2) world()->addActor(new EColi(world(), getX(), getY()));
                bacteriaArr[index] -= 1;
                spawned = true;
            }
        }
        world()->playSound(SOUND_BACTERIUM_BORN);
    }
    if (pitEmpty()) die();
}

bool Pit::preventsLevelCompletion() const
{
    if (pitEmpty())
        return false;
    return true;
}

bool Pit::pitEmpty() const
{
    bool empty = true;
    for (int i = 0; i < 3; i++)
    {
        if (bacteriaArr[i] != 0) empty = false;
    }
    
    return empty;
}

// HEALTHYACTOR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
HealthyActor::HealthyActor(int hp, StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth)
: Actor(world, imageID, startX, startY, startDirection, depth)
{
    this->hp = hp;
}

HealthyActor::~HealthyActor() {}

void HealthyActor::incHP(int inc) { this->hp += inc; }

void HealthyActor::decHP(int dec)
{
    this->hp -= dec;
    if (this->hp <= 0 )
        die();
    
    if (isAlive())
        world()->playSound(SOUND_PLAYER_HURT);
}

int HealthyActor::getHP() const { return this->hp; }

void HealthyActor::setHP(int hp) { this->hp = hp; }

// SOCRATES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Socrates::Socrates(StudentWorld* world, int startX, int startY)
: HealthyActor(100, world, IID_PLAYER, startX, startY , 0, 0)
{
    this->sprayCharges = 20;
    this->flameCharges = 5;
}

Socrates::~Socrates() {}

void Socrates::doSomething()
{
    int key = 0;
    
    if (world()->getKey(key))
    {
        switch(key)
        {
            case KEY_PRESS_LEFT:
            case 'a':
                // move 5 degree in positional angle counter-clockwise
                move(KEY_PRESS_LEFT);
                setDirection(getDirection() + 5);
                break;
                
            case KEY_PRESS_RIGHT:
            case 'd':
                // move 5 degrees in positional angle clockwise
                move(KEY_PRESS_RIGHT);
                setDirection(getDirection() - 5);
                break;
                
            case KEY_PRESS_SPACE:
                if (sprayCharges > 0)
                {
                    world()->playSound(SOUND_PLAYER_SPRAY);
                    double dx = 0, dy = 0;
                    getPositionInThisDirection(getDirection(), 0, dx, dy);
                    DisinfectantSpray* spray = new DisinfectantSpray(world(), dx, dy, getDirection());
                    world()->addActor(spray);
                    --sprayCharges;
                }
                break;
            case KEY_PRESS_ENTER:
                if (flameCharges > 0)
                {
                    world()->playSound(SOUND_PLAYER_FIRE);
                    double dx = 0, dy = 0;
                    for (int i = 0; i < 16; i++)
                    {
                        getPositionInThisDirection(22*i, 0, dx, dy);
                        Flame* flame = new Flame(world(), dx, dy, 22*i);
                        world()->addActor(flame);  
                    }
                    --flameCharges;
                }
                break;
            case KEY_PRESS_ESCAPE:
                die();
                break;
            default:
                break;
        }
    }
    else
        if (sprayCharges < 20)
            sprayCharges += 1;
}

void Socrates::incHP(int inc)
{
    if (getHP() + inc > 100)
        inc = 100 - getHP();
    
    setHP(getHP() + inc);
}

void Socrates::takeDamage(int damage)
{
    decHP(damage);
}

void Socrates::addFlameCharges(int charges)
{
    // WARNING!! This is assuming that Socrates does not have a FlameCharge cap.
    this->flameCharges += charges;
}

int Socrates::getFlameCharges() const { return flameCharges; }

int Socrates::getSprayCharges() const { return sprayCharges; }

void Socrates::move(const int KEY_PRESS)
{

    // Convert to unit circle, then transform
    double x = getX()/VIEW_RADIUS - 1;
    double y = getY()/VIEW_RADIUS - 1;
    double angle = 0;
    const double PI = 4 * atan(1);
    const double fiveD = ( KEY_PRESS == KEY_PRESS_LEFT) ? 5*PI/180 : -5*PI/180;
    
    // atan returns values (-180, 180), so we gotta catch the case when its Socrates is in 2nd, 3rd quadrants
    if (x < 0)
        angle = atan(y/x) + PI + fiveD;
    else
        angle = atan(y/x) + fiveD;
    
    x = VIEW_RADIUS*cos(angle) + VIEW_RADIUS;
    y = VIEW_RADIUS*sin(angle) + VIEW_RADIUS;
    
    moveTo(x, y);
}

void Socrates::playDeathSound() const
{
    world()->playSound(SOUND_PLAYER_DIE);
}

// BACTERIUM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bacterium::Bacterium(int nFood, int movementDistancePlan, int hp, StudentWorld* world, int imageID, double startX, double startY)
: HealthyActor(hp, world, imageID, startX, startY, 90, 0)
{
    this->nFood = nFood;
    this->movementDistancePlan = movementDistancePlan;
    this->overlapsWithSocrates = false;
}

Bacterium::~Bacterium() {}

void Bacterium::die()
{
    Actor::die();
    
    if (randInt(1, 2) == 1)
        world()->addActor(new Food(world(), getX(), getY()));
}

bool Bacterium::tryMove()
{
    double newX = 0, newY = 0;
    Actor* edible = world()->getOverlappingEdible(this);
    Socrates* socrates = world()->getOverlappingSocrates(this);
    if (isOverlappingWithSocrates())
    {
        socrates->takeDamage(getDamage());
    }
    else if (canMultiply(newX, newY))
    {
        addBacterium(newX, newY);  //change this to addBacterium(newX, newY)
    }
    else if (edible != nullptr)
        eat(edible);
    
    return true;
}

bool Bacterium::isDamageable() const { return true; }

bool Bacterium::preventsLevelCompletion() const { return true; }

bool Bacterium::canMultiply(double &newX, double &newY)
{
    if (nFood == 3)
    {
        double changeX = (getX() < VIEW_WIDTH/2) ? SPRITE_WIDTH/2 : -SPRITE_WIDTH/2;
        double changeY = (getY() < VIEW_HEIGHT/2) ? SPRITE_WIDTH/2 : -SPRITE_WIDTH/2;
        
        if (getX() != VIEW_WIDTH/2) newX = getX() + changeX;
        if (getY() != VIEW_HEIGHT/2) newY = getY() + changeY;
        
        nFood = 0;
        
        return true;
    }
    return false;
}


void Bacterium::eat(Actor* edible)
{
    if (edible != nullptr)
    {
        setFood(getFood()+1);
        edible->die();
    }
}

int Bacterium::getFood() const { return nFood; }

int Bacterium::getMovementDistancePlan() const { return this->movementDistancePlan; }

void Bacterium::setFood(int num) { this->nFood = num; }

void Bacterium::setMovementDistancePlan(int num) { this->movementDistancePlan = num; }

bool Bacterium::isOverlappingWithSocrates()
{
    Socrates* socrates = world()->getOverlappingSocrates(this);
    if (socrates != nullptr)
        overlapsWithSocrates = true;
    else
        overlapsWithSocrates = false;
    return overlapsWithSocrates;
}

// SALMONELLA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Salmonella::Salmonella(int hp, StudentWorld* world, double startX, double startY)
: Bacterium(0, 0, hp, world, IID_SALMONELLA, startX, startY)
{
}

Salmonella::~Salmonella() {}

bool Salmonella::tryMove()
{
    bool moved = false;
    int angle = 0;
    
    Bacterium::tryMove();
    
    if (getMovementDistancePlan() > 0 && !moved && !isOverlappingWithSocrates())
        moved = move();
    
    if (!moved && !isOverlappingWithSocrates())
    {
        if (world()->getAngleToNearestNearbyEdible(this, VIEW_RADIUS, angle))
        {
            setDirection(angle);
            moved = move();
            if (moved) return true;
        }
        else
        {
            setDirection(randInt(0, 359));
            setMovementDistancePlan(10);
        }
    }
    return false;
}

void Salmonella::takeDamage(int damage)
{
    decHP(damage);
    
    if(isAlive())
        world()->playSound(SOUND_SALMONELLA_HURT);
    else
        world()->increaseScore(100);
}

bool Salmonella::move()
{
    double dx = 0, dy = 0;
    bool blocked = false;
    // We will check if MDP is > 0 before we use move(), but this check is just in case.
    if (getMovementDistancePlan() > 0)
    {
        setMovementDistancePlan(getMovementDistancePlan()-1);
        getPositionInThisDirection(getDirection(), 3, dx, dy);
        blocked = world()->isBacteriumMovementBlockedAt(dx, dy);
        
        if (blocked)
        {
            setDirection(randInt(0, 359));
            setMovementDistancePlan(10);
            return false;
        }
        else
            moveAngle(getDirection(), 3);
    }
    setMovementDistancePlan(10);
    return true;
}

void Salmonella::playDeathSound() const
{
    world()->playSound(SOUND_SALMONELLA_DIE);
}

// REGULAR SALMONELLA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RegularSalmonella::RegularSalmonella(StudentWorld* world, double startX, double startY)
: Salmonella(4, world, startX, startY)
{}

RegularSalmonella::~RegularSalmonella() {}

void RegularSalmonella::doSomething()
{
    if (!isAlive()) return;

    Salmonella::tryMove();
}

void RegularSalmonella::addBacterium(double newX, double newY) const
{
    world()->addActor(new RegularSalmonella(world(), newX, newY));
}

int RegularSalmonella::getDamage() const { return 1; }

// AGGRESSIVE SALMONELLA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AggressiveSalmonella::AggressiveSalmonella(StudentWorld* world, double startX, double startY)
: Salmonella(10, world, startX, startY)
{}

AggressiveSalmonella::~AggressiveSalmonella() {}

void AggressiveSalmonella::doSomething()
{
    if (!isAlive()) return;
    
    int angle = 0;
    bool seesSocrates = world()->getAngleToNearbySocrates(this, 72, angle);
    bool moved = false;
    
    if (seesSocrates && !isOverlappingWithSocrates())
    {
        setDirection(angle);
        moved = move();
    }
    
    if (!moved)
        tryMove();

}

void AggressiveSalmonella::addBacterium(double newX, double newY) const
{
    world()->addActor(new AggressiveSalmonella(world(), newX, newY));
}

int AggressiveSalmonella::getDamage() const { return 2; }

// ECOLI ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EColi::EColi(StudentWorld* world, double startX, double startY)
: Bacterium(0, 0, 5, world, IID_ECOLI, startX, startY)
{}

EColi::~EColi() {}

void EColi::takeDamage(int damage)
{
    decHP(damage);
    
    if(isAlive())
        world()->playSound(SOUND_ECOLI_HURT);
    else
        world()->increaseScore(100);
}

void EColi::doSomething()
{
    if (!isAlive()) return;
    int angle = 0;
    bool seesSocrates = false;
    
    Bacterium::tryMove();
    
    seesSocrates = world()->getAngleToNearbySocrates(this, 256, angle);
    
    if (seesSocrates)
    {
        setDirection(angle);
        move();
    }
}

bool EColi::move()
{
    double dx = 0, dy = 0;
    bool blocked = false;
    for (int i = 0; i < 10; i++)
    {
        getPositionInThisDirection(getDirection(), 2, dx, dy);
        blocked = world()->isBacteriumMovementBlockedAt(dx, dy);
        if (!blocked)
        {
            moveAngle(getDirection(), 2);
            return true;
        }
        else
            setDirection(getDirection() + 10);
    }
    return false;
    
}

int EColi::getDamage() const { return 4; }

void EColi::addBacterium(double newX, double newY) const
{
    world()->addActor(new EColi(world(), newX, newY));
}

void EColi::playDeathSound() const
{
    world()->playSound(SOUND_ECOLI_DIE);
}

// FOOD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Food::Food(StudentWorld* world, int startX, int startY)
: Actor(world, IID_FOOD, startX, startY, 90, 1)
{
    // Nothing needed here as Food doesn't do anything
}

Food::~Food() {}

void Food::doSomething()
{
    // Food does nothing.
}

bool Food::isEdible() const { return true; }

// GOODIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Goodie::Goodie(int remainingTicks, StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth)
: Actor(world, imageID, startX, startY, startDirection, depth)
{
    // Note: the max(randInt(0, 300-10*L-1), 50) will by passed in as remainingTicks by StudentWorld
    this->remainingTicks = remainingTicks;
}

Goodie::~Goodie() {}

void Goodie::doSomething()
{
    if (!isAlive()) return;
    Socrates* socrates = world()->getOverlappingSocrates(this);
    if (socrates != nullptr)
    {
        pickUp(socrates);
        world()->playSound(SOUND_GOT_GOODIE);
        die();
    }
    
    remainingTicks--;
    if (remainingTicks <= 0)
        die();
}

int Goodie::getRemainingTicks() const { return this->remainingTicks; }

bool Goodie::isDamageable() const { return true; }

bool Goodie::isGoodie() const { return true; }


// RESTORE HEALTH GOODIE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
RestoreHealthGoodie::RestoreHealthGoodie(int remainingTicks, StudentWorld* world, double startX, double startY)
: Goodie(remainingTicks, world, IID_RESTORE_HEALTH_GOODIE, startX, startY, 0, 1)
{}

RestoreHealthGoodie::~RestoreHealthGoodie() {}

void RestoreHealthGoodie::pickUp(Socrates* socrates)
{
    // user get 250 points
    world()->increaseScore(250);
    socrates->incHP(100);
}

// FLAMETHROWER GOODIE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FlamethrowerGoodie::FlamethrowerGoodie(int remainingTicks, StudentWorld* world, double startX, double startY)
: Goodie(remainingTicks, world, IID_FLAME_THROWER_GOODIE, startX, startY, 0, 1)
{}

FlamethrowerGoodie::~FlamethrowerGoodie() {}

void FlamethrowerGoodie::pickUp(Socrates *socrates)
{
    // TO-DO: tell studentworld to give user 300 points
    world()->increaseScore(300);
    socrates->addFlameCharges(5);
}

// EXTRA LIFE GOODIE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ExtraLifeGoodie::ExtraLifeGoodie(int remainingTicks, StudentWorld* world, double startX, double startY)
: Goodie(remainingTicks, world, IID_EXTRA_LIFE_GOODIE, startX, startY, 0, 1)
{}

ExtraLifeGoodie::~ExtraLifeGoodie() {}

void ExtraLifeGoodie::pickUp(Socrates *socrates)
{
    // TO-DO: user gets 500 points
    world()->increaseScore(500);
    world()->incLives();
}

// FUNGUS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fungus::Fungus(int remainingTicks, StudentWorld* world, double startX, double startY)
: Goodie(remainingTicks, world, IID_FUNGUS, startX, startY, 0, 1)
{}

Fungus::~Fungus() {}

void Fungus::pickUp(Socrates *socrates)
{
    // TO-DO: user gets -50 points
    world()->increaseScore(-50);
    socrates->decHP(20);
}

// PROJECTILE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Projectile::Projectile(int travelDistance, StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth)
: Actor(world, imageID, startX, startY, startDirection, 1)
{
    this->travelDistance = travelDistance;
}

Projectile::~Projectile() {}

void Projectile::doSomething()
{
    if (!isAlive()) return;
    
    if (world()->damageOneActor(this, getDamage()))
        die();
    
    move();
}


bool Projectile::move()
{
    moveAngle(getDirection(), SPRITE_WIDTH);
    setTravelDist(getTravelDist() - SPRITE_WIDTH);
    
    if (getTravelDist() <= 0)
        die();
    
    return true;
}

int Projectile::getTravelDist() const { return this->travelDistance; }

void Projectile::setTravelDist(int dist) { this->travelDistance = dist; }

// FLAME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Flame::Flame(StudentWorld* world, int startX, int startY, int startDirection)
: Projectile(32 + SPRITE_WIDTH, world, IID_FLAME, startX, startY, startDirection, 1)
{}

Flame::~Flame() {}

void Flame::doSomething() { Projectile::doSomething(); }

int Flame::getDamage() const { return 5; }


// DISINFECTANT SPRAY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DisinfectantSpray::DisinfectantSpray(StudentWorld* world, int startX, int startY, int startDirection)
: Projectile(112 + SPRITE_WIDTH, world, IID_SPRAY, startX, startY, startDirection, 1)
{}

DisinfectantSpray::~DisinfectantSpray() {}

void DisinfectantSpray::doSomething() { Projectile::doSomething(); }

int DisinfectantSpray::getDamage() const { return 2; }

