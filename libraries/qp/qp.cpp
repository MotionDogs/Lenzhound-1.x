//////////////////////////////////////////////////////////////////////////////
// Product: QP/C++
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

// "qep_pkg.h" ===============================================================
QP_BEGIN_

Q_DEFINE_THIS_MODULE("qp")

//////////////////////////////////////////////////////////////////////////////
/// preallocated reserved events
extern QEvt const QEP_reservedEvt_[];

/// empty signal for internal use only
QSignal const QEP_EMPTY_SIG_ = static_cast<QSignal>(0);

/// maximum depth of state nesting (including the top level), must be >= 3
int8_t const QEP_MAX_NEST_DEPTH_ = static_cast<int8_t>(6);

uint8_t const u8_0  = static_cast<uint8_t>(0); ///< \brief constant (uint8_t)0
uint8_t const u8_1  = static_cast<uint8_t>(1); ///< \brief constant (uint8_t)1
int8_t  const s8_0  = static_cast<int8_t>(0);  ///< \brief constant  (int8_t)0
int8_t  const s8_1  = static_cast<int8_t>(1);  ///< \brief constant  (int8_t)1
int8_t  const s8_n1 = static_cast<int8_t>(-1); ///< \brief constant (int8_t)-1

QP_END_

/// helper macro to trigger internal event in an HSM
#define QEP_TRIG_(state_, sig_) \
    ((*(state_))(this, &QEP_reservedEvt_[sig_]))

/// helper macro to trigger exit action in an HSM
#define QEP_EXIT_(state_) do { \
    if (QEP_TRIG_(state_, Q_EXIT_SIG) == Q_RET_HANDLED) { \
        QS_BEGIN_(QS_QEP_STATE_EXIT, QS::smObj_, this) \
            QS_OBJ_(this); \
            QS_FUN_(state_); \
        QS_END_() \
    } \
} while (false)

/// helper macro to trigger entry action in an HSM
#define QEP_ENTER_(state_) do { \
    if (QEP_TRIG_(state_, Q_ENTRY_SIG) == Q_RET_HANDLED) { \
        QS_BEGIN_(QS_QEP_STATE_ENTRY, QS::smObj_, this) \
            QS_OBJ_(this); \
            QS_FUN_(state_); \
        QS_END_() \
    } \
} while (false)

// "qep.cpp" =================================================================
/// \brief ::QEP_reservedEvt_ definition and QEP::getVersion() implementation.

QP_BEGIN_

// Package-scope objects -----------------------------------------------------
QEvt const QEP_reservedEvt_[] = {
#ifdef Q_EVT_CTOR                         // Is the QEvt constructor provided?
    static_cast<QSignal>(0),
    static_cast<QSignal>(1),
    static_cast<QSignal>(2),
    static_cast<QSignal>(3)
#else                                    // QEvt is a POD (Plain Old Datatype)
    { static_cast<QSignal>(0), u8_0, u8_0 },
    { static_cast<QSignal>(1), u8_0, u8_0 },
    { static_cast<QSignal>(2), u8_0, u8_0 },
    { static_cast<QSignal>(3), u8_0, u8_0 }
#endif
};
//............................................................................
char_t const Q_ROM * Q_ROM_VAR QEP::getVersion(void) {
    uint8_t const u8_zero = static_cast<uint8_t>('0');
    static char_t const Q_ROM Q_ROM_VAR version[] = {
        static_cast<char_t>(((QP_VERSION >> 12) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  8) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  4) & 0xFU) + u8_zero),
        static_cast<char_t>((QP_VERSION         & 0xFU) + u8_zero),
        static_cast<char_t>('\0')
    };
    return version;
}

QP_END_

// "qhsm_top.cpp" ============================================================
/// \brief QHsm::top() implementation.

QP_BEGIN_

//............................................................................
QState QHsm::top(void * const, QEvt const * const) {
    return Q_RET_IGNORED;                  // the top state ignores all events
}

QP_END_

// "qhsm_ini.cpp" ============================================================
/// \brief QHsm::init() implementation.

QP_BEGIN_

//............................................................................
QHsm::~QHsm() {
}
//............................................................................
void QHsm::init(QEvt const * const e) {
    QStateHandler t = m_state;

    Q_REQUIRE((m_temp != Q_STATE_CAST(0))             // ctor must be executed
              && (t == Q_STATE_CAST(&QHsm::top)));  // initial tran. NOT taken

                              // the top-most initial transition must be taken
    Q_ALLEGE((*m_temp)(this, e) == Q_RET_TRAN);

    QS_CRIT_STAT_
    do {                                           // drill into the target...
        QStateHandler path[QEP_MAX_NEST_DEPTH_];
        int8_t ip = s8_0;                       // transition entry path index

        QS_BEGIN_(QS_QEP_STATE_INIT, QS::smObj_, this)
            QS_OBJ_(this);                        // this state machine object
            QS_FUN_(t);                                    // the source state
            QS_FUN_(m_temp);           // the target of the initial transition
        QS_END_()

        path[0] = m_temp;
        (void)QEP_TRIG_(m_temp, QEP_EMPTY_SIG_);
        while (m_temp != t) {
            ++ip;
            path[ip] = m_temp;
            (void)QEP_TRIG_(m_temp, QEP_EMPTY_SIG_);
        }
        m_temp = path[0];
                                               // entry path must not overflow
        Q_ASSERT(ip < QEP_MAX_NEST_DEPTH_);

        do {           // retrace the entry path in reverse (desired) order...
            QEP_ENTER_(path[ip]);                            // enter path[ip]
            --ip;
        } while (ip >= s8_0);

        t = path[0];                   // current state becomes the new source
    } while (QEP_TRIG_(t, Q_INIT_SIG) == Q_RET_TRAN);

    QS_BEGIN_(QS_QEP_INIT_TRAN, QS::smObj_, this)
        QS_TIME_();                                              // time stamp
        QS_OBJ_(this);                            // this state machine object
        QS_FUN_(t);                                    // the new active state
    QS_END_()

    m_state = t;                            // change the current active state
    m_temp  = t;                           // mark the configuration as stable
}

QP_END_

// "qhsm_dis.cpp" ============================================================
/// \brief QHsm::dispatch() implementation.

QP_BEGIN_

//............................................................................
void QHsm::dispatch(QEvt const * const e) {
    QStateHandler t = m_state;

    Q_REQUIRE(t == m_temp);          // the state configuration must be stable

    QStateHandler s;
    QState r;
    QS_CRIT_STAT_

    QS_BEGIN_(QS_QEP_DISPATCH, QS::smObj_, this)
        QS_TIME_();                                              // time stamp
        QS_SIG_(e->sig);                            // the signal of the event
        QS_OBJ_(this);                            // this state machine object
        QS_FUN_(t);                                       // the current state
    QS_END_()

    do {                                // process the event hierarchically...
        s = m_temp;
        r = (*s)(this, e);                           // invoke state handler s

        if (r == Q_RET_UNHANDLED) {               // unhandled due to a guard?

            QS_BEGIN_(QS_QEP_UNHANDLED, QS::smObj_, this)
                QS_SIG_(e->sig);                    // the signal of the event
                QS_OBJ_(this);                    // this state machine object
                QS_FUN_(s);                               // the current state
            QS_END_()

            r = QEP_TRIG_(s, QEP_EMPTY_SIG_);          // find superstate of s
        }
    } while (r == Q_RET_SUPER);

    if (r == Q_RET_TRAN) {                                // transition taken?
        QStateHandler path[QEP_MAX_NEST_DEPTH_];
        int8_t ip = s8_n1;                      // transition entry path index
        int8_t iq;                       // helper transition entry path index
#ifdef Q_SPY
        QStateHandler src = s;       // save the transition source for tracing
#endif

        path[0] = m_temp;                 // save the target of the transition
        path[1] = t;

        while (t != s) {       // exit current state to transition source s...
            if (QEP_TRIG_(t, Q_EXIT_SIG) == Q_RET_HANDLED) {   //exit handled?
                QS_BEGIN_(QS_QEP_STATE_EXIT, QS::smObj_, this)
                    QS_OBJ_(this);                // this state machine object
                    QS_FUN_(t);                            // the exited state
                QS_END_()

                (void)QEP_TRIG_(t, QEP_EMPTY_SIG_);    // find superstate of t
            }
            t = m_temp;                         // m_temp holds the superstate
        }

        t = path[0];                               // target of the transition

        if (s == t) {         // (a) check source==target (transition to self)
            QEP_EXIT_(s);                                   // exit the source
            ip = s8_0;                                     // enter the target
        }
        else {
            (void)QEP_TRIG_(t, QEP_EMPTY_SIG_);        // superstate of target
            t = m_temp;
            if (s == t) {                   // (b) check source==target->super
                ip = s8_0;               // enter the target
            }
            else {
                (void)QEP_TRIG_(s, QEP_EMPTY_SIG_);       // superstate of src
                                     // (c) check source->super==target->super
                if (m_temp == t) {
                    QEP_EXIT_(s);                           // exit the source
                    ip = s8_0;           // enter the target
                }
                else {
                                            // (d) check source->super==target
                    if (m_temp == path[0]) {
                        QEP_EXIT_(s);                       // exit the source
                    }
                    else { // (e) check rest of source==target->super->super..
                           // and store the entry path along the way
                           //
                        iq = s8_0; // indicate LCA not found
                        ip = s8_1; // enter target's superst
                        path[1] = t;          // save the superstate of target
                        t = m_temp;                      // save source->super
                                                  // find target->super->super
                        r = QEP_TRIG_(path[1], QEP_EMPTY_SIG_);
                        while (r == Q_RET_SUPER) {
                            ++ip;
                            path[ip] = m_temp;         // store the entry path
                            if (m_temp == s) {            // is it the source?
                                                    // indicate that LCA found
                                iq = s8_1;
                                               // entry path must not overflow
                                Q_ASSERT(ip < QEP_MAX_NEST_DEPTH_);
                                --ip;               // do not enter the source
                                r = Q_RET_HANDLED;       // terminate the loop
                            }
                            else {      // it is not the source, keep going up
                                r = QEP_TRIG_(m_temp, QEP_EMPTY_SIG_);
                            }
                        }
                        if (iq == s8_0) {  // LCA found yet?

                                               // entry path must not overflow
                            Q_ASSERT(ip < QEP_MAX_NEST_DEPTH_);

                            QEP_EXIT_(s);                   // exit the source

                            // (f) check the rest of source->super
                            //                  == target->super->super...
                            //
                            iq = ip;
                            r = Q_RET_IGNORED;       // indicate LCA NOT found
                            do {
                                if (t == path[iq]) {       // is this the LCA?
                                    r = Q_RET_HANDLED;   // indicate LCA found
                                                           // do not enter LCA
                                    ip = static_cast<int8_t>(iq - s8_1);
                                                         // terminate the loop
                                    iq = s8_n1;
                                }
                                else {
                                    --iq;    // try lower superstate of target
                                }
                            } while (iq >= s8_0);

                            if (r != Q_RET_HANDLED) {    // LCA not found yet?
                                // (g) check each source->super->...
                                // for each target->super...
                                //
                                r = Q_RET_IGNORED;             // keep looping
                                do {
                                                          // exit t unhandled?
                                    if (QEP_TRIG_(t, Q_EXIT_SIG)
                                        == Q_RET_HANDLED)
                                    {
                                        QS_BEGIN_(QS_QEP_STATE_EXIT,
                                                  QS::smObj_, this)
                                            QS_OBJ_(this);
                                            QS_FUN_(t);
                                        QS_END_()

                                        (void)QEP_TRIG_(t, QEP_EMPTY_SIG_);
                                    }
                                    t = m_temp;          //  set to super of t
                                    iq = ip;
                                    do {
                                        if (t == path[iq]) {   // is this LCA?
                                                           // do not enter LCA
                                            ip = static_cast<int8_t>(iq-s8_1);
                                                // break out of the inner loop
                                            iq = s8_n1;
                                            r = Q_RET_HANDLED;  // break outer
                                        }
                                        else {
                                            --iq;
                                        }
                                    } while (iq >= s8_0);
                                } while (r != Q_RET_HANDLED);
                            }
                        }
                    }
                }
            }
        }
                       // retrace the entry path in reverse (desired) order...
        for (; ip >= s8_0; --ip) {
            QEP_ENTER_(path[ip]);                            // enter path[ip]
        }
        t = path[0];                         // stick the target into register
        m_temp = t;                                   // update the next state

                                         // drill into the target hierarchy...
        while (QEP_TRIG_(t, Q_INIT_SIG) == Q_RET_TRAN) {

            QS_BEGIN_(QS_QEP_STATE_INIT, QS::smObj_, this)
                QS_OBJ_(this);                    // this state machine object
                QS_FUN_(t);                        // the source (pseudo)state
                QS_FUN_(m_temp);              // the target of the transition
            QS_END_()

            ip = s8_0;
            path[0] = m_temp;
            (void)QEP_TRIG_(m_temp, QEP_EMPTY_SIG_);       // find superstate
            while (m_temp != t) {
                ++ip;
                path[ip] = m_temp;
                (void)QEP_TRIG_(m_temp, QEP_EMPTY_SIG_);   // find superstate
            }
            m_temp = path[0];
                                               // entry path must not overflow
            Q_ASSERT(ip < QEP_MAX_NEST_DEPTH_);

            do {       // retrace the entry path in reverse (correct) order...
                QEP_ENTER_(path[ip]);                        // enter path[ip]
                --ip;
            } while (ip >= s8_0);

            t = path[0];
        }

        QS_BEGIN_(QS_QEP_TRAN, QS::smObj_, this)
            QS_TIME_();                                          // time stamp
            QS_SIG_(e->sig);                        // the signal of the event
            QS_OBJ_(this);                        // this state machine object
            QS_FUN_(src);                      // the source of the transition
            QS_FUN_(t);                                // the new active state
        QS_END_()

    }
    else {                                             // transition not taken
#ifdef Q_SPY
        if (r == Q_RET_HANDLED) {

            QS_BEGIN_(QS_QEP_INTERN_TRAN, QS::smObj_, this)
                QS_TIME_();                                      // time stamp
                QS_SIG_(e->sig);                    // the signal of the event
                QS_OBJ_(this);                    // this state machine object
                QS_FUN_(m_state);          // the state that handled the event
            QS_END_()

        }
        else {

            QS_BEGIN_(QS_QEP_IGNORED, QS::smObj_, this)
                QS_TIME_();                                      // time stamp
                QS_SIG_(e->sig);                    // the signal of the event
                QS_OBJ_(this);                    // this state machine object
                QS_FUN_(m_state);                         // the current state
            QS_END_()

        }
#endif
    }

    m_state = t;                            // change the current active state
    m_temp  = t;                           // mark the configuration as stable
}

