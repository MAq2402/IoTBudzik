Keypad/                                                                                             000755  001750  001750  00000000000 12546456104 013242  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/Keypad.cpp                                                                                   000644  001750  001750  00000022211 12546456104 015161  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /*
||
|| @file Keypad.cpp
|| @version 3.1
|| @author Mark Stanley, Alexander Brevig
|| @contact mstanley@technologist.com, alexanderbrevig@gmail.com
||
|| @description
|| | This library provides a simple interface for using matrix
|| | keypads. It supports multiple keypresses while maintaining
|| | backwards compatibility with the old single key library.
|| | It also supports user selectable pins and definable keymaps.
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/
#include <Keypad.h>

// <<constructor>> Allows custom keymap, pin configuration, and keypad sizes.
Keypad::Keypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols) {
	rowPins = row;
	columnPins = col;
	sizeKpd.rows = numRows;
	sizeKpd.columns = numCols;

	begin(userKeymap);

	setDebounceTime(10);
	setHoldTime(500);
	keypadEventListener = 0;

	startTime = 0;
	single_key = false;
}

// Let the user define a keymap - assume the same row/column count as defined in constructor
void Keypad::begin(char *userKeymap) {
    keymap = userKeymap;
}

// Returns a single key only. Retained for backwards compatibility.
char Keypad::getKey() {
	single_key = true;

	if (getKeys() && key[0].stateChanged && (key[0].kstate==PRESSED))
		return key[0].kchar;
	
	single_key = false;

	return NO_KEY;
}

// Populate the key list.
bool Keypad::getKeys() {
	bool keyActivity = false;

	// Limit how often the keypad is scanned. This makes the loop() run 10 times as fast.
	if ( (millis()-startTime)>debounceTime ) {
		scanKeys();
		keyActivity = updateList();
		startTime = millis();
	}

	return keyActivity;
}

// Private : Hardware scan
void Keypad::scanKeys() {
	// Re-intialize the row pins. Allows sharing these pins with other hardware.
	for (byte r=0; r<sizeKpd.rows; r++) {
		pin_mode(rowPins[r],INPUT_PULLUP);
	}

	// bitMap stores ALL the keys that are being pressed.
	for (byte c=0; c<sizeKpd.columns; c++) {
		pin_mode(columnPins[c],OUTPUT);
		pin_write(columnPins[c], LOW);	// Begin column pulse output.
		for (byte r=0; r<sizeKpd.rows; r++) {
			bitWrite(bitMap[r], c, !pin_read(rowPins[r]));  // keypress is active low so invert to high.
		}
		// Set pin to high impedance input. Effectively ends column pulse.
		pin_write(columnPins[c],HIGH);
		pin_mode(columnPins[c],INPUT);
	}
}

// Manage the list without rearranging the keys. Returns true if any keys on the list changed state.
bool Keypad::updateList() {

	bool anyActivity = false;

	// Delete any IDLE keys
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kstate==IDLE) {
			key[i].kchar = NO_KEY;
			key[i].kcode = -1;
			key[i].stateChanged = false;
		}
	}

	// Add new keys to empty slots in the key list.
	for (byte r=0; r<sizeKpd.rows; r++) {
		for (byte c=0; c<sizeKpd.columns; c++) {
			boolean button = bitRead(bitMap[r],c);
			char keyChar = keymap[r * sizeKpd.columns + c];
			int keyCode = r * sizeKpd.columns + c;
			int idx = findInList (keyCode);
			// Key is already on the list so set its next state.
			if (idx > -1)	{
				nextKeyState(idx, button);
			}
			// Key is NOT on the list so add it.
			if ((idx == -1) && button) {
				for (byte i=0; i<LIST_MAX; i++) {
					if (key[i].kchar==NO_KEY) {		// Find an empty slot or don't add key to list.
						key[i].kchar = keyChar;
						key[i].kcode = keyCode;
						key[i].kstate = IDLE;		// Keys NOT on the list have an initial state of IDLE.
						nextKeyState (i, button);
						break;	// Don't fill all the empty slots with the same key.
					}
				}
			}
		}
	}

	// Report if the user changed the state of any key.
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].stateChanged) anyActivity = true;
	}

	return anyActivity;
}

// Private
// This function is a state machine but is also used for debouncing the keys.
void Keypad::nextKeyState(byte idx, boolean button) {
	key[idx].stateChanged = false;

	switch (key[idx].kstate) {
		case IDLE:
			if (button==CLOSED) {
				transitionTo (idx, PRESSED);
				holdTimer = millis(); }		// Get ready for next HOLD state.
			break;
		case PRESSED:
			if ((millis()-holdTimer)>holdTime)	// Waiting for a key HOLD...
				transitionTo (idx, HOLD);
			else if (button==OPEN)				// or for a key to be RELEASED.
				transitionTo (idx, RELEASED);
			break;
		case HOLD:
			if (button==OPEN)
				transitionTo (idx, RELEASED);
			break;
		case RELEASED:
			transitionTo (idx, IDLE);
			break;
	}
}

// New in 2.1
bool Keypad::isPressed(char keyChar) {
	for (byte i=0; i<LIST_MAX; i++) {
		if ( key[i].kchar == keyChar ) {
			if ( (key[i].kstate == PRESSED) && key[i].stateChanged )
				return true;
		}
	}
	return false;	// Not pressed.
}

// Search by character for a key in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int Keypad::findInList (char keyChar) {
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kchar == keyChar) {
			return i;
		}
	}
	return -1;
}

// Search by code for a key in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int Keypad::findInList (int keyCode) {
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kcode == keyCode) {
			return i;
		}
	}
	return -1;
}

// New in 2.0
char Keypad::waitForKey() {
	char waitKey = NO_KEY;
	while( (waitKey = getKey()) == NO_KEY );	// Block everything while waiting for a keypress.
	return waitKey;
}

// Backwards compatibility function.
KeyState Keypad::getState() {
	return key[0].kstate;
}

// The end user can test for any changes in state before deciding
// if any variables, etc. needs to be updated in their code.
bool Keypad::keyStateChanged() {
	return key[0].stateChanged;
}

// The number of keys on the key list, key[LIST_MAX], equals the number
// of bytes in the key list divided by the number of bytes in a Key object.
byte Keypad::numKeys() {
	return sizeof(key)/sizeof(Key);
}

// Minimum debounceTime is 1 mS. Any lower *will* slow down the loop().
void Keypad::setDebounceTime(uint debounce) {
	debounce<1 ? debounceTime=1 : debounceTime=debounce;
}

void Keypad::setHoldTime(uint hold) {
    holdTime = hold;
}

void Keypad::addEventListener(void (*listener)(char)){
	keypadEventListener = listener;
}

void Keypad::transitionTo(byte idx, KeyState nextState) {
	key[idx].kstate = nextState;
	key[idx].stateChanged = true;

	// Sketch used the getKey() function.
	// Calls keypadEventListener only when the first key in slot 0 changes state.
	if (single_key)  {
	  	if ( (keypadEventListener!=NULL) && (idx==0) )  {
			keypadEventListener(key[0].kchar);
		}
	}
	// Sketch used the getKeys() function.
	// Calls keypadEventListener on any key that changes state.
	else {
	  	if (keypadEventListener!=NULL)  {
			keypadEventListener(key[idx].kchar);
		}
	}
}

/*
|| @changelog
|| | 3.1 2013-01-15 - Mark Stanley     : Fixed missing RELEASED & IDLE status when using a single key.
|| | 3.0 2012-07-12 - Mark Stanley     : Made library multi-keypress by default. (Backwards compatible)
|| | 3.0 2012-07-12 - Mark Stanley     : Modified pin functions to support Keypad_I2C
|| | 3.0 2012-07-12 - Stanley & Young  : Removed static variables. Fix for multiple keypad objects.
|| | 3.0 2012-07-12 - Mark Stanley     : Fixed bug that caused shorted pins when pressing multiple keys.
|| | 2.0 2011-12-29 - Mark Stanley     : Added waitForKey().
|| | 2.0 2011-12-23 - Mark Stanley     : Added the public function keyStateChanged().
|| | 2.0 2011-12-23 - Mark Stanley     : Added the private function scanKeys().
|| | 2.0 2011-12-23 - Mark Stanley     : Moved the Finite State Machine into the function getKeyState().
|| | 2.0 2011-12-23 - Mark Stanley     : Removed the member variable lastUdate. Not needed after rewrite.
|| | 1.8 2011-11-21 - Mark Stanley     : Added decision logic to compile WProgram.h or Arduino.h
|| | 1.8 2009-07-08 - Alexander Brevig : No longer uses arrays
|| | 1.7 2009-06-18 - Alexander Brevig : Every time a state changes the keypadEventListener will trigger, if set.
|| | 1.7 2009-06-18 - Alexander Brevig : Added setDebounceTime. setHoldTime specifies the amount of
|| |                                          microseconds before a HOLD state triggers
|| | 1.7 2009-06-18 - Alexander Brevig : Added transitionTo
|| | 1.6 2009-06-15 - Alexander Brevig : Added getState() and state variable
|| | 1.5 2009-05-19 - Alexander Brevig : Added setHoldTime()
|| | 1.4 2009-05-15 - Alexander Brevig : Added addEventListener
|| | 1.3 2009-05-12 - Alexander Brevig : Added lastUdate, in order to do simple debouncing
|| | 1.2 2009-05-09 - Alexander Brevig : Changed getKey()
|| | 1.1 2009-04-28 - Alexander Brevig : Modified API, and made variables private
|| | 1.0 2007-XX-XX - Mark Stanley : Initial Release
|| #
*/
                                                                                                                                                                                                                                                                                                                                                                                       Keypad/examples/                                                                                    000755  001750  001750  00000000000 12546456104 015060  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/EventKeypad/                                                                        000755  001750  001750  00000000000 12546456104 017277  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/EventKeypad/EventKeypad.ino                                                         000644  001750  001750  00000003737 12546456104 022237  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /* @file EventSerialKeypad.pde
 || @version 1.0
 || @author Alexander Brevig
 || @contact alexanderbrevig@gmail.com
 ||
 || @description
 || | Demonstrates using the KeypadEvent.
 || #
 */
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
byte ledPin = 13; 

