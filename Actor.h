#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Compiler.h"
#include "GameConstants.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* world, int depth)
    : GraphObject(imageID, startX, startY, dir, depth), m_world(world), m_shouldBeRemoved(false)//, m_justMoved(false)
    {}
    
    virtual void doSomething() = 0;
    virtual bool shouldBeRemoved() const {return m_shouldBeRemoved;}
    void setShouldBeRemoved() {m_shouldBeRemoved = true;}
    
    int getHitPoints() const {return m_hitPoints;}
    void setHitPoints(int n) {m_hitPoints = n;}
    void updateHitPoints(int amt) {m_hitPoints += amt;}
    void decay(){updateHitPoints(-1);}              // for Anthill, Pheromone, and Insects
    
    StudentWorld* getWorld() const {return m_world;}

    //void setJustMoved(bool value) {m_justMoved = value;}
    //bool justMoved() {return m_justMoved;}
private:
    StudentWorld*   m_world;
    //bool            m_justMoved;
    bool            m_shouldBeRemoved;
    int             m_hitPoints;                    // to be implemented in derived classes
};


class Pebble : public Actor
{
public:
    Pebble( int startX, int startY, StudentWorld* world)
    : Actor(IID_ROCK, startX, startY, Direction::right, world, 1)
    {setHitPoints(999999);}                         // just to make sure its m_HitPoint doesn't because 0 when the tick is counting
    
    virtual void doSomething() {}                   // do nothing
};


class EnergyHolder : public Actor
{
public:
    EnergyHolder (int imageID, int startX, int startY, Direction dir, StudentWorld* world,int depth, int hitPoints)
    : Actor(imageID,startX,startY,dir,world,depth),m_energy(hitPoints)
    {setHitPoints(hitPoints);}
    
    int getEnergy() const {return m_energy;}
    void setEnergy(int amt) {m_energy = amt;}
private:
    int m_energy;
};

class Poison : public Actor
{
public:
    Poison(int startX, int startY, StudentWorld* world)
    : Actor(IID_POISON, startX, startY, Direction::right, world, 2)
    {setHitPoints(999999);}                         // just to make sure its m_HitPoint doesn't because 0 when the tick is counting
    
    virtual void doSomething();                     // must attempt to poison all Insects on the same square as it
};


class Water : public Actor
{
public:
    Water(int startX, int startY, StudentWorld* world)
    : Actor(IID_WATER_POOL, startX, startY, GraphObject::right, world, 2)
    {setHitPoints(999999);}                         // just to make sure its m_HitPoint doesn't because 0 when the tick is counting
    
    virtual void doSomething();                     //must attempt to stun all Insects on the same square as it.
                                                    //Stunning an insect (if the insect can be stunned), causes it to sleep for 2 additional ticks (above and beyond any existing sleeping the insect is doing).
};






class Anthill: public EnergyHolder
{
public:
    Anthill(int startX, int startY, StudentWorld* world, int colony, Compiler* compiler)
    : EnergyHolder(IID_ANT_HILL, startX, startY, GraphObject::right, world, 2, 8999),m_colony(colony),m_compiler(compiler)
    {}
    int getColony() const {return m_colony;}
    Compiler* getCompiler() const {return m_compiler;}
    
    virtual void doSomething();                     // see the .cpp file
    
private:
    int m_colony;
    Compiler* m_compiler;
};





class Food : public EnergyHolder
{
public:
    Food (int sx, int sy, StudentWorld* sw, int energy)
    : EnergyHolder(IID_FOOD, sx, sy, Direction::right, sw, 2, energy)
    {}
    
    virtual void doSomething() {}                   // do nothing
};




class Pheromone : public EnergyHolder
{
public:
    Pheromone (int sx, int sy, StudentWorld* sw, int imageID)
    :EnergyHolder(imageID, sx,sy,Direction::right,sw,2,256)
    {}
    
    virtual void doSomething();
};




//************************* THE HARD PART ************************************





class Insect : public Actor
{
public:
    Insect(int imageID, int startX, int startY, Direction dir, StudentWorld* world, int hp)
    : Actor(imageID, startX, startY, dir, world, 1), m_ticksLeft(0),m_wasBitten(false),m_stunned(false)
    {
        setHitPoints(hp);
        m_distance = randInt(2, 10);
    }
    
    void resetStepsAndDirection()
    {
        setDirection(GraphObject::Direction(randInt(1, 4)));
        m_distance = randInt(2, 10);
    }
    
    bool moveIfPossible(int x, int y);
    
    virtual void doSomething() = 0;
    
    int getTicksLeft() {return m_ticksLeft;}
    void setTicksTo(int amt) {m_ticksLeft = amt;}
    void updateTicks(int amt) {m_ticksLeft += amt;}
    
    int getDistance() {return m_distance;}
    void setDistanceTo(int amt) {m_distance = amt;}
    
    void getStunned() {m_ticksLeft += 2;}
    bool returnStunnedStatus() {return m_stunned;}
    void resetStunStatus() {m_stunned = false;}
    
    void getPoisoned() {updateHitPoints(-150);}
    
    void wasBitten(bool value) {m_wasBitten = value;}
    bool returnBitStatus() const {return m_wasBitten;}
    void getBittenByAdultGrasshopper() {updateHitPoints(-50); m_wasBitten = true;}
    void getBittenByAnt() {updateHitPoints(-15); m_wasBitten = true;}
    
    
private:
    int m_ticksLeft;
    bool m_stunned;
    int m_distance;
    bool m_wasBitten;
};

class Ant : public Insect
{
public:
    Ant(int startX, int startY, StudentWorld* sw, int colony, Compiler* program, int imageID, Anthill* anthill)
    :Insect(imageID,startX,startY,GraphObject::Direction(randInt(1, 4)),sw, 1500),m_colony(colony),m_compiler(program),m_wasBlocked(false),m_lastRand(0),m_instCounter(0),m_food(0),m_anthill(anthill)
    {}
    
    virtual void doSomething();
    int returnColony() const {return m_colony;}
    
    // returns true if the simple commands were run, false if goto_, if_, or rand was run)
    bool runCommand(const Compiler::Command& c);
    Compiler* getCompiler() const {return m_compiler;}
    
    
    
    // command helpers
    void moveTheAntForward();
    void rotateTheAntClockwise();
    void rotateCounterClockwise();
    void antEat();
    void antBite();
    void pickupFood();
    void emitPheromone();
    bool thisConditionIsTrue(int conditionNum);
    
    
    
private:
    int m_colony;
    int m_lastRand;
    int m_instCounter;                  // starts with 0, line of the command
    int m_food;
    bool m_wasBlocked;
    Compiler* m_compiler;
    Anthill* m_anthill;
};





class Grasshopper : public Insect
{
public:
    Grasshopper(int imageID, int startX, int startY, StudentWorld* world, int hp)
    : Insect(imageID, startX, startY, GraphObject::Direction(randInt(1, 4)), world, hp)
    {}
};



class BabyGrasshopper : public Grasshopper
{
public:
    BabyGrasshopper(int startX, int startY, StudentWorld* world)
    : Grasshopper(IID_BABY_GRASSHOPPER, startX, startY, world, 500)
    {}
    
    virtual void doSomething();

};


class AdultGrasshopper : public Grasshopper
{
public:
    AdultGrasshopper(int startX, int startY, StudentWorld* world)
    : Grasshopper(IID_ADULT_GRASSHOPPER, startX, startY, world, 1600)
    {}
    
    virtual void doSomething();
};




#endif // ACTOR_H_