QP_END_

// "qf_pkg.h" ================================================================
/// \brief Internal (package scope) QF/C++ interface.

/// \brief helper macro to cast const away from an event pointer \a e_
#define QF_EVT_CONST_CAST_(e_) const_cast<QEvt *>(e_)

QP_BEGIN_
                                               // QF-specific critical section
#ifndef QF_CRIT_STAT_TYPE
    /// \brief This is an internal macro for defining the critical section
    /// status type.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// provides the definition of the critical section status varaible.
    /// Otherwise this macro is empty.
    /// \sa #QF_CRIT_STAT_TYPE
    ///
    #define QF_CRIT_STAT_

    /// \brief This is an internal macro for entering a critical section.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// invokes #QF_CRIT_ENTRY passing the key variable as the parameter.
    /// Otherwise #QF_CRIT_ENTRY is invoked with a dummy parameter.
    /// \sa #QF_CRIT_ENTRY
    ///
    #define QF_CRIT_ENTRY_()    QF_CRIT_ENTRY(dummy)

    /// \brief This is an internal macro for exiting a cricial section.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// invokes #QF_CRIT_EXIT passing the key variable as the parameter.
    /// Otherwise #QF_CRIT_EXIT is invoked with a dummy parameter.
    /// \sa #QF_CRIT_EXIT
    ///
    #define QF_CRIT_EXIT_()     QF_CRIT_EXIT(dummy)

#else
    #define QF_CRIT_STAT_       QF_CRIT_STAT_TYPE critStat_;
    #define QF_CRIT_ENTRY_()    QF_CRIT_ENTRY(critStat_)
    #define QF_CRIT_EXIT_()     QF_CRIT_EXIT(critStat_)
#endif

// package-scope objects -----------------------------------------------------
extern QTimeEvt *QF_timeEvtListHead_;  ///< head of linked list of time events
extern QF_EPOOL_TYPE_ QF_pool_[QF_MAX_EPOOL];        ///< allocate event pools
extern uint8_t QF_maxPool_;                  ///< # of initialized event pools
extern QSubscrList *QF_subscrList_;             ///< the subscriber list array
extern enum_t QF_maxSignal_;                 ///< the maximum published signal

//............................................................................
/// \brief Structure representing a free block in the Native QF Memory Pool
/// \sa ::QMPool
struct QFreeBlock {
    QFreeBlock *m_next;
};

//////////////////////////////////////////////////////////////////////////////
// internal helper inline functions

/// \brief access to the poolId_ of an event \a e
inline uint8_t QF_EVT_POOL_ID_(QEvt const * const e) { return e->poolId_; }

/// \brief access to the refCtr_ of an event \a e
inline uint8_t QF_EVT_REF_CTR_(QEvt const * const e) { return e->refCtr_; }

/// \brief increment the refCtr_ of an event \a e
inline void QF_EVT_REF_CTR_INC_(QEvt const * const e) {
    ++(QF_EVT_CONST_CAST_(e))->refCtr_;
}

/// \brief decrement the refCtr_ of an event \a e
inline void QF_EVT_REF_CTR_DEC_(QEvt const * const e) {
    --(QF_EVT_CONST_CAST_(e))->refCtr_;
}

//////////////////////////////////////////////////////////////////////////////
// internal frequently used srongly-typed constants

QTimeEvtCtr const tc_0 = static_cast<QTimeEvtCtr>(0);

void     * const null_void = static_cast<void *>(0);
QEvt const * const null_evt  = static_cast<QEvt const *>(0);
QTimeEvt * const null_tevt = static_cast<QTimeEvt *>(0);
QActive  * const null_act  = static_cast<QActive *>(0);

QP_END_

/// \brief access element at index \a i_ from the base pointer \a base_
#define QF_PTR_AT_(base_, i_) (base_[i_])

//////////////////////////////////////////////////////////////////////////////
#ifdef Q_SPY                                   // QS software tracing enabled?

    #if (QF_EQUEUE_CTR_SIZE == 1)

        /// \brief Internal QS macro to output an unformatted event queue
        /// counter data element
        /// \note the counter size depends on the macro #QF_EQUEUE_CTR_SIZE.
        #define QS_EQC_(ctr_)       QS::u8_(ctr_)
    #elif (QF_EQUEUE_CTR_SIZE == 2)
        #define QS_EQC_(ctr_)       QS::u16_(ctr_)
    #elif (QF_EQUEUE_CTR_SIZE == 4)
        #define QS_EQC_(ctr_)       QS::u32_(ctr_)
    #else
        #error "QF_EQUEUE_CTR_SIZE not defined"
    #endif


    #if (QF_EVENT_SIZ_SIZE == 1)

        /// \brief Internal QS macro to output an unformatted event size
        /// data element
        /// \note the event size depends on the macro #QF_EVENT_SIZ_SIZE.
        #define QS_EVS_(size_)      QS::u8_(size_)
    #elif (QF_EVENT_SIZ_SIZE == 2)
        #define QS_EVS_(size_)      QS::u16_(size_)
    #elif (QF_EVENT_SIZ_SIZE == 4)
        #define QS_EVS_(size_)      QS::u32_(size_)
    #endif


    #if (QF_MPOOL_SIZ_SIZE == 1)

        /// \brief Internal QS macro to output an unformatted memory pool
        /// block-size data element
        /// \note the block-size depends on the macro #QF_MPOOL_SIZ_SIZE.
        #define QS_MPS_(size_)      QS::u8_(size_)
    #elif (QF_MPOOL_SIZ_SIZE == 2)
        #define QS_MPS_(size_)      QS::u16_(size_)
    #elif (QF_MPOOL_SIZ_SIZE == 4)
        #define QS_MPS_(size_)      QS::u32_(size_)
    #endif

    #if (QF_MPOOL_CTR_SIZE == 1)

        /// \brief Internal QS macro to output an unformatted memory pool
        /// block-counter data element
        /// \note the counter size depends on the macro #QF_MPOOL_CTR_SIZE.
        #define QS_MPC_(ctr_)       QS::u8_(ctr_)
    #elif (QF_MPOOL_CTR_SIZE == 2)
        #define QS_MPC_(ctr_)       QS::u16_(ctr_)
    #elif (QF_MPOOL_CTR_SIZE == 4)
        #define QS_MPC_(ctr_)       QS::u32_(ctr_)
    #endif


    #if (QF_TIMEEVT_CTR_SIZE == 1)

        /// \brief Internal QS macro to output an unformatted time event
        /// tick-counter data element
        /// \note the counter size depends on the macro #QF_TIMEEVT_CTR_SIZE.
        #define QS_TEC_(ctr_)       QS::u8_(ctr_)
    #elif (QF_TIMEEVT_CTR_SIZE == 2)
        #define QS_TEC_(ctr_)       QS::u16_(ctr_)
    #elif (QF_TIMEEVT_CTR_SIZE == 4)
        #define QS_TEC_(ctr_)       QS::u32_(ctr_)
    #endif

#endif                                                                // Q_SPY

// "qa_defer.cpp" ============================================================
/// \brief QActive::defer() and QActive::recall() implementation.
///

QP_BEGIN_

//............................................................................
void QActive::defer(QEQueue * const eq, QEvt const * const e) const {
    eq->postFIFO(e);
}
//............................................................................
bool QActive::recall(QEQueue * const eq) {
    QEvt const * const e = eq->get();  // try to get evt from deferred queue
    bool const recalled = (e != null_evt);                 // event available?
    if (recalled) {
        postLIFO(e);      // post it to the front of the Active Object's queue

        QF_CRIT_STAT_
        QF_CRIT_ENTRY_();

        if (QF_EVT_POOL_ID_(e) != u8_0) {            // is it a dynamic event?

            // after posting to the AO's queue the event must be referenced
            // at least twice: once in the deferred event queue (eq->get()
            // did NOT decrement the reference counter) and once in the
            // AO's event queue.
            Q_ASSERT(QF_EVT_REF_CTR_(e) > u8_1);

            // we need to decrement the reference counter once, to account
            // for removing the event from the deferred event queue.
            //
            QF_EVT_REF_CTR_DEC_(e);         // decrement the reference counter
        }

        QF_CRIT_EXIT_();
    }
    return recalled;                                     // event not recalled
}

QP_END_


// "qa_fifo.cpp" =============================================================
/// \brief QActive::postFIFO() implementation.
///
/// \note this source file is only included in the QF library when the native
/// QF active object queue is used (instead of a message queue of an RTOS).

QP_BEGIN_

