#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <sstream>
#include <iostream>
#include <iomanip>
#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include "Actor.h"
#include <string>
#include <list>
#include <cmath>
#include <algorithm>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}

	virtual int init()
	{
        //If the field data file exists but is not in the proper format (the field loader class we provide will return an error to you if this is the case), the init() method must return GWSTATUS_LEVEL_ERROR
        
        Field f;
        std::string fieldFile = getFieldFilename();
        if (f.loadField(fieldFile) != Field::LoadResult::load_success)
            return GWSTATUS_LEVEL_ERROR;
        
        
        std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
        compilerForEntrant0 = new Compiler;
        std::string error;
        
        if ( ! compilerForEntrant0->compile(fileNames[0], error) )
        {
            setError(fileNames[0] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        if(fileNames.size() > 1)
        {
            compilerForEntrant1 = new Compiler;
            if ( ! compilerForEntrant1->compile(fileNames[1], error) )
            {
                setError(fileNames[1] + " " + error);
                return GWSTATUS_LEVEL_ERROR;
            }
        }
        if(fileNames.size() > 2)
        {
            compilerForEntrant2 = new Compiler;
            if ( ! compilerForEntrant2->compile(fileNames[2], error) )
            {
                setError(fileNames[2] + " " + error);
                return GWSTATUS_LEVEL_ERROR;
            }
        }
        if(fileNames.size() > 3)
        {
            compilerForEntrant3 = new Compiler;
            if ( ! compilerForEntrant3->compile(fileNames[3], error) )
            {
                setError(fileNames[3] + " " + error);
                return GWSTATUS_LEVEL_ERROR;
            }
        }
        
        
        
        // using a two dimensional array of linked list to store objects
        for (int i = 0; i < VIEW_WIDTH; i++)
            for (int j = 0; j < VIEW_HEIGHT; j++)
            {
                Field::FieldItem item = f.getContentsOf(i, j);
                switch(item)
                {
                    case Field::FieldItem::rock:
                        m_grid[i][j].push_back(new Pebble(i, j, this));
                        break;
                        
                    case Field::FieldItem::grasshopper:
                        m_grid[i][j].push_back(new BabyGrasshopper(i, j,this));
                        break;
                        
                    case Field::FieldItem::poison:
                        m_grid[i][j].push_back(new Poison(i,j,this));
                        break;
                        
                    case Field::FieldItem::food:
                        m_grid[i][j].push_back(new Food(i,j, this, 6000));
                        break;
                        
                    case Field::FieldItem::water:
                        m_grid[i][j].push_back(new Water(i,j,this));
                        break;
                        
                    case Field::FieldItem::anthill0:
                        if(compilerForEntrant0 != nullptr)
                            m_grid[i][j].push_back(new Anthill(i,j,this, 0, compilerForEntrant0));
                        break;
                        
                    case Field::FieldItem::anthill1:
                        if(compilerForEntrant1 != nullptr)
                            m_grid[i][j].push_back(new Anthill(i,j,this, 1, compilerForEntrant1));
                        break;
                        
                    case Field::FieldItem::anthill2:
                        if(compilerForEntrant2 != nullptr)
                            m_grid[i][j].push_back(new Anthill(i,j,this, 2, compilerForEntrant2));
                        break;
                        
                    case Field::FieldItem::anthill3:
                        if(compilerForEntrant3 != nullptr)
                            m_grid[i][j].push_back(new Anthill(i,j,this, 3, compilerForEntrant3));
                        break;
                        
                    default:
                        break;
                }
            }
        
        
		return GWSTATUS_CONTINUE_GAME;
	}
    
    /*void resetAllActors()
    {
        for (int i = 0; i < VIEW_WIDTH; i++)
            for (int j = 0; j < VIEW_HEIGHT; j++)
            {
                std::list<Actor*>::iterator it = m_grid[i][j].begin();
                while(it != m_grid[i][j].end())
                {
                    if((*it)!=nullptr)
                        (*it)->setJustMoved(false);
                    
                    
                    it++;
                }
            }
    }*/
    
    
    bool canBlock(int x, int y)
    {
        if(x >= 63 || x <= 0 || y >= 63 || y <= 0)
            return true;
        
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                
                if((*it)->getID() == IID_ROCK)
                    return true;
                it++;

            }
        }
        return false;
        
    }
    
    
    bool canBeStunned(int x, int y)
    {
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                if((*it)->getID() == IID_BABY_GRASSHOPPER
                   ||(*it)->getID() == IID_ANT_TYPE0
                   ||(*it)->getID() == IID_ANT_TYPE1
                   ||(*it)->getID() == IID_ANT_TYPE2
                   ||(*it)->getID() == IID_ANT_TYPE3)
                {
                    Insect* ip = dynamic_cast<Insect*>(*it);
                    if(ip != nullptr)
                        if(!ip->returnStunnedStatus())              // if the insect was not previously stunned
                            return true;
                }
                it++;
            }
        }
        return false;
    }
    
    void stunAll(int x, int y)
    {
        std::list<Actor*>::iterator it = m_grid[x][y].begin();
        while (it != m_grid[x][y].end())
        {
            if((*it)->getID() == IID_BABY_GRASSHOPPER
               ||(*it)->getID() == IID_ANT_TYPE0
               ||(*it)->getID() == IID_ANT_TYPE1
               ||(*it)->getID() == IID_ANT_TYPE2
               ||(*it)->getID() == IID_ANT_TYPE3)
            {
                Insect* ip = dynamic_cast<Insect*>(*it);
                if(ip != nullptr)
                    ip->getStunned();
            }
            it++;
        }
    }

    bool canBePoisoned(int x, int y)
    {
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                if((*it)->getID() == IID_BABY_GRASSHOPPER
                   ||(*it)->getID() == IID_ANT_TYPE0
                   ||(*it)->getID() == IID_ANT_TYPE1
                   ||(*it)->getID() == IID_ANT_TYPE2
                   ||(*it)->getID() == IID_ANT_TYPE3)
                {
                    return true;
                }
                it++;
            }
        }
        return false;
    }
    
    void poisonAll(int x, int y)
    {
        std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                if((*it)->getID() == IID_BABY_GRASSHOPPER
                   ||(*it)->getID() == IID_ANT_TYPE0
                   ||(*it)->getID() == IID_ANT_TYPE1
                   ||(*it)->getID() == IID_ANT_TYPE2
                   ||(*it)->getID() == IID_ANT_TYPE3)
                {
                    Insect* ip = dynamic_cast<Insect*>(*it);
                    if(ip != nullptr)
                        ip->getPoisoned();
                    if(ip -> getHitPoints() < 0)
                        ip->setShouldBeRemoved();
                }
                it++;
            }
    }
    
    
    Actor* isFood(int x, int y)
    {
        if(x >= 63 || x <= 0 || y >= 63 || y <= 0)
            return nullptr;
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                if((*it)->getID() == IID_FOOD)
                    return (*it);
                it++;
            }
        }
        return nullptr;
    }
    
    
    
    void addFood(int x, int y)          // it's always 100 units so we don't need a separate parameter
    {
        auto p = isFood(x,y);
        if(p == nullptr)
        {
            m_grid[x][y].push_back(new Food(x,y,this,100));
        }
        else
        {
            p->updateHitPoints(100);
        }
    }
    
    // for Ant class
    void addFood(int x, int y, int amt)
    {
        auto p = isFood(x,y);
        if(p == nullptr)
        {
            m_grid[x][y].push_back(new Food(x,y,this,amt));
        }
        else
        {
            p->updateHitPoints(amt);
        }
    }
    
    
    // anthill & grasshopper
    // for ants it's more like carry
    int eat(Actor* food, int amt)
    {
        if(food->getHitPoints() > amt)
        {
            food->updateHitPoints(-amt);
            return amt;
        }
        else
        {
            food->setShouldBeRemoved();
            return (food->getHitPoints());
        }
    }
    
    
    
    // for Ant class
    bool isDangerous (int x, int y, int colony)
    {
        if(x >= 63 || x <= 0 || y >= 63 || y <= 0)
            return true;
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                Ant* ant = dynamic_cast<Ant*>(*it);
                if(ant != nullptr && (ant->returnColony() != colony))
                    return true;
                else if((*it)->getID() == IID_BABY_GRASSHOPPER
                    ||(*it)->getID() == IID_ADULT_GRASSHOPPER
                    ||(*it)->getID() == IID_POISON
                    ||(*it)->getID() == IID_WATER_POOL)
                    return true;
                it++;
            }
        }
        return false;
    }
    
    // for Ant class
    Actor* hasPheromone (int x, int y, int colony)
    {
        if(x >= 63 || x <= 0 || y >= 63 || y <= 0)
            return nullptr;
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                switch(colony)
                {
                    case 0:
                        if((*it)->getID() == IID_PHEROMONE_TYPE0)
                            return (*it);
                        return nullptr;
                        break;
                    case 1:
                        if((*it)->getID() == IID_PHEROMONE_TYPE1)
                            return (*it);
                        return nullptr;
                        break;
                    case 2:
                        if((*it)->getID() == IID_PHEROMONE_TYPE2)
                            return (*it);
                        return nullptr;
                        break;
                    case 3:
                        if((*it)->getID() == IID_PHEROMONE_TYPE3)
                            return (*it);
                        return nullptr;
                        break;
                    default:
                        break;
                }
                it++;
            }
        }
        return nullptr;
    }
    
    
    // for Ant class
    Actor* isEnemyAt(int x, int y, int colony)
    {
        if(x >= 63 || x <= 0 || y >= 63 || y <= 0)
            return nullptr;
        if((m_grid[x][y]).size() != 0)
        {
            std::list<Actor*>::iterator it = m_grid[x][y].begin();
            while (it != m_grid[x][y].end())
            {
                if(randInt(0, 1)==0)
                {
                    if((*it)->getID() == IID_BABY_GRASSHOPPER
                       ||(*it)->getID() == IID_ADULT_GRASSHOPPER)
                        return (*it);
                    else
                    {
                        Ant* ant = dynamic_cast<Ant*>(*it);
                        if(ant != nullptr && (ant->returnColony() != colony))
                            return (*it);
                    }
                }
                else
                {
                    Ant* ant = dynamic_cast<Ant*>(*it);
                    if(ant != nullptr && ( ant->returnColony() != colony))
                        return (*it);
                    else
                    {
                        if((*it)->getID() == IID_BABY_GRASSHOPPER
                           ||(*it)->getID() == IID_ADULT_GRASSHOPPER)
                            return (*it);
                    }
                }
                it++;
            }
        }
        return nullptr;
    }
    
    
    
    // this could be used to add pheromone in the future
    void addActor(Actor* p)
    {
        int x = p->getX();
        int y = p->getY();
        
        BabyGrasshopper* bp = dynamic_cast<BabyGrasshopper*>(p);
        if(bp != nullptr)
        {
            m_grid[x][y].push_back(new AdultGrasshopper(x,y,this));
            return;
        }
        
        Anthill* antp = dynamic_cast<Anthill*>(p);
        if(antp != nullptr)
        {
            int colony = antp->getColony();
            if(colony == 0)
                m_grid[x][y].push_back(new Ant(x,y,this,colony, antp->getCompiler(),IID_ANT_TYPE0,antp));
            else if(colony == 1)
                m_grid[x][y].push_back(new Ant(x,y,this,colony, antp->getCompiler(),IID_ANT_TYPE1,antp));
            else if(colony == 2)
                m_grid[x][y].push_back(new Ant(x,y,this,colony, antp->getCompiler(),IID_ANT_TYPE2,antp));
            else if(colony == 3)
                m_grid[x][y].push_back(new Ant(x,y,this,colony, antp->getCompiler(),IID_ANT_TYPE3,antp));
            return;
        }
        
        Ant* ap = dynamic_cast<Ant*>(p);
        if(ap != nullptr)
        {
            int col = ap->returnColony();
            if(col == 0)
                m_grid[x][y].push_back(new Pheromone(x,y,this,IID_PHEROMONE_TYPE0));
            else if (col == 1)
                m_grid[x][y].push_back(new Pheromone(x,y,this,IID_PHEROMONE_TYPE1));
            else if (col == 2)
                m_grid[x][y].push_back(new Pheromone(x,y,this,IID_PHEROMONE_TYPE2));
            else if (col == 3)
                m_grid[x][y].push_back(new Pheromone(x,y,this,IID_PHEROMONE_TYPE3));
            return;
        }
    }
    
    
    
    // p is the grasshopper that temps to bite (calls the function)
    // returns the random pointer to the insect that is going to be bitten
    Actor* grasshopperCanBite(Actor* p)
    {
        int x = p->getX();
        int y = p->getY();
        std::list<Actor*>::iterator it = m_grid[x][y].begin();
        while (it != m_grid[x][y].end())
        {
            if((*it)->getID() == IID_BABY_GRASSHOPPER
               ||(*it)->getID() == IID_ADULT_GRASSHOPPER
               ||(*it)->getID() == IID_ANT_TYPE0
               ||(*it)->getID() == IID_ANT_TYPE1
               ||(*it)->getID() == IID_ANT_TYPE2
               ||(*it)->getID() == IID_ANT_TYPE3)
            {
                if(p != (*it))
                    return (*it);
            }
            it++;
        }
        return nullptr;
    }
    
    
    void grasshopperJump(Actor* p)
    {
        int x = p->getX();
        int y = p->getY();
        
        for(;;)
        {

            int rand = randInt(0, 10);
            int pi = 4 * atan(1.0);
            int chance = randInt(0, 3);
            
            int t = 2* pi * rand;
            int u = 2*rand;
            int r = u > 10 ? 20 - u : u;                // condition ? result_if_true : result_if_false
            
            int newX = x + r*cos(t);
            int newY = y + r*sin(t);
            
            if(chance == 0)
            {
                newX = x - r*cos(t);
                newY = y - r*sin(t);
            }
            else if(chance == 1)
            {
                newX = x + r*cos(t);
                newY = y - r*sin(t);
            }
            else if (chance == 2)
            {
                newX = x - r*cos(t);
                newY = y + r*sin(t);
            }
            
            if(newX >= VIEW_WIDTH || newX < 0 || newY >= VIEW_WIDTH || newY < 0 || canBlock(newX, newY))
            {
                continue;
            }
            
            p->moveTo(newX, newY);
            
            break;
        }
    }
    
    
    // returns the colony # of the leading colony
    int findWinner()
    {
        if(antCount0 > std::fmax(antCount1,std::fmax(antCount2,antCount3)))
            return 0;
        if(antCount1 > std::fmax(antCount0,std::fmax(antCount2,antCount3)))
            return 1;
        if(antCount2 > std::fmax(std::fmax(antCount0,antCount1),antCount3))
            return 2;
        if(antCount3 > std::fmax(std::fmax(antCount0,antCount1),antCount2))
            return 3;
        return -1;
    }
    
    void setDisplayText()
    {
        // get colony names for display
        if(compilerForEntrant0 != nullptr)
            file0 = compilerForEntrant0->getColonyName();
        if(compilerForEntrant1 != nullptr)
            file1 = compilerForEntrant1->getColonyName();
        if(compilerForEntrant2 != nullptr)
            file2 = compilerForEntrant2->getColonyName();
        if(compilerForEntrant3 != nullptr)
            file3 = compilerForEntrant3->getColonyName();
        
        
        m_currentWinner = findWinner();
        
        
        // adding the star to the current winner
        if(m_currentWinner == 0)
            file0 += "*";
        else if(m_currentWinner == 1)
            file1 += "*";
        else if(m_currentWinner == 2)
            file2 += "*";
        else if(m_currentWinner == 3)
            file3 += "*";
        
        
        if(file0 != "")
            file0 += ": ";
        if(file1 != "")
            file1 += ": ";
        if(file2 != "")
            file2 += ": ";
        if(file3 != "")
            file3 += ": ";
        
        
        std::ostringstream s;
        if(compilerForEntrant1!=nullptr)
            s << "Ticks:" << std::setw(5) << m_ticks << " - " << file0 << antCount0 << "  " << file1 << antCount1;
        if(compilerForEntrant2!=nullptr)
            s << "Ticks:" << std::setw(5) << m_ticks << " - " << file0 << antCount0 << "  " << file1 << antCount1 << "  " << file2 << antCount2;
        if(compilerForEntrant3 != nullptr)
            s << "Ticks:" << std::setw(5) << m_ticks << " - " << file0 << antCount0 << "  " << file1 << antCount1 << "  " << file2 << antCount2 << " " << file3 << antCount3;
        std::string k = s.str();
        setGameStatText(k);
    }
    
	virtual int move()
	{
        updateTickCount();
        
        //resetAllActors();
        
        for (int i = 0; i < VIEW_WIDTH; i++)
            for (int j = 0; j < VIEW_HEIGHT; j++)
            {
                if(!m_grid[i][j].empty())
                {
                    std::list<Actor*>::iterator it = m_grid[i][j].begin();
                    while(it != m_grid[i][j].end())
                    {
                        // get the actor’s current location
                        int oldX = (*it)->getX();
                        int oldY = (*it)->getY();
                        
                        if(!(*it)->shouldBeRemoved() )
                        {
                            //if(!(*it)->justMoved())
                                (*it)->doSomething();
                            
                            //(*it)->setJustMoved(true);
                        }
                        
                        // erase the old position from the grid
                        if(oldX != (*it)->getX() || oldY != (*it)->getY())
                        {
                            Actor* temp = (*it);
                            int newX = (*it)->getX();
                            int newY = (*it)->getY();
                            m_grid[newX][newY].push_back(temp);
                            it = m_grid[oldX][oldY].erase(it);          // increment iterator
                        }
                        else
                            it++;
                    }
                }
                
            }
        
        
        eraseDead();
        
        
        int winnerNum =findWinner();
        if (winnerNum!=-1){
            if (winnerNum == 0)
                setWinner(compilerForEntrant0->getColonyName());
            else if (winnerNum == 1)
                setWinner(compilerForEntrant1->getColonyName());
            else if (winnerNum == 2)
                setWinner(compilerForEntrant2->getColonyName());
            else if (winnerNum == 3)
                setWinner(compilerForEntrant3->getColonyName());
        }
        
        
        setDisplayText();
        
        if(m_ticks == 2000)
        {
            if (antCount0 <= 5 && antCount1 <=5 && antCount2<=5 && antCount3<=5)
                return GWSTATUS_NO_WINNER;
            else{
                return GWSTATUS_PLAYER_WON;
            }
        }
        
		// the simulation is not yet over, continue!
        return GWSTATUS_CONTINUE_GAME;
	}
    

	virtual void cleanUp()
	{
        for(int i = 0; i < VIEW_WIDTH; i++)
            for(int j = 0; j < VIEW_HEIGHT; j++)
            {
                if(m_grid[i][j].size()!=0)
                {
                    std::list<Actor*>::iterator it = m_grid[i][j].begin();
                    for(; it != m_grid[i][j].end();)
                    {
                        delete *it;
                        it = m_grid[i][j].erase(it);
                    }
                }
            }
	}
    
    
    
    virtual void updateTickCount()
    {
        m_ticks++;
    }
    
    virtual void increaseAntCount(int colony)
    {
        switch(colony)
        {
            case 0:
                antCount0++;
                break;
            case 1:
                antCount1++;
                break;
            case 2:
                antCount2++;
                break;
            case 3:
                antCount3++;
                break;
            default:
                break;
        }
    }
    
    void eraseDead()
    {
        for(int i = 0; i < VIEW_WIDTH; i++)
            for(int j = 0; j < VIEW_HEIGHT; j++)
            {
                std::list<Actor*>::iterator it = m_grid[i][j].begin();
                while(it != m_grid[i][j].end())
                {
                    if((*it)->shouldBeRemoved())
                    {
                        if((*it)->getID() == IID_BABY_GRASSHOPPER
                           ||(*it)->getID() == IID_ADULT_GRASSHOPPER
                           ||(*it)->getID() == IID_ANT_TYPE0
                           ||(*it)->getID() == IID_ANT_TYPE1
                           ||(*it)->getID() == IID_ANT_TYPE2
                           ||(*it)->getID() == IID_ANT_TYPE3)
                            addFood(i, j);
                        delete (*it);
                        it = m_grid[i][j].erase(it);
                        
                    }
                    else
                        it++;
                }
            }
    }
    
    ~StudentWorld() {
        cleanUp();
        delete compilerForEntrant0;
        delete compilerForEntrant1;
        delete compilerForEntrant2;
        delete compilerForEntrant3;
    }

private:
    std::list<Actor*> m_grid [VIEW_HEIGHT][VIEW_WIDTH];
    int m_ticks = 0;
    Compiler *compilerForEntrant0 = nullptr;
    Compiler *compilerForEntrant1 = nullptr;
    Compiler *compilerForEntrant2 = nullptr;
    Compiler *compilerForEntrant3 = nullptr;
    std::string file0 = "";
    std::string file1= "";
    std::string file2= "";
    std::string file3= "";
    int antCount0 = 0;
    int antCount1 = 0;
    int antCount2 = 0;
    int antCount3 = 0;
    int m_currentWinner = -1;
};

#endif // STUDENTWORLD_H_
