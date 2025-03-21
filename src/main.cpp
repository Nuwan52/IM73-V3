
#include "ClearCore.h"
#include "defines.h"
#include "EthernetProcessor.h"
#include "maintread.h"
#include "hmi.h"



int delay_[] = {300, 300, 1000, 1000, 1000, 1000};
int delay1_[] = {0, 100, 200};
int delay2_[] = {1000, 1000, 1000, 1000, 1000, 1000, 1000};

maintread time = maintread(delay_, 6);
maintread time1 = maintread(delay1_, 3);
maintread time2 = maintread(delay2_, 7);


int current_cupCounnt = 0;
int Activate_Ejection = false;
int CONVE_RUN_SPEED = 10000;


void Converyor_Move()
{

    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);
    Converyor.AccelMax(12000);
    Converyor.EnableRequest(true);
    Converyor.MoveVelocity(15000);

                          
}

void stacking_step_01()
{

    if (time1.Delay() == 1)
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
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOB6))->State(true);
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
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOB6))->State(false);
        time1.finish();
    }
}

void stacking_stap_02()
{

    if (time.Delay() == 1)
    {
        //   do something 1
        
        if(Activate_Ejection){  
            CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOA5))->State(true);
            Activate_Ejection = false;
            time.finish();
        }
    }

    if (time.Delay() == 2)
    {
        //   do something 1
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOA4))->State(true);
        time.finish();
    }

    if (time.Delay() == 3)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOB7))->State(true);
        time.finish();
    }

    if (time.Delay() == 4)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOB4))->State(true);
        time.finish();
    }

    if (time.Delay() == 5)
    {
        //   do something 1
        
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOB4))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOA4))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOA5))->State(false);
       

        
        time.finish();
    }

    if (time.Delay() == 6)
    {
        //   do something 1

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOB7))->State(false);
        time.finish();
    }


}


void IML_interface(){



    if (time2.Delay() == 1)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 01");
        SUCTION_LEFT.State(true);
        SUCTION_RIGHT.State(true);  


        time2.finish();
    }

    if (time2.Delay() == 2)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 02");
        CUP_ROTATOR_RIGHT.State(true);
        BOTTOM_LIFT.State(true);
        SUCTION_BOTTOM.State(true);     
        time2.finish();
    }

    if (time2.Delay() == 3)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 03");
        SUCTION_RIGHT.State(false);
        CUP_ROTATOR_RIGHT.State(false);
        BOTTOM_LIFT.State(false);

        time2.finish();
    }

    if (time2.Delay() == 4)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 04");
        SUCTION_BOTTOM.State(false);


        time2.finish();
    }

    if (time2.Delay() == 5)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 05");
        CUP_ROTATOR_LEFT.State(true);
        BOTTOM_LIFT.State(true);
        SUCTION_BOTTOM.State(true); 


        time2.finish();
    }

    if (time2.Delay() == 6)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 06");
        SUCTION_LEFT.State(false);
        CUP_ROTATOR_LEFT.State(false);  
        BOTTOM_LIFT.State(false);   

        time2.finish();
    }

     if (time2.Delay() == 7)
    {
        //   do something 1
        SerialPort.SendLine("Thread 03d 07");
        SUCTION_BOTTOM.State(false);
        time2.finish();
    }





}


void Manual_Overide(String Data){
CcioMgr.PinByIndex(static_cast<ClearCorePins>(CLEARCORE_PIN_CCIOA0))->State(true);


}

int ConveryorSpeed(int flag){

if(flag > 0 && CONVE_RUN_SPEED < CONVE_MAX_SPEED){
CONVE_RUN_SPEED+=CONVE_INCREMENT;
}else if(CONVE_RUN_SPEED > CONVE_MIN_SPEED){
CONVE_RUN_SPEED-=CONVE_INCREMENT;
}

return CONVE_RUN_SPEED;

}

void initIO()
{

    SENSOR_PIN.Mode(Connector::INPUT_DIGITAL);
    Y_AIXS_LIMIT.Mode(Connector::INPUT_DIGITAL);
    SUCTION_RIGHT.Mode(Connector::OUTPUT_DIGITAL);
    SUCTION_LEFT.Mode(Connector::OUTPUT_DIGITAL);
    SUCTION_BOTTOM.Mode(Connector::OUTPUT_DIGITAL);
    CUP_ROTATOR_LEFT.Mode(Connector::OUTPUT_DIGITAL);
    CUP_ROTATOR_RIGHT.Mode(Connector::OUTPUT_DIGITAL);
    BOTTOM_LIFT.Mode(Connector::OUTPUT_DIGITAL);

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
    Manual_Overide("DD");

    while (true)
    {
        //stacking_step_01();
        //stacking_stap_02();
        //IML_interface();
        //RuningEthernetThread();
    }
}
