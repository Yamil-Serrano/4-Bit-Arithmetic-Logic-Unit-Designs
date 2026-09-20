#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Follow your ALU design and pinout to update these arrays if needed
// Current pin setup assumes Minimalistic 4-Bit ALU Modified 3-Bit OPP Code | Rev. 3.2[[ ]]

// For ESP32
const int PIN_A[] = {26, 25, 33, 32};         // Nibble A [LSB to MSB]
const int PIN_B[] = {4, 16, 17, 5};           // Nibble B [LSB to MSB]
const int PIN_OPCODE[] = {13, 14, 27};        // Opcode [LSB to MSB]
const int PIN_RESULT[] = {18, 19, 35, 34};    // ALU output [LSB to MSB]
const int PIN_FLAG_EQ = 15;
const int PIN_FLAG_ZR = 2;
const int PIN_FLAG_Carry = 23;                

// For Arduino Uno
// const int PIN_A[]      = {2, 3, 4, 5};      // Nibble A [LSB to MSB]
// const int PIN_B[]      = {6, 7, 8, 9};      // Nibble B [LSB to MSB]
// const int PIN_RESULT[] = {10, 11, 12, 13};  // ALU output [LSB to MSB]
// const int PIN_OPCODE[] = {A0, A1, A2};      // Opcode [LSB to MSB]
// const int PIN_FLAG_EQ  = A3;
// const int PIN_FLAG_ZR  = A4;

// ANSI Color Codes
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_RESET "\033[0m"

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Operation Definitions
typedef byte (*OpFunc)(byte, byte);

struct Operation
{
  const char *name;
  OpFunc calc;
};
struct OpcodeResult
{
  const char *opName;
  int hits;
  float pct;
};
struct FlagResult
{
  int correct;
  int total;
  float pct;
};

// ALU Operations according to opcode:
// 000: A + B
// 100: A - B
// X01: A NAND B
// X10: A XOR B
// X11: A NOR B
byte calcSum(byte a, byte b) { return (a + b) & 0x0F; }
byte calcSub(byte a, byte b)
{
  int r = a - b;
  return (r < 0 ? r + 16 : r) & 0x0F;
}
byte calcNAND(byte a, byte b) { return ~(a & b) & 0x0F; }
byte calcXOR(byte a, byte b) { return (a ^ b) & 0x0F; }
byte calcNOR(byte a, byte b) { return ~(a | b) & 0x0F; }

// Mapping opcode to operation function
OpFunc getOpFunc(int opcode)
{
  int b2 = (opcode >> 2) & 1; // MSB
  int b1 = (opcode >> 1) & 1;
  int b0 = opcode & 1;

  if (b2 == 0 && b1 == 0 && b0 == 0)
    return calcSum; // 000
  if (b2 == 1 && b1 == 0 && b0 == 0)
    return calcSub; // 100
  if (b1 == 0 && b0 == 1)
    return calcNAND; // X01 (bit1=0, bit0=1)
  if (b1 == 1 && b0 == 0)
    return calcXOR; // X10 (bit1=1, bit0=0)
  if (b1 == 1 && b0 == 1)
    return calcNOR; // X11 (bit1=1, bit0=1)

  return calcNOR; // fallback
}

const char *getOpName(int opcode)
{
  int b2 = (opcode >> 2) & 1;
  int b1 = (opcode >> 1) & 1;
  int b0 = opcode & 1;

  if (b2 == 0 && b1 == 0 && b0 == 0)
    return "SUM";
  if (b2 == 1 && b1 == 0 && b0 == 0)
    return "SUB";
  if (b1 == 0 && b0 == 1)
    return "NAND";
  if (b1 == 1 && b0 == 0)
    return "XOR";
  if (b1 == 1 && b0 == 1)
    return "NOR";

  return "NOR";
}

OpcodeResult results[8];

// Hardware Helpers
void setOpcode(int opcode)
{
  for (int i = 0; i < 3; i++)
    digitalWrite(PIN_OPCODE[i], (opcode >> i) & 1);
}

void setInputs(byte a, byte b)
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(PIN_A[i], bitRead(a, i));
    digitalWrite(PIN_B[i], bitRead(b, i));
  }
}

byte readResult()
{
  byte val = 0;
  for (int i = 0; i < 4; i++)
    if (digitalRead(PIN_RESULT[i]))
      val |= (1 << i);
  return val;
}

