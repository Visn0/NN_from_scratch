#include <iostream>
#include <cmath>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>

// Global vars
const int maxSteps = 7500;
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
      if (i == 3)
      {
         std::printf("\n");
      }
      
      std::printf("\n%02X | ", add);
      for (std::size_t j = 0; j < 16; j++, add++)
      {
         std::printf("%02X ", RAM.get(add));
      }

      if (i == 3)
      {
         std::printf("\n");
      }
   }
   std::printf("\n====================================================\n");
}

void writeRAM(int teclas[])
{
   FILE* file;

   file = fopen("datos.csv", "a");

   const auto& RAM = alei.getRAM();

   uint8_t add = 0;
   
   for (std::size_t i = 0; i < 8; i++)
   {
      for (std::size_t j = 0; j < 16; j++, add++)
      {
         std::fprintf(file, "%02X;", RAM.get(add));
      }
   }
   
   for (std::size_t i = 0; i < 4; i++)
   {
      std::fprintf(file, "%d;", teclas[i]);
   }
   std::fprintf(file, "\n");
   fclose(file);
}

///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
float agentStep() {
   float reward = 0;

   auto* keystate = SDL_GetKeyState(0);

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
   
   for (step = 0; 
        !alei.game_over() && step < maxSteps; 
        ++step) 
   {
      totalReward += agentStep();
   }

   std::cout << "Steps: " << step << std::endl;
   std::cout << "Reward: " << totalReward << std::endl;

   return 0;
}
