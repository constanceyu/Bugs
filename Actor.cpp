#include "Actor.h"
#include "StudentWorld.h"
#include "Compiler.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

bool Insect::moveIfPossible(int x, int y)
{
    if(!getWorld()->canBlock(x, y))                                             // if it is not a pebble
    {
        --m_distance;
        moveTo(x, y);
        return true;
    }
    else
        return false;
}

/*virtual*/ void Ant::doSomething()
{
    decay();
    
    if(getHitPoints() <= 0)
    {
        setShouldBeRemoved();
        return;
    }
    resetStunStatus();
    if(getTicksLeft() > 0)
    {
        updateTicks(-1);
        return;
    }
    else
    {
        Compiler::Command currCommand;
        int timesExecuted = 0;
        do{
            if(!getCompiler()->getCommand(m_instCounter, currCommand))
            {
                setShouldBeRemoved();
                return;
            }
            else
                timesExecuted++;
        } while((runCommand(currCommand)) && timesExecuted < 10);
    }
    
}


/*virtual*/ void BabyGrasshopper::doSomething()
{
    decay();
    
    // check if the baby grasshopper has died or not
    if(getHitPoints() <= 0)
    {
        //getWorld()->addFood(getX(), getY());
        setShouldBeRemoved();
        return;
    }
    
    if(getTicksLeft() > 0)                                                        // if the baby grasshopper is sleeping
    {
        updateTicks(-1);
        return;
    }
    else                                                                            // when it's time to move
    {
        if(getHitPoints() >= 1600)
        {
            getWorld()->addActor(this);
            //getWorld()->addFood(getX(), getY());
            setShouldBeRemoved();
            return;
        }
        
        
        resetStunStatus();
        setTicksTo(2);
        
        Actor* food = getWorld()->isFood(getX(), getY());
        if( food != nullptr)                                                        // there is food!
        {
            int energyincrease = getWorld()->eat(food,200);
            updateHitPoints(energyincrease);
            if(randInt(0, 1)==1)                                                  // 50% chance to fall asleep
                return;
        }
        
        if(getDistance() == 0)
        {
            resetStepsAndDirection();
            return;
        }
        
        // attempt to move
        GraphObject::Direction dir = getDirection();
        switch (dir)
        {
            case GraphObject::Direction::up:
                if(!moveIfPossible(getX(), getY()+1))
                    setDistanceTo(0);
                break;
                
                
            case GraphObject::Direction::right:
                if(!moveIfPossible(getX()+1, getY()))
                    setDistanceTo(0);
                break;
                
                
            case GraphObject::Direction::down:
                if(!moveIfPossible(getX(), getY()-1))
                    setDistanceTo(0);
                break;
                
                
            case GraphObject::Direction::left:
                if(!moveIfPossible(getX()-1, getY()))
                    setDistanceTo(0);
                break;
                
                
            default:
                break;
        }
        
        return;
    }
    
}

