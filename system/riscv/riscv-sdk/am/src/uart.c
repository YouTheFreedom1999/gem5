#include "types.h"
#include <stddef.h>
#include "platform.h"

#ifdef QEMU_SIM

/*
 * The UART control registers are memory-mapped at address UART0.
 * This macro returns the address of one of the registers.
 */
#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))

/*
 * Reference
 * [1]: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html
 */

/*
 * UART control registers map. see [1] "PROGRAMMING TABLE"
 * note some are reused by multiple functions
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 */
#define RHR 0	// Receive Holding Register (read mode)
#define THR 0	// Transmit Holding Register (write mode)
#define DLL 0	// LSB of Divisor Latch (write mode)
#define IER 1	// Interrupt Enable Register (write mode)
#define DLM 1	// MSB of Divisor Latch (write mode)
#define FCR 2	// FIFO Control Register (write mode)
#define ISR 2	// Interrupt Status Register (read mode)
#define LCR 3	// Line Control Register
#define MCR 4	// Modem Control Register
#define LSR 5	// Line Status Register
#define MSR 6	// Modem Status Register
#define SPR 7	// ScratchPad Register

/*
 * POWER UP DEFAULTS
 * IER = 0: TX/RX holding register interrupts are both disabled
 * ISR = 1: no interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 */

/*
 * LINE STATUS REGISTER (LSR)
 * LSR BIT 0:
 * 0 = no data in receive holding register or FIFO.
 * 1 = data has been receive and saved in the receive holding register or FIFO.
 * ......
 * LSR BIT 5:
 * 0 = transmit holding register is full. 16550 will not accept any data for transmission.
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * ......
 */
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void uart_init()
{
	/* disable interrupts. */
	uart_write_reg(IER, 0x00);

	/*
	 * Setting baud rate. Just a demo here if we care about the divisor,
	 * but for our purpose [QEMU-virt], this doesn't really do anything.
	 *
	 * Notice that the divisor register DLL (divisor latch least) and DLM (divisor
	 * latch most) have the same base address as the receiver/transmitter and the
	 * interrupt enable register. To change what the base address points to, we
	 * open the "divisor latch" by writing 1 into the Divisor Latch Access Bit
	 * (DLAB), which is bit index 7 of the Line Control Register (LCR).
	 *
	 * Regarding the baud rate value, see [1] "BAUD RATE GENERATOR PROGRAMMING TABLE".
	 * We use 38.4K when 1.8432 MHZ crystal, so the corresponding value is 3.
	 * And due to the divisor register is two bytes (16 bits), so we need to
	 * split the value of 3(0x0003) into two bytes, DLL stores the low byte,
	 * DLM stores the high byte.
	 */
	uint8_t lcr = uart_read_reg(LCR);
	uart_write_reg(LCR, lcr | (1 << 7));
	uart_write_reg(DLL, 0x03);
	uart_write_reg(DLM, 0x00);

	/*
	 * Continue setting the asynchronous data communication format.
	 * - number of the word length: 8 bits
	 * - number of stop bits：1 bit when word length is 8 bits
	 * - no parity
	 * - no break control
	 * - disabled baud latch
	 */
	lcr = 0;
	uart_write_reg(LCR, lcr | (3 << 0));

	/*
	 * enable receive interrupts.
	//  */
	// uint8_t ier = uart_read_reg(IER);
	// uart_write_reg(IER, ier | (1 << 0));
}

int uart_putc(char ch)
{
	while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
	return uart_write_reg(THR, ch);
}


int uart_getc(void)
{
	while(!(uart_read_reg(LSR) & LSR_RX_READY));
    return uart_read_reg(RHR);
}

int uart_getc_nonblock(void){
    if(uart_read_reg(LSR) & LSR_RX_READY)
        return uart_read_reg(RHR);
    else
        return -1;
}

/*
 * handle a uart interrupt, raised because input has arrived, called from trap.c.
 */
