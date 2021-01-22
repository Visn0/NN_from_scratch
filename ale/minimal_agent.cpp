#include <iostream>
#include <fstream>
#include <cmath>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include "net_t.h"
#include <unistd.h>

// Global vars
const double BOT_THRESHOLD = 0.2;
const bool USE_BOT = !false;
const int maxSteps = 10000;
int lastLives;
float totalReward;
ALEInterface alei;

///////////////////////////////////////////////////////////////////////////////
/// Get info from RAM
///////////////////////////////////////////////////////////////////////////////
void readRamIndexes(const std::string& filename, VecInt_t& indexes)
{
   std::ifstream file;
   file.open(filename);

   if(!file.is_open())
   {
      throw std::runtime_error("[EXCEPTION]: File not found : " + filename);
   }
   std::string line("");           
   
   while(getline(file, line))
   {                            
      indexes.push_back( std::stoi(line) );
   }       
   
   file.close();   
}

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

void writeRAM(int teclas[], const VecInt_t& indexes)
{
   std::ofstream ram_file;

   ram_file.open("x.csv", std::ios::app);
   const auto& RAM = alei.getRAM();
   for(const auto& i: indexes)
   {
      ram_file << std::to_string(RAM.get(i)) << ",";
   }

   // uint8_t add = 0;   

   // for (std::size_t i = 0; i < 8; i++)
   // {
   //    for (std::size_t j = 0; j < 16; j++, add++)
   //    {
   //       // std::fprintf(file, "%02X;", RAM.get(add));
   //       ram_file << std::to_string(RAM.get(add)) << ",";
   //    }
   // }
   
   ram_file << std::endl;
   ram_file.close();

   std::ofstream user_input_file;
   user_input_file.open("y.csv", std::ios::app);      
   for (std::size_t i = 0; i < 4; i++)
   {      
      user_input_file << std::to_string(teclas[i]) << ",";
      // std::cout << "TECLA: " << std::to_string(teclas[i]) << std::endl;
   }   
   user_input_file << std::endl;
   user_input_file.close();
}

///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
VecDouble_t ram_to_VecDouble() {
   const auto& RAM = alei.getRAM();
   uint8_t add = 0;   

   VecDouble_t result(8*16);
   for (std::size_t i = 0; i < 8; i++)
   {
      for (std::size_t j = 0; j < 16; j++, add++)
      {
         // std::fprintf(file, "%02X;", RAM.get(add));
         result[add] = RAM.get(add);
      }
   }   
   
   return result;
}

float playBot(Net_t& bot, int teclas[]) {
   float reward = 0.0;

   VecDouble_t RAM = ram_to_VecDouble();
   VecDouble_t prediction = bot.predict(RAM);

   // print("\n");
   // print(prediction);
   // print("\n");

   // sleep(1);

   // bot
   if (prediction[0] > BOT_THRESHOLD) // UP
   {
      teclas[0] = 1;
      reward += alei.act(PLAYER_A_UP);
   }          

   if (prediction[1] > BOT_THRESHOLD) // SPACE
   {
      teclas[1] = 1;
      if (prediction[2] > prediction[3]) // LEFT > RIGHT
      {
         if (prediction[2] > BOT_THRESHOLD)
         {
            reward += alei.act(PLAYER_A_LEFTFIRE);
            teclas[2] = 1;
         }
      }
      else
      {
         if (prediction[3] > BOT_THRESHOLD)
         {
            reward += alei.act(PLAYER_A_RIGHTFIRE);
            teclas[3] = 1;
         }
      }  
   }
   else
   {
      if (prediction[2] > prediction[3]) // LEFT > RIGHT
      {
         if (prediction[2] > BOT_THRESHOLD)
         {
            reward += alei.act(PLAYER_A_LEFT);
            teclas[2] = 1;
         }
      }
      else
      {
         if (prediction[3] > BOT_THRESHOLD)
         {
            reward += alei.act(PLAYER_A_RIGHT);
            teclas[3] = 1;
         }
      } 
   }

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
      reward += playBot(bot, teclas);
   else
      reward += playManual(teclas);
   
   writeRAM(teclas, indexes);

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
   }

   std::cout << "Steps: " << step << std::endl;
   std::cout << "Reward: " << totalReward << std::endl;

   return 0;
}
