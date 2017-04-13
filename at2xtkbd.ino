/*
DIN connector pins:
1 clock     black
2 data      red
3 rst       yellow
4 gnd       brown
5 vcc (5V)  orange

Arduino UNO connections:

Source Keyboard (AT) DIN:
orange      5V
brown       GND
black       D3
red         D4

Destination Keyboard (XT) DIN:
orange      5V
brown       GND
black       D2
red         D5

Summary:
power:
  yellow: GND
  orange: 5V
atkbd:
  black: D3
  red: D4
xtkbd:
  black: D2
  red: D5
 */

// pins
#define AT_CLK          3     // must be interrupt pin
#define AT_CLK_INT      1     // interrupt #1 -> digital pin 3
#define AT_DATA         4
#define XT_CLK          2
#define XT_DATA         5

// states
#define RD_START           1
#define RD_DATA            2
#define RD_PARITY          3
#define RD_STOP            4

#define WR_START           5
#define WR_DATA            6
#define WR_PARITY          7
#define WR_WAIT            8
#define WR_ACK             9
#define WR_IGNORE          10


// keyboard state
#define KBD_START                   0
#define KBD_BRK                     0x80

#define KBD_EXT                     1
#define KBD_EXT_12                  2
#define KBD_EXT_12_EXT              3

#define KBD_EXT_BRK                 4
#define KBD_EXT_BRK_CODE            5
#define KBD_EXT_BRK_CODE_EXT        6
#define KBD_EXT_BRK_CODE_EXT_BRK    7

#define KBD_NON_EXT                 8
#define KBD_WAIT_EXT                9
#define KBD_PAUSE_BRK               10

#define KBD_SET_LED                 11
#define KBD_INIT                    12

// special scan codes
#define ACK             0xFA
#define BRK             0xF0
#define EXT             0xE0
#define EXT1            0xE1

// leds
#define LED_NUM     2
#define LED_CAPS    4
#define LED_SCROLL  1

// XT clock: 10 KHz (10*10^6)
// period = 1 / 10*10^6 = 0.1 * 10^-6 = 100 * 10^-9 = 100 microseconds
// (values in microseconds)
#define XT_PERIOD       100
#define XT_HCLK         (XT_PERIOD/2)
#define XT_QCLK         (XT_PERIOD/4)

#define DEBUG
// #define DEBUG_INTR

// debug print
inline
void dprint(const char *str)
{
#ifdef DEBUG
    Serial.print(str);
#endif
}

inline
void dprintln(const char *str)
{
#ifdef DEBUG
    Serial.println(str);
#endif
}

inline
void dprintx(byte value)
{
#ifdef DEBUG
    Serial.print(value, HEX);
#endif
}

inline
void dprintlnx(byte value)
{
#ifdef DEBUG
    Serial.println(value, HEX);
#endif
}

inline void dprintc(char c)
{
#ifdef DEBUG
    Serial.print(c);
#endif
}

// types

struct keydata
{
    byte ch;
    byte at_code;
    byte xt_code;
};

// global data

// keyboard keys table

