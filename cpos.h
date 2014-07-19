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

typedef uint16_t (* header_callback)(uint8_t *head);

enum format {
  LPS,
#define LPS LPS
  RPS,
#define RPS RPS
  LPZ,
#define LPZ LPZ
  RPZ,
#define RPZ RPZ
  NONE
#define NONE NONE
};

enum error {
  CPOS_OK,
#define CPOS_OK CPOS_OK
  CPOS_ERROR,
#define CPOS_ERROR CPOS_ERROR
  CPOS_NOMEM,
#define CPOS_NOMEM CPOS_NOMEM
  CPOS_LENERR,
#define CPOS_LENERR CPOS_LENERR  
};

struct isofield_cfg {
  uint8_t flag; // (0 fixed length)
  uint16_t len;
  uint16_t maxlen;
  uint8_t format;
};

struct isomsg {
  uint16_t len;
  void *data;
};


/* Private */
inline uint16_t utils_get_field_cfg(struct isofield_cfg *,
		const uint8_t bit, uint8_t *flag, uint16_t *, uint8_t *);
uint8_t* utils_hex2bin(const uint8_t *, uint8_t *, size_t);
uint8_t* utils_bin2hex(const uint8_t *, uint8_t *, size_t);
void* utils_fill(const void* , uint16_t , uint16_t, uint8_t);
char* util_get_error(uint8_t);


/* Public utils (malloc(ed) must be freed)) */
uint8_t* cpos_space_trim(const uint8_t *);
uint8_t* cpos_zero_trim(const uint8_t *);
uint8_t* cpos_ascii2hex(const uint8_t *);
uint8_t* cpos_hex2ascii(const uint8_t *);
uint8_t* cpos_ascii2ebcdic(const void *, size_t);
uint8_t* cpos_ebcdic2ascii(const void *, size_t);

/* Public initialitation */
struct isofield_cfg* cpos_init(const char *);
void cpos_close(struct isofield_cfg *);

/* Public constructor/destructor */
struct isomsg* cpos_msg_new();
void cpos_msg_free(struct isomsg *);

/* Public Parse */
uint8_t cpos_parse(struct isofield_cfg *, struct isomsg *, uint8_t *,
                   uint16_t, char **);

/* Public Build */
uint8_t cpos_build_set_field(struct isomsg *, const uint8_t,
                      const void *, size_t);
uint8_t* cpos_build_msg(struct isofield_cfg *, struct isomsg *, const char *,
                   void *, size_t);

/* Public socket */
int cpos_socket_bind(uint16_t, uint32_t, uint8_t);
int cpos_socket_connect(const char *, uint16_t, uint8_t);
int cpos_socket_non_blocking(int);
int cpos_socket_send(int, void *, int);
int cpos_socket_recv(int, void *, int, uint16_t *, uint8_t,
            uint16_t (* header_callback)(uint8_t *));

#ifdef __cplusplus
}
#endif

#endif

