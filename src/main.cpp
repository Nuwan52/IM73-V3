
#include "ClearCore.h"
#include "defines.h"
#include "EthernetProcessor.h"
#include "maintread.h"
#include "hmi.h"

// change this to adjest the timing of tub and lid
int Tub_timer = 60;
int Lid_timer = 150;
// end
bool LetStacking_Machanisum = false;

int delay_[] = {10, 100, 100, 300, 1000, 1000, 1000, 1000};
int delay1_[] = {0, 60, 60};
int delay4_[] = {0, 100, 100};
int delay5_[] = {0, 100, 200};
int delay6_[] = {0, 400, 100};
int delay3_[] = {100, 100, 100};
int delay2_[] = {1000, 100, 1000, 1000, 1000, 1000, 1000};

maintread time = maintread(delay_, 8);
maintread time1 = maintread(delay1_, 3);
maintread time6 = maintread(delay6_, 3);
maintread time4 = maintread(delay4_, 3);
maintread time5 = maintread(delay5_, 3);
maintread time3 = maintread(delay3_, 3);
maintread time2 = maintread(delay2_, 7);

// LATCHING

bool sensor_latch = false;

int current_cupCounnt = 0;
int Activate_Ejection = false;
int Ejected_CUP_amount = 0;
int CONVE_RUN_SPEED = 10000;
bool startFlag = false;

int CAM_SENSOR_COUNT = 0;
int EJECTION_SENSOR_COUNT = 0;
int MAX_CUP_QUIE_COUNT = 0;
int CUP_QUE[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Timer tiggerning funtions
unsigned long CUP_TIMER[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int CUP_DISTANCE_TIME = 1050;
bool CUP_VERTUAL_SENSOR = false;

volatile bool UP_CAM_FLAG = false;
bool CAM_READ_ENABLE = false;

void UP_CAM_CALLBACK()
{
    if (CAM_READ_ENABLE)
    {
        if (!UP_CAM_FLAG && UP_CAM.State())
        {
            UP_CAM_FLAG = true;
        }
    }
}
void TubLidExchnage(int data)
{
    delay1_[1] = data;
    delay1_[2] = data;
}
void Cup_count_up()
{

    if (CAM_SENSOR_COUNT == 10)
    {
        CAM_SENSOR_COUNT = 0;
    }

    if (EJECTION_SENSOR_COUNT == 10)
    {
        EJECTION_SENSOR_COUNT = 0;
    }
}

void EjectionToConveryor()
{
    Ejection_Converyor.Move(EJECTION_CONVEYOR_MOVE_AMOUNT);
}

void Converyor_Move()
{

    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);
    Converyor.AccelMax(1200);
    Converyor.EnableRequest(true);
    Converyor.MoveVelocity(10000);

    Ejection_Converyor.AccelMax(1200);
    Ejection_Converyor.EnableRequest(true);
    Ejection_Converyor.VelMax(10000);

    EjectionToConveryor();
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

void Camera_Tiggering()
{

    if (time5.Delay() == 1 && startFlag && CAM_TIGGER_SENSOR.State())
    {
        time5.finish();
    }

    if (time5.Delay() == 2)
    {

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CAM_TIGGER_OUTPUT))->State(false);

        time5.finish();
    }

    if (time5.Delay() == 3)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CAM_TIGGER_OUTPUT))->State(true);

        time5.finish();
    }
}

void VacuumEjection_by_timer()
{

    if (VAUUM_EJECTOR.State())
    {

        for (int i = 0; i < 10; i++)
        {

            if (CUP_TIMER[i] != 0)
            {
                if (CUP_TIMER[i] + CUP_DISTANCE_TIME <= millis())
                {
                    CUP_TIMER[i] = 0;
                    CUP_VERTUAL_SENSOR = true;
                    break;
                }
            }
        }
    }

    if (time4.Delay() == 1 && startFlag && CUP_VERTUAL_SENSOR && VAUUM_EJECTOR.State())
    {

        time4.finish();
    }

    if (time4.Delay() == 2)
    {

        ERROR_EJECTOR.State(true);

        time4.finish();
    }

    if (time4.Delay() == 3)
    {
        //   do something 1
        ERROR_EJECTOR.State(false);
        CUP_VERTUAL_SENSOR = false;
        time4.finish();
    }
}

void vacuumEjection()
{

    for (int i = 0; i < 10; i++)
    {

        if (CUP_TIMER[i] + CUP_DISTANCE_TIME < millis())
        {
            CUP_TIMER[i] = 0;
            CUP_VERTUAL_SENSOR = true;
            break;
        }
    }

    if (!VAUUM_EJECTOR.State() && sensor_latch)
    {
        sensor_latch = false;
    }
    if (time4.Delay() == 1 && startFlag && VAUUM_EJECTOR.State() && !sensor_latch)
    {

        sensor_latch = true;
        time4.finish();
    }

    if (time4.Delay() == 2)
    {
        Cup_count_up();

        if (CUP_QUE[EJECTION_SENSOR_COUNT] == 2)
        {

            Serial.println("CUP REJECTING");
            ERROR_EJECTOR.State(true);
        }

        if (CUP_QUE[EJECTION_SENSOR_COUNT] == 1)
        {

            Serial.println("CUP PASSING");
            ERROR_EJECTOR.State(false);
        }

        Serial.println(EJECTION_SENSOR_COUNT - CAM_SENSOR_COUNT);

        CUP_QUE[EJECTION_SENSOR_COUNT] = 0;

        EJECTION_SENSOR_COUNT++;

        time4.finish();
    }

    if (time4.Delay() == 3)
    {
        //   do something 1
        ERROR_EJECTOR.State(false);

        time4.finish();
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
        LetStacking_Machanisum = false;

        time1.finish();
    }

    if (time1.Delay() == 3)
    {
        //   do something 1

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_UPWORD))->State(false);
        LetStacking_Machanisum = true;
        current_cupCounnt++;
        if (current_cupCounnt == MAX_CUP_COUNT)
        {
            Activate_Ejection = true;
            current_cupCounnt = 0;
        }
        time1.finish();
    }
}