struct keydata keytbl[] =
{
    // 0
    // alpha
    {'a', 0x1C, 0x1E},
    {'b', 0x32, 0x30},
    {'c', 0x21, 0x2E},
    {'d', 0x23, 0x20},
    {'e', 0x24, 0x12},
    {'f', 0x2B, 0x21},
    {'g', 0x34, 0x22},
    {'h', 0x33, 0x23},
    {'i', 0x43, 0x17},
    {'j', 0x3B, 0x24},
    {'k', 0x42, 0x25},
    {'l', 0x4B, 0x26},
    {'m', 0x3A, 0x32},
    {'n', 0x31, 0x31},
    {'o', 0x44, 0x18},
    {'p', 0x4D, 0x19},
    {'q', 0x15, 0x10},
    {'r', 0x2D, 0x13},
    {'s', 0x1B, 0x1F},
    {'t', 0x2C, 0x14},
    {'u', 0x3C, 0x16},
    {'v', 0x2A, 0x2F},
    {'w', 0x1D, 0x11},
    {'x', 0x22, 0x2D},
    {'y', 0x35, 0x15},
    {'z', 0x1A, 0x2C},
    // 26
    // num
    {'0', 0x45, 0x0B},
    {'1', 0x16, 0x02},
    {'2', 0x1E, 0x03},
    {'3', 0x26, 0x04},
    {'4', 0x25, 0x05},
    {'5', 0x2E, 0x06},
    {'6', 0x36, 0x07},
    {'7', 0x3D, 0x08},
    {'8', 0x3E, 0x09},
    {'9', 0x46, 0x0A},
    // 36
    // other
    {'`', 0x0E, 0x29},  // backtick
    {'-', 0x4E, 0x0C},  // minus
    {'=', 0x55, 0x0D},  // equal
    {'\\', 0x5D, 0x2B}, // backslash
    {'[', 0x54, 0x1A},  // open bracket
    {']', 0x5B, 0x1B},  // close bracket
    {';', 0x4C, 0x27},  // semicolon
    {'\'', 0x52, 0x28}, // single quote
    {',', 0x41, 0x33},  // comma
    {'.', 0x49, 0x34},  // period
    {'/', 0x4A, 0x35},  // slash
    // 47
    {' ', 0x29, 0x39},  // space
    {'B', 0x66, 0x0E},  // backsapce
    {'T', 0x0D, 0x0F},  // tab
    {'N', 0x5A, 0x1C},  // enter
    // 51
    // control
    {'^', 0x58, 0x3A},  // caps
    {'S', 0x12, 0x2A},  // left shift
    {'C', 0x14, 0x1D},  // left ctrl
    {'A', 0x11, 0x38},  // left alt
    {'R', 0x59, 0x36},  // right shift
    {'E', 0x76, 0x01},  // esc
    // 57
    // function keys
    {'F', 0x05, 0x3B},  // F1
    {'F', 0x06, 0x3C},  // F2
    {'F', 0x04, 0x3D},  // F3
    {'F', 0x0C, 0x3E},  // F4
    {'F', 0x03, 0x3F},  // F5
    {'F', 0x0B, 0x40},  // F6
    {'F', 0x83, 0x41},  // F7
    {'F', 0x0A, 0x42},  // F8
    {'F', 0x01, 0x43},  // F9
    {'F', 0x09, 0x44},  // F10
    {'F', 0x78, 0x57},  // F11
    {'F', 0x07, 0x58},  // F12
    // 69
    {'L', 0x7E, 0x46},  // SCROLL
    // 70
    // keypad
    {'N', 0x77, 0x45},  // Num Lock
    {'0', 0x70, 0x52},  // Keypad 0
    {'1', 0x69, 0x4F},  // Keypad 1
    {'2', 0x72, 0x50},  // Keypad 2
    {'3', 0x7A, 0x51},  // Keypad 3
    {'4', 0x6B, 0x4B},  // Keypad 4
    {'5', 0x73, 0x4C},  // Keypad 5
    {'6', 0x74, 0x4D},  // Keypad 6
    {'7', 0x6C, 0x47},  // Keypad 7
    {'8', 0x75, 0x48},  // Keypad 8
    {'9', 0x7D, 0x49},  // Keypad 9
    {'+', 0x79, 0x4E},  // Keypad +
    {'-', 0x7B, 0x4A},  // Keypad -
    {'*', 0x7C, 0x37},  // Keypad *
    {'.', 0x71, 0x53},  // Keypad .
    // 85

