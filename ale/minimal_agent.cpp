#include <iostream>
#include <fstream>
#include <cmath>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include "net_t.h"
#include <unistd.h>

// Global vars
const double BOT_THRESHOLD = 0.3;
const bool USE_BOT = !false;
const int maxSteps = 10000;
int lastLives;
float totalReward;
ALEInterface alei;

///////////////////////////////////////////////////////////////////////////////
/// Get info from RAM
///////////////////////////////////////////////////////////////////////////////
int getPlayerX() {
   return alei.getRAM().get(72) + ((rand() % 3) - 1);
}

int getBallX() {
   return alei.getRAM().get(99) + ((rand() % 3) - 1);
}

void cls() { std::printf("\033[2J"); }

/*
double hex2dec(double hexd)
{
   std::string hex = std::to_string(hexd);
    unsigned long result = 0;
    for (int i=0; i<hex.length(); i++) {
        if (hex[i]>=48 && hex[i]<=57)
        {
            result += (hex[i]-48)*pow(16,hex.length()-i-1);
        } else if (hex[i]>=65 && hex[i]<=70) {
            result += (hex[i]-55)*pow(16,hex.length( )-i-1);
        } else if (hex[i]>=97 && hex[i]<=102) {
            result += (hex[i]-87)*pow(16,hex.length()-i-1);
        }
    }
    return result;
}
*/

