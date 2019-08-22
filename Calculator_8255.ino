#define allOUT 0xff 
#define allINPUT 0x00
#define partial_OUT_IN 0xf0 
#define LcdClear 0x01

char MyKeypad[4][4]={{'7','4','1','N'},
          {'/','*','-','+'},
          {'9','6','3','='},
          {'8','5','2','0'}};

void init1 ()
{
   // Write control word iside control Register
    DDRD = 0xff;
    DDRB= 0xff;
    DDRC= 0xff; // address A0 , 
    DDRD = 0xff; // all output 
    PORTC = 0X03; // address of control register 
    PORTD = 0X80;// control word 
    PORTB = 0xff ; // idle RD , WR = HIGH
    PORTB = ~0x04; // write if read 0x08 " read , write >> active low"
    delay (1);
    PORTB = 0xff ; // idle     
 }

 void writeOnPort (  uint8_t portMode ,uint8_t address  , uint8_t data)
{
    DDRD = portMode; // all output 
    PORTC = address ; // 0X00; // address of portA a0=0, a1 =0
    PORTD = data ; //0X09;  or 0x00// data  
    PORTB = 0xff ; // idle RD , WR = HIGH
    PORTB = ~0x04; // write if read 0x08 " read , write >> active low"
    delay (1);
    PORTB = 0xff ; // idle 
   // delay (1000);   
}
byte readfromLogicState (uint8_t portMode, uint8_t address )
{
    DDRD = portMode; // pinMode(portD , Input ) 0 > 7 pins // el port ele mtwsl
    PORTC = address ; // 0X01; // address of port b
    PORTB = 0xff ;
    PORTB = ~0x08; // read 0x08  activelow
    delay (1);
    byte data = PIND & 1 ; // data
    PORTB = 0xff ; // idle
    
    return data;
}
byte readFromPort (uint8_t portMode, uint8_t address )
{
    DDRD = portMode; // pinMode(portD , Input ) 0 > 7 pins // el port ele mtwsl
    PORTC = address ; // 0X01; // address of port b
    PORTB = 0xff ;
    PORTB = ~0x08; // read 0x08  activelow
    delay (1);    
    byte data = PIND  ; // data
    PORTB = 0xff ; // idle
    
    return data;
}

char get_key ()
{
  uint8_t Col_val_out[4] ={0X8F,0X4F,0X2F,0X1F};
  while (1)
  {
    for (uint8_t i=0;i<4;i++)
    {
      uint8_t tmp;
      writeOnPort(partial_OUT_IN,0x02 ,Col_val_out[i]);
      tmp = readFromPort(allINPUT , 0x02);
      tmp &= 0X0F;
      switch (tmp) {
        case 0X0E : _delay_ms(300) ; return MyKeypad[i][0]; break;
        case 0X0D : _delay_ms(300) ; return MyKeypad[i][1]; break;
        case 0X0B : _delay_ms(300) ; return MyKeypad[i][2]; break;
        case 0X07 : _delay_ms(300) ; return MyKeypad[i][3]; break;
      }         
    }   
  }
}
void SEND_TO_LCD( uint8_t dataPort,  uint8_t controlPort, char data , uint8_t mode )
{
  writeOnPort(allOUT, dataPort,data);
  
  uint8_t lcd_Control = 0 ; // data or CMD 
  lcd_Control = mode ;      //CMD => RS=0 ,DATA =>RS =1 
  lcd_Control |= 2;       // EN = 1
  writeOnPort(allOUT, controlPort , lcd_Control);
  delay(1);
  lcd_Control &= ~2 ;       // EN =0
  writeOnPort(allOUT, controlPort , lcd_Control);
}
void INIT_LCD()
{
  int CMD=0; //CMD => RS=0
  SEND_TO_LCD(0x00, 0x01 ,0x38,CMD); // set 16*2 >> 16 col , 2 rows for 8 bit data 
  SEND_TO_LCD(0x00, 0x01 ,0x01,CMD); // clear
  SEND_TO_LCD(0x00, 0x01 ,0x0E,CMD) ; // display on 
}

