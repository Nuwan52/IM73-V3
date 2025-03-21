#include "ClearCore.h"

class maintread
{
private:
    /* data */
public:
    maintread(int* delayArray , int MaxSteps);
    int Delay();
    void finish();
    unsigned long startTime = 0;
    int* DelayArray = nullptr;
    int currentSteps = 1;
    int maxStep = 0;
};

maintread::maintread(int* delayArray , int MaxStep)
{
    startTime = Milliseconds();
    DelayArray = delayArray;
    maxStep = MaxStep;
  
}

int maintread::Delay(){
    
if(Milliseconds() > startTime + DelayArray[currentSteps - 1]){
   return currentSteps;
}else{
    return 0;
}

}
void maintread::finish(){
startTime = Milliseconds();

if(maxStep > currentSteps){
currentSteps++;
}else{
currentSteps = 1;

}

}