    // extended
    {'I', 0x70, 0x52},  // insert
    {'X', 0x71, 0x53},  // delete
    {'H', 0x6C, 0x47},  // home
    {'E', 0x69, 0x4F},  // end
    {'U', 0x7D, 0x49},  // page up
    {'D', 0x7A, 0x51},  // page down
    // 91
    {'C', 0x14, 0x1D},  // right ctrl
    {'A', 0x11, 0x38},  // right alt
    {'<', 0x6B, 0x4B},  // left
    {'>', 0x74, 0x4D},  // right
    {'^', 0x75, 0x48},  // up
    {'_', 0x72, 0x50},  // down
    {'P', 0x7C, 0x37},  // print screen
    {'/', 0x4A, 0x35},  // Keypad /
    {'N', 0x5A, 0x1C}   // Keypad Enter
    // 100
};

struct keydata kd_pause_brk = {'B', 0x77, 0x19 };

#define KEYTBL_SIZE        100
#define KEYTBL_EXT         85

/*
 * XT scan codes
 *
0x01    // esc    // 'E' 0x76
0x02    // '1' 0x16
0x03    // '2' 0x1E
0x04    // '3' 0x26
0x05    // '4' 0x25
0x06    // '5' 0x2E
0x07    // '6' 0x36
0x08    // '7' 0x3D
0x09    // '8' 0x3E
0x0A    // '9' 0x46
0x0B    // '0' 0x45
0x0C    // minus    // '-' 0x4E
0x0D    // equal    // '=' 0x55
0x0E    // backsapce    // 'B' 0x66
0x0F    // tab    // 'T' 0x0D
0x10    // 'q' 0x15
0x11    // 'w' 0x1D
0x12    // 'e' 0x24
0x13    // 'r' 0x2D
0x14    // 't' 0x2C
0x15    // 'y' 0x35
0x16    // 'u' 0x3C
0x17    // 'i' 0x43
0x18    // 'o' 0x44
0x19    // 'p' 0x4D
0x1A    // open bracket    // '[' 0x54
0x1B    // close bracket    // ']' 0x5B
0x1C    // enter    // 'N' 0x5A
0x1C    // Keypad Enter // 'N' 0x5A
0x1D    // left ctrl    // 'C' 0x14
0x1D    // right ctrl    // 'C' 0x14
0x1E    // 'a' 0x1C
0x1F    // 's' 0x1B
0x20    // 'd' 0x23
0x21    // 'f' 0x2B
0x22    // 'g' 0x34
0x23    // 'h' 0x33
0x24    // 'j' 0x3B
0x25    // 'k' 0x42
0x26    // 'l' 0x4B
0x27    // semicolon    // ';' 0x4C
0x28    // single quote    // '\'' 0x52
0x29    // backtick    // '`' 0x0E
0x2A    // left shift    // 'S' 0x12
0x2B    // backslash    // '\\' 0x5D
0x2C    // 'z' 0x1A
0x2D    // 'x' 0x22
0x2E    // 'c' 0x21
0x2F    // 'v' 0x2A
0x30    // 'b' 0x32
0x31    // 'n' 0x31
0x32    // 'm' 0x3A
0x33    // comma    // ',' 0x41
0x34    // period    // '.' 0x49
0x35    // Keypad /    // '/' 0x4A
0x35    // slash    // '/' 0x4A
0x36    // right shift    // 'R' 0x59
0x37    // Keypad *    // '*' 0x7C
0x37    // print screen    // 'P' 0x7C
0x38    // left alt    // 'A' 0x11
0x38    // right alt    // 'A' 0x11
0x39    // space    // ' ' 0x29
0x3A    // caps    // '^' 0x58
0x3B    // F1    // 'F' 0x05
0x3C    // F2    // 'F' 0x06
0x3D    // F3    // 'F' 0x04
0x3E    // F4    // 'F' 0x0C
0x3F    // F5    // 'F' 0x03
0x40    // F6    // 'F' 0x0B
0x41    // F7    // 'F' 0x83
0x42    // F8    // 'F' 0x0A
0x43    // F9    // 'F' 0x01
0x44    // F10    // 'F' 0x09
0x45    // Num Lock    // 'N' 0x77
0x46    // SCROLL    // 'L' 0x7E
0x47    // Keypad 7    // '7' 0x6C
0x47    // home    // 'H' 0x6C
0x48    // Keypad 8    // '8' 0x75
0x48    // up    // '^' 0x75
0x49    // Keypad 9    // '9' 0x7D
0x49    // page up    // 'U' 0x7D
0x4A    // Keypad -    // '-' 0x7B
0x4B    // Keypad 4    // '4' 0x6B
0x4B    // left    // '<' 0x6B
0x4C    // Keypad 5    // '5' 0x73
0x4D    // Keypad 6    // '6' 0x74
0x4D    // right    // '>' 0x74
0x4E    // Keypad +    // '+' 0x79
0x4F    // Keypad 1    // '1' 0x69
0x4F    // end    // 'E' 0x69
0x50    // Keypad 2    // '2' 0x72
0x50    // down    // '_' 0x72
0x51    // Keypad 3    // '3' 0x7A
0x51    // page down    // 'D' 0x7A
0x52    // Keypad 0    // '0' 0x70
0x52    // insert    // 'I' 0x70
0x53    // Keypad .    // '.' 0x71
0x53    // delete    // 'X' 0x71
0x57    // F11    // 'F' 0x78
0x58    // F12    // 'F' 0x07
 */




