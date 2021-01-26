#include <iostream>
#include <fstream>
#include <cmath>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include "net_t.h"
#include <unistd.h>

const std::vector<MatDouble_t> bot_weights {
   { // Layer 1
      {0.369897,1.37263,1.42714,1.39157,0.502884,-0.456154,-0.79606,-1.76305,-0.902739,-0.08491,1.2528,-0.0726584,-0.421692,1.34416,-0.647721,0.590214,-0.52028,1.82808,-1.43399,1.47703,-0.0958515,1.20762,0.0880903,0.71363,0.87889},
      {0.326726,0.148876,1.0302,-1.56987,-0.105163,-1.24949,0.943763,-1.12912,-1.45311,-0.700534,-1.39552,-1.10613,-0.452171,1.60375,-0.199374,0.450388,1.60276,-1.59627,1.87148,0.610033,-1.31093,-0.565051,0.998608,0.429543,-0.696924},
      {-1.83868,0.534881,1.82822,0.60864,0.538007,1.97303,0.326051,-0.341113,-0.100792,0.492002,-0.645297,0.696126,-0.728177,1.10879,1.79087,0.647426,-1.93769,0.489357,0.691773,1.87999,1.50653,0.0383387,-1.76981,-0.194557,-1.91213},
      {-0.232183,1.91034,-0.560045,-0.0883095,0.749147,1.51507,1.66407,-1.12486,0.258586,1.44606,0.0358916,1.65992,1.68615,-1.65322,-0.884985,-1.9551,1.36457,0.593944,1.36207,-0.934023,-0.406867,0.20866,-1.33899,-0.51978,-1.40857},
      {0.277325,0.811587,-0.842604,-0.265747,1.0202,-0.413892,1.57762,0.553392,1.55976,0.717251,-0.202371,1.90639,-1.52886,1.06369,-0.351264,0.698861,-0.996686,-0.744045,1.85399,0.352401,0.636039,0.132277,-1.07342,-0.418788,0.473274},
      {-0.100156,-0.12386,0.855851,-0.853832,-0.466306,0.988088,1.50578,-1.5901,-1.62986,-0.590912,0.205006,-1.85949,1.8704,-0.719695,-1.11034,-1.42963,-1.60596,1.93238,-0.958318,0.139365,-0.208841,-1.60054,-0.593302,-0.123275,1.35886},
      {1.612,-1.86593,0.00932162,-1.31553,1.08741,1.47222,-0.37264,-1.43862,-1.8487,1.88259,-0.530583,-0.31471,-1.77555,-0.573046,-1.92535,-1.07506,1.08049,1.85524,1.00624,-0.644184,-0.0674684,-0.623598,-1.3024,-1.25619,-0.144667},
      {1.49211,1.76915,0.431225,0.385027,1.1299,0.000104742,-1.7911,0.793107,1.96146,-0.927109,0.713407,1.45111,0.999238,1.85029,0.216075,-1.14569,-1.10565,-1.12036,0.277146,-0.190774,1.87319,0.7192,-1.65197,-1.76701,-0.0484485},
      {1.51773,1.89776,0.46869,0.169294,1.4126,0.971315,-0.0852612,0.705404,0.426414,0.855245,-0.121508,-0.175216,1.61897,-1.44513,-1.07865,1.52005,1.61103,0.580733,-0.698375,0.0785195,-1.99153,-0.749455,-0.296964,1.53499,0.71655},
      {-0.174829,-0.0831948,1.13393,-1.09292,1.50228,-0.750179,1.80887,-0.103104,0.836718,-1.37959,0.90385,0.565713,-1.15371,-1.26286,1.22552,0.983437,0.670953,-0.896621,-1.29286,0.823654,-0.164037,1.37516,-1.2108,-1.33142,-1.49434},
      {0.884658,-1.87044,0.983901,-1.62289,-1.12554,0.992905,0.922952,-1.81004,-1.15857,-0.848824,0.706128,-1.74064,0.221679,-1.95494,1.32666,1.92933,0.810621,-1.26822,0.0493694,0.320461,1.14542,0.424145,-1.12174,-0.178585,1.50923},
      {-0.0307995,0.858688,-0.0551334,0.830751,-0.0074084,1.37251,-1.21759,1.08879,1.88921,1.44326,1.11708,1.93213,1.01009,-1.97562,-0.918278,-0.356554,-0.28592,-0.805297,-0.393839,-1.51111,1.91487,-0.373969,0.275701,-0.622997,1.15072},
      {-0.353026,-0.558186,-0.397768,-0.790925,1.10233,1.69147,-0.689898,1.79615,-1.93183,0.137639,-0.775288,1.53229,-0.997403,0.718788,1.23719,-0.270696,1.00867,1.31972,-0.489924,-1.615,-0.968262,0.372409,-0.0813836,-0.0428258,-0.164982},
      {0.0979859,-0.230341,1.4054,-0.263912,1.30573,0.0390638,-1.64361,0.636741,-1.16695,1.38837,0.725792,-1.27883,-1.71029,-1.95184,1.56177,-1.45975,1.11509,1.69544,0.85628,-0.0375012,-0.11808,1.52682,-0.045797,-0.341876,-1.29804}
   },
   { // Layer 2
      {-0.0995618,1.12878,0.224892,-1.3561,-1.41888,0.594323,0.156436,-0.69439,-1.40688,-1.35566,-1.47645,0.314387,-1.62556,1.63504,-1.66124},
      {1.28825,1.5792,-1.60311,-0.201911,-0.0167546,0.412117,0.859214,-1.8565,0.995675,1.11802,-1.86668,-0.763692,-0.97139,0.0784227,1.8485},
      {0.311245,-1.52253,0.484532,1.52762,-1.28286,1.96667,-1.58496,-1.61725,0.613094,1.87096,1.73964,-0.209618,0.988209,0.963399,-0.691997},
      {-1.5395,-0.24839,-0.429131,0.101888,1.35054,0.123082,0.716113,0.43611,-1.61184,-1.64826,-1.7677,-1.64705,-1.07412,1.32966,0.0682572},
      {0.292678,0.834463,0.177472,-1.62184,0.531954,1.23443,-0.418671,-0.0409825,2.16056,-0.775124,-0.358566,-1.80986,-0.51584,1.30512,1.2481},
      {0.5564,-0.526644,-0.746229,1.20072,-1.71249,0.730339,-0.475617,0.533592,1.69961,1.40907,-0.00904718,-0.287472,1.28341,0.948006,-0.211356},
      {1.86505,-0.547953,-0.407947,0.948216,-0.205616,1.74123,-0.969965,-0.471138,0.782883,1.57872,-1.51202,-1.02955,-1.08541,-0.0376913,1.55765}
   },
   { // Layer 3
      {-0.195821,-0.456717,1.99686,-1.15317,0.520932,-1.06379,-0.131176,-0.333801},
      {0.406867,1.17662,-0.259592,-1.03118,1.53106,-0.722481,0.641734,-0.083966},
      {-0.719101,0.192757,0.535516,-0.133067,-1.88815,-0.913221,-0.995601,1.29931}
   }
};