/*virtual*/ void AdultGrasshopper::doSomething()
{
    decay();
    
    // check if the baby grasshopper has died or not
    if(getHitPoints() <= 0)
    {
        setShouldBeRemoved();
        return;
    }
    
    if(getTicksLeft() > 0)                                                        // if the baby grasshopper is sleeping
    {
        updateTicks(-1);
        return;
    }
    else                                                                            // when it's time to move
    {
        setTicksTo(2);
        
        // 1/3 chance to bite other insects
        if(randInt(0, 2) == 0)
        {
            Actor* poorthing = getWorld()->grasshopperCanBite(this);
            if(poorthing!=nullptr)
            {
                poorthing->updateHitPoints(-50);
                Insect* temp = dynamic_cast<Insect*>(poorthing);
                if(temp != nullptr)
                    temp->wasBitten(true);
            }
            return;
        }
        
        // 1/10 chance to jump
        if(randInt(0, 9)==0)
        {
            getWorld()->grasshopperJump(this);
            return;
        }
        
        
        Actor* food = getWorld()->isFood(getX(), getY());
        if( food != nullptr)                                                        // there is food!
        {
            int energyincrease = getWorld()->eat(food,200);
            updateHitPoints(energyincrease);
            if(randInt(0, 1)==1)                                                  // 50% chance to fall asleep
                return;
        }
        
        if(getDistance() == 0)
        {
            resetStepsAndDirection();
            return;
        }
        
        // attempt to move
        GraphObject::Direction dir = getDirection();
        switch (dir)
        {
            case GraphObject::Direction::up:
                if(!moveIfPossible(getX(), getY()+1))
                    setDistanceTo(0);
                break;
                
                
            case GraphObject::Direction::right:
                if(!moveIfPossible(getX()+1, getY()))
                    setDistanceTo(0);
                break;
                
                
            case GraphObject::Direction::down:
                if(!moveIfPossible(getX(), getY()-1))
                    setDistanceTo(0);
                break;
                
                
            case GraphObject::Direction::left:
                if(!moveIfPossible(getX()-1, getY()))
                    setDistanceTo(0);
                break;
                
                
            default:
                break;
        }
        
        return;
    }
}




/*virtual*/ void Poison::doSomething()
{
    if(getWorld()->canBePoisoned(getX(), getY()))
        getWorld()->poisonAll(getX(), getY());
    //must attempt to poison all Insects on the same square as it
}



/*virtual*/ void Water::doSomething()
{
    if(getWorld()->canBeStunned(getX(), getY()))
        getWorld()->stunAll(getX(), getY());
    //must attempt to stun all Insects on the same square as it.
    //Stunning an insect (if the insect can be stunned), causes it to sleep for 2 additional ticks (above and beyond any existing sleeping the insect is doing).
}


/*virtual*/ void Anthill::doSomething()
{
    //1. It decreases its (queen’s) hit points by 1 unit.
    decay();
    
    //2. If the anthill’s (queen’s) hit points reaches zero units, then:
    if(getHitPoints() <= 0)
    {
        setShouldBeRemoved();
        return;
    }
    
    //3. The anthill checks to see if there is any food on its square. If so:
    Actor* food = getWorld()->isFood(getX(), getY());
    if(food != nullptr)
    {
        int energyincrease = getWorld()->eat(food,10000);
        updateHitPoints(energyincrease);
        return;
    }
    
    //4. The anthill checks to see if it has enough energy - at least 2,000 hit points - to produce a
    //   new ant. If so:
    if(getHitPoints() >= 2000)
    {
        getWorld()->addActor(this);
        updateHitPoints(-1500);
        getWorld()->increaseAntCount(m_colony);
    }
    
}


/*virtual*/ void Pheromone::doSomething()
{
    decay();
    if(getHitPoints() <= 0)
        setShouldBeRemoved();
}


void Ant::moveTheAntForward()
{
    GraphObject::Direction dir = getDirection();
    switch (dir)
    {
        case GraphObject::Direction::up:
            if(moveIfPossible(getX(), getY()+1))
            {
                m_wasBlocked = false;
                wasBitten (false);
            }
            else
            {
                m_wasBlocked = true;
            }
            break;
            
            
        case GraphObject::Direction::right:
            if(moveIfPossible(getX()+1, getY()))
            {
                m_wasBlocked = false;
                wasBitten (false);
            }
            else
            {
                m_wasBlocked = true;
            }
            break;
            
            
        case GraphObject::Direction::down:
            if(moveIfPossible(getX(), getY()-1))
            {
                m_wasBlocked = false;
                wasBitten (false);
            }
            else
            {
                m_wasBlocked = true;
            }
            break;
            
            
        case GraphObject::Direction::left:
            if(moveIfPossible(getX()-1, getY()))
            {
                m_wasBlocked = false;
                wasBitten (false);
            }
            else
            {
                m_wasBlocked = true;
            }
            break;
            
            
        default:
            break;
    }
}

