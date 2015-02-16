//////////////////////////////////////////////////////////////////////////////
// Product: QP/C++ port to Arduino
// Last Updated for QP ver: 4.5.03 (modified to fit in one file)
// Date of the Last Update: Oct 02, 2012
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
#ifndef qp_port_h
#define qp_port_h

#include <stdint.h>                       // C99-standard exact-width integers
#include <avr/pgmspace.h>    // accessing data in the program memory (PROGMEM)
#include <avr/io.h>                                         // SREG definition
#include <avr/interrupt.h>                                      // cli()/sei()

                   // the macro QK_PREEMPTIVE selects the preemptive QK kernel
                   // (default is the cooperative "Vanilla" kernel)
//#define QK_PREEMPTIVE           1
                                  // allow using the QK priority ceiling mutex
//#define QK_MUTEX                1

                        // various QF object sizes configuration for this port
#define QF_MAX_ACTIVE           8
#define QF_MAX_EPOOL            3
#define QF_EVENT_SIZ_SIZE       1
#define QF_EQUEUE_CTR_SIZE      1
#define QF_MPOOL_SIZ_SIZE       1
#define QF_MPOOL_CTR_SIZE       1
#define QF_TIMEEVT_CTR_SIZE     2

                         // the macro 'PROGMEM' allocates const objects to ROM
#define Q_ROM                   PROGMEM

                               // the macro 'Q_ROM_BYTE' reads a byte from ROM
#define Q_ROM_BYTE(rom_var_)    pgm_read_byte_near(&(rom_var_))

                                                // QF interrupt disable/enable
#define QF_INT_DISABLE()        cli()
#define QF_INT_ENABLE()         sei()

                                             // QF critical section entry/exit
#define QF_CRIT_STAT_TYPE       uint8_t
#define QF_CRIT_ENTRY(stat_)    do { \
    (stat_) = SREG; \
    cli(); \
} while (0)
#define QF_CRIT_EXIT(stat_)     (SREG = (stat_))

#ifdef QK_PREEMPTIVE
                                                // QK interrupt entry and exit
    #define QK_ISR_ENTRY()    (++QK_intNest_)

    #define QK_ISR_EXIT()     do { \
        --QK_intNest_; \
        if (QK_intNest_ == (uint8_t)0) { \
            uint8_t p = QK_schedPrio_(); \
            if (p != (uint8_t)0) { \
                QK_sched_(p); \
            } \
        } \
    } while (0)

#endif                                                        // QK_PREEMPTIVE

#ifdef Q_SPY                                   // QS software tracing enabled?
    #define QS_TIME_SIZE            4
    #define QS_OBJ_PTR_SIZE         4
    #define QS_FUN_PTR_SIZE         4
#endif                                                                // Q_SPY

//////////////////////////////////////////////////////////////////////////////
// DO NOT CHANGE ANYTHING BELOW THIS LINE

// "qevt.h" ==================================================================
/// \brief QEvt class and basic macros used by all QP components.
///
/// This header file must be included, perhaps indirectly, in all modules
/// (*.cpp files) that use any component of QP/C++ (such as QEP, QF, or QK).

//////////////////////////////////////////////////////////////////////////////
/// \brief The current QP version number
///
/// \return version of the QP as a hex constant constant 0xXYZZ, where X is
/// a 1-digit major version number, Y is a 1-digit minor version number, and
/// ZZ is a 2-digit release number.
#define QP_VERSION      0x4503U

#ifndef Q_ROM
    /// \brief Macro to specify compiler-specific directive for placing a
    /// constant object in ROM.
    ///
    /// Many compilers for Harvard-architecture MCUs provide non-stanard
    /// extensions to support placement of objects in different memories.
    /// In order to conserve the precious RAM, QP uses the Q_ROM macro for
    /// all constant objects that can be allocated in ROM.
    ///
    /// To override the following empty definition, you need to define the
    /// Q_ROM macro in the qep_port.h header file. Some examples of valid
    /// Q_ROM macro definitions are: __code (IAR 8051 compiler), code (Keil
    /// Cx51 compiler), PROGMEM (gcc for AVR), __flash (IAR for AVR).
    #define Q_ROM
#endif
#ifndef Q_ROM_VAR            // if NOT defined, provide the default definition
    /// \brief Macro to specify compiler-specific directive for accessing a
    /// constant object in ROM.
    ///
    /// Many compilers for MCUs provide different size pointers for
    /// accessing objects in various memories. Constant objects allocated
    /// in ROM (see #Q_ROM macro) often mandate the use of specific-size
    /// pointers (e.g., far pointers) to get access to ROM objects. The
    /// macro Q_ROM_VAR specifies the kind of the pointer to be used to access
    /// the ROM objects.
    ///
    /// To override the following empty definition, you need to define the
    /// Q_ROM_VAR macro in the qep_port.h header file. An example of valid
    /// Q_ROM_VAR macro definition is: __far (Freescale HC(S)08 compiler).
    #define Q_ROM_VAR
#endif
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

#ifndef Q_SIGNAL_SIZE
    /// \brief The size (in bytes) of the signal of an event. Valid values:
    /// 1, 2, or 4; default 1
    ///
    /// This macro can be defined in the QEP port file (qep_port.h) to
    /// configure the ::QSignal type. When the macro is not defined, the
    /// default of 1 byte is chosen.
    #define Q_SIGNAL_SIZE 2
#endif

#ifndef Q_NNAMESPACE

    /// \brief begin of the namespace QP
    ///
    /// \note defines to nothing if #Q_USE_NAMESPACE is undefined
    #define QP_BEGIN_  namespace QP {

    /// \brief end of the namespace QP
    ///
    /// \note defines to nothing if #Q_USE_NAMESPACE is undefined
    #define QP_END_    }

    /// \brief namespace QP prefix
    ///
    /// \note defines to nothing if #Q_USE_NAMESPACE is undefined
    #define QP_        QP::

#else

    #define QP_BEGIN_
    #define QP_END_
    #define QP_

#endif

QP_BEGIN_

#if (Q_SIGNAL_SIZE == 1)
    typedef uint8_t QSignal;
#elif (Q_SIGNAL_SIZE == 2)
    /// \brief QSignal represents the signal of an event.
    ///
    /// The relationship between an event and a signal is as follows. A signal
    /// in UML is the specification of an asynchronous stimulus that triggers
    /// reactions [<A HREF="http://www.omg.org/docs/ptc/03-08-02.pdf">UML
    /// document ptc/03-08-02</A>], and as such is an essential part of an
    /// event. (The signal conveys the type of the occurrence-what happened?)
    /// However, an event can also contain additional quantitative information
    /// about the occurrence in form of event parameters. Please refer to the
    typedef uint16_t QSignal;
#elif (Q_SIGNAL_SIZE == 4)
    typedef uint32_t QSignal;
#else
    #error "Q_SIGNAL_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

#ifdef Q_EVT_CTOR               // Provide the constructor for the QEvt class?

    //////////////////////////////////////////////////////////////////////////
    /// \brief QEvt base class.
    ///
    /// QEvt represents events without parameters and serves as the base class
    /// for derivation of events with parameters.
    ///
    /// The following example illustrates how to add an event parameter by
    /// inheriting from the QEvt class.
    /// \include qep_qevent.cpp
    class QEvt {
    public:
        QSignal sig;                         ///< signal of the event instance

        QEvt(QSignal const s)           // poolId_ intentionally uninitialized
          : sig(s) {}

#ifdef Q_EVT_VIRTUAL
        virtual ~QEvt() {}                               // virtual destructor
#endif

    private:
        uint8_t poolId_;                     ///< pool ID (0 for static event)
        uint8_t refCtr_;                                ///< reference counter

        friend class QF;
        friend class QTimeEvt;
        friend uint8_t QF_EVT_POOL_ID_ (QEvt const * const e);
        friend uint8_t QF_EVT_REF_CTR_ (QEvt const * const e);
        friend void QF_EVT_REF_CTR_INC_(QEvt const * const e);
        friend void QF_EVT_REF_CTR_DEC_(QEvt const * const e);
    };

#else                                    // QEvt is a POD (Plain Old Datatype)

    struct QEvt {
        QSignal sig;                         ///< signal of the event instance
        uint8_t poolId_;                     ///< pool ID (0 for static event)
        uint8_t refCtr_;                                ///< reference counter
    };

#endif                                                           // Q_EVT_CTOR

QP_END_

//////////////////////////////////////////////////////////////////////////////
/// helper macro to calculate static dimension of a 1-dim array \a array_
#define Q_DIM(array_) (sizeof(array_) / sizeof(array_[0]))

//////////////////////////////////////////////////////////////////////////////
// typedefs for basic numerical types; MISRA-C++ 2008 rule 3-9-2(req).

/// \brief typedef for character strings.
///
/// This typedef specifies character type for exclusive use in character
/// strings. Use of this type, rather than plain 'char', is in compliance
/// with the MISRA-C 2004 Rules 6.1(req), 6.3(adv).
///
typedef char char_t;

/// typedef for 32-bit IEEE 754 floating point numbers
typedef float float32_t;

/// typedef for 64-bit IEEE 754 floating point numbers
typedef double float64_t;

/// typedef for enumerations used for event signals
typedef int enum_t;

/// \brief Perform cast from unsigned integer \a uint_ to pointer
/// of type \a type_.
///
/// This macro encapsulates the cast to (type_ *), which QP ports or
/// application might use to access embedded hardware registers.
/// Such uses can trigger PC-Lint "Note 923: cast from int to pointer"
/// and this macro helps to encapsulate this deviation.
///
#define Q_UINT2PTR_CAST(type_, uint_)  (reinterpret_cast<type_ *>(uint_))

/// \brief Initializer of static constant QEvt instances
///
/// This macro encapsulates the ugly casting of enumerated signals
/// to QSignal and constants for QEvt.poolID and QEvt.refCtr_.
///
#define QEVT_INITIALIZER(sig_) { static_cast<QP_ QSignal>(sig_), \
    static_cast<uint8_t>(0), static_cast<uint8_t>(0) }

//////////////////////////////////////////////////////////////////////////////
// facilities for backwards compatibility
//
#ifndef Q_NQEVENT
//    typedef struct QEvt QEvent;
    #define QEvent QEvt
#endif


// "qep.h" ===================================================================
/// \brief QEP/C++ platform-independent public interface.
///
/// This header file must be included directly or indirectly
/// in all modules (*.cpp files) that use QEP/C++.

/// \brief Designates a target for an initial or regular transition.
/// Q_TRAN() can be used both in the FSMs and HSMs.
///
/// \include qep_qtran.c
///
#define Q_TRAN(target_)     (me->tran(Q_STATE_CAST(target_)))

/// \brief Designates the superstate of a given state in an HSM.
///
/// \include qep_qhsm.c
///
#define Q_SUPER(super_)     (me->super(Q_STATE_CAST(super_)))

/// \brief Perform cast to QStateHandler.
///
/// This macro encapsulates the cast of a specific state handler function
/// pointer to QStateHandler, which violates MISRA-C 2004 rule 11.4(advisory).
/// This macro helps to localize this deviation.
///
#define Q_STATE_CAST(handler_) \
    (reinterpret_cast<QP_ QStateHandler>(handler_))

/// \brief Perform downcast of an event onto a subclass of QEvt \a class_
///
/// This macro encapsulates the downcast of QEvt pointers, which violates
/// MISRA-C 2004 rule 11.4(advisory). This macro helps to localize this
/// deviation.
///
#define Q_EVT_CAST(class_)   (static_cast<class_ const *>(e))

//////////////////////////////////////////////////////////////////////////////
QP_BEGIN_

/// \brief Provides miscellaneous QEP services.
class QEP {
public:
    /// \brief get the current QEP version number string
    ///
    /// \return version of the QEP as a constant 6-character string of the
    /// form x.y.zz, where x is a 1-digit major version number, y is a
    /// 1-digit minor version number, and zz is a 2-digit release number.
    static char_t const Q_ROM * Q_ROM_VAR getVersion(void);
};

//////////////////////////////////////////////////////////////////////////////

                       /// \brief Type returned from  a state-handler function
typedef uint8_t QState;
                                  /// \brief pointer to state-handler function
typedef QState (*QStateHandler)(void * const me, QEvt const * const e);

//////////////////////////////////////////////////////////////////////////////
/// \brief Value returned by a state-handler function when it handles
/// the event.
QState const Q_RET_HANDLED = static_cast<QState>(0);

/// \brief Value returned by a non-hierarchical state-handler function when
/// it ignores (does not handle) the event.
QState const Q_RET_IGNORED = static_cast<QState>(1);

/// \brief Value returned by a state-handler function when it takes a
/// regular state transition.
QState const Q_RET_TRAN    = static_cast<QState>(2);

/// \brief Value returned by a state-handler function when it forwards
/// the event to the superstate to handle.
QState const Q_RET_SUPER   = static_cast<QState>(3);

/// \brief Value returned by a state-handler function when a guard
/// condition prevents it from handling the event.
///
QState const Q_RET_UNHANDLED = static_cast<QState>(4);


/// Offset or the user signals
enum_t const Q_USER_SIG    = static_cast<enum_t>(4);


//////////////////////////////////////////////////////////////////////////////
/// \brief Finite State Machine base class
///
/// QFsm represents a traditional non-hierarchical Finite State Machine (FSM)
/// without state hierarchy, but with entry/exit actions.
///
/// QFsm is also a base structure for the ::QHsm class.
///
/// \note QFsm is not intended to be instantiated directly, but rather serves
/// as the base class for derivation of state machines in the application
/// code.
///
/// The following example illustrates how to derive a state machine class
/// from QFsm.
/// \include qep_qfsm.cpp
class QFsm {
private:
    QStateHandler m_state;          ///< current active state (state-variable)
    QStateHandler m_temp;  ///< temporary state: target of tran. or superstate

public:
    /// \brief virtual destructor
    virtual ~QFsm();

    /// \brief Performs the second step of FSM initialization by triggering
    /// the top-most initial transition.
    ///
    /// The argument \a e is constant pointer to ::QEvt or a class
    /// derived from ::QEvt.
    ///
    /// \note Must be called only ONCE before QFsm::dispatch()
    ///
    /// The following example illustrates how to initialize a FSM, and
    /// dispatch events to it:
    /// \include qep_qfsm_use.cpp
    void init(QEvt const * const e = static_cast<QEvt const *>(0));

    /// \brief Dispatches an event to a FSM
    ///
    /// Processes one event at a time in Run-to-Completion (RTC) fashion.
    /// The argument \a e is a constant pointer the ::QEvt or a
    /// class derived from ::QEvt.
    ///
    /// \note Must be called after QFsm::init().
    ///
    /// \sa example for QFsm::init()
    void dispatch(QEvt const * const e);

protected:
    /// \brief Protected constructor of a FSM.
    ///
    /// Performs the first step of FSM initialization by assigning the
    /// initial pseudostate to the currently active state of the state
    /// machine.
    ///
    /// \note The constructor is protected to prevent direct instantiating
    /// of QFsm objects. This class is intended for subclassing only.
    ///
    /// \sa The ::QFsm example illustrates how to use the QHsm constructor
    /// in the constructor initializer list of the derived state machines.
    QFsm(QStateHandler const initial)
      : m_state(Q_STATE_CAST(0)),
        m_temp(initial)
    {}

    /// \brief Return the current active state.
    QStateHandler state(void) const {
        return m_state;
    }

    /// \brief Inline function to specify the return of a state-handler
    /// when it igonres (does not handle in any way) the event.
    static QState Q_IGNORED(void) { return Q_RET_IGNORED; }

    /// \brief Inline function to specify the return of a state-handler
    /// when it handles the event.
    ///
    /// \include qep_qhsm.cpp
    static QState Q_HANDLED(void) { return Q_RET_HANDLED; }

    /// \brief Macro to specify the return of a state-handler function when
    /// it attempts to handle the event but a guard condition evaluates to
    /// false and there is no other explicit way of handling the event.
    static QState Q_UNHANDLED(void) { return Q_RET_UNHANDLED; }

    /// \brief internal helper function to record a state transition
    QState tran(QStateHandler const target) {
        m_temp = target;
        return Q_RET_TRAN;
    }

    enum ReservedFsmSignals {
        Q_ENTRY_SIG = 1,                         ///< signal for entry actions
        Q_EXIT_SIG                                ///< signal for exit actions
    };
};

//////////////////////////////////////////////////////////////////////////////
/// \brief Hierarchical State Machine base class
///
/// QHsm represents a Hierarchical Finite State Machine (HSM). QHsm derives
/// from the ::QFsm class and extends the capabilities of a basic FSM
/// with state hierarchy.
///
/// \note QHsm is not intended to be instantiated directly, but rather serves
/// as the base structure for derivation of state machines in the application
/// code.
///
/// The following example illustrates how to derive a state machine class
/// from QHsm.
/// \include qep_qhsm.cpp
class QHsm {
private:
    QStateHandler m_state;          ///< current active state (state-variable)
    QStateHandler m_temp;  ///< temporary state: target of tran. or superstate

public:
    /// \brief virtual destructor
    virtual ~QHsm();

    /// \brief Performs the second step of HSM initialization by triggering
    /// the top-most initial transition.
    ///
    /// \param e constant pointer ::QEvt or a class derived from ::QEvt
    /// \note Must be called only ONCE before QHsm::dispatch()
    ///
    /// The following example illustrates how to initialize a HSM, and
    /// dispatch events to it:
    /// \include qep_qhsm_use.cpp
    void init(QEvt const * const e = static_cast<QEvt const *>(0));

    /// \brief Dispatches an event to a HSM
    ///
    /// Processes one event at a time in Run-to-Completion (RTC) fashion.
    /// The argument \a e is a constant pointer the ::QEvt or a
    /// class derived from ::QEvt.
    ///
    /// \note Must be called after QHsm::init().
    ///
    /// \sa example for QHsm::init()
    void dispatch(QEvt const * const e);

    /// \brief Tests if a given state is part of the current active state
    /// configuratioin
    ///
    /// \param state is a pointer to the state handler function, e.g.,
    /// &QCalc::on.
    bool isIn(QStateHandler const s);

protected:
    /// \brief Protected constructor of a HSM.
    ///
    /// Performs the first step of HSM initialization by assigning the
    /// initial pseudostate to the currently active state of the state
    /// machine.
    ///
    /// \note The constructor is protected to prevent direct instantiating
    /// of QHsm objects. This class is intended for subclassing only.
    ///
    /// \sa The ::QHsm example illustrates how to use the QHsm constructor
    /// in the constructor initializer list of the derived state machines.
    /// \sa QFsm::QFsm()
    QHsm(QStateHandler const initial)
      : m_state(Q_STATE_CAST(&QHsm::top)),
        m_temp(initial)
    {}