boolean blink = false;
boolean ledPin_state;

void setup(){
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);              // Sets the digital pin as output.
    digitalWrite(ledPin, HIGH);           // Turn the LED on.
    ledPin_state = digitalRead(ledPin);   // Store initial LED state. HIGH when LED is on.
    keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
}

void loop(){
    char key = keypad.getKey();

    if (key) {
        Serial.println(key);
    }
    if (blink){
        digitalWrite(ledPin,!digitalRead(ledPin));    // Change the ledPin from Hi2Lo or Lo2Hi.
        delay(100);
    }
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        if (key == '#') {
            digitalWrite(ledPin,!digitalRead(ledPin));
            ledPin_state = digitalRead(ledPin);        // Remember LED state, lit or unlit.
        }
        break;

    case RELEASED:
        if (key == '*') {
            digitalWrite(ledPin,ledPin_state);    // Restore LED state from before it started blinking.
            blink = false;
        }
        break;

    case HOLD:
        if (key == '*') {
            blink = true;    // Blink the LED when holding the * key.
        }
        break;
    }
}
                                 Keypad/examples/DynamicKeypad/                                                                      000755  001750  001750  00000000000 12546456104 017602  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/DynamicKeypad/DynamicKeypad.ino                                                     000644  001750  001750  00000022017 12546456104 023035  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /* @file   DynamicKeypad.pde
|| @version 1.2
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
||  07/11/12 - Re-modified (from DynamicKeypadJoe2) to use direct-connect kpds
||  02/28/12 - Modified to use I2C i/o G. D. (Joe) Young
||
||
|| @dificulty:  Intermediate
||
|| @description
|| |    This is a demonstration of keypadEvents. It's used to switch between keymaps
|| |    while using only one keypad.  The main concepts being demonstrated are:
|| |
|| |        Using the keypad events, PRESSED, HOLD and RELEASED to simplify coding.
|| |        How to use setHoldTime() and why.
|| |        Making more than one thing happen with the same key.
|| |        Assigning and changing keymaps on the fly.
|| |
|| |    Another useful feature is also included with this demonstration although
|| |    it's not really one of the concepts that I wanted to show you.  If you look
|| |    at the code in the PRESSED event you will see that the first section of that
|| |    code is used to scroll through three different letters on each key.  For
|| |    example, pressing the '2' key will step through the letters 'd', 'e' and 'f'.
|| |
|| |
|| |  Using the keypad events, PRESSED, HOLD and RELEASED to simplify coding
|| |    Very simply, the PRESSED event occurs imediately upon detecting a pressed
|| |    key and will not happen again until after a RELEASED event.  When the HOLD
|| |    event fires it always falls between PRESSED and RELEASED.  However, it will
|| |    only occur if a key has been pressed for longer than the setHoldTime() interval.
|| |
|| |  How to use setHoldTime() and why
|| |    Take a look at keypad.setHoldTime(500) in the code.  It is used to set the
|| |    time delay between a PRESSED event and the start of a HOLD event.  The value
|| |    500 is in milliseconds (mS) and is equivalent to half a second.  After pressing
|| |    a key for 500mS the HOLD event will fire and any code contained therein will be
|| |    executed.  This event will stay active for as long as you hold the key except
|| |    in the case of bug #1 listed above.
|| |
|| |  Making more than one thing happen with the same key.
|| |    If you look under the PRESSED event (case PRESSED:) you will see that the '#'
|| |    is used to print a new line, Serial.println().  But take a look at the first
|| |    half of the HOLD event and you will see the same key being used to switch back
|| |    and forth between the letter and number keymaps that were created with alphaKeys[4][5]
|| |    and numberKeys[4][5] respectively.
|| |
|| |  Assigning and changing keymaps on the fly
|| |    You will see that the '#' key has been designated to perform two different functions
|| |    depending on how long you hold it down.  If you press the '#' key for less than the
|| |    setHoldTime() then it will print a new line.  However, if you hold if for longer
|| |    than that it will switch back and forth between numbers and letters.  You can see the
|| |    keymap changes in the HOLD event.
|| |
|| |
|| |  In addition...
|| |      You might notice a couple of things that you won't find in the Arduino language
|| |    reference.  The first would be #include <ctype.h>.  This is a standard library from
|| |    the C programming language and though I don't normally demonstrate these types of
|| |    things from outside the Arduino language reference I felt that its use here was
|| |    justified by the simplicity that it brings to this sketch.
|| |      That simplicity is provided by the two calls to isalpha(key) and isdigit(key).
|| |    The first one is used to decide if the key that was pressed is any letter from a-z
|| |    or A-Z and the second one decides if the key is any number from 0-9.  The return
|| |    value from these two functions is either a zero or some positive number greater
|| |    than zero.  This makes it very simple to test a key and see if it is a number or
|| |    a letter.  So when you see the following:
|| |
|| |    if (isalpha(key))    // this tests to see if your key was a letter
|| |
|| |    And the following may be more familiar to some but it is equivalent:
|| |
|| |    if (isalpha(key) != 0)   // this tests to see if your key was a letter
|| |
|| |  And Finally...
|| |    To better understand how the event handler affects your code you will need to remember
|| |    that it gets called only when you press, hold or release a key.  However, once a key
|| |    is pressed or held then the event handler gets called at the full speed of the loop().
|| |
|| #
*/
#include <Keypad.h>
#include <ctype.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
// Define the keymaps.  The blank spot (lower left) is the space character.
char alphaKeys[ROWS][COLS] = {
    { 'a','d','g' },
    { 'j','m','p' },
    { 's','v','y' },
    { ' ','.','#' }
};