void uart_isr(void)
{
	while (1) {
		int c = uart_getc();
		if (c == -1) {
			break;
		} else {
			uart_putc((char)c);
			uart_putc('\n');
		}
	}
}

#else

void uart_init()
{
    // if (__RARELY(uart == NULL)) {
    //     return -1;
    // }

    // unsigned int uart_div = 100000000;
    unsigned int uart_div = 115200;
    unsigned int DL = (int)((100000000 / uart_div ) - 1);
    UART0->LCR = 0x80;
    UART0->DLM = (DL >> 8) & 0xFF;
    UART0->DLL =  DL       & 0xFF;
    UART0->FCR = 0xC6;
    UART0->LCR = 0x03;

    // return 0;
}

// int32_t uart_config_stopbit(UART_TypeDef *uart, UART_STOP_BIT stopbit)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->LCR &= 0xFFFFFFFB;
//     uart->LCR |= (stopbit << 2);

//     return 0;
// }

// int32_t uart_enable_paritybit(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->LCR |= 0x8;

//     return 0;
// }

// int32_t uart_disable_paritybit(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->LCR &= 0xFFFFFFF7;

//     return 0;
// }

// int32_t uart_set_parity(UART_TypeDef *uart, UART_PARITY_BIT paritybit)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->LCR &= 0xFFFFFFCF;
//     uart->LCR |= (paritybit << 4);

//     return 0;
// }

int32_t uart_write(UART_TypeDef *uart, uint8_t val)
{
    if (__RARELY(uart == NULL)) {
        return -1;
    }

    while ((uart->LSR & 0x20) == 0);
    uart->THR = val;

    return 0;
}

uint8_t uart_read(UART_TypeDef *uart)
{
    uint32_t reg;
    if (__RARELY(uart == NULL)) {
        return -1;
    }

    while ((uart->LSR & 0x1) == 0);
    reg = uart->RBR;

    return (uint8_t)(reg & 0xFF);
}


// int32_t uart_enable_tx_empt_int(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->IER |= 0x2;
//     return 0;
// }

// int32_t uart_disable_tx_empt_int(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->IER &= 0xFFFFFFFD;
//     return 0;
// }

// int32_t uart_set_rx_th(UART_TypeDef *uart, uint8_t th)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     if(th > 3) {
//        th = 3;
//     }

//     uart->FCR &= 0xFFFFFF3F;
//     uart->FCR |= (th << 6);
//     return 0;
// }

// int32_t uart_enable_rx_th_int(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->IER |= 0x1;
//     return 0;
// }

// int32_t uart_disable_rx_th_int(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->IER &= 0xFFFFFFFE;
//     return 0;
// }

// int32_t uart_enable_rx_err_int(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->IER |= 0x4;
//     return 0;
// }

// int32_t uart_disable_rx_err_int(UART_TypeDef *uart)
// {
//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     uart->IER &= 0xFFFFFFFB;
//     return 0;
// }

// int32_t uart_get_int_status(UART_TypeDef *uart)
// {

//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     return uart->IIR;
// }

// int32_t uart_get_status(UART_TypeDef *uart)
// {

//     if (__RARELY(uart == NULL)) {
//         return -1;
//     }

//     return uart->LSR;
// }



int uart_putc(char ch)
{
	// while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
	// return uart_write_reg(THR, ch);
	return uart_write(UART0 , ch);
}

int uart_getc(void) {
    // if (uart_read_reg(LSR) & LSR_RX_READY){
    //     return uart_read_reg(RHR);
    // } else {
    //     return -1;
    // }
    // while(!(UART0->LSR & 0x01));
	return uart_read(UART0);
}

#endif

void uart_puts(char *s)
{
	while (*s) {
		uart_putc(*s++);
	}
}

void uart_getstr(char * buf){
	char ch;
	do{
		ch = uart_getc();
		*buf++ = ch;
	}while(ch != '\n' );
}