// keytbl ops

struct keydata *keytbl_find(byte at_code)
{
    byte i;
    for (i = 0; i < KEYTBL_EXT; i++) {
        if (keytbl[i].at_code == at_code)
            return &keytbl[i];
    }

    return NULL;
}

struct keydata *exttbl_find(byte at_code)
{
    byte i;
    for (i = KEYTBL_EXT; i < KEYTBL_SIZE; i++) {
        if (keytbl[i].at_code == at_code)
            return &keytbl[i];
    }

    return NULL;
}

// setup

void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
#endif
    pinMode(AT_CLK,  INPUT);
    pinMode(AT_DATA, INPUT);
    pinMode(XT_CLK,  OUTPUT);
    pinMode(XT_DATA, OUTPUT);
    attachInterrupt(AT_CLK_INT, at_clock_low, FALLING);
    dprintln("AT2XT KBD started!");
}

//
// serial reception state machine state
byte at_state = RD_START;

// keyboard state data
byte kbd_state = KBD_INIT;
byte kbd_state2 = 0;
byte ecode = 0;     // saved extended code
byte ecount = 0;    // extended code state counter
byte leds = 0;      // leds' state


//
// kbd handlers
//

byte handleBRK(byte code, byte new_state)
{
    if (code == BRK) {
        dprint("BRK ");
        kbd_state = new_state | KBD_BRK;
        return 1;
    }
    return 0;
}


byte handleEXT(byte code, byte new_state)
{
    if (code == EXT) {
        dprint("EXT ");
        kbd_state = new_state;
        return 1;
    }
    return 0;
}


void kbd_print(struct keydata *kd, byte code)
{
    dprintx(code);
    dprint("'");
    if (kd)
        dprintc(kd->ch);
    else
        dprint("?");
    dprintln("'");
}


void kbd_reset()
{
    kbd_state = KBD_START;
    ecode = 0;
    ecount = 0;
}


void kbd_error(byte code)
{
    dprint("???_");
    dprintlnx(code);

    kbd_reset();
}

void kbd_send_xt(struct keydata *kd, byte brk)
{
    // unknown key
    if (!kd)
        ;
    // key press
    else if (!brk)
        xt_write(kd->xt_code);
    // key release
    else
        xt_write(kd->xt_code | 0x80);

    kbd_reset();
}

void kbd_expect(byte exp_code, byte at_code)
{
    if (at_code == exp_code) {
        dprintx(at_code);
        dprint(" ");
        ecount++;
    } else
        kbd_error(at_code);
}


struct keydata *saved_kd = NULL;
byte saved_leds = 0;
byte wait_wr = 0;

byte cycles = 0;

