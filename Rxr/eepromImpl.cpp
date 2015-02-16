
#include <EEPROM.h>
#include "eepromImpl.h"


//
// Returns true if the address is between the
// minimum and maximum allowed values,
// false otherwise.
//
// This function is used by the other, higher-level functions
// to prevent bugs and runtime errors due to invalid addresses.
//
char eeprom::IsAddrOK(int addr) {
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}


//
// Writes a sequence of bytes to eeprom starting at the specified address.
// Returns true if the whole array is successfully written.
// Returns false if the start or end addresses aren't between
// the minimum and maximum allowed values.
// When returning false, nothing gets written to eeprom.
//
char eeprom::WriteBytes(int startAddr, const char* array, int numBytes) {
  // counter
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range  
  if (!IsAddrOK(startAddr) || !IsAddrOK(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }

  return true;
}


//
// Reads the specified number of bytes from the specified address into the provided buffer.
// Returns true if all the bytes are successfully read.
// Returns false if the star or end addresses aren't between
// the minimum and maximum allowed values.
// When returning false, the provided array is untouched.
//
// Note: the caller must ensure that array[] has enough space
// to store at most numBytes bytes.
//
char eeprom::ReadBytes(int startAddr, char array[], int numBytes) {
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range  
  if (!IsAddrOK(startAddr) || !IsAddrOK(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    array[i] = EEPROM.read(startAddr + i);
  }

  return true;
}


//
// Writes an 32 bit int variable at the specified address.
// Returns true if the variable value is successfully written.
// Returns false if the specified address is outside the
// allowed range or too close to the maximum value
// to store all of the bytes (an int variable requires
// more than one byte).
//
char eeprom::WriteInt32(int addr, long value) {
  char *ptr;

  ptr = (char*)&value;
  return WriteBytes(addr, ptr, sizeof(value));
}


//
// Writes an int variable at the specified address.
// Returns true if the variable value is successfully written.
// Returns false if the specified address is outside the
// allowed range or too close to the maximum value
// to store all of the bytes (an int variable requires
// more than one byte).
//
char eeprom::WriteInt16(int addr, int value) {
  char *ptr;

  ptr = (char*)&value;
  return WriteBytes(addr, ptr, sizeof(value));
}


//
// Writes a char variable at the specified address.
// Returns true if the variable value is successfully written.
// Returns false if the specified address is outside the
// allowed range.
//
char eeprom::WriteChar(int addr, char value) {
  char *ptr;

  ptr = (char*)&value;
  return WriteBytes(addr, ptr, sizeof(value));
}


//
// Reads an integer value at the specified address.
// Returns true if the variable is successfully read.
// Returns false if the specified address is outside the
// allowed range or too close to the maximum vlaue
// to hold all of the bytes (an int variable requires
// more than one byte).
//
char eeprom::ReadInt32(int addr, long* value) {
  return ReadBytes(addr, (char*)value, sizeof(long));
}


//
// Reads an integer value at the specified address.
// Returns true if the variable is successfully read.
// Returns false if the specified address is outside the
// allowed range or too close to the maximum vlaue
// to hold all of the bytes (an int variable requires
// more than one byte).
//
char eeprom::ReadInt16(int addr, int* value) {
  return ReadBytes(addr, (char*)value, sizeof(int));
}


//
// Reads a char value at the specified address.
// Returns true if the variable is successfully read.
// Returns false if the specified address is outside the
// allowed range.
//
char eeprom::ReadChar(int addr, char* value) {
  return ReadBytes(addr, (char*)value, sizeof(char));
}
