#include "Arduino.h"
#include "line_robot.h"

#define rightSpeed_1     5 
#define rightDir_1       4
 
#define leftSpeed_1      6
#define leftDir_1        7
 
#define IN1 5
#define IN2 4

#define IN3 6
#define IN4 7

//===============================Чтение датчиков======================================
void AnalogLineRobot::Read_private()
{
    for(unsigned int i=0;i<_num_sensor;i++)
    {
        _sensor_values[i] = analogRead(_pins[i]);
    }
}

void DigitalLineRobot::Read_private()
{
    _byte_sensor_values = 0x0;
    // for(char i=_num_sensor-1;i>=0;i--)
    // {
    //     _byte_sensor_values = _byte_sensor_values << 1;
    //     _byte_sensor_values |= digitalRead(_pins[i]);
    // }

     for(char i=0;i<_num_sensor;i++)
    {

        _byte_sensor_values = _byte_sensor_values << 1;
        _byte_sensor_values |= digitalRead(_pins[i]);

        //===============================================================
        if (_num_sensor==3)
        {
            if(i==1){
                if(analogRead(A4)>200) _byte_sensor_values |= 0x1;
                else _byte_sensor_values &= 0x6;
            }
        }
        //===============================================================
    }
}


//=========================Калибровка аналогового==================================
void AnalogLineRobot::Set_Calibrate(short int calmax,short int calmin)
{
    _calmin = calmin;
    _calmax = calmax;
    _denomination = _calmax - _calmin;
}


void AnalogLineRobot::Calibrate()
{
        Read_private();
        if(_calmin>*_sensor_values) _calmin= *_sensor_values;
        if(_calmax<*_sensor_values) _calmax= *_sensor_values;
        _denomination = _calmax - _calmin;
}

//=========================Установка скоростей моторов============================
void LineRobot::procSetMotors( int leftSpeed, int rightSpeed)
{
     if (leftSpeed>0) digitalWrite(leftDir_1,HIGH);
     else digitalWrite(leftDir_1, LOW);

      if (rightSpeed>0) digitalWrite(rightDir_1,HIGH); 
      else digitalWrite(rightDir_1, LOW);

      if (leftSpeed>255)
        leftSpeed=255;
      else if (leftSpeed<-255)
        leftSpeed=-255;

      if (rightSpeed>255)
        rightSpeed=255;
      else if (rightSpeed<-255)
        rightSpeed=-255;

      analogWrite(leftSpeed_1, abs(leftSpeed));
      analogWrite(rightSpeed_1,abs(rightSpeed));
}


 //============================Получить значения======================================
void LineRobot::private_GetSensor(short int* sensor_values,bool Mode_print) //запись в массив и вывод в com port
{
    for(int i=0;i<_num_sensor;i++)
    {
         sensor_values[i]=_sensor_values[i];
         if(Mode_print==true)
         {
            Serial.print(_sensor_values[i]);
            if(i==_num_sensor-1) Serial.println(" ");
            else Serial.print(" ");

         }
    }
}

void LineRobot::private_GetSensor(short int* sensor_values) //запись в массив
{
    for(int i=0;i<_num_sensor;i++)
    {
         sensor_values[i]=_sensor_values[i];
    }
}
//==============================Digital===========================================
DigitalLineRobot::DigitalLineRobot(unsigned char* pins,int num_sensor)
{
    _num_sensor = num_sensor;
    _Speed = 120;

    if (_pins == 0||_sensor_values == 0)
    {
        _pins = (unsigned char*)malloc(sizeof(unsigned char)*_num_sensor);
        _sensor_values = (short int*)malloc(sizeof(short int)*_num_sensor);
        if (_pins == 0||_sensor_values == 0)
            return;
    }
    _pins=pins;
    for (unsigned int i=0;i<_num_sensor;i++) pinMode(_pins[i],INPUT);
    for(int i = 4; i < 8; i++) pinMode(i, OUTPUT);  

    koef_smooth = 1;//коэффициент плавного поворота
    koef_sharp = 1;//коэффициент резкого поворота
    _byte_sensor_values = 0x0;//значение датчиков
}

void DigitalLineRobot::Move()
{
    Read_private();
    switch(_num_sensor)
    {
        case 3:
            three_private_Move();
            break;
        case 2:
            two_private_Move();
            break;
    }
}