    /// \brief Return the current active state.
    QStateHandler state(void) const {
        return m_state;
    }

    /// \brief the top-state.
    ///
    /// QHsm::top() is the ultimate root of state hierarchy in all HSMs
    /// derived from ::QHsm. This state handler always returns (QSTATE)0,
    /// which means that it "handles" all events.
    ///
    /// \sa Example of the QCalc::on() state handler.
    static QState top(void * const me, QEvt const * const e);

    /// \brief Inline function to specify the return of a state-handler
    /// when it handles the event.
    ///
    /// \include qep_qhsm.cpp
    static QState Q_HANDLED(void) { return Q_RET_HANDLED; }

    /// \brief Macro to specify the return of a state-handler function when
    /// it attempts to handle the event but a guard condition evaluates to
    /// false and there is no other explicit way of handling the event.
    static QState Q_UNHANDLED(void) { return Q_RET_UNHANDLED; }

    /// \brief internal helper function to record a state transition
    QState tran(QStateHandler const target) {
        m_temp = target;
        return Q_RET_TRAN;
    }

    /// \brief internal helper function to record the superstate
    QState super(QStateHandler const superstate) {
        m_temp = superstate;
        return Q_RET_SUPER;
    }

    enum ReservedHsmSignals {
        Q_ENTRY_SIG = 1,                         ///< signal for entry actions
        Q_EXIT_SIG,                               ///< signal for exit actions
        Q_INIT_SIG                  ///< signal for nested initial transitions
    };
};

//////////////////////////////////////////////////////////////////////////////

QP_END_

// "qequeue.h" ===============================================================
/// \brief platform-independent event queue interface.
///
/// This header file must be included in all QF ports that use native QF
/// event queue implementation. Also, this file is needed when the "raw"
/// thread-safe queues are used for communication between active objects
/// and non-framework entities, such as ISRs, device drivers, or legacy
/// code.

#ifndef QF_EQUEUE_CTR_SIZE

    /// \brief The size (in bytes) of the ring-buffer counters used in the
    /// native QF event queue implementation. Valid values: 1, 2, or 4;
    /// default 1.
    ///
    /// This macro can be defined in the QF port file (qf_port.h) to
    /// configure the ::QEQueueCtr type. Here the macro is not defined so the
    /// default of 1 byte is chosen.
    #define QF_EQUEUE_CTR_SIZE 1
#endif

QP_BEGIN_

#if (QF_EQUEUE_CTR_SIZE == 1)
    /// \brief The data type to store the ring-buffer counters based on
    /// the macro #QF_EQUEUE_CTR_SIZE.
    ///
    /// The dynamic range of this data type determines the maximum length
    /// of the ring buffer managed by the native QF event queue.
    typedef uint8_t QEQueueCtr;
#elif (QF_EQUEUE_CTR_SIZE == 2)
    typedef uint16_t QEQueueCtr;
#elif (QF_EQUEUE_CTR_SIZE == 4)
    typedef uint32_t QEQueueCtr;
#else
    #error "QF_EQUEUE_CTR_SIZE defined incorrectly, expected 1, 2, or 4"
#endif


//////////////////////////////////////////////////////////////////////////////
/// \brief Native QF Event Queue class
///
/// This structure describes the native QF event queue, which can be used as
/// the event queue for active objects, or as a simple "raw" event queue for
/// thread-safe event passing among non-framework entities, such as ISRs,
/// device drivers, or other third-party components.
///
/// The native QF event queue is configured by defining the macro
/// #QF_EQUEUE_TYPE as ::QEQueue in the specific QF port header file.
///
/// The ::QEQueue structure contains only data members for managing an event
/// queue, but does not contain the storage for the queue buffer, which must
/// be provided externally during the queue initialization.
///
/// The event queue can store only event pointers, not the whole events. The
/// internal implementation uses the standard ring-buffer plus one external
/// location that optimizes the queue operation for the most frequent case
/// of empty queue.
///
/// The ::QEQueue structure is used with two sets of functions. One set is for
/// the active object event queue, which needs to block the active object
/// task when the event queue is empty and unblock it when events are posted
/// to the queue. The interface for the native active object event queue
/// consists of the following functions: QActive::postFIFO_(),
/// QActive::postLIFO_(), and QActive::get_(). Additionally the function
/// QEQueue_init() is used to initialize the queue.
///
/// The other set of functions, uses this structure as a simple "raw" event
/// queue to pass events between entities other than active objects, such as
/// ISRs. The "raw" event queue is not capable of blocking on the get()
/// operation, but is still thread-safe because it uses QF critical section
/// to protect its integrity. The interface for the "raw" thread-safe queue
/// consists of the following functions: QEQueue::postFIFO(),
/// QEQueue::postLIFO(), and QEQueue::get(). Additionally the function
/// QEQueue::init() is used to initialize the queue.
///
/// \note Most event queue operations (both the active object queues and
/// the "raw" queues) internally use  the QF critical section. You should be
/// careful not to invoke those operations from other critical sections when
/// nesting of critical sections is not supported.
class QEQueue {
private:

    /// \brief pointer to event at the front of the queue
    ///
    /// All incoming and outgoing events pass through the m_frontEvt location.
    /// When the queue is empty (which is most of the time), the extra
    /// m_frontEvt location allows to bypass the ring buffer altogether,
    /// greatly optimizing the performance of the queue. Only bursts of events
    /// engage the ring buffer.
    ///
    /// The additional role of this attribute is to indicate the empty status
    /// of the queue. The queue is empty if the m_frontEvt location is NULL.
    QEvt const *m_frontEvt;

    /// \brief pointer to the start of the ring buffer
    QEvt const **m_ring;

    /// \brief offset of the end of the ring buffer from the start of the
    /// buffer m_ring
    QEQueueCtr m_end;

    /// \brief offset to where next event will be inserted into the buffer
    QEQueueCtr m_head;

    /// \brief offset of where next event will be extracted from the buffer
    QEQueueCtr m_tail;

    /// \brief number of free events in the ring buffer
    QEQueueCtr m_nFree;

    /// \brief minimum number of free events ever in the ring buffer.
    ///
    /// \note this attribute remembers the low-watermark of the ring buffer,
    /// which provides a valuable information for sizing event queues.
    /// \sa QF::getQueueMargin().
    QEQueueCtr m_nMin;

public:

    /// \brief Initializes the native QF event queue
    ///
    /// The parameters are as follows: \a qSto[] is the ring buffer storage,
    /// \a qLen is the length of the ring buffer in the units of event-
    /// pointers.
    ///
    /// \note The actual capacity of the queue is qLen + 1, because of the
    /// extra location fornEvt_.
    void init(QEvt const *qSto[], QEQueueCtr const qLen);

    /// \brief "raw" thread-safe QF event queue implementation for the
    /// First-In-First-Out (FIFO) event posting. You can call this function
    /// from any task context or ISR context. Please note that this function
    /// uses internally a critical section.
    ///
    /// \note The function raises an assertion if the native QF queue becomes
    /// full and cannot accept the event.
    ///
    /// \sa QEQueue::postLIFO(), QEQueue::get()
    void postFIFO(QEvt const * const e);

    /// \brief "raw" thread-safe QF event queue implementation for the
    /// First-In-First-Out (FIFO) event posting. You can call this function
    /// from any task context or ISR context. Please note that this function
    ///  uses internally a critical section.
    ///
    /// \note The function raises an assertion if the native QF queue becomes
    /// full and cannot accept the event.
    ///
    /// \sa QEQueue::postLIFO(), QEQueue::get()
    void postLIFO(QEvt const * const e);

    /// \brief "raw" thread-safe QF event queue implementation for the
    /// Last-In-First-Out (LIFO) event posting.
    ///
    /// \note The LIFO policy should be used only with great caution because
    /// it alters order of events in the queue.
    /// \note The function raises an assertion if the native QF queue becomes
    /// full and cannot accept the event. You can call this function from
    /// any task context or ISR context. Please note that this function uses
    /// internally a critical section.
    ///
    /// \sa QEQueue::postFIFO(), QEQueue::get()
    QEvt const *get(void);

    /// \brief "raw" thread-safe QF event queue operation for
    /// obtaining the number of free entries still available in the queue.
    ///
    /// \note This operation needs to be used with caution because the
    /// number of free entries can change unexpectedly. The main intent for
    /// using this operation is in conjunction with event deferral. In this
    /// case the queue is accessed only from a single thread (by a single AO),
    /// so the number of free entries cannot change unexpectedly.
    ///
    /// \sa QActive::defer(), QActive::recall()
    QEQueueCtr getNFree(void) const {
        return m_nFree;
    }

private:
    friend class QF;
    friend class QActive;
};

QP_END_

// "qmpool.h" ================================================================
/// \brief platform-independent memory pool interface.
///
/// This header file must be included in all QF ports that use native QF
/// memory pool implementation.

#ifndef QF_MPOOL_SIZ_SIZE
    /// \brief macro to override the default ::QMPoolSize size.
    /// Valid values 1, 2, or 4; default 2
    #define QF_MPOOL_SIZ_SIZE 2
#endif

#ifndef QF_MPOOL_CTR_SIZE

    /// \brief macro to override the default QMPoolCtr size.
    /// Valid values 1, 2, or 4; default 2
    #define QF_MPOOL_CTR_SIZE 2
#endif

//////////////////////////////////////////////////////////////////////////////
QP_BEGIN_

#if (QF_MPOOL_SIZ_SIZE == 1)
    /// \brief The data type to store the block-size based on the macro
    /// #QF_MPOOL_SIZ_SIZE.
    ///
    /// The dynamic range of this data type determines the maximum size
    /// of blocks that can be managed by the native QF event pool.
    typedef uint8_t QMPoolSize;
#elif (QF_MPOOL_SIZ_SIZE == 2)

    typedef uint16_t QMPoolSize;
#elif (QF_MPOOL_SIZ_SIZE == 4)
    typedef uint32_t QMPoolSize;
#else
    #error "QF_MPOOL_SIZ_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

#if (QF_MPOOL_CTR_SIZE == 1)
    /// \brief The data type to store the block-counter based on the macro
    /// #QF_MPOOL_CTR_SIZE.
    ///
    /// The dynamic range of this data type determines the maximum number
    /// of blocks that can be stored in the pool.
    typedef uint8_t QMPoolCtr;
#elif (QF_MPOOL_CTR_SIZE == 2)
    typedef uint16_t QMPoolCtr;
#elif (QF_MPOOL_CTR_SIZE == 4)
    typedef uint32_t QMPoolCtr;
#else
    #error "QF_MPOOL_CTR_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

//////////////////////////////////////////////////////////////////////////////
/// \brief Native QF memory pool class
///
/// This class describes the native QF memory pool, which can be used as
/// the event pool for dynamic event allocation, or as a fast, deterministic
/// fixed block-size heap for any other objects in your application.
///
/// The ::QMPool structure contains only data members for managing a memory
/// pool, but does not contain the pool storage, which must be provided
/// externally during the pool initialization.
///
/// The native QF event pool is configured by defining the macro
/// #QF_EPOOL_TYPE_ as QEQueue in the specific QF port header file.
class QMPool {
private:

    /// start of the memory managed by this memory pool
    void *m_start;

    /// end of the memory managed by this memory pool
    void *m_end;

    /// linked list of free blocks
    void *m_free;

    ///  maximum block size (in bytes)
    QMPoolSize m_blockSize;

    /// total number of blocks
    QMPoolCtr m_nTot;

    /// number of free blocks remaining
    QMPoolCtr m_nFree;

    /// minimum number of free blocks ever present in this pool
    ///
    /// \note this attribute remembers the low watermark of the pool,
    /// which provides a valuable information for sizing event pools.
    /// \sa QF::getPoolMargin().
    QMPoolCtr m_nMin;

public:

    /// \brief Initializes the native QF event pool
    ///
    /// The parameters are as follows: \a poolSto is the pool storage,
    /// \a poolSize is the size of the pool storage in bytes, and
    /// \a blockSize is the block size of this pool.
    ///
    /// The caller of this method must make sure that the \a poolSto pointer
    /// is properly aligned. In particular, it must be possible to efficiently
    /// store a pointer at the location pointed to by \a poolSto.
    /// Internally, the QMPool::init() function rounds up the block size
    /// \a blockSize so that it can fit an integer number of pointers.
    /// This is done to achieve proper alignment of the blocks within the
    /// pool.
    ///
    /// \note Due to the rounding of block size the actual capacity of the
    /// pool might be less than (\a poolSize / \a blockSize). You can check
    ///  the capacity of the pool by calling the QF::getPoolMargin() function.
    void init(void * const poolSto, uint32_t const poolSize,
              QMPoolSize const blockSize);

    /// \brief Obtains a memory block from a memory pool.
    ///
    /// The only parameter \a me is a pointer to the ::QMPool from which the
    /// block is requested. The function returns a pointer to the allocated
    /// memory block or NULL if no free blocks are available.
    ///
    /// A allocated block must be returned to the same pool from which it has
    /// been allocated.
    ///
    /// This function can be called from any task level or ISR level.
    ///
    /// \note The memory pool \a me must be initialized before any events can
    /// be requested from it. Also, the QMPool::get() function uses internally
    /// a QF critical section, so you should be careful not to call it from
    /// within a critical section when nesting of critical section is not
    /// supported.
    ///
    /// \sa QMPool::put()
    void *get(void);

    /// \brief Returns a memory block back to a memory pool.
    ///
    ///
    /// This function can be called from any task level or ISR level.
    ///
    /// \note The block must be allocated from the same memory pool to which
    /// it is returned. The QMPool::put() function raises an assertion if the
    /// returned pointer to the block points outside of the original memory
    /// buffer managed by the memory pool. Also, the QMPool::put() function
    /// uses internally a QF critical section, so you should be careful not
    /// to call it from within a critical section when nesting of critical
    /// section is not supported.
    ///
    /// \sa QMPool::get()
    void put(void * const b);

    /// \brief return the fixed block-size of the blocks managed by this pool
    QMPoolSize getBlockSize(void) const {
        return m_blockSize;
    }

private:
    friend class QF;
};

QP_END_

/// \brief Memory pool element to allocate correctly aligned storage
/// for QMPool class.
///
#define QF_MPOOL_EL(type_) \
    struct { void *sto_[((sizeof(type_) - 1U)/sizeof(void*)) + 1U]; }

// "qpset.h" =================================================================
/// \brief platform-independent priority sets of 8 or 64 elements.
///
/// This header file must be included in those QF ports that use the
/// cooperative multitasking QF scheduler or the QK.

QP_BEGIN_

//////////////////////////////////////////////////////////////////////////////
// useful lookup tables

/// \brief Lookup table for (log2(n) + 1), where n is the index
/// into the table.
///
/// This lookup delivers the 1-based number of the most significant 1-bit
/// of a byte.
///
/// \note Index range n = 0..255. The first index (n == 0) should never
/// be used.
///
extern uint8_t const Q_ROM Q_ROM_VAR QF_log2Lkup[256];

/// \brief Lookup table for (1 << ((n-1) % 8)), where n is the index
/// into the table.
///
/// \note Index range n = 0..64. The first index (n == 0) should never
/// be used.
extern uint8_t const Q_ROM Q_ROM_VAR QF_pwr2Lkup[65];

/// \brief Lookup table for ~(1 << ((n-1) % 8)), where n is the index
/// into the table.
///
/// \note Index range n = 0..64. The first index (n == 0) should never
/// be used.
extern uint8_t const Q_ROM Q_ROM_VAR QF_invPwr2Lkup[65];

/// \brief Lookup table for (n-1)/8
///
/// \note Index range n = 0..64. The first index (n == 0) should never
/// be used.
extern uint8_t const Q_ROM Q_ROM_VAR QF_div8Lkup[65];


//////////////////////////////////////////////////////////////////////////////
/// \brief Priority Set of up to 8 elements for building various schedulers,
/// but also useful as a general set of up to 8 elements of any kind.
///
/// The priority set represents the set of active objects that are ready to
/// run and need to be considered by scheduling processing. The set is capable
/// of storing up to 8 priority levels.
class QPSet8 {

    //////////////////////////////////////////////////////////////////////////
    /// \brief bimask representing elements of the set
    uint8_t m_bits;

public:

    /// \brief the function evaluates to TRUE if the set is empty,
    /// which means that no active objects are ready to run.
    bool isEmpty(void) const {
        return (m_bits == static_cast<uint8_t>(0));
    }

    /// \brief the function evaluates to TRUE if the set has elements,
    /// which means that some active objects are ready to run.
    bool notEmpty(void) const {
        return (m_bits != static_cast<uint8_t>(0));
    }

    /// \brief the function evaluates to TRUE if the priority set has the
    /// element \a n.
    bool hasElement(uint8_t const n) const {
        return ((m_bits & Q_ROM_BYTE(QF_pwr2Lkup[n]))
                != static_cast<uint8_t>(0));
    }

    /// \brief insert element \a n into the set, n = 1..8
    void insert(uint8_t const n) {
        m_bits |= Q_ROM_BYTE(QF_pwr2Lkup[n]);
    }

    /// \brief remove element \a n from the set, n = 1..8
    void remove(uint8_t const n) {
        m_bits &= Q_ROM_BYTE(QF_invPwr2Lkup[n]);
    }

    /// \brief find the maximum element in the set,
    /// \note returns zero if the set is empty
    uint8_t findMax(void) const {
        return Q_ROM_BYTE(QF_log2Lkup[m_bits]);
    }

    friend class QPSet64;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief Priority Set of up to 64 elements for building various schedulers,
/// but also useful as a general set of up to 64 elements of any kind.
///
/// The priority set represents the set of active objects that are ready to
/// run and need to be considered by scheduling processing. The set is capable
/// of storing up to 64 priority levels.
///
/// The priority set allows to build cooperative multitasking schedulers
/// to manage up to 64 tasks. It is also used in the Quantum Kernel (QK)
/// preemptive scheduler.
///
/// The inherited 8-bit set is used as the 8-elemtn set of of 8-bit subsets
/// Each bit in the super.bits set represents a subset (8-elements)
/// as follows: \n
/// bit 0 in this->m_bits is 1 when subset[0] is not empty \n
/// bit 1 in this->m_bits is 1 when subset[1] is not empty \n
/// bit 2 in this->m_bits is 1 when subset[2] is not empty \n
/// bit 3 in this->m_bits is 1 when subset[3] is not empty \n
/// bit 4 in this->m_bits is 1 when subset[4] is not empty \n
/// bit 5 in this->m_bits is 1 when subset[5] is not empty \n
/// bit 6 in this->m_bits is 1 when subset[6] is not empty \n
/// bit 7 in this->m_bits is 1 when subset[7] is not empty \n
class QPSet64 {