void Ant::rotateTheAntClockwise()
{
    GraphObject::Direction dir = getDirection();
    switch (dir)
    {
        case GraphObject::Direction::up:
            setDirection(Direction::right);
            break;
            
            
        case GraphObject::Direction::right:
            setDirection(Direction::down);
            break;
            
            
        case GraphObject::Direction::down:
            setDirection(Direction::left);
            break;
            
            
        case GraphObject::Direction::left:
            setDirection(Direction::up);
            break;
            
            
        default:
            break;
    }
}

void Ant::rotateCounterClockwise()
{
    GraphObject::Direction dir = getDirection();
    switch (dir)
    {
        case GraphObject::Direction::up:
            setDirection(Direction::left);
            break;
            
            
        case GraphObject::Direction::right:
            setDirection(Direction::up);
            break;
            
            
        case GraphObject::Direction::down:
            setDirection(Direction::right);
            break;
            
            
        case GraphObject::Direction::left:
            setDirection(Direction::down);
            break;
            
            
        default:
            break;
    }
}

void Ant::antEat()
{
    if(m_food >= 100)
    {
        updateHitPoints(100);
        m_food -= 100;
    }
    else
    {
        updateHitPoints(m_food);
        m_food = 0;
    }
}

void Ant::antBite()
{
    Actor* poorthing = getWorld()->isEnemyAt(getX(), getY(), m_colony);
    if(poorthing != nullptr)
    {
        poorthing->updateHitPoints(-15);
        Insect* temp = dynamic_cast<Insect*>(poorthing);
        if(temp != nullptr)
            temp->wasBitten(true);
    }
    
}

void Ant::pickupFood()
{
    if(m_food >= 1800)
        return;
    else
    {
        int space = 1800 - m_food;
        Actor* p = getWorld()->isFood(getX(), getY());
        if(p != nullptr)
        {
            if(space >= 400)
                m_food += getWorld()->eat(p, 400);
            else
                m_food += getWorld()->eat(p, space);
        }
            
    }
}


void Ant::emitPheromone()
{
    Actor* ap = getWorld()->hasPheromone(getX(), getY(), returnColony());
    if(ap == nullptr)
        getWorld()->addActor(this);
    else
    {
        if(ap->getHitPoints() >= 700)
            ap->setHitPoints(768);
        else
            ap->updateHitPoints(256);
    }
    
    /*emitPheromone: Increases the pheromone scent of the current square of the grid by 256 points, up to a maximum of 768. If the pheromone strength already in the square were 700, and the ant emitted a new pheromone, the square’s pheromone strength would jump from 700 to 768. Note: Each ant colony has its own pheromones, so there may be up to 4 different pheromone objects (each with its own strength) in each square of the field. If a square already holds a pheromone object for a given colony of ants, the ant should simply increase the units of that pheromone object rather than create another pheromone object in the same square. (Better: The ant asks the StudentWorld to figure this out,and StudentWorld either creates a new pheromone object or adds units to an existing pheromone object)*/
}