#ifdef DEBUG_INTR
byte bits[128];
byte states[128];
byte intr = 0;

void dump_intr()
{
    byte saved_intr;

    if (intr) {
        saved_intr = intr;
        intr = 0;
        dprint("INTR: ");
        dprintx(saved_intr);
        dprintln(" states/bits:");

        byte i;
        for (i = 0; i < saved_intr; i++)
            dprintx(states[i]);
        dprintln("");

        for (i = 0; i < saved_intr; i++)
            dprintx(bits[i]);
        dprintln("");
    }
}
#endif

inline
void kbd_abort()
{
    // wait to flush serial
    delay(10000);
    exit(0);
}

void kbd_init()
{
    byte at_code;

    // reset keyboard
    at_write(0xFF);
    while (!at_write_finished())
        delayMicroseconds(1);
    wait_wr = 0;

    // wait for ACK
    while ((at_code = at_read()) == 0)
        delayMicroseconds(1);
    if (at_code != ACK) {
        dprintln("No ACK after RESET");
        kbd_abort();
    }

    // wait for AA
    while ((at_code = at_read()) == 0)
        delayMicroseconds(1);
    if (at_code != 0xAA) {
        dprint("Expected 0xAA from keyboard, got 0x");
        dprintlnx(at_code);
        kbd_abort();
    }

    // send power on self test OK
    delay(10);
    xt_write(0xAA);
    kbd_state = KBD_START;
}

void loop()
{
    struct keydata *kd = NULL;
    byte st = kbd_state & ~KBD_BRK;
    byte brk = kbd_state & KBD_BRK;
    byte at_code = 0;

    if (st == KBD_INIT)
        kbd_init();

    // waiting for write to complete?
    if (wait_wr) {
        if (!at_write_finished())
            goto loop_end;
        else {
            wait_wr = 0;
#ifdef DEBUG_INTR
            dprintln("write complete");
            dump_intr();
#endif
        }

    // read one code from at kbd or 0 if unavailable
    } else {
        at_code = at_read();
        if (!at_code)
            goto loop_end;
#ifdef DEBUG_INTR
        dprintln("\nread complete");
        dump_intr();
#endif
    }

    // handle AT code read
    switch (st) {
        case KBD_START:
            dprint("AT ");

            if (at_code == EXT1) {
                dprint("EXT1 ");
                kbd_state = KBD_PAUSE_BRK;
                break;
            }

        case KBD_WAIT_EXT:
            if (handleEXT(at_code, KBD_EXT))
                break;

            if (handleBRK(at_code, KBD_NON_EXT))
                break;

        case KBD_NON_EXT:
            kd = keytbl_find(at_code);
            kbd_print(kd, at_code);

            // num lock
            if (brk && kd &&
                (kd->at_code == 0x77 ||
                 kd->at_code == 0x58 ||
                 kd->at_code == 0x7E))
            {
                byte led = 0;
                switch (kd->at_code) {
                    case 0x77:  // num lock
                        led = LED_NUM;
                        break;

                    case 0x58:  // caps lock
                        led = LED_CAPS;
                        break;

                    case 0x7E:  // scroll lock
                        led = LED_SCROLL;
                        break;
                }

                at_write(0xED);     // set leds command
                saved_kd = kd;
                saved_leds = leds;
                if (leds & led)
                    saved_leds = leds & ~led;
                else
                    saved_leds = leds | led;
                kbd_state = KBD_SET_LED;
                kbd_state2 = 0;
                break;
            }

            kbd_send_xt(kd, kbd_state & KBD_BRK);
            break;

        case KBD_SET_LED:
            switch (kbd_state2) {
                case 0: // write complete
                    kbd_state2++;
                    break;

                case 1: // ACK?
                    if (at_code != ACK) {
                        dprintln("ERROR: no ACK after set LEDs command:");
                        dprintlnx(at_code);
                        kbd_reset();
                    } else {
                        at_write(saved_leds);
                        kbd_state2++;
                    }
                    break;

                case 2: // write complete
                    kbd_state2++;
                    break;

                case 3: // ACK
                    if (at_code != ACK) {
                        dprintln("ERROR: no ACK after set LEDs command:");
                        dprintlnx(at_code);
                        kbd_reset();
                    } else {
                        leds = saved_leds;
                        dprintln("LED set");
                        kbd_send_xt(saved_kd, 1);
                        kbd_reset();
                    }
                    break;
            }
            break;

        case KBD_EXT:
            if (handleBRK(at_code, KBD_EXT_BRK))
                break;

        case KBD_EXT_BRK:
            // handle print screen
            // EXT 0x12, EXT 0x7C / EXT BRK 0x7C, EXT BRK 0x12
            if (!brk && at_code == 0x12 || brk && at_code == 0x7C) {
                dprintx(at_code);
                dprint(" ");
                ecode = at_code;
                kbd_state = KBD_WAIT_EXT;
                break;
            }

            if (at_code == 0x12) {
                dprintx(at_code);
                dprint(" : ");
            }

            if (!ecode || ecode == 0x12)
                ecode = at_code;

            // find key data
            kd = exttbl_find(ecode);
            kbd_print(kd, ecode);
            kbd_send_xt(kd, kbd_state & KBD_BRK);
            break;

        // pause/break - 0xE1 0x14 0x77 / 0xE1 BRK 0x14 BRK 0x77
        case KBD_PAUSE_BRK:
            switch (ecount) {
                case 0:
                    if (at_code == BRK) {
                        dprint("BRK ");
                        kbd_state |= KBD_BRK;
                        ecount++;
                        break;
                    }
                    ecount++;

                case 1:
                    kbd_expect(0x14, at_code);
                    if (ecount) {
                        if (brk)
                            ecount = 2;
                        else
                            ecount = 3;
                    }
                    break;

                case 2:
                    if (at_code == BRK) {
                        dprint("BRK ");
                        ecount++;
                    } else {
                        kbd_error(at_code);
                    }
                    break;

                case 3:
                    kbd_expect(0x77, at_code);
                    // error?
                    if (ecount == 0)
                        break;

                case 4:
                    kd = &kd_pause_brk;
                    dprintln(": pause/break");
                    kbd_send_xt(kd, kbd_state & KBD_BRK);
                    break;

                default:
                    kbd_error(at_code);
            }
            break;
        // case KBD_PAUSE_BRK
    }   // switch

loop_end:
    // dprintln("loop");
    // delay(10);
    ;
}