//............................................................................
#ifndef Q_SPY
void QActive::postFIFO(QEvt const * const e) {
#else
void QActive::postFIFO(QEvt const * const e, void const * const sender) {
#endif

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_POST_FIFO, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(sender);                                  // the sender object
        QS_SIG_(e->sig);                            // the signal of the event
        QS_OBJ_(this);                                   // this active object
        QS_U8_(QF_EVT_POOL_ID_(e));                // the pool Id of the event
        QS_U8_(QF_EVT_REF_CTR_(e));              // the ref count of the event
        QS_EQC_(m_eQueue.m_nFree);                   // number of free entries
        QS_EQC_(m_eQueue.m_nMin);                // min number of free entries
    QS_END_NOCRIT_()

    if (QF_EVT_POOL_ID_(e) != u8_0) {                // is it a dynamic event?
        QF_EVT_REF_CTR_INC_(e);             // increment the reference counter
    }

    if (m_eQueue.m_frontEvt == null_evt) {              // is the queue empty?
        m_eQueue.m_frontEvt = e;                     // deliver event directly
        QACTIVE_EQUEUE_SIGNAL_(this);                // signal the event queue
    }
    else {               // queue is not empty, leave event in the ring-buffer
                                        // queue must accept all posted events
        Q_ASSERT(m_eQueue.m_nFree != static_cast<QEQueueCtr>(0));
                              // insert event pointer e into the buffer (FIFO)
        QF_PTR_AT_(m_eQueue.m_ring, m_eQueue.m_head) = e;
        if (m_eQueue.m_head == static_cast<QEQueueCtr>(0)) {  // need to wrap?
            m_eQueue.m_head = m_eQueue.m_end;                   // wrap around
        }
        --m_eQueue.m_head;

        --m_eQueue.m_nFree;                    // update number of free events
        if (m_eQueue.m_nMin > m_eQueue.m_nFree) {
            m_eQueue.m_nMin = m_eQueue.m_nFree;       // update minimum so far
        }
    }
    QF_CRIT_EXIT_();
}

QP_END_

// "qa_get_.cpp" =============================================================
/// \brief QActive::get_() and QF::getQueueMargin() definitions.
///
/// \note this source file is only included in the QF library when the native
/// QF active object queue is used (instead of a message queue of an RTOS).

QP_BEGIN_

//............................................................................
QEvt const *QActive::get_(void) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QACTIVE_EQUEUE_WAIT_(this);           // wait for event to arrive directly

    QEvt const *e = m_eQueue.m_frontEvt;

    if (m_eQueue.m_nFree != m_eQueue.m_end) { //any events in the ring buffer?
                                                 // remove event from the tail
        m_eQueue.m_frontEvt = QF_PTR_AT_(m_eQueue.m_ring, m_eQueue.m_tail);
        if (m_eQueue.m_tail == static_cast<QEQueueCtr>(0)) {  // need to wrap?
            m_eQueue.m_tail = m_eQueue.m_end;                   // wrap around
        }
        --m_eQueue.m_tail;

        ++m_eQueue.m_nFree;          // one more free event in the ring buffer

        QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_GET, QS::aoObj_, this)
            QS_TIME_();                                           // timestamp
            QS_SIG_(e->sig);                       // the signal of this event
            QS_OBJ_(this);                               // this active object
            QS_U8_(QF_EVT_POOL_ID_(e));            // the pool Id of the event
            QS_U8_(QF_EVT_REF_CTR_(e));          // the ref count of the event
            QS_EQC_(m_eQueue.m_nFree);               // number of free entries
        QS_END_NOCRIT_()
    }
    else {
        m_eQueue.m_frontEvt = null_evt;             // the queue becomes empty
        QACTIVE_EQUEUE_ONEMPTY_(this);

        QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_GET_LAST, QS::aoObj_, this)
            QS_TIME_();                                           // timestamp
            QS_SIG_(e->sig);                       // the signal of this event
            QS_OBJ_(this);                               // this active object
            QS_U8_(QF_EVT_POOL_ID_(e));            // the pool Id of the event
            QS_U8_(QF_EVT_REF_CTR_(e));          // the ref count of the event
        QS_END_NOCRIT_()
    }
    QF_CRIT_EXIT_();
    return e;
}
//............................................................................
uint32_t QF::getQueueMargin(uint8_t const prio) {
    Q_REQUIRE((prio <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (active_[prio] != static_cast<QActive *>(0)));

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();
    uint32_t margin = static_cast<uint32_t>(active_[prio]->m_eQueue.m_nMin);
    QF_CRIT_EXIT_();

    return margin;
}

QP_END_

// "qa_lifo.cpp" =============================================================
/// \brief QActive::postLIFO() implementation.
///
/// \note this source file is only included in the QF library when the native
/// QF active object queue is used (instead of a message queue of an RTOS).

QP_BEGIN_

//............................................................................
void QActive::postLIFO(QEvt const * const e) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_POST_LIFO, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(e->sig);                           // the signal of this event
        QS_OBJ_(this);                                   // this active object
        QS_U8_(QF_EVT_POOL_ID_(e));                // the pool Id of the event
        QS_U8_(QF_EVT_REF_CTR_(e));              // the ref count of the event
        QS_EQC_(m_eQueue.m_nFree);                   // number of free entries
        QS_EQC_(m_eQueue.m_nMin);                // min number of free entries
    QS_END_NOCRIT_()

    if (QF_EVT_POOL_ID_(e) != u8_0) {                // is it a dynamic event?
        QF_EVT_REF_CTR_INC_(e);             // increment the reference counter
    }

    if (m_eQueue.m_frontEvt == null_evt) {              // is the queue empty?
        m_eQueue.m_frontEvt = e;                     // deliver event directly
        QACTIVE_EQUEUE_SIGNAL_(this);                // signal the event queue
    }
    else {               // queue is not empty, leave event in the ring-buffer
                                        // queue must accept all posted events
        Q_ASSERT(m_eQueue.m_nFree != static_cast<QEQueueCtr>(0));

        ++m_eQueue.m_tail;
        if (m_eQueue.m_tail == m_eQueue.m_end) {     // need to wrap the tail?
            m_eQueue.m_tail = static_cast<QEQueueCtr>(0);       // wrap around
        }

        QF_PTR_AT_(m_eQueue.m_ring, m_eQueue.m_tail) = m_eQueue.m_frontEvt;
        m_eQueue.m_frontEvt = e;                         // put event to front

        --m_eQueue.m_nFree;                    // update number of free events
        if (m_eQueue.m_nMin > m_eQueue.m_nFree) {
            m_eQueue.m_nMin = m_eQueue.m_nFree;       // update minimum so far
        }
    }
    QF_CRIT_EXIT_();
}

QP_END_

// "qa_sub.cpp" ==============================================================
/// \brief QActive::subscribe() implementation.

QP_BEGIN_

//............................................................................
void QActive::subscribe(enum_t const sig) const {
    uint8_t p = m_prio;
    Q_REQUIRE((Q_USER_SIG <= sig)
              && (sig < QF_maxSignal_)
              && (u8_0 < p) && (p <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (QF::active_[p] == this));

    uint8_t const i = Q_ROM_BYTE(QF_div8Lkup[p]);

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_SUBSCRIBE, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(sig);                              // the signal of this event
        QS_OBJ_(this);                                   // this active object
    QS_END_NOCRIT_()
                                                       // set the priority bit
    QF_PTR_AT_(QF_subscrList_, sig).m_bits[i] |= Q_ROM_BYTE(QF_pwr2Lkup[p]);
    QF_CRIT_EXIT_();
}

QP_END_

// "qa_usub.cpp" =============================================================
/// \brief QActive::unsubscribe() implementation.

QP_BEGIN_

//............................................................................
void QActive::unsubscribe(enum_t const sig) const {
    uint8_t p = m_prio;
    Q_REQUIRE((Q_USER_SIG <= sig)
              && (sig < QF_maxSignal_)
              && (u8_0 < p) && (p <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (QF::active_[p] == this));

    uint8_t const i = Q_ROM_BYTE(QF_div8Lkup[p]);

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_UNSUBSCRIBE, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(sig);                              // the signal of this event
        QS_OBJ_(this);                                   // this active object
    QS_END_NOCRIT_()
                                                     // clear the priority bit
    QF_PTR_AT_(QF_subscrList_,sig).m_bits[i] &= Q_ROM_BYTE(QF_invPwr2Lkup[p]);
    QF_CRIT_EXIT_();
}

QP_END_

// "qa_usuba.cpp" ============================================================
/// \brief QActive::unsubscribeAll() implementation.

QP_BEGIN_

//............................................................................
void QActive::unsubscribeAll(void) const {
    uint8_t const p = m_prio;
    Q_REQUIRE((u8_0 < p) && (p <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (QF::active_[p] == this));

    uint8_t const i = Q_ROM_BYTE(QF_div8Lkup[p]);

    enum_t sig;
    for (sig = Q_USER_SIG; sig < QF_maxSignal_; ++sig) {
        QF_CRIT_STAT_
        QF_CRIT_ENTRY_();
        if ((QF_PTR_AT_(QF_subscrList_, sig).m_bits[i]
             & Q_ROM_BYTE(QF_pwr2Lkup[p])) != u8_0)
        {

            QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_UNSUBSCRIBE, QS::aoObj_, this)
                QS_TIME_();                                       // timestamp
                QS_SIG_(sig);                      // the signal of this event
                QS_OBJ_(this);                           // this active object
            QS_END_NOCRIT_()
                                                     // clear the priority bit
            QF_PTR_AT_(QF_subscrList_, sig).m_bits[i] &=
                Q_ROM_BYTE(QF_invPwr2Lkup[p]);
        }
        QF_CRIT_EXIT_();
    }
}

QP_END_

// "qeq_fifo.cpp" ============================================================
/// \brief QEQueue::postFIFO() implementation.

QP_BEGIN_

//............................................................................
void QEQueue::postFIFO(QEvt const * const e) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_EQUEUE_POST_FIFO, QS::eqObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(e->sig);                           // the signal of this event
        QS_OBJ_(this);                                    // this queue object
        QS_U8_(QF_EVT_POOL_ID_(e));                // the pool Id of the event
        QS_U8_(QF_EVT_REF_CTR_(e));              // the ref count of the event
        QS_EQC_(m_nFree);                            // number of free entries
        QS_EQC_(m_nMin);                         // min number of free entries
    QS_END_NOCRIT_()

    if (QF_EVT_POOL_ID_(e) != u8_0) {                // is it a dynamic event?
        QF_EVT_REF_CTR_INC_(e);             // increment the reference counter
    }

    if (m_frontEvt == null_evt) {                       // is the queue empty?
        m_frontEvt = e;                              // deliver event directly
    }
    else {               // queue is not empty, leave event in the ring-buffer
               // the queue must be able to accept the event (cannot overflow)
        Q_ASSERT(m_nFree != static_cast<QEQueueCtr>(0));

        QF_PTR_AT_(m_ring, m_head) = e; // insert event into the buffer (FIFO)
        if (m_head == static_cast<QEQueueCtr>(0)) {           // need to wrap?
            m_head = m_end;                                     // wrap around
        }
        --m_head;

        --m_nFree;                             // update number of free events
        if (m_nMin > m_nFree) {
            m_nMin = m_nFree;                         // update minimum so far
        }
    }
    QF_CRIT_EXIT_();
}

QP_END_

// "qeq_get.cpp" =============================================================
/// \brief QEQueue::get() implementation.

QP_BEGIN_

//............................................................................
QEvt const *QEQueue::get(void) {
    QEvt const *e;
    QF_CRIT_STAT_

    QF_CRIT_ENTRY_();
    if (m_frontEvt == null_evt) {                       // is the queue empty?
        e = null_evt;                       // no event available at this time
    }
    else {
        e = m_frontEvt;

        if (m_nFree != m_end) {          // any events in the the ring buffer?
            m_frontEvt = QF_PTR_AT_(m_ring, m_tail);   // remove from the tail
            if (m_tail == static_cast<QEQueueCtr>(0)) {       // need to wrap?
                m_tail = m_end;                                 // wrap around
            }
            --m_tail;

            ++m_nFree;               // one more free event in the ring buffer

            QS_BEGIN_NOCRIT_(QS_QF_EQUEUE_GET, QS::eqObj_, this)
                QS_TIME_();                                       // timestamp
                QS_SIG_(e->sig);                   // the signal of this event
                QS_OBJ_(this);                            // this queue object
                QS_U8_(QF_EVT_POOL_ID_(e));        // the pool Id of the event
                QS_U8_(QF_EVT_REF_CTR_(e));      // the ref count of the event
                QS_EQC_(m_nFree);                    // number of free entries
            QS_END_NOCRIT_()
        }
        else {
            m_frontEvt = null_evt;                  // the queue becomes empty

            QS_BEGIN_NOCRIT_(QS_QF_EQUEUE_GET_LAST, QS::eqObj_, this)
                QS_TIME_();                                       // timestamp
                QS_SIG_(e->sig);                   // the signal of this event
                QS_OBJ_(this);                            // this queue object
                QS_U8_(QF_EVT_POOL_ID_(e));        // the pool Id of the event
                QS_U8_(QF_EVT_REF_CTR_(e));      // the ref count of the event
            QS_END_NOCRIT_()
        }
    }
    QF_CRIT_EXIT_();
    return e;
}

QP_END_

// "qeq_init.cpp" ============================================================
/// \brief QEQueue::init() implementation.

QP_BEGIN_

//............................................................................
void QEQueue::init(QEvt const *qSto[], QEQueueCtr const qLen) {
    m_frontEvt = null_evt;                           // no events in the queue
    m_ring     = &qSto[0];
    m_end      = qLen;
    m_head     = static_cast<QEQueueCtr>(0);
    m_tail     = static_cast<QEQueueCtr>(0);
    m_nFree    = qLen;
    m_nMin     = qLen;

    QS_CRIT_STAT_
    QS_BEGIN_(QS_QF_EQUEUE_INIT, QS::eqObj_, this)
        QS_OBJ_(qSto);                                  // this QEQueue object
        QS_EQC_(qLen);                              // the length of the queue
    QS_END_()
}

QP_END_

// "qeq_lifo.cpp" ============================================================
/// \brief QEQueue::postLIFO() implementation.

QP_BEGIN_

//............................................................................
void QEQueue::postLIFO(QEvt const * const e) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_EQUEUE_POST_LIFO, QS::eqObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(e->sig);                           // the signal of this event
        QS_OBJ_(this);                                    // this queue object
        QS_U8_(QF_EVT_POOL_ID_(e));                // the pool Id of the event
        QS_U8_(QF_EVT_REF_CTR_(e));              // the ref count of the event
        QS_EQC_(m_nFree);                            // number of free entries
        QS_EQC_(m_nMin);                         // min number of free entries
    QS_END_NOCRIT_()

    if (QF_EVT_POOL_ID_(e) != u8_0) {                // is it a dynamic event?
        QF_EVT_REF_CTR_INC_(e);             // increment the reference counter
    }

    if (m_frontEvt != null_evt) {                   // is the queue not empty?
               // the queue must be able to accept the event (cannot overflow)
        Q_ASSERT(m_nFree != static_cast<QEQueueCtr>(0));

        ++m_tail;
        if (m_tail == m_end) {                       // need to wrap the tail?
            m_tail = static_cast<QEQueueCtr>(0);                // wrap around
        }

        QF_PTR_AT_(m_ring, m_tail) = m_frontEvt;   // buffer the old front evt

        --m_nFree;                             // update number of free events
        if (m_nMin > m_nFree) {
            m_nMin = m_nFree;                         // update minimum so far
        }
    }

    m_frontEvt = e;                        // stick the new event to the front

    QF_CRIT_EXIT_();
}

