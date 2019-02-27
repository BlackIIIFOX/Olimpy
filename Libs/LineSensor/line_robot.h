/*
	Классы для реализации движения по линии при количестве датчиков от одного до трех.
	Абстрактный класс line_robot определяет общую функциональность объектов. 
	Наследуемые от него классы описывают методы для движения по линии при разных количествах датчиков.
*/
//#include "Arduino.h"
#ifndef line_robot_H
#define line_robot_H


class LineRobot
{
	public:	
		LineRobot()
		{
			_num_sensor=0;
		}
		unsigned char _Speed; //скорость двигателей
		
	protected:
		void Read_private(); //Считать значения с датчиков
		
		void private_GetSensor(short int* sensor_values,bool Mode_print); //вывести значения с датчиков через com port,и записать в массив
		void private_GetSensor(short int* sensor_values); //записать значения датчиков в массив
		
		void procSetMotors( int leftSpeed, int rightSpeed); //установка скоростей моторов
		unsigned char *_pins;//массив пинов подключенных датчиков линии
		int _num_sensor; //кол-во датчиков
		short int *_sensor_values; //показания датчиков
		
};

class AnalogLineRobot : public LineRobot
{
	public:
		AnalogLineRobot(unsigned char* pins,int num_sensor);//конструкт
		void GetSensor(short int* sensor_values,bool Mode_print);//считать аналоговые значения(0-1023) в массив и вывести в com порт
		void GetSensor(short int* sensor_values);//считать аналоговые значения(0-1023) в массив
		void Calibrate();//калибровка
		void Move();//движение,вызывает разные _private_Move
		void Set_Calibrate(short int calmax,short int calmin);//установить минимальные и максимальные значения
		short int _calmin,_calmax;//минимальное и максимальное значение калибровки
	private:
		void three_private_Move(); //движение на трех аналоговых датчиках
		void two_private_Move(); //движение на двух аналоговых датчиках
		void one_private_Move(); //движение на одном аналоговом датчике
		void Read_private();//чтение датичков аналоговых в массив
		short int _denomination;//значение коэффициента калибровки
		double kp = 0.15; // коэф. проп. части регулятора
		double kd = 0;	// коэф. диф. части регулятора
		int old_error = 0;	// Ошибка регулятора на предыдущем шаге

};

class DigitalLineRobot : public LineRobot
{
	public:
		DigitalLineRobot(unsigned char* pins,int num_sensor);//конструкт
		unsigned char koef_smooth;//коэффициент плавного поворота
		unsigned char koef_sharp;//коэффициент резкого поворота
		void GetSensor(short int* sensor_values,bool Mode_print);
		void GetSensor(short int* sensor_values);
		void Move();//движение,вызывает разные _private_Move
		unsigned char _byte_sensor_values; //значения датчиков
	private:
		void three_private_Move(); //движение на трех датчиках в режиме цифрового
		void two_private_Move(); //движение на двух датчиках в режиме цифрового
		void Read_private();//чтение дискретных значений в байт
		
};


#endif