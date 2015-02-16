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

#include "qp_port.h"
#include "Txr.h"
#include "bsp.h"
#include <Encoder.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "Arduino.h"
#include <CmdMessenger.h>
#include <EEPROM.h>


// Local-scope objects -------------------------------------------------------
static QEvt const *txrQueueSto[10];
static QSubscrList subscrSto[MAX_PUB_SIG];

static QF_MPOOL_EL(PositionButtonEvt) smlPoolSto[10]; // storage for the small event pool

//............................................................................
void setup() {

  // initialize the BSP  
  BSP_init();                                          

  // initialize the framework and the underlying RT kernel
  QF::init();       

  // initialize event pools...
  QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

  // init publish-subscribe
  QF::psInit(subscrSto, Q_DIM(subscrSto));     

  // start the active objects...
  AO_Txr->start(1U, txrQueueSto, Q_DIM(txrQueueSto), (void *)0, 0U);
}

//////////////////////////////////////////////////////////////////////////////
// NOTE: Do not define the loop() function, because this function is
// already defined in the QP port to Arduino