QP_END_

// "qf_act.cpp" ==============================================================
/// \brief QF::active_[], QF::getVersion(), and QF::add_()/QF::remove_()
/// implementation.

QP_BEGIN_

// public objects ------------------------------------------------------------
QActive *QF::active_[QF_MAX_ACTIVE + 1];        // to be used by QF ports only
uint8_t QF_intLockNest_;                       // interrupt-lock nesting level

//............................................................................
char_t const Q_ROM * Q_ROM_VAR QF::getVersion(void) {
    uint8_t const u8_zero = static_cast<uint8_t>('0');
    static char_t const Q_ROM Q_ROM_VAR version[] = {
        static_cast<char_t>(((QP_VERSION >> 12) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  8) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  4) & 0xFU) + u8_zero),
        static_cast<char_t>((QP_VERSION         & 0xFU) + u8_zero),
        static_cast<char_t>('\0')
    };
    return version;
}
//............................................................................
void QF::add_(QActive * const a) {
    uint8_t p = a->m_prio;

    Q_REQUIRE((u8_0 < p) && (p <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (active_[p] == static_cast<QActive *>(0)));

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    active_[p] = a;            // registger the active object at this priority

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_ADD, QS::aoObj_, a)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(a);                                       // the active object
        QS_U8_(p);                        // the priority of the active object
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
}
//............................................................................
void QF::remove_(QActive const * const a) {
    uint8_t p = a->m_prio;

    Q_REQUIRE((u8_0 < p) && (p <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (active_[p] == a));

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    active_[p] = static_cast<QActive *>(0);      // free-up the priority level

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_REMOVE, QS::aoObj_, a)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(a);                                       // the active object
        QS_U8_(p);                        // the priority of the active object
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
}

QP_END_


// "qf_gc.cpp" ===============================================================
/// \brief QF::gc() implementation.

QP_BEGIN_

//............................................................................
void QF::gc(QEvt const * const e) {
    if (QF_EVT_POOL_ID_(e) != u8_0) {                // is it a dynamic event?
        QF_CRIT_STAT_
        QF_CRIT_ENTRY_();

        if (QF_EVT_REF_CTR_(e) > u8_1) {     // isn't this the last reference?
            QF_EVT_REF_CTR_DEC_(e);               // decrement the ref counter

            QS_BEGIN_NOCRIT_(QS_QF_GC_ATTEMPT, null_void, null_void)
                QS_TIME_();                                       // timestamp
                QS_SIG_(e->sig);                    // the signal of the event
                QS_U8_(QF_EVT_POOL_ID_(e));        // the pool Id of the event
                QS_U8_(QF_EVT_REF_CTR_(e));      // the ref count of the event
            QS_END_NOCRIT_()

            QF_CRIT_EXIT_();
        }
        else {         // this is the last reference to this event, recycle it
            uint8_t idx = static_cast<uint8_t>(QF_EVT_POOL_ID_(e) - u8_1);

            QS_BEGIN_NOCRIT_(QS_QF_GC, null_void, null_void)
                QS_TIME_();                                       // timestamp
                QS_SIG_(e->sig);                    // the signal of the event
                QS_U8_(QF_EVT_POOL_ID_(e));        // the pool Id of the event
                QS_U8_(QF_EVT_REF_CTR_(e));      // the ref count of the event
            QS_END_NOCRIT_()

            QF_CRIT_EXIT_();

            Q_ASSERT(idx < QF_maxPool_);

#ifdef Q_EVT_VIRTUAL
            QF_EVT_CONST_CAST_(e)->~QEvt();        // xtor, cast 'const' away,
                             // which is legitimate, because it's a pool event
#endif
                  // cast 'const' away, which is OK, because it's a pool event
            QF_EPOOL_PUT_(QF_pool_[idx], QF_EVT_CONST_CAST_(e));
        }
    }
}

QP_END_

// "qf_log2.cpp" =============================================================
/// \brief QF_log2Lkup[] implementation.

QP_BEGIN_

// Global objects ------------------------------------------------------------
uint8_t const Q_ROM Q_ROM_VAR QF_log2Lkup[256] = {
    static_cast<uint8_t>(0),
    static_cast<uint8_t>(1),
    static_cast<uint8_t>(2), static_cast<uint8_t>(2),
    static_cast<uint8_t>(3), static_cast<uint8_t>(3), static_cast<uint8_t>(3),
    static_cast<uint8_t>(3),
    static_cast<uint8_t>(4), static_cast<uint8_t>(4), static_cast<uint8_t>(4),
    static_cast<uint8_t>(4), static_cast<uint8_t>(4), static_cast<uint8_t>(4),
    static_cast<uint8_t>(4), static_cast<uint8_t>(4),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8), static_cast<uint8_t>(8),
    static_cast<uint8_t>(8), static_cast<uint8_t>(8)
};

QP_END_

// "qf_new.cpp" ==============================================================
/// \brief QF::new_() implementation.

QP_BEGIN_

//............................................................................
QEvt *QF::new_(QEvtSize const evtSize, enum_t const sig) {
                    // find the pool id that fits the requested event size ...
    uint8_t idx = u8_0;
    while (evtSize
           > static_cast<QEvtSize>(QF_EPOOL_EVENT_SIZE_(QF_pool_[idx])))
    {
        ++idx;
        Q_ASSERT(idx < QF_maxPool_);     // cannot run out of registered pools
    }

    QS_CRIT_STAT_
    QS_BEGIN_(QS_QF_NEW, null_void, null_void)
        QS_TIME_();                                               // timestamp
        QS_EVS_(evtSize);                             // the size of the event
        QS_SIG_(static_cast<QSignal>(sig));         // the signal of the event
    QS_END_()

    QEvt *e;
    QF_EPOOL_GET_(QF_pool_[idx], e);
    Q_ASSERT(e != static_cast<QEvt *>(0));// pool must not run out of events

    e->sig     = static_cast<QSignal>(sig);       // set signal for this event
    e->poolId_ = static_cast<uint8_t>(idx + u8_1); // store pool ID in the evt
    e->refCtr_ = u8_0;                       // set the reference counter to 0

    return e;
}

QP_END_

// "qf_pool.cpp" =============================================================
/// \brief QF_pool_[] and QF_maxPool_ definition, QF::poolInit()
/// implementation.

QP_BEGIN_

// Package-scope objects -----------------------------------------------------
QF_EPOOL_TYPE_ QF_pool_[QF_MAX_EPOOL];             // allocate the event pools
uint8_t QF_maxPool_;                      // number of initialized event pools

//............................................................................
void QF::poolInit(void * const poolSto,
                  uint32_t const poolSize, uint32_t const evtSize)
{
                         // cannot exceed the number of available memory pools
    Q_REQUIRE(QF_maxPool_ < static_cast<uint8_t>(Q_DIM(QF_pool_)));
               // please initialize event pools in ascending order of evtSize:
    Q_REQUIRE((QF_maxPool_ == u8_0)
              || (QF_EPOOL_EVENT_SIZE_(QF_pool_[QF_maxPool_ - u8_1])
                  < evtSize));
    QF_EPOOL_INIT_(QF_pool_[QF_maxPool_], poolSto, poolSize, evtSize);
    ++QF_maxPool_;                                            // one more pool
}

QP_END_


// "qf_psini.cpp" ============================================================
/// \brief QF_subscrList_ and QF_maxSignal_ definition, QF::psInit()
/// implementation.

QP_BEGIN_

// Package-scope objects -----------------------------------------------------
QSubscrList *QF_subscrList_;
enum_t QF_maxSignal_;

//............................................................................
void QF::psInit(QSubscrList * const subscrSto, uint32_t const maxSignal) {
    QF_subscrList_ = subscrSto;
    QF_maxSignal_  = static_cast<enum_t>(maxSignal);
}

QP_END_

// "qf_pspub.cpp" ============================================================
/// \brief QF::publish() implementation.

