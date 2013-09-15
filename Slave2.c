																																							/*program to scan from 0 to 180 degrees, take ultrasonic sensor readings,
 /*store distance and angle of the minimum reading over the entire span
 and send this data over xbee*/
#include <math.h> 
#include <reg51.h>
#include <stdio.h>
#include <reg_c51.h>


#define ITERVAL 2
#define UL 0xF5
#define LL 0xE7
#define readings 15
#define Xnode 2
#define Ynode 94
#define step 82

sbit RX = P3^3;
sbit PW = P3^5;


sbit clock = P3^7;
sbit pwm = P1^3;
sbit clock_in =P1^2;

void timer_delay();
void _delay_ms(unsigned int t);
void dig_disp(unsigned int);
void send( unsigned char);
void delay();
unsigned int take_readings();
void send_number(unsigned int);
//void send_w();
void calculate_coordinates();
//unsigned int i, j;
sbit T2ex = P1^1;
//unsigned char A[12];
unsigned int M[readings],iter=2;
unsigned int c, i, j, k;
unsigned char recbyte, sendbyte,flag = 1, key, flagX=0;
float temp;
unsigned int a1,a2,a3,a0,n;
long unsigned int w;
unsigned int min_angle;
unsigned int min_dist, current_dist,cur_angle, x, y;
double actual_min_angle;
double sin_min_angle, cos_min_angle;
int int_sin, int_cos;

void timer1() interrupt 5 
	{
		TF2=0;
		clock=!clock;
		TH2=0xFF;
		TL2=0xE2;
		TR2=1;	
		
	}
		
void serial(void) interrupt 4
{	
	if (RI == 1)
	{ 
		TR1 = 0;
		_delay_ms(20);
		TMOD=0x20;
		TH1=0xFD;
		SCON=0x50;
		TR1=1;
		_delay_ms(20);	

		recbyte = SBUF;
		key = recbyte;
		_delay_ms(20);
		RI = 0;
		
	}
	else if (TI == 1)
	{	
		if (flag != 0 )
		{
			TR1=0;
			_delay_ms(40);
			TMOD=0x20;
			TH1=0xFD;
			SCON=0x50;
			TR1=1;
			_delay_ms(80);
			SBUF=sendbyte;   
			TI = 0;
			flag = 0;
			
		}
		else 
		{ TI = 0; }
		
	}
	return;
}