char numberKeys[ROWS][COLS] = {
    { '1','2','3' },
    { '4','5','6' },
    { '7','8','9' },
    { ' ','0','#' }
};

boolean alpha = false;   // Start with the numeric keypad.

byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

// Create two new keypads, one is a number pad and the other is a letter pad.
Keypad numpad( makeKeymap(numberKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );
Keypad ltrpad( makeKeymap(alphaKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );


unsigned long startTime;
const byte ledPin = 13;                            // Use the LED on pin 13.

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);                 // Turns the LED on.
    ltrpad.begin( makeKeymap(alphaKeys) );
    numpad.begin( makeKeymap(numberKeys) );
    ltrpad.addEventListener(keypadEvent_ltr);  // Add an event listener.
    ltrpad.setHoldTime(500);                   // Default is 1000mS
    numpad.addEventListener(keypadEvent_num);  // Add an event listener.
    numpad.setHoldTime(500);                   // Default is 1000mS
}

char key;

void loop() {

    if( alpha )
        key = ltrpad.getKey( );
    else
        key = numpad.getKey( );

    if (alpha && millis()-startTime>100) {           // Flash the LED if we are using the letter keymap.
        digitalWrite(ledPin,!digitalRead(ledPin));
        startTime = millis();
    }
}

static char virtKey = NO_KEY;      // Stores the last virtual key press. (Alpha keys only)
static char physKey = NO_KEY;      // Stores the last physical key press. (Alpha keys only)
static char buildStr[12];
static byte buildCount;
static byte pressCount;