QP_BEGIN_

//............................................................................
#ifndef Q_SPY
void QF::publish(QEvt const * const e) {
#else
void QF::publish(QEvt const * const e, void const * const sender) {
#endif
         // make sure that the published signal is within the configured range
    Q_REQUIRE(static_cast<enum_t>(e->sig) < QF_maxSignal_);

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_PUBLISH, null_void, null_void)
        QS_TIME_();                                           // the timestamp
        QS_OBJ_(sender);                                  // the sender object
        QS_SIG_(e->sig);                            // the signal of the event
        QS_U8_(QF_EVT_POOL_ID_(e));                // the pool Id of the event
        QS_U8_(QF_EVT_REF_CTR_(e));              // the ref count of the event
    QS_END_NOCRIT_()

    if (QF_EVT_POOL_ID_(e) != u8_0) {                // is it a dynamic event?
        QF_EVT_REF_CTR_INC_(e);     // increment the reference counter, NOTE01
    }
    QF_CRIT_EXIT_();

#if (QF_MAX_ACTIVE <= 8)
    uint8_t tmp = QF_PTR_AT_(QF_subscrList_, e->sig).m_bits[0];
    while (tmp != u8_0) {
        uint8_t p = Q_ROM_BYTE(QF_log2Lkup[tmp]);
        tmp &= Q_ROM_BYTE(QF_invPwr2Lkup[p]);      // clear the subscriber bit
        Q_ASSERT(active_[p] != static_cast<QActive *>(0));//must be registered

                           // POST() asserts internally if the queue overflows
        active_[p]->POST(e, sender);
    }
#else
                                                // number of bytes in the list
    uint8_t i = QF_SUBSCR_LIST_SIZE;
    do {                       // go through all bytes in the subsciption list
        --i;
        uint8_t tmp = QF_PTR_AT_(QF_subscrList_, e->sig).m_bits[i];
        while (tmp != u8_0) {
            uint8_t p = Q_ROM_BYTE(QF_log2Lkup[tmp]);
            tmp &= Q_ROM_BYTE(QF_invPwr2Lkup[p]);  // clear the subscriber bit
                                                        // adjust the priority
            p = static_cast<uint8_t>(p + static_cast<uint8_t>(i << 3));
            Q_ASSERT(active_[p] != static_cast<QActive *>(0));   // registered

                           // POST() asserts internally if the queue overflows
            active_[p]->POST(e, sender);
        }
    } while (i != u8_0);
#endif

    gc(e);                            // run the garbage collector, see NOTE01
}

QP_END_


// "qf_pwr2.cpp" =============================================================
/// \brief QF_pwr2Lkup[], QF_invPwr2Lkup[], and QF_div8Lkup[] definitions.

QP_BEGIN_

// Global objects ------------------------------------------------------------
uint8_t const Q_ROM Q_ROM_VAR QF_pwr2Lkup[65] = {
    static_cast<uint8_t>(0x00),                             // unused location
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80),
    static_cast<uint8_t>(0x01), static_cast<uint8_t>(0x02),
    static_cast<uint8_t>(0x04), static_cast<uint8_t>(0x08),
    static_cast<uint8_t>(0x10), static_cast<uint8_t>(0x20),
    static_cast<uint8_t>(0x40), static_cast<uint8_t>(0x80)
};

uint8_t const Q_ROM Q_ROM_VAR QF_invPwr2Lkup[65] = {
    static_cast<uint8_t>(0xFF),                             // unused location
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F),
    static_cast<uint8_t>(0xFE), static_cast<uint8_t>(0xFD),
    static_cast<uint8_t>(0xFB), static_cast<uint8_t>(0xF7),
    static_cast<uint8_t>(0xEF), static_cast<uint8_t>(0xDF),
    static_cast<uint8_t>(0xBF), static_cast<uint8_t>(0x7F)
};

uint8_t const Q_ROM Q_ROM_VAR QF_div8Lkup[65] = {
    static_cast<uint8_t>(0),                                // unused location
    static_cast<uint8_t>(0), static_cast<uint8_t>(0), static_cast<uint8_t>(0),
    static_cast<uint8_t>(0), static_cast<uint8_t>(0), static_cast<uint8_t>(0),
    static_cast<uint8_t>(0), static_cast<uint8_t>(0),
    static_cast<uint8_t>(1), static_cast<uint8_t>(1), static_cast<uint8_t>(1),
    static_cast<uint8_t>(1), static_cast<uint8_t>(1), static_cast<uint8_t>(1),
    static_cast<uint8_t>(1), static_cast<uint8_t>(1),
    static_cast<uint8_t>(2), static_cast<uint8_t>(2), static_cast<uint8_t>(2),
    static_cast<uint8_t>(2), static_cast<uint8_t>(2), static_cast<uint8_t>(2),
    static_cast<uint8_t>(2), static_cast<uint8_t>(2),
    static_cast<uint8_t>(3), static_cast<uint8_t>(3), static_cast<uint8_t>(3),
    static_cast<uint8_t>(3), static_cast<uint8_t>(3), static_cast<uint8_t>(3),
    static_cast<uint8_t>(3), static_cast<uint8_t>(3),
    static_cast<uint8_t>(4), static_cast<uint8_t>(4), static_cast<uint8_t>(4),
    static_cast<uint8_t>(4), static_cast<uint8_t>(4), static_cast<uint8_t>(4),
    static_cast<uint8_t>(4), static_cast<uint8_t>(4),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(5), static_cast<uint8_t>(5),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(6), static_cast<uint8_t>(6),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7), static_cast<uint8_t>(7),
    static_cast<uint8_t>(7), static_cast<uint8_t>(7)
};

QP_END_

// "qf_tick.cpp" =============================================================
/// \brief QF::tick() implementation.

QP_BEGIN_

//............................................................................
#ifndef Q_SPY
void QF::tick(void) {                                            // see NOTE01
#else
void QF::tick(void const * const sender) {
#endif

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_TICK, null_void, null_void)
        QS_TEC_(static_cast<QTimeEvtCtr>(++QS::tickCtr_)); // the tick counter
    QS_END_NOCRIT_()

    QTimeEvt *prev = null_tevt;
    for (QTimeEvt *t = QF_timeEvtListHead_; t != null_tevt; t = t->m_next) {
        if (t->m_ctr == tc_0) {            // time evt. scheduled for removal?
            if (t == QF_timeEvtListHead_) {
                QF_timeEvtListHead_ = t->m_next;
            }
            else {
                Q_ASSERT(prev != null_tevt);
                prev->m_next = t->m_next;
            }
            QF_EVT_REF_CTR_DEC_(t);                      // mark as not linked
        }
        else {
            --t->m_ctr;
            if (t->m_ctr == tc_0) {                        // about to expire?
                if (t->m_interval != tc_0) {           // periodic time event?
                    t->m_ctr = t->m_interval;            // rearm the time evt
                }
                else {
                    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_AUTO_DISARM, QS::teObj_, t)
                        QS_OBJ_(t);                  // this time event object
                        QS_OBJ_(t->m_act);                // the active object
                    QS_END_NOCRIT_()
                }

                QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_POST, QS::teObj_, t)
                    QS_TIME_();                                   // timestamp
                    QS_OBJ_(t);                       // the time event object
                    QS_SIG_(t->sig);          // the signal of this time event
                    QS_OBJ_(t->m_act);                    // the active object
                QS_END_NOCRIT_()

                QF_CRIT_EXIT_();         // leave crit. section before posting
                           // POST() asserts internally if the queue overflows
                t->m_act->POST(t, sender);
                QF_CRIT_ENTRY_();        // re-enter crit. section to continue

                if (t->m_ctr == tc_0) {             // still marked to expire?
                    if (t == QF_timeEvtListHead_) {
                        QF_timeEvtListHead_ = t->m_next;
                    }
                    else {
                        Q_ASSERT(prev != null_tevt);
                        prev->m_next = t->m_next;
                    }
                    QF_EVT_REF_CTR_DEC_(t);                 // mark as removed
                }
                else {
                    prev = t;
                }
            }
            else {
                prev = t;
            }
        }
    }
    QF_CRIT_EXIT_();
}

QP_END_

// "qmp_get.cpp" =============================================================
/// \brief QMPool::get() and QF::getPoolMargin() implementation.

QP_BEGIN_

//............................................................................
void *QMPool::get(void) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QFreeBlock *fb = static_cast<QFreeBlock *>(m_free);  // free block or NULL
    if (fb != static_cast<QFreeBlock *>(0)) {         // free block available?
        m_free = fb->m_next;        // adjust list head to the next free block

        Q_ASSERT(m_nFree > static_cast<QMPoolCtr>(0));    // at least one free
        --m_nFree;                                      // one free block less
        if (m_nMin > m_nFree) {
            m_nMin = m_nFree;                   // remember the minimum so far
        }
    }

    QS_BEGIN_NOCRIT_(QS_QF_MPOOL_GET, QS::mpObj_, m_start)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(m_start);                   // the memory managed by this pool
        QS_MPC_(m_nFree);             // the number of free blocks in the pool
        QS_MPC_(m_nMin);     // the mninimum number of free blocks in the pool
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
    return fb;               // return the block or NULL pointer to the caller
}
//............................................................................
uint32_t QF::getPoolMargin(uint8_t const poolId) {
    Q_REQUIRE((u8_1 <= poolId) && (poolId <= QF_maxPool_));

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();
    uint32_t margin = static_cast<uint32_t>(QF_pool_[poolId - u8_1].m_nMin);
    QF_CRIT_EXIT_();

    return margin;
}

QP_END_

// "qmp_init.cpp" ============================================================
/// \brief QMPool::init() implementation.

QP_BEGIN_

//............................................................................
void QMPool::init(void * const poolSto, uint32_t const poolSize,
                  QMPoolSize const blockSize)
{
    // The memory block must be valid
    // and the poolSize must fit at least one free block
    // and the blockSize must not be too close to the top of the dynamic range
    Q_REQUIRE((poolSto != null_void)
              && (poolSize >= static_cast<uint32_t>(sizeof(QFreeBlock)))
              && ((blockSize + static_cast<QMPoolSize>(sizeof(QFreeBlock)))
                    > blockSize));

    m_free = poolSto;

                // round up the blockSize to fit an integer number of pointers
    m_blockSize = static_cast<QMPoolSize>(sizeof(QFreeBlock));//start with one
    uint32_t nblocks = static_cast<uint32_t>(1);//# free blocks in a mem block
    while (m_blockSize < blockSize) {
        m_blockSize += static_cast<QMPoolSize>(sizeof(QFreeBlock));
        ++nblocks;
    }

               // the whole pool buffer must fit at least one rounded-up block
    Q_ASSERT(poolSize >= static_cast<uint32_t>(m_blockSize));

                                // chain all blocks together in a free-list...
                                                     // don't chain last block
    uint32_t availSize = poolSize - static_cast<uint32_t>(m_blockSize);
    m_nTot = static_cast<QMPoolCtr>(1);    // one (the last) block in the pool
                                          //start at the head of the free list
    QFreeBlock *fb = static_cast<QFreeBlock *>(m_free);
    while (availSize >= static_cast<uint32_t>(m_blockSize)) {
        fb->m_next = &QF_PTR_AT_(fb, nblocks);          // setup the next link
        fb = fb->m_next;                              // advance to next block
        availSize -= static_cast<uint32_t>(m_blockSize);// less available size
        ++m_nTot;                     // increment the number of blocks so far
    }

    fb->m_next = static_cast<QFreeBlock *>(0); // the last link points to NULL
    m_nFree    = m_nTot;                                // all blocks are free
    m_nMin     = m_nTot;                  // the minimum number of free blocks
    m_start    = poolSto;               // the original start this pool buffer
    m_end      = fb;                            // the last block in this pool

    QS_CRIT_STAT_
    QS_BEGIN_(QS_QF_MPOOL_INIT, QS::mpObj_, m_start)
        QS_OBJ_(m_start);                   // the memory managed by this pool
        QS_MPC_(m_nTot);                         // the total number of blocks
    QS_END_()
}

