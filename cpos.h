/*
**  Author:
**     Lulus Wijayakto  <l.wijayakto@yahoo.com>
**
**  License: LGPL
**
**  27022013
*/


#ifndef C_POS__H
#define C_POS__H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>


#define ISO_BIT_LEN 129

/* Private struct */
struct bit_cfg__ {
  uint8_t flag; // (0 fixed length)
  uint16_t len;
};


struct build_msg {
  uint8_t bit;
  size_t data_len;
  void *data;
};

struct parser_msg {
  size_t id;
  void *data;
};


/* Private */
inline uint16_t utils_get_element_cfg(const uint8_t bit, uint8_t *flag);
uint8_t* utils_hex2bin(const uint8_t *, uint8_t *, size_t);
uint8_t* utils_bin2hex(const uint8_t *, uint8_t *, size_t);


/* Public initialitation */
uint8_t cpos_init(const char *);
void cpos_close();

/* Public Parser */
uint8_t parser_get_bitmap(const uint8_t *msg, const uint8_t flag,
                          uint8_t *bitmap, const uint16_t blen);
uint8_t parser_check_get_data(const uint8_t *msg, const uint8_t *bitmap,
                              const uint8_t f, uint8_t *buf,
                              const uint16_t blen, const uint8_t bit);

struct parser_msg* parser_get_data_init();
void parser_get_data_free(struct parser_msg *);
uint8_t parser_get_data(struct parser_msg *, uint8_t *);

/* Public Build */
uint8_t build_element(struct build_msg *, const uint8_t,
                      const void *, size_t);
struct build_msg* build_init(void);
void build_free(struct build_msg *);
uint8_t* build_msg(struct build_msg *, const char *,
                   void *, size_t, uint8_t);

/* Public socket */
int socket_bind(uint16_t, uint8_t);
int socket_connect(const char *, uint16_t, uint8_t);
int socket_non_blocking(int);
int socket_send(int, void *, int);
int socket_recv(int, void *, int);

#ifdef __cplusplus
}
#endif

#endif