    //////////////////////////////////////////////////////////////////////////
    /// \brief bimask representing 8-element subsets of the set
    uint8_t m_bytes;

    /// \brief bits representing elements in the set as follows: \n
    /// m_bits[0] represent elements  1..8  \n
    /// m_bits[1] represent elements  9..16 \n
    /// m_bits[2] represent elements 17..24 \n
    /// m_bits[3] represent elements 25..32 \n
    /// m_bits[4] represent elements 33..40 \n
    /// m_bits[5] represent elements 41..48 \n
    /// m_bits[6] represent elements 49..56 \n
    /// m_bits[7] represent elements 57..64 \n
    uint8_t m_bits[8];

public:

    /// \brief the function evaluates to TRUE if the set is empty,
    /// which means that no active objects are ready to run.
    bool isEmpty(void) const {
        return (m_bytes == static_cast<uint8_t>(0));
    }

    /// \brief the function evaluates to TRUE if the set has elements,
    /// which means that some active objects are ready to run.
    bool notEmpty(void) const {
        return (m_bytes != static_cast<uint8_t>(0));
    }

    /// \brief the function evaluates to TRUE if the priority set has the
    /// element \a n.
    bool hasElement(uint8_t const n) const {
        uint8_t const m = Q_ROM_BYTE(QF_div8Lkup[n]);
        return ((m_bits[m] & Q_ROM_BYTE(QF_pwr2Lkup[n]))
                != static_cast<uint8_t>(0));
    }

    /// \brief insert element \a n into the set, n = 1..64
    void insert(uint8_t const n) {
        uint8_t m = Q_ROM_BYTE(QF_div8Lkup[n]);
        m_bits[m] |= Q_ROM_BYTE(QF_pwr2Lkup[n]);
        m_bytes |= Q_ROM_BYTE(QF_pwr2Lkup[m + static_cast<uint8_t>(1)]);
    }

    /// \brief remove element \a n from the set, n = 1..64
    void remove(uint8_t const n) {
        uint8_t m = Q_ROM_BYTE(QF_div8Lkup[n]);
        if ((m_bits[m] &= Q_ROM_BYTE(QF_invPwr2Lkup[n]))
             == static_cast<uint8_t>(0))
        {
            m_bytes &=Q_ROM_BYTE(QF_invPwr2Lkup[m + static_cast<uint8_t>(1)]);
        }
    }

    /// \brief find the maximum element in the set,
    /// \note returns zero if the set is empty
    uint8_t findMax(void) const {
        uint8_t n;
        if (m_bytes != static_cast<uint8_t>(0)) {
            n = static_cast<uint8_t>(Q_ROM_BYTE(QF_log2Lkup[m_bytes])
                                     - static_cast<uint8_t>(1));
            n = static_cast<uint8_t>(
                    Q_ROM_BYTE(QF_log2Lkup[m_bits[n]])
                               + static_cast<uint8_t>(n << 3));
        }
        else {
            n = static_cast<uint8_t>(0);
        }
        return n;
    }
};

QP_END_


//////////////////////////////////////////////////////////////////////////////
// Kernel selection based on QK_PREEMPTIVE
//
#ifdef QK_PREEMPTIVE

// "qk.h" ====================================================================
/// \brief This macro defines the type of the event queue used for the
/// active objects.
///
/// \note This is just an example of the macro definition. Typically, you need
/// to define it in the specific QF port file (qf_port.h). In case of QK,
/// which always depends on the native QF queue, this macro is defined at the
/// level of the platform-independent interface qk.h.
#define QF_EQUEUE_TYPE             QEQueue

#if defined(QK_TLS) || defined(QK_EXT_SAVE)
    /// \brief This macro defines the type of the OS-Object used for blocking
    /// the native QF event queue when the queue is empty
    ///
    /// In QK, the OS object is used to hold the per-thread flags, which might
    /// be used, for example, to rembember the thread attributes (e.g.,
    /// if the thread uses a floating point co-processor). The OS object value
    /// is set on per-thread basis in QActive::start(). Later, the extended
    /// context switch macros (QK_EXT_SAVE() and QK_EXT_RESTORE()) might use
    /// the per-thread flags to determine what kind of extended context switch
    /// this particular thread needs (e.g., the thread might not be using the
    /// coprocessor or might be using a different one).
    #define QF_OS_OBJECT_TYPE      uint8_t

    /// \brief This macro defines the type of the thread handle used for the
    /// active objects.
    ///
    /// The thread type in QK is the pointer to the thread-local storage (TLS)
    /// This thread-local storage can be set on per-thread basis in
    /// QActive::start(). Later, the QK scheduler, passes the pointer to the
    /// thread-local storage to the macro #QK_TLS.
    #define QF_THREAD_TYPE         void *
#endif                                                // QK_TLS || QK_EXT_SAVE

#if (QF_MAX_ACTIVE <= 8)
    extern "C" QP_ QPSet8  QK_readySet_;                  ///< ready set of QK
#else
    extern "C" QP_ QPSet64 QK_readySet_;                  ///< ready set of QK
#endif

extern "C" uint8_t QK_currPrio_;          ///< current task/interrupt priority
extern "C" uint8_t QK_intNest_;                   ///< interrupt nesting level

// QK active object queue implementation .....................................

/// \brief Platform-dependent macro defining how QF should block the calling
/// task when the QF native queue is empty
///
/// \note This is just an example of #QACTIVE_EQUEUE_WAIT_ for the QK-port
/// of QF. QK never activates a task that has no events to process, so in this
/// case the macro asserts that the queue is not empty. In other QF ports you
// need to define the macro appropriately for the underlying kernel/OS you're
/// using.
#define QACTIVE_EQUEUE_WAIT_(me_) \
    Q_ASSERT((me_)->m_eQueue.m_frontEvt != static_cast<QEvt const *>(0))

/// \brief Platform-dependent macro defining how QF should signal the
/// active object task that an event has just arrived.
///
/// The macro is necessary only when the native QF event queue is used.
/// The signaling of task involves unblocking the task if it is blocked.
///
/// \note #QACTIVE_EQUEUE_SIGNAL_ is called from a critical section.
/// It might leave the critical section internally, but must restore
/// the critical section before exiting to the caller.
///
/// \note This is just an example of #QACTIVE_EQUEUE_SIGNAL_ for the QK-port
/// of QF. In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QACTIVE_EQUEUE_SIGNAL_(me_) do { \
    QK_readySet_.insert((me_)->m_prio); \
    if (QK_intNest_ == static_cast<uint8_t>(0)) { \
        uint8_t p = QK_schedPrio_(); \
        if (p != static_cast<uint8_t>(0)) { \
            QK_sched_(p); \
        } \
    } \
} while (false)

/// \brief Platform-dependent macro defining the action QF should take
/// when the native QF event queue becomes empty.
///
/// \note #QACTIVE_EQUEUE_ONEMPTY_ is called from a critical section.
/// It should not leave the critical section.
///
/// \note This is just an example of #QACTIVE_EQUEUE_ONEMPTY_ for the QK-port
/// of QF. In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QACTIVE_EQUEUE_ONEMPTY_(me_) \
    QK_readySet_.remove((me_)->m_prio)

// QK event pool operations ..................................................

/// \brief This macro defines the type of the event pool used in this QF port.
///
/// \note This is a specific implementation for the QK-port of QF.
/// In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QF_EPOOL_TYPE_              QMPool

/// \brief Platform-dependent macro defining the event pool initialization
///
/// \note This is a specific implementation for the QK-port of QF.
/// In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
    (p_).init((poolSto_), (poolSize_), \
        static_cast<QMPoolSize>(evtSize_))

/// \brief Platform-dependent macro defining how QF should obtain the
/// event pool block-size
///
/// \note This is a specific implementation for the QK-port of QF.
/// In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QF_EPOOL_EVENT_SIZE_(p_)    \
    static_cast<uint32_t>((p_).getBlockSize())

/// \brief Platform-dependent macro defining how QF should obtain an event
/// \a e_ from the event pool \a p_
///
/// \note This is a specific implementation for the QK-port of QF.
/// In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QF_EPOOL_GET_(p_, e_)       ((e_) = static_cast<QEvt *>((p_).get()))

/// \brief Platform-dependent macro defining how QF should return an event
/// \a e_ to the event pool \a p_
///
/// \note This is a specific implementation for the QK-port of QF.
/// In other QF ports you need to define the macro appropriately for
/// the underlying kernel/OS you're using.
#define QF_EPOOL_PUT_(p_, e_)       ((p_).put(e_))

//////////////////////////////////////////////////////////////////////////////
QP_BEGIN_

#ifndef QK_NO_MUTEX
    //////////////////////////////////////////////////////////////////////////
    /// \brief QK Mutex type.
    ///
    /// QMutex represents the priority-ceiling mutex available in QK.
    /// \sa QK::mutexLock()
    /// \sa QK::mutexUnlock()
    typedef uint8_t QMutex;
#endif                                                          // QK_NO_MUTEX

//////////////////////////////////////////////////////////////////////////////
/// \brief QK services.
///
/// This class groups together QK services. It has only static members and
/// should not be instantiated.
///
// \note The QK scheduler, QK priority, QK ready set, etc. belong conceptually
/// to the QK class (as static class members). However, to avoid C++ potential
/// name-mangling problems in assembly language, these elements are defined
/// outside of the QK class and use the extern "C" linkage specification.
///
class QK {
public:

    /// \brief get the current QK version number string
    ///
    /// \return version of the QK as a constant 6-character string of the
    /// form x.y.zz, where x is a 1-digit major version number, y is a
    /// 1-digit minor version number, and zz is a 2-digit release number.
    static char_t const Q_ROM * Q_ROM_VAR getVersion(void);

    /// \brief QK idle callback (customized in BSPs for QK)
    ///
    /// QK::onIdle() is called continously by the QK idle loop. This callback
    /// gives the application an opportunity to enter a power-saving CPU mode,
    /// or perform some other idle processing.
    ///
    /// \note QK::onIdle() is invoked with interrupts unlocked and must also
    /// return with interrupts unlocked.
    ///
    /// \sa QF::onIdle()
    static void onIdle(void);

#ifndef QK_NO_MUTEX

    /// \brief QK priority-ceiling mutex lock
    ///
    /// Lock the QK scheduler up to the priority level \a prioCeiling.
    ///
    // \note This function should be always paired with QK::mutexUnlock().
    /// The code between QK::mutexLock() and QK::mutexUnlock() should be
    /// kept to the minimum.
    ///
    /// \include qk_mux.cpp
    static QMutex mutexLock(uint8_t const prioCeiling);

    /// \brief QK priority-ceiling mutex unlock
    ///
    /// \note This function should be always paired with QK::mutexLock().
    /// The code between QK::mutexLock() and QK::mutexUnlock() should be
    /// kept to the minimum.
    ///
    /// \include qk_mux.cpp
    static void mutexUnlock(QMutex const mutex);

#endif                                                          // QK_NO_MUTEX

};

QP_END_

//////////////////////////////////////////////////////////////////////////////
extern "C" {

/// \brief QK initialization
///
/// QK_init() is called from QF_init() in qk.cpp. This function is
/// defined in the QK ports.
void QK_init(void);

/// \brief The QK scheduler
///
/// \note The QK scheduler must be always called with the interrupts
/// disabled and enables interrupts internally.
///
/// \sa QK_schedPrio_()
void QK_sched_(uint8_t p);

/// \brief The QK extended scheduler for interrupt context
///
/// \note The QK extended exscheduler must be always called with the
/// interrupts disabled and enables interrupts internally.
///
/// \sa QK_schedPrio_()
void QK_schedExt_(uint8_t p);

/// \brief Find the highest-priority task ready to run
///
/// \note QK_schedPrio_() must be always called with interrupts disabled
/// and returns with interrupts disabled.
uint8_t QK_schedPrio_(void);

}                                                                // extern "C"

#else                                                         // QK_PREEMPTIVE

// "qvanilla.h" ==============================================================
/// \brief platform-independent interface to the cooperative "vanilla" kernel.

QP_BEGIN_
                              // the event queue type for the "Vanilla" kernel
#define QF_EQUEUE_TYPE     QEQueue
                                              // native event queue operations
#define QACTIVE_EQUEUE_WAIT_(me_) \
    Q_ASSERT((me_)->m_eQueue.m_frontEvt != static_cast<QEvt const *>(0))

#define QACTIVE_EQUEUE_SIGNAL_(me_) \
    (QF_readySet_.insert((me_)->m_prio))

#define QACTIVE_EQUEUE_ONEMPTY_(me_) \
    (QF_readySet_.remove((me_)->m_prio))

                                            // native QF event pool operations
#define QF_EPOOL_TYPE_           QMPool
#define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
    (p_).init((poolSto_), (poolSize_), static_cast<QMPoolSize>(evtSize_))
#define QF_EPOOL_EVENT_SIZE_(p_) \
    static_cast<uint32_t>((p_).getBlockSize())
#define QF_EPOOL_GET_(p_, e_)    ((e_) = static_cast<QEvt *>((p_).get()))
#define QF_EPOOL_PUT_(p_, e_)    ((p_).put(e_))

QP_END_

#if (QF_MAX_ACTIVE <= 8)
    extern "C" QP_ QPSet8  QF_readySet_;      ///< ready set of active objects
#else
    extern "C" QP_ QPSet64 QF_readySet_;      ///< ready set of active objects
#endif

extern "C" uint8_t QF_currPrio_;          ///< current task/interrupt priority
extern "C" uint8_t QF_intNest_;                   ///< interrupt nesting level

#endif                                                        // QK_PREEMPTIVE


// qf.h (QF platform-independent public interface) ===========================

/// \brief QF/C++ platform-independent public interface.
///
/// This header file must be included directly or indirectly
/// in all modules (*.cpp files) that use QF/C++.

//////////////////////////////////////////////////////////////////////////////
#ifdef Q_EVT_CTOR
#include <new>                                            // for placement new
#endif

//////////////////////////////////////////////////////////////////////////////
// apply defaults for all undefined configuration parameters
//
#ifndef QF_EVENT_SIZ_SIZE
    /// \brief Default value of the macro configurable value in qf_port.h
    #define QF_EVENT_SIZ_SIZE    2
#endif

#ifndef QF_MAX_EPOOL
    /// \brief Default value of the macro configurable value in qf_port.h
    #define QF_MAX_EPOOL         3
#endif

#ifndef QF_TIMEEVT_CTR_SIZE
    /// \brief macro to override the default QTimeEvtCtr size.
    /// Valid values 1, 2, or 4; default 2
    #define QF_TIMEEVT_CTR_SIZE  2
#endif

#if (QF_MAX_ACTIVE < 1) || (63 < QF_MAX_ACTIVE)
    #error "QF_MAX_ACTIVE not defined or out of range. Valid range is 1..63"
#endif

#ifndef QF_ACTIVE_SUPER_

    //////////////////////////////////////////////////////////////////////////
    /// \brief The macro defining the base class for QActive.
    ///
    /// By default, the ::QActive class is derived from ::QHsm. However,
    /// if the macro QF_ACTIVE_SUPER_ is defined, QActive is derived from
    /// QF_ACTIVE_SUPER_.
    ///
    /// Clients might choose, for example, to define QF_ACTIVE_SUPER_ as QFsm
    /// to avoid the 1-2KB overhead of the hierarchical event processor.
    ///
    /// Clients might also choose to define QF_ACTIVE_SUPER_ as their own
    /// completely customized class that has nothing to do with QHsm or QFsm.
    /// The QF_ACTIVE_SUPER_ class must provide member functions init() and
    /// dispatch(), consistent with the signatures of QHsm and QFsm. But
    /// the implementatin of these functions is completely open.
    #define QF_ACTIVE_SUPER_  QHsm

    /// \brief The argument of the base class' constructor.
    #define QF_ACTIVE_STATE_  QStateHandler

#endif

//////////////////////////////////////////////////////////////////////////////
QP_BEGIN_

//////////////////////////////////////////////////////////////////////////////
#if (QF_EVENT_SIZ_SIZE == 1)

    /// \brief The data type to store the block-size defined based on
    /// the macro #QF_EVENT_SIZ_SIZE.
    ///
    /// The dynamic range of this data type determines the maximum block
    /// size that can be managed by the pool.
    typedef uint8_t QEvtSize;
#elif (QF_EVENT_SIZ_SIZE == 2)
    typedef uint16_t QEvtSize;
#elif (QF_EVENT_SIZ_SIZE == 4)
    typedef uint32_t QEvtSize;
#else
    #error "QF_EVENT_SIZ_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

//////////////////////////////////////////////////////////////////////////////
#if (QF_TIMEEVT_CTR_SIZE == 1)
    /// \brief type of the Time Event counter, which determines the dynamic
    /// range of the time delays measured in clock ticks.
    ///
    /// This typedef is configurable via the preprocessor switch
    /// #QF_TIMEEVT_CTR_SIZE. The other possible values of this type are
    /// as follows: \n
    /// uint8_t when (QF_TIMEEVT_CTR_SIZE == 1), and \n
    /// uint32_t when (QF_TIMEEVT_CTR_SIZE == 4).
    typedef uint8_t QTimeEvtCtr;
#elif (QF_TIMEEVT_CTR_SIZE == 2)
    typedef uint16_t QTimeEvtCtr;
#elif (QF_TIMEEVT_CTR_SIZE == 4)
    typedef uint32_t QTimeEvtCtr;
#else
    #error "QF_TIMEEVT_CTR_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

class QEQueue;                                          // forward declaration

