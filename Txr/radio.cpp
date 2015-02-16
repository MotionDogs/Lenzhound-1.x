#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include "Settings.h"
#include "radio.h"

#define RATE_MASK     0b00101000
#define RATE_250KB    0b00100000
#define RATE_1MB      0b00000000
#define RATE_2MB      0b00001000
#define PALEVEL_MASK  0b00000111
#define PALEVEL_18    0b00000000
#define PALEVEL_12    0b00000010
#define PALEVEL_6     0b00000100
#define PALEVEL_0     0b00000011

#define RF_DEFAULT    0b00100000  // 250kbps -18dB

const char rates[] =  { 0b00100000, 0b00000000, 0b00001000 };
const char levels[] = { 0b00000000, 0b00000010, 0b00000100, 0b00000011 };

Radio::Radio(int packetSize)
{
  Mirf.spi = &MirfHardwareSpi; 
  Mirf.init(); // Setup pins / SPI
  Mirf.setTADDR((byte *)"serv1");
  Mirf.payload = packetSize; // Payload length
  LoadSettings();
  Mirf.config(); // Power up reciver
}

void Radio::LoadSettings()
{
	// Get and apply settings if within allowable ranges
  Settings settings;
  byte reg[] =  {RF_DEFAULT,0}; 
  int setting = settings.GetPALevel();
  if (setting >= 0 && setting <= 3) {
    reg[0] &= ~PALEVEL_MASK;
    reg[0] |= levels[setting];
  } 
  setting = settings.GetDataRate(); 
  if (setting >= 0 && setting <= 2) {
    reg[0] &= ~RATE_MASK;
    reg[0] |= rates[setting];
  }    
  setting = settings.GetChannel();
  if (setting >= 1 && setting <= 82) {
    Mirf.channel = setting;
  }  
  Mirf.writeRegister(RF_SETUP, (byte *)reg, 1);
}

void Radio::ReloadSettings()
{
  Mirf.powerDown();  // not sure if this is necessary
  LoadSettings();
  Mirf.config();
}

void Radio::SendPacket(byte *message) 
{
  if (!Mirf.isSending()) {
    Mirf.send(message);
  }
}

int Radio::IsAlive()
{
  uint8_t addr[mirf_ADDR_LEN];
  uint8_t orig[mirf_ADDR_LEN] = {'s','e','r','v','1'};
  Mirf.readRegister(TX_ADDR, addr, mirf_ADDR_LEN);
  return memcmp(addr, orig, mirf_ADDR_LEN) == 0;
}  