void main()
{

	for (i =0;i<readings;i++)
	{
		M[i] = 255;
	}
	clock_in=1;
	clock=0;

	T2MOD = 0x01;
	T2CON = 0x40;
	TH2=0xFF;	//FFE4 timer value gives a pwm with cycle length of 21.1 ms
	TL2=0xE2;	//FFE7 gives 20ms
	T2ex = 1;
	
	
	IE=0xB0;
	TR2=1;
	

			
	CMOD=0x0006;	
	CL=0x00;
	CH=0x00;
	CCAPM0=0x42;	
	CCAP0L=0xEF;
	CCAP0H=0xEF;	 //F9-577us at full length 21.1; F9-547us at 20ms full length
	pwm=1;
	
	CCON=0x40;

	RX = 1;
	timer_delay();
	TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	c=UL;
	iter = 0;
	while(1)
	{	c =UL;
	CCAP0H=c;
		TR1=1;
		//if (iter) key = 'C';
	//	else key = ' ';
		while(1)
		{
			if(iter)
			{	
				_delay_ms(100);
				break;
			}
			if(key=='C')
			{
				key=' ';
				iter  = ITERVAL;
				for (i =0;i<readings;i++)
				{
					M[i] = 255;
				}
				break;
			}
			if(key=='T')
			{
				key=' ';
				iter  = 0;
				break;
			}
		}
		TR1=0;
		k =100;
		min_dist = 255;
		min_angle = 200;
		while(c>=LL)
			{
				TR2=1;
			
				CCAP0H=c;
				_delay_ms(1000);
			   	TR2=0;
				_delay_ms(500);

				current_dist=take_readings();
				cur_angle=(int)(((250.0-(double)c)*1800.0)/22.0);
				//send_number(0xUL-c);
				if (iter)
				{	if (current_dist<M[UL-c])
					{	
						if(current_dist>30)
						M[UL-c] = current_dist;
					}
					//send_number(current_dist);
				//	M[0xUL-c] = M[UL-c]/ITERVAL;
					c--;
					continue;
				}
				
				j = (unsigned int)(current_dist*100)/(M[UL-c]);
							
				if(j<80&&current_dist>30)
				{  	
					if (current_dist<min_dist) 
					{	//k = j;
						min_dist = current_dist;
						min_angle = cur_angle;
					}
				}	
				key=' ';
				c-=1;
		//		send('A');
				if (c == (LL-1))
				{	if (!iter)
					{	
						TMOD=0x20;
						TH1=0xFD;
						SCON=0x50;
						TR1=1;
		//				send('B');
						//					send('D');
							while(key!='E');
							//if(key='E');
							//{	//flagX =1;
							//key='E';
							send('X');
							_delay_ms(50);

							while(key=='E')
							{ 	key=' ';
							  	send('X');
								_delay_ms(500);
							}
							_delay_ms(100);
							while(key=='E')
							{ 	key=' ';
							  	send('X');
								_delay_ms(50);
							}
							_delay_ms(100);
							while(key=='E')
							{ 	key=' ';
							  	send('X');
								_delay_ms(5);
							}
						//	if (flagX ==1)
						//	{
						//		flagX=0;
						//		break;
						//	}
						//	}
					//	_delay_ms(50);
						//send_number(min_dist);
						//send('@');
					//	send_number(min_angle);
						_delay_ms(500);
						send('V');
						_delay_ms(500);
						if (min_dist == 255) 
						{
							for(i=0;i<6;i++)
							{send(0);
							}
						}
						else
						{
						min_angle-=step;
						calculate_coordinates();
						min_angle+=step;
						calculate_coordinates();
						min_angle+=step;
						calculate_coordinates();
						}

						//send_number((int)(sin(((float)min_angle)/100.0)*min_dist));
					//	send_number(50-(int)(cos(((float)min_angle)/100.0)*min_dist));
						send('F');
						while(key!='G')
						{	key=' ';
							send('F');
							_delay_ms(100);
						}
						key=' ';
					}
					
				}
			}
			TR1 = 0;
			_delay_ms(20);
			TMOD=0x20;
			TH1=0xFD;
			SCON=0x50;
			TR1=1;
			_delay_ms(20);

		if (iter)
		{	iter--;
			if (iter == 0)
			for (i=0;i<readings;i++)
			{	
				//M[i] = M[i]/ITERVAL;
				
				send(' ');
				send_number(i);
				send(' ');
				send_number((int)(((250.0-(double)(UL -i))*1800.0)/22.0));
				send(' ');
				send_number(M[i]);
			}
		}


		c  = LL;
		CCAP0H=c;
		//if (iter) key = 'C';
		//else key = ' ';
		TR1=1;
		while(1)
		{
			if(iter)
			{	_delay_ms(10);
				break;
			}
			if(key=='C')
			{
				key=' ';
				iter  = ITERVAL;
				for (i =0;i<readings;i++)
				{
					M[i] = 255;
				}
				break;
			}

			if(key=='T')
			{
				key=' ';
				
				iter = 0;
				break;
			} 
		}	
			_delay_ms(10);
			k = 100;
			min_dist = 255;
			min_angle = 200;
			TR1=0;
		while(c<=UL)
			{
				TR2=1;	
					
				CCAP0H=c;
				_delay_ms(1000);
				TR2=0;
				_delay_ms(500);
				current_dist=take_readings();
				cur_angle=(int)(((250.0-(double)c)*1800.0)/22.0);
				if (iter)
				{	if (current_dist<M[UL-c])
					{		
						if(current_dist>30)						
						//M[UL -c]= M[UL -c]*ITERVAL;
						M[UL -c] = current_dist;
					}
					//send_number(current_dist);
					//M[UL-c] = M[UL-c]/ITERVAL;
					c++;
					continue;
				}
				
				j = (unsigned int)(current_dist*100)/M[UL-c];
				
								
				if(j<80&&current_dist>30)
				{  	
					if (current_dist<min_dist) 
					{	//k = j;
						min_dist = current_dist;
						min_angle = cur_angle;
					}
				}
				key=' ';
				c+=1;

		//		send('A');
				if (c == (UL+1))
				{	if (!iter)
					{	
						TMOD=0x20;
						TH1=0xFD;
						SCON=0x50;
						TR1=1;
		//			send('B');
													//send('C');
							while(key!='E');
							//if(key='E');
							//{	//flagX =1;
							//key='E';
							send('X');
							_delay_ms(50);

							while(key=='E')
							{ 	key=' ';
							  	send('X');
								_delay_ms(500);
							}
							_delay_ms(100);
							while(key=='E')
							{ 	key=' ';
							  	send('X');
								_delay_ms(50);
							}
							_delay_ms(100);
							while(key=='E')
							{ 	key=' ';
							  	send('X');
								_delay_ms(5);
							}
						//_delay_ms(50);
					//	send_number(min_dist);
					//	send('@');
						//send_number(min_angle);
						
						_delay_ms(500);
						send('V');
						_delay_ms(500);
						if (min_dist == 255) 
						{
							for(i=0;i<6;i++)
							{send (0);
							}
						}
						else
						{
					//	send(' ');
					//	send_number(min_dist);
						//send(' ');
					//	send_number(min_angle);
					//	send(' ');
						
						min_angle-=step;
						calculate_coordinates();
						min_angle+=step;
						calculate_coordinates();
						min_angle+=step;
						calculate_coordinates();
						}
						send('F');
						while(key!='G')
						{	key=' ';
							send('F');
							_delay_ms(100);
						}
						key=' ';
					}
				}
			}
		
			TR1 = 0;
			_delay_ms(20);
			TMOD=0x20;
			TH1=0xFD;
			SCON=0x50;
			TR1=1;
			_delay_ms(20);
			
		
		if (iter)
		{	iter--;
			if (iter == 0)
			{
				for (i=0;i<readings;i++)
				{	
					//M[i] = M[i]/ITERVAL;
					
					send(' ');
					send_number(i);
					send(' ');
					send_number((int)(((250.0-(double)(UL -i))*1800.0)/22.0));
					send(' ');
					send_number(M[i]);
				}

			}
		}
	}
}