QP_END_

// "qmp_put.cpp" =============================================================
/// \brief QMPool::put() implementation.

QP_BEGIN_

// This macro is specifically and exclusively used for checking the range
// of a block pointer returned to the pool. Such a check must rely on the
// pointer arithmetic not compliant with the MISRA-C++:2008 rules ??? and
// ???. Defining a specific macro for this purpose allows to selectively
// disable the warnings for this particular case.
//
#define QF_PTR_RANGE_(x_, min_, max_)  (((min_) <= (x_)) && ((x_) <= (max_)))

//............................................................................
void QMPool::put(void * const b) {

    Q_REQUIRE(m_nFree < m_nTot);         // adding one free so, # free < total
    Q_REQUIRE(QF_PTR_RANGE_(b, m_start, m_end));         // b must be in range

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

                                                    // link into the free list
    (static_cast<QFreeBlock*>(b))->m_next = static_cast<QFreeBlock *>(m_free);
    m_free = b;                            // set as new head of the free list
    ++m_nFree;                             // one more free block in this pool

    QS_BEGIN_NOCRIT_(QS_QF_MPOOL_PUT, QS::mpObj_, m_start)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(m_start);                   // the memory managed by this pool
        QS_MPC_(m_nFree);             // the number of free blocks in the pool
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
}

QP_END_


// "qte_arm.cpp" =============================================================
/// \brief QF_timeEvtListHead_ definition and QTimeEvt::arm_() implementation.

QP_BEGIN_

// Package-scope objects -----------------------------------------------------
QTimeEvt *QF_timeEvtListHead_;           // head of linked list of time events

//............................................................................
bool QF::noTimeEvtsActive(void) {
    return QF_timeEvtListHead_ == null_tevt;
}
//............................................................................
void QTimeEvt::arm_(QActive * const act, QTimeEvtCtr const nTicks) {
    Q_REQUIRE((nTicks > tc_0)                    /* cannot arm with 0 ticks */
              && (act != null_act)        /* Active object must be provided */
              && (m_ctr == tc_0)                        /* must be disarmed */
              && (static_cast<enum_t>(sig) >= Q_USER_SIG));    // valid signal
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();
    m_ctr  = nTicks;
    m_act  = act;
    if (refCtr_ == u8_0) {                                      // not linked?
        m_next = QF_timeEvtListHead_;
        QF_timeEvtListHead_ = this;
        QF_EVT_REF_CTR_INC_(this);                           // mark as linked
    }

    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_ARM, QS::teObj_, this)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(this);                               // this time event object
        QS_OBJ_(act);                                     // the active object
        QS_TEC_(nTicks);                                // the number of ticks
        QS_TEC_(m_interval);                                   // the interval
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
}

QP_END_


// "qte_ctor.cpp" ============================================================
/// \brief QTimeEvt::QTimeEvt() implementation.
QP_BEGIN_

//............................................................................
QTimeEvt::QTimeEvt(enum_t const s)
  :
#ifdef Q_EVT_CTOR
    QEvt(static_cast<QSignal>(s)),
#endif
    m_next(null_tevt),
    m_act(null_act),
    m_ctr(tc_0),
    m_interval(tc_0)
{
    Q_REQUIRE(s >= Q_USER_SIG);                        // signal must be valid
#ifndef Q_EVT_CTOR
    sig = static_cast<QSignal>(s);
#endif
                                      // time event must be static, see NOTE01
    poolId_ = u8_0;                                 // not from any event pool
    refCtr_ = u8_0;                                              // not linked
}

QP_END_


// "qte_ctr.cpp" =============================================================
/// \brief QTimeEvt::ctr() implementation.

QP_BEGIN_

//............................................................................
QTimeEvtCtr QTimeEvt::ctr(void) const {
    QF_CRIT_STAT_

    QF_CRIT_ENTRY_();
    QTimeEvtCtr ret = m_ctr;

    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_CTR, QS::teObj_, this)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(this);                               // this time event object
        QS_OBJ_(m_act);                                   // the active object
        QS_TEC_(ret);                                   // the current counter
        QS_TEC_(m_interval);                                   // the interval
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
    return ret;
}

QP_END_

// "qte_darm.cpp" ============================================================
/// \brief QTimeEvt::disarm() implementation.

QP_BEGIN_

//............................................................................
// NOTE: disarm a time evt (no harm in disarming an already disarmed time evt)
bool QTimeEvt::disarm(void) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();
    bool wasArmed;
    if (m_ctr != tc_0) {                  // is the time event actually armed?
        wasArmed = true;

        QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_DISARM, QS::teObj_, this)
            QS_TIME_();                                           // timestamp
            QS_OBJ_(this);                           // this time event object
            QS_OBJ_(m_act);                               // the active object
            QS_TEC_(m_ctr);                             // the number of ticks
            QS_TEC_(m_interval);                               // the interval
        QS_END_NOCRIT_()

        m_ctr = tc_0;                        // schedule removal from the list
    }
    else {                                     // the time event was not armed
        wasArmed = false;

        QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_DISARM_ATTEMPT, QS::teObj_, this)
            QS_TIME_();                                           // timestamp
            QS_OBJ_(this);                           // this time event object
            QS_OBJ_(m_act);                               // the active object
        QS_END_NOCRIT_()
    }
    QF_CRIT_EXIT_();
    return wasArmed;
}

QP_END_


// "qte_rarm.cpp" ============================================================
/// \brief QTimeEvt::rearm() implementation.

QP_BEGIN_

//............................................................................
bool QTimeEvt::rearm(QTimeEvtCtr const nTicks) {
    Q_REQUIRE((nTicks > tc_0)                  /* cannot rearm with 0 ticks */
              && (m_act != null_act)         /* active object must be valid */
              && (static_cast<enum_t>(sig) >= Q_USER_SIG));    // valid signal

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    bool isArmed;
    if (m_ctr == tc_0) {                       // is this time event disarmed?
        isArmed = false;
        m_next = QF_timeEvtListHead_;
        if (QF_timeEvtListHead_ != null_tevt) {
            m_next = QF_timeEvtListHead_;
            QF_timeEvtListHead_ = this;
            QF_EVT_REF_CTR_INC_(this);                       // mark as linked
        }
    }
    else {
        isArmed = true;
    }
    m_ctr = nTicks;            // re-load the tick counter (shift the phasing)

    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_REARM, QS::teObj_, this)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(this);                               // this time event object
        QS_OBJ_(m_act);                                   // the active object
        QS_TEC_(m_ctr);                                 // the number of ticks
        QS_TEC_(m_interval);                                   // the interval
        QS_U8_(isArmed ? u8_1 : u8_0);                 // was the timer armed?
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
    return isArmed;
}

QP_END_


//////////////////////////////////////////////////////////////////////////////
// Kernel selection based on QK_PREEMPTIVE
//
#ifdef QK_PREEMPTIVE

// "qk_pkg.h" ================================================================
/// \brief Internal (package scope) QK/C interface.

#ifndef QF_CRIT_STAT_TYPE
    /// \brief This is an internal macro for defining the critical section
    /// status type.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// provides the definition of the critical section status varaible.
    /// Otherwise this macro is empty.
    /// \sa #QF_CRIT_STAT_TYPE
    #define QF_CRIT_STAT_

    /// \brief This is an internal macro for entering a critical section.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// invokes #QF_CRIT_ENTRY passing the key variable as the parameter.
    /// Otherwise #QF_CRIT_ENTRY is invoked with a dummy parameter.
    /// \sa #QF_CRIT_ENTRY
    #define QF_CRIT_ENTRY_()    QF_CRIT_ENTRY(dummy)

    /// \brief This is an internal macro for exiting a cricial section.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// invokes #QF_CRIT_EXIT passing the key variable as the parameter.
    /// Otherwise #QF_CRIT_EXIT is invoked with a dummy parameter.
    /// \sa #QF_CRIT_EXIT
    #define QF_CRIT_EXIT_()     QF_CRIT_EXIT(dummy)

#else
    #define QF_CRIT_STAT_       QF_CRIT_STAT_TYPE critStat_;
    #define QF_CRIT_ENTRY_()    QF_CRIT_ENTRY(critStat_)
    #define QF_CRIT_EXIT_()     QF_CRIT_EXIT(critStat_)
#endif
                             // define for backwards compatibility, see NOTE01
#ifndef QF_CRIT_STAT_TYPE
#if !defined(QF_INT_DISABLE) && defined(QF_CRIT_ENTRY)
    #define QF_INT_DISABLE()    QF_CRIT_ENTRY(dummy)
#endif
#if !defined(QF_INT_ENABLE) && defined(QF_CRIT_EXIT)
    #define QF_INT_ENABLE()     QF_CRIT_EXIT(dummy)
#endif
#endif                                                    // QF_CRIT_STAT_TYPE

                                                   // package-scope objects...
#ifndef QK_NO_MUTEX
    extern "C" uint8_t QK_ceilingPrio_;         ///< QK mutex priority ceiling
#endif

// "qk.cpp" ==================================================================
/// \brief QK_readySet_, QK_currPrio_, and QK_intNest_ definitions and
/// QK::getVersion(), QF::init(), QF::run(), QF::stop(),
/// QActive::start(), QActive::stop() implementations.

// Public-scope objects ------------------------------------------------------
extern "C" {
#if (QF_MAX_ACTIVE <= 8)
    QP_ QPSet8  QK_readySet_;                               // ready set of QK
#else
    QP_ QPSet64 QK_readySet_;                               // ready set of QK
#endif
                                         // start with the QK scheduler locked
uint8_t QK_currPrio_ = static_cast<uint8_t>(QF_MAX_ACTIVE + 1);
uint8_t QK_intNest_;                          // start with nesting level of 0

}                                                                // extern "C"

QP_BEGIN_

//............................................................................
char_t const Q_ROM * Q_ROM_VAR QK::getVersion(void) {
    uint8_t const u8_zero = static_cast<uint8_t>('0');
    static char_t const Q_ROM Q_ROM_VAR version[] = {
        static_cast<char_t>(((QP_VERSION >> 12) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  8) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  4) & 0xFU) + u8_zero),
        static_cast<char_t>((QP_VERSION         & 0xFU) + u8_zero),
        static_cast<char_t>('\0')
    };
    return version;
}
//............................................................................
void QF::init(void) {
    QK_init();           // QK initialization ("C" linkage, might be assembly)
}
//............................................................................
void QF::stop(void) {
    QF::onCleanup();                                       // cleanup callback
    // nothing else to do for the QK preemptive kernel
}
//............................................................................
static void initialize(void) {
    QK_currPrio_ = static_cast<uint8_t>(0);   // priority for the QK idle loop
    uint8_t p = QK_schedPrio_();
    if (p != static_cast<uint8_t>(0)) {
        QK_sched_(p);                    // process all events produced so far
    }
}
//............................................................................
int16_t QF::run(void) {
    QF_INT_DISABLE();
    initialize();
    onStartup();                                           // startup callback
    QF_INT_ENABLE();

    for (;;) {                                             // the QK idle loop
        QK::onIdle();                        // invoke the QK on-idle callback
    }
                      // this unreachable return is to make the compiler happy
    return static_cast<int16_t>(0);
}
//............................................................................
void QActive::start(uint8_t const prio,
                   QEvt const *qSto[], uint32_t const qLen,
                   void * const stkSto, uint32_t const stkSize,
                   QEvt const * const ie)
{
    Q_REQUIRE((static_cast<uint8_t>(0) < prio)
              && (prio <= static_cast<uint8_t>(QF_MAX_ACTIVE)));

    m_eQueue.init(qSto, static_cast<QEQueueCtr>(qLen));    // initialize queue
    m_prio = prio;
    QF::add_(this);                     // make QF aware of this active object

#if defined(QK_TLS) || defined(QK_EXT_SAVE)
    // in the QK port the parameter stkSize is used as the thread flags
    m_osObject = static_cast<uint8_t>(stkSize);   // m_osObject contains flags

    // in the QK port the parameter stkSto is used as the thread-local-storage
    m_thread   = stkSto;   // contains the pointer to the thread-local-storage
#else
    Q_ASSERT((stkSto == static_cast<void *>(0))
             && (stkSize == static_cast<uint32_t>(0)));
#endif

    init(ie);                                    // execute initial transition

    QS_FLUSH();                          // flush the trace buffer to the host
}
//............................................................................
void QActive::stop(void) {
    QF::remove_(this);                // remove this active object from the QF
}