//////////////////////////////////////////////////////////////////////////////
/// \brief Base class for derivation of application-level active object
/// classes.
///
/// QActive is the base class for derivation of active objects. Active objects
/// in QF are encapsulated tasks (each embedding a state machine and an event
/// queue) that communicate with one another asynchronously by sending and
/// receiving events. Within an active object, events are processed
/// sequentially in a run-to-completion (RTC) fashion, while QF encapsulates
/// all the details of thread-safe event exchange and queuing.
///
/// \note QActive is not intended to be instantiated directly, but rather
/// serves as the base class for derivation of active objects in the
/// application code.
///
/// The following example illustrates how to derive an active object from
/// QActive.
/// \include qf_qactive.cpp
///
/// \sa #QF_ACTIVE_SUPER_ defines the base class for QActive
class QActive : public QF_ACTIVE_SUPER_ {

#ifdef QF_EQUEUE_TYPE
    /// \brief OS-dependent event-queue type.
    ///
    /// The type of the queue depends on the underlying operating system or
    /// a kernel. Many kernels support "message queues" that can be adapted
    /// to deliver QF events to the active object. Alternatively, QF provides
    /// a native event queue implementation that can be used as well.
    ///
    /// The native QF event queue is configured by defining the macro
    /// #QF_EQUEUE_TYPE as ::QEQueue.
    QF_EQUEUE_TYPE m_eQueue;
#endif

#ifdef QF_OS_OBJECT_TYPE
    /// \brief OS-dependent per-thread object.
    ///
    /// This data might be used in various ways, depending on the QF port.
    /// In some ports m_osObject is used to block the calling thread when
    /// the native QF queue is empty. In other QF ports the OS-dependent
    /// object might be used differently.
    QF_OS_OBJECT_TYPE m_osObject;
#endif

#ifdef QF_THREAD_TYPE
    /// \brief OS-dependent representation of the thread of the active
    /// object.
    ///
    /// This data might be used in various ways, depending on the QF port.
    /// In some ports m_thread is used store the thread handle. In other ports
    /// m_thread can be a pointer to the Thread-Local-Storage (TLS).
    QF_THREAD_TYPE m_thread;
#endif

    /// \brief QF priority associated with the active object.
    /// \sa QActive::start()
    uint8_t m_prio;

public:
    /// \brief Starts execution of an active object and registers the object
    /// with the framework.
    ///
    /// The function takes six arguments.
    /// \a prio is the priority of the active object. QF allows you to start
    /// up to 63 active objects, each one having a unique priority number
    /// between 1 and 63 inclusive, where higher numerical values correspond
    /// to higher priority (urgency) of the active object relative to the
    /// others.
    /// \a qSto[] and \a qLen arguments are the storage and size of the event
    /// queue used by this active object.
    /// \a stkSto and \a stkSize are the stack storage and size in bytes.
    /// Please note that a per-active object stack is used only when the
    /// underlying OS requies it. If the stack is not required, or the
    /// underlying OS allocates the stack internally, the \a stkSto should be
    /// NULL and/or \a stkSize should be 0.
    /// \a ie is an optional initialization event that can be used to pass
    /// additional startup data to the active object. (Pass NULL if your
    /// active object does not expect the initialization event).
    ///
    /// \note This function is strongly OS-dependent and must be defined in
    /// the QF port to a particular platform.
    ///
    /// The following example shows starting of the Philosopher object when a
    /// per-task stack is required:
    /// \include qf_start.cpp
    void start(uint8_t const prio,
               QEvt const *qSto[], uint32_t const qLen,
               void * const stkSto, uint32_t const stkSize,
               QEvt const * const ie = static_cast<QEvt *>(0));

    /// \brief Posts an event \a e directly to the event queue of the acitve
    /// object \a me using the First-In-First-Out (FIFO) policy.
    ///
    /// Direct event posting is the simplest asynchronous communication method
    /// available in QF. The following example illustrates how the Philosopher
    /// active obejct posts directly the HUNGRY event to the Table active
    /// object. \include qf_post.cpp
    ///
    /// \note The producer of the event (Philosopher in this case) must only
    /// "know" the recipient (Table) by a generic (QActive *QDPP_table)
    /// pointer, but the specific definition of the Table class is not
    /// required.
    ///
    /// \note Direct event posting should not be confused with direct event
    /// dispatching. In contrast to asynchronous event posting through event
    /// queues, direct event dispatching is synchronous. Direct event
    /// dispatching occurs when you call QHsm::dispatch(), or QFsm::dispatch()
    /// function.
#ifndef Q_SPY
    void postFIFO(QEvt const * const e);
#else
    void postFIFO(QEvt const * const e, void const * const sender);
#endif

    /// \brief Un-subscribes from the delivery of all signals to the active
    /// object.
    ///
    /// This function is part of the Publish-Subscribe event delivery
    /// mechanism available in QF. Un-subscribing from all events means that
    /// the framework will stop posting any published events to the event
    /// queue of the active object.
    ///
    /// \note Due to the latency of event queues, an active object should NOT
    /// assume that no events will ever be dispatched to the state machine of
    /// the active object after un-subscribing from all events.
    /// The events might be already in the queue, or just about to be posted
    /// and the un-subscribe operation will not flush such events. Also, the
    /// alternative event-delivery mechanisms, such as direct event posting or
    /// time events, can be still delivered to the event queue of the active
    /// object.
    ///
    /// \sa QF::publish(), QActive::subscribe(), and QActive::unsubscribe()
    void unsubscribeAll(void) const;

protected:
    /// \brief protected constructor
    ///
    /// Performs the first step of active object initialization by assigning
    /// the initial pseudostate to the currently active state of the state
    /// machine.
    ///
    /// \note The constructor is protected to prevent direct instantiation
    /// of QActive objects. This class is intended only for derivation
    /// (abstract class).
    QActive(QF_ACTIVE_STATE_ const initial) : QF_ACTIVE_SUPER_(initial) {
    }

    /// \brief Posts an event directly to the event queue of the active object
    /// \a me using the Last-In-First-Out (LIFO) policy.
    ///
    /// \note The LIFO policy should be used only for self-posting and with
    /// great caution because it alters order of events in the queue.
    ///
    /// \sa QActive::postFIFO()
    void postLIFO(QEvt const * const e);

    /// \brief Stops execution of an active object and removes it from the
    /// framework's supervision.
    ///
    /// The preferred way of calling this function is from within the active
    /// object that needs to stop (that's why this function is protected).
    /// In other words, an active object should stop itself rather than being
    /// stopped by some other entity. This policy works best, because only
    /// the active object itself "knows" when it has reached the appropriate
    /// state for the shutdown.
    ///
    /// \note This function is strongly OS-dependent and should be defined in
    /// the QF port to a particular platform. This function is optional in
    /// embedded systems where active objects never need to be stopped.
    void stop(void);

    /// \brief Subscribes for delivery of signal \a sig to the active object
    ///
    /// This function is part of the Publish-Subscribe event delivery
    /// mechanism available in QF. Subscribing to an event means that the
    /// framework will start posting all published events with a given signal
    /// \a sig to the event queue of the active object.
    ///
    /// The following example shows how the Table active object subscribes
    /// to three signals in the initial transition:
    /// \include qf_subscribe.cpp
    ///
    /// \sa QF::publish(), QActive::unsubscribe(), and
    /// QActive::unsubscribeAll()
    void subscribe(enum_t const sig) const;

    /// \brief Un-subscribes from the delivery of signal \a sig to the
    /// active object.
    ///
    /// This function is part of the Publish-Subscribe event delivery
    /// mechanism available in QF. Un-subscribing from an event means that
    /// the framework will stop posting published events with a given signal
    /// \a sig to the event queue of the active object.
    ///
    /// \note Due to the latency of event queues, an active object should NOT
    /// assume that a given signal \a sig will never be dispatched to the
    /// state machine of the active object after un-subscribing from that
    /// signal. The event might be already in the queue, or just about to be
    /// posted and the un-subscribe operation will not flush such events.
    ///
    /// \note Un-subscribing from a signal that has never been subscribed in
    /// the first place is considered an error and QF will rise an assertion.
    ///
    /// \sa QF::publish(), QActive::subscribe(), and QActive::unsubscribeAll()
    void unsubscribe(enum_t const sig) const;

    /// \brief Defer an event to a given separate event queue.
    ///
    /// This function is part of the event deferral support. An active object
    /// uses this function to defer an event \a e to the QF-supported native
    /// event queue \a eq. QF correctly accounts for another outstanding
    /// reference to the event and will not recycle the event at the end of
    /// the RTC step. Later, the active object might recall one event at a
    /// time from the event queue.
    ///
    /// An active object can use multiple event queues to defer events of
    /// different kinds.
    ///
    /// \sa QActive::recall(), QEQueue
    void defer(QEQueue * const eq, QEvt const * const e) const;

    /// \brief Recall a deferred event from a given event queue.
    ///
    /// This function is part of the event deferral support. An active object
    /// uses this function to recall a deferred event from a given QF
    /// event queue. Recalling an event means that it is removed from the
    /// deferred event queue \a eq and posted (LIFO) to the event queue of
    /// the active object.
    ///
    /// QActive::recall() returns true if an event has been recalled.
    /// Otherwise the function returns false.
    ///
    /// An active object can use multiple event queues to defer events of
    /// different kinds.
    ///
    /// \sa QActive::defer(), QEQueue, QActive::postLIFO()
    bool recall(QEQueue * const eq);

private:

    /// \brief Get an event from the event queue of an active object.
    ///
    /// This function is used internally by a QF port to extract events from
    /// the event queue of an active object. This function depends on the
    /// event queue implementation and is sometimes implemented in the QF port
    /// (qf_port.cpp file). Depending on the underlying OS or kernel, the
    /// function might block the calling thread when no events are available.
    ///
    /// \note QActive::get_() is public because it often needs to be called
    /// from thread-run routines with difficult to foresee signature (so
    /// declaring friendship with such function(s) is not possible.)
    ///
    /// \sa QActive::postFIFO(), QActive::postLIFO()
    QEvt const *get_(void);

    friend class QF;
    friend class QTimeEvt;

#ifdef QK_PREEMPTIVE                                       // is this QK port?
    friend void ::QK_schedExt_(uint8_t p);
    friend void ::QK_sched_(uint8_t p);
#endif

};

//////////////////////////////////////////////////////////////////////////////
/// \brief Time Event class
///
/// Time events are special QF events equipped with the notion of time
/// passage. The basic usage model of the time events is as follows. An
/// active object allocates one or more QTimeEvt objects (provides the
/// storage for them). When the active object needs to arrange for a timeout,
/// it arms one of its time events to fire either just once (one-shot) or
/// periodically. Each time event times out independently from the others,
/// so a QF application can make multiple parallel timeout requests (from the
/// same or different active objects). When QF detects that the appropriate
/// moment has arrived, it inserts the time event directly into the
/// recipient's event queue. The recipient then processes the time event just
/// like any other event.
///
/// Time events, as any other QF events derive from the ::QEvt base
/// class. Typically, you will use a time event as-is, but you can also
/// further derive more specialized time events from it by adding some more
/// data members and/or specialized functions that operate on the specialized
/// time events.
///
/// Internally, the armed time events are organized into a bi-directional
/// linked list. This linked list is scanned in every invocation of the
/// QF::tick() function. Only armed (timing out) time events are in the list,
/// so only armed time events consume CPU cycles.
///
/// \note QF manages the time events in the function QF::tick(), which
/// must be called periodically, preferably from the clock tick ISR.
/// \note In this version of QF QTimeEvt objects should be allocated
/// statically rather than dynamically from event pools. Currently, QF will
/// not correctly recycle the dynamically allocated Time Events.
class QTimeEvt : public QEvt {
private:

    /// link to the next time event in the list
    QTimeEvt *m_next;

    /// the active object that receives the time events.
    QActive *m_act;

    /// the internal down-counter of the time event. The down-counter
    /// is decremented by 1 in every QF_tick() invocation. The time event
    /// fires (gets posted or published) when the down-counter reaches zero.
    QTimeEvtCtr m_ctr;

    /// the interval for the periodic time event (zero for the one-shot
    /// time event). The value of the interval is re-loaded to the internal
    /// down-counter when the time event expires, so that the time event
    /// keeps timing out periodically.
    QTimeEvtCtr m_interval;

public:

    /// \brief The Time Event constructor.
    ///
    /// The most important initialization performed in  the constructor is
    /// assigning a signal to the Time Event. You can reuse the Time Event
    /// any number of times, but you cannot change the signal.
    /// This is because pointers to Time Events might still be held in event
    /// queues and changing signal could to hard-to-detect errors.
    ///
    /// The following example shows the use of QTimeEvt::QTimeEvt()
    /// constructor in the constructor initializer list of the Philosopher
    /// active object constructor that owns the time event
    /// \include qf_ctor.cpp
    QTimeEvt(enum_t const s);

    /// \brief Arm a one-shot time event for direct event posting.
    ///
    /// Arms a time event to fire in \a nTicks clock ticks (one-shot time
    /// event). The time event gets directly posted (using the FIFO policy)
    /// into the event queue of the active object \a act.
    ///
    /// After posting, the time event gets automatically disarmed and can be
    /// reused for a one-shot or periodic timeout requests.
    ///
    /// A one-shot time event can be disarmed at any time by calling the
    /// QTimeEvt::disarm() function. Also, a one-shot time event can be
    /// re-armed to fire in a different number of clock ticks by calling the
    /// QTimeEvt::rearm() function.
    ///
    /// The following example shows how to arm a one-shot time event from a
    /// state machine of an active object:
    /// \include qf_state.cpp
    void postIn(QActive * const act, QTimeEvtCtr const nTicks) {
        m_interval = static_cast<QTimeEvtCtr>(0);
        arm_(act, nTicks);
    }

    /// \brief Arm a periodic time event for direct event posting.
    ///
    /// Arms a time event to fire every \a nTicks clock ticks (periodic time
    /// event). The time event gets directly posted (using the FIFO policy)
    /// into the event queue of the active object \a act.
    ///
    /// After posting, the time event gets automatically re-armed to fire
    /// again in the specified \a nTicks clock ticks.
    ///
    /// A periodic time event can be disarmed only by calling the
    /// QTimeEvt::disarm() function. After disarming, the time event can be
    /// reused for a one-shot or periodic timeout requests.
    ///
    /// \note An attempt to reuse (arm again) a running periodic time event
    /// raises an assertion.
    ///
    /// Also, a periodic time event can be re-armed to shorten or extend the
    /// current period by calling the QTimeEvt_rearm() function. After
    /// adjusting the current period, the periodic time event goes back
    /// timing out at the original rate.
    void postEvery(QActive * const act, QTimeEvtCtr const nTicks) {
        m_interval = nTicks;
        arm_(act, nTicks);
    }

    /// \brief Disarm a time event.
    ///
    /// The time event gets disarmed and can be reused. The function
    /// returns true if the time event was truly disarmed, that is, it
    /// was running. The return of false means that the time event was
    /// not truly disarmed because it was not running. The 'false' return is
    /// only possible for one-shot time events that have been automatically
    /// disarmed upon expiration. In this case the 'false' return means that
    /// the time event has already been posted or published and should be
    /// expected in the active object's state machine.
    bool disarm(void);

    /// \brief Rearm a time event.
    ///
    /// The time event gets rearmed with a new number of clock ticks
    /// \a nTicks. This facility can be used to prevent a one-shot time event
    /// from expiring (e.g., a watchdog time event), or to adjusts the
    /// current period of a periodic time event. Rearming a periodic timer
    /// leaves the interval unchanged and is a convenient method to adjust
    /// the phasing of the periodic time event.
    ///
    /// The function returns true if the time event was running as it
    /// was re-armed. The return of false means that the time event was
    /// not truly rearmed because it was not running. The 'false' return is
    /// only possible for one-shot time events that have been automatically
    /// disarmed upon expiration. In this case the 'false' return means that
    /// the time event has already been posted or published and should be
    /// expected in the active object's state machine.
    bool rearm(QTimeEvtCtr const nTicks);

    /// \brief Get the current value of the down-counter of a time event.
    ///
    /// If the time event is armed, the function returns the current value of
    /// the down-counter of the given time event. If the time event is not
    /// armed, the function returns 0.
    ///
    /// /note The function is thread-safe.
    QTimeEvtCtr ctr(void) const;

private:

    /// \brief Arm a time event (internal function to be used through macros
    /// only).
    ///
    /// \sa QTimeEvt::postIn(), QTimeEvt::postEvery(),
    /// \sa QTimeEvt::publishIn(), QTimeEvt::publishEvery()
    void arm_(QActive * const act, QTimeEvtCtr const nTicks);

    friend class QF;
};


//////////////////////////////////////////////////////////////////////////////
/// \brief The size of the Subscriber list bit array
///
/// The size is determined of the maximum number of active objects in the
/// application configured by the #QF_MAX_ACTIVE macro.
uint8_t const QF_SUBSCR_LIST_SIZE =
    static_cast<uint8_t>(((QF_MAX_ACTIVE - 1) / 8) + 1);

/// \brief Subscriber List class
///
/// This data type represents a set of active objects that subscribe to
/// a given signal. The set is represented as an array of bits, where each
/// bit corresponds to the unique priority of an active object.
class QSubscrList {
private:

    /// An array of bits representing subscriber active objects. Each bit
    /// in the array corresponds to the unique priority of the active object.
    /// The size of the array is determined of the maximum number of active
    /// objects in the application configured by the #QF_MAX_ACTIVE macro.
    /// For example, an active object of priority p is a subscriber if the
    /// following is true: ((m_bits[QF_div8Lkup[p]] & QF_pwr2Lkup[p]) != 0)
    ///
    /// \sa QF::psInit(), QF_div8Lkup, QF_pwr2Lkup, #QF_MAX_ACTIVE
    uint8_t m_bits[QF_SUBSCR_LIST_SIZE];

    friend class QF;
    friend class QActive;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief QF services.
///
/// This class groups together QF services. It has only static members and
/// should not be instantiated.
class QF {
public:

    /// \brief QF initialization.
    ///
    /// This function initializes QF and must be called exactly once before
    /// any other QF function.
    static void init(void);

    /// \brief Publish-subscribe initialization.
    ///
    /// This function initializes the publish-subscribe facilities of QF and
    /// must be called exactly once before any subscriptions/publications
    /// occur in the application. The arguments are as follows: \a subscrSto
    /// is a pointer to the array of subscriber-lists. \a maxSignal is the
    /// dimension of this array and at the same time the maximum signal that
    /// can be published or subscribed.
    ///
    /// The array of subscriber-lists is indexed by signals and provides
    /// mapping between the signals and subscirber-lists. The subscriber-
    /// lists are bitmasks of type ::QSubscrList, each bit in the bitmask
    /// corresponding to the unique priority of an active object. The size
    /// of the ::QSubscrList bitmask depends on the value of the
    /// #QF_MAX_ACTIVE macro.
    ///
    /// \note The publish-subscribe facilities are optional, meaning that
    /// you might choose not to use publish-subscribe. In that case calling
    /// QF::psInit() and using up memory for the subscriber-lists is
    /// unnecessary.
    ///
    /// \sa ::QSubscrList
    ///
    /// The following example shows the typical initialization sequence of
    /// QF: \include qf_main.cpp
    static void psInit(QSubscrList * const subscrSto,
                       uint32_t const maxSignal);

