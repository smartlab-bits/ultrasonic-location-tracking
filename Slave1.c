#include <reg51.h>
#include <stdio.h>
#include <reg_c51.h>
#include <math.h>
#define ITERVAL 2
#define readings 13
#define UL 0xF5
#define LL	0xE9
sbit RX = P3^2;
sbit PW = P3^5;
sbit BW = P3^6;
#define Xnode 124
#define Ynode 192
#define step 82
sbit clock = P3^7;
sbit pwm = P1^3;
sbit clock_in =P1^2;
sbit T2ex = P1^1;

void timer_delay();
void _delay_ms(unsigned int t);
void dig_disp(unsigned int);
void send( unsigned char);
void delay();
void rec();
void recE(unsigned char);
void send_number(unsigned int);
void calculate_coordinates();
void get_commands();
void move_and_read(unsigned char);
void send_readings();
void send_calibration();
unsigned int a1,a2,a3,a0,n;
unsigned char recbyte, flag = 0;
unsigned int take_readings();



int i=0, j; 

unsigned int min_angle;
unsigned int min_dist, current_dist,cur_angle, x, y;
unsigned char A[5], c, ratio, key;
unsigned int M[readings],iter=2;

double actual_min_angle;
double sin_min_angle, cos_min_angle;


void timer1() interrupt 5 
	{
		TF2=0;
		clock=!clock;
		TH2=0xFF;
		TL2=0xE2;
		TR2=1;	
		
	}
	
	/*void serial(void) interrupt 4
{	
	/*if (RI == 1)
	{ 
		/*TR1 = 0;
		_delay_ms(20);
		TMOD=0x20;
		TH1=0xFD;
		SCON=0x50;
		TR1=1;
		_delay_ms(20);	
		
		recbyte = SBUF;
		key 
		_delay_ms(20);
		RI = 0;
		
	}*/
	/*if (RI == 1)
	{ 
		/*TR1 = 0;
		_delay_ms(20);
		TMOD=0x20;
		TH1=0xFD;
		SCON=0x50;
		TR1=1;
		_delay_ms(20);	
	
		A[i]=SBUF; 
		i++;
		if(i==4)
		{
			i=0;
			flag2=1;
		}
		_delay_ms(20);
		RI = 0;
		
	}
	 if (TI == 1)
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
} */


