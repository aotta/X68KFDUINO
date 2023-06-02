/*
 * X68KFDINO - External FD adapter for Sharp X68000
 * by Andrea Ottaviani june 2023
 * 
 * 
Access/Write strobe :

/OPTION_SEL_X : "Chip select" for the extra functions of drive DSX (X: 0-3)

Input / functions signals :

/EJECT_FUNC : If this signal is low when OPTION_SEL_X is asserted, the disk will be ejected.
/LOCK_FUNC (Eject MSK?): If this signal is low when OPTION_SEL_X is asserted, the disk will be locked (eject button off). If this signal is high when OPTION_SEL_X is asserted the disk will be unlocked (eject button on).
/BLINK_FUNC : If this signal is low when OPTION_SEL_X is asserted, the LED blink. If this signal is high when OPTION_SEL_X is asserted, the LED stop blinking.

Output signals (drive status):

/DISK_IN_DRIVE : when OPTION_SEL_X is asserted : Low = a disk in the drive. High = No disk.
/INSERT_FAULT : when OPTION_SEL_X is asserted : Low = a disk insert failure occured . High = No problem. This status is cleared at the rising edge of OPTION_SEL_X.

Output signal (Interrupt line):

/INT : Interrupt line : This signal is activated at each disk status change (inserted/ejected). The interrupt is cleared at the rising edge of /OPTION_SEL_X. (Note : more than one drive can activate this line in the same time -> Softwares scan all disk drive at each /INT assertion).

Note 1: For proper operation all output should be able to drive 5/150=33mA since 150 ohms pull-up are used.
Note 2: The X68000 /OPTION_SEL_X assertion duration is ~2.5us per access. This can depend of the software, the CPU speed.
 */


//Cablage
#define OPT0                2  // Set D2 as OPT0 Read strobe
#define OPT1                3  // Set D3 as OPT1 Read strobe
#define OPT2                4  // Set D4 as OPT2 Read strobe
#define OPT3                5  // Set D5 as OPT3 Read strobe
#define Eject               6  // Set D6 as Eject Read strobe
#define Inserted            7 // Set D7 as Inserted Read strobe
#define FDDINT              8   // Set D8 as FD Interrupt Read strobe
#define BT_FD0              9  // Set D9 as Button FD0 Read strobe
#define BT_FD1             10  // Set D10 as Button FD1 Read strobe
#define LED_RED            11  // Set D11 as LED_RED output (RED LED)
#define LEDFD0            12  // Set D12 as LED FD0 output (Green LED)
#define LED_BLINK          A0  // Set A0 as LED (Blink) output
#define Error              A1  // Set A1 as Error Read strobe
#define EjectMSK           A2  // Set A2 as EjectMSK Read strobe
#define Motor              A3  // Set A3 as Motor Read strobe
#define LEDFD1             A4  // Set A4 as LED FD1 output (Green LED)


void setup() {
  // put your setup code here, to run once:
  // initialize digital pin 13 as an output.
  pinMode(OPT0, INPUT);
  pinMode(OPT1, INPUT);
  pinMode(OPT2, INPUT);
  pinMode(OPT3, INPUT);
  pinMode(Eject, INPUT);
  pinMode(BT_FD0, INPUT);
  pinMode(BT_FD1, INPUT);
  pinMode(LED_BLINK, INPUT); // Blink
  pinMode(EjectMSK, INPUT); // LOCK (Red led)
  pinMode(Motor, INPUT); // 
  pinMode(Inserted, OUTPUT);
  pinMode(FDDINT, OUTPUT);
  pinMode(LED_RED, OUTPUT); // used for LED LOCK, Red light
  pinMode(LEDFD0, OUTPUT);
  pinMode(Error, OUTPUT);
  pinMode(LEDFD1, OUTPUT);

  Serial.begin(115200);

}