QP_END_

// "qk_sched" ================================================================
/// \brief QK_schedPrio_() and QK_sched_() implementation.

//............................................................................
// NOTE: the QK scheduler is entered and exited with interrupts DISABLED.
// QK_sched_() is extern "C", so it does not belong to the QP namespace.
//
extern "C" {

//............................................................................
// NOTE: QK schedPrio_() is entered and exited with interrupts DISABLED
uint8_t QK_schedPrio_(void) {

    uint8_t p = QK_readySet_.findMax();

#ifdef QK_NO_MUTEX
    if (p > QK_currPrio_) {                        // do we have a preemption?
#else                                   // QK priority-ceiling mutexes allowed
    if ((p > QK_currPrio_) && (p > QK_ceilingPrio_)) {
#endif
    }
    else {
        p = static_cast<uint8_t>(0);
    }
    return p;
}
//............................................................................
void QK_sched_(uint8_t p) {

    uint8_t pin = QK_currPrio_;                   // save the initial priority
    QP_ QActive *a;
#ifdef QK_TLS                                    // thread-local storage used?
    uint8_t pprev = pin;
#endif
    do {
        a = QP_ QF::active_[p];                // obtain the pointer to the AO
        QK_currPrio_ = p;            // this becomes the current task priority

#ifdef QK_TLS                                    // thread-local storage used?
        if (p != pprev) {                          // are we changing threads?
            QK_TLS(a);                      // switch new thread-local storage
            pprev = p;
        }
#endif
        QS_BEGIN_NOCRIT_(QP_ QS_QK_SCHEDULE, QP_ QS::aoObj_, a)
            QS_TIME_();                                           // timestamp
            QS_U8_(p);                    // the priority of the active object
            QS_U8_(pin);                             // the preempted priority
        QS_END_NOCRIT_()

        QF_INT_ENABLE();                  // unconditionally enable interrupts

        QP_ QEvt const *e = a->get_();       // get the next event for this AO
        a->dispatch(e);                               // dispatch e to this AO
        QP_ QF::gc(e);              // garbage collect the event, if necessary

                        // determine the next highest-priority AO ready to run
        QF_INT_DISABLE();                                // disable interrupts
        p = QK_readySet_.findMax();

#ifdef QK_NO_MUTEX
    } while (p > pin);             // is the new priority higher than initial?
#else                                   // QK priority-ceiling mutexes allowed
    } while ((p > pin) && (p > QK_ceilingPrio_));
#endif

    QK_currPrio_ = pin;                        // restore the initial priority

#ifdef QK_TLS                                    // thread-local storage used?
    if (pin != static_cast<uint8_t>(0)) { // no extended context for idle loop
        a = QP_ QF::active_[pin];           // the pointer to the preempted AO
        QK_TLS(a);                                 // restore the original TLS
    }
#endif
}

}                                                                // extern "C"

#ifndef QK_NO_MUTEX

// "qk_mutex.cpp" ============================================================
/// \brief QK::mutexLock()/QK::mutexUnlock() implementation.

// package-scope objects -----------------------------------------------------
extern "C" {
    uint8_t QK_ceilingPrio_;                    // ceiling priority of a mutex
}

QP_BEGIN_

//............................................................................
QMutex QK::mutexLock(uint8_t const prioCeiling) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();
    uint8_t mutex = QK_ceilingPrio_; // original QK priority ceiling to return
    if (QK_ceilingPrio_ < prioCeiling) {
        QK_ceilingPrio_ = prioCeiling;        // raise the QK priority ceiling
    }

    QS_BEGIN_NOCRIT_(QS_QK_MUTEX_LOCK,
                     static_cast<void *>(0), static_cast<void *>(0))
        QS_TIME_();                                               // timestamp
        QS_U8_(mutex);                                // the original priority
        QS_U8_(QK_ceilingPrio_);               // the current priority ceiling
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
    return mutex;
}
//............................................................................
void QK::mutexUnlock(QMutex mutex) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QK_MUTEX_UNLOCK,
                     static_cast<void *>(0), static_cast<void *>(0))
        QS_TIME_();                                               // timestamp
        QS_U8_(mutex);                                // the original priority
        QS_U8_(QK_ceilingPrio_);               // the current priority ceiling
    QS_END_NOCRIT_()

    if (QK_ceilingPrio_ > mutex) {
        QK_ceilingPrio_ = mutex;         // restore the saved priority ceiling
        mutex = QK_schedPrio_();             // reuse 'mutex' to hold priority
        if (mutex != static_cast<uint8_t>(0)) {
            QK_sched_(mutex);
        }
    }
    QF_CRIT_EXIT_();
}

QP_END_

#endif                                                          // QK_NO_MUTEX

#else                                                         // QK_PREEMPTIVE

// "qvanilla.cpp" ============================================================
/// \brief "vanilla" cooperative kernel,
/// QActive::start(), QActive::stop(), and QF::run() implementation.

QP_BEGIN_

// Package-scope objects -----------------------------------------------------
extern "C" {
#if (QF_MAX_ACTIVE <= 8)
    QPSet8  QF_readySet_;                       // ready set of active objects
#else
    QPSet64 QF_readySet_;                       // ready set of active objects
#endif

uint8_t QF_currPrio_;                     ///< current task/interrupt priority
uint8_t QF_intNest_;                              ///< interrupt nesting level

}                                                                // extern "C"

//............................................................................
void QF::init(void) {
    // nothing to do for the "vanilla" kernel
}
//............................................................................
void QF::stop(void) {
    onCleanup();                                           // cleanup callback
    // nothing else to do for the "vanilla" kernel
}
//............................................................................
int16_t QF::run(void) {
    onStartup();                                           // startup callback

    for (;;) {                                           // the bacground loop
        QF_INT_DISABLE();
        if (QF_readySet_.notEmpty()) {
            uint8_t p = QF_readySet_.findMax();
            QActive *a = active_[p];
            QF_currPrio_ = p;                     // save the current priority
            QF_INT_ENABLE();

            QEvt const *e = a->get_();     // get the next event for this AO
            a->dispatch(e);                         // dispatch evt to the HSM
            gc(e);       // determine if event is garbage and collect it if so
        }
        else {
            onIdle();                                            // see NOTE01
        }
    }
                      // this unreachable return is to make the compiler happy
    return static_cast<int16_t>(0);
}
//............................................................................
void QActive::start(uint8_t const prio,
                    QEvt const *qSto[], uint32_t const qLen,
                    void * const stkSto, uint32_t const,
                    QEvt const * const ie)
{
    Q_REQUIRE((u8_0 < prio) && (prio <= static_cast<uint8_t>(QF_MAX_ACTIVE))
              && (stkSto == null_void));      // does not need per-actor stack

    m_eQueue.init(qSto, static_cast<QEQueueCtr>(qLen));  // initialize QEQueue
    m_prio = prio;                // set the QF priority of this active object
    QF::add_(this);                     // make QF aware of this active object
    init(ie);                                    // execute initial transition

    QS_FLUSH();                          // flush the trace buffer to the host
}
//............................................................................
void QActive::stop(void) {
    QF::remove_(this);
}

QP_END_

//////////////////////////////////////////////////////////////////////////////
// NOTE01:
// QF::onIdle() must be called with interrupts DISABLED because the
// determination of the idle condition (no events in the queues) can change
// at any time by an interrupt posting events to a queue. The QF::onIdle()
// MUST enable interrups internally, perhaps at the same time as putting the
// CPU into a power-saving mode.
//

extern "C" {

void QK_sched_(uint8_t p) {            // dummy empty definition for qk_port.s
    (void)p;
}

uint8_t QK_schedPrio_(void) {          // dummy empty definition for qk_port.s
    return static_cast<uint8_t>(0U);
}

}                                                                // extern "C"

#endif                                                        // QK_PREEMPTIVE

//////////////////////////////////////////////////////////////////////////////
#ifdef Q_SPY

// "qs_pkg.h" ================================================================
/// \brief Internal (package scope) QS/C++ interface.

QP_BEGIN_

/// \brief QS ring buffer counter and offset type
typedef uint16_t QSCtr;

/// \brief Internal QS macro to insert an un-escaped byte into
/// the QS buffer
////
#define QS_INSERT_BYTE(b_) \
    QS_PTR_AT_(QS_head_) = (b_); \
    ++QS_head_; \
    if (QS_head_ == QS_end_) { \
        QS_head_ = static_cast<QSCtr>(0); \
    } \
    ++QS_used_;

/// \brief Internal QS macro to insert an escaped byte into the QS buffer
#define QS_INSERT_ESC_BYTE(b_) \
    QS_chksum_ = static_cast<uint8_t>(QS_chksum_ + (b_)); \
    if (((b_) == QS_FRAME) || ((b_) == QS_ESC)) { \
        QS_INSERT_BYTE(QS_ESC) \
        QS_INSERT_BYTE(static_cast<uint8_t>((b_) ^ QS_ESC_XOR)) \
    } \
    else { \
        QS_INSERT_BYTE(b_) \
    }

/// \brief Internal QS macro to insert a escaped checksum byte into
/// the QS buffer
#define QS_INSERT_CHKSUM_BYTE() \
    QS_chksum_ = static_cast<uint8_t>(~QS_chksum_); \
    if ((QS_chksum_ == QS_FRAME) || (QS_chksum_ == QS_ESC)) { \
        QS_INSERT_BYTE(QS_ESC) \
        QS_INSERT_BYTE(static_cast<uint8_t>(QS_chksum_ ^ QS_ESC_XOR)) \
    } \
    else { \
        QS_INSERT_BYTE(QS_chksum_) \
    }


/// \brief Internal QS macro to access the QS ring buffer
///
/// \note The QS buffer is allocated by the user and is accessed through the
/// pointer QS_ring_, which violates the MISRA-C 2004 Rule 17.4(req), pointer
/// arithmetic other than array indexing. Encapsulating this violation in a
/// macro allows to selectively suppress this specific deviation.
#define QS_PTR_AT_(i_) (QS_ring_[i_])


/// \brief Frame character of the QS output protocol
uint8_t const QS_FRAME = static_cast<uint8_t>(0x7E);

/// \brief Escape character of the QS output protocol
uint8_t const QS_ESC   = static_cast<uint8_t>(0x7D);

/// \brief Escape modifier of the QS output protocol
///
/// The escaped byte is XOR-ed with the escape modifier before it is inserted
/// into the QS buffer.
uint8_t const QS_ESC_XOR = static_cast<uint8_t>(0x20);