    /// \brief Event pool initialization for dynamic allocation of events.
    ///
    /// This function initializes one event pool at a time and must be called
    /// exactly once for each event pool before the pool can be used.
    /// The arguments are as follows: \a poolSto is a pointer to the memory
    /// block for the events. \a poolSize is the size of the memory block in
    /// bytes. \a evtSize is the block-size of the pool in bytes, which
    /// determines the maximum size of events that can be allocated from the
    /// pool.
    ///
    /// You might initialize one, two, and up to three event pools by making
    /// one, two, or three calls to the QF_poolInit() function. However,
    /// for the simplicity of the internal implementation, you must initialize
    /// event pools in the ascending order of the event size.
    ///
    /// Many RTOSes provide fixed block-size heaps, a.k.a. memory pools that
    /// can be used for QF event pools. In case such support is missing, QF
    /// provides a native QF event pool implementation. The macro
    /// #QF_EPOOL_TYPE_ determines the type of event pool used by a
    /// particular QF port. See class ::QMPool for more information.
    ///
    /// \note The actual number of events available in the pool might be
    /// actually less than (\a poolSize / \a evtSize) due to the internal
    /// alignment of the blocks that the pool might perform. You can always
    /// check the capacity of the pool by calling QF::getPoolMargin().
    ///
    /// \note The dynamic allocation of events is optional, meaning that you
    /// might choose not to use dynamic events. In that case calling
    /// QF::poolInit() and using up memory for the memory blocks is
    /// unnecessary.
    ///
    /// \sa QF initialization example for QF::init()
    static void poolInit(void * const poolSto, uint32_t const poolSize,
                         uint32_t const evtSize);

    /// \brief Transfers control to QF to run the application.
    ///
    /// QF::run() is typically called from your startup code after you
    /// initialize the QF and start at least one active object with
    /// QActive::start(). Also, QF::start() call must precede the transfer
    /// of control to QF::run(), but some QF ports might call QF::start()
    /// from QF::run(). QF::run() typically never returns to the caller,
    /// but when it does, it returns the error code (0 for success)
    ///
    /// \note This function is strongly platform-dependent and is not
    /// implemented in the QF, but either in the QF port or in the
    /// Board Support Package (BSP) for the given application. All QF ports
    /// must implement QF::run().
    ///
    /// \note When the Quantum Kernel (QK) is used as the underlying real-time
    /// kernel for the QF, all platfrom dependencies are handled in the QK, so
    /// no porting of QF is necessary. In other words, you only need to
    /// recompile the QF platform-independent code with the compiler for your
    /// platform, but you don't need to provide any platform-specific
    /// implementation (so, no qf_port.cpp file is necessary). Moreover, QK
    /// implements the function QF::run() in a platform-independent way,
    /// in the modile qk.cpp.
    static int16_t run(void);

    /// \brief Startup QF callback.
    ///
    /// The timeline for calling QF::onStartup() depends on the particular
    /// QF port. In most cases, QF::onStartup() is called from QF::run(),
    /// right before starting any multitasking kernel or the background loop.
    static void onStartup(void);

    /// \brief Cleanup QF callback.
    ///
    /// QF::onCleanup() is called in some QF ports before QF returns to the
    /// underlying operating system or RTOS.
    ///
    /// This function is strongly platform-specific and is not implemented in
    /// the QF but either in the QF port or in the Board Support Package (BSP)
    /// for the given application. Some QF ports might not require
    /// implementing QF::onCleanup() at all, because many embedded
    /// applications don't have anything to exit to.
    ///
    /// \sa QF::init() and QF::stop()
    static void onCleanup(void);

    /// \brief QF idle callback (customized in BSPs for QF)
    ///
    /// QF::onIdle() is called by the non-preemptive "Vanilla" scheduler built
    /// into QF when the framework detects that no events are available for
    /// active objects (the idle condition). This callback gives the
    /// application an opportunity to enter a power-saving CPU mode, or
    /// perform some other idle processing (such as Q-Spy output).
    ///
    /// \note QF_onIdle() is invoked with interrupts DISABLED because the idle
    /// condition can be asynchronously changed at any time by an interrupt.
    /// QF_onIdle() MUST enable the interrupts internally, but not before
    /// putting the CPU into the low-power mode. (Ideally, enabling interrupts
    /// and low-power mode should happen atomically). At the very least, the
    /// function MUST enable interrupts, otherwise interrups will remain
    /// disabled permanently.
    ///
    /// \note QF::onIdle() is only used by the non-preemptive "Vanilla"
    /// scheduler in the "bare metal" QF port, and is NOT used in any other
    /// QF ports. When QF is combined with QK, the QK idle loop calls a
    /// different function QK::onIdle(), with different semantics than
    /// QF::onIdle(). When QF is combined with a 3rd-party RTOS or kernel, the
    /// idle processing mechanism of the RTOS or kernal is used instead of
    /// QF::onIdle().
    ///
    static void onIdle(void);

    /// \brief Function invoked by the application layer to stop the QF
    /// application and return control to the OS/Kernel.
    ///
    /// This function stops the QF application. After calling this function,
    /// QF attempts to gracefully stop the  application. This graceful
    /// shutdown might take some time to complete. The typical use of this
    /// funcition is for terminating the QF application to return back to the
    /// operating system or for handling fatal errors that require shutting
    /// down (and possibly re-setting) the system.
    ///
    /// This function is strongly platform-specific and is not implemented in
    /// the QF but either in the QF port or in the Board Support Package (BSP)
    /// for the given application. Some QF ports might not require
    /// implementing QF::stop() at all, because many embedded application
    /// don't have anything to exit to.
    ///
    /// \sa QF::stop() and QF::onCleanup()
    static void stop(void);

    /// \brief Publish event to the framework.
    ///
    /// This function posts (using the FIFO policy) the event \a e it to ALL
    /// active object that have subscribed to the signal \a e->sig.
    /// This function is designed to be callable from any part of the system,
    /// including ISRs, device drivers, and active objects.
    ///
    /// In the general case, event publishing requires multi-casting the
    /// event to multiple subscribers. This happens in the caller's thread
    /// with the scheduler locked to prevent preemptions during the multi-
    /// casting process. (Please note that the interrupts are not locked.)
#ifndef Q_SPY
    static void publish(QEvt const *e);
#else
    static void publish(QEvt const *e, void const *sender);
#endif

    /// \brief Processes all armed time events at every clock tick.
    ///
    /// This function must be called periodically from a time-tick ISR or from
    /// the highest-priority task so that QF can manage the timeout events.
    ///
    /// \note The QF::tick() function is not reentrant meaning that it must
    /// run to completion before it is called again. Also, QF::tick() assumes
    /// that it never will get preempted by a task, which is always the case
    /// when it is called from an ISR or the highest-priority task.
    ///
    /// \sa ::QTimeEvt.
    ///
    /// The following example illustrates the call to QF::tick():
    /// \include qf_tick.cpp
#ifndef Q_SPY
    static void tick(void);
#else
    static void tick(void const * const sender);
#endif

    /// \brief Returns true if all time events are inactive and false
    /// any time event is active.
    ///
    /// \note This function should be called in critical section.
    static bool noTimeEvtsActive(void);

    /// \brief Returns the QF version.
    ///
    /// This function returns constant version string in the format x.y.zz,
    /// where x (one digit) is the major version, y (one digit) is the minor
    /// version, and zz (two digits) is the maintenance release version.
    /// An example of the version string is "3.1.03".
    ///
    /// The following example illustrates the usage of this function:
    /// \include qf_version.cpp
    static char_t const Q_ROM * Q_ROM_VAR getVersion(void);

    /// \brief This function returns the margin of the given event pool.
    ///
    /// This function returns the margin of the given event pool \a poolId,
    /// where poolId is the ID of the pool initialized by the call to
    /// QF::poolInit(). The poolId of the first initialized pool is 1, the
    /// second 2, and so on.
    ///
    /// The returned pool margin is the minimal number of free blocks
    /// encountered in the given pool since system startup.
    ///
    /// \note Requesting the margin of an un-initialized pool raises an
    /// assertion in the QF.
    static uint32_t getPoolMargin(uint8_t const poolId);

    /// \brief This function returns the margin of the given event queue.
    ///
    /// This function returns the margin of the given event queue of an active
    /// object with priority \a prio. (QF priorities start with 1 and go up to
    /// #QF_MAX_ACTIVE.) The margin is the minimal number of free events
    /// encountered in the given queue since system startup.
    ///
    /// \note QF::getQueueMargin() is available only when the native QF event
    /// queue implementation is used. Requesting the queue margin of an unused
    /// priority level raises an assertion in the QF. (A priority level
    /// becomes used in QF after the call to the QF::add_() function.)
    static uint32_t getQueueMargin(uint8_t const prio);

    /// \brief Internal QF implementation of the dynamic event allocator.
    ///
    /// \note The application code should not call this function directly.
    /// Please use the macro #Q_NEW.
    static QEvt *new_(QEvtSize const evtSize, enum_t const sig);

    /// \brief Recycle a dynamic event.
    ///
    /// This function implements a simple garbage collector for the dynamic
    /// events. Only dynamic events are candidates for recycling. (A dynamic
    /// event is one that is allocated from an event pool, which is
    /// determined as non-zero e->attrQF__ attribute.) Next, the function
    /// decrements the reference counter of the event, and recycles the event
    /// only if the counter drops to zero (meaning that no more references
    /// are outstanding for this event). The dynamic event is recycled by
    /// returning it to the pool from which it was originally allocated.
    /// The pool-of-origin information is stored in the upper 2-MSBs of the
    /// e->attrQF__ attribute.)
    ///
    /// \note QF invokes the garbage collector at all appropriate contexts,
    /// when an event can become garbage (automatic garbage collection),
    /// so the application code should have NO need to call QF::gc() directly.
    /// The QF::gc() function is exposed only for special cases when your
    /// application sends dynamic events to the "raw" thread-safe queues
    /// (see ::QEQueue). Such queues are processed outside of QF and the
    /// automatic garbage collection CANNOT be performed for these events.
    /// In this case you need to call QF::gc() explicitly.
    static void gc(QEvt const *e);

private:                              // functions to be used in QF ports only

    /// \brief Register an active object to be managed by the framework
    ///
    /// This function should not be called by the application directly, only
    /// through the function QActive::start(). The priority of the active
    /// object \a a should be set before calling this function.
    ///
    /// \note This function raises an assertion if the priority of the active
    /// object exceeds the maximum value #QF_MAX_ACTIVE. Also, this function
    /// raises an assertion if the priority of the active object is already in
    /// use. (QF requires each active object to have a UNIQUE priority.)
    static void add_(QActive * const a);

public:
    /// \brief Remove the active object from the framework.
    ///
    /// This function should not be called by the application directly, only
    /// inside the QF port. The priority level occupied by the active object
    /// is freed-up and can be reused for another active object.
    ///
    /// The active object that is removed from the framework can no longer
    /// participate in the publish-subscribe event exchange.
    ///
    /// \note This function raises an assertion if the priority of the active
    /// object exceeds the maximum value #QF_MAX_ACTIVE or is not used.
    static void remove_(QActive const * const a);

    /// \brief array of registered active objects
    ///
    /// \note Not to be used by Clients directly, only in ports of QF
    static QActive *active_[];

    /// \brief Thread routine for executing an active object \a act.
    ///
    /// This function is actually implemented internally by certain QF ports
    /// to be called by the active object thread routine.
    static void thread_(QActive *act);

    friend class QActive;
};

QP_END_

//////////////////////////////////////////////////////////////////////////////
#ifdef Q_EVT_CTOR               // Provide the constructor for the QEvt class?

