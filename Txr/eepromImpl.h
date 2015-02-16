
#ifndef eepromImpl_h
#define eepromImpl_h

//#define EEPROM_MIN_ADDR  0;
//#define EEPROM_MAX_ADDR  1023;

class eeprom
{
public:
  static const int EEPROM_MIN_ADDR = 0;
  static const int EEPROM_MAX_ADDR = 1023;

  // good idea to make these all static since you don't want
  // the idea that a seperate instance of the object gives
  // you a seperate eeprom to read/write to.  Alternative is to
  // make this into a singleton class.
  static char ReadChar(int addr, char* value);
  static char ReadInt16(int addr, int* value);
  static char ReadInt32(int addr, long* value);
  static char WriteChar(int addr, char value);
  static char WriteInt16(int addr, int value);
  static char WriteInt32(int addr, long value);

private:
  static char IsAddrOK(int addr);
  static char ReadBytes(int startAddr, char array[], int numBytes);
  static char WriteBytes(int startAddr, const char* array, int numBytes);
};

#endif  // eepromImpl_h
