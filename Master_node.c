																/*program to scan from 0 to 180 degrees, take ultrasonic sensor readings,
 store distance and angle of the minimum reading over the entire span
 and send this data over xbee*/
 
#include <reg51.h>
#include <stdio.h>
#include <reg_c51.h>
#include<math.h>

#define ITERVAL 2
#define step 82
#define UL 0xF3
#define LL 0xE9
#define Xnode 148
#define Ynode 2
#define readings 11

sbit RX = P3^3;
sbit PW = P3^5;
sbit sw = P2^7;

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
void calculate_coordinates();
void move_and_read(unsigned char);
void send_result();
void send_calibration();
//void send_w();

//unsigned int i, j;
sbit T2ex = P1^1;
//unsigned char A[12];
unsigned char iter=6,M[readings];
unsigned char c, i, j;//k,v2[9],w2[9],x1,x2,y1,y2,max_x,max_y,eq1,eq2,k,m1,m2;
unsigned char recbyte, sendbyte,flag = 1, key, C1[3][6], iterval;
//float temp;
int a1,a2,a3,a0,n,x,y;
char a4, a5, clusterflag;
long unsigned int w;
unsigned int min_angle,cur_angle; 
double actual_min_angle;
unsigned char ctr=0;
unsigned char min_dist, current_dist;
int Xav, Yav;
char size =2;
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
	key=' ';
	c=UL;
	while(key!='c');
	key=' ';
	iter=ITERVAL;
	for (i =0;i<readings;i++)
				{
					M[i] = 255;
				}

	while(iter!=0)
	{ 	c=UL;
		CCAP0H=c;
		move_and_read(LL-1);
		send_calibration();
		c=LL;
		CCAP0H=c;
		move_and_read(UL+1);		
		send_calibration();
	}
	sw=1;
	sw=1;
	TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	while(1)
	{
		while(key!='S');
		key=' ';
		iter=0;
		while(sw==1)
		{
			c=UL;
			CCAP0H=c;
			send('T');
			move_and_read(LL-1);
			c=LL;
			CCAP0H=c;
			send('T');
			move_and_read(UL+1);		
		}
	}

} // close main


