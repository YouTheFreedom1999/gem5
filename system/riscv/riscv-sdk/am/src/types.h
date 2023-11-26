#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef          int        int32_t;
typedef unsigned long long  uint64_t;

/** \brief Defines 'read only' permissions */
#ifdef __cplusplus
  #define   __I     volatile
#else
  #define   __I     volatile const
#endif
/** \brief Defines 'write only' permissions */
#define     __O     volatile
/** \brief Defines 'read / write' permissions */
#define     __IO    volatile

/* following defines should be used for structure members */
/** \brief Defines 'read only' structure member permissions */
#define     __IM     volatile const
/** \brief Defines 'write only' structure member permissions */
#define     __OM     volatile
/** \brief Defines 'read/write' structure member permissions */
#define     __IOM    volatile


/** \brief provide the compiler with branch prediction information, the branch is rarely true */
#ifndef   __RARELY
  #define __RARELY(exp)                          __builtin_expect((exp), 0)
#endif


#endif /* __TYPES_H__ */
