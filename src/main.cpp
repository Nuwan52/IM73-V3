
#include "ClearCore.h"
#include "defines.h"
#include "EthernetProcessor.h"
#include "maintread.h"
#include "hmi.h"

//change this to adjest the timing of tub and lid 
int Tub_timer  = 60;
int Lid_timer  = 150;
// end 



int delay_[] = {300, 300, 1000, 1000, 1000, 1000};
int delay1_[] = {0, 60, 60};
int delay3_[] = {100, 100, 100};
int delay2_[] = {1000, 100, 1000, 1000, 1000, 1000, 1000};

maintread time = maintread(delay_, 6);
maintread time1 = maintread(delay1_, 3);
maintread time3 = maintread(delay3_, 3);
maintread time2 = maintread(delay2_, 7);


int current_cupCounnt = 0;
int Activate_Ejection = false;
int CONVE_RUN_SPEED = 10000;
bool startFlag = false;


void TubLidExchnage(int data){
    delay1_[1] = data;
    delay1_[2] = data;
}


void Converyor_Move()
{

    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);
    Converyor.AccelMax(1200);
    Converyor.EnableRequest(true);
    Converyor.MoveVelocity(10000);

                          
}

void converyor_speed_adjust()
{
    
   
    if (time3.Delay() == 1)
    {
        //   do something 1
        Converyor.MoveVelocity(20000);
       
        time3.finish();
     
    }

    if (time3.Delay() == 2)
    {
        //   do something 1
       Converyor.MoveStopDecel(1500000000);
       
        
       
        time3.finish();
    }

    if (time3.Delay() == 3)
    {
        //   do something 1
        
        
        time3.finish();
    }
}

void stacking_step_01()
{

    if (time1.Delay() == 1 && startFlag)
    {
        //   do something 1

        if (SENSOR_PIN.State())
        {
            time1.finish();
        }
    }

    if (time1.Delay() == 2)
    {
        //   do something 1
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_UPWORD))->State(true);
        current_cupCounnt++;
        if(current_cupCounnt == MAX_CUP_COUNT){
            Activate_Ejection = true;  
            current_cupCounnt = 0;
        }
        time1.finish();
    }

    if (time1.Delay() == 3)
    {
        //   do something 1
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_UPWORD))->State(false);
        time1.finish();
    }
}

void stacking_stap_02()
{

    if (time.Delay() == 1)
    {
        //   do something 1
        
        if(Activate_Ejection){  
            CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_GRABBER))->State(true);
            Activate_Ejection = false;
            time.finish();
        }
    }

    if (time.Delay() == 2)
    {
        //   do something 1
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_LIFTER))->State(true);
        time.finish();
    }

    if (time.Delay() == 3)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_DROO))->State(true);
        time.finish();
    }

    if (time.Delay() == 4)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER))->State(true);
        time.finish();
    }

    if (time.Delay() == 5)
    {
        //   do something 1
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_LIFTER))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_GRABBER))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR))->State(true);

        
        time.finish();
    }

    if (time.Delay() == 6)
    {
        //   do something 1

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_DROO))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR))->State(false);
        time.finish();
    }


}


void IML_interface(){



    if (time2.Delay() == 1 && IM_SENSOR.State())
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONLEFT))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONRIGHT))->State(true);


        time2.finish();
    }

    if (time2.Delay() == 2)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(ROTATERIGHT))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(BOTTOMLIFT))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONBOTTOM))->State(true);  
        time2.finish();
    }

    if (time2.Delay() == 3)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 03");
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONRIGHT))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(ROTATERIGHT))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(BOTTOMLIFT))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONBOTTOM))->State(false);

        time2.finish();
    }

    if (time2.Delay() == 4)
    {
        //   do something 1
        



        time2.finish();
    }

    if (time2.Delay() == 5)
    {

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(ROTATELEFT))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(BOTTOMLIFT))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONBOTTOM))->State(true);


        time2.finish();
    }

    if (time2.Delay() == 6)
    {
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONLEFT))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(ROTATELEFT))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(BOTTOMLIFT))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(SUCTIONBOTTOM))->State(false);

        time2.finish();
    }

     if (time2.Delay() == 7)
    {
      
       

        time2.finish();
    }





}


void Manual_Overide(int Data , bool status){
CcioMgr.PinByIndex(static_cast<ClearCorePins>(Data))->State(status);
}

int ConveryorSpeed(int flag){

if(flag > 0 && CONVE_RUN_SPEED < CONVE_MAX_SPEED){
CONVE_RUN_SPEED+=CONVE_INCREMENT;
}else if(CONVE_RUN_SPEED > CONVE_MIN_SPEED){
CONVE_RUN_SPEED-=CONVE_INCREMENT;
}
Converyor.MoveVelocity(CONVE_RUN_SPEED);
return CONVE_RUN_SPEED;

}

void initIO()
{

    SENSOR_PIN.Mode(Connector::INPUT_DIGITAL);
    IM_SENSOR.Mode(Connector::INPUT_DIGITAL);
    
    for (int16_t pin = CLEARCORE_PIN_CCIOA0; pin <= CLEARCORE_PIN_CCIOA7; pin++)
    {
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(pin))->Mode(Connector::OUTPUT_DIGITAL);
    }

    for (int16_t pin = CLEARCORE_PIN_CCIOB0; pin <= CLEARCORE_PIN_CCIOB7; pin++)
    {
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(pin))->Mode(Connector::OUTPUT_DIGITAL);
    }
    for (int16_t pin = CLEARCORE_PIN_CCIOC0; pin <= CLEARCORE_PIN_CCIOC7; pin++)
    {
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(pin))->Mode(Connector::OUTPUT_DIGITAL);
    }
}

void HMI_Loading()
{
    StartHMI("Starting all hardware");
}

void system_init()
{
    SerialPort.Mode(Connector::USB_CDC);
    IOConnector.Mode(Connector::CCIO);
    IOConnector.PortOpen();
    SerialPort.Speed(9600);
    SerialPort.PortOpen();
    EthernetStart();
    HMI_Loading();
    initIO();
    Converyor_Move();

}


void defult_IO(){



}
int main()
{

    system_init();
    CcioMgr.PinByIndex(static_cast<ClearCorePins>(PROCESS_SELECTOR))->State(true);


    while (true)
    {
        stacking_step_01();
        stacking_stap_02();
        IML_interface();
        RuningEthernetThread();
        
       // converyor_speed_adjust();
        
    }
}