void xt_write(byte value)
{
    dprint("XT ");
    dprintlnx(value);
    dprintln("");

    // wait for clk & data to be high: clear to send
    // TODO maybe only need to check clk?
    // TODO use a retry limit to don't get stuck in the loop forever
    pinMode(XT_CLK, INPUT_PULLUP);
    pinMode(XT_DATA, INPUT_PULLUP);
    while (digitalRead(XT_CLK) != HIGH ||
                 digitalRead(XT_DATA) != HIGH)
        delayMicroseconds(10);

    // send start bit (HIGH)
    pinMode(XT_CLK, OUTPUT);
    pinMode(XT_DATA, OUTPUT);
    // hold high first
    digitalWrite(XT_DATA, HIGH);
    digitalWrite(XT_CLK, HIGH);
    delayMicroseconds(XT_QCLK);
    // clk down
    digitalWrite(XT_CLK, LOW);
    delayMicroseconds(XT_HCLK);
    // clk up
    digitalWrite(XT_CLK, HIGH);
    delayMicroseconds(XT_QCLK);

    // send data (8 bits)
    byte i = 0;
    for (i=0; i < 8; i++) {
        // set data
        digitalWrite(XT_DATA, value & 1<<i? HIGH : LOW);
        // clk is high: wait one quarter for stabilization
        delayMicroseconds(XT_QCLK);
        // clk down
        digitalWrite(XT_CLK, LOW);
        delayMicroseconds(XT_HCLK);

        if (i != 7) {
            // clk up for one quarter
            digitalWrite(XT_CLK, HIGH);
            delayMicroseconds(XT_QCLK);
        }
    }
    pinMode(XT_CLK, INPUT_PULLUP);
    pinMode(XT_DATA, INPUT_PULLUP);
}


