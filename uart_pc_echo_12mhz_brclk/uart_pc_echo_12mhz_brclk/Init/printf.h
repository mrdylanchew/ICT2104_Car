#ifndef PRINTF_H_
#define PRINTF_H_

void initprintf(void);
void Uartputs(uint32_t moduleInstance, char *s);
void Uartputc(uint32_t moduleInstance, unsigned b);
void Uartprintf(uint32_t moduleInstance, char *, ...);

#endif /* PRINTF_H_ */