static byte kpadState;

// Take care of some special events.

void keypadEvent_ltr(KeypadEvent key) {
    // in here when in alpha mode.
    kpadState = ltrpad.getState( );
    swOnState( key );
} // end ltrs keypad events

void keypadEvent_num( KeypadEvent key ) {
    // in here when using number keypad
    kpadState = numpad.getState( );
    swOnState( key );
} // end numbers keypad events

void swOnState( char key ) {
    switch( kpadState ) {
        case PRESSED:
            if (isalpha(key)) {              // This is a letter key so we're using the letter keymap.
                if (physKey != key) {        // New key so start with the first of 3 characters.
                    pressCount = 0;
                    virtKey = key;
                    physKey = key;
                }
                else {                       // Pressed the same key again...
                    virtKey++;                   // so select the next character on that key.
                    pressCount++;                // Tracks how many times we press the same key.
                }
                    if (pressCount > 2) {    // Last character reached so cycle back to start.
                        pressCount = 0;
                        virtKey = key;
                    }
                    Serial.print(virtKey);   // Used for testing.
                }
                if (isdigit(key) || key == ' ' || key == '.')
                    Serial.print(key);
                if (key == '#')
                    Serial.println();
                break;

        case HOLD:
            if (key == '#')  {               // Toggle between keymaps.
                if (alpha == true)  {        // We are currently using a keymap with letters
                    alpha = false;           // Now we want a keymap with numbers.
                    digitalWrite(ledPin, LOW);
                }
                else  {                      // We are currently using a keymap with numbers
                    alpha = true;            // Now we want a keymap with letters.
                }
            }
            else  {                          // Some key other than '#' was pressed.
                buildStr[buildCount++] = (isalpha(key)) ? virtKey : key;
                buildStr[buildCount] = '\0';
                Serial.println();
                Serial.println(buildStr);
            }
            break;

        case RELEASED:
            if (buildCount >= sizeof(buildStr))  buildCount = 0;  // Our string is full. Start fresh.
            break;
    }  // end switch-case
}// end switch on state function

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 Keypad/examples/MultiKey/                                                                           000755  001750  001750  00000000000 12546456104 016623  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/MultiKey/MultiKey.ino                                                               000644  001750  001750  00000004070 12546456104 021076  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /* @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;


void setup() {
    Serial.begin(9600);
    loopCount = 0;
    startTime = millis();
    msg = "";
}


void loop() {
    loopCount++;
    if ( (millis()-startTime)>5000 ) {
        Serial.print("Average loops per second = ");
        Serial.println(loopCount/5);
        startTime = millis();
        loopCount = 0;
    }

    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                    msg = " PRESSED.";
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    msg = " RELEASED.";
                break;
                    case IDLE:
                    msg = " IDLE.";
                }
                Serial.print("Key ");
                Serial.print(kpd.key[i].kchar);
                Serial.println(msg);
            }
        }
    }
}  // End loop
                                                                                                                                                                                                                                                                                                                                                                                                                                                                        Keypad/examples/loopCounter/                                                                        000755  001750  001750  00000000000 12546456104 017371  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/loopCounter/loopCounter.ino                                                         000644  001750  001750  00000002460 12546456104 022413  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         #include <Keypad.h>


const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount = 0;
unsigned long timer_t = 0;

void setup(){
	Serial.begin(9600);
	
	// Try playing with different debounceTime settings to see how it affects
	// the number of times per second your loop will run. The library prevents
	// setting it to anything below 1 millisecond.
	kpd.setDebounceTime(10);	// setDebounceTime(mS)
}

void loop(){
	char key = kpd.getKey();
	
	// Report the number of times through the loop in 1 second. This will give
	// you a relative idea of just how much the debounceTime has changed the
	// speed of your code. If you set a high debounceTime your loopCount will
	// look good but your keypresses will start to feel sluggish.
	if ((millis() - timer_t) > 1000) {
		Serial.print("Your loop code ran ");
		Serial.print(loopCount);
		Serial.println(" times over the last second");
		loopCount = 0;
		timer_t = millis();
	}
	loopCount++;
	if(key)
		Serial.println(key);
}
                                                                                                                                                                                                                Keypad/examples/CustomKeypad/                                                                       000755  001750  001750  00000000000 12546456104 017470  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/CustomKeypad/CustomKeypad.ino                                                       000644  001750  001750  00000001643 12546456104 022613  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /* @file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
*/
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
  }
}
                                                                                             Keypad/examples/HelloKeypad3/                                                                       000755  001750  001750  00000000000 12546456104 017344  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/HelloKeypad3/HelloKeypad3.ino                                                       000644  001750  001750  00000003065 12546456104 022343  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         #include <Keypad.h>


