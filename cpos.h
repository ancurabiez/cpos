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

struct isofield_cfg {
  uint8_t flag; // (0 fixed length)
  uint16_t len;
};

struct isomsg {
  uint8_t bit;
  size_t data_len;
  void *data;
};


/* Private */
inline uint16_t utils_get_field_cfg(struct isofield_cfg *,
		const uint8_t bit, uint8_t *flag);
uint8_t* utils_hex2bin(const uint8_t *, uint8_t *, size_t);
uint8_t* utils_bin2hex(const uint8_t *, uint8_t *, size_t);


/* Public initialitation */
struct isofield_cfg* cpos_init(const char *);
void cpos_close(struct isofield_cfg *);

/* Public Parsing */
struct isomsg* cpos_parse_init();
void cpos_parse_free(struct isomsg *);
uint8_t cpos_parse(struct isofield_cfg *, struct isomsg *, uint8_t *);

/* Public Build */
struct isomsg* cpos_build_init(void);
void cpos_build_free(struct isomsg *);
uint8_t cpos_build_set_field(struct isomsg *, const uint8_t,
                      const void *, size_t);
uint8_t* cpos_build_msg(struct isofield_cfg *, struct isomsg *, const char *,
                   void *, size_t, uint8_t);

/* Public socket */
int cpos_socket_bind(uint16_t, uint8_t);
int cpos_socket_connect(const char *, uint16_t, uint8_t);
int cpos_socket_non_blocking(int);
int cpos_socket_send(int, void *, int);
int cpos_socket_recv(int, void *, int);

#ifdef __cplusplus
}
#endif

#endif

