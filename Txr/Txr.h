//****************************************************************************
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
#ifndef Txr_h
#define Txr_h

using namespace QP;

enum PelicanSignals {
    ENC_DOWN_SIG = Q_USER_SIG,
    ENC_UP_SIG,
    PLAY_MODE_SIG,
    FREE_MODE_SIG,
    Z_MODE_SIG,
    POSITION_BUTTON_SIG,
    UPDATE_PARAMS_SIG,
    ALIVE_SIG,
    MAX_PUB_SIG,  // the last published signal

    SEND_TIMEOUT_SIG,
    FLASH_RATE_SIG,
    CALIBRATION_SIG
};


struct PositionButtonEvt : public QP::QEvt
{
  char ButtonNum;
};


// active objects ..................................................
extern QActive * const AO_Txr;
extern QActive * const AO_Blinky;
   

#endif // Txr_h