const VecInt_t ram_indexes {
   2, 8, 16, 17, 18, 19, 33, 34, 35, 36, 37, 38, 39, 60, 61, 68, 69, 73, 80, 81, 102, 103, 106, 115
};


// Global vars
const double BOT_THRESHOLD = 0.3;
const bool USE_BOT = true;
const int maxSteps = 20000;
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

void writeTeclas(const int teclas[])
{
   if (USE_BOT) return;

   std::ofstream user_input_file;
   user_input_file.open("y.csv", std::ios::app);      
   for (std::size_t i = 0; i < 3; i++)
   {      
      user_input_file << std::to_string(teclas[i]) << ",";
   }   
   user_input_file << std::to_string(teclas[3]);
   user_input_file << std::endl;
   user_input_file.close();
}

void writeRAM(const ALERAM RAM)
{
   if (USE_BOT) return;

   std::ofstream ram_file;
   ram_file.open("x.csv", std::ios::app);
   for(std::size_t i = 0; i < 127; ++i)
   {
      ram_file << std::to_string(RAM.get(i)) << ",";
   }
   ram_file << std::to_string(RAM.get(127));
   ram_file << std::endl;
   ram_file.close();
}

///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
VecDouble_t ram_to_VecDouble(const VecInt_t& indexes) {
   
   VecDouble_t result(indexes.size()); 
   
   const auto& RAM = alei.getRAM();
   for(size_t i{0}; i < indexes.size(); ++i)
   {
      result[i] = RAM.get(indexes[i]);
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

float playBot(const Net_t& bot, int teclas[], VecInt_t const &indexes) {
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

   // if (prediction[maxIndex] > BOT_THRESHOLD)
   // {
   //    switch (maxIndex)
   //    {
   //       case 0:
   //          teclas[0] = 1;
   //          reward += alei.act(PLAYER_A_UP);

   //          if (prediction[1] > prediction[2] && prediction[1] > BOT_THRESHOLD)
   //          {
   //             reward += alei.act(PLAYER_A_LEFT);
   //             teclas[2] = 1;
   //          }
   //          else if (prediction[2] > BOT_THRESHOLD)
   //          {
   //             reward += alei.act(PLAYER_A_RIGHT);
   //             teclas[3] = 1;
   //          }

   //          break;

   //       // case 1:
   //       //    teclas[1] = 1;
   //       //    teclas[2] = 1;
   //       //    reward += alei.act(PLAYER_A_LEFTFIRE);            
   //       //    break;
         
   //       // case 2:
   //       //    teclas[1] = 1;
   //       //    teclas[3] = 1;
   //       //    reward += alei.act(PLAYER_A_RIGHTFIRE);            
   //       //    break;

   //       case 1:
   //          reward += alei.act(PLAYER_A_LEFT);
   //          teclas[2] = 1;
   //          break;

   //       case 2:
   //          reward += alei.act(PLAYER_A_RIGHT);
   //          teclas[3] = 1;
         
   //       default:
   //          break;
   //    }
   // }

   // FIRE UP
   if (prediction[0] > BOT_THRESHOLD)
   {
      teclas[0] = 1;
      reward += alei.act(PLAYER_A_UP);
   }  
   
   if (prediction[1] > prediction[2] && prediction[1] > BOT_THRESHOLD) // MOVEMENT LEFT
   {
      reward += alei.act(PLAYER_A_LEFT);
      teclas[2] = 1;
   }
   else if (prediction[2] > BOT_THRESHOLD) // MOVEMENT RIGHT
   {
      reward += alei.act(PLAYER_A_RIGHT);
      teclas[3] = 1;
   }
   // // FIRE LEFT
   // if (prediction[1] > prediction[2] && prediction[1] > BOT_THRESHOLD)
   // {
   //    teclas[1] = 1;
   //    teclas[2] = 1;
   //    reward += alei.act(PLAYER_A_LEFTFIRE);      
   // }
   // else if (prediction[2] > BOT_THRESHOLD) // FIRE RIGHT
   // {
   //    teclas[1] = 1;
   //    teclas[3] = 1;
   //    reward += alei.act(PLAYER_A_RIGHTFIRE);           
   // } 
   

   // print(prediction, reward, '\n');  
   // sleep(1);
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
   const ALERAM RAM = alei.getRAM();
   writeRAM(RAM);

   if(useBot)
   {
      reward += playBot(bot, teclas, indexes);
   }
   else
   {
      reward += playManual(teclas);
      usleep(50000);
   }
   
   writeTeclas(teclas);

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
   
   Net_t bot(24, bot_weights);         

   VecInt_t indexes(ram_indexes);
   // readRamIndexes("../ramindexes.txt", indexes);     
   
   for (step = 0; 
        !alei.game_over() && step < maxSteps; 
        ++step) 
   {
      // std::cout << step << std::endl;
      // if(USE_BOT && step < 50)
      // {
      //    totalReward += alei.act(PLAYER_A_LEFT);
      // }
      // else
      // {
         totalReward += agentStep(bot, USE_BOT, indexes);
      // }  
   }

   std::cout << "Steps: " << step << std::endl;
   std::cout << "Reward: " << totalReward << std::endl;

   return 0;
}