void main ()
{
	
	unsigned int i;
	
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
	
	
	IE=0xA0;
	TR2=1;
	

			
	CMOD=0x0006;	
	CL=0x00;
	CH=0x00;
	CCAPM0=0x42;	
	CCAP0L=0xEF;
	CCAP0H=0xEF;	 //F9-577us at full length 21.1; F9-547us at 20ms full length
	pwm=1;
	
	CCON=0x40;

	
//	send('a');
	//_delay_ms(250);
	//BW = 0;
	
	//IE=0x75;
	//TI = 1;
	//init_LCD();	
	
	c=UL;
	iter = 0;

//	RX = 0;			 
//	for(i=0;i<20;i++);	   //Wait for 10us 
//	RX = 1;			   //Trigger input
//	for(i=0;i<20;i++);	   //Wait for 100us 
//	BW = 0;
//	RI=0;


		while(1)
	{	
		RX=0;
		BW=1;
		c =UL;
		//if (iter) key = 'C';
		//else key = ' ';
		get_commands();
		
	
		//k =100;
		min_dist = 255;
		min_angle = 200;
		move_and_read(LL-1);
		send_calibration(); 
		  
	   
		c  = LL;
	//	if (iter) key = 'C';
	//	else key = ' ';
		get_commands();	
			_delay_ms(10);
//			k = 100;
			min_dist = 255;
			min_angle = 200;
			move_and_read(UL+1);
			_delay_ms(10000);
	
			
		
			/*TR1 = 0;
			_delay_ms(20);
			TMOD=0x20;
			TH1=0xFD;
			SCON=0x50;
			TR1=1;
			_delay_ms(20);
			  */
		
		send_calibration();
	}	
}
void move_and_read(unsigned char limit)
{
				
	while((limit==LL-1)?c>=LL:c<=UL)
	{
				CCAP0H=c;
				TR2=1;
				_delay_ms(1000);
			   	TR2=0;
				_delay_ms(1000);

				current_dist=take_readings();
				//cur_angle=

				if (iter)
				{	if (current_dist<M[UL-c])
					{	if (current_dist>30)
						//	M[0xF8-c]= M[0xF8 -c]*ITERVAL;
						M[UL-c] = current_dist;
					 }
					send_number(current_dist);
				//	M[0xF8-c] = M[0xF8-c]/ITERVAL;
					
					if(limit==LL-1)c--;
					else c++;

					continue;
				}
				
	   
				j = (unsigned int)(current_dist*100)/(M[UL-c]);
							
				if(j<90&&current_dist>30)
				{  	
					if (current_dist<min_dist) 
					{	//k = j;
						min_dist = current_dist;
						min_angle = (int)(((250.0-(double)c)*1800.0)/22.0);;
					}
				}	
				
				if(limit==LL-1)c-=1;
				else c+=1;

				
				if (c == (limit))
				{	if (!iter)
					{	
					send_readings();
						
					}
				}
		}
}
void send_calibration()
{
	if (iter)
		{	iter--;
			if (iter == 0)
			for (i=0;i<readings;i++)
			{	
				//M[i] = M[i]/ITERVAL;
				
				send(' ');
				send_number(i);
				send(' ');
				send_number((int)(((250.0-(double)(UL-i))*1800.0)/22.0));
				send(' ');
				send_number(M[i]);
			}
		}
}
void send_readings()
{
						
						TMOD=0x20;
						TH1=0xFD;
						SCON=0x50;
						TR1=1;
						
						while(recbyte!='M')
						rec();
						while(recbyte == 'M')
						{ 	recbyte=' ';
							send('A');
							recE(1000);
							//_delay_ms(50);
						}
					//	_delay_ms(50);
						//send(' ');
					//	send_number(min_dist);
						//send('@');
						//send_number(min_angle);
						_delay_ms(500);
						send('Q');
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
						send('E');
						recE(100);
						while(recbyte!='X')
						{	send('E');
							recE(100);
							//_delay_ms(100);
						}
						recbyte=' ';
						
}	
void get_commands()
{
	while(1)
		{
			if(iter)
			{	_delay_ms(10);
				break;
			}
			rec();
			key=recbyte;
			recbyte=' ';
			if(key=='b')
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
}		
void calculate_coordinates()
{
	actual_min_angle=((((double)min_angle)*3.142)/1800.0);
						
						sin_min_angle=sin(actual_min_angle);
						//int_sin=(int)(sin_min_angle*100.0);
						
						cos_min_angle=cos(actual_min_angle);
						//int_cos=(int)(cos_min_angle*100.0);

						x=(int)(((float)min_dist)*cos_min_angle);
						x=Xnode-x;
						
						y=(int)(((float)min_dist)*sin_min_angle);
						y=Ynode-y;
						
						//send('X');
						send(x/10);
					//	send('Y');
						send(y/10);

						//send_number((x/10)+1);
						//send_number((y/10)+1);
					}
 void send(unsigned char a)
 {
	TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	SBUF=a;
	while(TI==0);
	TI=0;
	TR1=0;
	_delay_ms(300);
	return;	
 }
void rec()
{
	TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	while(RI==0);
	recbyte=SBUF;
	RI=0;
	TR1=0;
	return;
	
}
void recE(unsigned char a)
{
	//TMOD = 0x01;
		 
	//	while(TF0==0);
	//	TR0 = 0;
		//TF0=0;
		
	int ctr=0;
	TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	
//	TH0 = 0x00;
//	TL0 = 0x00;
//	TR0=1;
	
	while(RI==0&ctr<a)
	{ctr++;
	_delay_ms(50);
	}
	if(ctr<a)
	{ 
	recbyte=SBUF;
	RI=0;
	TR1=0;
	}
		else recbyte=' ';
	return;
}

unsigned int take_readings()
{
	TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	
	RX=1;
	BW=0;
		 for(i=0;i<20;i++);	   //Wait for 100us 
		//init_LCD();
		  RI=0;
		_delay_ms(1);
		// BW =0;
		//send('W');//remove later
		 for(i=0;i<5;i++)
		 {
			while(RI==0);
			//send('!');//remove later
			 A[i]=SBUF;
			 RI=0;
			
		}
		//send('G');//remove later
		RX=0;
		BW=1;
//		for(i=0;i<4;i++)
//		send(A[i]);
		TR1=0;
		return(100*(A[1]-48)+10*(A[2]-48)+(A[3]-48));
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
	return;
} 
void _delay_ms (unsigned int t)
{	unsigned int i,j;
	for(i = 0;i<t;i++)
		for(j=0;j<10;j++);
}
/*void timer_delay()
{
		
} */
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
}	 