bool Ant::thisConditionIsTrue(int conditionNum)
{
    
    if(conditionNum == 0) // i_smell_danger_in_front_of_me
    {
        GraphObject::Direction dir = getDirection();
        switch (dir)
        {
            case GraphObject::Direction::up:
                return getWorld()->isDangerous(getX(), getY()+1, returnColony());
                break;
                
                
            case GraphObject::Direction::right:
                return getWorld()->isDangerous(getX()+1, getY(), returnColony());
                break;
                
                
            case GraphObject::Direction::down:
                return getWorld()->isDangerous(getX(), getY()-1, returnColony());
                break;
                
                
            case GraphObject::Direction::left:
                return getWorld()->isDangerous(getX()-1, getY(), returnColony());
                break;
                
            default:
                return false;
                break;
        }
    }
    else if (conditionNum == 1) // i_smell_pheromone_in_front_of_me
    {
        GraphObject::Direction dir = getDirection();
        switch (dir)
        {
            case GraphObject::Direction::up:
                return getWorld()->hasPheromone(getX(), getY()+1, returnColony());
                break;
                
                
            case GraphObject::Direction::right:
                return getWorld()->hasPheromone(getX()+1, getY(), returnColony());
                break;
                
                
            case GraphObject::Direction::down:
                return getWorld()->hasPheromone(getX(), getY()-1, returnColony());
                break;
                
                
            case GraphObject::Direction::left:
                return getWorld()->hasPheromone(getX()-1, getY(), returnColony());
                break;
                
            default:
                return false;
                break;
        }
    }
    else if (conditionNum == 2) // i_was_bit
    {
        return returnBitStatus();
    }
    else if (conditionNum == 3) // i_am_carrying_food
    {
        return (m_food > 0);
    }
    else if (conditionNum == 4) // i_am_hungry
    {
        return (getHitPoints() <= 25);
    }
    else if (conditionNum == 5) // i_am_standing_on_my_anthill
    {
        return (getX() == m_anthill->getX() && getY() == m_anthill->getY());
    }
    else if (conditionNum == 6) // i_am_standing_on_food
    {
        Actor* p = getWorld()->isFood(getX(), getY());
        if(p != nullptr)
            return true;
        else
            return false;
    }
    else if (conditionNum == 7) // i_am_standing_with_an_enemy
    {
        Actor* p = getWorld()->isEnemyAt(getX(), getY(), returnColony());
        if(p != nullptr)
            return true;
        else
            return false;
    }
    else if (conditionNum == 8) //i_was_blocked_from_moving
    {
        return m_wasBlocked;
    }
    else if (conditionNum == 9) // last_random_number_was_zero
    {
        return (m_lastRand == 0);
    }
    
    return false;
}



bool Ant::runCommand(const Compiler::Command& c)
{
    if (c.opcode == Compiler::moveForward)
    {
        moveTheAntForward();
        m_instCounter++;
        return false;
    }
    else if (c.opcode == Compiler::rotateClockwise)
    {
        rotateTheAntClockwise();
        m_instCounter++;
        return false;
    }
    else if (c.opcode == Compiler::rotateCounterClockwise)
    {
        rotateCounterClockwise();
        m_instCounter++;
        return false;
    }
    else if (c.opcode == Compiler::eatFood)
    {
        antEat();
        m_instCounter++;
        return false;
    }
    else if (c.opcode == Compiler::dropFood)
    {
        getWorld()->addFood(getX(), getY(),m_food);
        m_food = 0;
        m_instCounter++;
        return false;
    }
    else if (c.opcode == Compiler::bite)
    {
        antBite();
        m_instCounter++;
        return false;
    }
    else if (c.opcode == Compiler::pickupFood)
    {
        pickupFood();
        m_instCounter++;
        return false;
    }
    else if(c.opcode == Compiler::emitPheromone)
    {
        emitPheromone();
        m_instCounter++;
        return false;
    }
    else if(c.opcode == Compiler::faceRandomDirection)
    {
        setDirection(Direction(randInt(1, 4)));
        m_instCounter++;
        return false;
    }
    else if(c.opcode == Compiler::generateRandomNumber)
    {
        int n = std::stoi(c.operand1);
        if(n == 0)
            m_lastRand = 0;
        else
            m_lastRand = randInt(0, n-1);
        m_instCounter++;
        return true;
    }
    else if(c.opcode == Compiler::goto_command)
    {
        m_instCounter = std::stoi(c.operand1);
        return true;
    }
    else if(c.opcode == Compiler::if_command)
    {
        if (thisConditionIsTrue(std::stoi(c.operand1)))
            m_instCounter = stoi(c.operand2);
        else
            m_instCounter++;
        
        return true;
    }
    return false;
}