void loop() {
  unsigned int LDF0=0; // 0=unlocked, 1=locked
  unsigned int LDF1=0; // 0=unlocked, 1=locked
  unsigned int DF0=1; // 0=eject, 1=insert, 2=lock, 3=blink
  unsigned int DF1=1; // 0=eject, 1=insert, 2=lock, 3=blink
  unsigned int bit0Eject=0;
  unsigned int bit0Msk=0;
  unsigned int bit0Blink=0;
  unsigned int bit1Eject=0;
  unsigned int bit1Msk=0;
  unsigned int bit1Blink=0;
  
  // put your main code here, to run repeatedly:

 while(1) {

   digitalWrite(FDDINT,LOW);
   digitalWrite(LEDFD0,LOW);
 
   if ((digitalRead(BT_FD0)==HIGH)&&(LDF0==0)) {
     bit0Blink=0;
   //  digitalWrite(FDDINT,LOW);
     if (DF0==1) {
        DF0=0;
      } else {
        DF0=1;
      }
    Serial.print("DF0:");Serial.println(DF0);
    delay(800);
    }

  if ((digitalRead(BT_FD1)==HIGH)&&(LDF1==0)) {
      bit1Blink=0;
   //   digitalWrite(FDDINT,LOW);
      if (DF1==1) {
        DF1=0;
      } else {
        DF1=1;
      }
    Serial.print("DF1:");Serial.println(DF1);
    delay(800);
    }

   if (digitalRead(OPT0)==LOW) {
     if (digitalRead(Eject)==LOW) {
       DF0=0;
       bit0Blink=0;
       Serial.print("Ej 0-");
    }
    
    if (digitalRead(EjectMSK)==LOW) {
      LDF0=1;
      Serial.println("MSK0");
      digitalWrite(LED_RED,HIGH);
    } else {
      digitalWrite(LED_RED,LOW);
    }
 
    if (digitalRead(LED_BLINK)==LOW) {
       bit0Blink=1;
       digitalWrite(LEDFD0,LOW); 
        delay(80);
   //    digitalWrite(LEDFD0,HIGH);
       Serial.println("Blk 0-"); 
    }

    
    if (DF0==0) {
       digitalWrite(FDDINT,LOW);
       digitalWrite(LEDFD0,LOW);
      } else {
       if (bit0Blink==0) {
         digitalWrite(Inserted,LOW);
       //  Serial.print("df0-IN");
         digitalWrite(Error,HIGH);
         digitalWrite(FDDINT,HIGH);
         digitalWrite(LEDFD0,HIGH);
          } else {
         //digitalWrite(Inserted,LOW);
         //digitalWrite(Error,HIGH);
          digitalWrite(FDDINT,LOW);
          digitalWrite(LEDFD0,HIGH); 
         }
      }
  }  // opt 0

 if (digitalRead(OPT1)==LOW) {
  if (digitalRead(Eject)==LOW) {
      DF1=0;
      bit1Blink=0;
      Serial.print("Ej 1-");
    }
    
  if (digitalRead(EjectMSK)==LOW) {
      LDF1=1;
      Serial.println("MSK1");
    digitalWrite(LED_RED,HIGH);
  } else {
      digitalWrite(LED_RED,LOW);
    }
    
  if (digitalRead(LED_BLINK)==LOW) {
       bit1Blink=1;
       digitalWrite(LEDFD1,LOW); 
     delay(80);
      // digitalWrite(LEDFD1,HIGH); 
      Serial.print("Blk 1-");
       }
    
       if (DF1==0) {
       digitalWrite(FDDINT,LOW);
       digitalWrite(LEDFD1,LOW);
      } else {
       if (bit1Blink==0) {
         digitalWrite(Inserted,LOW);
         //Serial.print("df1-IN");
         digitalWrite(Error,HIGH);
         digitalWrite(FDDINT,HIGH);
         digitalWrite(LEDFD1,HIGH);
          } else {
         //digitalWrite(Inserted,LOW);
         //digitalWrite(Error,HIGH);
         //digitalWrite(FDDINT,HIGH);
          digitalWrite(LEDFD1,HIGH); 
         }
      }
  
  } //opt1
  
 } //while
 
}
