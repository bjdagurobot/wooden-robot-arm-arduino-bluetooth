#include <Servo.h>
#include<EEPROM.h>
char receive[500] ;         //存储蓝牙模块接收的数据,由于连续动作可能需要很多字节
int p = 0;                  //用于指向字符数组，指针
String str = "";
int servoState[4] = { 90,90,90,90};          //暂时保存舵机的动作，因为目标只有4个舵机
int moveNum;
int num = 0;
//-------------------------
//4个舵机定义：
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

int servoPin1 = A0;  
int servoPin2 = A1;  
int servoPin3 = A2;  
int servoPin4 = A3; 
//--------------------------------------------

void setup() {
Serial.begin(9600);  //暂时用于电脑串口通讯,调试程序用
servo1.attach(servoPin1);
servo2.attach(servoPin2);
servo3.attach(servoPin3);
servo4.attach(servoPin4);

for(int i=0;i<50;i++)
servo1.write(90);
for(int i=0;i<50;i++)
servo2.write(90);
for(int i=0;i<50;i++)
servo3.write(90);
for(int i=0;i<50;i++)
servo4.write(90);
}


void loop() {
 if(Serial.available()>0)                     //开始接收：有接收数据
    { 
      receive[p] = Serial.read();              
      str += (char) receive[p];            
      p++;                                 
      delay(10);                           
    }   
 if (Serial.available()==0 && str !="")        //接收完成：无接收数据并且字符串不为空。正式进入
   {
       //Serial.println(str);
       //Serial.println((int)receive[1]);
       str = "";                           
       p = 0;                              
       if(receive[0] == '#')               
       {  
         //Serial.println("# yes");
          int servoNum = (int)receive[1]-48; //创建pinNum保存舵机号
          int data;                        //创建data保存舵机参数
          if(receive[3] == '1' || receive[3] == '2')   //判断舵机参数是4位数，500~2500
          {
            data = ((int)receive[3] -48 )*1000 + ((int)receive[4] -48 ) *100 +((int)receive[5] -48 ) *10 +((int)receive[6] -48);
          }
          else                                       //如果是3位数
          {
            data = ((int)receive[3] -48 )*100 + ((int)receive[4] -48 ) *10 +((int)receive[5] -48 );
          }
          data = (data - 500)/11;                   //舵机参数转化为角度，方便servo函数使用
          //Serial.println(data);
          servoState[servoNum - 1] = data;          //将转化好的参数写入对应舵机的数组内
          if(servoNum == 1)                         //相应舵机运动
          {
            for(int i=0;i<50;i++)
            servo1.write(data);
          }
          else if(servoNum == 2)
          {
            for(int i=0;i<50;i++)
            servo2.write(data);
          }
           else if(servoNum == 3)
          {
            for(int i=0;i<50;i++)
            servo3.write(data);
          }
           else if(servoNum == 4)
          {
            for(int i=0;i<50;i++)
            servo4.write(data);
          }
       }
       else if(receive[0] == 'G')               //进入动作组模式
       {   
           if((int)receive[1] == 13)            //空动作
           {
           }
           else if((int)receive[2] == 13 || (int)receive[3] == 13)       //动作保存入EEPROM
           {
              if((int)receive[2] == 13)
              moveNum = (int)receive[1] - 48;
              if((int)receive[3] == 13)
              moveNum = 10 + (int)receive[2] - 48;
              for(int i = 0; i < 4; i++)
              EEPROM.write(moveNum * 4 + i,servoState[i]);             //动作保存入EEPROM 
              //Serial.println(EEPROM.read(moveNum * 4 +0));
             // Serial.println(EEPROM.read(moveNum * 4 +1));
             // Serial.println(EEPROM.read(moveNum * 4 +2));
              //Serial.println(EEPROM.read(moveNum * 4 +3));
           }
          else if((int)receive[1] == 45)            //进入连续动作，“-”减号作为每个动作的开始
          {
          //Serial.println("move yes");
          p = 2;                             //指针
          while((int)receive[p] != 13)       //进入循环，指针逐个元素访问，直到结束符
          {
              if(receive[p] == '-')
              {
                  if(num == 1)
                  moveNum = (int)receive[p - 1] - 48 ;                      
                  else if (num == 2)
                  moveNum = 10 + (int)receive[p - 1] - 48;
                  num = 0;   
                  for(int i=0;i<50;i++)
                  {
                  servo1.write(EEPROM.read(moveNum * 4 ));
                  servo2.write(EEPROM.read(moveNum * 4 +1));
                  servo3.write(EEPROM.read(moveNum * 4 +2));
                  servo4.write(EEPROM.read(moveNum * 4 +3));
                  } 
                  delay(500);                            
               }
               else
               {
                  num++;            //不是“-”，则加1，代表动作组号码的位数，一位还是两位
               }
               p++;                 //指针加1，指向数组下一个元素
           }
           if(num == 1)
           {
                moveNum = (int)receive[p - 1] - 48 ;
            }
            else if (num == 2)
           {
                moveNum = 10 + (int)receive[p - 1] - 48;
            }
            num = 0;
           for(int i=0;i<50;i++)
           {
            servo1.write(EEPROM.read(moveNum * 4 ));
            servo2.write(EEPROM.read(moveNum * 4 +1));
            servo3.write(EEPROM.read(moveNum * 4 +2));
            servo4.write(EEPROM.read(moveNum * 4 +3));
           } 
           delay(500);   
           p = 0;    //指针初始化
        }         
       }
   }
}