void DigitalLineRobot::three_private_Move()
{
    Read_private();
    //Serial.print((int)_byte_sensor_values);
    //Serial.print(" ");
    switch(_byte_sensor_values)
    {
        case 0x0: //Все видят белое
            procSetMotors(_Speed, _Speed);
            break;
        case 0x1: //правый на черном
            procSetMotors(_Speed+(160*koef_sharp) ,_Speed-(160*koef_sharp));
            break;
        case 0x2: //центарльный на черномs
            procSetMotors(_Speed,_Speed);    
            break;
        case 0x3: //центральный и правый на черном
            procSetMotors(_Speed+ (70 * koef_smooth),_Speed- (70 * koef_smooth));
            break;
        case 0x4: //левый на черном
            procSetMotors(_Speed-(160*koef_sharp),_Speed+(160*koef_sharp));   
            break;
        case 0x5: //левый и правый на черном
            break;
        case 0x6: //левый и центральный на черном
            procSetMotors(_Speed-(70 * koef_smooth),_Speed+(70 * koef_smooth));
            break;
        case 0x7: //все на черном
            procSetMotors(_Speed,_Speed);
            break;
    }
}

void DigitalLineRobot::two_private_Move()
{
    Read_private();
    switch(_byte_sensor_values)
    {
        case 0x0: //Все видят белое
            procSetMotors(_Speed,_Speed);
            break;
        case 0x1: //правый на черном
            procSetMotors(_Speed+(160*koef_sharp),_Speed-(160*koef_sharp));
            break;
        case 0x2: //левый на черном
            procSetMotors(_Speed-(160*koef_sharp),_Speed+(160*koef_sharp));
            break;
        case 0x3: //оба на черном
            procSetMotors(_Speed,_Speed);
            break;
    }
}

//==============================Analog===========================================
AnalogLineRobot::AnalogLineRobot(unsigned char* pins,int num_sensor)
{
    _num_sensor = num_sensor;
    _Speed = 120;

    if (_pins == 0||_sensor_values == 0)
    {
        _pins = (unsigned char*)malloc(sizeof(unsigned char)*_num_sensor);
        _sensor_values = (short int*)malloc(sizeof(short int)*_num_sensor);
        if (_pins == 0||_sensor_values == 0)
            return;
    }
    _pins=pins;
    for(int i = 4; i < 8; i++) pinMode(i, OUTPUT);
    for (unsigned int i=0;i<_num_sensor;i++) pinMode(_pins[i],INPUT);
      
}

void AnalogLineRobot::Move()
{
    switch(_num_sensor)
    {
        case 3:
            three_private_Move();
            break;
        case 2:
            two_private_Move();
            break;
        case 1:
            one_private_Move();
            break;
    }
}

void AnalogLineRobot::three_private_Move()
{
    Read_private();
}

void AnalogLineRobot::two_private_Move()
{
    Read_private();
	int datLeft = _sensor_values[0];
	int datRight = _sensor_values[1];
	
	
	int error = datLeft - datRight;
	int derr = error - old_error;
	
	double PID_sum = kp*error + kd*derr;
	old_error = error;
	
	procSetMotors(_Speed+PID_sum, _Speed-PID_sum);
}

void AnalogLineRobot::one_private_Move()
{
    Read_private();
    //*_sensor_values= (int)((*_sensor_values - _calmin) * 1024.0/(double)_denomination);

    if ((*_sensor_values>0)&&(*_sensor_values<=400)){
    procSetMotors(_Speed-100,_Speed+100);
    }
    else if ((*_sensor_values>400)&&(*_sensor_values<800)){
        procSetMotors(_Speed,_Speed);
    }
     else if ((*_sensor_values>=800)&&(*_sensor_values<=1023)){
         procSetMotors(_Speed+100,_Speed-100);
    }
}


//==============================Получить значения датчиков===========================================
void AnalogLineRobot::GetSensor(short int* sensor_values,bool Mode_print)
{
    private_GetSensor(sensor_values,Mode_print);
}

void AnalogLineRobot::GetSensor(short int* sensor_values)
{
    private_GetSensor(sensor_values);
}

void DigitalLineRobot::GetSensor(short int* sensor_values,bool Mode_print)
{
    for (unsigned char i = 0;i<_num_sensor;i++)
    {
        _sensor_values[_num_sensor-i-1] = _byte_sensor_values&0x1;
        _byte_sensor_values = _byte_sensor_values >> 1;
    }
    // for (unsigned char i = 0;i<_num_sensor;i++)
    // {
    //     _sensor_values[i] = _byte_sensor_values&0x1;
    //     _byte_sensor_values = _byte_sensor_values >> 1;
    // }
    private_GetSensor(sensor_values,Mode_print);
}

void DigitalLineRobot::GetSensor(short int* sensor_values)
{
    for (unsigned char i = 0;i<_num_sensor;i++)
    {
        _sensor_values[_num_sensor-i-1] = _byte_sensor_values&0x1;
        _byte_sensor_values = _byte_sensor_values >> 1;
    }
    private_GetSensor(sensor_values);
}