const byte ROWS = 2; // use 4X4 keypad for both instances
const byte COLS = 2;
char keys[ROWS][COLS] = {
  {'1','2'},
  {'3','4'}
};
byte rowPins[ROWS] = {5, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6}; //connect to the column pinouts of the keypad
Keypad kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


const byte ROWSR = 2;
const byte COLSR = 2;
char keysR[ROWSR][COLSR] = {
  {'a','b'},
  {'c','d'}
};
byte rowPinsR[ROWSR] = {3, 2}; //connect to the row pinouts of the keypad
byte colPinsR[COLSR] = {7, 6}; //connect to the column pinouts of the keypad
Keypad kpdR( makeKeymap(keysR), rowPinsR, colPinsR, ROWSR, COLSR );


const byte ROWSUR = 4;
const byte COLSUR = 1;
char keysUR[ROWSUR][COLSUR] = {
  {'M'},
  {'A'},
  {'R'},
  {'K'}
};
// Digitran keypad, bit numbers of PCF8574 i/o port
byte rowPinsUR[ROWSUR] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPinsUR[COLSUR] = {8}; //connect to the column pinouts of the keypad

Keypad kpdUR( makeKeymap(keysUR), rowPinsUR, colPinsUR, ROWSUR, COLSUR );


void setup(){
//  Wire.begin( );
  kpdUR.begin( makeKeymap(keysUR) );
  kpdR.begin( makeKeymap(keysR) );
  kpd.begin( makeKeymap(keys) );
  Serial.begin(9600);
  Serial.println( "start" );
}