/*	while(1)
	{	c =UL;
	CCAP0H=c;
		TR1=1;
		//if (iter) key = 'C';
		//else key = ' ';
		key=' ';
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
		TR1=0;//?
		//k =100;
		
		move_and_read(LL-1);
		move_and_read(UL+1);


		send_calibration();


		c  = LL;
		CCAP0H=c;
		if (iter) key = 'C';
		else key = ' ';
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
			//min_dist = 255;
			//min_angle = 200;
			
			TR1=0;//?
			move_and_read(UL+1);		
			
			
			
		
		send_calibration();
	}
}*/
void send_calibration()
{
		if (iter)
		{	iter--;
		if (iter == 0)
		{
			TR1 = 0;
			_delay_ms(20);
			TMOD=0x20;
			TH1=0xFD;
			SCON=0x50;
			TR1=1;
			_delay_ms(20);

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
}
void move_and_read(unsigned char limit)
{			
			TR1=0;
			min_dist = 255;
			min_angle = 200;			
			while(limit==LL-1?c>=LL:c<=UL)
			{
				TR2=1;
				CCAP0H=c;
				_delay_ms(1000);
			   	TR2=0;
				_delay_ms(500);

				current_dist=take_readings();
				cur_angle=(int)(((250.0-(double)c)*1800.0)/22.0);
				//send_number(0xFA-c);
				if (iter)
				{	if (current_dist<M[UL-c])
					{	
						if(current_dist>30)
						//	M[0xFA-c]= M[0xFA -c]*ITERVAL;
						M[UL-c] = current_dist;
					}
					TMOD=0x20;
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	//send_number(current_dist);
	TR1=0;
	//	M[0xFA-c] = M[0xFA-c]/ITERVAL;
					if(limit==LL-1)
					c--;
					else
					c++;
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
				
				if(limit==LL-1)
					c--;
					else
					c++;

				if (c == limit)
				{	if (!iter)
					{	
						TMOD=0x20;
						TH1=0xFD;
						SCON=0x50;
						TR1=1;
		//			send('B');
						
						
							send('M');
							_delay_ms(20);	
							//send('C');
							//while(key!='A');
						//	if(key='E');
							//{	//flagX =1;
						//	key=' ';
						
							while(key!='A')
							{ key=' ';
							_delay_ms(500);
							send('M');
							
							}
						
						while(key!='Q');	
						key=' ';
						for(i=0;i<6;i++)
						{
								while(key==' ');
								C1[0][i]=key;
								key=' ';
							
						}
						
						
						while(key!='V');
						key=' ';
						for(i=0;i<6;i++)
						{
							while(key==' ');
							C1[1][i]=key;
							key=' ';
						}
								
							//send('C');
							while(key!='F');
						//	if(key='E');
							//{	//flagX =1;
						//	key=' ';
							send('G');
							_delay_ms(500);
							while(key=='F')
							{ 	key=' ';
								send('G');
							_delay_ms(500);
							}
							_delay_ms(500);
							while(key=='F')
							{ 	key=' ';
								send('G');
							_delay_ms(50);
							}
							_delay_ms(50);
							while(key=='F')
							{ 	key=' ';
								send('G');
							_delay_ms(5);
							}
							
							
					//	for(i=0;i<6;i++)
						//{
						//	C1[0][i]-=48;
						//	C1[1][i]-=48;
						//}
					//	send_number(min_dist);
					//	send('@');
					//	send_number(min_angle);
					//	send(' ');
						if (min_dist == 255) 
						{
							for(i=0;i<6;i++)
						{
							C1[2][i]=0;
						}
						}
						else
						{
						
							
							i=0;
							
							min_angle-=step;
							calculate_coordinates();
							i+=2;
							min_angle+=step;
							calculate_coordinates();
							i+=2;
							min_angle+=step;
							calculate_coordinates();
						 }
						
						/*	for(i=0;i<3;i++)
							{for(j=0;j<3;j++)
							{	send('X');
								send_number(C1[i][2*j]);
 							    _delay_ms(100);
								send('Y');
								send_number(C1[i][2*j+1]);
								send(0x0D);
								//C1[1][i] = C1[1][i]/20;
								//C1[2][i] = C1[2][i]/20;
							}}*/
							a0=C1[0][2]-C1[1][2];
							a1=C1[1][2]-C1[2][2];
							a2=C1[2][2]-C1[0][2];
							if (a0<0) a0 = -1*a0;
							if (a1<0) a1 = -1*a1;
							if (a2<0) a2 = -1*a2;

							a3=C1[0][3]-C1[1][3];
							a4=C1[1][3]-C1[2][3];
							a5=C1[2][3]-C1[0][3];
							if (a3<0) a3 = -1*a3;
							if (a4<0) a4 = -1*a4;
							if (a5<0) a5 = -1*a5;


							clusterflag =7 ;
							
							if((a0<=size)&&(a3<=size))
							{
								if((a1<=size)&&(a4<=size))
									clusterflag=5;
								else
									if((a2<=size)&&(a5<=size))
									clusterflag=5;
									else
									clusterflag=2;
							}
							else if((a1<=size)&&(a4<=size))
							{
								if((a2<=size)&&(a5<=size))
									clusterflag=5;
								else
									//if(((a2<=4)||(a2>=-4))&&((a5<=4)||(a5>=-4))
									//clusterflag=1;
									//else
									clusterflag=0;
							}
							else if((a2<=size)&&(a5<=size))
							{
								//if(((a1<=4)||(a1>=-4))&&((a4<=4)||(a4>=-4))
								//	clusterflag=1;
								//else
								//	if(((a2<=4)||(a2>=-4))&&((a5<=4)||(a5>=-4))
								//	clusterflag=1;
								//	else
									clusterflag=1; //ctr = 6;
							}	
								ctr =0;
								for(i=0;i<3;i++)
								{
									if (i == clusterflag) continue;
									if((C1[i][0]!=0)&&(C1[i][1]!=0))ctr+=3;
								}
								Xav=0;
								Yav=0;
								for(i=0;i<3;i++)
								{	if (i == clusterflag) continue;
									else
									{
									 Xav+= C1[i][0]+C1[i][2]+C1[i][4];
									 Yav+= C1[i][1]+C1[i][3]+C1[i][5];
									}
								}
								Xav=(Xav/ctr)+1;
								Yav=(Yav/ctr)+1;

						/*	for(i=0;i<6;i++)
							{	
								C1[0][i] = C1[0][i]/20;
								C1[1][i] = C1[1][i]/20;
								C1[2][i] = C1[2][i]/20;
							}

							for(i=0;i<9;i++)
							{	v2[i] = 0 ; w2[i] = 0;		}

							for (i=0;i<3;i=i+1)
 						    {
								for(j=0;j<6;j+=2)
								{
									if (C1[0][j] == C1[i][0])	v2[3*i+j/2]++;
									if (C1[1][j] == C1[i][2])	v2[3*i+j/2]++;
									if (C1[2][j] == C1[i][4])	v2[3*i+j/2]++;
								
									if (C1[0][j+1] == C1[i][1]) w2[3*i+(j-1)/2]++;
									if (C1[1][j+1] == C1[i][3]) w2[3*i+(j-1)/2]++;
									if (C1[2][j+1] == C1[i][5]) w2[3*i+(j-1)/2]++;
								
								}

							}
							max_x = v2[0];
							max_y = w2[0];
							eq1=0;
							eq2=0;

							for (i=0;i<9;i++)
							{
								if (v2[i]>max_x)
								{	
									max_x = v2[i];
							 		eq1 = 0;
									j = i/3;
									k = i%3;

								}

								if (w2[i]>max_y)
								{	
									max_y = w2[i]; 
									eq2 = 0;
									m1 = i/3;
									m2 = i%3;
								}

								if (v2[i]==max_x)
								{	
									max_x = v2[i];
									if (C1[i/3][i%3] != C1[j][k])
							 		eq1++;
									if (eq1 ==1)
									{	x1 = i/3;x2 = i%3;	}
								}
							
								if (w2[i]==max_y)
								{	
									max_x = v2[i];
									if (C1[i/3][i%3] != C1[m1][m2])
							 		eq1++;
									if (eq2 ==1)
									{	y1 = i/3;y2 = i%3;	}
								}
	
							}*/
							

						//	Xav=(int)((((((float)Xav)-10.0)*1.25)+10)+0.5)/1;
						//	Yav=(int)((((((float)Yav)-10.0)*1.25)+10)+0.5)/1;
							
							if (clusterflag!=7)
							if(Xav<=20&&Yav<=20&&Xav>=1&&Yav>=1)
							{
								_delay_ms(5000);
								send('Z');
								send(Xav);
								send(Yav);
								_delay_ms(5000);
							}
							
							
							
							//send_number(Xav);
							//send_number(Yav);
							//_delay_ms(10);
							/*if ((!eq1)&&(!eq2))
							{
								send_number(C1[j][k]);
								_delay_ms(10);
								send_number(C1[m1][m2]);
								_delay_ms(10);
							}
							if (eq1 >1)
							{if (max_x!=1){send_number (0);_delay_ms(10);send_number(0);_delay_ms(10);}}

							if (eq2 >1)
							{if (max_y!=1){send_number(0);_delay_ms(10);send_number(0);_delay_ms(10);	}}
				
							if (eq1 ==1)
							{	if (max_x!=1)
								{	send_number((C1[(x1+j)/2][(x2+k)/2]));
								_delay_ms(10);
									send_number((C1[(y1+m1)/2][(y2+m2)/2]));
									_delay_ms(10);
								}
							}

							if (eq2 == 1)
							{	if (max_y!=1)
								{	send_number((C1[(x1+j)/2][(x2+k)/2]));
								_delay_ms(10);
									send_number((C1[(y1+m1)/2][(y2+m2)/2]));
									_delay_ms(10);
								}
							}
							else
							{
							send_number(0);
							_delay_ms(100);
							send_number(0);
							_delay_ms(100);
							}X*/
						}
						//send_result();
					
				}
			}
}
void calculate_coordinates()
{
	actual_min_angle=((((double)min_angle)*3.142)/1800.0);
					
						//sin_min_angle=);
						//int_sin=(int)(sin_min_angle*100.0);
						
						//cos_min_angle=);
						//int_cos=(int)(cos_min_angle*100.0);

						x=(int)(((float)min_dist)*(cos(actual_min_angle)));
						x+=Xnode;
						
						y=(int)(((float)min_dist)*(sin(actual_min_angle)));
						y=Ynode+y;
						
						//send('X');
						//send_number(x);
						//send('Y');
						//send_number(y);
						C1[2][i]=x/10;
						C1[2][i+1]=y/10;
						//send_number((x/10)+1);
						//send_number((y/10)+1);

}

void send_result()
{
		//send('Z');
		for(i=0;i<6;i++)
		{
			send_number(C1[0][i]);
		}
		//send('Z');
		for(i=0;i<6;i++)
		{
			send_number(C1[1][i]);
		}
		//send('Z');
		for(i=0;i<6;i++)
		{
			send_number(C1[2][i]);
		}
}
void send(unsigned char c)
{
	sendbyte=c;

	flag = 1;
	TI=1;
	

	_delay_ms(120);
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
		//temp = ((w*(24.0/11.0592))/147.0)*8.0/15.0;
				
		n = (int) (((w*(24.0/11.0592))/147.0)*8.0/15.0);
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
			send(' ');
			TR1=0;
			_delay_ms(50);
}
