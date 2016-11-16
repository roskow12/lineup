#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <array>
#include <chrono>
#include <stdlib.h>

using std::string;
using std::cout;
using std::endl;
using namespace std::chrono;

#define NUM_PLAYERS 160
#define MIN_AVG 0
#define MIN_PPD .0005
#define TEAM_SIZE 8

template <typename Iterator>
inline bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
   /* Credits: Thomas Draper */
   if ((first == last) || (first == k) || (last == k))
      return false;
   Iterator itr1 = first;
   Iterator itr2 = last;
   ++itr1;
   if (last == itr1)
      return false;
   itr1 = last;
   --itr1;
   itr1 = k;
   --itr2;
   while (first != itr1)
   {
      if (*--itr1 < *itr2)
      {
         Iterator j = k;
         while (!(*itr1 < *j)) ++j;
         std::iter_swap(itr1,j);
         ++itr1;
         ++j;
         itr2 = k;
         std::rotate(itr1,j,last);
         while (last != j)
         {
            ++j;
            ++itr2;
         }
         std::rotate(k,itr2,last);
         return true;
      }
   }
   std::rotate(first,k,last);
   return false;
}


enum class Position : uint8_t {
    PG,
    SG,
    SF,
    PF,
    C
};

const char positionStrings[5][3] = 
{
    {"PG"}, {"SG"}, {"SF"}, {"PF"}, {"C"}
};

class Player
{
public:
    uint16_t salary;
    float avgFP;
    float ppd;
    std::array<Position, 2> positions;
    bool hasTwoPositions;

    Player(){}
    Player(string& po, int sal, float avg, float ppd) :
        salary(sal),
        avgFP(avg),
        ppd(ppd),
        hasTwoPositions(false)
    {
        bool firstPosFound = false;
        if(po.find("PG")) {
            positions[0] = Position::PG;
            firstPosFound = true;
        }

        for(int i=1; i<5; ++i)
        {
            if(po.find(positionStrings[i])) {
                if(firstPosFound) {
                    positions[1] = static_cast<Position>(i);
                    hasTwoPositions = true;
                    return;
                }
                else {
                    positions[0] = static_cast<Position>(i);
                    firstPosFound = true;
                }
            }
        }
    }

    ~Player()
    {}

};

//n-choose-k
void generateCombos1(std::size_t numPlayers) {
    // std::size_t n = numPlayers;
    std::size_t n = 5;
    constexpr std::size_t k = 3;
    // constexpr std::size_t k = TEAM_SIZE;
    // constexpr std::size_t lastElement = TEAM_SIZE - 1;
    constexpr std::size_t lastElement = k - 1;

    std::array<int, k> currentCombination;
    // int* currCombo;
    int i;

    int fuck = 0;

    // fill initial combination is real first combination -1 for last number, 
    // as we will increase it in loop
    for(i=0; i<k; i++) {
        currentCombination[i]=i;    
    }
    currentCombination[lastElement] = k-2; 
    
    do
    {
        if (currentCombination[lastElement] == (n-1) ) // if last number is just before overwhelm
        {
            int i = k-2;
            while (currentCombination[i] == (n-k+i))
                i--;

            currentCombination[i]++;

            for (int j=(i+1); j<k; j++)
                currentCombination[j] = currentCombination[i]+j-i;
        }
        else
            currentCombination[lastElement]++;

        // currCombo = new int[8];
        for(auto i : currentCombination)
            cout << i;
        cout << '\n';
        // memcpy(currCombo, currentCombination, k*sizeof(int));
        fuck++;
        // processCombo(currCombo); 

    } while (! ((currentCombination[0] == (n-k)) && (currentCombination[lastElement] == (n-1))) );

    cout << "num combinations: " << fuck << endl;
}


int main(){

    std::vector<Player> playerVector;
    playerVector.reserve(NUM_PLAYERS);

    std::vector<string> playerNames;
    playerNames.reserve(NUM_PLAYERS);


    string delimiter = ",";

    std::ifstream file;
    file.open("input/DKSalaries.csv");

    string line;
    int pos=0, salary=0;
    string position, name, info;
    float avgFP;
    

    getline(file, line); //waste the categories line
    while(getline(file, line))
    {
        position = line.substr(0, pos = line.find(delimiter));
        line.erase(0, pos + delimiter.length());

        name = line.substr(0, pos = line.find(delimiter));
        line.erase(0, pos + delimiter.length());

        salary = atoi(line.substr(0, pos = line.find(delimiter)).c_str());
        line.erase(0, pos + delimiter.length());

        info = line.substr(0, pos = line.find(delimiter));
        line.erase(0, pos + delimiter.length());

        avgFP = atof(line.substr(0, pos = line.find(delimiter)).c_str());
        line.erase(0, pos + delimiter.length());

        if(avgFP > MIN_AVG)
        {
            float ppd = (float)(avgFP/((float)salary));
            if(ppd > MIN_PPD)
            {
                playerVector.push_back(Player(position, salary, avgFP, ppd));
                playerNames.push_back(name);         
            }
        }
    }

    file.close();

    // int numPlayers = playerVector.size();
    int numPlayers = 20;

    for (int i=0; i < numPlayers; ++i) {
        cout << playerNames[i] << "'s ppd: " << playerVector[i].ppd << '\n';
    }

    cout << numPlayers << std::endl;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    generateCombos1(numPlayers);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration0 = duration_cast<microseconds>(t2-t1).count();
    cout << "Legacy: " << duration0 << '\n';

    return 0;
}