    /// \brief Allocate a dynamic event.
    ///
    /// This macro returns an event pointer cast to the type \a evtT_. The
    /// event is initialized with the signal \a sig. Internally, the macro
    /// calls the internal QF function QF::new_(), which always returns a
    /// valid event pointer.
    ///
    /// \note The internal QF function QF::new_() raises an assertion when
    /// the allocation of the event turns out to be impossible due to
    /// event pool depletion, or incorrect (too big) size of the requested
    /// event.
    ///
    /// The following example illustrates dynamic allocation of an event:
    /// \include qf_post.cpp
    #define Q_NEW(evtT_, sig_, ...) \
        (new(QP_ QF::new_( \
            static_cast<QP_ QEvtSize>(sizeof(evtT_)), (sig_))) \
            evtT_((sig_),  ##__VA_ARGS__))
#else                                    // QEvt is a POD (Plain Old Datatype)

    #define Q_NEW(evtT_, sig_) \
        (static_cast<evtT_ *>( \
            QP_ QF::new_(static_cast<QP_ QEvtSize>(sizeof(evtT_)), (sig_))))
#endif

//////////////////////////////////////////////////////////////////////////////
// QS software tracing integration, only if enabled
#ifdef Q_SPY                                   // QS software tracing enabled?

    /// \brief Invoke the system clock tick processing QF::tick(). This macro
    /// is the recommended way of invoking clock tick processing, because it
    /// provides the vital information for software tracing and avoids any
    /// overhead when the tracing is disabled.
    ///
    /// This macro takes the argument \a sender_, which is a pointer to the
    /// sender object. This argument is actually only used when QS software
    /// tracing is enabled (macro #Q_SPY is defined). When QS software
    /// tracing is disabled, the macro calls QF::tick() without any
    /// arguments, so the overhead of passing this extra argument is
    /// entirely avoided.
    ///
    /// \note the pointer to the sender object is not necessarily a poiner
    /// to an active object. In fact, typically QF::TICK() will be called from
    /// an interrupt, in which case you would create a unique object just to
    /// unambiguously identify the ISR as the sender of the time events.
    ///
    /// \sa QF::tick()
    #define TICK(sender_)                  tick(sender_)

    /// \brief Invoke the event publishing facility QF::publish(). This macro
    /// is the recommended way of publishing events, because it provides the
    /// vital information for software tracing and avoids any overhead when
    /// the tracing is disabled.
    ///
    ///
    /// This macro takes the last argument \a sender_, which is a pointer to
    /// the sender object. This argument is actually only used when QS
    /// tracing is enabled (macro #Q_SPY is defined). When QS software
    /// tracing is disabled, the macro calls QF::publish() without the
    /// \a sender_ argument, so the overhead of passing this extra argument
    /// is entirely avoided.
    ///
    /// \note the pointer to the sender object is not necessarily a poiner
    /// to an active object. In fact, if QF::PUBLISH() is called from an
    /// interrupt or other context, you can create a unique object just to
    /// unambiguously identify the publisher of the event.
    ///
    /// \sa QF::publish()
    #define PUBLISH(e_, sender_)           publish((e_), (sender_))

    /// \brief Invoke the direct event posting facility QActive::postFIFO().
    /// This macro is the recommended way of posting events, because it
    /// provides the vital information for software tracing and avoids any
    /// overhead when the tracing is disabled.
    ///
    ///
    /// This macro takes the last argument \a sender_, which is a pointer to
    /// the sender object. This argument is actually only used when QS
    /// tracing is disabled (macro #Q_SPY is defined). When QS software
    /// tracing is not enabled, the macro calls QF_publish() without the
    /// \a sender_ argument, so the overhead of passing this extra argument
    /// is entirely avoided.
    ///
    /// \note the pointer to the sender object is not necessarily a poiner
    /// to an active object. In fact, if ao->POST() is called from an
    /// interrupt or other context, you can create a unique object just to
    /// unambiguously identify the publisher of the event.
    ///
    /// \sa QActive::postFIFO()
    #define POST(e_, sender_)              postFIFO((e_), (sender_))

#else

    #define TICK(dummy_)                   tick()
    #define PUBLISH(e_, dummy_)            publish(e_)
    #define POST(e_, dummy_)               postFIFO(e_)

#endif                                                                // Q_SPY

//////////////////////////////////////////////////////////////////////////////
// QS software tracing
#ifdef Q_SPY

// qs.h ======================================================================
/// \brief QS/C++ platform-independent public interface.
/// This header file must be included directly or indirectly
/// in all modules (*.cpp files) that use QS/C++.

#ifndef Q_SPY
    #error "Q_SPY must be defined to include qs.h"
#endif

#ifndef Q_ROM                      // provide the default if Q_ROM NOT defined
    #define Q_ROM
#endif
#ifndef Q_ROM_VAR              // provide the default if Q_ROM_VAR NOT defined
    #define Q_ROM_VAR
#endif
#ifndef Q_ROM_BYTE            // provide the default if Q_ROM_BYTE NOT defined
    #define Q_ROM_BYTE(rom_var_)   (rom_var_)
#endif

#ifndef QS_TIME_SIZE

    /// \brief The size (in bytes) of the QS time stamp. Valid values: 1, 2,
    /// or 4; default 4.
    ///
    /// This macro can be defined in the QS port file (qs_port.h) to
    /// configure the ::QSTimeCtr type. Here the macro is not defined so the
    /// default of 4 byte is chosen.
    #define QS_TIME_SIZE 4
#endif

//////////////////////////////////////////////////////////////////////////////
QP_BEGIN_

/// \brief Quantum Spy record types.
///
/// The following constants specify the QS record types used in QP components.
/// You can specify your own record types starting from the ::QS_USER offset.
/// Currently, the maximum of all records cannot exceed 256.
/// \sa QS::filterOn()/#QS_FILTER_ON and QS::filterOff()/#QS_FILTER_OFF

enum QSpyRecords {
    QS_QP_RESET,                 ///< reset the QP (start of a new QS session)

    // QEP records
    QS_QEP_STATE_ENTRY,                               ///< a state was entered
    QS_QEP_STATE_EXIT,                                 ///< a state was exited
    QS_QEP_STATE_INIT,          ///< an intial transition was taken in a state
    QS_QEP_INIT_TRAN,           ///< the top-most initial transition was taken
    QS_QEP_INTERN_TRAN,                  ///< an internal transition was taken
    QS_QEP_TRAN,                           ///< a regular transition was taken
    QS_QEP_IGNORED,             ///< an event was ignored (silently discarded)
    QS_QEP_DISPATCH,          ///< an event was dispatched (begin of RTC step)
    QS_QEP_UNHANDLED,               ///< an event was unhandled due to a guard

    // QF records
    QS_QF_ACTIVE_ADD,                ///< an AO has been added to QF (started)
    QS_QF_ACTIVE_REMOVE,         ///< an AO has been removed from QF (stopped)
    QS_QF_ACTIVE_SUBSCRIBE,                  ///< an AO subscribed to an event
    QS_QF_ACTIVE_UNSUBSCRIBE,              ///< an AO unsubscribed to an event
    QS_QF_ACTIVE_POST_FIFO,  ///< an event was posted (FIFO) directly to an AO
    QS_QF_ACTIVE_POST_LIFO,  ///< an event was posted (LIFO) directly to an AO
    QS_QF_ACTIVE_GET, ///< an AO got an event and its queue is still not empty
    QS_QF_ACTIVE_GET_LAST,      ///< an AO got an event and its queue is empty
    QS_QF_EQUEUE_INIT,                     ///< an event queue was initialized
    QS_QF_EQUEUE_POST_FIFO,     ///< an event was posted (FIFO) to a raw queue
    QS_QF_EQUEUE_POST_LIFO,     ///< an event was posted (LIFO) to a raw queue
    QS_QF_EQUEUE_GET,              ///< get an event and queue still not empty
    QS_QF_EQUEUE_GET_LAST,              ///< get the last event from the queue
    QS_QF_MPOOL_INIT,                       ///< a memory pool was initialized
    QS_QF_MPOOL_GET,        ///< a memory block was removed from a memory pool
    QS_QF_MPOOL_PUT,         ///< a memory block was returned to a memory pool
    QS_QF_PUBLISH,       ///< an event was truly published to some subscribers
    QS_QF_RESERVED8,
    QS_QF_NEW,                                         ///< new event creation
    QS_QF_GC_ATTEMPT,                          ///< garbage collection attempt
    QS_QF_GC,                                          ///< garbage collection
    QS_QF_TICK,                                     ///< QF::tick() was called
    QS_QF_TIMEEVT_ARM,                             ///< a time event was armed
    QS_QF_TIMEEVT_AUTO_DISARM,      ///< a time event expired and was disarmed
    QS_QF_TIMEEVT_DISARM_ATTEMPT,///< an attempt to disarmed a disarmed tevent
    QS_QF_TIMEEVT_DISARM,           ///< true disarming of an armed time event
    QS_QF_TIMEEVT_REARM,                         ///< rearming of a time event
    QS_QF_TIMEEVT_POST,      ///< a time event posted itself directly to an AO
    QS_QF_TIMEEVT_CTR,                 ///< a time event counter was requested
    QS_QF_CRIT_ENTRY,                        ///< critical section was entered
    QS_QF_CRIT_EXIT,                          ///< critical section was exited
    QS_QF_ISR_ENTRY,                                   ///< an ISR was entered
    QS_QF_ISR_EXIT,                                     ///< an ISR was exited
    QS_QF_INT_DISABLE,                           ///< interrupts were disabled
    QS_QF_INT_ENABLE,                             ///< interrupts were enabled
    QS_QF_RESERVED4,
    QS_QF_RESERVED3,
    QS_QF_RESERVED2,
    QS_QF_RESERVED1,
    QS_QF_RESERVED0,

    // QK records
    QS_QK_MUTEX_LOCK,                             ///< the QK mutex was locked
    QS_QK_MUTEX_UNLOCK,                         ///< the QK mutex was unlocked
    QS_QK_SCHEDULE,      ///< the QK scheduler scheduled a new task to execute
    QS_QK_RESERVED6,
    QS_QK_RESERVED5,
    QS_QK_RESERVED4,
    QS_QK_RESERVED3,
    QS_QK_RESERVED2,
    QS_QK_RESERVED1,
    QS_QK_RESERVED0,

    // Miscellaneous QS records
    QS_SIG_DIC,                                   ///< signal dictionary entry
    QS_OBJ_DIC,                                   ///< object dictionary entry
    QS_FUN_DIC,                                 ///< function dictionary entry
    QS_USR_DIC,                           ///< user QS record dictionary entry
    QS_RESERVED4,
    QS_RESERVED3,
    QS_RESERVED2,
    QS_RESERVED1,
    QS_RESERVED0,
    QS_ASSERT,                                ///< assertion fired in the code

    // User records
    QS_USER                ///< the first record available for user QS records
};

/// \brief Specification of all QS records for the QS::filterOn() and
/// QS::filterOff()
uint8_t const QS_ALL_RECORDS = static_cast<uint8_t>(0xFF);

/// \brief Constant representing End-Of-Data condition returned from the
/// QS::getByte() function.
uint16_t const QS_EOD  = static_cast<uint16_t>(0xFFFF);


#if (QS_TIME_SIZE == 1)
    typedef uint8_t QSTimeCtr;
    #define QS_TIME_()   (QP_ QS::u8_(QP_ QS::onGetTime()))
#elif (QS_TIME_SIZE == 2)
    typedef uint16_t QSTimeCtr;
    #define QS_TIME_()   (QP_ QS::u16_(QP_ QS::onGetTime()))
#elif (QS_TIME_SIZE == 4)

    /// \brief The type of the QS time stamp
    ///
    /// This type determines the dynamic range of QS time stamps
    typedef uint32_t QSTimeCtr;

    /// \brief Internal macro to output time stamp to the QS record
    #define QS_TIME_()   (QP_ QS::u32_(QP_ QS::onGetTime()))
#else
    #error "QS_TIME_SIZE defined incorrectly, expected 1, 2, or 4"
#endif

/// \brief Quantum Spy logging facilities
///
/// This class groups together QS services. It has only static members and
/// should not be instantiated.
class QS {
public:

    /// \brief Get the current version of QS
    ///
    /// \return version of the QS as a constant 6-character string of the form
    /// x.y.zz, where x is a 1-digit major version number, y is a 1-digit
    /// minor version number, and zz is a 2-digit release number.
    static char_t const Q_ROM * Q_ROM_VAR getVersion(void);

    /// \brief Initialize the QS data buffer.
    ///
    /// This function should be called from QS_init() to provide QS with the
    /// data buffer. The first argument \a sto[] is the address of the memory
    /// block, and the second argument \a stoSize is the size of this block
    /// in bytes. Currently the size of the QS buffer cannot exceed 64KB.
    ///
    /// QS can work with quite small data buffers, but you will start losing
    /// data if the buffer is too small for the bursts of logging activity.
    /// The right size of the buffer depends on the data production rate and
    /// the data output rate. QS offers flexible filtering to reduce the data
    /// production rate.
    ///
    /// \note If the data output rate cannot keep up with the production rate,
    /// QS will start overwriting the older data with newer data. This is
    /// consistent with the "last-is-best" QS policy. The record sequence
    ///  counters and checksums on each record allow to easily detect data
    /// loss.
    static void initBuf(uint8_t sto[], uint32_t const stoSize);

    /// \brief Turn the global Filter on for a given record type \a rec.
    ///
    /// This function sets up the QS filter to enable the record type \a rec.
    /// The argument #QS_ALL_RECORDS specifies to filter-on all records.
    /// This function should be called indirectly through the macro
    /// #QS_FILTER_ON.
    ///
    /// \note Filtering based on the record-type is only the first layer of
    /// filtering. The second layer is based on the object-type. Both filter
    /// layers must be enabled for the QS record to be inserted into the QS
    /// buffer.
    /// \sa QS_filterOff(), #QS_FILTER_SM_OBJ, #QS_FILTER_AO_OBJ,
    /// #QS_FILTER_MP_OBJ, #QS_FILTER_EQ_OBJ, and #QS_FILTER_TE_OBJ.
    static void filterOn(uint8_t const rec);

    /// \brief Turn the global Filter off for a given record type \a rec.
    ///
    /// This function sets up the QS filter to disable the record type \a rec.
    /// The argument #QS_ALL_RECORDS specifies to suppress all records.
    /// This function should be called indirectly through the macro
    /// #QS_FILTER_OFF.
    ///
    /// \note Filtering records based on the record-type is only the first
    /// layer of filtering. The second layer is based on the object-type.
    /// Both filter layers must be enabled for the QS record to be inserted
    /// into the QS buffer.
    /// \sa
    static void filterOff(uint8_t const rec);

    /// \brief Mark the begin of a QS record \a rec
    ///
    /// This function must be called at the beginning of each QS record.
    /// This function should be called indirectly through the macro #QS_BEGIN,
    /// or #QS_BEGIN_NOCRIT, depending if it's called in a normal code or from
    /// a critical section.
    static void begin(uint8_t const rec);

    /// \brief Mark the end of a QS record \a rec
    ///
    /// This function must be called at the end of each QS record.
    /// This function should be called indirectly through the macro #QS_END,
    /// or #QS_END_NOCRIT, depending if it's called in a normal code or from
    /// a critical section.
    static void end(void);

    // unformatted data elements output ......................................

    /// \brief output uint8_t data element without format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u8_(uint8_t const d);

    /// \brief Output uint16_t data element without format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u16_(uint16_t d);

    /// \brief Output uint32_t data element without format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u32_(uint32_t d);

    /// \brief Output zero-terminated ASCII string element without format
    /// information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void str_(char_t const *s);

    /// \brief Output zero-terminated ASCII string element  allocated in ROM
    /// without format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void str_ROM_(char_t const Q_ROM * Q_ROM_VAR s);

    // formatted data elements output ........................................

    /// \brief Output uint8_t data element with format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u8(uint8_t const format, uint8_t const d);

    /// \brief output uint16_t data element with format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u16(uint8_t const format, uint16_t d);

    /// \brief Output uint32_t data element with format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u32(uint8_t const format, uint32_t d);

    /// \brief Output 32-bit floating point data element with format
    /// information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void f32(uint8_t const format, float32_t const d);

    /// \brief Output 64-bit floating point data element with format
    /// information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void f64(uint8_t const format, float64_t const d);

    /// \brief Output zero-terminated ASCII string element with format
    /// information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void str(char_t const *s);

    /// \brief Output zero-terminated ASCII string element allocated in ROM
    /// with format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void str_ROM(char_t const Q_ROM * Q_ROM_VAR s);

    /// \brief Output memory block of up to 255-bytes with format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void mem(uint8_t const *blk, uint8_t size);

#if (QS_OBJ_PTR_SIZE == 8) || (QS_FUN_PTR_SIZE == 8)
    /// \brief Output uint64_t data element without format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u64_(uint64_t d);

    /// \brief Output uint64_t data element with format information
    /// \note This function is only to be used through macros, never in the
    /// client code directly.
    static void u64(uint8_t format, uint64_t d);
#endif

    // QS buffer access ......................................................

    /// \brief Byte-oriented interface to the QS data buffer.
    ///
    /// This function delivers one byte at a time from the QS data buffer.
    /// The function returns the byte in the least-significant 8-bits of the
    /// 16-bit return value if the byte is available. If no more data is
    /// available at the time, the function returns QS_EOD (End-Of-Data).
    ///
    /// \note QS::getByte() is NOT protected with a critical section.
    static uint16_t getByte(void);

    /// \brief Block-oriented interface to the QS data buffer.
    ///
    /// This function delivers a contiguous block of data from the QS data
    /// buffer. The function returns the pointer to the beginning of the
    /// block, and writes the number of bytes in the block to the location
    /// pointed to by \a pNbytes. The argument \a pNbytes is also used as
    /// input to provide the maximum size of the data block that the caller
    /// can accept.
    ///
    /// If no bytes are available in the QS buffer when the function is
    /// called, the function returns a NULL pointer and sets the value
    /// pointed to by \a pNbytes to zero.
    ///
    /// \note Only the NULL return from QS::getBlock() indicates that the QS
    /// buffer is empty at the time of the call. The non-NULL return often
    /// means that the block is at the end of the buffer and you need to call
    /// QS::getBlock() again to obtain the rest of the data that "wrapped
    /// around" to the beginning of the QS data buffer.
    ///
    /// \note QS::getBlock() is NOT protected with a critical section.
    static uint8_t const *getBlock(uint16_t * const pNbytes);

    //////////////////////////////////////////////////////////////////////////
    // platform-dependent callback functions to be implemented by clients

    // platform-specific callback functions, need to be implemented by clients
    /// \brief Callback to startup the QS facility
    ///
    /// This is a platform-dependent "callback" function invoked through the
    /// macro #QS_INIT. You need to implement this function in your
    /// application. At a minimum, the function must configure the QS buffer
    /// by calling QS::initBuf(). Typically, you will also want to open/
    /// configure the QS output channel, such as a serial port, or a file.
    /// The void* argument \a arg can be used to pass parameter(s) needed to
    /// configure the output channel.
    ///
    /// The function returns true if the QS initialization was successful,
    /// or false if it failed.
    ///
    /// The following example illustrates an implementation of QS_onStartup():
    /// \include qs_startup.cpp
    static bool onStartup(void const *arg);

    /// \brief Callback to cleanup the QS facility
    ///
    /// This is a platform-dependent "callback" function invoked through the
    /// macro #QS_EXIT. You need to implement this function in your
    /// application. The main purpose of this function is to close the QS
    /// output channel, if necessary.
    static void onCleanup(void);

    /// \brief Callback to flush the QS trace data to the host
    ///
    /// This is a platform-dependent "callback" function to flush the QS
    /// trace buffer to the host. The function typically busy-waits until all
    /// the data in the buffer is sent to the host. This is acceptable only
    /// in the initial transient.
    static void onFlush(void);

    /// \brief Callback to obtain a timestamp for a QS record.
    ///
    /// This is a platform-dependent "callback" function invoked from the
    /// macro #QS_TIME_ to add the time stamp to the QS record.
    ///
    /// \note Some of the pre-defined QS records from QP do not output the
    /// time stamp. However, ALL user records do output the time stamp.
    /// \note QS::onGetTime() is called in a critical section and should not
    /// exit critical section.
    ///
    /// The following example shows using a system call to implement QS
    /// time stamping:
    /// \include qs_onGetTime.cpp
    static QSTimeCtr onGetTime(void);

    //////////////////////////////////////////////////////////////////////////
    // Global and Local QS filters
    static uint8_t glbFilter_[32];                ///< global on/off QS filter
    static void const *smObj_;         ///< state machine for QEP local filter
    static void const *aoObj_;       ///< active object for QF/QK local filter
    static void const *mpObj_;            ///<  event pool for QF local filter
    static void const *eqObj_;             ///<  raw queue for QF local filter
    static void const *teObj_;            ///<  time event for QF local filter
    static void const *apObj_;///<  generic object Application QF local filter

    //////////////////////////////////////////////////////////////////////////
    // Miscallaneous
                                    /// tick counter for the QS_QF_TICK record
    static QSTimeCtr tickCtr_;
};

/// \brief Enumerates data formats recognized by QS
///
/// QS uses this enumeration is used only internally for the formatted user
/// data elements.
enum QSType {
    QS_I8_T,                                  ///< signed 8-bit integer format
    QS_U8_T,                                ///< unsigned 8-bit integer format
    QS_I16_T,                                ///< signed 16-bit integer format
    QS_U16_T,                              ///< unsigned 16-bit integer format
    QS_I32_T,                                ///< signed 32-bit integer format
    QS_U32_T,                              ///< unsigned 32-bit integer format
    QS_F32_T,                                ///< 32-bit floating point format
    QS_F64_T,                                ///< 64-bit floating point format
    QS_STR_T,                         ///< zero-terminated ASCII string format
    QS_MEM_T,                         ///< up to 255-bytes memory block format
    QS_SIG_T,                                         ///< event signal format
    QS_OBJ_T,                                       ///< object pointer format
    QS_FUN_T,                                     ///< function pointer format
    QS_I64_T,                                ///< signed 64-bit integer format
    QS_U64_T,                              ///< unsigned 64-bit integer format
    QS_U32_HEX_T                    ///< unsigned 32-bit integer in hex format
};

/// \brief critical section nesting level
///
/// \note Not to be used by Clients directly, only in ports of QF
extern uint8_t QF_critNest_;

QP_END_


//////////////////////////////////////////////////////////////////////////////
// Macros for adding QS instrumentation to the client code

/// \brief Initialize the QS facility.
///
/// This macro provides an indirection layer to invoke the QS initialization
/// routine if #Q_SPY is defined, or do nothing if #Q_SPY is not defined.
/// \sa QS::onStartup(), example of setting up a QS filter in #QS_FILTER_IN
#define QS_INIT(arg_)           (QP_ QS::onStartup(arg_))

/// \brief Cleanup the QS facility.
///
/// This macro provides an indirection layer to invoke the QS cleanup
/// routine if #Q_SPY is defined, or do nothing if #Q_SPY is not defined.
/// \sa QS::onCleanup()
#define QS_EXIT()               (QP_ QS::onCleanup())

/// \brief Global Filter ON for a given record type \a rec.
///
/// This macro provides an indirection layer to call QS::filterOn() if #Q_SPY
/// is defined, or do nothing if #Q_SPY is not defined.
///
/// The following example shows how to use QS filters:
/// \include qs_filter.cpp
#define QS_FILTER_ON(rec_)      (QP_ QS::filterOn(static_cast<uint8_t>(rec_)))

/// \brief Global filter OFF for a given record type \a rec.
///
/// This macro provides an indirection layer to call QS::filterOff() if #Q_SPY
/// is defined, or do nothing if #Q_SPY is not defined.
///
/// \sa Example of using QS filters in #QS_FILTER_ON documentation
#define QS_FILTER_OFF(rec_)    (QP_ QS::filterOff(static_cast<uint8_t>(rec_)))

/// \brief Local Filter for a given state machine object \a obj_.
///
/// This macro sets up the state machine object local filter if #Q_SPY is
/// defined, or does nothing if #Q_SPY is not defined. The argument \a obj_
/// is the pointer to the state machine object that you want to monitor.
///
/// The state machine object filter allows you to filter QS records pertaining
/// only to a given state machine object. With this filter disabled, QS will
/// output records from all state machines in your application. The object
/// filter is disabled by setting the state machine pointer to NULL.
///
/// The state machine filter affects the following QS records:
/// ::QS_QEP_STATE_ENTRY, ::QS_QEP_STATE_EXIT, ::QS_QEP_STATE_INIT,
/// ::QS_QEP_INIT_TRAN, ::QS_QEP_INTERN_TRAN, ::QS_QEP_TRAN,
/// and ::QS_QEP_IGNORED.
///
/// \note Because active objects are state machines at the same time,
/// the state machine filter (#QS_FILTER_SM_OBJ) pertains to active
/// objects as well. However, the state machine filter is more general,
/// because it can be used only for state machines that are not active
/// objects, such as "Orthogonal Components".
///
/// \sa Example of using QS filters in #QS_FILTER_ON documentation
#define QS_FILTER_SM_OBJ(obj_)  (QP_ QS::smObj_ = (obj_))

/// \brief Local Filter for a given active object \a obj_.
///
/// This macro sets up the active object local filter if #Q_SPY is defined,
/// or does nothing if #Q_SPY is not defined. The argument \a obj_ is the
/// pointer to the active object that you want to monitor.
///
/// The active object filter allows you to filter QS records pertaining
/// only to a given active object. With this filter disabled, QS will
/// output records from all active objects in your application. The object
/// filter is disabled by setting the active object pointer \a obj_ to NULL.
///
/// The active object filter affects the following QS records:
/// ::QS_QF_ACTIVE_ADD, ::QS_QF_ACTIVE_REMOVE, ::QS_QF_ACTIVE_SUBSCRIBE,
/// ::QS_QF_ACTIVE_UNSUBSCRIBE, ::QS_QF_ACTIVE_POST_FIFO,
/// ::QS_QF_ACTIVE_POST_LIFO, ::QS_QF_ACTIVE_GET, and ::QS_QF_ACTIVE_GET_LAST.
///
/// \sa Example of using QS filters in #QS_FILTER_ON documentation
#define QS_FILTER_AO_OBJ(obj_)  (QP_ QS::aoObj_ = (obj_))

/// \brief Local Filter for a given memory pool object \a obj_.
///
/// This macro sets up the memory pool object local filter if #Q_SPY is
/// defined, or does nothing if #Q_SPY is not defined. The argument \a obj_
/// is the pointer to the memory buffer used during the initialization of the
/// event pool with QF::poolInit().
///
/// The memory pool filter allows you to filter QS records pertaining
/// only to a given memory pool. With this filter disabled, QS will
/// output records from all memory pools in your application. The object
/// filter is disabled by setting the memory pool pointer \a obj_ to NULL.
///
/// The memory pool filter affects the following QS records:
/// ::QS_QF_MPOOL_INIT, ::QS_QF_MPOOL_GET, and ::QS_QF_MPOOL_PUT.
///
/// \sa Example of using QS filters in #QS_FILTER_ON documentation
#define QS_FILTER_MP_OBJ(obj_)  (QP_ QS::mpObj_ = (obj_))

/// \brief Filter for a given event queue object \a obj_.
///
/// This macro sets up the event queue object filter if #Q_SPY is defined,
/// or does nothing if #Q_SPY is not defined. The argument \a obj_ is the
/// pointer to the "raw" thread-safe queue object you want to monitor.
///
/// The event queue filter allows you to filter QS records pertaining
/// only to a given event queue. With this filter disabled, QS will
/// output records from all event queues in your application. The object
/// filter is disabled by setting the event queue pointer \a obj_ to NULL.
///
/// The event queue filter affects the following QS records:
/// ::QS_QF_EQUEUE_INIT, ::QS_QF_EQUEUE_POST_FIFO, ::QS_QF_EQUEUE_POST_LIFO,
/// ::QS_QF_EQUEUE_GET, and ::QS_QF_EQUEUE_GET_LAST.
///
/// \sa Example of using QS filters in #QS_FILTER_IN documentation
#define QS_FILTER_EQ_OBJ(obj_)  (QP_ QS::eqObj_ = (obj_))

/// \brief Local Filter for a given time event object \a obj_.
///
/// This macro sets up the time event object local filter if #Q_SPY is
/// defined, or does nothing if #Q_SPY is not defined. The argument \a obj_
/// is the pointer to the time event object you want to monitor.
///
/// The time event filter allows you to filter QS records pertaining
/// only to a given time event. With this filter disabled, QS will
/// output records from all time events in your application. The object
/// filter is disabled by setting the time event pointer \a obj_ to NULL.
///
/// The time event filter affects the following QS records:
/// ::QS_QF_TIMEEVT_ARM, ::QS_QF_TIMEEVT_AUTO_DISARM,
/// ::QS_QF_TIMEEVT_DISARM_ATTEMPT, ::QS_QF_TIMEEVT_DISARM,
/// ::QS_QF_TIMEEVT_REARM, ::QS_QF_TIMEEVT_POST, and ::QS_QF_TIMEEVT_PUBLISH.
///
/// \sa Example of using QS filters in #QS_FILTER_ON documentation
#define QS_FILTER_TE_OBJ(obj_)  (QP_ QS::teObj_ = (obj_))

/// \brief Local Filter for a generic application object \a obj_.
///
/// This macro sets up the local application object filter if #Q_SPY is
/// defined, or does nothing if #Q_SPY is not defined. The argument \a obj_
/// is the pointer to the application object you want to monitor.
///
/// The application object filter allows you to filter QS records pertaining
/// only to a given application object. With this filter disabled, QS will
/// output records from all application-records enabled by the global filter.
/// The local filter is disabled by setting the time event pointer \a obj_
/// to NULL.
///
/// \sa Example of using QS filters in #QS_FILTER_ON documentation
#define QS_FILTER_AP_OBJ(obj_)  (QP_ QS::apObj_ = (obj_))


//////////////////////////////////////////////////////////////////////////////
// Macros to generate user QS records

#define QS_GLB_FILTER_(rec_) \
    ((QP_ QS::glbFilter_[static_cast<uint8_t>(rec_) >> 3] \
      & (static_cast<uint8_t>(1U << (static_cast<uint8_t>(rec_) \
                                     & static_cast<uint8_t>(7))))) \
             != static_cast<uint8_t>(0))

/// \brief Begin a QS user record without entering critical section.
#define QS_BEGIN_NOCRIT(rec_, obj_) \
    if (QS_GLB_FILTER_(rec_) \
        && ((QP_ QS::apObj_ == static_cast<void *>(0)) \
            || (QP_ QS::apObj_ == (obj_)))) \
    { \
        QP_ QS::begin(static_cast<uint8_t>(rec_)); \
        QS_TIME_();

/// \brief End a QS user record without exiting critical section.
#define QS_END_NOCRIT() \
    QS_END_NOCRIT_()

                                               // QS-specific critical section
#ifndef QF_CRIT_STAT_TYPE
    /// \brief This is an internal macro for defining the critical section
    /// status type.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// provides the definition of the critical section status variable.
    /// Otherwise this macro is empty.
    /// \sa #QF_CRIT_STAT_TYPE
    ///
    #define QS_CRIT_STAT_

    /// \brief This is an internal macro for entering a critical section.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// invokes #QF_CRIT_ENTRY passing the key variable as the parameter.
    /// Otherwise #QF_CRIT_ENTRY is invoked with a dummy parameter.
    /// \sa #QF_CRIT_ENTRY
    ///
    #define QS_CRIT_ENTRY_()    QF_CRIT_ENTRY(dummy)

    /// \brief This is an internal macro for exiting a cricial section.
    ///
    /// The purpose of this macro is to enable writing the same code for the
    /// case when critical sectgion status type is defined and when it is not.
    /// If the macro #QF_CRIT_STAT_TYPE is defined, this internal macro
    /// invokes #QF_CRIT_EXIT passing the key variable as the parameter.
    /// Otherwise #QF_CRIT_EXIT is invoked with a dummy parameter.
    /// \sa #QF_CRIT_EXIT
    ///
    #define QS_CRIT_EXIT_()     QF_CRIT_EXIT(dummy)

#else
    #define QS_CRIT_STAT_       QF_CRIT_STAT_TYPE critStat_;
    #define QS_CRIT_ENTRY_()    QF_CRIT_ENTRY(critStat_)
    #define QS_CRIT_EXIT_()     QF_CRIT_EXIT(critStat_)
#endif

/// \brief Begin a user QS record with entering critical section.
///
/// The following example shows how to build a user QS record using the
/// macros #QS_BEGIN, #QS_END, and the formatted output macros: #QS_U8 and
/// #QS_STR.
/// \include qs_user.cpp
/// \note Must always be used in pair with #QS_END
#define QS_BEGIN(rec_, obj_) \
    if (QS_GLB_FILTER_(rec_) \
        && ((QP_ QS::apObj_ == static_cast<void *>(0)) \
            || (QP_ QS::apObj_ == (obj_)))) \
    { \
        QS_CRIT_STAT_ \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(rec_)); \
        QS_TIME_();

/// \brief End a QS record with exiting critical section.
/// \sa example for #QS_BEGIN
/// \note Must always be used in pair with #QS_BEGIN
#define QS_END() \
    QS_END_()


//////////////////////////////////////////////////////////////////////////////
// Macros for use inside other macros or internally in the QP code

/// \brief Internal QS macro to begin a QS record with entering critical
/// section.
/// \note This macro is intended to use only inside QP components and NOT
/// at the application level. \sa #QS_BEGIN
#define QS_BEGIN_(rec_, objFilter_, obj_) \
    if (QS_GLB_FILTER_(rec_) \
        && (((objFilter_) == static_cast<void *>(0)) \
            || ((objFilter_) == (obj_)))) \
    { \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(rec_));

/// \brief  Internal QS macro to end a QS record with exiting critical
/// section.
/// \note This macro is intended to use only inside QP components and NOT
/// at the application level. \sa #QS_END
#define QS_END_() \
        QP_ QS::end(); \
        QS_CRIT_EXIT_(); \
    }

/// \brief Internal QS macro to begin a QS record without entering critical
/// section.
/// \note This macro is intended to use only inside QP components and NOT
/// at the application level. \sa #QS_BEGIN_NOCRIT
#define QS_BEGIN_NOCRIT_(rec_, objFilter_, obj_) \
    if (QS_GLB_FILTER_(rec_) \
        && (((objFilter_) == static_cast<void *>(0)) \
            || ((objFilter_) == (obj_)))) \
    { \
        QP_ QS::begin(static_cast<uint8_t>(rec_));

/// \brief Internal QS macro to end a QS record without exiting critical
/// section.
/// \note This macro is intended to use only inside QP components and NOT
/// at the application level. \sa #QS_END_NOCRIT
#define QS_END_NOCRIT_() \
        QP_ QS::end(); \
    }

#if (Q_SIGNAL_SIZE == 1)
    /// \brief Internal QS macro to output an unformatted event signal
    /// data element
    /// \note the size of the pointer depends on the macro #Q_SIGNAL_SIZE.
    #define QS_SIG_(sig_)    (QP_ QS::u8_(static_cast<uint8_t>(sig_)))
#elif (Q_SIGNAL_SIZE == 2)
    #define QS_SIG_(sig_)    (QP_ QS::u16_(static_cast<uint16_t>(sig_)))
#elif (Q_SIGNAL_SIZE == 4)
    #define QS_SIG_(sig_)    (QP_ QS::u32_(static_cast<uint32_t>(sig_)))
#endif

/// \brief Internal QS macro to output an unformatted uint8_t data element
#define QS_U8_(data_)        (QP_ QS::u8_(data_))

/// \brief Internal QS macro to output an unformatted uint16_t data element
#define QS_U16_(data_)       (QP_ QS::u16_(data_))

/// \brief Internal QS macro to output an unformatted uint32_t data element
#define QS_U32_(data_)       (QP_ QS::u32_(data_))


#if (QS_OBJ_PTR_SIZE == 1)
    #define QS_OBJ_(obj_)    (QP_ QS::u8_(reinterpret_cast<uint8_t>(obj_)))
#elif (QS_OBJ_PTR_SIZE == 2)
    #define QS_OBJ_(obj_)    (QP_ QS::u16_(reinterpret_cast<uint16_t>(obj_)))
#elif (QS_OBJ_PTR_SIZE == 4)
    #define QS_OBJ_(obj_)    (QP_ QS::u32_(reinterpret_cast<uint32_t>(obj_)))
#elif (QS_OBJ_PTR_SIZE == 8)
    #define QS_OBJ_(obj_)    (QP_ QS::u64_(reinterpret_cast<uint64_t>(obj_)))
#else

    /// \brief Internal QS macro to output an unformatted object pointer
    /// data element
    /// \note the size of the pointer depends on the macro #QS_OBJ_PTR_SIZE.
    /// If the size is not defined the size of pointer is assumed 4-bytes.
    #define QS_OBJ_(obj_)    (QP_ QS::u32_(reinterpret_cast<uint32_t>(obj_)))
#endif


#if (QS_FUN_PTR_SIZE == 1)
    #define QS_FUN_(fun_)    (QP_ QS::u8_(reinterpret_cast<uint8_t>(fun_)))
#elif (QS_FUN_PTR_SIZE == 2)
    #define QS_FUN_(fun_)    (QP_ QS::u16_(reinterpret_cast<uint16_t>(fun_)))
#elif (QS_FUN_PTR_SIZE == 4)
    #define QS_FUN_(fun_)    (QP_ QS::u32_(reinterpret_cast<uint32_t>(fun_)))
#elif (QS_FUN_PTR_SIZE == 8)
    #define QS_FUN_(fun_)    (QP_ QS::u64_(reinterpret_cast<uint64_t>(fun_)))
#else

    /// \brief Internal QS macro to output an unformatted function pointer
    /// data element
    /// \note the size of the pointer depends on the macro #QS_FUN_PTR_SIZE.
    /// If the size is not defined the size of pointer is assumed 4-bytes.
    #define QS_FUN_(fun_)    (QP_ QS::u32_(reinterpret_cast<uint32_t>(fun_)))
#endif

/// \brief Internal QS macro to output a zero-terminated ASCII string
/// data element
#define QS_STR_(msg_)        (QP_ QS::str_(msg_))

/// \brief Internal QS macro to output a zero-terminated ASCII string
/// allocated in ROM data element
#define QS_STR_ROM_(msg_)    (QP_ QS::str_ROM_(msg_))

//////////////////////////////////////////////////////////////////////////////
// Macros for use in the client code

/// \brief Output formatted int8_t to the QS record
#define QS_I8(width_, data_) \
    (QP_ QS::u8(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_I8_T)), (data_)))

/// \brief Output formatted uint8_t to the QS record
#define QS_U8(width_, data_) \
    (QP_ QS::u8(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_U8_T)), (data_)))