void calculate_coordinates()
{
	actual_min_angle=((((double)min_angle)*3.142)/1800.0);
					
						sin_min_angle=sin(actual_min_angle);
						//int_sin=(int)(sin_min_angle*100.0);
						
						cos_min_angle=cos(actual_min_angle);
						//int_cos=(int)(cos_min_angle*100.0);

						x=(int)(((float)min_dist)*sin_min_angle);
						x+=Xnode;
						
						y=(int)(((float)min_dist)*cos_min_angle);
						y=Ynode-y;
						
					//	send('X');
						send(x/10);
					//	send('Y');
						send(y/10);

						//send((x/10)+1);
						//send((y/10)+1);

}
void send(unsigned char c)
{
	sendbyte=c;

	flag = 1;
	TI=1;
	

	_delay_ms(300);
	//	sendbyte=' ';
	TI=0;
	
}
unsigned int take_readings()
{
		RX=0;
		
		TMOD = 0x01;
		TH0 = 0x00;
		TL0 = 0x00;
		timer_delay();

		RX=1;	
		while (PW==0);
		TR0=1;
		while (PW==1);
		TR0=0;
		RX=0;
		
		w = TH0;
		w = (w*256) + TL0;
		temp = ((w*(24.0/11.0592))/147.0)*8.0/15.0;
				
		n = (int) temp;
		return n;	
}
void dig_disp(unsigned int a)
{
	switch (a)
	{
		case 0: send('0'); 
				break;
		case 1: send('1');
				break;
		case 2: send('2');
				break;
		case 3: send('3');
				break;
		case 4: send('4');
				break;
		case 5: send('5');
				break;
		case 6: send('6');
				break;
		case 7: send('7');
				break;
		case 8: send('8');
				break;
		case 9: send('9');
				break;

	}
} 
void _delay_ms (unsigned int t)
{	unsigned int i,j;
	for(i = 0;i<t;i++)
		for(j=0;j<10;j++);
}
void timer_delay()
{
		TMOD = 0x01;
		TH0 = 0x00;
		TL0 = 0x00;
		TR0=1; 
		while(TF0==0);
		TR0 = 0;
		TF0=0;
}
void send_number(unsigned int n)
{					   
			a0 = n%10;
			n  = n/10;	
			a1 = n%10;
			n = n/10;
			a2 = n%10;
			n = n/10;
			a3 = n%10;
			n=n/10;
			
					
			dig_disp(a3);
			dig_disp(a2);
			dig_disp(a1);
			dig_disp(a0);
			send('!');
			TR1=0;
			_delay_ms(50);
}
