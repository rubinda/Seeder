/* Program, ki preverja pretok umetnega gnoja 
 v sejalnici OLT. Deluje preko dveh senzorjev
 in upozarja uporabnika, ko zazna zamaseno cev.

 @author David Rubin, 2019

 TODO: branje senzorjev
 TODO: zvok se naj sprozi glede na senzorja
 TODO: casovnik za silence (ce se slucajno ne ponastavi z odmasitvijo)
*/

// Pini za zvocnik in LEDice za levo in 
// desno stran. V kolikor je ok gori
// LED_OK, v primeru napake se sprozi
// zvocnik in gori LED_ERR.
#define SPEAKER       2
#define LEFT_LED_OK   3
#define LEFT_LED_ERR  4  
#define RIGHT_LED_OK  5
#define RIGHT_LED_ERR 6

// Gumb, ki ponastavi zvocnik (ga utisa
// do naslednje zamasitve)
#define RESET_SPEAKER A0

// Senzorja za pretok (inductive 
// capacitive sensor).
#define LEFT_SENSOR   A1
#define RIGHT_SENSOR  A2

// Kako dolgo naj drzi gumb za preglasitev (ms)
#define LONG_PRESS    1000

// Prebrani podatki za stanje senzorjev
// V kolikor > 0 pomeni da je cev zadelana
short leftState = 0;
short rightState = 0;

// Vrednosti za zvocnik, kdaj je zacel predvajat
// in ali je bil pritisnjen gumb za preglastiev
boolean silence = false;
unsigned long soundStart = 0;

// Casovnik za drzanje gumba in aktivnosti
// ali se gumb drzi ali ne
unsigned long buttonPressTime = 0;
boolean shortPressActive = false;
boolean longPressActive = false;

// Casovnik in stevec za predvajanje zvoka
// v intervalih po tri piske
boolean sound = false;
unsigned long soundTimer = 0;
short beepCount = 0;


// UpdateLEDs posodobi prizgane ledice glede
// na stanje senzorjev
void updateLEDs() {
  if (leftState > 0) {
    digitalWrite(LEFT_LED_OK, LOW);
    digitalWrite(LEFT_LED_ERR, HIGH);
  } else {
    digitalWrite(LEFT_LED_OK, HIGH);
    digitalWrite(LEFT_LED_ERR, LOW);
  }

  if (rightState > 0) {
    digitalWrite(RIGHT_LED_OK, LOW);
    digitalWrite(RIGHT_LED_ERR, HIGH);
  } else {
    digitalWrite(RIGHT_LED_OK, HIGH);
    digitalWrite(RIGHT_LED_ERR, LOW);
  }
}


// MakeSound sprozi priskanje zvocnika
void makeSound() {
  // Preveri ali je potrebna tisina (uporabnik s gumbom preglasil)
   if (!silence) {
    // Piski so v intervalih po 3
    if (beepCount < 3) {
      // Ce je zvok utisan in je preteklo 50ms zapiskaj
      if (!sound && millis() - soundTimer > 50) {
        sound = true;
        digitalWrite(SPEAKER, HIGH);
        soundTimer = millis();
      }
      // Ce je zvok prizgan in je preteklo 100ms ga ugasni
      if (sound && millis() - soundTimer > 100) {
        sound = false;
        // Povecaj stevilo piskov
        beepCount += 1;
        digitalWrite(SPEAKER, LOW);
        soundTimer = millis();
      }
    } else {
      // V kolikor so se izvedli 3 piski, utisaj piskanje
      // in ga znova dovoli po 750ms
      if (millis() - soundTimer > 750) {
        beepCount = 0;
      }
    }
  } else {
    // V kolikor je bil gumb drzan za LONG_PRESS ugasni piskanje
    digitalWrite(2, LOW);
    sound = false;
  }
}


// CheckSoundButton skrbi za stanje gumba za zvok.
// V kolikor je bil drzan za LONG_PRESS cas se 
// izklopi piskanje zvoka do odmasitve cevi.
void checkSoundButton() {
  if (digitalRead(RESET_SPEAKER)) {
    // Ce ga se ni prtisnal, zazeni timer za dolgo drzanje
    if (!shortPressActive) {
      shortPressActive = true;
      buttonPressTime = millis();
    }

    // Gumb je drzal dovolj dolgo casa, longPress je aktiven
    // in lahko utisamo zvocnik
    if ((millis() - buttonPressTime > LONG_PRESS) && !longPressActive) {
      longPressActive = true;
      silence = true;
    }
    
  } else {
    // Gumb je spustil in je samo kratko casa drzal
    if (shortPressActive) {
      if (longPressActive) {
        // Long press je bil aktiven, ponastavi
        longPressActive = false;
      }
      // ponastavi, ker ne drzi knofa vec
      shortPressActive = false;
    }
  }
}

void setup() {
  pinMode(SPEAKER, OUTPUT);
  pinMode(LEFT_LED_OK, OUTPUT);
  pinMode(LEFT_LED_ERR, OUTPUT);
  pinMode(RIGHT_LED_OK, OUTPUT);
  pinMode(RIGHT_LED_ERR, OUTPUT);
  
  pinMode(RESET_SPEAKER, INPUT);
  pinMode(LEFT_SENSOR, INPUT);
  // TODO SENSORS...
}



void loop() {
  // Preveri vrednosti senzorjev (ali je 
  // katera izmed cevi zadelana)
  leftState = analogRead(LEFT_SENSOR);

  // Posodobi lucke glede na stanje senzorjev
  updateLEDs();

  // Preveri ali je pritisnjen gumb za zvok
  checkSoundButton();

  // Sprozi alaram
  makeSound();
    
}
