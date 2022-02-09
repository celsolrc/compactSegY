#pragma once

/**
 * @file swap.h
 * @author Celso Luiz Ramos Cruz
 *
 * @brief       Cabeçalho com o protótipo da classe para macros de chamada para funções internas de swap. Estas funções já existem nos compiladores GNUC.
 *              Se estiver sendo um outro padrão de compilador, as chamadas são redefinidas na macro.
 *
 * @version 0.1
 * @date 2020-04-27
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef __SWAP_H__
#define __SWAP_H__

#if defined __GNUC__
#define bswap64(x) __builtin_bswap64((x))
#define bswap32(x) __builtin_bswap32((x))
#define bswap16(x) __builtin_bswap16((x))
#else
#define bswap64(v) ( ((v & 0x00000000000000FFull) << 56) \
                       | ((v & 0x000000000000FF00ull) << 40) \
                       | ((v & 0x0000000000FF0000ull) << 24) \
                       | ((v & 0x00000000FF000000ull) <<  8) \
                       | ((v & 0x000000FF00000000ull) >>  8) \
                       | ((v & 0x0000FF0000000000ull) >> 24) \
                       | ((v & 0x00FF000000000000ull) >> 40) \
                       | ((v & 0xFF00000000000000ull) >> 56) \
                       )

#define bswap32(v) ( (((v) & 0x000000FF) << 24) \
                       | (((v) & 0x0000FF00) <<  8) \
                       | (((v) & 0x00FF0000) >>  8) \
                       | (((v) & 0xFF000000) >> 24) \
                       )

#define bswap16(v) ( (((v) & 0x00FF) << 8) \
                       | (((v) & 0xFF00) >> 8) \
                       )
#endif // __GNUC__

#endif