// data used by clock() and _read() only
//
// byte value that is being received from AT keyboard
byte value = 0;
// has_data flag
byte has_data = 0;

// return last AT KBD byte read, if any
byte at_read()
{
    if (has_data) {
        has_data = 0;
        return value;
    }
    return 0;
}

byte at_read_last()
{
    return value;
}

// data to be written to at kbd
byte wrdata = 0;
byte parity = 0;
byte wrack = 0;

byte at_write_finished()
{
    if (wrack) {
        if (wrack == 2)
            dprintln("write error");
        wrack = 0;
        return 1;
    }
    return 0;
}

void at_write(byte b)
{
    wrdata = b;

    // wait until idle
    pinMode(AT_CLK, INPUT_PULLUP);
    pinMode(AT_DATA, INPUT_PULLUP);
    while (digitalRead(AT_CLK) != HIGH ||
                 digitalRead(AT_DATA) != HIGH)
        delayMicroseconds(10);

    // take CLK low
    at_state = WR_IGNORE;
    pinMode(AT_CLK, OUTPUT);
    digitalWrite(AT_CLK, LOW);
    delayMicroseconds(60);

    // take DATA low
    pinMode(AT_DATA, OUTPUT);
    digitalWrite(AT_DATA, LOW);
    delayMicroseconds(1);

    // release CLK
    pinMode(AT_CLK, INPUT_PULLUP);

    // send data on interrupt handler
    at_state = WR_START;
    wait_wr = 1;
}


// on AT KBD clock LOW interrupt handler
void at_clock_low()
{
    byte bit = 0;
#ifdef DEBUG_INTR
    states[intr] = at_state;
    intr++;
#endif

    switch (at_state) {
        // read
        case RD_START:
            // start bit
            bit = digitalRead(AT_DATA);
            if (bit == LOW) {
                at_state = RD_DATA;
                cycles = 0;
                has_data = 0;
                value = 0;
            }
            break;

        case RD_DATA: {
            bit = digitalRead(AT_DATA);
            // data bits
            value |= (bit << cycles);
            cycles++;
            if (cycles == 8)
                at_state = RD_PARITY;
            break;
        }

        case RD_PARITY:
            bit = digitalRead(AT_DATA);
            // ignoring parity bit
            at_state = RD_STOP;
            break;

        case RD_STOP:
            // stop bit
            bit = digitalRead(AT_DATA);
            if (bit == HIGH)
                has_data = 1;
            at_state = RD_START;
            break;

        // write
        case WR_IGNORE:
            bit = 0;
            break;

        case WR_START:
            cycles = 0;
            parity = 1;
            wrack = 0;
            at_state = WR_DATA;
            bit = 0;
            // break;

        case WR_DATA: {
            bit = (wrdata & (1u << cycles))? 1 : 0;
            digitalWrite(AT_DATA, bit);
            if (bit)
                parity = !parity;
            cycles++;
            if (cycles == 8)
                at_state = WR_PARITY;
            break;
        }

        case WR_PARITY:
            bit = parity;
            digitalWrite(AT_DATA, bit);
            at_state = WR_WAIT;
            break;

        case WR_WAIT:
            bit = 0;
            // release DATA line
            pinMode(AT_DATA, INPUT_PULLUP);
            at_state = WR_ACK;
            break;

        case WR_ACK:
            bit = digitalRead(AT_DATA);
            if (bit == LOW)
                wrack = 1;
            else
                wrack = 2;  // unexpected error
            at_state = RD_START;
            break;
    }

#ifdef DEBUG_INTR
    bits[intr-1] = bit;
#endif
}

