/*
 * V-Metor.c
 *
 * Created: 29.03.2020 0:49:45
 * Author : Gold_Nik
 */ 
#define F_CPU 1000000UL // задание тактовой частоты микроконтроллера
#include <avr/io.h> // заголовок чтобы разрешить контроль данных на контактах
#include <util/delay.h> // заголовок чтобы задействовать функции задержки в программе
#include <avr/interrupt.h>

#define  Chislo PORTD
//Razrad
#define Y0 4 //PD4
#define Y1 5 //PD5
#define H 6 //PD5

unsigned int chisla [10] = {

	0b0000000,0b0000001, 0b0000010, 0b0000011, 0b0000100, 0b0000101, 0b0000110, 0b0000111, 0b0001000, 0b0001001
};

unsigned int raz1 = 0 , raz2 = 0 , raz3 = 0, raz4 = 0 ;// raz5 = 0, raz6 = 0;
unsigned char Lag = 1;

float value = 1;//коефицент делителя напряжения 
float Uvh = 0;

unsigned int u = 0; //Глобальная переменная с содержимым преобразования
//double Vref = 2.56;
float Uin = 0, mVolt = 0;
char K1 = 100;

 
void RezridCislo(unsigned long int razridP)
{
	//razrad4
	raz1 = razridP%10;//единицы
	raz2 = razridP%100/10;//десятки
	raz3 = razridP%1000/100;//сотни
	raz4 = razridP/1000;
	//razrad6
	/*
	raz1 = razridP/100000;
	raz2 = razridP%100000/10000;
	raz3 = razridP%10000/1000;
	raz4 = razridP%1000/100;
	raz5 = razridP%100/10;
	raz6 = razridP%10;
	*/
}
//функция приревания
ISR(TIMER0_OVF_vect)
{
	if(Lag == 1) {Chislo = (chisla [raz1])|((1<<Y1)|(1<<Y0)); } // отображаем 1-ю цифру
	if(Lag == 2) {Chislo = (chisla [raz2])|((1<<Y1)|(0<<Y0)); 
		if ((Uin >= 10) || (Uin < 1)) {Chislo |= (1<<H);}else {Chislo &= ~(1<<H);}  } // отображаем 2-ю цифру
	if(Lag == 3) {Chislo = (chisla [raz3])|((0<<Y1)|(1<<Y0)); 
		if ((Uin < 10)&& (Uin >= 1)) {Chislo |= (1<<H);}
			else {Chislo &= ~(1<<H);} } // отображаем 3-ю цифру
	if(Lag == 4) {Chislo = (chisla [raz4])|((0<<Y1)|(0<<Y0)); } // отображаем 4-ю цифру
	//if(Lag == 5) {Razrid = 0b0000100; Chislo = chisla [raz5];} // отображаем 5-ю цифру
	//if(Lag == 6) {Razrid = 0b0000101; Chislo = chisla [raz6];} // отображаем 6-ю цифру
	Lag ++;
	if(Lag >= 5)
	{
		Lag = 1;
	}
}

int main(void)
{
	
	DDRD = 0xFF; //иницилезипуем порт D на выход 
	DDRC &= ~(1<<0);
	DDRC |= (1<<2);
	Chislo = 0b0000000;
	//Включаме АЦП
	ADCSRA |=(1<<ADEN) ;//разрешаем работу АЦП
	ADCSRA |= (1<<ADFR); //режим неприрывного измерения включен 
	//Задаем частоту АЦП
	ADCSRA &= ~(1<<ADPS2);
	ADCSRA |= (1<<ADPS1) | (1<<ADPS0); //Частота дискретизации 125 кГц /8
	
	ADMUX |= (1<<REFS0); ADMUX &= ~(0<<REFS1); //ИСПОЛЬЗУЕМ внешний  источник опорного напряж = 5 В
 
	ADMUX &= ~(1<<ADLAR); //Правосторонние выравнивание
	ADMUX &= ~((1<<MUX3) |(1<<MUX2) |(1<<MUX1) |(1<<MUX0) );// Задействуем ADC0
	ADCSRA |= (1<<ADSC);//Стартуем АЦП
	//long int a = 0;
	
	//настройка Т0
	TCCR0 |= (1<<1); TCCR0 &= ~((1<<0) | (1<<2)); //f/8
	TIMSK |= (1<<0);
	TCNT0 = 0;
	_delay_ms(50); 
	sei();  //Глобальное разрешение прерываний

	
	while (1)
	{
		if(ADCSRA & (1<<4))//чекаем флаг бита 4 ADCSRA
		{
			u = ADC;//Присваиваем значение хранимое в регистре АЦП
			Uin = (5.01*u*value)/1024;//Опред величину входного напряжения (значение хранимое в регистре АЦП * Uопор*коеф.делителя напряжения )/1024
			//value = value + ADC;                  //Чтение и накопление результата преобразования
			//mVolt=(value+5)/32;
			if(Uin >= 10)
			{
				K1 = 10;
			}else if((Uin < 10)&& (Uin > 1) )
			{
				K1 = 100;
			}
			else
			{
				K1 = 10; 
			}
				
			RezridCislo((Uin*K1)); //Отображение числа на дисплей
			sei();
			
			_delay_ms(300);
			
			ADCSRA|=(1<<4);//START ADC
		}
	}
		
}