//byte alternate = false;
char key, keyR, keyUR;
void loop(){

//  alternate = !alternate;
  key = kpd.getKey( );
  keyUR = kpdUR.getKey( );
  keyR = kpdR.getKey( );

  if (key){
    Serial.println(key);
  }
  if( keyR ) {
    Serial.println( keyR );
  }
  if( keyUR ) {
    Serial.println( keyUR );
  }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                           Keypad/examples/HelloKeypad/                                                                        000755  001750  001750  00000000000 12546456104 017261  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/examples/HelloKeypad/HelloKeypad.ino                                                         000644  001750  001750  00000001417 12546456104 022174  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /* @file HelloKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates the simplest use of the matrix Keypad library.
|| #
*/
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
}
  
void loop(){
  char key = keypad.getKey();
  
  if (key){
    Serial.println(key);
  }
}
                                                                                                                                                                                                                                                 Keypad/keywords.txt                                                                                 000644  001750  001750  00000001335 12546456104 015654  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         # Keypad Library data types
KeyState	KEYWORD1
Keypad	KEYWORD1
KeypadEvent	KEYWORD1

# Keypad Library constants
NO_KEY	LITERAL1
IDLE	LITERAL1
PRESSED	LITERAL1
HOLD	LITERAL1
RELEASED	LITERAL1

