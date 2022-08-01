/*
 * V-Metor.c
 *
 * Created: 29.03.2020 0:49:45
 * Author : Gold_Nik
 */ 
#define F_CPU 1000000UL // ������� �������� ������� ����������������
#include <avr/io.h> // ��������� ����� ��������� �������� ������ �� ���������
#include <util/delay.h> // ��������� ����� ������������� ������� �������� � ���������
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

float value = 1;//��������� �������� ���������� 
float Uvh = 0;

unsigned int u = 0; //���������� ���������� � ���������� ��������������
//double Vref = 2.56;
float Uin = 0, mVolt = 0;
char K1 = 100;

 
void RezridCislo(unsigned long int razridP)
{
	//razrad4
	raz1 = razridP%10;//�������
	raz2 = razridP%100/10;//�������
	raz3 = razridP%1000/100;//�����
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
//������� ����������
ISR(TIMER0_OVF_vect)
{
	if(Lag == 1) {Chislo = (chisla [raz1])|((1<<Y1)|(1<<Y0)); } // ���������� 1-� �����
	if(Lag == 2) {Chislo = (chisla [raz2])|((1<<Y1)|(0<<Y0)); 
		if ((Uin >= 10) || (Uin < 1)) {Chislo |= (1<<H);}else {Chislo &= ~(1<<H);}  } // ���������� 2-� �����
	if(Lag == 3) {Chislo = (chisla [raz3])|((0<<Y1)|(1<<Y0)); 
		if ((Uin < 10)&& (Uin >= 1)) {Chislo |= (1<<H);}
			else {Chislo &= ~(1<<H);} } // ���������� 3-� �����
	if(Lag == 4) {Chislo = (chisla [raz4])|((0<<Y1)|(0<<Y0)); } // ���������� 4-� �����
	//if(Lag == 5) {Razrid = 0b0000100; Chislo = chisla [raz5];} // ���������� 5-� �����
	//if(Lag == 6) {Razrid = 0b0000101; Chislo = chisla [raz6];} // ���������� 6-� �����
	Lag ++;
	if(Lag >= 5)
	{
		Lag = 1;
	}
}

int main(void)
{
	
	DDRD = 0xFF; //������������� ���� D �� ����� 
	DDRC &= ~(1<<0);
	DDRC |= (1<<2);
	Chislo = 0b0000000;
	//�������� ���
	ADCSRA |=(1<<ADEN) ;//��������� ������ ���
	ADCSRA |= (1<<ADFR); //����� ������������ ��������� ������� 
	//������ ������� ���
	ADCSRA &= ~(1<<ADPS2);
	ADCSRA |= (1<<ADPS1) | (1<<ADPS0); //������� ������������� 125 ��� /8
	
	ADMUX |= (1<<REFS0); ADMUX &= ~(0<<REFS1); //���������� �������  �������� �������� ������ = 5 �
 
	ADMUX &= ~(1<<ADLAR); //�������������� ������������
	ADMUX &= ~((1<<MUX3) |(1<<MUX2) |(1<<MUX1) |(1<<MUX0) );// ����������� ADC0
	ADCSRA |= (1<<ADSC);//�������� ���
	//long int a = 0;
	
	//��������� �0
	TCCR0 |= (1<<1); TCCR0 &= ~((1<<0) | (1<<2)); //f/8
	TIMSK |= (1<<0);
	TCNT0 = 0;
	_delay_ms(50); 
	sei();  //���������� ���������� ����������

	
	while (1)
	{
		if(ADCSRA & (1<<4))//������ ���� ���� 4 ADCSRA
		{
			u = ADC;//����������� �������� �������� � �������� ���
			Uin = (5.01*u*value)/1024;//����� �������� �������� ���������� (�������� �������� � �������� ��� * U����*����.�������� ���������� )/1024
			//value = value + ADC;                  //������ � ���������� ���������� ��������������
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
				
			RezridCislo((Uin*K1)); //����������� ����� �� �������
			sei();
			
			_delay_ms(300);
			
			ADCSRA|=(1<<4);//START ADC
		}
	}
		
}