// Core Test Logic
void testOpcode(int opcode)
{
  setOpcode(opcode);
  delayMicroseconds(5);

  OpFunc expectedFunc = getOpFunc(opcode);
  const char *expectedName = getOpName(opcode);

  int hits = 0;
  for (byte a = 0; a < 16; a++)
  {
    for (byte b = 0; b < 16; b++)
    {
      setInputs(a, b);
      delayMicroseconds(5);
      if (readResult() == expectedFunc(a, b))
        hits++;
    }
  }

  results[opcode] = {expectedName, hits, (hits * 100.0f) / 256};
}

// Flag Tests
FlagResult testEqualFlag()
{
  int hits = 0;
  setOpcode(0);
  delayMicroseconds(5);
  for (byte a = 0; a < 16; a++)
  {
    for (byte b = 0; b < 16; b++)
    {
      setInputs(a, b);
      delayMicroseconds(5);
      if ((bool)digitalRead(PIN_FLAG_EQ) == (a == b))
        hits++;
    }
  }
  return {hits, 256, (hits * 100.0f) / 256};
}

FlagResult testZeroFlag()
{
  int hits = 0, total = 0;
  for (int op = 0; op < 8; op++)
  {
    setOpcode(op);
    delayMicroseconds(5);
    OpFunc func = getOpFunc(op);
    for (byte a = 0; a < 16; a++)
    {
      for (byte b = 0; b < 16; b++)
      {
        setInputs(a, b);
        delayMicroseconds(5);
        bool expected = (func(a, b) == 0x00);
        if ((bool)digitalRead(PIN_FLAG_ZR) == expected)
          hits++;
        total++;
      }
    }
  }
  return {hits, total, (hits * 100.0f) / total};
}

FlagResult testCarryFlag(){
  int hits = 0, total = 0;

  for (int op = 0; op < 8; op++){
    // Carry only has meaning for SUM and SUB
    if (op != 0 && op != 4)
      continue;

    setOpcode(op);
    delayMicroseconds(5);

    for (byte a = 0; a < 16; a++){
      for (byte b = 0; b < 16; b++){
        setInputs(a, b);
        delayMicroseconds(5);

        bool expected = false;

        if (op == 0) // SUM: A + B
          expected = (a + b) > 15;

        else if (op == 4) // SUB: A - B
          expected = (a >= b);

        if ((bool)digitalRead(PIN_FLAG_Carry) == expected)
          hits++;

        total++;
      }
    }
  }

  return {hits, total, (hits * 100.0f) / total};
}

// Reporting
void printDivider() { Serial.println("------------------------------------------------------------"); }

void printColored(float pct)
{
  if (pct == 100.0f)
    Serial.print(ANSI_GREEN);
  else
    Serial.print(ANSI_YELLOW);
}

void printOpcodeRow(int op)
{
  int b2 = (op >> 2) & 1, b1 = (op >> 1) & 1, b0 = op & 1;
  Serial.print("  ");
  Serial.print(op);
  Serial.print("   |  ");
  Serial.print(b2);
  Serial.print(" ");
  Serial.print(b1);
  Serial.print(" ");
  Serial.print(b0);
  Serial.print("   |  ");
  Serial.print(results[op].opName);
  for (int i = strlen(results[op].opName); i < 6; i++)
    Serial.print(" ");
  Serial.print("   |  ");
  Serial.print(results[op].hits);
  Serial.print("/256 (");
  printColored(results[op].pct);
  Serial.print(results[op].pct, 1);
  Serial.print("%");
  Serial.print(ANSI_RESET);
  Serial.println(")");
}

void printFlagRow(const char *label, FlagResult r)
{
  Serial.print("  ");
  Serial.print(label);
  Serial.print(r.correct);
  Serial.print("/");
  Serial.print(r.total);
  Serial.print(" (");
  printColored(r.pct);
  Serial.print(r.pct, 1);
  Serial.print("%");
  Serial.print(ANSI_RESET);
  Serial.println(")");
}