# Keypad Library methods & functions
addEventListener	KEYWORD2
bitMap	KEYWORD2
findKeyInList	KEYWORD2
getKey	KEYWORD2
getKeys	KEYWORD2
getState	KEYWORD2
holdTimer	KEYWORD2
isPressed	KEYWORD2
keyStateChanged	KEYWORD2
numKeys	KEYWORD2
pin_mode	KEYWORD2
pin_write	KEYWORD2
pin_read	KEYWORD2
setDebounceTime	KEYWORD2
setHoldTime	KEYWORD2
waitForKey	KEYWORD2

# this is a macro that converts 2d arrays to pointers
makeKeymap	KEYWORD2

# List of objects created in the example sketches.
kpd	KEYWORD3
keypad	KEYWORD3
kbrd	KEYWORD3
keyboard	KEYWORD3
                                                                                                                                                                                                                                                                                                   Keypad/utility/                                                                                     000755  001750  001750  00000000000 12546456104 014745  5                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         Keypad/utility/Key.cpp                                                                              000644  001750  001750  00000002720 12546456104 016202  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /*
|| @file Key.cpp
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | Key class provides an abstract definition of a key or button
|| | and was initially designed to be used in conjunction with a
|| | state-machine.
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/
#include <Key.h>


// default constructor
Key::Key() {
	kchar = NO_KEY;
	kstate = IDLE;
	stateChanged = false;
}

// constructor
Key::Key(char userKeyChar) {
	kchar = userKeyChar;
	kcode = -1;
	kstate = IDLE;
	stateChanged = false;
}


void Key::key_update (char userKeyChar, KeyState userState, boolean userStatus) {
	kchar = userKeyChar;
	kstate = userState;
	stateChanged = userStatus;
}



/*
|| @changelog
|| | 1.0 2012-06-04 - Mark Stanley : Initial Release
|| #
*/
                                                Keypad/utility/Key.h                                                                                000644  001750  001750  00000003211 12546456104 015643  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /*
||
|| @file Key.h
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | Key class provides an abstract definition of a key or button
|| | and was initially designed to be used in conjunction with a
|| | state-machine.
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef KEY_H
#define KEY_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

#define OPEN LOW
#define CLOSED HIGH

typedef unsigned int uint;
typedef enum{ IDLE, PRESSED, HOLD, RELEASED } KeyState;

const char NO_KEY = '\0';

class Key {
public:
	// members
	char kchar;
	int kcode;
	KeyState kstate;
	boolean stateChanged;

	// methods
	Key();
	Key(char userKeyChar);
	void key_update(char userKeyChar, KeyState userState, boolean userStatus);

private:

};

#endif

/*
|| @changelog
|| | 1.0 2012-06-04 - Mark Stanley : Initial Release
|| #
*/
                                                                                                                                                                                                                                                                                                                                                                                       Keypad/Keypad.h                                                                                     000644  001750  001750  00000013153 12546456104 014633  0                                                                                                    ustar 00szymex                          szymex                          000000  000000                                                                                                                                                                         /*
||
|| @file Keypad.h
|| @version 3.1
|| @author Mark Stanley, Alexander Brevig
|| @contact mstanley@technologist.com, alexanderbrevig@gmail.com
||
|| @description
|| | This library provides a simple interface for using matrix
|| | keypads. It supports multiple keypresses while maintaining
|| | backwards compatibility with the old single key library.
|| | It also supports user selectable pins and definable keymaps.
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef KEYPAD_H
#define KEYPAD_H

#include "utility/Key.h"

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// bperrybap - Thanks for a well reasoned argument and the following macro(s).
// See http://arduino.cc/forum/index.php/topic,142041.msg1069480.html#msg1069480
#ifndef INPUT_PULLUP
#warning "Using  pinMode() INPUT_PULLUP AVR emulation"
#define INPUT_PULLUP 0x2
#define pinMode(_pin, _mode) _mypinMode(_pin, _mode)
#define _mypinMode(_pin, _mode)  \
do {							 \
	if(_mode == INPUT_PULLUP)	 \
		pinMode(_pin, INPUT);	 \
		digitalWrite(_pin, 1);	 \
	if(_mode != INPUT_PULLUP)	 \
		pinMode(_pin, _mode);	 \
}while(0)
#endif


#define OPEN LOW
#define CLOSED HIGH

typedef char KeypadEvent;
typedef unsigned int uint;
typedef unsigned long ulong;

// Made changes according to this post http://arduino.cc/forum/index.php?topic=58337.0
// by Nick Gammon. Thanks for the input Nick. It actually saved 78 bytes for me. :)
typedef struct {
    byte rows;
    byte columns;
} KeypadSize;

#define LIST_MAX 10		// Max number of keys on the active list.
#define MAPSIZE 10		// MAPSIZE is the number of rows (times 16 columns)
#define makeKeymap(x) ((char*)x)


//class Keypad : public Key, public HAL_obj {
class Keypad : public Key {
public:

	Keypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols);

	virtual void pin_mode(byte pinNum, byte mode) { pinMode(pinNum, mode); }
	virtual void pin_write(byte pinNum, boolean level) { digitalWrite(pinNum, level); }
	virtual int  pin_read(byte pinNum) { return digitalRead(pinNum); }

	uint bitMap[MAPSIZE];	// 10 row x 16 column array of bits. Except Due which has 32 columns.
	Key key[LIST_MAX];
	unsigned long holdTimer;

	char getKey();
	bool getKeys();
	KeyState getState();
	void begin(char *userKeymap);
	bool isPressed(char keyChar);
	void setDebounceTime(uint);
	void setHoldTime(uint);
	void addEventListener(void (*listener)(char));
	int findInList(char keyChar);
	int findInList(int keyCode);
	char waitForKey();
	bool keyStateChanged();
	byte numKeys();

private:
	unsigned long startTime;
	char *keymap;
    byte *rowPins;
    byte *columnPins;
	KeypadSize sizeKpd;
	uint debounceTime;
	uint holdTime;
	bool single_key;

	void scanKeys();
	bool updateList();
	void nextKeyState(byte n, boolean button);
	void transitionTo(byte n, KeyState nextState);
	void (*keypadEventListener)(char);
};

#endif

/*
|| @changelog
|| | 3.1 2013-01-15 - Mark Stanley     : Fixed missing RELEASED & IDLE status when using a single key.
|| | 3.0 2012-07-12 - Mark Stanley     : Made library multi-keypress by default. (Backwards compatible)
|| | 3.0 2012-07-12 - Mark Stanley     : Modified pin functions to support Keypad_I2C
|| | 3.0 2012-07-12 - Stanley & Young  : Removed static variables. Fix for multiple keypad objects.
|| | 3.0 2012-07-12 - Mark Stanley     : Fixed bug that caused shorted pins when pressing multiple keys.
|| | 2.0 2011-12-29 - Mark Stanley     : Added waitForKey().
|| | 2.0 2011-12-23 - Mark Stanley     : Added the public function keyStateChanged().
|| | 2.0 2011-12-23 - Mark Stanley     : Added the private function scanKeys().
|| | 2.0 2011-12-23 - Mark Stanley     : Moved the Finite State Machine into the function getKeyState().
|| | 2.0 2011-12-23 - Mark Stanley     : Removed the member variable lastUdate. Not needed after rewrite.
|| | 1.8 2011-11-21 - Mark Stanley     : Added test to determine which header file to compile,
|| |                                          WProgram.h or Arduino.h.
|| | 1.8 2009-07-08 - Alexander Brevig : No longer uses arrays
|| | 1.7 2009-06-18 - Alexander Brevig : This library is a Finite State Machine every time a state changes
|| |                                          the keypadEventListener will trigger, if set
|| | 1.7 2009-06-18 - Alexander Brevig : Added setDebounceTime setHoldTime specifies the amount of
|| |                                          microseconds before a HOLD state triggers
|| | 1.7 2009-06-18 - Alexander Brevig : Added transitionTo
|| | 1.6 2009-06-15 - Alexander Brevig : Added getState() and state variable
|| | 1.5 2009-05-19 - Alexander Brevig : Added setHoldTime()
|| | 1.4 2009-05-15 - Alexander Brevig : Added addEventListener
|| | 1.3 2009-05-12 - Alexander Brevig : Added lastUdate, in order to do simple debouncing
|| | 1.2 2009-05-09 - Alexander Brevig : Changed getKey()
|| | 1.1 2009-04-28 - Alexander Brevig : Modified API, and made variables private
|| | 1.0 2007-XX-XX - Mark Stanley : Initial Release
|| #
*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     