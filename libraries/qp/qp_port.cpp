//////////////////////////////////////////////////////////////////////////////
// Product: QP/C++ port to Arduino
// Last Updated for QP ver: 4.5.02 (modified to fit in one file)
// Date of the Last Update: Aug 24, 2012
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// Quantum Leaps Web sites: http://www.quantum-leaps.com
//                          http://www.state-machine.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"                                                // QP port

//Q_DEFINE_THIS_MODULE("qp_port")

//............................................................................
extern "C" void loop() {
    (void)QP_ QF::run(); //run the application, NOTE: QF::run() doesn't return
}

//............................................................................
#ifdef QK_PREEMPTIVE
void QK_init(void) {
}
#endif

                 // device driver signal offset at the top of the signal range
#if (Q_SIGNAL_SIZE == 1)
    #define DEV_DRIVER_SIG  static_cast<QP_ QSignal>(0xFFU - 8U)
#elif (Q_SIGNAL_SIZE == 2)
    #define DEV_DRIVER_SIG  static_cast<QP_ QSignal>(0xFFFFU - 32U)
#elif (Q_SIGNAL_SIZE == 4)
    #define DEV_DRIVER_SIG  static_cast<QP_ QSignal>(0xFFFFFFFFU - 256U)
#else
    #error "Q_SIGNAL_SIZE not defined or incorrect"
#endif