/// \brief Output formatted int16_t to the QS record
#define QS_I16(width_, data_) \
    (QP_ QS::u16(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_I16_T)), (data_)))

/// \brief Output formatted uint16_t to the QS record
#define QS_U16(width_, data_) \
    (QP_ QS::u16(static_cast<uint8_t>((((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_U16_T)), (data_)))

/// \brief Output formatted int32_t to the QS record
#define QS_I32(width_, data_) \
    (QP_ QS::u32(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_I32_T)), (data_)))

/// \brief Output formatted uint32_t to the QS record
#define QS_U32(width_, data_) \
    (QP_ QS::u32(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_U32_T)), (data_)))

/// \brief Output formatted 32-bit floating point number to the QS record
#define QS_F32(width_, data_) \
    QP_ QS::f32(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_F32_T)), (data_)))

/// \brief Output formatted 64-bit floating point number to the QS record
#define QS_F64(width_, data_) \
    (QP_ QS::f64(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_F64_T)), (data_)))

/// \brief Output formatted int64_t to the QS record
#define QS_I64(width_, data_) \
    (QP_ QS::u64(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_I64_T)), (data_)))

/// \brief Output formatted uint64_t to the QS record
#define QS_U64(width_, data_) \
    (QP_ QS::u64(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_U64_T)), (data_)))

/// \brief Output formatted uint32_t to the QS record
#define QS_U32_HEX(width_, data_) \
    (QP_ QS::u32(static_cast<uint8_t>((static_cast<uint8_t>((width_) << 4)) \
        | static_cast<uint8_t>(QP_ QS_U32_HEX_T)), (data_)))

/// \brief Output formatted zero-terminated ASCII string to the QS record
#define QS_STR(str_)            (QP_ QS::str(str_))

/// \brief Output formatted zero-terminated ASCII string from ROM
/// to the QS record
#define QS_STR_ROM(str_)        (QP_ QS::str_ROM(str_))

/// \brief Output formatted memory block of up to 255 bytes to the QS
/// record
#define QS_MEM(mem_, size_)     (QP_ QS::mem((mem_), (size_)))


#if (QS_OBJ_PTR_SIZE == 1)
    #define QS_OBJ(obj_)        (QP_ QS::u8(QS_OBJ_T, (uint8_t)(obj_)))
#elif (QS_OBJ_PTR_SIZE == 2)
    #define QS_OBJ(obj_)        (QP_ QS::u16(QS_OBJ_T, (uint16_t)(obj_)))
#elif (QS_OBJ_PTR_SIZE == 4)
    #define QS_OBJ(obj_)        (QP_ QS::u32(QS_OBJ_T, (uint32_t)(obj_)))
#elif (QS_OBJ_PTR_SIZE == 8)
    #define QS_OBJ(obj_)        (QP_ QS::u64(QS_OBJ_T, (uint64_t)(obj_)))
#else
    /// \brief Output formatted object pointer to the QS record
    #define QS_OBJ(obj_)        (QP_ QS::u32(QS_OBJ_T, (uint32_t)(obj_)))
#endif


#if (QS_FUN_PTR_SIZE == 1)
    #define QS_FUN(fun_)        (QP_ QS::u8(QS_FUN_T, (uint8_t)(fun_)))
#elif (QS_FUN_PTR_SIZE == 2)
    #define QS_FUN(fun_)        (QP_ QS::u16(QS_FUN_T, (uint16_t)(fun_)))
#elif (QS_FUN_PTR_SIZE == 4)
    #define QS_FUN(fun_)        (QP_ QS::u32(QS_FUN_T, (uint32_t)(fun_)))
#elif (QS_FUN_PTR_SIZE == 8)
    #define QS_FUN(fun_)        (QP_ QS::u64(QS_FUN_T, (uint64_t)(fun_)))
#else
    /// \brief Output formatted function pointer to the QS record
    #define QS_FUN(fun_)        (QP_ QS::u32(QS_FUN_T, (uint32_t)(fun_)))
#endif


/// \brief Reset the QS session.
///
/// This trace record should be generated at the beginning of the QS session.
/// It informs the QSPY host application that the new session has been started.
#define QS_RESET() do { \
    if (QS_GLB_FILTER_(QP_ QS_QP_RESET)) { \
        QS_CRIT_STAT_ \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(QP_ QS_QP_RESET)); \
        QP_ QS::end(); \
        QS_CRIT_EXIT_(); \
        QP_ QS::onFlush(); \
    } \
} while (false)