void printReport(FlagResult eqResult, FlagResult zrResult, FlagResult carryResult)
{
  Serial.println();
  printDivider();
  Serial.println("                   ALU OPCODE MAP");
  printDivider();
  Serial.println("  Op  |   bits   |   Func    |    Hits");
  printDivider();
  for (int op = 0; op < 8; op++)
    printOpcodeRow(op);
  printDivider();

  Serial.println("\n  [PASS] 100% match:");
  for (int op = 0; op < 8; op++)
    if (results[op].pct == 100.0f)
    {
      Serial.print("    Opcode ");
      Serial.print(op);
      Serial.print(" -> ");
      Serial.print(ANSI_GREEN);
      Serial.print(results[op].opName);
      Serial.println(ANSI_RESET);
    }

  bool anyWarn = false;
  for (int op = 0; op < 8; op++)
    if (results[op].pct < 100.0f && results[op].pct > 0)
    {
      anyWarn = true;
      break;
    }

  Serial.print("\n  [");
  Serial.print(ANSI_YELLOW);
  Serial.print("WARNING");
  Serial.print(ANSI_RESET);
  Serial.println("] Partial match:");

  if (!anyWarn)
  {
    Serial.print("    ");
    Serial.print(ANSI_GREEN);
    Serial.println("None");
    Serial.print(ANSI_RESET);
  }
  else
  {
    for (int op = 0; op < 8; op++)
      if (results[op].pct < 100.0f && results[op].pct > 0)
      {
        Serial.print("    Opcode ");
        Serial.print(op);
        Serial.print(" -> ");
        Serial.print(results[op].opName);
        Serial.print(" (");
        Serial.print(ANSI_YELLOW);
        Serial.print(results[op].pct, 1);
        Serial.print("%");
        Serial.print(ANSI_RESET);
        Serial.println(")");
      }
  }

  // Flag Tests
  Serial.println();
  printDivider();
  Serial.println("                     FLAG TESTS");
  printDivider();
  Serial.println("  Flag  |      Description                 |  Result");
  printDivider();
  printFlagRow("EQ    | Nibble A equal to Nibble B       | ", eqResult);
  printFlagRow("ZR    | When ALU Result = 0000           | ", zrResult);
  printFlagRow("C     | When ALU Result > 1111 or < 0000 | ", carryResult);
  printDivider();
  Serial.println("                    ALU TEST COMPLETE");
  printDivider();
}

void setup()
{
  delay(1000);
  Serial.begin(115200);

  // LCD Boot Screen
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Hardware Bench");
  lcd.setCursor(0, 1);
  lcd.print("   Initializing");
  lcd.setCursor(0, 3);
  lcd.print(" Made by: Neowizen");
  delay(500);

  for (int i = 0; i < 4; i++)
  {
    pinMode(PIN_A[i], OUTPUT);
    pinMode(PIN_B[i], OUTPUT);
    pinMode(PIN_RESULT[i], INPUT);
  }
  for (int i = 0; i < 3; i++)
    pinMode(PIN_OPCODE[i], OUTPUT);
  pinMode(PIN_FLAG_EQ, INPUT);
  pinMode(PIN_FLAG_ZR, INPUT);
  pinMode(PIN_FLAG_Carry, INPUT);
  delay(2000);

  Serial.println("\n");
  printDivider();
  Serial.println("               4-bit ALU Full Opcode Test");
  printDivider();

  for (int op = 0; op < 8; op++)
  {
    Serial.print("Testing opcode ");
    Serial.print(op);
    Serial.print("... ");
    testOpcode(op);
    Serial.print(results[op].opName);
    Serial.print(": ");
    for (int i = strlen(results[op].opName); i < 6; i++)
      Serial.print(" ");
    Serial.print(results[op].hits);
    Serial.print("/256 (");
    printColored(results[op].pct);
    Serial.print(results[op].pct, 1);
    Serial.print("%");
    Serial.print(ANSI_RESET);
    Serial.println(")");

    // LCD Real-Time Progress
    int progress = map(op + 1, 0, 8, 1, 18);
    for (int i = 1; i <= progress; i++)
    {
      lcd.setCursor(i, 2);
      lcd.print((char)255);
    }
  }

  FlagResult eqResult = testEqualFlag();
  FlagResult zrResult = testZeroFlag();
  FlagResult carryResult = testCarryFlag();

  printReport(eqResult, zrResult, carryResult);

  // LCD Final Report
  int perfect = 0;
  for (int op = 0; op < 8; op++)
  {
    if (results[op].pct == 100.0f)
      perfect++;
  }
  lcd.clear();
  float totalPct = 0.0f;
  for (int op = 0; op < 8; op++)
  {
    totalPct += results[op].pct;
  }
  float avgPct = totalPct / 8.0f;
  lcd.setCursor(0, 0);
  lcd.print("   TEST COMPLETED");
  lcd.setCursor(0, 1);
  lcd.print("   Result: ");
  lcd.print(avgPct, 1);
  lcd.print("%");
  lcd.setCursor(0, 2);
  lcd.print(" More bench details");
  lcd.setCursor(0, 3);
  lcd.print(" in Serial Terminal");
}

void loop() { delay(200); }