/*void print_to_LCD(char *str)
{
  while(*str != '\0')
  {
    SEND_TO_LCD( 0x00,0x01,*str , 1);
    str++;
  }
}*/
void print_to_LCD(String str)
{
  int i=0;
  while(str[i] != '\0')
  {
    SEND_TO_LCD( 0x00,0x01,str[i] , 1);
    i++;
  }
}
/*int main()
{
  init1();
  INIT_LCD();
  uint8_t startFirstLine = 0x80 +4 ;
  SEND_TO_LCD (0x00, 0x01 , startFirstLine , 0);  //0>>cmd  0x00 >> porta , 0x01>> portB
  print_to_LCD("Wellcome");
  delay (1000);
  
  while (1)
  {
    char KEY =' ';
    KEY = get_key(); 
    if (KEY != 'N')
      {
        SEND_TO_LCD (0x00, 0x01 , 0x08+2 , 0); 
        print_to_LCD("Ending the calculator ");
        break;
      }
      SEND_TO_LCD (0x00, 0x01 , KEY , 1);
  }
  return 0;
}
*/
int calculations (char KEY ,int val1 , int val2)
{
  switch (KEY)
  {
    case '+' : return val1 + val2; break;
    case '-' : return val1 - val2;; break;
    case '*' : return val1 * val2;; break;
    case '/' : return val1 / val2;; break;
    //case '=' : return Arr[index] + Arr[index +1]; break;
  }
}
int calc (int* valuesArr , char* operationsArr ,int* Result ,int index1,int index2 )
{
  int res=0;
  for (int i=0 ; i< index2 ; i++)
  {
    res =calculations(operationsArr[i] ,valuesArr[i], valuesArr[i+1]);
    Result[i]= res;
  }
  return res;
}
//bool Calc_Available = 1;
int* valuesArr ;
char* operationsArr;
int* Result ;
int index1 =0 ,index2 =0 ;

void setup() {
  // put your setup code here, to run once:
  //serial2.begin(9600);
  init1();
  INIT_LCD();
  uint8_t startFirstLine = 0x80 +4 ;
  SEND_TO_LCD (0x00, 0x01 , startFirstLine , 0);  //0>>cmd  0x00 >> porta , 0x01>> portB
  print_to_LCD("Wellcome");
  delay (1000);
  SEND_TO_LCD (0x00, 0x01 , LcdClear , 0); // clear lcd 0x01
  valuesArr = new int [100];
  operationsArr = new char [100];
  Result = new int [100];
}
/*char* convert_tochar (int number)
{
   stringstream strs;
  strs << number;
  string temp_str = strs.str();
  char* char_type = (char*) temp_str.c_str();
  return char_type;
}*/
int firstEntry =1;
void loop() {
  // put your main code here, to run repeatedly:
         
       //char* resstr;
    //itoa(number ,resstr , 10 );
   /*String resstr= String(number);
   print_to_LCD2(resstr);*/
  char KEY =' ';
   KEY = get_key(); 
   //serial2.println(KEY);

  if (KEY != 'N')
  {

    SEND_TO_LCD (0x00, 0x01 , KEY , 1);
    if (KEY == '=')
    {
      int res =calc(valuesArr , operationsArr ,Result , index1 , index2);
      String resstr= String(res);
      print_to_LCD(resstr);
    }
    else if (KEY == '+' || KEY == '-' || KEY == '*' || KEY == '/')
      {
        operationsArr[index2] = KEY;
        index2 ++;
        firstEntry =1;
      }
    else if (firstEntry ==1)
      {
        valuesArr [index1] = KEY - '0';
        index1 ++ ; 
        firstEntry ++;
      }
      else if (firstEntry >1)
      {
        int val = KEY -'0';
        valuesArr [index1 -1]= valuesArr [index1 -1] *10 +val;
      }
  }
  else
  {
    SEND_TO_LCD (0x00, 0x01 , LcdClear , 0);
    valuesArr = new int [100];
    operationsArr = new char [100];
    Result = new int [100];
    index1=index2=0;
    firstEntry =1;
    //uint8_t startLine = 0x80 +1;
    //SEND_TO_LCD (0x00, 0x01 , startLine , 0); 
    //print_to_LCD("Calculator End");
    //Calc_Available = 0;
    //writeOnPort (allOUT,0x00 , 0x00);
  }
}