void showRAM()
{
   const auto& RAM = alei.getRAM();

   uint8_t add = 0;
   std::printf("\033[H");
   std::printf("\nAD |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
   std::printf("\n====================================================");
   for (std::size_t i = 0; i < 8; i++)
   {
      std::printf("\n%02X | ", add);
      for (std::size_t j = 0; j < 16; j++, add++)
      {
         std::printf("%02X ", RAM.get(add));
      }
   }
   std::printf("\n====================================================\n");
}

void writeRAM(int teclas[])
{
   if (USE_BOT) return;

   std::ofstream ram_file;

   ram_file.open("x.csv", std::ios::app);
   const auto& RAM = alei.getRAM();
   for(std::size_t i = 0; i < 127; ++i)
   {
      ram_file << std::to_string(RAM.get(i)) << ",";
   }
   ram_file << std::to_string(RAM.get(127));
   ram_file << std::endl;
   ram_file.close();

   std::ofstream user_input_file;
   user_input_file.open("y.csv", std::ios::app);      
   for (std::size_t i = 0; i < 3; i++)
   {      
      user_input_file << std::to_string(teclas[i]) << ",";
      // std::cout << "TECLA: " << std::to_string(teclas[i]) << std::endl;
   }   
   user_input_file << std::to_string(teclas[3]);
   user_input_file << std::endl;
   user_input_file.close();
}

///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
VecDouble_t ram_to_VecDouble(const VecInt_t& indexes) {
   
   VecDouble_t result(indexes.size()); 

   const auto& RAM = alei.getRAM();
   for(const auto& i: indexes)
   {
      result[i] = RAM.get(i);
   }
   
   return result;
}

void saveBotPrediction(const VecDouble_t &prediction)
{
   std::ofstream file;

   file.open("bot_predictions.txt", std::ios::app);

   file << "( ";
   for(const auto& p: prediction)
   {
      file << p << " ";
   }
   file << ")" << std::endl;

   file.close();
}

float playBot(Net_t& bot, int teclas[], VecInt_t const &indexes) {
   float reward = 0.0;

   VecDouble_t RAM = ram_to_VecDouble(indexes);
   VecDouble_t prediction = bot.predict(RAM);   

   saveBotPrediction(prediction);
   //
   // PREDICTION
   // ----------
   // [0] = UP
   // [1] = SPACE+LEFT
   // [2] = SPACE+RIGHT
   // [3] = LEFT
   // [4] = RIGHT
   //
           

   // coger el mayor !=  up
   // if mayor > threshold then seguimos
   // if mayor == up then execute up and check left || right
   // if mayor == spaceLeft || spaceRigh then run it

   // std::size_t maxIndex = 0;
   // for(std::size_t i = 1; i < prediction.size(); ++i)
   // {
   //    if (prediction[maxIndex] < prediction[i])
   //    {
   //       maxIndex = i;
   //    }
   // }

   // print(prediction);
   // sleep(1);

   // if (prediction[maxIndex] > BOT_THRESHOLD)
   // {
   //    switch (maxIndex)
   //    {
   //       case 0:
   //          teclas[0] = 1;
   //          reward += alei.act(PLAYER_A_UP);

   //          if (prediction[3] > prediction[4] && prediction[3] > BOT_THRESHOLD*0.8)
   //          {
   //             reward += alei.act(PLAYER_A_LEFT);
   //             teclas[2] = 1;
   //          }
   //          else if (prediction[4] > BOT_THRESHOLD*0.8)
   //          {
   //             reward += alei.act(PLAYER_A_RIGHT);
   //             teclas[3] = 1;
   //          }

   //          break;

   //       case 1:
   //          teclas[1] = 1;
   //          teclas[2] = 1;
   //          reward += alei.act(PLAYER_A_LEFTFIRE);            
   //          break;
         
   //       case 2:
   //          teclas[1] = 1;
   //          teclas[3] = 1;
   //          reward += alei.act(PLAYER_A_RIGHTFIRE);            
   //          break;

   //       case 3:
   //          reward += alei.act(PLAYER_A_LEFT);
   //          teclas[2] = 1;
   //          break;

   //       case 4:
   //          reward += alei.act(PLAYER_A_RIGHT);
   //          teclas[3] = 1;
         
   //       default:
   //          break;
   //    }
   // }

   // FIRE UP
<<<<<<< HEAD
   if (prediction[0] > BOT_THRESHOLD) // UP
   {
      teclas[0] = 1;
      reward += alei.act(PLAYER_A_UP);
   }  

   // FIRE LEFT OR RIGHT
   if (prediction[1] > prediction[2] && prediction[1] > BOT_THRESHOLD)
   {
      teclas[1] = 1;
      teclas[2] = 1;
      reward += alei.act(PLAYER_A_LEFTFIRE);    
      return reward;   
   }
   else if (prediction[2] > BOT_THRESHOLD)
   {
      teclas[1] = 1;
      teclas[3] = 1;
      reward += alei.act(PLAYER_A_RIGHTFIRE);       
      return reward;    
   } 

   // MOVEMENT LEFT OR RIGHT
   if (prediction[3] > prediction[4] && prediction[3] > BOT_THRESHOLD)
   {
      reward += alei.act(PLAYER_A_LEFT);
      teclas[2] = 1;
   }
   else if (prediction[4] > BOT_THRESHOLD)
   {
      reward += alei.act(PLAYER_A_RIGHT);
      teclas[3] = 1;
   }
=======
   // if (prediction[0] > BOT_THRESHOLD) // UP
   // {
   //    teclas[0] = 1;
   //    reward += alei.act(PLAYER_A_UP);
   // }  

   // // FIRE LEFT OR RIGHT
   // if (prediction[1] > prediction[2] && prediction[1] > BOT_THRESHOLD)
   // {
   //    teclas[1] = 1;
   //    teclas[2] = 1;
   //    reward += alei.act(PLAYER_A_LEFTFIRE);    
   //    return reward;   
   // }
   // else if (prediction[2] > BOT_THRESHOLD)
   // {
   //    teclas[1] = 1;
   //    teclas[3] = 1;
   //    reward += alei.act(PLAYER_A_RIGHTFIRE);       
   //    return reward;    
   // } 

   // // MOVEMENT LEFT OR RIGHT
   // if (prediction[3] > prediction[4] && prediction[3] > BOT_THRESHOLD)
   // {
   //    reward += alei.act(PLAYER_A_LEFT);
   //    teclas[2] = 1;
   // }
   // else if (prediction[4] > BOT_THRESHOLD)
   // {
   //    reward += alei.act(PLAYER_A_RIGHT);
   //    teclas[3] = 1;
   // }
>>>>>>> 2d56884bc8513ba96e7cca83fbc73ad435eb0ac6

   return reward;
}

float playManual(int teclas[]) {
   float reward = 0.0;

   auto* keystate = SDL_GetKeyState(0);
   // manual
   if (keystate [SDLK_UP])
   {
      teclas[0] = 1;
      reward += alei.act(PLAYER_A_UP);
   }          

   if (keystate[SDLK_SPACE])
   {
      teclas[1] = 1;
      if (keystate [SDLK_LEFT])
      {
         teclas[2] = 1;
         reward += alei.act(PLAYER_A_LEFTFIRE);
      }        
      else if (keystate[SDLK_RIGHT])
      {
         teclas[3] = 1;
         reward += alei.act(PLAYER_A_RIGHTFIRE);
      }   
   }
   else
   {
      if (keystate [SDLK_LEFT])
      {
         reward += alei.act(PLAYER_A_LEFT);
         teclas[2] = 1;
      }        
      else if (keystate[SDLK_RIGHT])
      {
         reward += alei.act(PLAYER_A_RIGHT);
         teclas[3] = 1;
      } 
   }

   return reward;
}

float agentStep(Net_t& bot, bool useBot, const VecInt_t& indexes) {
   float reward = 0;

   showRAM();

   int teclas[4];
   //
   // 0 - UP
   // 1 - SPACE
   // 2 - LEFT
   // 3 - RIGHT
   //
   for (std::size_t i = 0; i < 4; i++)
   {
      teclas[i] = 0;
   }

   if(useBot)
      reward += playBot(bot, teclas, indexes);
   else
      reward += playManual(teclas);
   
   writeRAM(teclas);

   return (reward + alei.act(PLAYER_A_NOOP));
}

///////////////////////////////////////////////////////////////////////////////
/// Print usage and exit
///////////////////////////////////////////////////////////////////////////////
void usage(char* pname) {
   std::cerr
      << "\nUSAGE:\n" 
      << "   " << pname << " <romfile>\n";
   exit(-1);
}

///////////////////////////////////////////////////////////////////////////////
/// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
   // Check input parameter
   if (argc != 2)
      usage(argv[0]);

   // Create alei object.
   alei.setInt("random_seed", 0);
   alei.setFloat("repeat_action_probability", 0);
   alei.setBool("display_screen", true);
   alei.setBool("sound", true);
   alei.loadROM(argv[1]);

   // Init
   srand(time(NULL));
   lastLives = alei.lives();
   totalReward = .0f;

   // Main loop
   alei.act(PLAYER_A_FIRE);
   int step;
   cls();
   
   Net_t bot("../ale_bot.csv");         

   VecInt_t indexes;
   readRamIndexes("../ramindexes.txt", indexes);      

   for (step = 0; 
        !alei.game_over() && step < maxSteps; 
        ++step) 
   {
      totalReward += agentStep(bot, USE_BOT, indexes);
      bot.printArchitecture();
   }

   std::cout << "Steps: " << step << std::endl;
   std::cout << "Reward: " << totalReward << std::endl;

   return 0;
}