/// \brief Output signal dictionary record
///
/// A signal dictionary record associates the numerical value of the signal
/// and the binary address of the state machine that consumes that signal
/// with the human-readable name of the signal.
///
/// Providing a signal dictionary QS record can vastly improve readability of
/// the QS log, because instead of dealing with cryptic machine addresses the
/// QSpy host utility can display human-readable names.
///
/// A signal dictionary entry is associated with both the signal value \a sig_
/// and the state machine \a obj_, because signals are required to be unique
/// only within a given state machine and therefore the same numerical values
/// can represent different signals in different state machines.
///
/// For the "global" signals that have the same meaning in all state machines
/// (such as globally published signals), you can specify a signal dictionary
/// entry with the \a obj_ parameter set to NULL.
///
/// The following example shows the definition of signal dictionary entries
/// in the initial transition of the Table active object. Please note that
/// signals HUNGRY_SIG and DONE_SIG are associated with the Table state
/// machine only ("me" \a obj_ pointer). The EAT_SIG signal, on the other
/// hand, is global (0 \a obj_ pointer):
/// \include qs_sigDic.cpp
///
/// \note The QSpy log utility must capture the signal dictionary record
/// in order to use the human-readable information. You need to connect to
/// the target before the dictionary entries have been transmitted.
///
/// The following QSpy log example shows the signal dictionary records
/// generated from the Table initial transition and subsequent records that
/// show human-readable names of the signals:
/// \include qs_sigLog.txt
///
/// The following QSpy log example shows the same sequence of records, but
/// with dictionary records removed. The human-readable signal names are not
/// available.
/// \include qs_sigLog0.txt
#define QS_SIG_DICTIONARY(sig_, obj_) do { \
    if (QS_GLB_FILTER_(QP_ QS_SIG_DIC)) { \
        static char_t const Q_ROM Q_ROM_VAR sig_name_[] = #sig_; \
        QS_CRIT_STAT_ \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(QP_ QS_SIG_DIC)); \
        QS_SIG_(sig_); \
        QS_OBJ_(obj_); \
        QS_STR_ROM_(&sig_name_[0]); \
        QP_ QS::end(); \
        QS_CRIT_EXIT_(); \
        QP_ QS::onFlush(); \
    } \
} while (false)

/// \brief Output object dictionary record
///
/// An object dictionary record associates the binary address of an object
/// in the target's memory with the human-readable name of the object.
///
/// Providing an object dictionary QS record can vastly improve readability of
/// the QS log, because instead of dealing with cryptic machine addresses the
/// QSpy host utility can display human-readable object names.
///
/// The following example shows the definition of object dictionary entry
/// for the Table active object:
/// \include qs_objDic.cpp
#define QS_OBJ_DICTIONARY(obj_) do { \
    if (QS_GLB_FILTER_(QP_ QS_OBJ_DIC)) { \
        static char_t const Q_ROM Q_ROM_VAR obj_name_[] = #obj_; \
        QS_CRIT_STAT_ \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(QP_ QS_OBJ_DIC)); \
        QS_OBJ_(obj_); \
        QS_STR_ROM_(&obj_name_[0]); \
        QP_ QS::end(); \
        QS_CRIT_EXIT_(); \
        QP_ QS::onFlush(); \
    } \
} while (false)

/// \brief Output function dictionary record
///
/// A function dictionary record associates the binary address of a function
/// in the target's memory with the human-readable name of the function.
///
/// Providing a function dictionary QS record can vastly improve readability
/// of the QS log, because instead of dealing with cryptic machine addresses
/// the QSpy host utility can display human-readable function names.
///
/// The example from #QS_SIG_DICTIONARY shows the definition of a function
/// dictionary.
#define QS_FUN_DICTIONARY(fun_) do { \
    if (QS_GLB_FILTER_(QP_ QS_FUN_DIC)) { \
        static char_t const Q_ROM Q_ROM_VAR fun_name_[] = #fun_; \
        QS_CRIT_STAT_ \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(QP_ QS_FUN_DIC)); \
        QS_FUN_(fun_); \
        QS_STR_ROM_(&fun_name_[0]); \
        QP_ QS::end(); \
        QS_CRIT_EXIT_(); \
        QP_ QS::onFlush(); \
    } \
} while (false)

/// \brief Output user QS rectord dictionary record
///
/// A user QS record dictionary record associates the numerical value of a
/// user record with the human-readable identifier.
#define QS_USR_DICTIONARY(rec_) do { \
    if (QS_GLB_FILTER_(QP_ QS_USR_DIC)) { \
        static char_t const Q_ROM Q_ROM_VAR usr_name_[] = #rec_; \
        QS_CRIT_STAT_ \
        QS_CRIT_ENTRY_(); \
        QP_ QS::begin(static_cast<uint8_t>(QP_ QS_USR_DIC)); \
        QS_U8_(static_cast<uint8_t>(rec_)); \
        QS_STR_ROM_(&usr_name_[0]); \
        QP_ QS::end(); \
        QS_CRIT_EXIT_(); \
        QP_ QS::onFlush(); \
    } \
} while (false)

/// \brief Output the assertion violation
#define QS_ASSERTION(module_, loc_) do { \
    QS_BEGIN_NOCRIT_(QP_ QS_ASSERT, \
        static_cast<void *>(0), static_cast<void *>(0)) \
        QS_TIME_(); \
        QS_U16_(static_cast<uint16_t>(loc_)); \
        QS_STR_ROM_(module_); \
    QS_END_NOCRIT_() \
    QP_ QS::onFlush(); \
} while (false)

/// \brief Flush the QS trace data to the host
///
/// This macro invokes the QS::flush() platform-dependent callback function
/// to flush the QS trace buffer to the host. The function typically
/// busy-waits until all the data in the buffer is sent to the host.
/// This is acceptable only in the initial transient.
#define QS_FLUSH()   (QP_ QS::onFlush())

/// \brief Output the critical section entry record
#define QF_QS_CRIT_ENTRY() \
    QS_BEGIN_NOCRIT_(QP_ QS_QF_CRIT_ENTRY, \
        static_cast<void *>(0), static_cast<void *>(0)) \
        QS_TIME_(); \
        QS_U8_((uint8_t)(++QF_critNest_)); \
    QS_END_NOCRIT_()

/// \brief Output the critical section exit record
#define QF_QS_CRIT_EXIT() \
    QS_BEGIN_NOCRIT_(QP_ QS_QF_CRIT_EXIT, \
        static_cast<void *>(0), static_cast<void *>(0)) \
        QS_TIME_(); \
        QS_U8_((uint8_t)(QF_critNest_--)); \
    QS_END_NOCRIT_()

/// \brief Output the interrupt entry record
#define QF_QS_ISR_ENTRY(isrnest_, prio_) \
    QS_BEGIN_NOCRIT_(QP_ QS_QF_ISR_ENTRY, \
        static_cast<void *>(0), static_cast<void *>(0)) \
        QS_TIME_(); \
        QS_U8_(isrnest_); \
        QS_U8_(prio_); \
    QS_END_NOCRIT_()

/// \brief Output the interrupt exit record
#define QF_QS_ISR_EXIT(isrnest_, prio_) \
    QS_BEGIN_NOCRIT_(QP_ QS_QF_ISR_EXIT,  \
        static_cast<void *>(0), static_cast<void *>(0)) \
        QS_TIME_(); \
        QS_U8_(isrnest_); \
        QS_U8_(prio_); \
    QS_END_NOCRIT_()

/// \brief Execute an action that is only necessary for QS output
#define QF_QS_ACTION(act_)      (act_)

#else                                                                 // Q_SPY

// qs_dummy.h ================================================================
/// \brief Dummy definitions of the QS macros that avoid code generation from
/// the QS instrumentation.

#define QS_INIT(arg_)                   (true)
#define QS_EXIT()                       ((void)0)
#define QS_DUMP()                       ((void)0)
#define QS_RESET()                      ((void)0)
#define QS_FILTER_ON(rec_)              ((void)0)
#define QS_FILTER_OFF(rec_)             ((void)0)
#define QS_FILTER_SM_OBJ(obj_)          ((void)0)
#define QS_FILTER_AO_OBJ(obj_)          ((void)0)
#define QS_FILTER_MP_OBJ(obj_)          ((void)0)
#define QS_FILTER_EQ_OBJ(obj_)          ((void)0)
#define QS_FILTER_TE_OBJ(obj_)          ((void)0)
#define QS_FILTER_AP_OBJ(obj_)          ((void)0)

#define QS_GET_BYTE(pByte_)             (static_cast<uint16_t>(0xFFFFU))
#define QS_GET_BLOCK(pSize_)            (static_cast<uint8_t *>(0))

#define QS_BEGIN(rec_, obj_)            if (false) {
#define QS_END()                        }
#define QS_BEGIN_NOCRIT(rec_, obj_)     if (false) {
#define QS_END_NOCRIT()                 }

#define QS_I8(width_, data_)            ((void)0)
#define QS_U8(width_, data_)            ((void)0)
#define QS_I16(width_, data_)           ((void)0)
#define QS_U16(width_, data_)           ((void)0)
#define QS_I32(width_, data_)           ((void)0)
#define QS_U32(width_, data_)           ((void)0)
#define QS_F32(width_, data_)           ((void)0)
#define QS_F64(width_, data_)           ((void)0)
#define QS_U64(width_, data_)           ((void)0)
#define QS_STR(str_)                    ((void)0)
#define QS_U32_HEX(width_, data_)       ((void)0)
#define QS_STR_ROM(str_)                ((void)0)
#define QS_MEM(mem_, size_)             ((void)0)
#define QS_SIG(sig_, obj_)              ((void)0)
#define QS_OBJ(obj_)                    ((void)0)
#define QS_FUN(fun_)                    ((void)0)

#define QS_SIG_DICTIONARY(sig_, obj_)   ((void)0)
#define QS_OBJ_DICTIONARY(obj_)         ((void)0)
#define QS_FUN_DICTIONARY(fun_)         ((void)0)
#define QS_USR_DICTIONARY(rec_)         ((void)0)
#define QS_ASSERTION(module_, loc_)     ((void)0)
#define QS_FLUSH()                      ((void)0)

// internal QS macros used only in the QP components .........................
#define QS_CRIT_STAT_
#define QS_BEGIN_(rec_, refObj_, obj_)  if (false) {
#define QS_END_()                       }
#define QS_BEGIN_NOCRIT_(rec_, refObj_, obj_) if (false) {
#define QS_END_NOCRIT_()                }
#define QS_U8_(data_)                   ((void)0)
#define QS_U16_(data_)                  ((void)0)
#define QS_U32_(data_)                  ((void)0)
#define QS_U64_(data_)                  ((void)0)
#define QS_TIME_()                      ((void)0)
#define QS_SIG_(sig_)                   ((void)0)
#define QS_EVS_(size_)                  ((void)0)
#define QS_OBJ_(obj_)                   ((void)0)
#define QS_FUN_(fun_)                   ((void)0)
#define QS_EQC_(ctr_)                   ((void)0)
#define QS_MPC_(ctr_)                   ((void)0)
#define QS_MPS_(size_)                  ((void)0)
#define QS_TEC_(ctr_)                   ((void)0)

#define QF_QS_CRIT_ENTRY()              ((void)0)
#define QF_QS_CRIT_EXIT()               ((void)0)
#define QF_QS_ISR_ENTRY(isrnest_, prio_) ((void)0)
#define QF_QS_ISR_EXIT(isrnest_, prio_) ((void)0)
#define QF_QS_ACTION(act_)              ((void)0)

#endif                                                                // Q_SPY

// qassert.h =================================================================
/* \brief Customizable QP assertions.
*
* Defines customizable and memory-efficient assertions applicable to
* embedded systems. This header file can be used in C, C++, and mixed C/C++
* programs.
*
* \note The preprocessor switch Q_NASSERT disables checking assertions.
* In particular macros #Q_ASSERT, #Q_REQUIRE, #Q_ENSURE, #Q_INVARIANT,
* #Q_ERROR as well as  #Q_ASSERT_ID, #Q_REQUIRE_ID, #Q_ENSURE_ID,
* #Q_INVARIANT_ID, and #Q_ERROR_ID do NOT evaluate the test condition
* passed as the argument to these macros. One notable exception is the
* macro #Q_ALLEGE, that still evaluates the test condition, but does
* not report assertion failures when the switch Q_NASSERT is defined.
*/
#ifdef Q_NASSERT          /* Q_NASSERT defined--assertion checking disabled */

    #define Q_DEFINE_THIS_FILE
    #define Q_DEFINE_THIS_MODULE(name_)
    #define Q_ASSERT(test_)             ((void)0)
    #define Q_ASSERT_ID(id_, test_)     ((void)0)
    #define Q_ALLEGE(test_)             ((void)(test_))
    #define Q_ALLEGE_ID(id_, test_)     ((void)(test_))
    #define Q_ERROR()                   ((void)0)
    #define Q_ERROR_ID(id_)             ((void)0)

#else                  /* Q_NASSERT not defined--assertion checking enabled */

    #ifdef __cplusplus
        extern "C" {
    #endif

    /** \brief Type for line numbers.
    *
    * This typedef specifies strong type for line numbers. The use of this
    * type, rather than plain 'int', is in compliance with the MISRA-C 2004
    * Rule 6.3(adv).
    */
    typedef int int_t;

    /** callback invoked in case the condition passed to #Q_ASSERT,
    * #Q_REQUIRE, #Q_ENSURE, #Q_ERROR, #Q_ALLEGE as well as #Q_ASSERT_ID,
    * #Q_REQUIRE_ID, #Q_ENSURE_ID, #Q_ERROR_ID, and #Q_ALLEGE_ID evaluates
    * to FALSE.
    *
    * \param file name where the assertion failed
    * \param line number at which the assertion failed
    */
    void Q_onAssert(char_t const Q_ROM * const Q_ROM_VAR file,
                    int_t const line);

    #ifdef __cplusplus
        }
    #endif

    /** Place this macro at the top of each C/C++ module to define the file
    * name string using __FILE__ (NOTE: __FILE__ might contain lengthy path
    * name). This file name will be used in reporting assertions in this file.
    */
    #define Q_DEFINE_THIS_FILE \
        static char_t const Q_ROM Q_ROM_VAR l_this_file[] = __FILE__;

    /** Place this macro at the top of each C/C++ module to define the module
    * name as the argument \a name_. This file name will be used in reporting
    * assertions in this file.
    */
    #define Q_DEFINE_THIS_MODULE(name_) \
        static char_t const Q_ROM Q_ROM_VAR l_this_file[] = name_;

    /** General purpose assertion that makes sure the \a test_ argument is
    * TRUE. Calls the Q_onAssert() callback if the \a test_ evaluates
    * to FALSE.
    * \note the \a test_ is NOT evaluated if assertions are disabled with
    * the Q_NASSERT switch.
    * \sa #Q_ASSERT_ID
    */
    #define Q_ASSERT(test_) \
        ((test_) ? (void)0 : Q_onAssert(&l_this_file[0], (int_t)__LINE__))

    /** General purpose assertion that makes sure the \a test_ argument is
    * TRUE. Calls the Q_onAssert() callback if the \a test_ evaluates
    * to FALSE. The argument \a id_ is the ID number (unique within
    * the file) of the assertion. This assertion style is better suited
    * for unit testig, because it avoids the volatility of line numbers
    * for indentifying assertions.
    * \note the \a test_ is NOT evaluated if assertions are disabled with
    * the Q_NASSERT switch.
    * \sa #Q_ASSERT
    */
    #define Q_ASSERT_ID(id_, test_) \
        ((test_) ? (void)0 : Q_onAssert(&l_this_file[0], (int_t)(id_))

    /** General purpose assertion that ALWAYS evaluates the \a test_
    * argument and calls the Q_onAssert() callback if the \a test_
    * evaluates to FALSE.
    * \note the \a test_ argument IS always evaluated even when assertions
    * are disabled with the Q_NASSERT macro. When the Q_NASSERT macro is
    * defined, the Q_onAssert() callback is NOT called, even if the
    * \a test_ evaluates to FALSE.
    * \sa #Q_ALLEGE_ID
    */
    #define Q_ALLEGE(test_)    Q_ASSERT(test_)

    /** General purpose assertion that ALWAYS evaluates the \a test_
    * argument and calls the Q_onAssert() callback if the \a test_
    * evaluates to FALSE. This assertion style is better suited
    * for unit testig, because it avoids the volatility of line numbers
    * for indentifying assertions.
    * \note the \a test_ argument IS always evaluated even when assertions
    * are disabled with the Q_NASSERT macro. When the Q_NASSERT macro is
    * defined, the Q_onAssert() callback is NOT called, even if the
    * \a test_ evaluates to FALSE.
    * \sa #Q_ALLEGE
    */
    #define Q_ALLEGE_ID(id_, test_) Q_ASSERT_ID(id_, test_)

    /** Assertion that always calls the Q_onAssert() callback if
    * ever executed.
    * \note can be disabled with the Q_NASSERT switch.
    * \sa #Q_ERROR_ID
    */
    #define Q_ERROR() \
        Q_onAssert(&l_this_file[0], (int_t)__LINE__)

    /** Assertion that always calls the Q_onAssert() callback if
    * ever executed. This assertion style is better suited for unit
    * testig, because it avoids the volatility of line numbers for
    * indentifying assertions.
    * \note can be disabled with the Q_NASSERT switch.
    * \sa #Q_ERROR
    */
    #define Q_ERROR_ID(id_) \
        Q_onAssert(l_this_file, (int_t)(id_))

#endif                                                         /* Q_NASSERT */

/** Assertion that checks for a precondition. This macro is equivalent to
* #Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
* \sa #Q_REQUIRE_ID
*/
#define Q_REQUIRE(test_)         Q_ASSERT(test_)

/** Assertion that checks for a precondition. This macro is equivalent to
* #Q_ASSERT_ID, except the name provides a better documentation of the
* intention of this assertion.
* \sa #Q_REQUIRE
*/
#define Q_REQUIRE_ID(id_, test_) Q_ASSERT_ID(id_, test_)

/** Assertion that checks for a postcondition. This macro is equivalent to
* #Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
* \sa #Q_ENSURE_ID
*/
#define Q_ENSURE(test_)          Q_ASSERT(test_)

/** Assertion that checks for a postcondition. This macro is equivalent to
* #Q_ASSERT_ID, except the name provides a better documentation of the
* intention of this assertion.
* \sa #Q_ENSURE
*/
#define Q_ENSURE_ID(id_, test_)  Q_ASSERT_ID(id_, test_)

/** Assertion that checks for an invariant. This macro is equivalent to
* #Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
* \sa #Q_INVARIANT_ID
*/
#define Q_INVARIANT(test_)       Q_ASSERT(test_)

/** Assertion that checks for an invariant. This macro is equivalent to
* #Q_ASSERT_ID, except the name provides a better documentation of the
* intention of this assertion.
* \sa #Q_INVARIANT
*/
#define Q_INVARIANT_ID(id_, test_) Q_ASSERT_ID(id_, test_)

/** Compile-time assertion exploits the fact that in C/C++ a dimension of
* an array cannot be negative. The following declaration causes a compilation
* error if the compile-time expression (\a test_) is not TRUE. The assertion
* has no runtime side effects.
*/
#define Q_ASSERT_COMPILE(test_) \
    extern int_t Q_assert_compile[(test_) ? 1 : -1]

#endif                                                            // qp_port_h