void stacking_stap_02()
{

    if (time.Delay() == 1)
    {

        if (Activate_Ejection && LetStacking_Machanisum)
        {

            Activate_Ejection = false;
            time.finish();
        }
    }

    if (time.Delay() == 2)
    {
        //   do something 1

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_GRABBER))->State(true);

        time.finish();
    }

    if (time.Delay() == 3)
    {
        //   do something 1

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_LIFTER))->State(true);
        time.finish();
    }

    if (time.Delay() == 4)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_DROO))->State(false);
        time.finish();
    }

    if (time.Delay() == 5)
    {
        //   do something 1
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER))->State(false);
        time.finish();
    }

    if (time.Delay() == 6)
    {
        //   do something 1
        Ejected_CUP_amount++;
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_LIFTER))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_GRABBER))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR))->State(true);
        if (Ejected_CUP_amount == EJECTION_OUT_ON_CONVERYOR)
        {

            CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR_02))->State(true);
        }

        time.finish();
    }

    if (time.Delay() == 7)
    {
        //   do something 1

        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_DROO))->State(true);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR))->State(false);
        CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR_02))->State(false);
        if (Ejected_CUP_amount == EJECTION_OUT_ON_CONVERYOR)
        {

            CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR_03))->State(true);
        }

        time.finish();
    }

    if (time.Delay() == 8)
    {
        //   do something 1

        if (Ejected_CUP_amount == EJECTION_OUT_ON_CONVERYOR)
        {

            EjectionToConveryor();
            CcioMgr.PinByIndex(static_cast<ClearCorePins>(CUP_PUSHER_TO_CONVEYOR_03))->State(false);
            Ejected_CUP_amount = 0;
        }
        time.finish();
    }
}

void IML_interface()
{

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

void Manual_Overide(int Data, bool status)
{
    CcioMgr.PinByIndex(static_cast<ClearCorePins>(Data))->State(status);
}

int ConveryorSpeed(int flag)
{

    if (flag > 0 && CONVE_RUN_SPEED < CONVE_MAX_SPEED)
    {
        CONVE_RUN_SPEED += CONVE_INCREMENT;
    }
    else if (CONVE_RUN_SPEED > CONVE_MIN_SPEED)
    {
        CONVE_RUN_SPEED -= CONVE_INCREMENT;
    }
    Converyor.MoveVelocity(CONVE_RUN_SPEED);
    return CONVE_RUN_SPEED;
}

void initIO()
{

    SENSOR_PIN.Mode(Connector::INPUT_DIGITAL);
    IM_SENSOR.Mode(Connector::INPUT_DIGITAL);
    ERROR_EJECTOR.Mode(Connector::OUTPUT_DIGITAL);
    VAUUM_EJECTOR.Mode(Connector::INPUT_DIGITAL);
    CAM_TIGGER_SENSOR.Mode(Connector::INPUT_DIGITAL);
    UP_CAM.Mode(Connector::INPUT_DIGITAL);
    DOWN_CAM.Mode(Connector::INPUT_DIGITAL);
    RIGHT_CAM.Mode(Connector::INPUT_DIGITAL);
    LEFT_CAM.Mode(Connector::INPUT_DIGITAL);

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
    CcioMgr.PinByIndex(static_cast<ClearCorePins>(CAM_TIGGER_OUTPUT))->State(true);
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

void Ejection_haddler()
{

    if (time6.Delay() == 1 && startFlag && CAM_TIGGER_SENSOR.State())
    {
        time6.finish();
        CAM_READ_ENABLE = true;
    }

    if (time6.Delay() == 2)
    {
        Cup_count_up();
        if (UP_CAM_FLAG)
        {

            CUP_QUE[CAM_SENSOR_COUNT] = 1;
            UP_CAM_FLAG = false;

            Serial.println("All side have passed . Continue...");
        }
        else
        {

            CUP_QUE[CAM_SENSOR_COUNT] = 2;

            // Timer funtions

            for (int i = 0; i < 10; i++)
            {

                if (CUP_TIMER[i] == 0)
                {
                    CUP_TIMER[i] = millis();
                    break;
                }
            }

            // timer funtion end

            Serial.println("One or more side has Defects :  Ejecting...");
        }

        CAM_READ_ENABLE = false;

        CAM_SENSOR_COUNT++;

        time6.finish();
    }

    if (time6.Delay() == 3)
    {

        time6.finish();
    }
}

void defult_IO()
{
}
int main()
{

    system_init();
    CcioMgr.PinByIndex(static_cast<ClearCorePins>(PROCESS_SELECTOR))->State(true);

    while (true)
    {
        UP_CAM_CALLBACK();
        stacking_step_01();
        stacking_stap_02();
        VacuumEjection_by_timer();
        // vacuumEjection();
        IML_interface();
        RuningEthernetThread();
        Camera_Tiggering();
        Ejection_haddler();

        // converyor_speed_adjust();
    }
}