#ifndef Q_ROM_BYTE
    /// \brief Macro to access a byte allocated in ROM
    ///
    /// Some compilers for Harvard-architecture MCUs, such as gcc for AVR, do
    /// not generate correct code for accessing data allocated in the program
    /// space (ROM). The workaround for such compilers is to explictly add
    /// assembly code to access each data element allocated in the program
    /// space. The macro Q_ROM_BYTE() retrieves a byte from the given ROM
    /// address.
    ///
    /// The Q_ROM_BYTE() macro should be defined for the compilers that
    /// cannot handle correctly data allocated in ROM (such as the gcc).
    /// If the macro is left undefined, the default definition simply returns
    /// the argument and lets the compiler generate the correct code.
    #define Q_ROM_BYTE(rom_var_)   (rom_var_)
#endif

//............................................................................
extern uint8_t *QS_ring_;         ///< pointer to the start of the ring buffer
extern QSCtr QS_end_;                ///< offset of the end of the ring buffer
extern QSCtr QS_head_;         ///< offset to where next byte will be inserted
extern QSCtr QS_tail_;       ///< offset of where next event will be extracted
extern QSCtr QS_used_;       ///< number of bytes currently in the ring buffer
extern uint8_t QS_seq_;                        ///< the record sequence number
extern uint8_t QS_chksum_;             ///< the checksum of the current record
extern uint8_t QS_full_;              ///< the ring buffer is temporarily full

QP_END_


// "qs.cpp" ==================================================================
/// \brief QS internal variables definitions and core QS functions
/// implementations.

QP_BEGIN_

//............................................................................
uint8_t QS::glbFilter_[32];                                // global QS filter

//............................................................................
uint8_t *QS_ring_;                  // pointer to the start of the ring buffer
QSCtr QS_end_;                         // offset of the end of the ring buffer
QSCtr QS_head_;                  // offset to where next byte will be inserted
QSCtr QS_tail_;                 // offset of where next byte will be extracted
QSCtr QS_used_;                // number of bytes currently in the ring buffer
uint8_t QS_seq_;                                 // the record sequence number
uint8_t QS_chksum_;                      // the checksum of the current record
uint8_t QS_full_;                       // the ring buffer is temporarily full

//............................................................................
char_t const Q_ROM * Q_ROM_VAR QS::getVersion(void) {
    uint8_t const u8_zero = static_cast<uint8_t>('0');
    static char_t const Q_ROM Q_ROM_VAR version[] = {
        static_cast<char_t>(((QP_VERSION >> 12) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  8) & 0xFU) + u8_zero),
        static_cast<char_t>('.'),
        static_cast<char_t>(((QP_VERSION >>  4) & 0xFU) + u8_zero),
        static_cast<char_t>((QP_VERSION         & 0xFU) + u8_zero),
        static_cast<char_t>('\0')
    };
    return version;
}
//............................................................................
void QS::initBuf(uint8_t sto[], uint32_t const stoSize) {
    QS_ring_ = &sto[0];
    QS_end_  = static_cast<QSCtr>(stoSize);
}
//............................................................................
void QS::filterOn(uint8_t const rec) {
    if (rec == QS_ALL_RECORDS) {
        uint8_t i;
        for (i = static_cast<uint8_t>(0);
             i < static_cast<uint8_t>(sizeof(glbFilter_));
             ++i)
        {
            glbFilter_[i] = static_cast<uint8_t>(0xFF);
        }
    }
    else {
        glbFilter_[rec >> 3] |=
            static_cast<uint8_t>(1U << (rec & static_cast<uint8_t>(0x07)));
    }
}
//............................................................................
void QS::filterOff(uint8_t const rec) {
    if (rec == QS_ALL_RECORDS) {
        uint8_t i;
        for (i = static_cast<uint8_t>(0);
            i < static_cast<uint8_t>(sizeof(glbFilter_));
            ++i)
        {
            glbFilter_[i] = static_cast<uint8_t>(0);
        }
    }
    else {
        glbFilter_[rec >> 3] &= static_cast<uint8_t>(
           ~static_cast<uint8_t>((1U << (rec & static_cast<uint8_t>(0x07)))));
    }
}
//............................................................................
void QS::begin(uint8_t const rec) {
    QS_chksum_ = static_cast<uint8_t>(0);                // clear the checksum
    ++QS_seq_;                         // always increment the sequence number
    QS_INSERT_ESC_BYTE(QS_seq_)                   // store the sequence number
    QS_INSERT_ESC_BYTE(rec)                             // store the record ID
}
//............................................................................
void QS::end(void) {
    QS_INSERT_CHKSUM_BYTE()
    QS_INSERT_BYTE(QS_FRAME)
    if (QS_used_ > QS_end_) {                    // overrun over the old data?
        QS_tail_ = QS_head_;                 // shift the tail to the old data
        QS_used_ = QS_end_;                        // the whole buffer is used
    }
}
//............................................................................
void QS::u8(uint8_t const format, uint8_t const d) {
    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE(d)
}
//............................................................................
void QS::u16(uint8_t const format, uint16_t d) {
    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
}
//............................................................................
void QS::u32(uint8_t const format, uint32_t d) {
    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
}

QP_END_


// "qs_.cpp" =================================================================
/// \brief QS functions for internal use inside QP components

QP_BEGIN_

//............................................................................
void const *QS::smObj_;                  // local state machine for QEP filter
void const *QS::aoObj_;                   // local active object for QF filter
void const *QS::mpObj_;                     //  local event pool for QF filter
void const *QS::eqObj_;                      //  local raw queue for QF filter
void const *QS::teObj_;                     //  local time event for QF filter
void const *QS::apObj_;                    //  local object Application filter

QSTimeCtr QS::tickCtr_;              // tick counter for the QS_QF_TICK record

//............................................................................
void QS::u8_(uint8_t const d) {
    QS_INSERT_ESC_BYTE(d)
}
//............................................................................
void QS::u16_(uint16_t d) {
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
}
//............................................................................
void QS::u32_(uint32_t d) {
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
    d >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(d))
}
//............................................................................
void QS::str_(char_t const *s) {
    uint8_t b = static_cast<uint8_t>(*s);
    while (b != static_cast<uint8_t>(0)) {
                                       // ASCII characters don't need escaping
        QS_chksum_ = static_cast<uint8_t>(QS_chksum_ + b);
        QS_INSERT_BYTE(b)
        ++s;
        b = static_cast<uint8_t>(*s);
    }
    QS_INSERT_BYTE(static_cast<uint8_t>(0))
}
//............................................................................
void QS::str_ROM_(char_t const Q_ROM * Q_ROM_VAR s) {
    uint8_t b = static_cast<uint8_t>(Q_ROM_BYTE(*s));
    while (b != static_cast<uint8_t>(0)) {
                                       // ASCII characters don't need escaping
        QS_chksum_ = static_cast<uint8_t>(QS_chksum_ + b);
        QS_INSERT_BYTE(b)
        ++s;
        b = static_cast<uint8_t>(Q_ROM_BYTE(*s));
    }
    QS_INSERT_BYTE(static_cast<uint8_t>(0))
}

QP_END_

// "qs_blk.cpp" ==============================================================
/// \brief QS::getBlock() implementation

QP_BEGIN_

//............................................................................
// get up to *pn bytes of contiguous memory
uint8_t const *QS::getBlock(uint16_t * const pNbytes) {
    uint8_t *block;
    if (QS_used_ == static_cast<QSCtr>(0)) {
        *pNbytes = static_cast<uint16_t>(0);
        block = static_cast<uint8_t *>(0);     // no bytes to return right now
    }
    else {
        QSCtr n = static_cast<QSCtr>(QS_end_ - QS_tail_);
        if (n > QS_used_) {
            n = QS_used_;
        }
        if (n > static_cast<QSCtr>(*pNbytes)) {
            n = static_cast<QSCtr>(*pNbytes);
        }
        *pNbytes = static_cast<uint16_t>(n);
        QS_used_ = static_cast<QSCtr>(QS_used_ - n);
        QSCtr t  = QS_tail_;
        QS_tail_ = static_cast<QSCtr>(QS_tail_ + n);
        if (QS_tail_ == QS_end_) {
            QS_tail_ = static_cast<QSCtr>(0);
        }
        block = &QS_PTR_AT_(t);
    }
    return block;
}

QP_END_

// "qs_byte.cpp" =============================================================
/// \brief QS::getByte() implementation

QP_BEGIN_

//............................................................................
uint16_t QS::getByte(void) {
    uint16_t ret;
    if (QS_used_ == static_cast<QSCtr>(0)) {
        ret = QS_EOD;                                       // set End-Of-Data
    }
    else {
        ret = static_cast<uint16_t>(QS_PTR_AT_(QS_tail_));   // byte to return
        ++QS_tail_;                                        // advance the tail
        if (QS_tail_ == QS_end_) {                        // tail wrap around?
            QS_tail_ = static_cast<QSCtr>(0);
        }
        --QS_used_;                                      // one less byte used
    }
    return ret;                                      // return the byte or EOD
}

QP_END_


// "qs_f32.cpp" ==============================================================
/// \brief QS::f32() implementation

QP_BEGIN_

//............................................................................
void QS::f32(uint8_t const format, float32_t const d) {
    union F32Rep {
        float32_t f;
        uint32_t  u;
    } fu32;
    fu32.f = d;

    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(fu32.u))
    fu32.u >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(fu32.u))
    fu32.u >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(fu32.u))
    fu32.u >>= 8;
    QS_INSERT_ESC_BYTE(static_cast<uint8_t>(fu32.u))
}

QP_END_


// "qs_mem.cpp" ==============================================================
/// \brief QS::mem() implementation

QP_BEGIN_

//............................................................................
void QS::mem(uint8_t const *blk, uint8_t size) {
    QS_INSERT_BYTE(static_cast<uint8_t>(QS_MEM_T))
    QS_chksum_ =
        static_cast<uint8_t>(QS_chksum_ + static_cast<uint8_t>(QS_MEM_T));
    QS_INSERT_ESC_BYTE(size)
    while (size != static_cast<uint8_t>(0)) {
        QS_INSERT_ESC_BYTE(*blk)
        ++blk;
        --size;
    }
}

QP_END_


// "qs_str.cpp" ==============================================================
/// \brief QS::str() and QS::str_ROM() implementation

QP_BEGIN_

//............................................................................
void QS::str(char_t const *s) {
    uint8_t b = static_cast<uint8_t>(*s);
    QS_INSERT_BYTE(static_cast<uint8_t>(QS_STR_T))
    QS_chksum_ =
        static_cast<uint8_t>(QS_chksum_ + static_cast<uint8_t>(QS_STR_T));
    while (b != static_cast<uint8_t>(0)) {
                                       // ASCII characters don't need escaping
        QS_INSERT_BYTE(b)
        QS_chksum_ = static_cast<uint8_t>(QS_chksum_ + b);
        ++s;
        b = static_cast<uint8_t>(*s);
    }
    QS_INSERT_BYTE(static_cast<uint8_t>(0))
}
//............................................................................
void QS::str_ROM(char_t const Q_ROM * Q_ROM_VAR s) {
    uint8_t b = static_cast<uint8_t>(Q_ROM_BYTE(*s));
    QS_INSERT_BYTE(static_cast<uint8_t>(QS_STR_T))
    QS_chksum_ =
        static_cast<uint8_t>(QS_chksum_ + static_cast<uint8_t>(QS_STR_T));
    while (b != static_cast<uint8_t>(0)) {
                                       // ASCII characters don't need escaping
        QS_INSERT_BYTE(b)
        QS_chksum_ = static_cast<uint8_t>(QS_chksum_ + b);
        ++s;
        b = static_cast<uint8_t>(Q_ROM_BYTE(*s));
    }
    QS_INSERT_BYTE(static_cast<uint8_t>(0))
}

QP_END_

#endif                                                                // Q_SPY
