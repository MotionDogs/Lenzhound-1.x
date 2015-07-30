#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include "Settings.h"
#include "macros.h"
#include "receiver.h"
#include "util.h"

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

Receiver::Receiver() {
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init(); // Setup pins / SPI
  Mirf.setRADDR((byte *)"serv1"); // Configure recieving address
  Mirf.payload = sizeof(Packet); // Payload length
  LoadSettings();
  Mirf.config(); // Power up reciver
}

void Receiver::LoadSettings()
{
	// Get and apply settings if within allowable ranges
  Settings settings;
  byte reg[] =  {RF_DEFAULT,0}; 
  unsigned char setting = settings.GetPALevel();
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
  setting = settings.GetAntenna();
  if (setting == 0) {
    ANT_CTRL2(SET);
    ANT_CTRL1(CLR);
  } else {
    ANT_CTRL1(SET);
    ANT_CTRL2(CLR);
  }
  Mirf.writeRegister(RF_SETUP, (byte *)reg, 1);
}

void Receiver::ReloadSettings()
{
  Mirf.powerDown();  // not sure if this is necessary
  LoadSettings();
  Mirf.config();
}

void Receiver::GetData() {
  if(Mirf.dataReady()){ // Got packet
    Mirf.getData((byte *) &packet_);
    packet_.position = util::MakeFixed(packet_.position);
  }
}

long Receiver::Position(){
  return packet_.position;
}

int Receiver::Velocity() {
  if(packet_.mode==PLAYBACK_MODE || packet_.mode==Z_MODE)
    return packet_.velocity;
  return MAX_VELOCITY;
}

int Receiver::Acceleration() {
  if(packet_.mode==Z_MODE)
    return packet_.acceleration;
  return MAX_ACCELERATION;
}

int Receiver::Mode(){
  return packet_.mode;
}
