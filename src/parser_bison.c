/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         nft_parse
#define yylex           nft_lex
#define yyerror         nft_error
#define yydebug         nft_debug
#define yynerrs         nft_nerrs

/* First part of user prologue.  */
#line 11 "src/parser_bison.y"

#include <nft.h>

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <syslog.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <linux/netfilter/nf_nat.h>
#include <linux/netfilter/nf_log.h>
#include <linux/netfilter/nfnetlink_osf.h>
#include <linux/netfilter/nf_synproxy.h>
#include <linux/xfrm.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <libnftnl/common.h>
#include <libnftnl/set.h>
#include <libnftnl/udata.h>

#include <rule.h>
#include <statement.h>
#include <expression.h>
#include <headers.h>
#include <utils.h>
#include <parser.h>
#include <erec.h>
#include <sctp_chunk.h>

#include "parser_bison.h"

void parser_init(struct nft_ctx *nft, struct parser_state *state,
		 struct list_head *msgs, struct list_head *cmds,
		 struct scope *top_scope)
{
	memset(state, 0, sizeof(*state));
	state->msgs = msgs;
	state->cmds = cmds;
	state->scopes[0] = scope_init(top_scope, NULL);
	init_list_head(&state->indesc_list);
}

static void yyerror(struct location *loc, struct nft_ctx *nft, void *scanner,
		    struct parser_state *state, const char *s)
{
	erec_queue(error(loc, "%s", s), state->msgs);
}

static struct scope *current_scope(const struct parser_state *state)
{
	return state->scopes[state->scope];
}

static int open_scope(struct parser_state *state, struct scope *scope)
{
	if (state->scope >= array_size(state->scopes) - 1) {
		state->scope_err = true;
		return -1;
	}

	scope_init(scope, current_scope(state));
	state->scopes[++state->scope] = scope;

	return 0;
}

static void close_scope(struct parser_state *state)
{
	if (state->scope_err || state->scope == 0) {
		state->scope_err = false;
		return;
	}

	state->scope--;
}

static void location_init(void *scanner, struct parser_state *state,
			  struct location *loc)
{
	memset(loc, 0, sizeof(*loc));
	loc->indesc = state->indesc;
}

static void location_update(struct location *loc, struct location *rhs, int n)
{
	if (n) {
		loc->indesc       = rhs[n].indesc;
		loc->token_offset = rhs[1].token_offset;
		loc->line_offset  = rhs[1].line_offset;
		loc->first_line   = rhs[1].first_line;
		loc->first_column = rhs[1].first_column;
		loc->last_line    = rhs[n].last_line;
		loc->last_column  = rhs[n].last_column;
	} else {
		loc->indesc       = rhs[0].indesc;
		loc->token_offset = rhs[0].token_offset;
		loc->line_offset  = rhs[0].line_offset;
		loc->first_line   = loc->last_line   = rhs[0].last_line;
		loc->first_column = loc->last_column = rhs[0].last_column;
	}
}

static struct expr *handle_concat_expr(const struct location *loc,
					 struct expr *expr,
					 struct expr *expr_l, struct expr *expr_r,
					 struct location loc_rhs[3])
{
	if (expr->etype != EXPR_CONCAT) {
		expr = concat_expr_alloc(loc);
		compound_expr_add(expr, expr_l);
	} else {
		location_update(&expr_r->location, loc_rhs, 2);

		expr = expr_l;
		expr->location = *loc;
	}

	compound_expr_add(expr, expr_r);
	return expr;
}

static bool already_set(const void *attr, const struct location *loc,
			struct parser_state *state)
{
	if (!attr)
		return false;

	erec_queue(error(loc, "You can only specify this once. This statement is duplicated."),
		   state->msgs);
	return true;
}

static struct expr *ifname_expr_alloc(const struct location *location,
				      struct list_head *queue,
				      const char *name)
{
	size_t length = strlen(name);
	struct expr *expr;

	if (length == 0) {
		free_const(name);
		erec_queue(error(location, "empty interface name"), queue);
		return NULL;
	}

	if (length >= IFNAMSIZ) {
		free_const(name);
		erec_queue(error(location, "interface name too long"), queue);
		return NULL;
	}

	expr = constant_expr_alloc(location, &ifname_type, BYTEORDER_HOST_ENDIAN,
				   length * BITS_PER_BYTE, name);

	free_const(name);

	return expr;
}

static void timeout_state_free(struct timeout_state *s)
{
	free_const(s->timeout_str);
	free(s);
}

static void timeout_states_free(struct list_head *list)
{
	struct timeout_state *ts, *next;

	list_for_each_entry_safe(ts, next, list, head) {
		list_del(&ts->head);
		timeout_state_free(ts);
	}

	free(list);
}

#define YYLLOC_DEFAULT(Current, Rhs, N)	location_update(&Current, Rhs, N)

#define symbol_value(loc, str) \
	symbol_expr_alloc(loc, SYMBOL_VALUE, current_scope(state), str)

/* Declare those here to avoid compiler warnings */
void nft_set_debug(int, void *);
int nft_lex(void *, void *, void *);

#line 269 "src/parser_bison.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_NFT_SRC_PARSER_BISON_H_INCLUDED
# define YY_NFT_SRC_PARSER_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int nft_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    TOKEN_EOF = 0,                 /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    JUNK = 258,                    /* "junk"  */
    NEWLINE = 259,                 /* "newline"  */
    COLON = 260,                   /* "colon"  */
    SEMICOLON = 261,               /* "semicolon"  */
    COMMA = 262,                   /* "comma"  */
    DOT = 263,                     /* "."  */
    EQ = 264,                      /* "=="  */
    NEQ = 265,                     /* "!="  */
    LT = 266,                      /* "<"  */
    GT = 267,                      /* ">"  */
    GTE = 268,                     /* ">="  */
    LTE = 269,                     /* "<="  */
    LSHIFT = 270,                  /* "<<"  */
    RSHIFT = 271,                  /* ">>"  */
    AMPERSAND = 272,               /* "&"  */
    CARET = 273,                   /* "^"  */
    NOT = 274,                     /* "!"  */
    SLASH = 275,                   /* "/"  */
    ASTERISK = 276,                /* "*"  */
    DASH = 277,                    /* "-"  */
    AT = 278,                      /* "@"  */
    VMAP = 279,                    /* "vmap"  */
    PLUS = 280,                    /* "+"  */
    INCLUDE = 281,                 /* "include"  */
    DEFINE = 282,                  /* "define"  */
    REDEFINE = 283,                /* "redefine"  */
    UNDEFINE = 284,                /* "undefine"  */
    FIB = 285,                     /* "fib"  */
    SOCKET = 286,                  /* "socket"  */
    TRANSPARENT = 287,             /* "transparent"  */
    WILDCARD = 288,                /* "wildcard"  */
    CGROUPV2 = 289,                /* "cgroupv2"  */
    TPROXY = 290,                  /* "tproxy"  */
    OSF = 291,                     /* "osf"  */
    SYNPROXY = 292,                /* "synproxy"  */
    MSS = 293,                     /* "mss"  */
    WSCALE = 294,                  /* "wscale"  */
    TYPEOF = 295,                  /* "typeof"  */
    HOOK = 296,                    /* "hook"  */
    HOOKS = 297,                   /* "hooks"  */
    DEVICE = 298,                  /* "device"  */
    DEVICES = 299,                 /* "devices"  */
    TABLE = 300,                   /* "table"  */
    TABLES = 301,                  /* "tables"  */
    CHAIN = 302,                   /* "chain"  */
    CHAINS = 303,                  /* "chains"  */
    RULE = 304,                    /* "rule"  */
    RULES = 305,                   /* "rules"  */
    SETS = 306,                    /* "sets"  */
    SET = 307,                     /* "set"  */
    ELEMENT = 308,                 /* "element"  */
    MAP = 309,                     /* "map"  */
    MAPS = 310,                    /* "maps"  */
    FLOWTABLE = 311,               /* "flowtable"  */
    HANDLE = 312,                  /* "handle"  */
    RULESET = 313,                 /* "ruleset"  */
    TRACE = 314,                   /* "trace"  */
    INET = 315,                    /* "inet"  */
    NETDEV = 316,                  /* "netdev"  */
    ADD = 317,                     /* "add"  */
    UPDATE = 318,                  /* "update"  */
    REPLACE = 319,                 /* "replace"  */
    CREATE = 320,                  /* "create"  */
    INSERT = 321,                  /* "insert"  */
    DELETE = 322,                  /* "delete"  */
    GET = 323,                     /* "get"  */
    LIST = 324,                    /* "list"  */
    RESET = 325,                   /* "reset"  */
    FLUSH = 326,                   /* "flush"  */
    RENAME = 327,                  /* "rename"  */
    DESCRIBE = 328,                /* "describe"  */
    IMPORT = 329,                  /* "import"  */
    EXPORT = 330,                  /* "export"  */
    DESTROY = 331,                 /* "destroy"  */
    MONITOR = 332,                 /* "monitor"  */
    ALL = 333,                     /* "all"  */
    ACCEPT = 334,                  /* "accept"  */
    DROP = 335,                    /* "drop"  */
    CONTINUE = 336,                /* "continue"  */
    JUMP = 337,                    /* "jump"  */
    GOTO = 338,                    /* "goto"  */
    RETURN = 339,                  /* "return"  */
    TO = 340,                      /* "to"  */
    CONSTANT = 341,                /* "constant"  */
    INTERVAL = 342,                /* "interval"  */
    DYNAMIC = 343,                 /* "dynamic"  */
    AUTOMERGE = 344,               /* "auto-merge"  */
    TIMEOUT = 345,                 /* "timeout"  */
    GC_INTERVAL = 346,             /* "gc-interval"  */
    ELEMENTS = 347,                /* "elements"  */
    EXPIRES = 348,                 /* "expires"  */
    POLICY = 349,                  /* "policy"  */
    MEMORY = 350,                  /* "memory"  */
    PERFORMANCE = 351,             /* "performance"  */
    SIZE = 352,                    /* "size"  */
    FLOW = 353,                    /* "flow"  */
    OFFLOAD = 354,                 /* "offload"  */
    METER = 355,                   /* "meter"  */
    METERS = 356,                  /* "meters"  */
    FLOWTABLES = 357,              /* "flowtables"  */
    NUM = 358,                     /* "number"  */
    STRING = 359,                  /* "string"  */
    QUOTED_STRING = 360,           /* "quoted string"  */
    ASTERISK_STRING = 361,         /* "string with a trailing asterisk"  */
    LL_HDR = 362,                  /* "ll"  */
    NETWORK_HDR = 363,             /* "nh"  */
    TRANSPORT_HDR = 364,           /* "th"  */
    BRIDGE = 365,                  /* "bridge"  */
    ETHER = 366,                   /* "ether"  */
    SADDR = 367,                   /* "saddr"  */
    DADDR = 368,                   /* "daddr"  */
    TYPE = 369,                    /* "type"  */
    VLAN = 370,                    /* "vlan"  */
    ID = 371,                      /* "id"  */
    CFI = 372,                     /* "cfi"  */
    DEI = 373,                     /* "dei"  */
    PCP = 374,                     /* "pcp"  */
    ARP = 375,                     /* "arp"  */
    HTYPE = 376,                   /* "htype"  */
    PTYPE = 377,                   /* "ptype"  */
    HLEN = 378,                    /* "hlen"  */
    PLEN = 379,                    /* "plen"  */
    OPERATION = 380,               /* "operation"  */
    IP = 381,                      /* "ip"  */
    HDRVERSION = 382,              /* "version"  */
    HDRLENGTH = 383,               /* "hdrlength"  */
    DSCP = 384,                    /* "dscp"  */
    ECN = 385,                     /* "ecn"  */
    LENGTH = 386,                  /* "length"  */
    FRAG_OFF = 387,                /* "frag-off"  */
    TTL = 388,                     /* "ttl"  */
    PROTOCOL = 389,                /* "protocol"  */
    CHECKSUM = 390,                /* "checksum"  */
    PTR = 391,                     /* "ptr"  */
    VALUE = 392,                   /* "value"  */
    LSRR = 393,                    /* "lsrr"  */
    RR = 394,                      /* "rr"  */
    SSRR = 395,                    /* "ssrr"  */
    RA = 396,                      /* "ra"  */
    ICMP = 397,                    /* "icmp"  */
    CODE = 398,                    /* "code"  */
    SEQUENCE = 399,                /* "seq"  */
    GATEWAY = 400,                 /* "gateway"  */
    MTU = 401,                     /* "mtu"  */
    IGMP = 402,                    /* "igmp"  */
    MRT = 403,                     /* "mrt"  */
    OPTIONS = 404,                 /* "options"  */
    IP6 = 405,                     /* "ip6"  */
    PRIORITY = 406,                /* "priority"  */
    FLOWLABEL = 407,               /* "flowlabel"  */
    NEXTHDR = 408,                 /* "nexthdr"  */
    HOPLIMIT = 409,                /* "hoplimit"  */
    ICMP6 = 410,                   /* "icmpv6"  */
    PPTR = 411,                    /* "param-problem"  */
    MAXDELAY = 412,                /* "max-delay"  */
    TADDR = 413,                   /* "taddr"  */
    AH = 414,                      /* "ah"  */
    RESERVED = 415,                /* "reserved"  */
    SPI = 416,                     /* "spi"  */
    ESP = 417,                     /* "esp"  */
    COMP = 418,                    /* "comp"  */
    FLAGS = 419,                   /* "flags"  */
    CPI = 420,                     /* "cpi"  */
    PORT = 421,                    /* "port"  */
    UDP = 422,                     /* "udp"  */
    SPORT = 423,                   /* "sport"  */
    DPORT = 424,                   /* "dport"  */
    UDPLITE = 425,                 /* "udplite"  */
    CSUMCOV = 426,                 /* "csumcov"  */
    TCP = 427,                     /* "tcp"  */
    ACKSEQ = 428,                  /* "ackseq"  */
    DOFF = 429,                    /* "doff"  */
    WINDOW = 430,                  /* "window"  */
    URGPTR = 431,                  /* "urgptr"  */
    OPTION = 432,                  /* "option"  */
    ECHO = 433,                    /* "echo"  */
    EOL = 434,                     /* "eol"  */
    MPTCP = 435,                   /* "mptcp"  */
    NOP = 436,                     /* "nop"  */
    SACK = 437,                    /* "sack"  */
    SACK0 = 438,                   /* "sack0"  */
    SACK1 = 439,                   /* "sack1"  */
    SACK2 = 440,                   /* "sack2"  */
    SACK3 = 441,                   /* "sack3"  */
    SACK_PERM = 442,               /* "sack-permitted"  */
    FASTOPEN = 443,                /* "fastopen"  */
    MD5SIG = 444,                  /* "md5sig"  */
    TIMESTAMP = 445,               /* "timestamp"  */
    COUNT = 446,                   /* "count"  */
    LEFT = 447,                    /* "left"  */
    RIGHT = 448,                   /* "right"  */
    TSVAL = 449,                   /* "tsval"  */
    TSECR = 450,                   /* "tsecr"  */
    SUBTYPE = 451,                 /* "subtype"  */
    DCCP = 452,                    /* "dccp"  */
    VXLAN = 453,                   /* "vxlan"  */
    VNI = 454,                     /* "vni"  */
    GRE = 455,                     /* "gre"  */
    GRETAP = 456,                  /* "gretap"  */
    GENEVE = 457,                  /* "geneve"  */
    SCTP = 458,                    /* "sctp"  */
    CHUNK = 459,                   /* "chunk"  */
    DATA = 460,                    /* "data"  */
    INIT = 461,                    /* "init"  */
    INIT_ACK = 462,                /* "init-ack"  */
    HEARTBEAT = 463,               /* "heartbeat"  */
    HEARTBEAT_ACK = 464,           /* "heartbeat-ack"  */
    ABORT = 465,                   /* "abort"  */
    SHUTDOWN = 466,                /* "shutdown"  */
    SHUTDOWN_ACK = 467,            /* "shutdown-ack"  */
    ERROR = 468,                   /* "error"  */
    COOKIE_ECHO = 469,             /* "cookie-echo"  */
    COOKIE_ACK = 470,              /* "cookie-ack"  */
    ECNE = 471,                    /* "ecne"  */
    CWR = 472,                     /* "cwr"  */
    SHUTDOWN_COMPLETE = 473,       /* "shutdown-complete"  */
    ASCONF_ACK = 474,              /* "asconf-ack"  */
    FORWARD_TSN = 475,             /* "forward-tsn"  */
    ASCONF = 476,                  /* "asconf"  */
    TSN = 477,                     /* "tsn"  */
    STREAM = 478,                  /* "stream"  */
    SSN = 479,                     /* "ssn"  */
    PPID = 480,                    /* "ppid"  */
    INIT_TAG = 481,                /* "init-tag"  */
    A_RWND = 482,                  /* "a-rwnd"  */
    NUM_OSTREAMS = 483,            /* "num-outbound-streams"  */
    NUM_ISTREAMS = 484,            /* "num-inbound-streams"  */
    INIT_TSN = 485,                /* "initial-tsn"  */
    CUM_TSN_ACK = 486,             /* "cum-tsn-ack"  */
    NUM_GACK_BLOCKS = 487,         /* "num-gap-ack-blocks"  */
    NUM_DUP_TSNS = 488,            /* "num-dup-tsns"  */
    LOWEST_TSN = 489,              /* "lowest-tsn"  */
    SEQNO = 490,                   /* "seqno"  */
    NEW_CUM_TSN = 491,             /* "new-cum-tsn"  */
    VTAG = 492,                    /* "vtag"  */
    RT = 493,                      /* "rt"  */
    RT0 = 494,                     /* "rt0"  */
    RT2 = 495,                     /* "rt2"  */
    RT4 = 496,                     /* "srh"  */
    SEG_LEFT = 497,                /* "seg-left"  */
    ADDR = 498,                    /* "addr"  */
    LAST_ENT = 499,                /* "last-entry"  */
    TAG = 500,                     /* "tag"  */
    SID = 501,                     /* "sid"  */
    HBH = 502,                     /* "hbh"  */
    FRAG = 503,                    /* "frag"  */
    RESERVED2 = 504,               /* "reserved2"  */
    MORE_FRAGMENTS = 505,          /* "more-fragments"  */
    DST = 506,                     /* "dst"  */
    MH = 507,                      /* "mh"  */
    META = 508,                    /* "meta"  */
    MARK = 509,                    /* "mark"  */
    IIF = 510,                     /* "iif"  */
    IIFNAME = 511,                 /* "iifname"  */
    IIFTYPE = 512,                 /* "iiftype"  */
    OIF = 513,                     /* "oif"  */
    OIFNAME = 514,                 /* "oifname"  */
    OIFTYPE = 515,                 /* "oiftype"  */
    SKUID = 516,                   /* "skuid"  */
    SKGID = 517,                   /* "skgid"  */
    NFTRACE = 518,                 /* "nftrace"  */
    RTCLASSID = 519,               /* "rtclassid"  */
    IBRIPORT = 520,                /* "ibriport"  */
    OBRIPORT = 521,                /* "obriport"  */
    IBRIDGENAME = 522,             /* "ibrname"  */
    OBRIDGENAME = 523,             /* "obrname"  */
    PKTTYPE = 524,                 /* "pkttype"  */
    CPU = 525,                     /* "cpu"  */
    IIFGROUP = 526,                /* "iifgroup"  */
    OIFGROUP = 527,                /* "oifgroup"  */
    CGROUP = 528,                  /* "cgroup"  */
    TIME = 529,                    /* "time"  */
    CLASSID = 530,                 /* "classid"  */
    NEXTHOP = 531,                 /* "nexthop"  */
    CT = 532,                      /* "ct"  */
    L3PROTOCOL = 533,              /* "l3proto"  */
    PROTO_SRC = 534,               /* "proto-src"  */
    PROTO_DST = 535,               /* "proto-dst"  */
    ZONE = 536,                    /* "zone"  */
    DIRECTION = 537,               /* "direction"  */
    EVENT = 538,                   /* "event"  */
    EXPECTATION = 539,             /* "expectation"  */
    EXPIRATION = 540,              /* "expiration"  */
    HELPER = 541,                  /* "helper"  */
    LABEL = 542,                   /* "label"  */
    STATE = 543,                   /* "state"  */
    STATUS = 544,                  /* "status"  */
    ORIGINAL = 545,                /* "original"  */
    REPLY = 546,                   /* "reply"  */
    COUNTER = 547,                 /* "counter"  */
    NAME = 548,                    /* "name"  */
    PACKETS = 549,                 /* "packets"  */
    BYTES = 550,                   /* "bytes"  */
    AVGPKT = 551,                  /* "avgpkt"  */
    LAST = 552,                    /* "last"  */
    NEVER = 553,                   /* "never"  */
    COUNTERS = 554,                /* "counters"  */
    QUOTAS = 555,                  /* "quotas"  */
    LIMITS = 556,                  /* "limits"  */
    SYNPROXYS = 557,               /* "synproxys"  */
    HELPERS = 558,                 /* "helpers"  */
    LOG = 559,                     /* "log"  */
    PREFIX = 560,                  /* "prefix"  */
    GROUP = 561,                   /* "group"  */
    SNAPLEN = 562,                 /* "snaplen"  */
    QUEUE_THRESHOLD = 563,         /* "queue-threshold"  */
    LEVEL = 564,                   /* "level"  */
    LIMIT = 565,                   /* "limit"  */
    RATE = 566,                    /* "rate"  */
    BURST = 567,                   /* "burst"  */
    OVER = 568,                    /* "over"  */
    UNTIL = 569,                   /* "until"  */
    QUOTA = 570,                   /* "quota"  */
    USED = 571,                    /* "used"  */
    SECMARK = 572,                 /* "secmark"  */
    SECMARKS = 573,                /* "secmarks"  */
    SECOND = 574,                  /* "second"  */
    MINUTE = 575,                  /* "minute"  */
    HOUR = 576,                    /* "hour"  */
    DAY = 577,                     /* "day"  */
    WEEK = 578,                    /* "week"  */
    _REJECT = 579,                 /* "reject"  */
    WITH = 580,                    /* "with"  */
    ICMPX = 581,                   /* "icmpx"  */
    SNAT = 582,                    /* "snat"  */
    DNAT = 583,                    /* "dnat"  */
    MASQUERADE = 584,              /* "masquerade"  */
    REDIRECT = 585,                /* "redirect"  */
    RANDOM = 586,                  /* "random"  */
    FULLY_RANDOM = 587,            /* "fully-random"  */
    PERSISTENT = 588,              /* "persistent"  */
    QUEUE = 589,                   /* "queue"  */
    QUEUENUM = 590,                /* "num"  */
    BYPASS = 591,                  /* "bypass"  */
    FANOUT = 592,                  /* "fanout"  */
    DUP = 593,                     /* "dup"  */
    FWD = 594,                     /* "fwd"  */
    NUMGEN = 595,                  /* "numgen"  */
    INC = 596,                     /* "inc"  */
    MOD = 597,                     /* "mod"  */
    OFFSET = 598,                  /* "offset"  */
    JHASH = 599,                   /* "jhash"  */
    SYMHASH = 600,                 /* "symhash"  */
    SEED = 601,                    /* "seed"  */
    POSITION = 602,                /* "position"  */
    INDEX = 603,                   /* "index"  */
    COMMENT = 604,                 /* "comment"  */
    XML = 605,                     /* "xml"  */
    JSON = 606,                    /* "json"  */
    VM = 607,                      /* "vm"  */
    NOTRACK = 608,                 /* "notrack"  */
    EXISTS = 609,                  /* "exists"  */
    MISSING = 610,                 /* "missing"  */
    EXTHDR = 611,                  /* "exthdr"  */
    IPSEC = 612,                   /* "ipsec"  */
    REQID = 613,                   /* "reqid"  */
    SPNUM = 614,                   /* "spnum"  */
    IN = 615,                      /* "in"  */
    OUT = 616,                     /* "out"  */
    XT = 617                       /* "xt"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define TOKEN_EOF 0
#define YYerror 256
#define YYUNDEF 257
#define JUNK 258
#define NEWLINE 259
#define COLON 260
#define SEMICOLON 261
#define COMMA 262
#define DOT 263
#define EQ 264
#define NEQ 265
#define LT 266
#define GT 267
#define GTE 268
#define LTE 269
#define LSHIFT 270
#define RSHIFT 271
#define AMPERSAND 272
#define CARET 273
#define NOT 274
#define SLASH 275
#define ASTERISK 276
#define DASH 277
#define AT 278
#define VMAP 279
#define PLUS 280
#define INCLUDE 281
#define DEFINE 282
#define REDEFINE 283
#define UNDEFINE 284
#define FIB 285
#define SOCKET 286
#define TRANSPARENT 287
#define WILDCARD 288
#define CGROUPV2 289
#define TPROXY 290
#define OSF 291
#define SYNPROXY 292
#define MSS 293
#define WSCALE 294
#define TYPEOF 295
#define HOOK 296
#define HOOKS 297
#define DEVICE 298
#define DEVICES 299
#define TABLE 300
#define TABLES 301
#define CHAIN 302
#define CHAINS 303
#define RULE 304
#define RULES 305
#define SETS 306
#define SET 307
#define ELEMENT 308
#define MAP 309
#define MAPS 310
#define FLOWTABLE 311
#define HANDLE 312
#define RULESET 313
#define TRACE 314
#define INET 315
#define NETDEV 316
#define ADD 317
#define UPDATE 318
#define REPLACE 319
#define CREATE 320
#define INSERT 321
#define DELETE 322
#define GET 323
#define LIST 324
#define RESET 325
#define FLUSH 326
#define RENAME 327
#define DESCRIBE 328
#define IMPORT 329
#define EXPORT 330
#define DESTROY 331
#define MONITOR 332
#define ALL 333
#define ACCEPT 334
#define DROP 335
#define CONTINUE 336
#define JUMP 337
#define GOTO 338
#define RETURN 339
#define TO 340
#define CONSTANT 341
#define INTERVAL 342
#define DYNAMIC 343
#define AUTOMERGE 344
#define TIMEOUT 345
#define GC_INTERVAL 346
#define ELEMENTS 347
#define EXPIRES 348
#define POLICY 349
#define MEMORY 350
#define PERFORMANCE 351
#define SIZE 352
#define FLOW 353
#define OFFLOAD 354
#define METER 355
#define METERS 356
#define FLOWTABLES 357
#define NUM 358
#define STRING 359
#define QUOTED_STRING 360
#define ASTERISK_STRING 361
#define LL_HDR 362
#define NETWORK_HDR 363
#define TRANSPORT_HDR 364
#define BRIDGE 365
#define ETHER 366
#define SADDR 367
#define DADDR 368
#define TYPE 369
#define VLAN 370
#define ID 371
#define CFI 372
#define DEI 373
#define PCP 374
#define ARP 375
#define HTYPE 376
#define PTYPE 377
#define HLEN 378
#define PLEN 379
#define OPERATION 380
#define IP 381
#define HDRVERSION 382
#define HDRLENGTH 383
#define DSCP 384
#define ECN 385
#define LENGTH 386
#define FRAG_OFF 387
#define TTL 388
#define PROTOCOL 389
#define CHECKSUM 390
#define PTR 391
#define VALUE 392
#define LSRR 393
#define RR 394
#define SSRR 395
#define RA 396
#define ICMP 397
#define CODE 398
#define SEQUENCE 399
#define GATEWAY 400
#define MTU 401
#define IGMP 402
#define MRT 403
#define OPTIONS 404
#define IP6 405
#define PRIORITY 406
#define FLOWLABEL 407
#define NEXTHDR 408
#define HOPLIMIT 409
#define ICMP6 410
#define PPTR 411
#define MAXDELAY 412
#define TADDR 413
#define AH 414
#define RESERVED 415
#define SPI 416
#define ESP 417
#define COMP 418
#define FLAGS 419
#define CPI 420
#define PORT 421
#define UDP 422
#define SPORT 423
#define DPORT 424
#define UDPLITE 425
#define CSUMCOV 426
#define TCP 427
#define ACKSEQ 428
#define DOFF 429
#define WINDOW 430
#define URGPTR 431
#define OPTION 432
#define ECHO 433
#define EOL 434
#define MPTCP 435
#define NOP 436
#define SACK 437
#define SACK0 438
#define SACK1 439
#define SACK2 440
#define SACK3 441
#define SACK_PERM 442
#define FASTOPEN 443
#define MD5SIG 444
#define TIMESTAMP 445
#define COUNT 446
#define LEFT 447
#define RIGHT 448
#define TSVAL 449
#define TSECR 450
#define SUBTYPE 451
#define DCCP 452
#define VXLAN 453
#define VNI 454
#define GRE 455
#define GRETAP 456
#define GENEVE 457
#define SCTP 458
#define CHUNK 459
#define DATA 460
#define INIT 461
#define INIT_ACK 462
#define HEARTBEAT 463
#define HEARTBEAT_ACK 464
#define ABORT 465
#define SHUTDOWN 466
#define SHUTDOWN_ACK 467
#define ERROR 468
#define COOKIE_ECHO 469
#define COOKIE_ACK 470
#define ECNE 471
#define CWR 472
#define SHUTDOWN_COMPLETE 473
#define ASCONF_ACK 474
#define FORWARD_TSN 475
#define ASCONF 476
#define TSN 477
#define STREAM 478
#define SSN 479
#define PPID 480
#define INIT_TAG 481
#define A_RWND 482
#define NUM_OSTREAMS 483
#define NUM_ISTREAMS 484
#define INIT_TSN 485
#define CUM_TSN_ACK 486
#define NUM_GACK_BLOCKS 487
#define NUM_DUP_TSNS 488
#define LOWEST_TSN 489
#define SEQNO 490
#define NEW_CUM_TSN 491
#define VTAG 492
#define RT 493
#define RT0 494
#define RT2 495
#define RT4 496
#define SEG_LEFT 497
#define ADDR 498
#define LAST_ENT 499
#define TAG 500
#define SID 501
#define HBH 502
#define FRAG 503
#define RESERVED2 504
#define MORE_FRAGMENTS 505
#define DST 506
#define MH 507
#define META 508
#define MARK 509
#define IIF 510
#define IIFNAME 511
#define IIFTYPE 512
#define OIF 513
#define OIFNAME 514
#define OIFTYPE 515
#define SKUID 516
#define SKGID 517
#define NFTRACE 518
#define RTCLASSID 519
#define IBRIPORT 520
#define OBRIPORT 521
#define IBRIDGENAME 522
#define OBRIDGENAME 523
#define PKTTYPE 524
#define CPU 525
#define IIFGROUP 526
#define OIFGROUP 527
#define CGROUP 528
#define TIME 529
#define CLASSID 530
#define NEXTHOP 531
#define CT 532
#define L3PROTOCOL 533
#define PROTO_SRC 534
#define PROTO_DST 535
#define ZONE 536
#define DIRECTION 537
#define EVENT 538
#define EXPECTATION 539
#define EXPIRATION 540
#define HELPER 541
#define LABEL 542
#define STATE 543
#define STATUS 544
#define ORIGINAL 545
#define REPLY 546
#define COUNTER 547
#define NAME 548
#define PACKETS 549
#define BYTES 550
#define AVGPKT 551
#define LAST 552
#define NEVER 553
#define COUNTERS 554
#define QUOTAS 555
#define LIMITS 556
#define SYNPROXYS 557
#define HELPERS 558
#define LOG 559
#define PREFIX 560
#define GROUP 561
#define SNAPLEN 562
#define QUEUE_THRESHOLD 563
#define LEVEL 564
#define LIMIT 565
#define RATE 566
#define BURST 567
#define OVER 568
#define UNTIL 569
#define QUOTA 570
#define USED 571
#define SECMARK 572
#define SECMARKS 573
#define SECOND 574
#define MINUTE 575
#define HOUR 576
#define DAY 577
#define WEEK 578
#define _REJECT 579
#define WITH 580
#define ICMPX 581
#define SNAT 582
#define DNAT 583
#define MASQUERADE 584
#define REDIRECT 585
#define RANDOM 586
#define FULLY_RANDOM 587
#define PERSISTENT 588
#define QUEUE 589
#define QUEUENUM 590
#define BYPASS 591
#define FANOUT 592
#define DUP 593
#define FWD 594
#define NUMGEN 595
#define INC 596
#define MOD 597
#define OFFSET 598
#define JHASH 599
#define SYMHASH 600
#define SEED 601
#define POSITION 602
#define INDEX 603
#define COMMENT 604
#define XML 605
#define JSON 606
#define VM 607
#define NOTRACK 608
#define EXISTS 609
#define MISSING 610
#define EXTHDR 611
#define IPSEC 612
#define REQID 613
#define SPNUM 614
#define IN 615
#define OUT 616
#define XT 617

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 224 "src/parser_bison.y"

	uint64_t		val;
	uint32_t		val32;
	uint8_t			val8;
	const char *		string;

	struct list_head	*list;
	struct cmd		*cmd;
	struct handle		handle;
	struct table		*table;
	struct chain		*chain;
	struct rule		*rule;
	struct stmt		*stmt;
	struct expr		*expr;
	struct set		*set;
	struct obj		*obj;
	struct flowtable	*flowtable;
	struct ct		*ct;
	const struct datatype	*datatype;
	struct handle_spec	handle_spec;
	struct position_spec	position_spec;
	struct prio_spec	prio_spec;
	struct limit_rate	limit_rate;
	struct tcp_kind_field {
		uint16_t kind; /* must allow > 255 for SACK1, 2.. hack */
		uint8_t field;
	} tcp_kind_field;
	struct timeout_state	*timeout_state;

#line 1076 "src/parser_bison.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int nft_parse (struct nft_ctx *nft, void *scanner, struct parser_state *state);


#endif /* !YY_NFT_SRC_PARSER_BISON_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_JUNK = 3,                       /* "junk"  */
  YYSYMBOL_NEWLINE = 4,                    /* "newline"  */
  YYSYMBOL_COLON = 5,                      /* "colon"  */
  YYSYMBOL_SEMICOLON = 6,                  /* "semicolon"  */
  YYSYMBOL_COMMA = 7,                      /* "comma"  */
  YYSYMBOL_DOT = 8,                        /* "."  */
  YYSYMBOL_EQ = 9,                         /* "=="  */
  YYSYMBOL_NEQ = 10,                       /* "!="  */
  YYSYMBOL_LT = 11,                        /* "<"  */
  YYSYMBOL_GT = 12,                        /* ">"  */
  YYSYMBOL_GTE = 13,                       /* ">="  */
  YYSYMBOL_LTE = 14,                       /* "<="  */
  YYSYMBOL_LSHIFT = 15,                    /* "<<"  */
  YYSYMBOL_RSHIFT = 16,                    /* ">>"  */
  YYSYMBOL_AMPERSAND = 17,                 /* "&"  */
  YYSYMBOL_CARET = 18,                     /* "^"  */
  YYSYMBOL_NOT = 19,                       /* "!"  */
  YYSYMBOL_SLASH = 20,                     /* "/"  */
  YYSYMBOL_ASTERISK = 21,                  /* "*"  */
  YYSYMBOL_DASH = 22,                      /* "-"  */
  YYSYMBOL_AT = 23,                        /* "@"  */
  YYSYMBOL_VMAP = 24,                      /* "vmap"  */
  YYSYMBOL_PLUS = 25,                      /* "+"  */
  YYSYMBOL_INCLUDE = 26,                   /* "include"  */
  YYSYMBOL_DEFINE = 27,                    /* "define"  */
  YYSYMBOL_REDEFINE = 28,                  /* "redefine"  */
  YYSYMBOL_UNDEFINE = 29,                  /* "undefine"  */
  YYSYMBOL_FIB = 30,                       /* "fib"  */
  YYSYMBOL_SOCKET = 31,                    /* "socket"  */
  YYSYMBOL_TRANSPARENT = 32,               /* "transparent"  */
  YYSYMBOL_WILDCARD = 33,                  /* "wildcard"  */
  YYSYMBOL_CGROUPV2 = 34,                  /* "cgroupv2"  */
  YYSYMBOL_TPROXY = 35,                    /* "tproxy"  */
  YYSYMBOL_OSF = 36,                       /* "osf"  */
  YYSYMBOL_SYNPROXY = 37,                  /* "synproxy"  */
  YYSYMBOL_MSS = 38,                       /* "mss"  */
  YYSYMBOL_WSCALE = 39,                    /* "wscale"  */
  YYSYMBOL_TYPEOF = 40,                    /* "typeof"  */
  YYSYMBOL_HOOK = 41,                      /* "hook"  */
  YYSYMBOL_HOOKS = 42,                     /* "hooks"  */
  YYSYMBOL_DEVICE = 43,                    /* "device"  */
  YYSYMBOL_DEVICES = 44,                   /* "devices"  */
  YYSYMBOL_TABLE = 45,                     /* "table"  */
  YYSYMBOL_TABLES = 46,                    /* "tables"  */
  YYSYMBOL_CHAIN = 47,                     /* "chain"  */
  YYSYMBOL_CHAINS = 48,                    /* "chains"  */
  YYSYMBOL_RULE = 49,                      /* "rule"  */
  YYSYMBOL_RULES = 50,                     /* "rules"  */
  YYSYMBOL_SETS = 51,                      /* "sets"  */
  YYSYMBOL_SET = 52,                       /* "set"  */
  YYSYMBOL_ELEMENT = 53,                   /* "element"  */
  YYSYMBOL_MAP = 54,                       /* "map"  */
  YYSYMBOL_MAPS = 55,                      /* "maps"  */
  YYSYMBOL_FLOWTABLE = 56,                 /* "flowtable"  */
  YYSYMBOL_HANDLE = 57,                    /* "handle"  */
  YYSYMBOL_RULESET = 58,                   /* "ruleset"  */
  YYSYMBOL_TRACE = 59,                     /* "trace"  */
  YYSYMBOL_INET = 60,                      /* "inet"  */
  YYSYMBOL_NETDEV = 61,                    /* "netdev"  */
  YYSYMBOL_ADD = 62,                       /* "add"  */
  YYSYMBOL_UPDATE = 63,                    /* "update"  */
  YYSYMBOL_REPLACE = 64,                   /* "replace"  */
  YYSYMBOL_CREATE = 65,                    /* "create"  */
  YYSYMBOL_INSERT = 66,                    /* "insert"  */
  YYSYMBOL_DELETE = 67,                    /* "delete"  */
  YYSYMBOL_GET = 68,                       /* "get"  */
  YYSYMBOL_LIST = 69,                      /* "list"  */
  YYSYMBOL_RESET = 70,                     /* "reset"  */
  YYSYMBOL_FLUSH = 71,                     /* "flush"  */
  YYSYMBOL_RENAME = 72,                    /* "rename"  */
  YYSYMBOL_DESCRIBE = 73,                  /* "describe"  */
  YYSYMBOL_IMPORT = 74,                    /* "import"  */
  YYSYMBOL_EXPORT = 75,                    /* "export"  */
  YYSYMBOL_DESTROY = 76,                   /* "destroy"  */
  YYSYMBOL_MONITOR = 77,                   /* "monitor"  */
  YYSYMBOL_ALL = 78,                       /* "all"  */
  YYSYMBOL_ACCEPT = 79,                    /* "accept"  */
  YYSYMBOL_DROP = 80,                      /* "drop"  */
  YYSYMBOL_CONTINUE = 81,                  /* "continue"  */
  YYSYMBOL_JUMP = 82,                      /* "jump"  */
  YYSYMBOL_GOTO = 83,                      /* "goto"  */
  YYSYMBOL_RETURN = 84,                    /* "return"  */
  YYSYMBOL_TO = 85,                        /* "to"  */
  YYSYMBOL_CONSTANT = 86,                  /* "constant"  */
  YYSYMBOL_INTERVAL = 87,                  /* "interval"  */
  YYSYMBOL_DYNAMIC = 88,                   /* "dynamic"  */
  YYSYMBOL_AUTOMERGE = 89,                 /* "auto-merge"  */
  YYSYMBOL_TIMEOUT = 90,                   /* "timeout"  */
  YYSYMBOL_GC_INTERVAL = 91,               /* "gc-interval"  */
  YYSYMBOL_ELEMENTS = 92,                  /* "elements"  */
  YYSYMBOL_EXPIRES = 93,                   /* "expires"  */
  YYSYMBOL_POLICY = 94,                    /* "policy"  */
  YYSYMBOL_MEMORY = 95,                    /* "memory"  */
  YYSYMBOL_PERFORMANCE = 96,               /* "performance"  */
  YYSYMBOL_SIZE = 97,                      /* "size"  */
  YYSYMBOL_FLOW = 98,                      /* "flow"  */
  YYSYMBOL_OFFLOAD = 99,                   /* "offload"  */
  YYSYMBOL_METER = 100,                    /* "meter"  */
  YYSYMBOL_METERS = 101,                   /* "meters"  */
  YYSYMBOL_FLOWTABLES = 102,               /* "flowtables"  */
  YYSYMBOL_NUM = 103,                      /* "number"  */
  YYSYMBOL_STRING = 104,                   /* "string"  */
  YYSYMBOL_QUOTED_STRING = 105,            /* "quoted string"  */
  YYSYMBOL_ASTERISK_STRING = 106,          /* "string with a trailing asterisk"  */
  YYSYMBOL_LL_HDR = 107,                   /* "ll"  */
  YYSYMBOL_NETWORK_HDR = 108,              /* "nh"  */
  YYSYMBOL_TRANSPORT_HDR = 109,            /* "th"  */
  YYSYMBOL_BRIDGE = 110,                   /* "bridge"  */
  YYSYMBOL_ETHER = 111,                    /* "ether"  */
  YYSYMBOL_SADDR = 112,                    /* "saddr"  */
  YYSYMBOL_DADDR = 113,                    /* "daddr"  */
  YYSYMBOL_TYPE = 114,                     /* "type"  */
  YYSYMBOL_VLAN = 115,                     /* "vlan"  */
  YYSYMBOL_ID = 116,                       /* "id"  */
  YYSYMBOL_CFI = 117,                      /* "cfi"  */
  YYSYMBOL_DEI = 118,                      /* "dei"  */
  YYSYMBOL_PCP = 119,                      /* "pcp"  */
  YYSYMBOL_ARP = 120,                      /* "arp"  */
  YYSYMBOL_HTYPE = 121,                    /* "htype"  */
  YYSYMBOL_PTYPE = 122,                    /* "ptype"  */
  YYSYMBOL_HLEN = 123,                     /* "hlen"  */
  YYSYMBOL_PLEN = 124,                     /* "plen"  */
  YYSYMBOL_OPERATION = 125,                /* "operation"  */
  YYSYMBOL_IP = 126,                       /* "ip"  */
  YYSYMBOL_HDRVERSION = 127,               /* "version"  */
  YYSYMBOL_HDRLENGTH = 128,                /* "hdrlength"  */
  YYSYMBOL_DSCP = 129,                     /* "dscp"  */
  YYSYMBOL_ECN = 130,                      /* "ecn"  */
  YYSYMBOL_LENGTH = 131,                   /* "length"  */
  YYSYMBOL_FRAG_OFF = 132,                 /* "frag-off"  */
  YYSYMBOL_TTL = 133,                      /* "ttl"  */
  YYSYMBOL_PROTOCOL = 134,                 /* "protocol"  */
  YYSYMBOL_CHECKSUM = 135,                 /* "checksum"  */
  YYSYMBOL_PTR = 136,                      /* "ptr"  */
  YYSYMBOL_VALUE = 137,                    /* "value"  */
  YYSYMBOL_LSRR = 138,                     /* "lsrr"  */
  YYSYMBOL_RR = 139,                       /* "rr"  */
  YYSYMBOL_SSRR = 140,                     /* "ssrr"  */
  YYSYMBOL_RA = 141,                       /* "ra"  */
  YYSYMBOL_ICMP = 142,                     /* "icmp"  */
  YYSYMBOL_CODE = 143,                     /* "code"  */
  YYSYMBOL_SEQUENCE = 144,                 /* "seq"  */
  YYSYMBOL_GATEWAY = 145,                  /* "gateway"  */
  YYSYMBOL_MTU = 146,                      /* "mtu"  */
  YYSYMBOL_IGMP = 147,                     /* "igmp"  */
  YYSYMBOL_MRT = 148,                      /* "mrt"  */
  YYSYMBOL_OPTIONS = 149,                  /* "options"  */
  YYSYMBOL_IP6 = 150,                      /* "ip6"  */
  YYSYMBOL_PRIORITY = 151,                 /* "priority"  */
  YYSYMBOL_FLOWLABEL = 152,                /* "flowlabel"  */
  YYSYMBOL_NEXTHDR = 153,                  /* "nexthdr"  */
  YYSYMBOL_HOPLIMIT = 154,                 /* "hoplimit"  */
  YYSYMBOL_ICMP6 = 155,                    /* "icmpv6"  */
  YYSYMBOL_PPTR = 156,                     /* "param-problem"  */
  YYSYMBOL_MAXDELAY = 157,                 /* "max-delay"  */
  YYSYMBOL_TADDR = 158,                    /* "taddr"  */
  YYSYMBOL_AH = 159,                       /* "ah"  */
  YYSYMBOL_RESERVED = 160,                 /* "reserved"  */
  YYSYMBOL_SPI = 161,                      /* "spi"  */
  YYSYMBOL_ESP = 162,                      /* "esp"  */
  YYSYMBOL_COMP = 163,                     /* "comp"  */
  YYSYMBOL_FLAGS = 164,                    /* "flags"  */
  YYSYMBOL_CPI = 165,                      /* "cpi"  */
  YYSYMBOL_PORT = 166,                     /* "port"  */
  YYSYMBOL_UDP = 167,                      /* "udp"  */
  YYSYMBOL_SPORT = 168,                    /* "sport"  */
  YYSYMBOL_DPORT = 169,                    /* "dport"  */
  YYSYMBOL_UDPLITE = 170,                  /* "udplite"  */
  YYSYMBOL_CSUMCOV = 171,                  /* "csumcov"  */
  YYSYMBOL_TCP = 172,                      /* "tcp"  */
  YYSYMBOL_ACKSEQ = 173,                   /* "ackseq"  */
  YYSYMBOL_DOFF = 174,                     /* "doff"  */
  YYSYMBOL_WINDOW = 175,                   /* "window"  */
  YYSYMBOL_URGPTR = 176,                   /* "urgptr"  */
  YYSYMBOL_OPTION = 177,                   /* "option"  */
  YYSYMBOL_ECHO = 178,                     /* "echo"  */
  YYSYMBOL_EOL = 179,                      /* "eol"  */
  YYSYMBOL_MPTCP = 180,                    /* "mptcp"  */
  YYSYMBOL_NOP = 181,                      /* "nop"  */
  YYSYMBOL_SACK = 182,                     /* "sack"  */
  YYSYMBOL_SACK0 = 183,                    /* "sack0"  */
  YYSYMBOL_SACK1 = 184,                    /* "sack1"  */
  YYSYMBOL_SACK2 = 185,                    /* "sack2"  */
  YYSYMBOL_SACK3 = 186,                    /* "sack3"  */
  YYSYMBOL_SACK_PERM = 187,                /* "sack-permitted"  */
  YYSYMBOL_FASTOPEN = 188,                 /* "fastopen"  */
  YYSYMBOL_MD5SIG = 189,                   /* "md5sig"  */
  YYSYMBOL_TIMESTAMP = 190,                /* "timestamp"  */
  YYSYMBOL_COUNT = 191,                    /* "count"  */
  YYSYMBOL_LEFT = 192,                     /* "left"  */
  YYSYMBOL_RIGHT = 193,                    /* "right"  */
  YYSYMBOL_TSVAL = 194,                    /* "tsval"  */
  YYSYMBOL_TSECR = 195,                    /* "tsecr"  */
  YYSYMBOL_SUBTYPE = 196,                  /* "subtype"  */
  YYSYMBOL_DCCP = 197,                     /* "dccp"  */
  YYSYMBOL_VXLAN = 198,                    /* "vxlan"  */
  YYSYMBOL_VNI = 199,                      /* "vni"  */
  YYSYMBOL_GRE = 200,                      /* "gre"  */
  YYSYMBOL_GRETAP = 201,                   /* "gretap"  */
  YYSYMBOL_GENEVE = 202,                   /* "geneve"  */
  YYSYMBOL_SCTP = 203,                     /* "sctp"  */
  YYSYMBOL_CHUNK = 204,                    /* "chunk"  */
  YYSYMBOL_DATA = 205,                     /* "data"  */
  YYSYMBOL_INIT = 206,                     /* "init"  */
  YYSYMBOL_INIT_ACK = 207,                 /* "init-ack"  */
  YYSYMBOL_HEARTBEAT = 208,                /* "heartbeat"  */
  YYSYMBOL_HEARTBEAT_ACK = 209,            /* "heartbeat-ack"  */
  YYSYMBOL_ABORT = 210,                    /* "abort"  */
  YYSYMBOL_SHUTDOWN = 211,                 /* "shutdown"  */
  YYSYMBOL_SHUTDOWN_ACK = 212,             /* "shutdown-ack"  */
  YYSYMBOL_ERROR = 213,                    /* "error"  */
  YYSYMBOL_COOKIE_ECHO = 214,              /* "cookie-echo"  */
  YYSYMBOL_COOKIE_ACK = 215,               /* "cookie-ack"  */
  YYSYMBOL_ECNE = 216,                     /* "ecne"  */
  YYSYMBOL_CWR = 217,                      /* "cwr"  */
  YYSYMBOL_SHUTDOWN_COMPLETE = 218,        /* "shutdown-complete"  */
  YYSYMBOL_ASCONF_ACK = 219,               /* "asconf-ack"  */
  YYSYMBOL_FORWARD_TSN = 220,              /* "forward-tsn"  */
  YYSYMBOL_ASCONF = 221,                   /* "asconf"  */
  YYSYMBOL_TSN = 222,                      /* "tsn"  */
  YYSYMBOL_STREAM = 223,                   /* "stream"  */
  YYSYMBOL_SSN = 224,                      /* "ssn"  */
  YYSYMBOL_PPID = 225,                     /* "ppid"  */
  YYSYMBOL_INIT_TAG = 226,                 /* "init-tag"  */
  YYSYMBOL_A_RWND = 227,                   /* "a-rwnd"  */
  YYSYMBOL_NUM_OSTREAMS = 228,             /* "num-outbound-streams"  */
  YYSYMBOL_NUM_ISTREAMS = 229,             /* "num-inbound-streams"  */
  YYSYMBOL_INIT_TSN = 230,                 /* "initial-tsn"  */
  YYSYMBOL_CUM_TSN_ACK = 231,              /* "cum-tsn-ack"  */
  YYSYMBOL_NUM_GACK_BLOCKS = 232,          /* "num-gap-ack-blocks"  */
  YYSYMBOL_NUM_DUP_TSNS = 233,             /* "num-dup-tsns"  */
  YYSYMBOL_LOWEST_TSN = 234,               /* "lowest-tsn"  */
  YYSYMBOL_SEQNO = 235,                    /* "seqno"  */
  YYSYMBOL_NEW_CUM_TSN = 236,              /* "new-cum-tsn"  */
  YYSYMBOL_VTAG = 237,                     /* "vtag"  */
  YYSYMBOL_RT = 238,                       /* "rt"  */
  YYSYMBOL_RT0 = 239,                      /* "rt0"  */
  YYSYMBOL_RT2 = 240,                      /* "rt2"  */
  YYSYMBOL_RT4 = 241,                      /* "srh"  */
  YYSYMBOL_SEG_LEFT = 242,                 /* "seg-left"  */
  YYSYMBOL_ADDR = 243,                     /* "addr"  */
  YYSYMBOL_LAST_ENT = 244,                 /* "last-entry"  */
  YYSYMBOL_TAG = 245,                      /* "tag"  */
  YYSYMBOL_SID = 246,                      /* "sid"  */
  YYSYMBOL_HBH = 247,                      /* "hbh"  */
  YYSYMBOL_FRAG = 248,                     /* "frag"  */
  YYSYMBOL_RESERVED2 = 249,                /* "reserved2"  */
  YYSYMBOL_MORE_FRAGMENTS = 250,           /* "more-fragments"  */
  YYSYMBOL_DST = 251,                      /* "dst"  */
  YYSYMBOL_MH = 252,                       /* "mh"  */
  YYSYMBOL_META = 253,                     /* "meta"  */
  YYSYMBOL_MARK = 254,                     /* "mark"  */
  YYSYMBOL_IIF = 255,                      /* "iif"  */
  YYSYMBOL_IIFNAME = 256,                  /* "iifname"  */
  YYSYMBOL_IIFTYPE = 257,                  /* "iiftype"  */
  YYSYMBOL_OIF = 258,                      /* "oif"  */
  YYSYMBOL_OIFNAME = 259,                  /* "oifname"  */
  YYSYMBOL_OIFTYPE = 260,                  /* "oiftype"  */
  YYSYMBOL_SKUID = 261,                    /* "skuid"  */
  YYSYMBOL_SKGID = 262,                    /* "skgid"  */
  YYSYMBOL_NFTRACE = 263,                  /* "nftrace"  */
  YYSYMBOL_RTCLASSID = 264,                /* "rtclassid"  */
  YYSYMBOL_IBRIPORT = 265,                 /* "ibriport"  */
  YYSYMBOL_OBRIPORT = 266,                 /* "obriport"  */
  YYSYMBOL_IBRIDGENAME = 267,              /* "ibrname"  */
  YYSYMBOL_OBRIDGENAME = 268,              /* "obrname"  */
  YYSYMBOL_PKTTYPE = 269,                  /* "pkttype"  */
  YYSYMBOL_CPU = 270,                      /* "cpu"  */
  YYSYMBOL_IIFGROUP = 271,                 /* "iifgroup"  */
  YYSYMBOL_OIFGROUP = 272,                 /* "oifgroup"  */
  YYSYMBOL_CGROUP = 273,                   /* "cgroup"  */
  YYSYMBOL_TIME = 274,                     /* "time"  */
  YYSYMBOL_CLASSID = 275,                  /* "classid"  */
  YYSYMBOL_NEXTHOP = 276,                  /* "nexthop"  */
  YYSYMBOL_CT = 277,                       /* "ct"  */
  YYSYMBOL_L3PROTOCOL = 278,               /* "l3proto"  */
  YYSYMBOL_PROTO_SRC = 279,                /* "proto-src"  */
  YYSYMBOL_PROTO_DST = 280,                /* "proto-dst"  */
  YYSYMBOL_ZONE = 281,                     /* "zone"  */
  YYSYMBOL_DIRECTION = 282,                /* "direction"  */
  YYSYMBOL_EVENT = 283,                    /* "event"  */
  YYSYMBOL_EXPECTATION = 284,              /* "expectation"  */
  YYSYMBOL_EXPIRATION = 285,               /* "expiration"  */
  YYSYMBOL_HELPER = 286,                   /* "helper"  */
  YYSYMBOL_LABEL = 287,                    /* "label"  */
  YYSYMBOL_STATE = 288,                    /* "state"  */
  YYSYMBOL_STATUS = 289,                   /* "status"  */
  YYSYMBOL_ORIGINAL = 290,                 /* "original"  */
  YYSYMBOL_REPLY = 291,                    /* "reply"  */
  YYSYMBOL_COUNTER = 292,                  /* "counter"  */
  YYSYMBOL_NAME = 293,                     /* "name"  */
  YYSYMBOL_PACKETS = 294,                  /* "packets"  */
  YYSYMBOL_BYTES = 295,                    /* "bytes"  */
  YYSYMBOL_AVGPKT = 296,                   /* "avgpkt"  */
  YYSYMBOL_LAST = 297,                     /* "last"  */
  YYSYMBOL_NEVER = 298,                    /* "never"  */
  YYSYMBOL_COUNTERS = 299,                 /* "counters"  */
  YYSYMBOL_QUOTAS = 300,                   /* "quotas"  */
  YYSYMBOL_LIMITS = 301,                   /* "limits"  */
  YYSYMBOL_SYNPROXYS = 302,                /* "synproxys"  */
  YYSYMBOL_HELPERS = 303,                  /* "helpers"  */
  YYSYMBOL_LOG = 304,                      /* "log"  */
  YYSYMBOL_PREFIX = 305,                   /* "prefix"  */
  YYSYMBOL_GROUP = 306,                    /* "group"  */
  YYSYMBOL_SNAPLEN = 307,                  /* "snaplen"  */
  YYSYMBOL_QUEUE_THRESHOLD = 308,          /* "queue-threshold"  */
  YYSYMBOL_LEVEL = 309,                    /* "level"  */
  YYSYMBOL_LIMIT = 310,                    /* "limit"  */
  YYSYMBOL_RATE = 311,                     /* "rate"  */
  YYSYMBOL_BURST = 312,                    /* "burst"  */
  YYSYMBOL_OVER = 313,                     /* "over"  */
  YYSYMBOL_UNTIL = 314,                    /* "until"  */
  YYSYMBOL_QUOTA = 315,                    /* "quota"  */
  YYSYMBOL_USED = 316,                     /* "used"  */
  YYSYMBOL_SECMARK = 317,                  /* "secmark"  */
  YYSYMBOL_SECMARKS = 318,                 /* "secmarks"  */
  YYSYMBOL_SECOND = 319,                   /* "second"  */
  YYSYMBOL_MINUTE = 320,                   /* "minute"  */
  YYSYMBOL_HOUR = 321,                     /* "hour"  */
  YYSYMBOL_DAY = 322,                      /* "day"  */
  YYSYMBOL_WEEK = 323,                     /* "week"  */
  YYSYMBOL__REJECT = 324,                  /* "reject"  */
  YYSYMBOL_WITH = 325,                     /* "with"  */
  YYSYMBOL_ICMPX = 326,                    /* "icmpx"  */
  YYSYMBOL_SNAT = 327,                     /* "snat"  */
  YYSYMBOL_DNAT = 328,                     /* "dnat"  */
  YYSYMBOL_MASQUERADE = 329,               /* "masquerade"  */
  YYSYMBOL_REDIRECT = 330,                 /* "redirect"  */
  YYSYMBOL_RANDOM = 331,                   /* "random"  */
  YYSYMBOL_FULLY_RANDOM = 332,             /* "fully-random"  */
  YYSYMBOL_PERSISTENT = 333,               /* "persistent"  */
  YYSYMBOL_QUEUE = 334,                    /* "queue"  */
  YYSYMBOL_QUEUENUM = 335,                 /* "num"  */
  YYSYMBOL_BYPASS = 336,                   /* "bypass"  */
  YYSYMBOL_FANOUT = 337,                   /* "fanout"  */
  YYSYMBOL_DUP = 338,                      /* "dup"  */
  YYSYMBOL_FWD = 339,                      /* "fwd"  */
  YYSYMBOL_NUMGEN = 340,                   /* "numgen"  */
  YYSYMBOL_INC = 341,                      /* "inc"  */
  YYSYMBOL_MOD = 342,                      /* "mod"  */
  YYSYMBOL_OFFSET = 343,                   /* "offset"  */
  YYSYMBOL_JHASH = 344,                    /* "jhash"  */
  YYSYMBOL_SYMHASH = 345,                  /* "symhash"  */
  YYSYMBOL_SEED = 346,                     /* "seed"  */
  YYSYMBOL_POSITION = 347,                 /* "position"  */
  YYSYMBOL_INDEX = 348,                    /* "index"  */
  YYSYMBOL_COMMENT = 349,                  /* "comment"  */
  YYSYMBOL_XML = 350,                      /* "xml"  */
  YYSYMBOL_JSON = 351,                     /* "json"  */
  YYSYMBOL_VM = 352,                       /* "vm"  */
  YYSYMBOL_NOTRACK = 353,                  /* "notrack"  */
  YYSYMBOL_EXISTS = 354,                   /* "exists"  */
  YYSYMBOL_MISSING = 355,                  /* "missing"  */
  YYSYMBOL_EXTHDR = 356,                   /* "exthdr"  */
  YYSYMBOL_IPSEC = 357,                    /* "ipsec"  */
  YYSYMBOL_REQID = 358,                    /* "reqid"  */
  YYSYMBOL_SPNUM = 359,                    /* "spnum"  */
  YYSYMBOL_IN = 360,                       /* "in"  */
  YYSYMBOL_OUT = 361,                      /* "out"  */
  YYSYMBOL_XT = 362,                       /* "xt"  */
  YYSYMBOL_363_ = 363,                     /* '='  */
  YYSYMBOL_364_ = 364,                     /* '{'  */
  YYSYMBOL_365_ = 365,                     /* '}'  */
  YYSYMBOL_366_ = 366,                     /* '('  */
  YYSYMBOL_367_ = 367,                     /* ')'  */
  YYSYMBOL_368_ = 368,                     /* '|'  */
  YYSYMBOL_369_ = 369,                     /* '$'  */
  YYSYMBOL_370_ = 370,                     /* '['  */
  YYSYMBOL_371_ = 371,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 372,                 /* $accept  */
  YYSYMBOL_input = 373,                    /* input  */
  YYSYMBOL_stmt_separator = 374,           /* stmt_separator  */
  YYSYMBOL_opt_newline = 375,              /* opt_newline  */
  YYSYMBOL_close_scope_ah = 376,           /* close_scope_ah  */
  YYSYMBOL_close_scope_arp = 377,          /* close_scope_arp  */
  YYSYMBOL_close_scope_at = 378,           /* close_scope_at  */
  YYSYMBOL_close_scope_comp = 379,         /* close_scope_comp  */
  YYSYMBOL_close_scope_ct = 380,           /* close_scope_ct  */
  YYSYMBOL_close_scope_counter = 381,      /* close_scope_counter  */
  YYSYMBOL_close_scope_last = 382,         /* close_scope_last  */
  YYSYMBOL_close_scope_dccp = 383,         /* close_scope_dccp  */
  YYSYMBOL_close_scope_destroy = 384,      /* close_scope_destroy  */
  YYSYMBOL_close_scope_dst = 385,          /* close_scope_dst  */
  YYSYMBOL_close_scope_dup = 386,          /* close_scope_dup  */
  YYSYMBOL_close_scope_esp = 387,          /* close_scope_esp  */
  YYSYMBOL_close_scope_eth = 388,          /* close_scope_eth  */
  YYSYMBOL_close_scope_export = 389,       /* close_scope_export  */
  YYSYMBOL_close_scope_fib = 390,          /* close_scope_fib  */
  YYSYMBOL_close_scope_frag = 391,         /* close_scope_frag  */
  YYSYMBOL_close_scope_fwd = 392,          /* close_scope_fwd  */
  YYSYMBOL_close_scope_gre = 393,          /* close_scope_gre  */
  YYSYMBOL_close_scope_hash = 394,         /* close_scope_hash  */
  YYSYMBOL_close_scope_hbh = 395,          /* close_scope_hbh  */
  YYSYMBOL_close_scope_ip = 396,           /* close_scope_ip  */
  YYSYMBOL_close_scope_ip6 = 397,          /* close_scope_ip6  */
  YYSYMBOL_close_scope_vlan = 398,         /* close_scope_vlan  */
  YYSYMBOL_close_scope_icmp = 399,         /* close_scope_icmp  */
  YYSYMBOL_close_scope_igmp = 400,         /* close_scope_igmp  */
  YYSYMBOL_close_scope_import = 401,       /* close_scope_import  */
  YYSYMBOL_close_scope_ipsec = 402,        /* close_scope_ipsec  */
  YYSYMBOL_close_scope_list = 403,         /* close_scope_list  */
  YYSYMBOL_close_scope_limit = 404,        /* close_scope_limit  */
  YYSYMBOL_close_scope_meta = 405,         /* close_scope_meta  */
  YYSYMBOL_close_scope_mh = 406,           /* close_scope_mh  */
  YYSYMBOL_close_scope_monitor = 407,      /* close_scope_monitor  */
  YYSYMBOL_close_scope_nat = 408,          /* close_scope_nat  */
  YYSYMBOL_close_scope_numgen = 409,       /* close_scope_numgen  */
  YYSYMBOL_close_scope_osf = 410,          /* close_scope_osf  */
  YYSYMBOL_close_scope_policy = 411,       /* close_scope_policy  */
  YYSYMBOL_close_scope_quota = 412,        /* close_scope_quota  */
  YYSYMBOL_close_scope_queue = 413,        /* close_scope_queue  */
  YYSYMBOL_close_scope_reject = 414,       /* close_scope_reject  */
  YYSYMBOL_close_scope_reset = 415,        /* close_scope_reset  */
  YYSYMBOL_close_scope_rt = 416,           /* close_scope_rt  */
  YYSYMBOL_close_scope_sctp = 417,         /* close_scope_sctp  */
  YYSYMBOL_close_scope_sctp_chunk = 418,   /* close_scope_sctp_chunk  */
  YYSYMBOL_close_scope_secmark = 419,      /* close_scope_secmark  */
  YYSYMBOL_close_scope_socket = 420,       /* close_scope_socket  */
  YYSYMBOL_close_scope_tcp = 421,          /* close_scope_tcp  */
  YYSYMBOL_close_scope_tproxy = 422,       /* close_scope_tproxy  */
  YYSYMBOL_close_scope_type = 423,         /* close_scope_type  */
  YYSYMBOL_close_scope_th = 424,           /* close_scope_th  */
  YYSYMBOL_close_scope_udp = 425,          /* close_scope_udp  */
  YYSYMBOL_close_scope_udplite = 426,      /* close_scope_udplite  */
  YYSYMBOL_close_scope_log = 427,          /* close_scope_log  */
  YYSYMBOL_close_scope_synproxy = 428,     /* close_scope_synproxy  */
  YYSYMBOL_close_scope_xt = 429,           /* close_scope_xt  */
  YYSYMBOL_common_block = 430,             /* common_block  */
  YYSYMBOL_line = 431,                     /* line  */
  YYSYMBOL_base_cmd = 432,                 /* base_cmd  */
  YYSYMBOL_add_cmd = 433,                  /* add_cmd  */
  YYSYMBOL_replace_cmd = 434,              /* replace_cmd  */
  YYSYMBOL_create_cmd = 435,               /* create_cmd  */
  YYSYMBOL_insert_cmd = 436,               /* insert_cmd  */
  YYSYMBOL_table_or_id_spec = 437,         /* table_or_id_spec  */
  YYSYMBOL_chain_or_id_spec = 438,         /* chain_or_id_spec  */
  YYSYMBOL_set_or_id_spec = 439,           /* set_or_id_spec  */
  YYSYMBOL_obj_or_id_spec = 440,           /* obj_or_id_spec  */
  YYSYMBOL_delete_cmd = 441,               /* delete_cmd  */
  YYSYMBOL_destroy_cmd = 442,              /* destroy_cmd  */
  YYSYMBOL_get_cmd = 443,                  /* get_cmd  */
  YYSYMBOL_list_cmd = 444,                 /* list_cmd  */
  YYSYMBOL_basehook_device_name = 445,     /* basehook_device_name  */
  YYSYMBOL_basehook_spec = 446,            /* basehook_spec  */
  YYSYMBOL_reset_cmd = 447,                /* reset_cmd  */
  YYSYMBOL_flush_cmd = 448,                /* flush_cmd  */
  YYSYMBOL_rename_cmd = 449,               /* rename_cmd  */
  YYSYMBOL_import_cmd = 450,               /* import_cmd  */
  YYSYMBOL_export_cmd = 451,               /* export_cmd  */
  YYSYMBOL_monitor_cmd = 452,              /* monitor_cmd  */
  YYSYMBOL_monitor_event = 453,            /* monitor_event  */
  YYSYMBOL_monitor_object = 454,           /* monitor_object  */
  YYSYMBOL_monitor_format = 455,           /* monitor_format  */
  YYSYMBOL_markup_format = 456,            /* markup_format  */
  YYSYMBOL_describe_cmd = 457,             /* describe_cmd  */
  YYSYMBOL_table_block_alloc = 458,        /* table_block_alloc  */
  YYSYMBOL_table_options = 459,            /* table_options  */
  YYSYMBOL_table_flags = 460,              /* table_flags  */
  YYSYMBOL_table_flag = 461,               /* table_flag  */
  YYSYMBOL_table_block = 462,              /* table_block  */
  YYSYMBOL_chain_block_alloc = 463,        /* chain_block_alloc  */
  YYSYMBOL_chain_block = 464,              /* chain_block  */
  YYSYMBOL_subchain_block = 465,           /* subchain_block  */
  YYSYMBOL_typeof_verdict_expr = 466,      /* typeof_verdict_expr  */
  YYSYMBOL_typeof_data_expr = 467,         /* typeof_data_expr  */
  YYSYMBOL_typeof_expr = 468,              /* typeof_expr  */
  YYSYMBOL_set_block_alloc = 469,          /* set_block_alloc  */
  YYSYMBOL_typeof_key_expr = 470,          /* typeof_key_expr  */
  YYSYMBOL_set_block = 471,                /* set_block  */
  YYSYMBOL_set_block_expr = 472,           /* set_block_expr  */
  YYSYMBOL_set_flag_list = 473,            /* set_flag_list  */
  YYSYMBOL_set_flag = 474,                 /* set_flag  */
  YYSYMBOL_map_block_alloc = 475,          /* map_block_alloc  */
  YYSYMBOL_ct_obj_type_map = 476,          /* ct_obj_type_map  */
  YYSYMBOL_map_block_obj_type = 477,       /* map_block_obj_type  */
  YYSYMBOL_map_block_obj_typeof = 478,     /* map_block_obj_typeof  */
  YYSYMBOL_map_block_data_interval = 479,  /* map_block_data_interval  */
  YYSYMBOL_map_block = 480,                /* map_block  */
  YYSYMBOL_set_mechanism = 481,            /* set_mechanism  */
  YYSYMBOL_set_policy_spec = 482,          /* set_policy_spec  */
  YYSYMBOL_flowtable_block_alloc = 483,    /* flowtable_block_alloc  */
  YYSYMBOL_flowtable_block = 484,          /* flowtable_block  */
  YYSYMBOL_flowtable_expr = 485,           /* flowtable_expr  */
  YYSYMBOL_flowtable_list_expr = 486,      /* flowtable_list_expr  */
  YYSYMBOL_flowtable_expr_member = 487,    /* flowtable_expr_member  */
  YYSYMBOL_data_type_atom_expr = 488,      /* data_type_atom_expr  */
  YYSYMBOL_data_type_expr = 489,           /* data_type_expr  */
  YYSYMBOL_obj_block_alloc = 490,          /* obj_block_alloc  */
  YYSYMBOL_counter_block = 491,            /* counter_block  */
  YYSYMBOL_quota_block = 492,              /* quota_block  */
  YYSYMBOL_ct_helper_block = 493,          /* ct_helper_block  */
  YYSYMBOL_ct_timeout_block = 494,         /* ct_timeout_block  */
  YYSYMBOL_ct_expect_block = 495,          /* ct_expect_block  */
  YYSYMBOL_limit_block = 496,              /* limit_block  */
  YYSYMBOL_secmark_block = 497,            /* secmark_block  */
  YYSYMBOL_synproxy_block = 498,           /* synproxy_block  */
  YYSYMBOL_type_identifier = 499,          /* type_identifier  */
  YYSYMBOL_hook_spec = 500,                /* hook_spec  */
  YYSYMBOL_prio_spec = 501,                /* prio_spec  */
  YYSYMBOL_extended_prio_name = 502,       /* extended_prio_name  */
  YYSYMBOL_extended_prio_spec = 503,       /* extended_prio_spec  */
  YYSYMBOL_int_num = 504,                  /* int_num  */
  YYSYMBOL_dev_spec = 505,                 /* dev_spec  */
  YYSYMBOL_flags_spec = 506,               /* flags_spec  */
  YYSYMBOL_policy_spec = 507,              /* policy_spec  */
  YYSYMBOL_policy_expr = 508,              /* policy_expr  */
  YYSYMBOL_chain_policy = 509,             /* chain_policy  */
  YYSYMBOL_identifier = 510,               /* identifier  */
  YYSYMBOL_string = 511,                   /* string  */
  YYSYMBOL_time_spec = 512,                /* time_spec  */
  YYSYMBOL_time_spec_or_num_s = 513,       /* time_spec_or_num_s  */
  YYSYMBOL_family_spec = 514,              /* family_spec  */
  YYSYMBOL_family_spec_explicit = 515,     /* family_spec_explicit  */
  YYSYMBOL_table_spec = 516,               /* table_spec  */
  YYSYMBOL_tableid_spec = 517,             /* tableid_spec  */
  YYSYMBOL_chain_spec = 518,               /* chain_spec  */
  YYSYMBOL_chainid_spec = 519,             /* chainid_spec  */
  YYSYMBOL_chain_identifier = 520,         /* chain_identifier  */
  YYSYMBOL_set_spec = 521,                 /* set_spec  */
  YYSYMBOL_setid_spec = 522,               /* setid_spec  */
  YYSYMBOL_set_identifier = 523,           /* set_identifier  */
  YYSYMBOL_flowtable_spec = 524,           /* flowtable_spec  */
  YYSYMBOL_flowtableid_spec = 525,         /* flowtableid_spec  */
  YYSYMBOL_flowtable_identifier = 526,     /* flowtable_identifier  */
  YYSYMBOL_obj_spec = 527,                 /* obj_spec  */
  YYSYMBOL_objid_spec = 528,               /* objid_spec  */
  YYSYMBOL_obj_identifier = 529,           /* obj_identifier  */
  YYSYMBOL_handle_spec = 530,              /* handle_spec  */
  YYSYMBOL_position_spec = 531,            /* position_spec  */
  YYSYMBOL_index_spec = 532,               /* index_spec  */
  YYSYMBOL_rule_position = 533,            /* rule_position  */
  YYSYMBOL_ruleid_spec = 534,              /* ruleid_spec  */
  YYSYMBOL_comment_spec = 535,             /* comment_spec  */
  YYSYMBOL_ruleset_spec = 536,             /* ruleset_spec  */
  YYSYMBOL_rule = 537,                     /* rule  */
  YYSYMBOL_rule_alloc = 538,               /* rule_alloc  */
  YYSYMBOL_stmt_list = 539,                /* stmt_list  */
  YYSYMBOL_stateful_stmt_list = 540,       /* stateful_stmt_list  */
  YYSYMBOL_objref_stmt_counter = 541,      /* objref_stmt_counter  */
  YYSYMBOL_objref_stmt_limit = 542,        /* objref_stmt_limit  */
  YYSYMBOL_objref_stmt_quota = 543,        /* objref_stmt_quota  */
  YYSYMBOL_objref_stmt_synproxy = 544,     /* objref_stmt_synproxy  */
  YYSYMBOL_objref_stmt_ct = 545,           /* objref_stmt_ct  */
  YYSYMBOL_objref_stmt = 546,              /* objref_stmt  */
  YYSYMBOL_stateful_stmt = 547,            /* stateful_stmt  */
  YYSYMBOL_stmt = 548,                     /* stmt  */
  YYSYMBOL_xt_stmt = 549,                  /* xt_stmt  */
  YYSYMBOL_chain_stmt_type = 550,          /* chain_stmt_type  */
  YYSYMBOL_chain_stmt = 551,               /* chain_stmt  */
  YYSYMBOL_verdict_stmt = 552,             /* verdict_stmt  */
  YYSYMBOL_verdict_map_stmt = 553,         /* verdict_map_stmt  */
  YYSYMBOL_verdict_map_expr = 554,         /* verdict_map_expr  */
  YYSYMBOL_verdict_map_list_expr = 555,    /* verdict_map_list_expr  */
  YYSYMBOL_verdict_map_list_member_expr = 556, /* verdict_map_list_member_expr  */
  YYSYMBOL_connlimit_stmt = 557,           /* connlimit_stmt  */
  YYSYMBOL_counter_stmt = 558,             /* counter_stmt  */
  YYSYMBOL_counter_stmt_alloc = 559,       /* counter_stmt_alloc  */
  YYSYMBOL_counter_args = 560,             /* counter_args  */
  YYSYMBOL_counter_arg = 561,              /* counter_arg  */
  YYSYMBOL_last_stmt = 562,                /* last_stmt  */
  YYSYMBOL_log_stmt = 563,                 /* log_stmt  */
  YYSYMBOL_log_stmt_alloc = 564,           /* log_stmt_alloc  */
  YYSYMBOL_log_args = 565,                 /* log_args  */
  YYSYMBOL_log_arg = 566,                  /* log_arg  */
  YYSYMBOL_level_type = 567,               /* level_type  */
  YYSYMBOL_log_flags = 568,                /* log_flags  */
  YYSYMBOL_log_flags_tcp = 569,            /* log_flags_tcp  */
  YYSYMBOL_log_flag_tcp = 570,             /* log_flag_tcp  */
  YYSYMBOL_limit_stmt = 571,               /* limit_stmt  */
  YYSYMBOL_quota_mode = 572,               /* quota_mode  */
  YYSYMBOL_quota_unit = 573,               /* quota_unit  */
  YYSYMBOL_quota_used = 574,               /* quota_used  */
  YYSYMBOL_quota_stmt = 575,               /* quota_stmt  */
  YYSYMBOL_limit_mode = 576,               /* limit_mode  */
  YYSYMBOL_limit_burst_pkts = 577,         /* limit_burst_pkts  */
  YYSYMBOL_limit_rate_pkts = 578,          /* limit_rate_pkts  */
  YYSYMBOL_limit_burst_bytes = 579,        /* limit_burst_bytes  */
  YYSYMBOL_limit_rate_bytes = 580,         /* limit_rate_bytes  */
  YYSYMBOL_limit_bytes = 581,              /* limit_bytes  */
  YYSYMBOL_time_unit = 582,                /* time_unit  */
  YYSYMBOL_reject_stmt = 583,              /* reject_stmt  */
  YYSYMBOL_reject_stmt_alloc = 584,        /* reject_stmt_alloc  */
  YYSYMBOL_reject_with_expr = 585,         /* reject_with_expr  */
  YYSYMBOL_reject_opts = 586,              /* reject_opts  */
  YYSYMBOL_nat_stmt = 587,                 /* nat_stmt  */
  YYSYMBOL_nat_stmt_alloc = 588,           /* nat_stmt_alloc  */
  YYSYMBOL_tproxy_stmt = 589,              /* tproxy_stmt  */
  YYSYMBOL_synproxy_stmt = 590,            /* synproxy_stmt  */
  YYSYMBOL_synproxy_stmt_alloc = 591,      /* synproxy_stmt_alloc  */
  YYSYMBOL_synproxy_args = 592,            /* synproxy_args  */
  YYSYMBOL_synproxy_arg = 593,             /* synproxy_arg  */
  YYSYMBOL_synproxy_config = 594,          /* synproxy_config  */
  YYSYMBOL_synproxy_obj = 595,             /* synproxy_obj  */
  YYSYMBOL_synproxy_ts = 596,              /* synproxy_ts  */
  YYSYMBOL_synproxy_sack = 597,            /* synproxy_sack  */
  YYSYMBOL_primary_stmt_expr = 598,        /* primary_stmt_expr  */
  YYSYMBOL_shift_stmt_expr = 599,          /* shift_stmt_expr  */
  YYSYMBOL_and_stmt_expr = 600,            /* and_stmt_expr  */
  YYSYMBOL_exclusive_or_stmt_expr = 601,   /* exclusive_or_stmt_expr  */
  YYSYMBOL_inclusive_or_stmt_expr = 602,   /* inclusive_or_stmt_expr  */
  YYSYMBOL_basic_stmt_expr = 603,          /* basic_stmt_expr  */
  YYSYMBOL_concat_stmt_expr = 604,         /* concat_stmt_expr  */
  YYSYMBOL_map_stmt_expr_set = 605,        /* map_stmt_expr_set  */
  YYSYMBOL_map_stmt_expr = 606,            /* map_stmt_expr  */
  YYSYMBOL_prefix_stmt_expr = 607,         /* prefix_stmt_expr  */
  YYSYMBOL_range_stmt_expr = 608,          /* range_stmt_expr  */
  YYSYMBOL_multiton_stmt_expr = 609,       /* multiton_stmt_expr  */
  YYSYMBOL_stmt_expr = 610,                /* stmt_expr  */
  YYSYMBOL_nat_stmt_args = 611,            /* nat_stmt_args  */
  YYSYMBOL_masq_stmt = 612,                /* masq_stmt  */
  YYSYMBOL_masq_stmt_alloc = 613,          /* masq_stmt_alloc  */
  YYSYMBOL_masq_stmt_args = 614,           /* masq_stmt_args  */
  YYSYMBOL_redir_stmt = 615,               /* redir_stmt  */
  YYSYMBOL_redir_stmt_alloc = 616,         /* redir_stmt_alloc  */
  YYSYMBOL_redir_stmt_arg = 617,           /* redir_stmt_arg  */
  YYSYMBOL_dup_stmt = 618,                 /* dup_stmt  */
  YYSYMBOL_fwd_stmt = 619,                 /* fwd_stmt  */
  YYSYMBOL_nf_nat_flags = 620,             /* nf_nat_flags  */
  YYSYMBOL_nf_nat_flag = 621,              /* nf_nat_flag  */
  YYSYMBOL_queue_stmt = 622,               /* queue_stmt  */
  YYSYMBOL_queue_stmt_compat = 623,        /* queue_stmt_compat  */
  YYSYMBOL_queue_stmt_alloc = 624,         /* queue_stmt_alloc  */
  YYSYMBOL_queue_stmt_args = 625,          /* queue_stmt_args  */
  YYSYMBOL_queue_stmt_arg = 626,           /* queue_stmt_arg  */
  YYSYMBOL_queue_expr = 627,               /* queue_expr  */
  YYSYMBOL_queue_stmt_expr_simple = 628,   /* queue_stmt_expr_simple  */
  YYSYMBOL_queue_stmt_expr = 629,          /* queue_stmt_expr  */
  YYSYMBOL_queue_stmt_flags = 630,         /* queue_stmt_flags  */
  YYSYMBOL_queue_stmt_flag = 631,          /* queue_stmt_flag  */
  YYSYMBOL_set_elem_expr_stmt = 632,       /* set_elem_expr_stmt  */
  YYSYMBOL_set_elem_expr_stmt_alloc = 633, /* set_elem_expr_stmt_alloc  */
  YYSYMBOL_set_stmt = 634,                 /* set_stmt  */
  YYSYMBOL_set_stmt_op = 635,              /* set_stmt_op  */
  YYSYMBOL_map_stmt = 636,                 /* map_stmt  */
  YYSYMBOL_meter_stmt = 637,               /* meter_stmt  */
  YYSYMBOL_meter_stmt_alloc = 638,         /* meter_stmt_alloc  */
  YYSYMBOL_match_stmt = 639,               /* match_stmt  */
  YYSYMBOL_variable_expr = 640,            /* variable_expr  */
  YYSYMBOL_symbol_expr = 641,              /* symbol_expr  */
  YYSYMBOL_set_ref_expr = 642,             /* set_ref_expr  */
  YYSYMBOL_set_ref_symbol_expr = 643,      /* set_ref_symbol_expr  */
  YYSYMBOL_integer_expr = 644,             /* integer_expr  */
  YYSYMBOL_primary_expr = 645,             /* primary_expr  */
  YYSYMBOL_fib_expr = 646,                 /* fib_expr  */
  YYSYMBOL_fib_result = 647,               /* fib_result  */
  YYSYMBOL_fib_flag = 648,                 /* fib_flag  */
  YYSYMBOL_fib_tuple = 649,                /* fib_tuple  */
  YYSYMBOL_osf_expr = 650,                 /* osf_expr  */
  YYSYMBOL_osf_ttl = 651,                  /* osf_ttl  */
  YYSYMBOL_shift_expr = 652,               /* shift_expr  */
  YYSYMBOL_and_expr = 653,                 /* and_expr  */
  YYSYMBOL_exclusive_or_expr = 654,        /* exclusive_or_expr  */
  YYSYMBOL_inclusive_or_expr = 655,        /* inclusive_or_expr  */
  YYSYMBOL_basic_expr = 656,               /* basic_expr  */
  YYSYMBOL_concat_expr = 657,              /* concat_expr  */
  YYSYMBOL_prefix_rhs_expr = 658,          /* prefix_rhs_expr  */
  YYSYMBOL_range_rhs_expr = 659,           /* range_rhs_expr  */
  YYSYMBOL_multiton_rhs_expr = 660,        /* multiton_rhs_expr  */
  YYSYMBOL_map_expr = 661,                 /* map_expr  */
  YYSYMBOL_expr = 662,                     /* expr  */
  YYSYMBOL_set_expr = 663,                 /* set_expr  */
  YYSYMBOL_set_list_expr = 664,            /* set_list_expr  */
  YYSYMBOL_set_list_member_expr = 665,     /* set_list_member_expr  */
  YYSYMBOL_meter_key_expr = 666,           /* meter_key_expr  */
  YYSYMBOL_meter_key_expr_alloc = 667,     /* meter_key_expr_alloc  */
  YYSYMBOL_set_elem_expr = 668,            /* set_elem_expr  */
  YYSYMBOL_set_elem_key_expr = 669,        /* set_elem_key_expr  */
  YYSYMBOL_set_elem_expr_alloc = 670,      /* set_elem_expr_alloc  */
  YYSYMBOL_set_elem_options = 671,         /* set_elem_options  */
  YYSYMBOL_set_elem_time_spec = 672,       /* set_elem_time_spec  */
  YYSYMBOL_set_elem_option = 673,          /* set_elem_option  */
  YYSYMBOL_set_elem_expr_options = 674,    /* set_elem_expr_options  */
  YYSYMBOL_set_elem_stmt_list = 675,       /* set_elem_stmt_list  */
  YYSYMBOL_set_elem_stmt = 676,            /* set_elem_stmt  */
  YYSYMBOL_set_elem_expr_option = 677,     /* set_elem_expr_option  */
  YYSYMBOL_set_lhs_expr = 678,             /* set_lhs_expr  */
  YYSYMBOL_set_rhs_expr = 679,             /* set_rhs_expr  */
  YYSYMBOL_initializer_expr = 680,         /* initializer_expr  */
  YYSYMBOL_counter_config = 681,           /* counter_config  */
  YYSYMBOL_counter_obj = 682,              /* counter_obj  */
  YYSYMBOL_quota_config = 683,             /* quota_config  */
  YYSYMBOL_quota_obj = 684,                /* quota_obj  */
  YYSYMBOL_secmark_config = 685,           /* secmark_config  */
  YYSYMBOL_secmark_obj = 686,              /* secmark_obj  */
  YYSYMBOL_ct_obj_type = 687,              /* ct_obj_type  */
  YYSYMBOL_ct_cmd_type = 688,              /* ct_cmd_type  */
  YYSYMBOL_ct_l4protoname = 689,           /* ct_l4protoname  */
  YYSYMBOL_ct_helper_config = 690,         /* ct_helper_config  */
  YYSYMBOL_timeout_states = 691,           /* timeout_states  */
  YYSYMBOL_timeout_state = 692,            /* timeout_state  */
  YYSYMBOL_ct_timeout_config = 693,        /* ct_timeout_config  */
  YYSYMBOL_ct_expect_config = 694,         /* ct_expect_config  */
  YYSYMBOL_ct_obj_alloc = 695,             /* ct_obj_alloc  */
  YYSYMBOL_limit_config = 696,             /* limit_config  */
  YYSYMBOL_limit_obj = 697,                /* limit_obj  */
  YYSYMBOL_relational_expr = 698,          /* relational_expr  */
  YYSYMBOL_list_rhs_expr = 699,            /* list_rhs_expr  */
  YYSYMBOL_rhs_expr = 700,                 /* rhs_expr  */
  YYSYMBOL_shift_rhs_expr = 701,           /* shift_rhs_expr  */
  YYSYMBOL_and_rhs_expr = 702,             /* and_rhs_expr  */
  YYSYMBOL_exclusive_or_rhs_expr = 703,    /* exclusive_or_rhs_expr  */
  YYSYMBOL_inclusive_or_rhs_expr = 704,    /* inclusive_or_rhs_expr  */
  YYSYMBOL_basic_rhs_expr = 705,           /* basic_rhs_expr  */
  YYSYMBOL_concat_rhs_expr = 706,          /* concat_rhs_expr  */
  YYSYMBOL_boolean_keys = 707,             /* boolean_keys  */
  YYSYMBOL_boolean_expr = 708,             /* boolean_expr  */
  YYSYMBOL_keyword_expr = 709,             /* keyword_expr  */
  YYSYMBOL_primary_rhs_expr = 710,         /* primary_rhs_expr  */
  YYSYMBOL_relational_op = 711,            /* relational_op  */
  YYSYMBOL_verdict_expr = 712,             /* verdict_expr  */
  YYSYMBOL_chain_expr = 713,               /* chain_expr  */
  YYSYMBOL_meta_expr = 714,                /* meta_expr  */
  YYSYMBOL_meta_key = 715,                 /* meta_key  */
  YYSYMBOL_meta_key_qualified = 716,       /* meta_key_qualified  */
  YYSYMBOL_meta_key_unqualified = 717,     /* meta_key_unqualified  */
  YYSYMBOL_meta_stmt = 718,                /* meta_stmt  */
  YYSYMBOL_socket_expr = 719,              /* socket_expr  */
  YYSYMBOL_socket_key = 720,               /* socket_key  */
  YYSYMBOL_offset_opt = 721,               /* offset_opt  */
  YYSYMBOL_numgen_type = 722,              /* numgen_type  */
  YYSYMBOL_numgen_expr = 723,              /* numgen_expr  */
  YYSYMBOL_xfrm_spnum = 724,               /* xfrm_spnum  */
  YYSYMBOL_xfrm_dir = 725,                 /* xfrm_dir  */
  YYSYMBOL_xfrm_state_key = 726,           /* xfrm_state_key  */
  YYSYMBOL_xfrm_state_proto_key = 727,     /* xfrm_state_proto_key  */
  YYSYMBOL_xfrm_expr = 728,                /* xfrm_expr  */
  YYSYMBOL_hash_expr = 729,                /* hash_expr  */
  YYSYMBOL_nf_key_proto = 730,             /* nf_key_proto  */
  YYSYMBOL_rt_expr = 731,                  /* rt_expr  */
  YYSYMBOL_rt_key = 732,                   /* rt_key  */
  YYSYMBOL_ct_expr = 733,                  /* ct_expr  */
  YYSYMBOL_ct_dir = 734,                   /* ct_dir  */
  YYSYMBOL_ct_key = 735,                   /* ct_key  */
  YYSYMBOL_ct_key_dir = 736,               /* ct_key_dir  */
  YYSYMBOL_ct_key_proto_field = 737,       /* ct_key_proto_field  */
  YYSYMBOL_ct_key_dir_optional = 738,      /* ct_key_dir_optional  */
  YYSYMBOL_symbol_stmt_expr = 739,         /* symbol_stmt_expr  */
  YYSYMBOL_list_stmt_expr = 740,           /* list_stmt_expr  */
  YYSYMBOL_ct_stmt = 741,                  /* ct_stmt  */
  YYSYMBOL_payload_stmt = 742,             /* payload_stmt  */
  YYSYMBOL_payload_expr = 743,             /* payload_expr  */
  YYSYMBOL_payload_raw_len = 744,          /* payload_raw_len  */
  YYSYMBOL_payload_raw_expr = 745,         /* payload_raw_expr  */
  YYSYMBOL_payload_base_spec = 746,        /* payload_base_spec  */
  YYSYMBOL_eth_hdr_expr = 747,             /* eth_hdr_expr  */
  YYSYMBOL_eth_hdr_field = 748,            /* eth_hdr_field  */
  YYSYMBOL_vlan_hdr_expr = 749,            /* vlan_hdr_expr  */
  YYSYMBOL_vlan_hdr_field = 750,           /* vlan_hdr_field  */
  YYSYMBOL_arp_hdr_expr = 751,             /* arp_hdr_expr  */
  YYSYMBOL_arp_hdr_field = 752,            /* arp_hdr_field  */
  YYSYMBOL_ip_hdr_expr = 753,              /* ip_hdr_expr  */
  YYSYMBOL_ip_hdr_field = 754,             /* ip_hdr_field  */
  YYSYMBOL_ip_option_type = 755,           /* ip_option_type  */
  YYSYMBOL_ip_option_field = 756,          /* ip_option_field  */
  YYSYMBOL_icmp_hdr_expr = 757,            /* icmp_hdr_expr  */
  YYSYMBOL_icmp_hdr_field = 758,           /* icmp_hdr_field  */
  YYSYMBOL_igmp_hdr_expr = 759,            /* igmp_hdr_expr  */
  YYSYMBOL_igmp_hdr_field = 760,           /* igmp_hdr_field  */
  YYSYMBOL_ip6_hdr_expr = 761,             /* ip6_hdr_expr  */
  YYSYMBOL_ip6_hdr_field = 762,            /* ip6_hdr_field  */
  YYSYMBOL_icmp6_hdr_expr = 763,           /* icmp6_hdr_expr  */
  YYSYMBOL_icmp6_hdr_field = 764,          /* icmp6_hdr_field  */
  YYSYMBOL_auth_hdr_expr = 765,            /* auth_hdr_expr  */
  YYSYMBOL_auth_hdr_field = 766,           /* auth_hdr_field  */
  YYSYMBOL_esp_hdr_expr = 767,             /* esp_hdr_expr  */
  YYSYMBOL_esp_hdr_field = 768,            /* esp_hdr_field  */
  YYSYMBOL_comp_hdr_expr = 769,            /* comp_hdr_expr  */
  YYSYMBOL_comp_hdr_field = 770,           /* comp_hdr_field  */
  YYSYMBOL_udp_hdr_expr = 771,             /* udp_hdr_expr  */
  YYSYMBOL_udp_hdr_field = 772,            /* udp_hdr_field  */
  YYSYMBOL_udplite_hdr_expr = 773,         /* udplite_hdr_expr  */
  YYSYMBOL_udplite_hdr_field = 774,        /* udplite_hdr_field  */
  YYSYMBOL_tcp_hdr_expr = 775,             /* tcp_hdr_expr  */
  YYSYMBOL_inner_inet_expr = 776,          /* inner_inet_expr  */
  YYSYMBOL_inner_eth_expr = 777,           /* inner_eth_expr  */
  YYSYMBOL_inner_expr = 778,               /* inner_expr  */
  YYSYMBOL_vxlan_hdr_expr = 779,           /* vxlan_hdr_expr  */
  YYSYMBOL_vxlan_hdr_field = 780,          /* vxlan_hdr_field  */
  YYSYMBOL_geneve_hdr_expr = 781,          /* geneve_hdr_expr  */
  YYSYMBOL_geneve_hdr_field = 782,         /* geneve_hdr_field  */
  YYSYMBOL_gre_hdr_expr = 783,             /* gre_hdr_expr  */
  YYSYMBOL_gre_hdr_field = 784,            /* gre_hdr_field  */
  YYSYMBOL_gretap_hdr_expr = 785,          /* gretap_hdr_expr  */
  YYSYMBOL_optstrip_stmt = 786,            /* optstrip_stmt  */
  YYSYMBOL_tcp_hdr_field = 787,            /* tcp_hdr_field  */
  YYSYMBOL_tcp_hdr_option_kind_and_field = 788, /* tcp_hdr_option_kind_and_field  */
  YYSYMBOL_tcp_hdr_option_sack = 789,      /* tcp_hdr_option_sack  */
  YYSYMBOL_tcp_hdr_option_type = 790,      /* tcp_hdr_option_type  */
  YYSYMBOL_tcpopt_field_sack = 791,        /* tcpopt_field_sack  */
  YYSYMBOL_tcpopt_field_window = 792,      /* tcpopt_field_window  */
  YYSYMBOL_tcpopt_field_tsopt = 793,       /* tcpopt_field_tsopt  */
  YYSYMBOL_tcpopt_field_maxseg = 794,      /* tcpopt_field_maxseg  */
  YYSYMBOL_tcpopt_field_mptcp = 795,       /* tcpopt_field_mptcp  */
  YYSYMBOL_dccp_hdr_expr = 796,            /* dccp_hdr_expr  */
  YYSYMBOL_dccp_hdr_field = 797,           /* dccp_hdr_field  */
  YYSYMBOL_sctp_chunk_type = 798,          /* sctp_chunk_type  */
  YYSYMBOL_sctp_chunk_common_field = 799,  /* sctp_chunk_common_field  */
  YYSYMBOL_sctp_chunk_data_field = 800,    /* sctp_chunk_data_field  */
  YYSYMBOL_sctp_chunk_init_field = 801,    /* sctp_chunk_init_field  */
  YYSYMBOL_sctp_chunk_sack_field = 802,    /* sctp_chunk_sack_field  */
  YYSYMBOL_sctp_chunk_alloc = 803,         /* sctp_chunk_alloc  */
  YYSYMBOL_sctp_hdr_expr = 804,            /* sctp_hdr_expr  */
  YYSYMBOL_sctp_hdr_field = 805,           /* sctp_hdr_field  */
  YYSYMBOL_th_hdr_expr = 806,              /* th_hdr_expr  */
  YYSYMBOL_th_hdr_field = 807,             /* th_hdr_field  */
  YYSYMBOL_exthdr_expr = 808,              /* exthdr_expr  */
  YYSYMBOL_hbh_hdr_expr = 809,             /* hbh_hdr_expr  */
  YYSYMBOL_hbh_hdr_field = 810,            /* hbh_hdr_field  */
  YYSYMBOL_rt_hdr_expr = 811,              /* rt_hdr_expr  */
  YYSYMBOL_rt_hdr_field = 812,             /* rt_hdr_field  */
  YYSYMBOL_rt0_hdr_expr = 813,             /* rt0_hdr_expr  */
  YYSYMBOL_rt0_hdr_field = 814,            /* rt0_hdr_field  */
  YYSYMBOL_rt2_hdr_expr = 815,             /* rt2_hdr_expr  */
  YYSYMBOL_rt2_hdr_field = 816,            /* rt2_hdr_field  */
  YYSYMBOL_rt4_hdr_expr = 817,             /* rt4_hdr_expr  */
  YYSYMBOL_rt4_hdr_field = 818,            /* rt4_hdr_field  */
  YYSYMBOL_frag_hdr_expr = 819,            /* frag_hdr_expr  */
  YYSYMBOL_frag_hdr_field = 820,           /* frag_hdr_field  */
  YYSYMBOL_dst_hdr_expr = 821,             /* dst_hdr_expr  */
  YYSYMBOL_dst_hdr_field = 822,            /* dst_hdr_field  */
  YYSYMBOL_mh_hdr_expr = 823,              /* mh_hdr_expr  */
  YYSYMBOL_mh_hdr_field = 824,             /* mh_hdr_field  */
  YYSYMBOL_exthdr_exists_expr = 825,       /* exthdr_exists_expr  */
  YYSYMBOL_exthdr_key = 826                /* exthdr_key  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   8910

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  372
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  455
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1382
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  2369

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   617


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   369,     2,     2,     2,
     366,   367,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   363,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   370,     2,   371,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   364,   368,   365,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,  1010,  1010,  1011,  1020,  1021,  1024,  1025,  1028,  1029,
    1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,
    1040,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,
    1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,  1059,
    1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,  1069,
    1070,  1071,  1072,  1073,  1074,  1075,  1076,  1077,  1078,  1080,
    1081,  1082,  1084,  1092,  1107,  1114,  1126,  1134,  1135,  1136,
    1137,  1157,  1158,  1159,  1160,  1161,  1162,  1163,  1164,  1165,
    1166,  1167,  1168,  1169,  1170,  1171,  1172,  1175,  1179,  1186,
    1190,  1198,  1202,  1206,  1213,  1220,  1224,  1231,  1240,  1244,
    1248,  1252,  1256,  1260,  1264,  1268,  1272,  1276,  1280,  1284,
    1288,  1294,  1300,  1304,  1311,  1315,  1323,  1330,  1337,  1341,
    1348,  1357,  1361,  1365,  1369,  1373,  1377,  1381,  1385,  1391,
    1397,  1398,  1401,  1402,  1405,  1406,  1409,  1410,  1413,  1417,
    1421,  1429,  1433,  1437,  1441,  1445,  1449,  1453,  1460,  1464,
    1468,  1474,  1478,  1482,  1488,  1492,  1496,  1500,  1504,  1508,
    1512,  1516,  1520,  1527,  1531,  1535,  1541,  1545,  1549,  1556,
    1562,  1566,  1570,  1574,  1578,  1582,  1586,  1590,  1594,  1598,
    1602,  1606,  1610,  1614,  1618,  1622,  1626,  1630,  1634,  1638,
    1642,  1646,  1650,  1654,  1658,  1662,  1666,  1670,  1674,  1678,
    1682,  1686,  1690,  1694,  1700,  1706,  1710,  1720,  1724,  1728,
    1733,  1737,  1741,  1745,  1750,  1754,  1758,  1762,  1767,  1771,
    1776,  1780,  1784,  1788,  1794,  1798,  1802,  1806,  1810,  1814,
    1818,  1824,  1831,  1837,  1845,  1851,  1859,  1868,  1869,  1872,
    1873,  1874,  1875,  1876,  1877,  1878,  1879,  1882,  1883,  1886,
    1887,  1888,  1891,  1900,  1910,  1914,  1924,  1925,  1930,  1942,
    1943,  1944,  1945,  1946,  1957,  1967,  1978,  1988,  1999,  2010,
    2019,  2028,  2037,  2048,  2059,  2073,  2083,  2084,  2085,  2086,
    2087,  2088,  2089,  2094,  2103,  2113,  2114,  2115,  2122,  2143,
    2154,  2159,  2165,  2176,  2189,  2194,  2195,  2198,  2199,  2200,
    2201,  2211,  2216,  2221,  2226,  2232,  2241,  2246,  2247,  2258,
    2259,  2262,  2266,  2269,  2270,  2271,  2272,  2276,  2281,  2282,
    2285,  2286,  2287,  2288,  2289,  2292,  2293,  2296,  2297,  2300,
    2301,  2302,  2303,  2308,  2313,  2330,  2353,  2367,  2376,  2381,
    2387,  2392,  2401,  2404,  2408,  2414,  2415,  2419,  2424,  2425,
    2426,  2427,  2441,  2445,  2449,  2455,  2460,  2467,  2472,  2477,
    2480,  2489,  2498,  2505,  2518,  2525,  2526,  2538,  2543,  2544,
    2545,  2546,  2550,  2560,  2561,  2562,  2563,  2567,  2577,  2578,
    2579,  2580,  2584,  2595,  2600,  2601,  2602,  2606,  2616,  2617,
    2618,  2619,  2623,  2633,  2634,  2635,  2636,  2640,  2650,  2651,
    2652,  2653,  2657,  2667,  2668,  2669,  2670,  2674,  2684,  2685,
    2686,  2687,  2688,  2691,  2727,  2734,  2738,  2741,  2751,  2758,
    2769,  2782,  2797,  2798,  2801,  2812,  2818,  2822,  2825,  2831,
    2844,  2849,  2858,  2859,  2862,  2863,  2866,  2867,  2868,  2871,
    2887,  2888,  2891,  2892,  2895,  2896,  2897,  2898,  2899,  2900,
    2903,  2912,  2921,  2929,  2937,  2945,  2953,  2961,  2969,  2977,
    2985,  2993,  3001,  3009,  3017,  3025,  3033,  3041,  3045,  3050,
    3058,  3065,  3072,  3086,  3090,  3097,  3101,  3107,  3119,  3125,
    3132,  3138,  3145,  3153,  3161,  3169,  3177,  3184,  3192,  3193,
    3194,  3195,  3196,  3199,  3200,  3201,  3202,  3203,  3206,  3207,
    3208,  3209,  3210,  3211,  3212,  3213,  3214,  3215,  3216,  3217,
    3218,  3219,  3220,  3221,  3222,  3223,  3224,  3225,  3226,  3227,
    3228,  3231,  3242,  3243,  3246,  3255,  3259,  3265,  3271,  3276,
    3279,  3284,  3289,  3292,  3298,  3303,  3311,  3312,  3314,  3320,
    3324,  3327,  3332,  3339,  3343,  3347,  3355,  3356,  3359,  3365,
    3369,  3372,  3389,  3394,  3399,  3404,  3409,  3415,  3445,  3449,
    3453,  3457,  3461,  3467,  3471,  3474,  3478,  3484,  3498,  3509,
    3510,  3511,  3514,  3515,  3518,  3519,  3534,  3552,  3553,  3554,
    3557,  3558,  3561,  3568,  3569,  3572,  3586,  3593,  3594,  3609,
    3610,  3611,  3612,  3613,  3616,  3619,  3625,  3631,  3635,  3639,
    3646,  3653,  3660,  3667,  3673,  3679,  3685,  3688,  3689,  3692,
    3698,  3704,  3710,  3717,  3724,  3732,  3733,  3736,  3742,  3746,
    3749,  3754,  3759,  3763,  3769,  3785,  3804,  3810,  3811,  3817,
    3818,  3824,  3825,  3826,  3827,  3828,  3829,  3830,  3831,  3832,
    3833,  3834,  3835,  3836,  3839,  3840,  3844,  3850,  3851,  3857,
    3858,  3864,  3865,  3871,  3874,  3875,  3886,  3887,  3890,  3894,
    3897,  3903,  3909,  3910,  3913,  3914,  3915,  3918,  3922,  3926,
    3931,  3936,  3941,  3947,  3951,  3955,  3959,  3965,  3970,  3974,
    3982,  3991,  3992,  3995,  3998,  4002,  4007,  4013,  4014,  4017,
    4020,  4024,  4028,  4032,  4037,  4044,  4049,  4057,  4062,  4071,
    4072,  4078,  4079,  4080,  4083,  4084,  4088,  4092,  4098,  4099,
    4102,  4108,  4112,  4115,  4120,  4126,  4127,  4130,  4131,  4132,
    4138,  4139,  4140,  4141,  4144,  4145,  4151,  4152,  4155,  4156,
    4159,  4165,  4172,  4179,  4190,  4191,  4192,  4195,  4203,  4215,
    4218,  4227,  4238,  4244,  4270,  4271,  4280,  4281,  4284,  4293,
    4304,  4305,  4306,  4307,  4308,  4309,  4310,  4311,  4312,  4313,
    4314,  4315,  4316,  4317,  4318,  4321,  4344,  4345,  4346,  4349,
    4350,  4351,  4352,  4353,  4356,  4360,  4363,  4367,  4374,  4377,
    4393,  4394,  4398,  4404,  4405,  4411,  4412,  4418,  4419,  4425,
    4428,  4429,  4440,  4446,  4452,  4453,  4456,  4462,  4463,  4464,
    4467,  4474,  4479,  4484,  4487,  4491,  4495,  4501,  4502,  4509,
    4515,  4516,  4517,  4525,  4526,  4529,  4535,  4541,  4545,  4548,
    4569,  4573,  4577,  4587,  4591,  4594,  4600,  4607,  4611,  4617,
    4631,  4640,  4645,  4651,  4667,  4671,  4679,  4683,  4687,  4697,
    4700,  4701,  4704,  4705,  4706,  4707,  4718,  4729,  4735,  4756,
    4762,  4779,  4785,  4786,  4787,  4790,  4791,  4792,  4795,  4796,
    4799,  4822,  4828,  4834,  4841,  4854,  4862,  4870,  4876,  4880,
    4884,  4888,  4892,  4899,  4904,  4915,  4929,  4935,  4939,  4943,
    4947,  4951,  4955,  4959,  4963,  4969,  4975,  4983,  4984,  4985,
    4988,  4989,  4993,  4999,  5000,  5006,  5007,  5013,  5014,  5020,
    5023,  5024,  5025,  5034,  5045,  5046,  5049,  5057,  5058,  5059,
    5060,  5061,  5062,  5063,  5064,  5065,  5066,  5067,  5068,  5069,
    5070,  5073,  5074,  5075,  5076,  5077,  5084,  5091,  5098,  5105,
    5112,  5119,  5126,  5133,  5140,  5147,  5154,  5161,  5168,  5171,
    5172,  5173,  5174,  5175,  5176,  5177,  5180,  5184,  5188,  5192,
    5196,  5200,  5206,  5207,  5217,  5221,  5225,  5241,  5242,  5245,
    5246,  5247,  5248,  5249,  5252,  5253,  5254,  5255,  5256,  5257,
    5258,  5259,  5260,  5261,  5262,  5263,  5264,  5265,  5266,  5267,
    5268,  5269,  5270,  5271,  5272,  5273,  5274,  5275,  5278,  5298,
    5302,  5317,  5321,  5325,  5331,  5335,  5341,  5342,  5343,  5346,
    5347,  5350,  5351,  5354,  5360,  5361,  5364,  5365,  5368,  5369,
    5372,  5373,  5376,  5384,  5411,  5416,  5421,  5427,  5428,  5431,
    5435,  5455,  5456,  5457,  5458,  5461,  5465,  5469,  5475,  5476,
    5479,  5480,  5481,  5482,  5483,  5484,  5485,  5486,  5487,  5488,
    5489,  5490,  5491,  5492,  5493,  5494,  5495,  5498,  5499,  5500,
    5501,  5502,  5503,  5504,  5507,  5508,  5509,  5510,  5513,  5514,
    5515,  5516,  5519,  5520,  5523,  5529,  5537,  5550,  5556,  5565,
    5566,  5567,  5568,  5569,  5570,  5571,  5572,  5573,  5574,  5575,
    5576,  5577,  5578,  5579,  5580,  5581,  5582,  5583,  5584,  5585,
    5586,  5589,  5607,  5616,  5617,  5618,  5619,  5632,  5638,  5639,
    5640,  5643,  5649,  5650,  5651,  5652,  5653,  5656,  5662,  5663,
    5664,  5665,  5666,  5667,  5668,  5669,  5670,  5673,  5677,  5685,
    5692,  5693,  5694,  5695,  5696,  5697,  5698,  5699,  5700,  5701,
    5702,  5703,  5706,  5707,  5708,  5709,  5712,  5713,  5714,  5715,
    5716,  5719,  5725,  5726,  5727,  5728,  5729,  5730,  5731,  5734,
    5740,  5741,  5742,  5743,  5746,  5752,  5753,  5754,  5755,  5756,
    5757,  5758,  5759,  5760,  5762,  5768,  5769,  5770,  5771,  5772,
    5773,  5774,  5775,  5776,  5777,  5780,  5786,  5787,  5788,  5789,
    5790,  5793,  5799,  5800,  5803,  5809,  5810,  5811,  5814,  5820,
    5821,  5822,  5823,  5826,  5832,  5833,  5834,  5835,  5838,  5842,
    5847,  5855,  5862,  5863,  5864,  5865,  5866,  5867,  5868,  5869,
    5870,  5871,  5872,  5873,  5874,  5875,  5878,  5879,  5880,  5883,
    5884,  5887,  5895,  5903,  5904,  5907,  5915,  5923,  5924,  5927,
    5931,  5938,  5939,  5940,  5943,  5950,  5957,  5958,  5959,  5960,
    5961,  5962,  5963,  5964,  5965,  5966,  5969,  5974,  5979,  5984,
    5989,  5994,  6001,  6002,  6003,  6004,  6005,  6008,  6009,  6010,
    6011,  6012,  6013,  6014,  6015,  6016,  6017,  6018,  6019,  6028,
    6029,  6032,  6035,  6036,  6039,  6042,  6045,  6049,  6060,  6061,
    6062,  6065,  6066,  6067,  6068,  6069,  6070,  6071,  6072,  6073,
    6074,  6075,  6076,  6077,  6078,  6079,  6080,  6081,  6082,  6085,
    6086,  6087,  6090,  6091,  6092,  6093,  6096,  6097,  6098,  6099,
    6100,  6103,  6104,  6105,  6106,  6109,  6114,  6118,  6122,  6126,
    6130,  6134,  6139,  6144,  6149,  6154,  6159,  6166,  6170,  6176,
    6177,  6178,  6179,  6182,  6190,  6191,  6194,  6195,  6196,  6197,
    6198,  6199,  6200,  6201,  6204,  6210,  6211,  6214,  6220,  6221,
    6222,  6223,  6226,  6232,  6238,  6244,  6247,  6253,  6254,  6255,
    6256,  6262,  6268,  6269,  6270,  6271,  6272,  6273,  6276,  6282,
    6283,  6286,  6292,  6293,  6294,  6295,  6296,  6299,  6313,  6314,
    6315,  6316,  6317
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "\"junk\"",
  "\"newline\"", "\"colon\"", "\"semicolon\"", "\"comma\"", "\".\"",
  "\"==\"", "\"!=\"", "\"<\"", "\">\"", "\">=\"", "\"<=\"", "\"<<\"",
  "\">>\"", "\"&\"", "\"^\"", "\"!\"", "\"/\"", "\"*\"", "\"-\"", "\"@\"",
  "\"vmap\"", "\"+\"", "\"include\"", "\"define\"", "\"redefine\"",
  "\"undefine\"", "\"fib\"", "\"socket\"", "\"transparent\"",
  "\"wildcard\"", "\"cgroupv2\"", "\"tproxy\"", "\"osf\"", "\"synproxy\"",
  "\"mss\"", "\"wscale\"", "\"typeof\"", "\"hook\"", "\"hooks\"",
  "\"device\"", "\"devices\"", "\"table\"", "\"tables\"", "\"chain\"",
  "\"chains\"", "\"rule\"", "\"rules\"", "\"sets\"", "\"set\"",
  "\"element\"", "\"map\"", "\"maps\"", "\"flowtable\"", "\"handle\"",
  "\"ruleset\"", "\"trace\"", "\"inet\"", "\"netdev\"", "\"add\"",
  "\"update\"", "\"replace\"", "\"create\"", "\"insert\"", "\"delete\"",
  "\"get\"", "\"list\"", "\"reset\"", "\"flush\"", "\"rename\"",
  "\"describe\"", "\"import\"", "\"export\"", "\"destroy\"", "\"monitor\"",
  "\"all\"", "\"accept\"", "\"drop\"", "\"continue\"", "\"jump\"",
  "\"goto\"", "\"return\"", "\"to\"", "\"constant\"", "\"interval\"",
  "\"dynamic\"", "\"auto-merge\"", "\"timeout\"", "\"gc-interval\"",
  "\"elements\"", "\"expires\"", "\"policy\"", "\"memory\"",
  "\"performance\"", "\"size\"", "\"flow\"", "\"offload\"", "\"meter\"",
  "\"meters\"", "\"flowtables\"", "\"number\"", "\"string\"",
  "\"quoted string\"", "\"string with a trailing asterisk\"", "\"ll\"",
  "\"nh\"", "\"th\"", "\"bridge\"", "\"ether\"", "\"saddr\"", "\"daddr\"",
  "\"type\"", "\"vlan\"", "\"id\"", "\"cfi\"", "\"dei\"", "\"pcp\"",
  "\"arp\"", "\"htype\"", "\"ptype\"", "\"hlen\"", "\"plen\"",
  "\"operation\"", "\"ip\"", "\"version\"", "\"hdrlength\"", "\"dscp\"",
  "\"ecn\"", "\"length\"", "\"frag-off\"", "\"ttl\"", "\"protocol\"",
  "\"checksum\"", "\"ptr\"", "\"value\"", "\"lsrr\"", "\"rr\"", "\"ssrr\"",
  "\"ra\"", "\"icmp\"", "\"code\"", "\"seq\"", "\"gateway\"", "\"mtu\"",
  "\"igmp\"", "\"mrt\"", "\"options\"", "\"ip6\"", "\"priority\"",
  "\"flowlabel\"", "\"nexthdr\"", "\"hoplimit\"", "\"icmpv6\"",
  "\"param-problem\"", "\"max-delay\"", "\"taddr\"", "\"ah\"",
  "\"reserved\"", "\"spi\"", "\"esp\"", "\"comp\"", "\"flags\"", "\"cpi\"",
  "\"port\"", "\"udp\"", "\"sport\"", "\"dport\"", "\"udplite\"",
  "\"csumcov\"", "\"tcp\"", "\"ackseq\"", "\"doff\"", "\"window\"",
  "\"urgptr\"", "\"option\"", "\"echo\"", "\"eol\"", "\"mptcp\"",
  "\"nop\"", "\"sack\"", "\"sack0\"", "\"sack1\"", "\"sack2\"",
  "\"sack3\"", "\"sack-permitted\"", "\"fastopen\"", "\"md5sig\"",
  "\"timestamp\"", "\"count\"", "\"left\"", "\"right\"", "\"tsval\"",
  "\"tsecr\"", "\"subtype\"", "\"dccp\"", "\"vxlan\"", "\"vni\"",
  "\"gre\"", "\"gretap\"", "\"geneve\"", "\"sctp\"", "\"chunk\"",
  "\"data\"", "\"init\"", "\"init-ack\"", "\"heartbeat\"",
  "\"heartbeat-ack\"", "\"abort\"", "\"shutdown\"", "\"shutdown-ack\"",
  "\"error\"", "\"cookie-echo\"", "\"cookie-ack\"", "\"ecne\"", "\"cwr\"",
  "\"shutdown-complete\"", "\"asconf-ack\"", "\"forward-tsn\"",
  "\"asconf\"", "\"tsn\"", "\"stream\"", "\"ssn\"", "\"ppid\"",
  "\"init-tag\"", "\"a-rwnd\"", "\"num-outbound-streams\"",
  "\"num-inbound-streams\"", "\"initial-tsn\"", "\"cum-tsn-ack\"",
  "\"num-gap-ack-blocks\"", "\"num-dup-tsns\"", "\"lowest-tsn\"",
  "\"seqno\"", "\"new-cum-tsn\"", "\"vtag\"", "\"rt\"", "\"rt0\"",
  "\"rt2\"", "\"srh\"", "\"seg-left\"", "\"addr\"", "\"last-entry\"",
  "\"tag\"", "\"sid\"", "\"hbh\"", "\"frag\"", "\"reserved2\"",
  "\"more-fragments\"", "\"dst\"", "\"mh\"", "\"meta\"", "\"mark\"",
  "\"iif\"", "\"iifname\"", "\"iiftype\"", "\"oif\"", "\"oifname\"",
  "\"oiftype\"", "\"skuid\"", "\"skgid\"", "\"nftrace\"", "\"rtclassid\"",
  "\"ibriport\"", "\"obriport\"", "\"ibrname\"", "\"obrname\"",
  "\"pkttype\"", "\"cpu\"", "\"iifgroup\"", "\"oifgroup\"", "\"cgroup\"",
  "\"time\"", "\"classid\"", "\"nexthop\"", "\"ct\"", "\"l3proto\"",
  "\"proto-src\"", "\"proto-dst\"", "\"zone\"", "\"direction\"",
  "\"event\"", "\"expectation\"", "\"expiration\"", "\"helper\"",
  "\"label\"", "\"state\"", "\"status\"", "\"original\"", "\"reply\"",
  "\"counter\"", "\"name\"", "\"packets\"", "\"bytes\"", "\"avgpkt\"",
  "\"last\"", "\"never\"", "\"counters\"", "\"quotas\"", "\"limits\"",
  "\"synproxys\"", "\"helpers\"", "\"log\"", "\"prefix\"", "\"group\"",
  "\"snaplen\"", "\"queue-threshold\"", "\"level\"", "\"limit\"",
  "\"rate\"", "\"burst\"", "\"over\"", "\"until\"", "\"quota\"",
  "\"used\"", "\"secmark\"", "\"secmarks\"", "\"second\"", "\"minute\"",
  "\"hour\"", "\"day\"", "\"week\"", "\"reject\"", "\"with\"", "\"icmpx\"",
  "\"snat\"", "\"dnat\"", "\"masquerade\"", "\"redirect\"", "\"random\"",
  "\"fully-random\"", "\"persistent\"", "\"queue\"", "\"num\"",
  "\"bypass\"", "\"fanout\"", "\"dup\"", "\"fwd\"", "\"numgen\"",
  "\"inc\"", "\"mod\"", "\"offset\"", "\"jhash\"", "\"symhash\"",
  "\"seed\"", "\"position\"", "\"index\"", "\"comment\"", "\"xml\"",
  "\"json\"", "\"vm\"", "\"notrack\"", "\"exists\"", "\"missing\"",
  "\"exthdr\"", "\"ipsec\"", "\"reqid\"", "\"spnum\"", "\"in\"", "\"out\"",
  "\"xt\"", "'='", "'{'", "'}'", "'('", "')'", "'|'", "'$'", "'['", "']'",
  "$accept", "input", "stmt_separator", "opt_newline", "close_scope_ah",
  "close_scope_arp", "close_scope_at", "close_scope_comp",
  "close_scope_ct", "close_scope_counter", "close_scope_last",
  "close_scope_dccp", "close_scope_destroy", "close_scope_dst",
  "close_scope_dup", "close_scope_esp", "close_scope_eth",
  "close_scope_export", "close_scope_fib", "close_scope_frag",
  "close_scope_fwd", "close_scope_gre", "close_scope_hash",
  "close_scope_hbh", "close_scope_ip", "close_scope_ip6",
  "close_scope_vlan", "close_scope_icmp", "close_scope_igmp",
  "close_scope_import", "close_scope_ipsec", "close_scope_list",
  "close_scope_limit", "close_scope_meta", "close_scope_mh",
  "close_scope_monitor", "close_scope_nat", "close_scope_numgen",
  "close_scope_osf", "close_scope_policy", "close_scope_quota",
  "close_scope_queue", "close_scope_reject", "close_scope_reset",
  "close_scope_rt", "close_scope_sctp", "close_scope_sctp_chunk",
  "close_scope_secmark", "close_scope_socket", "close_scope_tcp",
  "close_scope_tproxy", "close_scope_type", "close_scope_th",
  "close_scope_udp", "close_scope_udplite", "close_scope_log",
  "close_scope_synproxy", "close_scope_xt", "common_block", "line",
  "base_cmd", "add_cmd", "replace_cmd", "create_cmd", "insert_cmd",
  "table_or_id_spec", "chain_or_id_spec", "set_or_id_spec",
  "obj_or_id_spec", "delete_cmd", "destroy_cmd", "get_cmd", "list_cmd",
  "basehook_device_name", "basehook_spec", "reset_cmd", "flush_cmd",
  "rename_cmd", "import_cmd", "export_cmd", "monitor_cmd", "monitor_event",
  "monitor_object", "monitor_format", "markup_format", "describe_cmd",
  "table_block_alloc", "table_options", "table_flags", "table_flag",
  "table_block", "chain_block_alloc", "chain_block", "subchain_block",
  "typeof_verdict_expr", "typeof_data_expr", "typeof_expr",
  "set_block_alloc", "typeof_key_expr", "set_block", "set_block_expr",
  "set_flag_list", "set_flag", "map_block_alloc", "ct_obj_type_map",
  "map_block_obj_type", "map_block_obj_typeof", "map_block_data_interval",
  "map_block", "set_mechanism", "set_policy_spec", "flowtable_block_alloc",
  "flowtable_block", "flowtable_expr", "flowtable_list_expr",
  "flowtable_expr_member", "data_type_atom_expr", "data_type_expr",
  "obj_block_alloc", "counter_block", "quota_block", "ct_helper_block",
  "ct_timeout_block", "ct_expect_block", "limit_block", "secmark_block",
  "synproxy_block", "type_identifier", "hook_spec", "prio_spec",
  "extended_prio_name", "extended_prio_spec", "int_num", "dev_spec",
  "flags_spec", "policy_spec", "policy_expr", "chain_policy", "identifier",
  "string", "time_spec", "time_spec_or_num_s", "family_spec",
  "family_spec_explicit", "table_spec", "tableid_spec", "chain_spec",
  "chainid_spec", "chain_identifier", "set_spec", "setid_spec",
  "set_identifier", "flowtable_spec", "flowtableid_spec",
  "flowtable_identifier", "obj_spec", "objid_spec", "obj_identifier",
  "handle_spec", "position_spec", "index_spec", "rule_position",
  "ruleid_spec", "comment_spec", "ruleset_spec", "rule", "rule_alloc",
  "stmt_list", "stateful_stmt_list", "objref_stmt_counter",
  "objref_stmt_limit", "objref_stmt_quota", "objref_stmt_synproxy",
  "objref_stmt_ct", "objref_stmt", "stateful_stmt", "stmt", "xt_stmt",
  "chain_stmt_type", "chain_stmt", "verdict_stmt", "verdict_map_stmt",
  "verdict_map_expr", "verdict_map_list_expr",
  "verdict_map_list_member_expr", "connlimit_stmt", "counter_stmt",
  "counter_stmt_alloc", "counter_args", "counter_arg", "last_stmt",
  "log_stmt", "log_stmt_alloc", "log_args", "log_arg", "level_type",
  "log_flags", "log_flags_tcp", "log_flag_tcp", "limit_stmt", "quota_mode",
  "quota_unit", "quota_used", "quota_stmt", "limit_mode",
  "limit_burst_pkts", "limit_rate_pkts", "limit_burst_bytes",
  "limit_rate_bytes", "limit_bytes", "time_unit", "reject_stmt",
  "reject_stmt_alloc", "reject_with_expr", "reject_opts", "nat_stmt",
  "nat_stmt_alloc", "tproxy_stmt", "synproxy_stmt", "synproxy_stmt_alloc",
  "synproxy_args", "synproxy_arg", "synproxy_config", "synproxy_obj",
  "synproxy_ts", "synproxy_sack", "primary_stmt_expr", "shift_stmt_expr",
  "and_stmt_expr", "exclusive_or_stmt_expr", "inclusive_or_stmt_expr",
  "basic_stmt_expr", "concat_stmt_expr", "map_stmt_expr_set",
  "map_stmt_expr", "prefix_stmt_expr", "range_stmt_expr",
  "multiton_stmt_expr", "stmt_expr", "nat_stmt_args", "masq_stmt",
  "masq_stmt_alloc", "masq_stmt_args", "redir_stmt", "redir_stmt_alloc",
  "redir_stmt_arg", "dup_stmt", "fwd_stmt", "nf_nat_flags", "nf_nat_flag",
  "queue_stmt", "queue_stmt_compat", "queue_stmt_alloc", "queue_stmt_args",
  "queue_stmt_arg", "queue_expr", "queue_stmt_expr_simple",
  "queue_stmt_expr", "queue_stmt_flags", "queue_stmt_flag",
  "set_elem_expr_stmt", "set_elem_expr_stmt_alloc", "set_stmt",
  "set_stmt_op", "map_stmt", "meter_stmt", "meter_stmt_alloc",
  "match_stmt", "variable_expr", "symbol_expr", "set_ref_expr",
  "set_ref_symbol_expr", "integer_expr", "primary_expr", "fib_expr",
  "fib_result", "fib_flag", "fib_tuple", "osf_expr", "osf_ttl",
  "shift_expr", "and_expr", "exclusive_or_expr", "inclusive_or_expr",
  "basic_expr", "concat_expr", "prefix_rhs_expr", "range_rhs_expr",
  "multiton_rhs_expr", "map_expr", "expr", "set_expr", "set_list_expr",
  "set_list_member_expr", "meter_key_expr", "meter_key_expr_alloc",
  "set_elem_expr", "set_elem_key_expr", "set_elem_expr_alloc",
  "set_elem_options", "set_elem_time_spec", "set_elem_option",
  "set_elem_expr_options", "set_elem_stmt_list", "set_elem_stmt",
  "set_elem_expr_option", "set_lhs_expr", "set_rhs_expr",
  "initializer_expr", "counter_config", "counter_obj", "quota_config",
  "quota_obj", "secmark_config", "secmark_obj", "ct_obj_type",
  "ct_cmd_type", "ct_l4protoname", "ct_helper_config", "timeout_states",
  "timeout_state", "ct_timeout_config", "ct_expect_config", "ct_obj_alloc",
  "limit_config", "limit_obj", "relational_expr", "list_rhs_expr",
  "rhs_expr", "shift_rhs_expr", "and_rhs_expr", "exclusive_or_rhs_expr",
  "inclusive_or_rhs_expr", "basic_rhs_expr", "concat_rhs_expr",
  "boolean_keys", "boolean_expr", "keyword_expr", "primary_rhs_expr",
  "relational_op", "verdict_expr", "chain_expr", "meta_expr", "meta_key",
  "meta_key_qualified", "meta_key_unqualified", "meta_stmt", "socket_expr",
  "socket_key", "offset_opt", "numgen_type", "numgen_expr", "xfrm_spnum",
  "xfrm_dir", "xfrm_state_key", "xfrm_state_proto_key", "xfrm_expr",
  "hash_expr", "nf_key_proto", "rt_expr", "rt_key", "ct_expr", "ct_dir",
  "ct_key", "ct_key_dir", "ct_key_proto_field", "ct_key_dir_optional",
  "symbol_stmt_expr", "list_stmt_expr", "ct_stmt", "payload_stmt",
  "payload_expr", "payload_raw_len", "payload_raw_expr",
  "payload_base_spec", "eth_hdr_expr", "eth_hdr_field", "vlan_hdr_expr",
  "vlan_hdr_field", "arp_hdr_expr", "arp_hdr_field", "ip_hdr_expr",
  "ip_hdr_field", "ip_option_type", "ip_option_field", "icmp_hdr_expr",
  "icmp_hdr_field", "igmp_hdr_expr", "igmp_hdr_field", "ip6_hdr_expr",
  "ip6_hdr_field", "icmp6_hdr_expr", "icmp6_hdr_field", "auth_hdr_expr",
  "auth_hdr_field", "esp_hdr_expr", "esp_hdr_field", "comp_hdr_expr",
  "comp_hdr_field", "udp_hdr_expr", "udp_hdr_field", "udplite_hdr_expr",
  "udplite_hdr_field", "tcp_hdr_expr", "inner_inet_expr", "inner_eth_expr",
  "inner_expr", "vxlan_hdr_expr", "vxlan_hdr_field", "geneve_hdr_expr",
  "geneve_hdr_field", "gre_hdr_expr", "gre_hdr_field", "gretap_hdr_expr",
  "optstrip_stmt", "tcp_hdr_field", "tcp_hdr_option_kind_and_field",
  "tcp_hdr_option_sack", "tcp_hdr_option_type", "tcpopt_field_sack",
  "tcpopt_field_window", "tcpopt_field_tsopt", "tcpopt_field_maxseg",
  "tcpopt_field_mptcp", "dccp_hdr_expr", "dccp_hdr_field",
  "sctp_chunk_type", "sctp_chunk_common_field", "sctp_chunk_data_field",
  "sctp_chunk_init_field", "sctp_chunk_sack_field", "sctp_chunk_alloc",
  "sctp_hdr_expr", "sctp_hdr_field", "th_hdr_expr", "th_hdr_field",
  "exthdr_expr", "hbh_hdr_expr", "hbh_hdr_field", "rt_hdr_expr",
  "rt_hdr_field", "rt0_hdr_expr", "rt0_hdr_field", "rt2_hdr_expr",
  "rt2_hdr_field", "rt4_hdr_expr", "rt4_hdr_field", "frag_hdr_expr",
  "frag_hdr_field", "dst_hdr_expr", "dst_hdr_field", "mh_hdr_expr",
  "mh_hdr_field", "exthdr_exists_expr", "exthdr_key", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-1851)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1064)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
   -1851,  8220, -1851,   660, -1851, -1851,   126,   141,   141,   141,
    1006,  1006,  1006,  1006,  1006,  1006,  1006,  1006, -1851, -1851,
    4550,   221,  2996,   248,  2560,   287,  7099,  1548,  1607,   256,
    7836,   213,   279,  3195,   247, -1851, -1851, -1851, -1851,   527,
    1006,  1006,  1006,  1006, -1851, -1851, -1851,   925, -1851,   141,
   -1851,   141,   185,  6921, -1851,   660, -1851, -1851,    70,   229,
     660,   141, -1851,   196,   210,  6921,   141, -1851,   575, -1851,
     141, -1851, -1851,  1006, -1851,  1006,  1006,  1006,  1006,  1006,
    1006,  1006,   555,  1006,  1006,  1006,  1006, -1851,  1006, -1851,
    1006,  1006,  1006,  1006,  1006,  1006,  1006,  1006,   663,  1006,
    1006,  1006,  1006, -1851,  1006, -1851,  1006,  1006,  1006,  1006,
    1006,  1006,  1172,  1006,  1006,  1006,  1006,  1006,  1050,  1006,
    1006,  1006,   634,  1006,  1363,  1613,  2286,  2514,  1006,  1006,
    1006,  2646, -1851,  1006,  2523,  1006,  1006,  1006,  1006,  1606,
    1824,  1006, -1851,  1006,  1006,  1006,  1006,  1006,   579,  1006,
   -1851,  1006, -1851,   728,  1194,   245,   487, -1851, -1851, -1851,
   -1851,   288,  1055,  1124,  1854,  3329,  1605,   616,  3106,  2570,
    1407,   749,   261,   966,   923,  2613,   938,  5905,   808, -1851,
    6114,   139,  2551,   390,   488,   556,   230,  1148,   670,  1658,
    4950, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851,  5162, -1851, -1851,   566,  7660,   408,  1263,   949,
    7836,   141, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851,   895, -1851, -1851,   429, -1851, -1851,   895, -1851, -1851,
    1006,  1006,  1006,  1006,  1006,  1006,  1006,  1006,   663,  1006,
    1006,  1006,  1006, -1851, -1851, -1851,  1879, -1851, -1851, -1851,
    1006,  1006,  1006,    53, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851,   656,   693,   726, -1851, -1851, -1851,   647,   547,  1167,
   -1851, -1851, -1851,   679, -1851, -1851, -1851,    92,    92, -1851,
     223,   141,  8499,  5373,   570,   558, -1851,   -56,   683, -1851,
   -1851, -1851, -1851, -1851,   205,   806,   699, -1851,   794,   917,
   -1851,   563,  6921, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1025, -1851,
   -1851,   594, -1851, -1851, -1851,   608, -1851,  5264, -1851, -1851,
     729, -1851,   411, -1851,   534, -1851, -1851, -1851, -1851,  1160,
   -1851,   138, -1851, -1851, -1851, -1851, -1851,  1309,   924,   927,
     609, -1851,   325, -1851,  6309, -1851, -1851, -1851,   910, -1851,
   -1851, -1851,   941, -1851, -1851,  6690,  6690, -1851, -1851,   151,
     637,   669, -1851, -1851,   674, -1851, -1851, -1851,   681, -1851,
     689,  1007,  6921, -1851,   196,   210, -1851,   575, -1851, -1851,
    1006,  1006,  1006,   791, -1851, -1851, -1851,  6921, -1851,   152,
   -1851, -1851, -1851,   207, -1851, -1851, -1851,   252,   210, -1851,
   -1851, -1851,   293, -1851, -1851,   575, -1851,   491,   695, -1851,
   -1851, -1851, -1851,  1006, -1851, -1851, -1851, -1851,   575, -1851,
   -1851, -1851,  1070, -1851, -1851, -1851, -1851,  1006, -1851, -1851,
   -1851, -1851, -1851, -1851,  1006,  1006, -1851, -1851, -1851,  1074,
    1086, -1851,  1006,  1092, -1851,  1006, -1851,  1006, -1851,  1006,
   -1851,  1006, -1851, -1851, -1851, -1851,  1006, -1851, -1851, -1851,
    1006,  1006,   258,   141, -1851, -1851, -1851,   575, -1851, -1851,
    1006, -1851, -1851,  1006, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851,  1006, -1851,   141, -1851, -1851, -1851, -1851,
    1132, -1851, -1851, -1851, -1851, -1851,  1119,   614, -1851, -1851,
     844, -1851, -1851,  1053,   120, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,   280,   486,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1337, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851,  2950, -1851, -1851,
   -1851, -1851,  1076, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    4974, -1851,  6384, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    3912, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851,   166, -1851, -1851,   819, -1851, -1851, -1851,
   -1851, -1851, -1851,   834, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851,  3047, -1851, -1851, -1851,
   -1851,   866,   330,   879,  1121, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851,   872,   869, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,   575,
   -1851,   695, -1851,  1006, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851,   895, -1851, -1851,
   -1851, -1851,    87,   337,   601,   593, -1851, -1851, -1851,  5473,
    1183,  7451,  7836,  1080, -1851, -1851, -1851, -1851,  1255,  1270,
     119,  1243,  1260,  1265,   135,  1276,  3047,  1284,  7451,   148,
    7451,  1018,  7451, -1851, -1851,  1236,  7836,  1017,  7451,  7451,
    1259,  1527, -1851,  6410,   160, -1851,  1527, -1851, -1851, -1851,
    1010, -1851,  1256,  1279,  1025, -1851, -1851, -1851,   891,  1527,
    1293,  1297,  1299,  1527,   594, -1851, -1851,   632, -1851, -1851,
    7451, -1851, -1851,  5682,  1294,  1055,  1124,  1854,  3329, -1851,
    3106,   684, -1851, -1851, -1851, -1851,  1322, -1851, -1851, -1851,
   -1851,  7451, -1851,  1394,  1396,  1404,  1068,   987,   366, -1851,
   -1851, -1851, -1851,  1438,  1324,  1440, -1851, -1851, -1851, -1851,
    1444, -1851, -1851, -1851, -1851,   571, -1851, -1851,  1448,  1453,
   -1851, -1851, -1851,  1342,  1359, -1851, -1851,   729, -1851, -1851,
    1463, -1851, -1851, -1851, -1851,  1457, -1851, -1851,  5891, -1851,
    1457, -1851, -1851, -1851,   103, -1851, -1851,  1160, -1851,  1481,
   -1851,   141, -1851,  1116, -1851,  8352,  8352,  8352,  8352,  8352,
    7836,   177,  8041, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  8352, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851,   332, -1851,  1401,  1476,
    1472,  1134,  1028,  1497, -1851, -1851, -1851,  8041,  7451,  7451,
    1409,   140,   660,  1509, -1851,  1101,   660,  1427, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1502,  1182,  1186,
    1192, -1851,  1197,  1202, -1851, -1851, -1851, -1851,  1269,  1248,
    1210,  1527, -1851, -1851,  1466,  1492,  1496,  1220,  1524, -1851,
    1531,  1272, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1541,
   -1851, -1851, -1851, -1851, -1851,  1006, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851,  1543,  1194, -1851, -1851, -1851,
   -1851,  1546, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
     947, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851,  1565, -1851,  1477, -1851, -1851,
    1467, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    1291, -1851,  1315,  1533, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851,   979,  1395,  1120,  1120, -1851, -1851, -1851,  1439, -1851,
   -1851, -1851, -1851,  1441,  1443, -1851,  1437,  1442,  1446,   774,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1576,
   -1851, -1851,  1579, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851,  1420, -1851,  1425, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851,  1581,  1594,  1355, -1851, -1851,
   -1851, -1851, -1851,  1598,   615, -1851, -1851, -1851,  1338, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851,  1349,  1351,  1354,  1603,
   -1851, -1851,  1018, -1851, -1851, -1851,  1619, -1851, -1851, -1851,
   -1851,  7451,  3329,  3106,  1719,  6100, -1851,   138,   226,  1717,
    3371,  1527,  1527,  1625,  7836,  7451,  7451,  7451, -1851,  1626,
    7451,  1679,  7451, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    1633, -1851,   318,  1715, -1851, -1851,   211,   377,   330, -1851,
     463,   485,   173,  1695, -1851,  7451, -1851, -1851,   917,  1549,
    1019,   398, -1851,  1082,  1497,   917, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851,  1604,   711, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851,   908,   912, -1851,  1060, -1851,
   -1851, -1851,  7451,  1752,  7451, -1851, -1851, -1851,   661,   720,
   -1851,  7451, -1851, -1851,  1391, -1851, -1851,  7451,  7451,  7451,
    7451,  7451,  1665,  7451,  7451,   197,  7451,  1457,  7451,  1676,
    1768,  1692,  3415,  3415, -1851, -1851, -1851,  7451,  1324,  7451,
    1324, -1851,  1759,  1760, -1851,  1017, -1851,  7836, -1851, -1851,
    1401,  1476,  1472, -1851,   917, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851,  1417,  8352,  8352,  8352,  8352,  8352,  8352,  8352,
    8352,  8541,  8352,  8352,   339, -1851,  1118, -1851, -1851, -1851,
   -1851, -1851,  1684, -1851,   822,   765, -1851,  2508,  3589,  1169,
    2931,   483, -1851, -1851, -1851, -1851, -1851, -1851,  1424,  1431,
    1434, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851,  1792, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851,  3371, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1429,  1435,
   -1851, -1851, -1851, -1851, -1851, -1851,  1355,   727,  1705, -1851,
   -1851, -1851, -1851, -1851,  1459, -1851, -1851, -1851, -1851, -1851,
    1514,   530, -1851,  1633,  2427, -1851,   858,   318, -1851,  1199,
   -1851, -1851,  7451,  7451,  1807, -1851, -1851,  1711,  1711, -1851,
     226, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    1452,  1717,  6921,   226, -1851, -1851, -1851, -1851, -1851, -1851,
    7451, -1851, -1851, -1851,   191,  1505,  1507,  1800, -1851, -1851,
   -1851,  1506,   103, -1851,  7836,   103,  7451,  1780, -1851,  8271,
   -1851,  1634,  1534,  1517,  1521,  1210,  1019, -1851,  1731,  1711,
   -1851,   722, -1851,  6410, -1851,  4988, -1851, -1851, -1851, -1851,
    1829, -1851, -1851,  1473, -1851, -1851,  1473, -1851,  1767,  1473,
   -1851, -1851,  7451, -1851, -1851, -1851, -1851, -1851,  1394,  1396,
    1404, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1834,  7451,
    1675,  7451, -1851, -1851, -1851, -1851,  1324, -1851,  1324,  1457,
   -1851, -1851,   296,  6584,   170, -1851, -1851, -1851,  1509,  1838,
   -1851, -1851,  1401,  1476,  1472, -1851,   313,  1509, -1851, -1851,
    1082,  8352,  8541, -1851,  1743,  1815, -1851, -1851, -1851, -1851,
   -1851,   141,   141,   141,   141,   141,  1753,   790,   141,   141,
     141,   141, -1851, -1851, -1851,   660, -1851,  1493,   144, -1851,
    1771, -1851, -1851, -1851,   660,   660,   660,   660,   660,  7836,
   -1851,  1711,  1711,  1504,  1484,  1756,  1140,  1278,  1681, -1851,
    1562,  1210, -1851, -1851, -1851,   660,   660,   660,   750, -1851,
    7836,  1711,  1711,  1511,  1140,  1278, -1851, -1851, -1851,   660,
     660,   750,  1772,  1523,  1781, -1851, -1851, -1851, -1851, -1851,
    4803,  3943,  2878,  4060,   926, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851,  4297,  1002, -1851, -1851,  1776, -1851, -1851, -1851,
    1880, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    1785, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1377, -1851,
    1261,  1187,   541,  1787, -1851, -1851, -1851, -1851, -1851,  1505,
    1507, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851,  1506, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  7451,
   -1851, -1851, -1851, -1851, -1851, -1851,  7836,  1528,   226, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851,  1067,  1874, -1851,  1794,
   -1851,  1796, -1851,  1067,  1797, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851,  7451,    92,    92,   917,  1497, -1851,   265,  1799,
   -1851,   192,  1018,  1802, -1851, -1851, -1851, -1851,  1019, -1851,
   -1851, -1851,   660, -1851,   711, -1851, -1851, -1851, -1851, -1851,
   -1851,  7451, -1851,  1810, -1851,  1457,  1457,  7836, -1851,   703,
    1909,   917, -1851,  1509,  1509,  1725,  1814, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1911, -1851,
     141,   141,   141, -1851, -1851, -1851, -1851, -1851,   617, -1851,
   -1851, -1851, -1851, -1851,  1816, -1851, -1851, -1851, -1851, -1851,
   -1851,  1913, -1851,   660,   660,   575, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1914, -1851, -1851,
   -1851, -1851, -1851,  1195, -1851, -1851, -1851, -1851, -1851, -1851,
    1179,   660,   660,   575,  1214,  1195, -1851, -1851, -1851,  1782,
     617,   660, -1851, -1851, -1851, -1851, -1851, -1851,  1586,  1955,
    2124, -1851, -1851, -1851, -1851,  1820, -1851,  1355, -1851, -1851,
   -1851,  1568,   812,  1006, -1851, -1851, -1851, -1851, -1851,  1711,
    1836,   812,  1837,  1006, -1851, -1851, -1851, -1851, -1851,  1843,
    1006, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851,  6921, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851,  1648, -1851,   328, -1851, -1851, -1851,
     318, -1851, -1851, -1851, -1851, -1851, -1851,  1846,  1657, -1851,
   -1851,  1633,   318, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
    7451,  1588,  7836, -1851,  1906,  6584, -1851, -1851,  1770,   660,
    1590,  1591,  1596,  1599,  1616,  1753, -1851, -1851, -1851,  1627,
    1629,  1631,  1632,    98,   660, -1851, -1851,  1921,  7836, -1851,
   -1851, -1851, -1851,  1140, -1851,  1278, -1851,  7192, -1851, -1851,
   -1851,   783, -1851,   183,   660,   660, -1851, -1851, -1851, -1851,
   -1851,  1961, -1851,  1635, -1851, -1851,   660,   660, -1851,   660,
     660,   660,   660,   660, -1851,  1835,   660, -1851,  1638, -1851,
   -1851, -1851, -1851, -1851,  1867, -1851, -1851,  1505,  1507,  1506,
   -1851, -1851, -1851, -1851,  1640,   917, -1851, -1851,  1725, -1851,
   -1851, -1851, -1851, -1851, -1851,  1642,  1645,  1651, -1851, -1851,
   -1851, -1851, -1851, -1851,   172, -1851, -1851, -1851,  1894, -1851,
   -1851, -1851, -1851,  7836,  5582, -1851, -1851, -1851, -1851, -1851,
     660,  2004, -1851,   660,  2008, -1851,   660,  1140,  1919, -1851,
   -1851, -1851,  1254, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851,  1776, -1851,  1922, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851,   812, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851,  1770,  1154,  4651,  3173,  4414,  2186, -1851,
   -1851, -1851,  1516,  2434,  1334,  2007,   145, -1851,  1561, -1851,
    1913, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  7836,
   -1851, -1851,  1436, -1851,  1925,  1937, -1851,  2020,   178, -1851,
     660, -1851, -1851, -1851, -1851, -1851,   660,   660,   660,   660,
     660,  2168,  2466,  2360,   660,   660,   660,   660, -1851, -1851,
     282,  1678,  1782, -1851,  2034, -1851, -1851, -1851, -1851,  1520,
    1922,   660, -1851, -1851, -1851, -1851, -1851, -1851,   660,   660,
     660, -1851, -1851, -1851, -1851, -1851, -1851,   617, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,     4,     5,     0,     0,     0,     0,
     442,   442,   442,   442,   442,   442,   442,   442,   446,   449,
     442,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   237,   448,     9,    28,    29,     0,
     442,   442,   442,   442,    68,    67,     3,     0,    71,     0,
     443,     0,   467,     0,    66,     0,   434,   435,     0,     0,
       0,     0,   626,    87,    89,     0,     0,   294,     0,   317,
       0,   347,    72,   442,    73,   442,   442,   442,   442,   442,
     442,   442,     0,   442,   442,   442,   442,    74,   442,    75,
     442,   442,   442,   442,   442,   442,   442,   442,     0,   442,
     442,   442,   442,    76,   442,    77,   442,   473,   442,   473,
     442,   473,   473,   442,   442,   473,   442,   473,     0,   442,
     473,   473,     0,   442,   473,   473,   473,   473,   442,   442,
     442,   473,    35,   442,   473,   442,   442,   442,   442,   473,
     473,   442,    47,   442,   442,   442,   442,   473,     0,   442,
      80,   442,    81,     0,     0,     0,   778,   749,   436,   437,
     438,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   964,   965,   966,   967,   968,   969,   970,   971,   972,
     973,   974,   975,   976,   977,   978,   979,   980,   981,   982,
     983,   985,     0,   987,   986,     0,     0,     0,     0,    34,
       0,     0,    85,   745,   744,   750,   751,   252,   761,   762,
     755,   955,   756,   759,   763,   760,   757,   758,   752,  1069,
    1070,  1071,  1072,  1073,  1074,  1075,  1076,  1077,  1078,  1079,
    1080,  1081,  1082,    53,  1087,  1088,  1089,  1090,  1084,  1085,
    1086,   753,  1336,  1337,  1338,  1339,  1340,  1341,  1342,  1343,
     754,     0,   249,   250,     0,    33,   233,     0,    21,   235,
     442,   442,   442,   442,   442,   442,   442,   442,     0,   442,
     442,   442,   442,    16,   238,    39,   239,   447,   444,   445,
     442,   442,   442,    13,   876,   849,   851,    70,    69,   450,
     452,     0,     0,     0,   469,   468,   470,     0,   617,     0,
     734,   735,   736,     0,   946,   947,   948,   522,   523,   951,
       0,     0,     0,     0,   538,   543,   548,     0,   571,   595,
     607,   608,   683,   689,   710,     0,     0,   991,     0,     7,
      92,   475,   477,   488,   489,   490,   491,   492,   520,   502,
     478,    61,   275,   517,   498,   526,   496,    13,   536,    14,
      59,   546,   494,   495,    46,   598,    40,     0,    54,    60,
     615,    40,   682,    40,   688,    18,    24,   508,    45,   708,
     514,     0,   515,   500,   739,   499,   780,   783,   785,   787,
     789,   790,   797,   799,     0,   798,   742,   525,   955,   503,
     509,   501,   752,   518,    62,     0,     0,    65,   461,     0,
       0,     0,    91,   455,     0,    95,   310,   309,     0,   458,
       0,     0,     0,   626,   112,   114,   294,     0,   317,   347,
     442,   442,   442,    13,   876,   849,   851,     0,    60,     0,
     136,   137,   138,     0,   130,   131,   139,     0,   132,   133,
     141,   142,     0,   134,   135,     0,   143,     0,   145,   146,
     853,   854,   852,   442,    13,    36,    44,    51,     0,    60,
     203,   474,   205,   170,   171,   172,   173,   442,   174,   176,
     200,   199,   198,   192,   442,   473,   196,   195,   197,   853,
     854,   855,   442,     0,    13,   442,   177,   442,   180,   442,
     183,   442,   189,    36,    44,    51,   442,   186,    78,   220,
     442,   442,   474,   216,   218,   215,   222,     0,   223,    13,
     442,   208,   207,   442,   213,   211,    44,    79,   224,   225,
     226,   227,   230,   442,   229,     0,  1096,  1093,  1094,    56,
       0,   769,   770,   771,   772,   773,   775,     0,   996,   998,
       0,   997,    52,     0,     0,  1334,  1335,    56,  1098,  1099,
      55,    20,    55,  1102,  1103,  1104,  1105,    30,     0,     0,
    1108,  1109,  1110,  1111,  1112,     9,  1130,  1131,  1125,  1120,
    1121,  1122,  1123,  1124,  1126,  1127,  1128,  1129,     0,    28,
      55,  1145,  1144,  1143,  1146,  1147,  1148,    31,    55,  1151,
    1152,  1153,    32,  1162,  1163,  1155,  1156,  1157,  1159,  1158,
    1160,  1161,    29,  1174,    55,  1170,  1167,  1166,  1171,  1169,
    1168,  1172,  1173,    31,  1177,  1180,  1176,  1178,  1179,     8,
    1183,  1182,    19,  1185,  1186,  1187,    11,  1191,  1192,  1189,
    1190,    57,  1197,  1194,  1195,  1196,    58,  1244,  1238,  1241,
    1242,  1236,  1237,  1239,  1240,  1243,  1245,     0,  1198,    55,
    1278,  1279,     0,    15,  1224,  1223,  1216,  1217,  1218,  1202,
    1203,  1204,  1205,  1206,  1207,  1208,  1209,  1210,  1211,    53,
    1220,  1219,  1222,  1221,  1213,  1214,  1215,  1231,  1233,  1232,
       0,    25,     0,  1228,  1227,  1226,  1225,  1332,  1329,  1330,
       0,  1331,    49,    55,    28,  1349,  1023,    29,  1348,  1351,
    1021,  1022,    34,     0,    48,    48,     0,    48,  1355,    48,
    1358,  1357,  1359,     0,    48,  1346,  1345,    27,  1367,  1364,
    1362,  1363,  1365,  1366,    23,  1370,  1369,    17,    55,  1373,
    1376,  1372,  1375,    38,    37,   959,   960,   961,    51,   962,
      34,    37,   957,   958,  1038,  1039,  1045,  1031,  1032,  1030,
    1040,  1041,  1061,  1034,  1043,  1036,  1037,  1042,  1033,  1035,
    1028,  1029,  1059,  1058,  1060,    51,     0,    12,  1046,  1002,
    1001,     0,   797,     0,     0,    48,    27,    23,    17,    38,
    1377,  1006,  1007,   984,  1005,     0,   743,  1083,   232,   251,
      82,   234,    83,    60,   154,   155,   132,   156,   157,     0,
     158,   160,   161,   442,    13,    36,    44,    51,    86,    84,
     240,   241,   243,   242,   245,   246,   244,   247,   873,   873,
     873,    97,     0,     0,   571,     0,   464,   465,   466,     0,
       0,     0,     0,     0,   953,   952,   949,   950,     0,     0,
       0,    37,    37,     0,     0,     0,     0,    12,     0,     0,
       0,   579,     0,   569,   570,     0,     0,     0,     0,     0,
       0,     0,     6,     0,     0,   801,     0,   476,   479,   519,
       0,   493,     0,     0,   537,   539,   497,   504,     0,     0,
       0,     0,     0,     0,   547,   549,   505,     0,   594,   506,
       0,    47,    16,     0,     0,    20,    30,     9,    28,   914,
      29,     0,   919,   917,   918,    14,     0,    40,    40,   904,
     905,     0,   644,   647,   649,   651,   653,   654,   659,   664,
     662,   663,   665,   667,   606,   631,   632,   642,   906,   633,
     640,   634,   641,   637,   638,     0,   635,   636,     0,   666,
     639,   507,   516,     0,     0,   623,   622,   616,   618,   510,
       0,   701,   702,   703,   681,   686,   699,   511,     0,   687,
     692,   512,   513,   704,     0,   726,   727,   709,   711,   714,
     724,     0,   747,     0,   746,     0,     0,     0,     0,     0,
       0,     0,     0,   939,   940,   941,   942,   943,   944,   945,
      20,    30,     9,    28,    31,   931,    29,    31,     8,    19,
      11,    57,    58,    53,    15,    25,    49,    40,     0,   921,
     889,   922,   794,   795,   901,   888,   878,   877,   893,   895,
     897,   899,   900,   887,   923,   924,   890,     0,     0,     0,
       0,     7,     0,   843,   842,   900,     0,     0,   403,    60,
     259,   276,   297,   329,   348,   471,   111,     0,     0,     0,
       0,   118,     0,     0,   873,   873,   873,   120,     0,     0,
     571,     0,   129,   153,     0,     0,     0,     0,     0,   144,
       0,     0,   873,   148,   151,   149,   152,   169,   191,     0,
     206,   175,   194,   193,    12,   442,   179,   178,   181,   184,
     190,   185,   182,   188,   187,   217,   219,   221,   210,   209,
     212,   214,   228,   231,  1095,     0,     0,    55,   766,   767,
      22,     0,   994,   779,    42,    42,  1333,  1100,  1097,  1106,
    1101,    20,    28,    20,    28,  1107,  1132,  1133,  1134,  1135,
      28,  1117,  1142,  1141,  1150,  1149,  1154,  1165,  1164,  1175,
    1181,  1184,  1188,  1193,    10,  1262,  1268,  1266,  1257,  1258,
    1261,  1263,  1252,  1253,  1254,  1255,  1256,  1264,  1259,  1260,
    1265,  1200,  1267,  1199,  1280,    15,  1276,  1212,  1230,  1229,
    1234,  1284,  1281,  1282,  1283,  1285,  1286,  1287,  1288,  1289,
    1290,  1291,  1292,  1293,  1294,  1295,  1296,  1297,  1298,  1315,
      50,  1327,  1350,  1017,  1018,  1024,    48,  1019,  1347,     0,
    1352,  1354,     0,  1356,  1344,  1361,  1368,  1374,  1371,   956,
     963,   954,  1044,  1047,  1048,     0,  1050,     0,  1049,  1051,
    1052,    12,    12,  1053,  1025,     0,     0,   999,  1379,  1378,
    1380,  1381,  1382,     0,     0,   764,   168,   159,     0,   873,
     163,   166,   164,   167,   236,   248,     0,     0,     0,     0,
     368,    13,   579,   393,    36,   373,     0,    44,   398,   850,
      51,     0,    28,    29,   609,     0,    60,     0,   728,   730,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,  1053,     0,    13,   439,   544,   545,    36,   577,   578,
       0,    44,     0,     0,   723,    45,   718,   717,     0,   722,
     720,   721,     0,   695,   697,     0,   521,   814,     7,     7,
     816,   810,   813,   900,   839,     7,   800,   472,   285,   541,
     542,   540,   562,    20,     0,     0,   560,   556,   551,   552,
     553,   554,   557,   555,   550,     0,     0,    53,     0,   673,
     915,   916,     0,   668,     0,   907,   910,   911,   908,   909,
     920,     0,   913,   912,     0,   631,   640,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   675,     0,     0,
       0,     0,     0,     0,   620,   621,   619,     0,     0,     0,
     690,   713,   718,   717,   712,     0,    10,     0,   781,   782,
     784,   786,   788,   791,     7,   527,   529,   796,   908,   930,
     909,   932,   929,   928,   934,   926,   927,   925,   935,   933,
     936,   937,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   884,   883,   900,   989,  1068,   845,
     844,    63,     0,    64,     0,     0,   109,     0,     0,     0,
       0,     0,    60,   259,   276,   297,   329,   348,     0,     0,
       0,    13,    36,    44,    51,   462,   451,   453,   276,   456,
     459,   348,    12,   204,   201,    12,     0,   774,   768,   765,
      52,   776,   777,  1113,  1115,  1114,  1116,    55,  1137,  1139,
    1138,  1140,  1119,    28,     0,  1274,  1246,  1271,  1248,  1275,
    1251,  1272,  1273,  1249,  1269,  1270,  1247,  1250,  1277,  1312,
    1311,  1313,  1314,  1320,  1302,  1303,  1304,  1305,  1317,  1306,
    1307,  1308,  1309,  1310,  1318,  1319,  1321,  1322,  1323,  1324,
    1325,  1326,    55,  1301,  1300,  1316,    49,  1020,     0,     0,
      28,    28,    29,    29,  1026,  1027,   999,   999,     0,    26,
    1004,  1008,  1009,    34,     0,   348,    12,   383,   388,   378,
       0,     0,    98,     0,     0,   105,     0,     0,   100,     0,
     107,   611,     0,     0,   610,   485,   731,     0,     0,   822,
     729,   817,  1262,  1266,  1261,  1265,  1267,    53,    10,    10,
       0,   809,     0,   807,    37,    37,    12,   534,    12,    12,
       0,    12,   482,   483,     0,   580,   583,     0,   484,   573,
     572,   574,     0,   705,     0,     0,     0,     0,   804,     0,
     805,     0,    13,     0,     0,   571,   815,   825,     0,     0,
     838,   811,   823,   803,   802,     0,   561,    28,   565,   566,
      53,   564,   596,     0,   600,   597,     0,   602,     0,     0,
     604,   674,     0,   678,   680,   643,   645,   646,   648,   650,
     652,   660,   661,   655,   658,   657,   656,   670,   669,     0,
       0,     0,  1062,  1063,  1064,  1065,   684,   700,   691,   693,
     725,   748,     0,     0,     0,   530,   938,   886,   880,     0,
     891,   892,   894,   896,   898,   885,   792,   879,   793,   902,
     903,     0,     0,   792,     0,     0,    60,   405,   404,   407,
     406,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    88,   261,   260,     0,   255,     0,     0,    55,
       0,    90,   278,   277,     0,     0,     0,     0,     0,     0,
     306,     0,     0,     0,     0,     0,     0,     0,     0,   538,
       0,   571,    93,   299,   298,     0,     0,     0,     0,   480,
       0,     0,     0,     0,     0,     0,    94,   331,   330,     0,
       0,     0,     0,     0,     0,    13,    96,   350,   349,   128,
       0,     0,     0,     0,     0,   383,   388,   378,   121,   126,
     122,   127,     0,     0,   150,   202,     0,   995,  1136,  1118,
       0,  1299,  1328,  1353,  1360,  1054,  1055,  1056,  1057,    41,
       0,    26,  1000,  1016,  1012,  1011,  1010,    34,     0,   165,
       0,     0,     0,     0,    13,   370,   369,   372,   371,   580,
     583,    36,   395,   394,   397,   396,    44,   375,   374,   377,
     376,   574,    51,   400,   399,   402,   401,   612,   614,     0,
     820,   821,   818,  1235,   993,   992,     0,     0,   808,   990,
     988,   486,   535,   487,    12,  1066,     0,   585,   587,     0,
      36,     0,    36,     0,     0,    44,   719,   715,   716,    45,
      45,   696,     0,     0,     0,     7,   840,   841,     0,     0,
     827,     0,   579,     0,   826,   819,   836,   837,   812,   824,
     524,   286,     0,   559,     0,   558,    55,    55,    47,    55,
     671,     0,   677,     0,   679,   685,   694,     0,   732,     0,
       0,     7,   528,   882,   881,   627,     0,   110,   463,   367,
     454,   275,   457,   294,   317,   460,   347,   258,   254,   256,
       0,     0,     0,   367,   367,   367,   367,   262,     0,   432,
     433,    43,   431,   430,     0,   428,   279,   281,   280,   284,
     282,   295,   292,     0,     0,     0,   346,   345,    43,   344,
     408,   410,   411,   409,   364,   412,   365,    55,   363,   313,
     314,   316,   315,     0,   312,   300,   307,   308,   304,   481,
       0,     0,     0,     0,     0,     0,   342,   341,   339,     0,
       0,     0,   353,   113,   115,   116,   117,   119,     0,     0,
       0,   140,   147,  1091,    10,     0,  1003,   999,  1015,  1013,
     162,     0,     0,     0,    12,   385,   384,   387,   386,     0,
       0,     0,     0,     0,    12,   390,   389,   392,   391,     0,
       0,    12,   380,   379,   382,   381,   846,    99,   874,   875,
     106,   101,   848,   108,   613,     0,   740,  1067,   589,   590,
     591,   592,   593,   582,     0,   567,     0,   584,   568,   586,
       0,   576,   706,   707,   698,   806,    12,     0,     0,    14,
      14,     0,     0,   287,   563,    31,    31,   605,   603,   672,
       0,     0,     0,   733,     0,   532,   531,   628,   629,     0,
       0,     0,     0,     0,     0,     0,   367,   367,   367,     0,
       0,     0,     0,     0,     0,   356,   429,     0,     0,   302,
     303,   305,   343,     0,   296,     0,   301,     0,   332,   333,
     340,   328,   338,     0,     0,     0,   354,    12,    12,    12,
    1092,     0,    26,     0,    57,    53,     0,     0,   103,     0,
       0,     0,     0,     0,   104,     0,     0,   102,     0,   581,
     588,   575,   831,    12,     0,   834,   835,   580,   583,   574,
     599,   601,   676,   737,     0,     7,   630,   624,   627,   403,
     276,   297,   329,   348,   257,     0,     0,     0,   368,   393,
     373,   398,   361,   360,     0,   357,   362,   283,     0,   293,
     366,   311,    60,     0,     0,    13,    36,    44,    51,   291,
       0,     0,   325,     0,   288,   327,     0,     0,     0,   422,
     416,   415,   419,   414,   417,   418,   351,   352,   124,   125,
     123,     0,  1014,     0,   859,   858,   865,   867,   870,   871,
     868,   869,   872,     0,   861,   741,   832,    13,    36,    36,
      44,   738,   533,   629,     0,     0,     0,     0,     0,   383,
     388,   378,     0,     0,     0,     0,     7,   355,   427,   324,
     290,   318,   319,    12,   320,   322,   321,   323,   335,     0,
     337,    55,     0,   423,     0,     0,  1201,     0,     0,   862,
       0,   828,   829,   830,   833,   625,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   359,   358,
       0,     0,     0,   326,   289,   336,    55,   421,   420,     0,
       0,     0,    55,    60,   263,   264,   265,   266,     0,     0,
       0,    13,    36,    44,    51,   424,   425,     0,   413,   334,
     440,   441,   864,   863,    43,   860,   274,    12,    12,    12,
     267,   272,   268,   273,   426,   866,   270,   271,   269
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1851, -1851,    -1, -1242,  1029,    61, -1299,  1023,   995,  -347,
    -904,  -781,  1137,  1252, -1851,  1027,  -527, -1851, -1851,  1257,
   -1851,  -147, -1626,  1262,   -22,   -26,  1486,  -596, -1851, -1851,
    -694, -1851,  -489,  -722,  1266, -1851,  -301, -1851,   918, -1835,
    -505, -1210, -1851,  -882,  -493,  -961, -1851,  -510,   574,  -646,
   -1851,  -425,  1499,  -995,  1045, -1851,  -441, -1851,    12, -1851,
   -1851,  2048, -1851, -1851, -1851,  1788,  1789,   644,  1960, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851,    21, -1851,  1639, -1851, -1851,   -41,
     612,  -352, -1400, -1851, -1851, -1851, -1631,  -422, -1851, -1409,
    -410,   303,   -54,  -413, -1851,   -51, -1851, -1851, -1406, -1411,
   -1851,  -421, -1415, -1850, -1851,  -199,   -50, -1665,  -844,  -119,
    -109, -1672, -1669, -1663,  -107,  -106,   -93, -1851, -1851,  -224,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,   127,  -771,
   -1458, -1851,   235,   -36,  3288, -1851,   268, -1851, -1851,   349,
   -1851,   376,   736,  1818, -1851,  2678, -1851,   -90,  1670, -1851,
   -1851,   292,   628,   627,   142,   -42, -1851, -1851, -1420, -1851,
   -1851, -1851, -1851, -1851, -1851, -1358,  -348, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851,   171, -1851, -1851, -1851, -1851,  1209,
   -1851, -1851, -1851, -1851,  1205, -1851, -1851, -1851,   200, -1851,
    -322, -1477, -1650, -1851, -1180, -1649, -1459, -1648, -1442,   225,
     227, -1851, -1851, -1031, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851,  1149,  -350,  1682,   -71,  -141,  -665,   747,   752,   746,
   -1851,  -811, -1851, -1851, -1851, -1851, -1851, -1851,  1790, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,  -343,   737,
   -1851, -1851, -1851, -1851,  1147,   514,  -907,   515,  1264,   733,
   -1340, -1851, -1851,  1826, -1851, -1851, -1851, -1851,   -67,    -8,
    -847,  -361,   253,     3, -1851, -1851, -1851,  1020,     5, -1851,
   -1851, -1851, -1851, -1851,  -178,  -213, -1851, -1851,   704,  -825,
    1932,   -62, -1851,   814,   291, -1851, -1552, -1851, -1851,   551,
   -1851, -1404, -1851,   522, -1438,   523, -1851, -1851,  1750,  -612,
    1724,  -598,  1726,  -541,  1727,   162, -1851, -1806, -1851, -1851,
    -160, -1851, -1851,  -600,  -477,  1732, -1851,  -385,  -332,  -851,
    -840,  -835, -1851,  -354,  -866, -1851,  -292,  1623,  -826, -1851,
   -1521,  -320,   302,  1847, -1851,   -17, -1851,   622, -1851, -1419,
   -1851,   630, -1851, -1851, -1851, -1851, -1851,   730,  -283,   824,
    1455,   883,  1842,  1848, -1851, -1851,  -495,   250, -1851, -1851,
   -1851,   945,   -57, -1851, -1851,   -25, -1851,   -12, -1851,     7,
   -1851,   -64, -1851, -1851, -1851,   -59, -1851,   -58, -1851,   -35,
   -1851,   -29, -1851,   -24, -1851,   -18, -1851,   -14, -1851,    -6,
   -1851,    14, -1851,    27,  1482, -1851,   -81, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851,  1518, -1068, -1851,
   -1851, -1851, -1851, -1851,    33, -1851, -1851, -1851, -1851,   989,
   -1851, -1851,    37, -1851,    38, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851, -1851,
   -1851, -1851, -1851, -1851, -1851
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,  1787,   883,  1159,  1367,  1504,  1161,  1244,   841,
     896,  1186,   828,  1226,   981,  1160,  1365,   812,  1489,  1225,
     982,   700,  1823,  1224,  1418,  1420,  1366,  1153,  1155,   810,
     803,   518,  1094,  1229,  1228,   829,   909,  2026,  1491,  2126,
    1095,   983,   906,   537,  1217,  1211,  1546,  1096,  1132,   807,
     961,  1137,  1124,  1162,  1163,   897,   962,   889,  1788,    46,
      47,    48,    74,    87,    89,   452,   456,   461,   448,   103,
     293,   105,   132,  1100,   480,   142,   150,   152,   275,   278,
     295,   296,   837,  1264,   276,   222,   420,  1735,  1948,  1949,
    1457,   421,  1458,  1645,  2219,  2220,  1971,   424,  1765,  1459,
     425,  1993,  1994,   428,  2283,  2222,  2223,  2227,  1460,  1766,
    1978,   430,  1461,  2124,  2204,  2205,  1986,  1987,  2110,  1571,
    1576,  1832,  1830,  1831,  1574,  1579,  1455,  1988,  1744,  2144,
    2232,  2233,  2234,  2322,  1745,  1746,  1961,  1962,  1938,   223,
    1306,  2352,    49,    50,    61,   455,    52,   459,  1941,   463,
     464,  1943,    71,   469,  1946,   450,   451,  1939,   314,   315,
     316,    53,   432,  1589,   482,  1748,   351,   352,  1768,   353,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   363,
     364,   365,  1415,  1694,  1695,   366,   367,   368,   894,   895,
     369,   370,   371,   904,   905,  1353,  1347,  1650,  1651,   372,
    1276,  1621,  1885,   373,  1310,  1880,  1615,  1882,  1616,  1617,
    2073,   374,   375,  1654,   908,   376,   377,   378,   379,   380,
     967,   968,  1720,   419,  2108,  2187,   932,   933,   934,   935,
     936,   937,   938,  1674,   939,   940,   941,   942,   943,   944,
     381,   382,   974,   383,   384,   979,   385,   386,   975,   976,
     387,   388,   389,   987,   988,  1313,  1314,  1315,   989,   990,
    1287,  1288,   390,   391,   392,   393,   394,   395,   224,   945,
     993,  1030,   946,   396,   228,  1130,   556,   557,   947,   564,
     397,   398,   399,   400,   401,   402,  1032,  1033,  1034,   403,
     404,   405,   884,   885,  1602,  1603,  1329,  1330,  1331,  1590,
    1906,  1591,  1641,  1636,  1637,  1642,  1332,  1895,  1052,  1838,
     842,  1850,   844,  1856,   845,   473,   503,  2156,  2055,  2298,
    2299,  2038,  2048,  1266,  1845,   843,   406,  1053,  1054,  1038,
    1039,  1040,  1041,  1333,  1043,   948,   949,   950,  1046,  1047,
     407,   856,   951,   761,   762,   231,   409,   952,   562,  1559,
     791,   953,  1254,   804,  1563,  1827,   234,   954,   723,   956,
     724,   957,   786,   787,  1241,  1242,   788,   958,   959,   410,
     411,   960,  2024,   239,   550,   240,   571,   241,   577,   242,
     585,   243,   599,  1150,  1503,   244,   607,   245,   612,   246,
     622,   247,   633,   248,   639,   249,   642,   250,   646,   251,
     651,   252,   656,   253,   690,   691,   692,   254,   693,   255,
     706,   256,   701,   257,   413,   668,  1181,  1596,  1183,  1516,
    1508,  1513,  1506,  1510,   258,   673,  1209,  1545,  1528,  1534,
    1523,  1210,   259,   712,   260,   567,   261,   262,   737,   263,
     725,   264,   727,   265,   729,   266,   734,   267,   744,   268,
     747,   269,   753,   270,   800
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      44,   426,    54,   792,   888,  1113,   427,  1083,   857,  1112,
     890,   350,   299,    45,  1070,   298,   875,  1334,  1073,  1036,
     891,  1370,   225,   422,  1111,  1072,  1425,  1071,  1215,  1360,
     994,  1121,   702,   227,   850,   229,   408,  1158,  1098,  1231,
    1781,   980,   805,  1187,  1138,   225,   308,  1091,   408,  1779,
    1042,  1319,  1794,   279,   414,  1089,  1792,   225,   229,   417,
    1793,  1055,  1055,   880,  1791,  1430,  1803,  1692,  1097,  1059,
     229,   481,  1037,   481,  1279,   481,   481,  1401,  1802,   481,
     969,   481,   977,  1069,   481,   481,  1628,  1630,   481,   481,
     481,   481,  1573,  1643,   955,   481,  1077,   297,   522,   705,
    1851,  1769,  1769,   522,   522,  1623,  1087,  1691,  1897,  2004,
    1326,   481,  1044,   679,  1839,  1337,   679,  1117,   680,   681,
    1374,   680,   681,  1044,  1044,  2020,  2018,  1093,  1348,  1860,
    1861,  1840,  1352,  2019,    58,    59,    60,  1819,  1821,  2000,
    1231,  1930,   682,  2132,   882,   682,  1410,  1139,   683,   882,
    1828,   683,   676,   684,  1416,   676,   684,  1106,  1411,   685,
    2145,   991,   685,   686,  1412,   677,   686,  1335,   677,  1408,
    1409,   687,  1693,   763,   687,  1152,   309,  1931,   310,  2276,
    1405,  1907,  1118,  1154,   678,  2330,  1862,   678,   418,  1057,
    2058,   688,  2059,   423,   688,  2028,    56,   429,  1904,  1157,
     991,  2062,  2202,  2203,   689,  2228,   157,   689,   225,  1084,
     694,  1876,   225,   694,   695,   696,  1293,   695,   696,  -744,
     991,   229,  1597,  1959,  1960,   229,  -744,  -744,  -744,  -744,
    1271,    55,  1218,  -715,  1220,  2161,  1221,   870,  1298,  1267,
    1268,  1223,   311,  1428,  1184,    56,  1277,  1134,  1230,  2202,
    2203,   484,  1304,   486,   488,   871,    56,   491,  1624,   493,
     855,   855,   497,   498,  1085,  -744,   506,   508,   510,   512,
      73,   271,  1929,   517,   707,  1232,   525,   558,   559,   560,
      64,   532,   535,   226,   502,   858,  2229,  2230,  1212,   542,
     876,  1243,   808,  1973,  1974,  1877,  1304,    88,   811,  1864,
    1865,  1927,  1248,   151,  1280,    65,   226,   708,   709,  1086,
    1279,    56,   716,  2001,  2002,   763,  1587,  1263,   226,  1588,
    -749,  1262,   859,  1227,   992,  1657,   453,  1660,  -749,  -749,
    -749,  -749,   230,  1000,  1769,   408,  1261,   277,  1000,  1433,
     104,   431,  1035,   710,   225,   435,  1433,  -847,   806,  1001,
    1088,   294,  1434,  1035,  1035,   230,    56,   229,   735,  1711,
     458,   431,  -443,    67,    68,    69,  1274,   230,  2086,   877,
     426,  1301,  1256,  1781,  1384,   427,   711,  1427,   485,  1002,
     447,  1269,  1779,   736,  1002,  -751,   158,   159,   160,    57,
    1066,  1141,  -751,  -751,  -751,  -751,  1029,    56,   426,  -716,
    1258,   431,   524,   427,  1518,  1082,  1142,  1029,  1029,  1257,
    1999,   426,   539,  1135,   643,   408,   427,  -847,  1419,   545,
    1385,  1421,  1619,  1999,   225,   644,   645,   436,   437,   438,
     408,  -751,  2170,   415,  1769,  1769,  1810,   229,    57,   225,
    1586,   720,   721,  2090,   465,   466,  1305,  2300,  1299,    57,
     823,  1270,   229,   478,   854,   854,   565,   566,   860,   481,
     426,   221,   489,   490,  1862,   427,  1471,   221,   496,   226,
    1904,  -759,   221,   226,  1468,  1469,  1470,  1260,  -759,  -759,
    -759,  -759,  1473,  1294,     3,   527,  1878,     4,  1638,     5,
    2089,  1639,  1482,  -760,   540,   541,   970,  2364,   544,   561,
    -760,  -760,  -760,  -760,    57,  1450,  2221,   221,  1625,     6,
       7,     8,     9,   221,   221,  1058,   453,  -759,   230,  2365,
    1598,  1599,   230,   722,  1782,  1336,  2242,  1783,  2258,  2260,
    2259,     3,   312,   313,     4,  1932,     5,  2277,  1675,  -760,
    1474,  1414,     3,  2331,  2231,     4,   221,     5,  1090,    57,
     816,   431,   221,  1930,  1189,  -443,     6,     7,     8,     9,
    -253,   349,  2292,   272,   273,   274,   221,     6,     7,     8,
       9,  1999,  1672,  1758,  -275,   886,   418,  1151,  2087,  -744,
     309,  2159,  2280,  2185,   310,  1812,  1702,  2101,  1759,   423,
      57,  1044,   416,   335,   429,    56,  1156,  1143,  1703,  2313,
    2311,  1387,  1472,  2171,  1704,   226,  1760,  2312,  2152,  1700,
    1701,  1761,  1144,  1620,  1493,  2179,  1495,   300,  1456,   978,
     563,  1190,  1916,  1878,   543,  1917,  1372,  1373,  1919,   272,
     273,   274,  2177,   726,   819,   820,   679,  1103,   679,  1289,
     994,   680,   681,   680,   681,   440,  1145,  1784,  1272,  2178,
     310,   221,   232,  2085,   230,  2049,  1388,  1031,  1389,  1566,
     233,  1928,  1444,  1318,     4,   682,     5,   682,  1031,  1031,
    1417,   683,  1123,   683,  1432,   232,   684,   676,   684,  2082,
    2083,  -749,   685,   233,   685,   226,   686,   232,   686,  2105,
     677,  1214,  1213,  1446,   687,   233,   687,   158,   159,   160,
     226,  1273,  1488,  1044,  1044,  1044,  1044,  1044,  2102,   678,
    1044,  1658,  1666,  1667,   688,  1445,   688,  1462,  1890,  1673,
     730,   460,  2091,  1547,   499,  2150,  1431,   689,  1127,   689,
     608,   728,   849,   694,   230,   694,  1044,   695,   696,   695,
     696,   714,   971,   972,   973,  -751, -1017,   886, -1017,   230,
     794,   609,   426,   470,     4,  1044,     5,   427,   898,   846,
     235,   519,  2184,  1896,   610,   717,     3,   963,   964,     4,
    1580,     5,  1578,   714,  1355,  1785,  1561,  1334,  2268,   526,
     809,   528,  2266,   235,   879,  1575,  2267,  1356,    57,  1116,
    2265,     6,     7,     8,     9,   235,   847,   717,   745,  1319,
     731,   732,   733,  1057,  1357, -1018,  1618, -1018,  1279,  1316,
     714,   301,  1638,   302,  1390,  1639,  1646,   439,  1613,  2050,
    2212,  1328,  1413,   746,  1269,   714,     4,  1334,     5,   848,
     716,  -759,   546,   468,   717,   547,   548,   549,   232,   441,
     851,   442,   232,  1102,   225,  1585,   233,  1781,  1786,   717,
     233,   853,   492,  -760,   236,  1648,  1779,   229,  1265,     3,
    1649,  1714,     4,   868,     5,   971,   972,   973,   225,  1824,
    2225,  2351,  1128,  1129,   869,  1029,  1391,   236,  1429,   886,
    1950,   229,  1869,  1870,     6,     7,     8,     9,  1542,   236,
     886,   878,  1122,   640,  1369,  1834,  1368,   789,   881,   899,
     900,   901,   902,   903, -1017,  1543,  2051,   790,  1769,  1769,
     641,   817,   886,   237,   873,   874,   965,  1402,   500,   966,
     472,   882,   611,  1375,  1572,   307,   994,     3,   818,     4,
       4,     5,     5,   907,   992,   697,   237,   501,  1544,   349,
    1035,   997,   698,  2262,   221,   998,   235,   471,   237,   472,
     235,  1863,     6,     7,     8,     9,  1612,  1278,  1358,   720,
     721,  -571,  1048, -1018,  1801,  1275, -1017,  1782,  1800,  1342,
    1783,  1564,   699,  1562,   232,   238,   872,   999,   887,  2154,
    1758,  2123,   233,  1799,  2155,  1035,   221,  1029,  1029,  1029,
    1029,  1029,   225,  1049,  1029,  1759,   873,   874,   412,  1631,
     335,  1060,  1343,     3,  1915,   229,     4,  1382,     5,  1383,
     412,   157,  1652,  1760,  1632,   157,  1652,  1344,  1761,  1633,
    1029,  1789,  1653,   821,   994, -1018,  1656,  1758,     6,     7,
       8,     9,  1634,  1061,  2318,  1440,  2097,  1635,  1062,  1029,
     236,   722,  1759,  1782,   236,  1063,  1783,   335,  1441,  1698,
    1442,  1451,   669,  1064,   232,  1453,  1707,  1689,   652,  -347,
    1760,  1497,   233,  1345,   311,  1761,    18,    19,  2103,   232,
    1558,   886,  1808,  1820,  1951,  2215,  1952,   233,  1498,  1697,
    1699,  1601,   235,  1499,  1500,  -847,  1705,  1699,  1708,  1710,
    1784,   653,   654,  2216,   655,   494,   495,   647,  2217,   237,
    2218,   648,  1452,   237,  1442,   226,   670,   671,  1440,  2119,
    2120,  2121,  2122,  1099,   886,   672,    35,  1811,  1406,  -856,
    1494,  1452,  1496,  1442,  1798,  1440,    36,  1126,  1502,  1317,
    1716,  -857,    37,  2029,   649,   650,  1031,  1105,  1712,  1125,
    1442,  1044,  1044,  1044,  1044,  1044,  1044,  1044,  1044,  1044,
    1044,  1044,  1346,  1131,   230,     3,    38,  1133,     4,  2244,
       5,   238,   235,   157,  1652,   238,  1784,   568,   569,   570,
       3,   873,   874,     4,  1659,     5,   236,   235,   230,  1185,
       6,     7,     8,     9,  2137,  2175,  2176,  2128,     3,  1219,
    1501,     4,  1057,     5,  1289,     6,     7,     8,     9,     4,
       3,     5,  2135,     4,  1222,     5,  1519,   886,  1245,  1749,
    1520,  1521,  1522,     6,     7,     8,     9,   487,  1785,  2141,
     992,  1246,  2133,  1846,  1247,     6,     7,     8,     9,   320,
     321,  1253,    18,    19,   322,   237,  1255,  1403,   572,  1334,
     573,   574,   575,   576,  1980,   272,   273,   274,  1031,  1031,
    1031,  1031,  1031,   226,  1867,  1031,   236,  1290,  1750,  1751,
    1752,  1753,     3,  1754,   738,     4,  1755,     5,  1285,  1981,
    1982,   236,  2195,  2196,  2197,  1937,  2294,  2039,  1291,  2295,
     739,  1031,    35,  1756,  2040,  1900,   225,     6,     7,     8,
       9,  2017,    36,  1292,  1785,  1295,  1631,   412,    37,   229,
    1031,   740,   230,   158,   159,   160,   551,   552,   741,   801,
     802,  1632,  1296,  1903,  1964,   237,  1633,  1297,   992,   892,
     893,  2041,    38,  1676,   995,   996,  1933,  1934,  1300,  1634,
     237,  1308,  1309,  1757,  1635,     3,  1302,  1044,     4,  1312,
       5,  2061,  2063,  1925,  1325,  1926,  1529,  1530,  1531,  1532,
    1533,  1044,  2060,   985,   986,  2031,  2042,  1699,  1699,  1339,
       6,     7,     8,     9,  1989,  1990,  1991,  2022,  1992,  1375,
    1375,  1375,  1375,  1375,  1338,  1375,  1375,   412,     3,  1364,
    2081,     4,  1340,     5,  1682,  1682,  2068,  2069,  2070,  2071,
    2072,  2075,   412,  2078,  1983,  2032,  1349,   742,   743,   225,
    1350,  1044,  1351,     6,     7,     8,     9,  1371,   505,  1377,
    1378,  1318,   229,  1379,  1984,  1985,  1435,  1436,  1782,  1044,
    1044,  1783,  1380,    18,    19,  1029,  1029,  1029,  1029,  1029,
    1029,  1029,  1029,  1029,  1029,  1029,  1381,  -571,  2012,   875,
       4,   408,     5,  1386,  2133,  1394,  1758, -1062,   553,   554,
     225, -1063,   555,  1715,  1717,  1392,  1733,  1742,  1763,  1777,
    1393,  1759,  1395,   229,  1398,  2043,   335,  1718,  1397,  1734,
    1743,  1764,  1778,    35,   232,  1146,  1147,  1148,  1149,  1760,
    1407,  1809,   233,    36,  1761,  1511,  1512,  2057,  1405,    37,
    1438,  2095,  2096,  1437,  2098,   984,   985,   986,   232,  2180,
    2181,   795,  1439,   886,  1279,  1443,  1320,  1514,  1515,  2245,
     796,   797,  1449,    38,   798,   799,  1433,     3,   886,  2306,
       4,  2112,     5,   873,   874,  2114,  1817,  1818,  1815,  1816,
    1454,  2113,  1550,  1551,  1762,   634,   886,  1552,  1553,  2033,
    1057,  1784,     6,     7,     8,     9,  1463,   226,   886,  2345,
    1464,   635,  2044,   882,  1629,  1887,  1465,  1316,  1402,  1272,
     636,  1466,  2134,  1269,  1852,  2131,  1467,   637,   638,  1475,
    1835,  1825,  1826,  1842,   857,  1847,   157,  1652,  1853,  1976,
    1977,  1328,   235,  1836,  1478,   408,  1843,     3,  1848,  2111,
       4,  1854,     5,  2140,   225,  1476,   230,   133,   134,  1477,
     135,   136,   137,  1912,  2320,  2321,  1321,   229,  1655,  1655,
     886,  1655,     6,     7,     8,     9,   225,  1524,  1525,  1526,
    1527,  1029,   232,  2350,  1304,  1913,  2034,  1479,   408,   229,
     233,   158,   159,   160,  1480,  1029,  1481,   225,  1953,  1954,
    1955,  1956,  1684,  1685,  1911,  1483,  1486,   873,   874,  1490,
     229,   530,   143,  1601,   144,   971,   972,   973,   507,   145,
     226,   146,  1505,  1509,  1517,   147,    18,    19,  1507,  1785,
    1536,  1963,  1539,    18,    19,  1537,   236,  1538,  1540,  1548,
    2031,  1541,  1549,   886,  1556,  1029,  1031,  1031,  1031,  1031,
    1031,  1031,  1031,  1031,  1031,  1031,  1031,  1557,  1558,  2316,
     236,  1560,  1565,  1029,  1029,   148,  1570,   149,  2287,   230,
    -442,   226,  2286,  1567,  1289,  1568,    35,  2168,  1569,   600,
    2032,   601,  1577,    35,  1582,  1000,    36,  2285,  1600,  1608,
     235,  1610,    37,    36,  1957,   237,  1614,  1622,  1626,    37,
     602,   225,  2030,  1966,  1967,  1968,  1969,  1970,   603,   604,
     605,   606,  1972,  1647,   229,  2304,    38,  1662,  1665,   237,
     230,  1679,   225,    38,  1995,  1996,  1997,  1998,  1671,  2302,
    2303,  2279,   748,  1972,   408,   229,  1680,  1681,  2006,  2007,
    2008,  -715,  -716,   225,  1696,   408,   749,  1713,  1795,  1733,
    1742,  1763,  1777,   750,   225,  1796,   229,   238,  1797,  1806,
    1813,  1742,  1734,  1743,  1764,  1778,  1814,   229,  1822,  1833,
    1269,   751,  1859,  1044,  1743,  1304,  1866,  1879,   752,  1881,
    1883,   238,  1884,  1892,   236,  1898,   855,   855,  1899,  2035,
    2045,  2052,  1902,  1901,  2363,  1905,  1914,  1918,  2362,  1921,
     138,  1923,  2036,  2046,  2053,  1440,  1935,   139,   140,  1940,
    1942,  1942,  1945,  2361,  1936,   226,  1958,  1947,   225,  1979,
    2116,  2117,  2118,   141,  2033,   886,  2325,  1975,  2284,   533,
    1965,   229,   864,   871,  2003,  1888,  2009,  1317,  1403,  2023,
    2011,  2314,  1031,   237,    18,    19,  2010,  2025,  2027,  1289,
    2056,  2125,  2356,  2066,  -588,  2100,  1031,  2074,   226,  2076,
    2080,  2349,  2088,  -442,   230,  2092,  1655,  2355,   426,  1655,
    2301,  2093,  1655,   427,  2104,  2107,   232,  2109,  2115,   225,
    2127,  2128,  2133,  2151,   233,   830,   230,   831,  -442,   832,
     833,  2153,   229,  2143,    35,   886,   426,   834,   835,  2160,
    2162,   427,  2169,  2125,    36,   238,  1031,   230,  2165,  2173,
      37,  2147,  2174,  2183,  2189,  2190,     3,  2186,  1640,     4,
    2191,     5,  2208,  2192,  1031,  1031,   578,   579,  2241,  2253,
    2257,   836,  2129,  2130,    38,   580,   581,   582,   583,   584,
    2193,     6,     7,     8,     9,   324,   325,   326,  1893,  1894,
     329,  2198,  2136,  2199,  2360,  2200,  2201,  2157,  2278,  2243,
    2138,  2139,   226,  2255,  2142,  2261,  2269,  2163,     3,  2270,
    2146,     4,  2289,     5,  2166,  2271,  -292,  2035,  2045,  2052,
     854,   854,  2293,   226,   235,  2329,  2297,  1045,  2327,   232,
    2036,  2046,  2053,     6,     7,     8,     9,   233,  1045,  1045,
    2328,  2347,  -293,  1424,   226,  2039,  1423,  1422,   408,  1361,
    1251,   230,  2040,  1492,  1250,   226,  2206,   225,  1249,   474,
     475,   476,   477,  1140,  1807,  1252,  1136,  1426,    72,   814,
     229,   815,   230,  1068,  2194,  1790,  2235,  2319,  2005,  2272,
     232,  2211,  1719,  2210,  1736,  1747,  1767,  1780,   233,  2041,
    2226,  2274,  2273,   230,   225,  2275,  2264,  1029,  2348,  1484,
    1944,  1065,  2106,  1341,   230,   822,  2077,   229,  2188,  1354,
    2079,   158,   159,   160,  2094,  1067,  1396,  2263,   236,   226,
     225,  -442,  2305,  2207,  2042,     3,  1668,  1670,     4,   225,
       5,  2209,  1669,   229,  1404,  1687,  1886,   235,  1690,  1889,
    2224,  1322,   229,  2236,  2237,   852,  1487,  1709,   793,  1644,
       6,     7,     8,     9,  1868,  2246,  2247,  2065,  2248,  2249,
    2250,  2251,  2252,  1908,  1909,  2254,  1056,  1078,   230,     3,
    2353,  1080,     4,  1081,     5,   866,  1079,   237,  1216,   862,
     226,   867,  1188,  1535,  2296,  1182,     0,     3,   235,     0,
       4,     0,     5,     0,     6,     7,     8,     9,  1837,     0,
       0,  1844,     0,  1849,     0,   225,  1855,     0,     0,  2206,
       0,     0,     6,     7,     8,     9,  1972,     0,   229,  2288,
       0,     0,  2290,     0,   232,  2291,     0,  1782,     0,   230,
    1783,   236,   233,  2043,     0,     0,  1554,  1555,  2049,   238,
     813,     0,     0,     0,     0,     0,   232,     0,   408,   824,
     825,   826,   827,  2346,  1320,     0,     0,   225,     0,     0,
       0,     0,  2031,  1717,  1742,  1763,  1777,   232,  1640,     0,
     229,  1835,  1842,  1847,  1853,   233,  1718,  1743,  1764,  1778,
    2125,   225,   236,     0,  1836,  1843,  1848,  1854,     0,     0,
     237,  2326,  2324,  1607,   229,     0,     0,     0,     0,  2332,
       0,     0,  2032,     0,   886,  2333,  2334,  2335,  2336,  2337,
    2035,  2045,  2052,  2341,  2342,  2343,  2344,     0,   226,     0,
    2148,     0,     0,  2036,  2046,  2053,     0,     0,     0,     0,
    2354,   509,   235,     0,     0,     0,     0,  2357,  2358,  2359,
       0,   237,     0,     0,     0,     0,    18,    19,     0,     0,
    1784,     0,   238,     0,  1321,   226,   886,     0,  1031,     0,
       0,     3,     0,     0,     4,     0,     5,   230,     0,     0,
       0,   232,  2317,     0,     0,   235,     0,     0,     0,   233,
       0,   226,     0,     0,     0,     0,     6,     7,     8,     9,
     226,     0,   232,     0,     0,     0,    35,     0,     0,     0,
     233,     0,  2050,   412,   230,     0,    36,     0,     0,     0,
       0,     0,    37,   232,     0,     0,     0,  1736,  1747,  1767,
    1780,   233,     0,     0,   232,     0,   236,     0,     3,  1747,
     230,     4,   233,     5,     0,     3,    38,     0,     4,   230,
       5,     0,     0,     0,     0,     0,  2033,     0,   236,     0,
       0,     0,     0,     6,     7,     8,     9,  2037,  2047,  2054,
       6,     7,     8,     9,     0,     0,   226,     3,     0,   236,
       4,     0,     5,   886,  2049,     0,     0,  1804,  1785,   235,
    1805,     0,     0,     0,     0,   237,     0,     0,   232,  2149,
       0,     0,     6,     7,     8,     9,   233,     0,     0,     0,
     235,     0,     0,     0,     0,     0,  1045,   237,     0,     3,
       0,     0,     4,     0,     5,   230,     0,   886,   226,     0,
       0,   235,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,   235,  2338,     6,     7,     8,     9,     0,     0,
       0,     0,   226,     0,     0,  1721,     0,   412,     0,   232,
       0,  2310,     0,     0,  1376,  1722,  2039,   233,     0,   511,
    1723,  1829,  1724,  2040,  1725,     0,     0,   230,   520,   238,
     521,     0,     0,   236,    18,    19,     0,     0,     0,     0,
       0,     0,     0,    18,    19,     0,     0,     0,     0,     0,
     412,   230,     0,     0,   236,     0,   235,    90,     0,     0,
    2041,  1871,     0,  1872,  1873,    91,  1875,    92,     0,    93,
       0,     0,    94,    95,    96,   236,    97,     0,  1045,  1045,
    1045,  1045,  1045,     0,    35,  1045,   236,  -442,     0,     0,
       0,     0,   237,    35,    36,  2042,     0,     0,  2050,  1284,
      37,  1286,     0,    36,     0,  2037,  2047,  2054,     0,    37,
       0,  1045,     0,   237,     0,     0,     0,   235,  1303,     0,
    1307,     0,  1311,     0,    38,   713,     0,     0,  1323,  1324,
    1045,     0,  1726,    38,   237,     0,     0,   714,     0,   715,
       0,     0,     0,   623,   624,   237,   625,   232,    62,     0,
     236,   516,     0,     0,   238,   233,     0,   716,     0,     0,
    1359,   717,     0,  1363,   718,   626,    18,    19,     0,   886,
       0,     0,     0,   627,   628,   238,   629,     0,   303,   304,
     305,   306,     0,     0,   232,  2340,   630,   631,   632,     0,
       0,     0,   233,     0,     0,     0,   412,     0,  1272,     0,
       0,     0,     0,     0,  2043,  1272,     0,   412,   657,   237,
     232,   236,     0,   433,     0,     0,    35,   658,   233,   232,
       0,   443,   444,   445,   446,     0,    36,   233,  1400,     0,
       0,     0,    37,   659,     0,     0,   886,   660,     0,     0,
       0,   661,   662,   886,   479,  1727,   663,   664,   665,   666,
     667,     0,  1841,   719,     0,   235,    38,     0,     0,  2315,
    1728,   504,     0,     0,     0,     0,   513,   514,   515,     0,
     237,   238,     0,     0,     0,   886,   529,     0,  1729,   536,
    -442,     0,     0,  1730,     0,  1731,   720,   721,     0,     0,
       0,  2339,   235,     0,     0,   232,     0,    98,  1447,  1448,
       0,     0,     0,   233,     0,     0,     0,     0,     0,     0,
       0,     0,    99,     0,     0,     0,     0,   886,   235,     0,
       0,     0,     0,     0,     0,     0,     0,   235,     0,  2067,
     100,     0,   238,  1732,     0,   101,     0,   102,     0,     3,
       0,     0,     4,     0,     5,     0,     0,   232,     0,   236,
       0,  1719,  1747,  1767,  1780,   233,     0,     0,     0,  1837,
    1844,  1849,  1855,     0,     6,     7,     8,     9,   722,     0,
       0,   232,     0,     0,     0,     0,     0,     0,  1749,   233,
       0,     0,     0,     0,     0,     0,   236,     0,     0,     0,
       0,     0,     3,     0,     0,     4,     0,     5,  2037,  2047,
    2054,     0,     0,   235,     0,     0,     0,     0,   237,     0,
       0,     0,   236,     0,     0,     0,     0,     6,     7,     8,
       9,   236,     0,     0,     0,     0,     0,  1750,  1751,  1752,
    1753,  1770,  1754,  1164,     0,  1755,     0,     0,   838,   839,
     840,     0,     0,     0,     0,   237,     0,     0,  1165,     0,
       0,     0,  1756,     0,     0,   235,     0,     0,     0,     0,
    1376,  1376,  1376,  1376,  1376,     0,  1376,  1376,     0,     0,
     412,   237,     0,     0,     0,  1683,  1683,     0,     0,   235,
     237,  1771,  1772,  1773,     0,  1754,     0,     0,  1755,  2158,
       0,     0,     0,    75,     0,     0,     0,   236,     0,  2164,
       0,    76,  1757,    77,     0,  1774,  2167,   238,    78,    79,
      80,     0,    81,  1166,     0,     0,  1045,  1045,  1045,  1045,
    1045,  1045,  1045,  1045,  1045,  1045,  1045,     0,     0,     0,
       0,  1581,     0,   238,     0,  1584,     0,     0,     0,     0,
       0,  2172,   238,     0,     0,  1604,  1605,  1606,     0,   236,
    1609,     0,  1611,     0,     0,  1775,   237,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   236,     0,  1627,     0,     0,  1074,  1075,
    1076,     0,     0,     0,     0,  1167,     0,     0,  1168,  1169,
    1170,  1171,  1172,  1173,  1174,  1175,  1176,  1177,  1178,  1179,
    1180,     0,  2238,  2239,  2240,     0,     0,     0,   237,     0,
       0,  1092,  1661,     0,  1663,  1758,     0,     0,   238,  1233,
    1234,  1664,     0,     0,     0,     0,     0,     0,  2256,     0,
    1759,     0,   237,  1235,     3,   335,  1677,     4,  1678,     5,
    1104,  1236,     0,     0,     0,     0,     0,  1686,  1760,  1688,
       0,     0,     0,  1761,     0,     0,     0,  1237,     0,     6,
       7,     8,     9,     0,     0,     0,     0,     0,  1758,     0,
     412,     0,     0,  1749,     0,     0,     0,     0,   613,   614,
       0,     0,     0,  1759,     0,     0,     0,   886,   335,     0,
       0,     0,   280,   615,   238,   616,   617,   618,     0,     0,
     281,  1760,   282,  2015,   283,     0,  1761,   284,   285,   286,
       0,   287,  1045,     0,     0,     0,     0,     0,   619,   620,
     621,     0,  1750,  1751,  1752,  1753,  1045,  1754,     0,     0,
    1755,     0,     0,    82,     0,     0,     0,     0,  2323,     0,
     886,     0,     0,     0,     0,     0,     0,  1756,    83,    51,
       0,     0,     0,     0,     0,     0,  1776,     0,     0,    63,
      51,    51,    66,    66,    66,    70,    84,     0,    51,     0,
       0,    85,     0,    86,     0,     0,  1045,     0,     0,     0,
       0,     0,     0,     0,     0,  1238,  1239,  1240,   772,     0,
       0,     0,     0,     0,  1045,  1045,     0,  1757,     0,     0,
       0,   782,   783,   784,     0,     0,     0,     0,     0,     0,
       0,     0,  2366,  2367,  2368,     0,     0,     0,     0,     0,
       0,    51,     0,     0,   434,    51,    66,    66,    66,    70,
       0,     0,  1857,  1858,     0,     0,    51,     0,   449,   454,
     457,    51,   462,    66,    66,   467,     0,   449,   449,   449,
     449,     0,    66,     0,     0,     0,   483,     0,    51,     0,
    1874,    66,    66,     0,    70,     0,     0,    66,     0,  1592,
       0,     0,     0,     0,     0,     0,  1891,     0,     0,     0,
       0,    51,   523,   462,    66,   462,     0,   531,   534,     0,
       0,   538,    51,    66,    66,     0,     0,    66,     0,    51,
       0,   586,   587,     0,     0,   588,     0,     0,     0,     0,
    1758,     0,  1920,     0,     0,     0,   589,   590,   591,   592,
     593,   594,   595,   596,   597,  1759,     0,     0,     0,  1922,
     335,  1924,   288,     0,  1166,     0,     0,     0,     0,     0,
       0,     0,     0,  1760,     0,   911,     0,   289,  1761,     0,
       0,   912,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1259,     0,     0,     0,   290,   598,     0,     0,     0,
     291,     0,   292,     0,     0,     0,     0,     0,     0,   158,
     159,   160,   886,     0,     0,     0,  1010,     0,     0,     0,
    1011,     0,     0,     0,     0,  1012,     0,     0,  2308,     0,
       0,  1013,     0,     0,     0,   919,  1593,     0,     0,  1168,
    1169,  1594,  1171,  1172,  1173,  1174,  1175,  1176,  1177,  1178,
    1179,  1595,     0,     0,     0,  1016,     0,     0,   449,   454,
     457,    51,   462,    66,    66,   467,     0,   449,   449,   449,
     449,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,     0,     0,     4,     0,     5,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     6,     7,     8,     9,   154,
     155,     0,     0,     0,   317,   156,   318,     0,     0,     0,
       0,     0,     0,  1737,     0,     0,     0,     0,     0,     0,
       0,   319,     0,     0,     0,     0,     0,     0,     0,  2064,
       0,   320,   321,     0,     0,     0,   322,     0,     0,   323,
       0,     0,     0,     0,     0,     0,     0,     0,   324,   325,
     326,   327,   328,   329,     0,     0,     0,     0,     0,     0,
       0,     0,  2084,  1738,     0,     0,     0,   330,     0,   331,
       0,     0,   157,   158,   159,   160,     0,     0,   161,     0,
     162,     0,   922,  1739,   163,   923,   924,     0,     0,   164,
       0,  2099,   925,     0,     0,   165,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1045,     0,
       0,   166,     0,     0,     0,     0,   167,     0,     0,   168,
       0,     0,   927,   928,   169,     0,     0,     0,   170,     0,
       0,   171,   172,  1740,     0,     0,   173,     0,     0,   174,
       0,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1101,     0,     0,     0,     0,
       0,     0,    66,     0,   221,     0,   176,   177,     0,   178,
     179,   180,   181,  1107,     0,  1108,     0,  1109,     0,  1110,
       0,     0,     0,     0,  1114,     0,     0,     0,  1115,    51,
       0,     0,     0,     0,     0,     0,     0,     0,  1119,     0,
       0,  1120,     0,     0,     0,     0,     0,   182,   183,   184,
     185,    66,     0,     0,     0,     0,   186,   187,     0,     0,
     188,   189,   332,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,     0,     0,   333,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   334,     0,     0,     0,     0,   335,     0,     0,     0,
    2182,     0,     0,   336,     0,     0,     0,     0,     0,   337,
       0,     0,     0,     0,   338,     0,     0,     0,     0,     0,
     213,   214,     0,   339,     0,     0,   340,   341,   342,   343,
       0,     0,     0,   344,     0,     0,     0,   345,   346,   215,
       0,     0,     0,   216,   217,     0,     0,     0,   886,     0,
       0,     0,   347,     0,     3,   218,   219,     4,     0,     5,
       0,   348,     0,   349,  1741,   220,     0,     0,   221,     0,
       0,     0,     0,     0,     0,     0,   153,     0,     0,     6,
       7,     8,     9,   154,   155,     0,     0,     0,   317,   156,
     318,     0,     0,     0,     0,     0,     0,  1737,     0,     0,
       0,     0,     0,     0,     0,   319,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   320,   321,     0,     0,     0,
     322,     0,     0,   323,     0,     0,     0,     0,     0,     0,
       0,     0,   324,   325,   326,   327,   328,   329,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1738,     0,     0,
       0,   330,     0,   331,     0,     0,   157,   158,   159,   160,
       0,     0,   161,     0,   162,     0,     0,  1739,   163,     0,
       0,     3,     0,   164,     4,     0,     5,     0,     0,   165,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   166,     6,     7,     8,     9,
     167,     0,     0,   168,  1191,     0,     0,     0,   169,     0,
    1770,     0,   170,     0,     0,   171,   172,  1740,     0,     0,
     173,     0,     0,   174,     0,   175,     0,  1192,  1193,  1194,
    1195,  1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,
    1205,  1206,  1207,  1208,     0,     0,     0,     0,     0,     0,
     176,   177,     0,   178,   179,   180,   181,     0,     0,     0,
    1771,  1772,  1773,     0,  1754,     0,     0,  1755,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1774,     0,     0,     0,     0,     0,
       0,   182,   183,   184,   185,     0,     0,     0,     0,     0,
     186,   187,     0,     0,   188,   189,   332,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,     0,     0,
     333,     0,     0,     0,  1775,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   334,     0,     0,     0,     0,
     335,     0,     0,     0,     0,     0,     0,   336,     0,     0,
       0,     0,     0,   337,     0,     0,     0,     0,   338,     0,
       0,     0,     0,     0,   213,   214,     0,   339,     0,     0,
     340,   341,   342,   343,     0,     0,     0,   344,     0,     0,
       0,   345,   346,   215,     0,     0,     0,   216,   217,     0,
       0,     0,   886,     0,     0,     0,   347,     0,     3,   218,
     219,     4,     0,     5,     0,   348,     0,   349,  2014,   220,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
     153,     0,     0,     6,     7,     8,     9,   154,   155,     0,
       0,     0,   317,   156,   318,     0,     0,  1758,     0,     0,
       0,  1737,     0,     0,     0,     0,     0,     0,     0,   319,
       0,     0,  1759,     0,     0,     0,     0,   335,     0,   320,
     321,     0,     0,     0,   322,     0,     0,   323,     0,     0,
    1760,     0,     0,     0,     0,  1761,   324,   325,   326,   327,
     328,   329,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1738,     0,  1485,     0,   330,     0,   331,     0,     0,
     157,   158,   159,   160,     0,     0,   161,     0,   162,   886,
       0,  1739,   163,     0,     0,     3,     0,   164,     4,     0,
       5,     0,     0,   165,     0,  2016,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   166,
       6,     7,     8,     9,   167,     0,     0,   168,     0,     0,
       0,     0,   169,     0,  1770,     0,   170,     0,     0,   171,
     172,  1740,     0,     0,   173,     0,     0,   174,     0,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,     0,   178,   179,   180,
     181,     0,     0,     0,  1771,  1772,  1773,     0,  1754,     0,
       0,  1755,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1774,     0,
       0,     0,     0,     0,     0,   182,   183,   184,   185,     0,
       0,     0,     0,     0,   186,   187,     0,     0,   188,   189,
     332,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,     0,     0,   333,     0,     0,     0,  1775,     0,
       0,     0,     0,     0,     0,     0,     0,    10,     0,   334,
       0,     0,     0,     0,   335,    11,     0,    12,     0,    13,
       0,   336,    14,    15,    16,     0,    17,   337,     0,     0,
      18,    19,   338,     0,     0,     0,     0,     0,   213,   214,
       0,   339,     0,     0,   340,   341,   342,   343,     0,     0,
       0,   344,     0,     0,     0,   345,   346,   215,     0,     0,
       0,   216,   217,     0,     0,     0,   886,     0,     0,     0,
     347,     0,     3,   218,   219,     4,     0,     5,     0,   348,
      35,   349,  2021,   220,     0,     0,   221,     0,     0,     0,
      36,     0,     0,     0,   153,     0,    37,     6,     7,     8,
       9,   154,   155,     0,     0,     0,   317,   156,   318,     0,
       0,  1758,     0,     0,     0,  1737,     0,     0,     0,     0,
      38,     0,     0,   319,     0,     0,  1759,     0,     0,     0,
       0,   335,     0,   320,   321,     0,     0,     0,   322,     0,
       0,   323,     0,     0,  1760,     0,     0,     0,     0,  1761,
     324,   325,   326,   327,   328,   329,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1738,     0,     0,     0,   330,
       0,   331,     0,     0,   157,   158,   159,   160,     0,     0,
     161,     0,   162,   886,     0,  1739,   163,     0,     0,     0,
       0,   164,     0,     0,     0,     0,     0,   165,     0,  2309,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,     0,   167,     0,
       0,   168,     0,     0,     3,     0,   169,     4,     0,     5,
     170,     0,     0,   171,   172,  1740,     0,     0,   173,     0,
       0,   174,     0,   175,     0,     0,     0,    39,     0,     6,
       7,     8,     9,     0,     0,     0,     0,     0,     0,     0,
    1721,     0,    40,     0,     0,     0,     0,     0,   176,   177,
    1722,   178,   179,   180,   181,  1723,     0,  1724,     0,  1725,
      41,     0,     0,     0,     0,    42,     0,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   182,
     183,   184,   185,     0,     0,     0,     0,     0,   186,   187,
       0,     0,   188,   189,   332,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,     0,     0,   333,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   334,     0,     0,     0,     0,   335,     0,
       0,     0,     0,     0,     0,   336,     0,     0,     0,     0,
       0,   337,     0,     0,     0,     0,   338,  1726,     0,     0,
       0,     0,   213,   214,     0,   339,     0,     0,   340,   341,
     342,   343,     0,     0,     0,   344,     0,     0,     0,   345,
     346,   215,     4,     0,     5,   216,   217,     0,     0,     0,
     886,     0,     0,     0,   347,     0,     0,   218,   219,     0,
       0,   153,     0,   348,     0,   349,  2307,   220,   154,   155,
     221,     0,     0,   317,   156,   318,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     319,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     320,   321,     0,     0,   754,   322,     0,     0,   323,     0,
       0,     0,     0,     0,     0,     0,     0,   324,   325,   326,
     327,   328,   329,     0,     0,     0,     0,     0,     0,     0,
    1727,   755,     0,   161,   756,     0,   330,     0,   331,     0,
       0,   157,   158,   159,   160,  1728,     0,   161,     0,   162,
     165,   757,     0,   163,     0,     0,     0,     0,   164,     0,
       0,     0,     0,  1729,   165,     0,   166,     0,  1730,     0,
    1731,   167,     0,     0,   168,     0,     0,     0,     0,   169,
     166,     0,     0,   170,     0,   167,   171,   172,   168,     0,
       0,   173,     0,   169,   174,     0,   175,   170,     0,     0,
     171,   172,   886,     0,     0,   173,     0,     0,   174,     0,
     175,     0,     0,     0,     0,     0,     0,     0,  2013,     0,
       0,   176,     0,     0,     0,     0,     0,   181,     0,     0,
       0,     0,     0,     0,     0,   176,   177,     0,   178,   179,
     180,   181,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,     0,   182,   183,   184,   185,
       0,     0,     0,     0,     0,   186,   187,     0,     0,   188,
     189,   332,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,     0,     0,   333,     0,   758,     0,   910,
       0,   213,   214,     0,   764,   765,     0,     0,   766,     0,
     334,   759,     0,     0,     0,   335,     0,   153,     0,     0,
       0,     0,   336,     0,     0,   155,   767,     0,   337,     0,
     156,     0,     0,   338,     0,     0,     0,   760,     0,   213,
     214,     0,   339,     0,     0,   340,   341,   342,   343,     0,
       0,     0,   344,     0,     0,     0,   345,   346,   215,     0,
       0,     0,   216,   217,   911,     0,     0,     0,     0,     0,
     912,   347,     0,     0,   218,   219,     0,     0,     0,   913,
     348,   914,   349,  1910,   220,     0,     0,   221,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   157,   158,   159,
     160,     0,     0,   161,     0,   915,     0,     0,     0,   916,
       0,     0,     0,     0,   917,     0,     0,     0,     0,     0,
     918,     0,     0,     0,   919,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   166,     0,     0,     0,
       0,   167,     0,     0,   920,     0,   768,     0,     0,   169,
       0,     0,     0,   170,     0,     0,   171,   172,     0,     0,
       0,   173,     0,     0,   174,     0,   175,     0,     0,     0,
     769,   770,   771,   772,   773,   774,     0,   775,   776,   777,
     778,   779,   780,   781,     0,     0,   782,   783,   784,     0,
       0,   176,   177,   863,   178,   179,   180,   181,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1281,   785,
       0,     0,     0,     0,     0,   764,   765,     0,     0,   766,
       0,     0,     0,     0,     0,     0,   153,     0,     0,     0,
       0,     0,   921,     0,   155,     0,     0,   767,     0,   156,
       0,     0,     0,     0,     0,     0,     0,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,     0,
       0,   212,     0,   911,     0,     0,     0,     0,     0,   912,
       0,   922,     0,     0,   923,   924,     0,     0,     0,     0,
       0,   925,     0,     0,   864,     0,     0,     0,     0,   926,
       0,     0,     0,     0,     0,     0,   157,   158,   159,   160,
       0,     0,   161,     0,   915,   213,   214,     0,   916,     0,
       0,   927,   928,   917,     0,     0,     0,     0,     0,  1282,
       0,     0,     0,   919,   215,     0,     0,     0,   216,   217,
       0,     0,     0,     0,     0,   166,     0,     0,   929,   930,
     167,   760,     0,  1283,     0,     0,     0,   768,   169,     0,
     931,     0,   170,   221,     0,   171,   172,     0,     0,     0,
     173,     0,     0,   174,     0,   175,     0,     0,     0,     0,
       0,   769,   770,   771,   772,   773,   774,   865,   775,   776,
     777,   778,   779,   780,   781,     0,     0,   782,   783,   784,
     176,   177,  2281,   178,   179,   180,   181,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1362,     0,     0,
     785,     0,     0,     0,   764,   765,     0,     0,   766,     0,
       0,     0,     0,     0,     0,   153,     0,     0,     0,     0,
       0,   921,     0,   155,     0,     0,   767,     0,   156,     0,
       0,     0,     0,     0,     0,     0,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,     0,     0,
     212,     0,   911,     0,     0,     0,     0,     0,   912,     0,
     922,     0,     0,   923,   924,     0,     0,     0,     0,     0,
     925,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,     0,
       0,   161,     0,   915,   213,   214,     0,   916,     0,     0,
     927,   928,   917,     0,     0,     0,     0,     0,  1282,     0,
       0,     0,   919,   215,     0,     0,     0,   216,   217,     0,
       0,     0,     0,     0,   166,     0,     0,   929,   930,   167,
     760,     0,  1283,     0,     0,     0,   768,   169,     0,   931,
       0,   170,   221,     0,   171,   172,     0,     0,     0,   173,
       0,     0,   174,     0,   175,     0,     0,     0,     0,     0,
     769,   770,   771,   772,   773,   774,  2282,   775,   776,   777,
     778,   779,   780,   781,     0,     0,   782,   783,   784,   176,
     177,     0,   178,   179,   180,   181,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1399,     0,     0,   785,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   153,     0,     0,     0,     0,     0,
     921,     0,   155,     0,     0,     0,     0,   156,     0,     0,
       0,     0,     0,     0,     0,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,     0,     0,   212,
       0,   911,     0,     0,     0,     0,     0,   912,     0,   922,
       0,     0,   923,   924,     0,     0,     0,     0,     0,   925,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   157,   158,   159,   160,     0,     0,
     161,     0,   915,   213,   214,     0,   916,     0,     0,   927,
     928,   917,     0,     0,   161,     0,   162,  1282,     0,     0,
     163,   919,   215,     0,     0,   164,   216,   217,     0,     0,
       0,   165,     0,   166,     0,     0,   929,   930,   167,   760,
       0,  1283,     0,     0,     0,     0,   169,   166,   931,     0,
     170,   221,   167,   171,   172,   168,     0,     0,   173,     0,
     169,   174,     0,   175,   170,     0,     0,   171,   172,   674,
       0,     0,   173,     0,     0,   174,     0,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
       0,   178,   179,   180,   181,     0,     0,     0,     0,     0,
       0,     0,   176,     0,   675,  1583,     0,     0,   181,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   153,     0,     0,     0,     0,     0,   921,
       0,   155,     0,     0,     0,     0,   156,     0,     0,     0,
       0,     0,     0,     0,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,     0,     0,   212,     0,
     911,     0,     0,     0,     0,     0,   912,     0,   922,     0,
       0,   923,   924,     0,     0,     0,     0,     0,   925,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   157,   158,   159,   160,     0,     0,   161,
       0,   915,   213,   214,     0,   916,     0,     0,   927,   928,
     917,     0,     0,   161,     0,   162,  1282,     0,   703,   163,
     919,   215,     0,     0,   164,   216,   217,     0,     0,     0,
     165,     0,   166,     0,     0,   929,   930,   167,   760,     0,
    1283,     0,     0,     0,     0,   169,   166,   931,     0,   170,
     221,   167,   171,   172,   168,     0,     0,   173,     0,   169,
     174,     0,   175,   170,     0,     0,   171,   172,     0,     0,
       0,   173,     0,     0,   174,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,   177,     0,
     178,   179,   180,   181,     0,     0,     0,     0,     0,     0,
       0,   176,     0,   704,     0,     0,     0,   181,  1003,  1004,
    1005,  1006,  1007,  1008,     0,     0,     0,     0,  1009,     0,
       0,     0,   991,     0,     0,     0,     0,     0,   921,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,     0,     0,   212,     0,   911,
       0,     0,     0,     0,     0,   912,     0,   922,     0,     0,
     923,   924,     0,     0,     0,     0,     0,   925,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   157,   158,   159,   160,     0,     0,     0,     0,
    1010,   213,   214,     0,  1011,     0,     0,   927,   928,  1012,
       0,  1327,     0,     0,     0,  1013,     0,     0,     0,   919,
     215,     0,     0,     0,   216,   217,     0,     0,     0,     0,
       0,  1014,     0,     0,   929,   930,  1015,   760,     0,  1016,
       0,     0,     0,     0,  1017,     0,   931,     0,  1018,   221,
       0,  1019,  1020,     0,     0,     0,  1021,     0,     0,  1022,
     911,  1023,     0,     0,     0,     0,   912,     0,     0,     0,
       0,     0,     0,   161,     0,   162,     0,     0,     0,   163,
       0,     0,     0,     0,   164,     0,  1024,     0,     0,  1025,
     165,     0,  1026,   157,   158,   159,   160,     0,     0,     0,
       0,  1010,     0,     0,     0,  1011,   166,     0,     0,     0,
    1012,   167,     0,     0,   168,     0,  1013,     0,     0,   169,
     919,     0,     0,   170,     0,     0,   171,   172,     0,     0,
       0,   173,  1014,     0,   174,     0,   175,  1015,     0,     0,
    1016,     0,     0,     0,     0,  1017,     0,     0,     0,  1018,
       0,     0,  1019,  1020,     0,     0,     0,  1021,     0,     0,
    1022,   176,  1023,     0,     0,     0,     0,   181,     0,     0,
       0,     0,     0,     0,     0,     0,   922,     0,     0,   923,
     924,     0,     0,     0,     0,  1327,   925,  1024,     0,     0,
    1025,     0,     0,  1026,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   927,   928,     0,  1027,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   911,     0,     0,     0,     0,     0,
     912,     0,     0,   929,   930,     0,     0,     0,     0,     0,
       0,     0,     0,   349,     0,  1028,     0,     0,   221,     0,
       0,     0,     0,     0,     0,     0,     0,   157,   158,   159,
     160,     0,     0,     0,     0,  1010,     0,   922,     0,  1011,
     923,   924,     0,     0,  1012,     0,     0,   925,     0,     0,
    1013,     0,  1050,   991,   919,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1014,     0,     0,     0,
       0,  1015,     0,     0,  1016,     0,     0,   927,   928,  1017,
    1027,     0,     0,  1018,     0,     0,  1019,  1020,     0,     0,
       0,  1021,     0,     0,  1022,     0,  1023,     0,     0,     0,
     911,     0,     0,     0,   929,   930,   912,     0,     0,     0,
       0,     0,     0,     0,   349,     0,  1028,     0,     0,   221,
       0,  1024,     0,     0,  1025,     0,     0,  1026,     0,     0,
       0,     0,     0,   157,   158,   159,   160,     0,     0,     0,
       0,  1010,     0,     0,     0,  1011,     0,     0,     0,     0,
    1012,     0,     0,     0,     0,     0,  1013,     0,     0,     0,
     919,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1014,     0,     0,     0,     0,  1015,     0,     0,
    1016,     0,     0,     0,     0,  1017,     0,     0,     0,  1018,
       0,     0,  1019,  1020,     0,     0,     0,  1021,     0,     0,
    1022,     0,  1023,     0,     0,     0,     0,     0,     0,     0,
       0,   922,     0,     0,   923,   924,     0,     0,     0,     0,
       0,   925,     0,     0,     0,     0,     0,  1024,     0,     0,
    1025,     0,     0,  1026,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   927,   928,     0,  1027,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   929,   930,
       0,     0,     0,     0,   153,     0,     0,     0,     0,     0,
    1028,   154,   155,   221,     0,     0,   317,   156,   318,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   319,     0,     0,     0,   922,     0,     0,
     923,   924,     0,   320,   321,     0,     0,   925,   322,     0,
       0,   323,     0,     0,     0,     0,     0,     0,     0,     0,
     324,   325,   326,   327,   328,   329,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   927,   928,   330,
    1027,   331,     0,     0,   157,   158,   159,   160,     0,     0,
     161,     0,   162,     0,     0,     0,   163,     0,     0,     0,
       0,   164,     0,     0,   929,   930,     0,   165,     0,     0,
       0,     0,     0,     0,  1051,     0,  1028,     0,     0,   221,
       0,     0,     0,   166,     0,     0,     0,     0,   167,     0,
       0,   168,     0,     0,     0,     0,   169,     0,     0,     0,
     170,     0,     0,   171,   172,     0,     0,     0,   173,     0,
       0,   174,     0,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
       0,   178,   179,   180,   181,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   106,     0,     0,     0,
       0,   107,     0,     0,   108,   109,   110,   111,     0,     0,
     112,   113,     0,   114,   115,   116,     0,   117,     0,   182,
     183,   184,   185,     0,     0,     0,     0,     0,   186,   187,
       0,     0,   188,   189,   332,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,     0,   118,   333,   119,
     120,   121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   334,     0,   153,     0,     0,   335,     0,
       0,     0,   154,   155,     0,   336,     0,     0,   156,  2212,
       0,   337,     0,     0,     0,     0,   338,     0,     0,     0,
       0,     0,   213,   214,     0,   339,     0,     0,   340,   341,
     342,   343,     0,     0,     0,   344,     0,     0,     0,   345,
     346,   215,     0,     0,     0,   216,   217,     0,     0,     0,
       0,     0,     0,     0,   347,     0,     0,   218,   219,  2213,
       0,     0,     0,   348,     0,   349,     0,   220,     0,     0,
     221,     0,     0,     0,     0,   157,   158,   159,   160,     0,
       0,   161,     0,   162,     0,     0,     0,   163,     0,     0,
       0,     0,   164,     0,     0,     0,     0,     0,   165,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   166,     0,     0,     0,     0,   167,
       0,     0,   168,     0,     0,     0,     0,   169,     0,     0,
       0,   170,     0,     0,   171,   172,     0,     0,     0,   173,
       0,     0,   174,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   122,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
     177,   123,   178,   179,   180,   181,     0,     0,   124,   125,
     126,   127,     0,     0,     0,     0,     0,     0,     0,   128,
       0,     0,     0,     0,   129,     0,   130,   131,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     182,   183,   184,   185,     0,     0,     0,     0,     0,   186,
     187,     0,     0,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,     0,     0,  2214,
       0,     0,     0,     0,   153,     0,     0,     0,     0,     0,
       0,     0,   155,     0,  2215,     0,     0,   156,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  2216,     0,     0,     0,     0,  2217,     0,  2218,
       0,     0,     0,   213,   214,     0,     0,     0,     0,     0,
       0,   911,     0,     0,     0,     0,     0,   912,     0,     0,
       0,     0,   215,     0,     0,     0,   216,   217,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   218,   219,
       0,     0,     0,     0,   157,   158,   159,   160,   220,     0,
     161,   221,   915,     0,     0,     0,   916,     0,     0,     0,
       0,   917,     0,     0,     0,     0,     0,  1282,     0,     0,
       0,   919,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,     0,   167,     0,
       0,  1283,     0,     0,     0,     0,   169,     0,     0,     0,
     170,     0,     0,   171,   172,     0,     0,     0,   173,     0,
       0,   174,     0,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
       0,   178,   179,   180,   181,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   153,     0,     0,     0,     0,     0,   921,
     154,   155,     0,     0,     0,     0,   156,     0,     0,     0,
       0,     0,     0,     0,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,     0,     0,   212,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   922,     0,
       0,   923,   924,     0,     0,     0,     0,     0,   925,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   157,   158,   159,   160,     0,     0,   161,
       0,   162,   213,   214,     0,   163,     0,     0,   927,   928,
     164,     0,     0,     0,     0,     0,   165,     0,     0,     0,
       0,   215,     0,     0,     0,   216,   217,     0,     0,     0,
       0,     0,   166,     0,     0,   929,   930,   167,   760,     0,
     168,     0,     0,     0,     0,   169,     0,   931,     0,   170,
     221,     0,   171,   172,     0,     0,     0,   173,     0,     0,
     174,     0,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,   177,   153,
     178,   179,   180,   181,     0,     0,   154,   155,     0,     0,
       0,     0,   156,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   182,   183,
     184,   185,     0,     0,     0,     0,     0,   186,   187,     0,
       0,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,     0,     0,   212,     0,   157,
     158,   159,   160,     0,     0,   161,     0,   162,     0,     0,
       0,   163,     0,     0,     0,     0,   164,     0,     0,     0,
       0,     0,   165,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   166,     0,
       0,   213,   214,   167,     0,     0,   168,     0,     0,     0,
       0,   169,     0,     0,     0,   170,     0,     0,   171,   172,
     215,     0,     0,   173,   216,   217,   174,     0,   175,     0,
       0,     0,     0,     0,     0,     0,   218,   219,     0,     0,
       0,     0,     0,     0,   349,     0,   220,     0,     0,   221,
       0,     0,     0,   176,   177,     0,   178,   179,   180,   181,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   991,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   182,   183,   184,   185,     0,     0,
       0,     0,     0,   186,   187,     0,     0,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   911,     0,   212,     0,     0,     0,   912,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   157,   158,   159,   160,     0,     0,
       0,     0,  1010,     0,     0,     0,  1011,   213,   214,     0,
       0,  1012,     0,     0,     0,     0,     0,  1013,     0,     0,
       0,   919,     0,     0,     0,     0,   215,     0,     0,     0,
     216,   217,     0,  1014,     0,     0,     0,     0,  1015,     0,
       0,  1016,   218,   219,     0,     0,  1017,     0,     0,     0,
    1018,     0,   220,  1019,  1020,   221,     0,     0,  1021,     0,
       0,  1022,     0,  1023,     0,     0,     0,     0,     0,     0,
       2,     3,     0,     0,     4,     0,     5,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1024,     0,
       0,  1025,     0,     0,  1026,     0,     6,     7,     8,     9,
       0,     0,     0,     0,     0,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,    11,     0,    12,     0,    13,
       0,     0,    14,    15,    16,     0,    17,     0,     0,     0,
      18,    19,    20,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -442,     0,     0,     0,   922,     0,
      35,   923,   924,     0,     0,     0,     0,     0,   925,     0,
      36,   911,     0,     0,     0,     0,    37,   912,     0,     0,
     324,   325,   326,  1893,  1894,   329,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   927,   928,
      38,  1027,     0,     0,   157,   158,   159,   160,     0,     0,
       0,     0,  1010,     0,     0,     0,  1011,     0,     0,     0,
       0,  1012,     0,     0,     0,   929,   930,  1013,     0,     0,
       0,   919,     0,     0,     0,   349,     0,  1028,     0,     0,
     221,     0,     0,  1014,     0,     0,     0,     0,  1015,     0,
       0,  1016,   911,     0,     0,     0,  1017,     0,   912,     0,
    1018,     0,     0,  1019,  1020,     0,     0,     0,  1021,     0,
       0,  1022,     0,  1023,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,     0,
       0,     0,     0,  1010,     0,     0,     0,  1011,  1024,     0,
       0,  1025,  1012,     0,  1026,     0,     0,     0,  1013,     0,
       0,     0,   919,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1014,     0,     0,    39,     0,  1015,
       0,     0,  1016,     0,     0,     0,     0,  1017,     0,     0,
       0,  1018,    40,     0,  1019,  1020,     0,  -442,     0,  1021,
       0,     0,  1022,     0,  1023,     0,     0,     0,     0,     0,
      41,     0,     0,     0,     0,    42,     0,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1024,
       0,     0,  1025,     0,     0,  1026,     0,     0,   922,     0,
       0,   923,   924,     0,     0,     0,     0,     0,   925,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   927,   928,
       0,  1027,     0,   861,     0,     0,     0,     0,     0,     0,
       0,   911,     0,     0,     0,     0,     0,   912,     0,     0,
       0,     0,     0,     0,     0,   929,   930,     0,     0,     0,
     755,     0,     0,   756,     0,     0,     0,  1028,     0,   922,
     221,     0,   923,   924,  1706,   158,   159,   160,     0,   925,
     757,     0,  1010,     0,     0,     0,  1011,     0,     0,     0,
       0,  1012,     0,     0,     0,     0,     0,  1013,     0,     0,
       0,   919,     0,     0,     0,     0,     0,     0,     0,   927,
     928,     0,  1027,  1014,     0,     0,     0,     0,  1015,     0,
       0,  1016,     0,     0,     0,     0,  1017,     0,     0,     0,
    1018,     0,     0,  1019,  1020,     0,   929,   930,  1021,     0,
       0,  1022,     0,  1023,     0,     0,     0,     0,  1028,     0,
       0,   221,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1024,     0,
       0,  1025,     0,     0,  1026,     0,     0,     0,     0,     0,
       0,     0,     0,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   758,     0,     0,     0,
     213,   214,     0,     0,     0,     0,     0,     0,   922,     0,
     759,   923,   924,     0,     0,     0,     0,     0,   925,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   760,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   927,   928,
       0,  1027,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   929,   930,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1028,     0,     0,
     221
};

static const yytype_int16 yycheck[] =
{
       1,    68,     3,   216,   352,   515,    68,   448,   328,   514,
     362,    53,    38,     1,   436,    37,   338,   883,   439,   404,
     367,   925,    30,    65,   513,   438,  1021,   437,   722,   911,
     391,   536,   179,    30,   317,    30,    53,   633,   479,   761,
    1460,   384,   220,   689,   571,    53,    47,   468,    65,  1460,
     404,   876,  1467,    32,    55,   465,  1465,    65,    53,    60,
    1466,   415,   416,   346,  1464,  1026,  1481,  1407,   478,   419,
      65,   107,   404,   109,   845,   111,   112,   984,  1478,   115,
     381,   117,   383,   435,   120,   121,  1328,  1329,   124,   125,
     126,   127,  1272,  1335,   377,   131,   443,    36,   134,   180,
    1577,  1459,  1460,   139,   140,  1315,   458,  1406,  1629,  1774,
     881,   147,   404,   177,  1573,   886,   180,   527,   177,   177,
     931,   180,   180,   415,   416,  1797,  1795,   474,   899,  1587,
    1588,  1573,   903,  1796,     7,     8,     9,  1556,  1557,  1770,
     862,  1693,   177,  1978,     4,   180,   997,   572,   177,     4,
    1565,   180,   177,   177,  1001,   180,   180,   504,   998,   177,
    2010,    23,   180,   177,   999,   177,   180,     7,   180,   995,
     996,   177,  1414,   190,   180,   600,    49,     7,    51,     7,
       7,  1639,   529,   608,   177,     7,  1590,   180,    61,    38,
    1839,   177,  1840,    66,   180,  1821,   104,    70,  1636,   624,
      23,  1851,   104,   105,   177,    22,   103,   180,   216,    57,
     177,    20,   220,   180,   177,   177,    97,   180,   180,     8,
      23,   216,  1290,    79,    80,   220,    15,    16,    17,    18,
     842,   105,   725,    22,   727,  2041,   729,   293,   103,   839,
     840,   734,    57,  1024,   669,   104,   844,   127,   758,   104,
     105,   109,   104,   111,   112,   311,   104,   115,    85,   117,
     327,   328,   120,   121,    57,    54,   124,   125,   126,   127,
      49,    58,  1692,   131,   135,   785,   134,    32,    33,    34,
      12,   139,   140,    30,   122,    62,   103,   104,   713,   147,
      85,   786,   271,  1751,  1752,   104,   104,    49,   277,  1598,
    1599,     5,   795,    47,   845,    13,    53,   168,   169,    57,
    1081,   104,   146,  1771,  1772,   332,    90,   827,    65,    93,
       7,   826,    99,   748,   391,  1356,    91,  1358,    15,    16,
      17,    18,    30,     8,  1692,   352,   825,    58,     8,     7,
      53,    73,   404,   204,   352,    77,     7,   294,   221,    24,
      57,   104,    20,   415,   416,    53,   104,   352,   128,    20,
      92,    93,   104,    14,    15,    16,   843,    65,   103,   164,
     437,   866,   813,  1793,     8,   437,   237,  1023,   110,    54,
      88,   294,  1793,   153,    54,     8,   104,   105,   106,   297,
     432,   111,    15,    16,    17,    18,   404,   104,   465,    22,
     821,   133,   134,   465,  1185,   447,   126,   415,   416,   819,
    1768,   478,   144,   293,   153,   432,   478,   364,  1014,   151,
      54,  1017,   104,  1781,   432,   164,   165,    78,    79,    80,
     447,    54,   104,   363,  1792,  1793,  1504,   432,   297,   447,
    1287,   275,   276,  1901,    95,    96,   298,  2253,   313,   297,
     288,   364,   447,   104,   327,   328,   168,   169,   331,   495,
     527,   369,   113,   114,  1868,   527,  1078,   369,   119,   216,
    1908,     8,   369,   220,  1074,  1075,  1076,   824,    15,    16,
      17,    18,  1080,   364,     1,   136,   295,     4,    90,     6,
     298,    93,  1092,     8,   145,   146,    85,  2347,   149,   254,
      15,    16,    17,    18,   297,   365,  2137,   369,   335,    26,
      27,    28,    29,   369,   369,   364,   281,    54,   216,  2354,
    1291,  1292,   220,   357,    41,   365,  2152,    44,  2177,  2179,
    2178,     1,   347,   348,     4,   365,     6,   365,  1385,    54,
    1081,   364,     1,   365,   361,     4,   369,     6,    57,   297,
     282,   283,   369,  2105,   701,   297,    26,    27,    28,    29,
     364,   364,  2227,   350,   351,   352,   369,    26,    27,    28,
      29,  1929,  1383,   277,   364,   349,   449,   599,   313,   368,
     453,  2039,  2213,  2104,   457,  1546,  1437,  1927,   292,   462,
     297,   883,   363,   297,   467,   104,   622,   111,  1438,  2271,
    2269,   944,  1079,  2080,  1439,   352,   310,  2270,  2027,  1435,
    1436,   315,   126,   295,  1141,  2092,  1143,    90,  1059,    85,
     133,   702,  1653,   295,    45,  1656,   927,   928,  1659,   350,
     351,   352,  2091,   243,   285,   286,   700,   495,   702,   852,
    1001,   700,   700,   702,   702,    90,   585,   164,   311,  2091,
     523,   369,    30,  1895,   352,   114,    85,   404,    87,  1259,
      30,   365,  1047,   876,     4,   700,     6,   702,   415,   416,
    1002,   700,   545,   702,  1028,    53,   700,   702,   702,  1889,
    1890,   368,   700,    53,   702,   432,   700,    65,   702,  1931,
     702,   717,   714,  1047,   700,    65,   702,   104,   105,   106,
     447,   364,  1127,   995,   996,   997,   998,   999,     5,   702,
    1002,  1357,  1377,  1378,   700,  1047,   702,  1067,  1625,  1384,
     164,    93,  1902,  1216,    90,  2024,  1027,   700,   114,   702,
     114,   243,    85,   700,   432,   702,  1028,   700,   700,   702,
     702,   126,   331,   332,   333,   368,    85,   349,    87,   447,
     342,   135,   819,    90,     4,  1047,     6,   819,   164,   103,
      30,   133,  2102,  1629,   148,   150,     1,    38,    39,     4,
    1280,     6,  1277,   126,   142,   292,   161,  1643,  2193,   135,
     351,   137,  2191,    53,    85,  1274,  2192,   155,   297,   521,
    2190,    26,    27,    28,    29,    65,   103,   150,   128,  1624,
     244,   245,   246,    38,   172,    85,  1311,    87,  1579,   876,
     126,   284,    90,   286,   243,    93,  1343,    81,  1307,   278,
      37,   883,  1000,   153,   294,   126,     4,  1693,     6,   103,
     146,   368,   104,    97,   150,   107,   108,   109,   216,   284,
     293,   286,   220,   494,   852,  1286,   216,  2267,   365,   150,
     220,   172,   116,   368,    30,   144,  2267,   852,   837,     1,
     149,    39,     4,   293,     6,   331,   332,   333,   876,  1563,
      87,  2329,   258,   259,   316,   883,   305,    53,  1025,   349,
      90,   876,  1604,  1605,    26,    27,    28,    29,   114,    65,
     349,    85,   543,   144,   920,   365,   918,   331,   104,   305,
     306,   307,   308,   309,   243,   131,   365,   341,  2266,  2267,
     161,   283,   349,    30,   313,   314,   187,   984,   284,   190,
     286,     4,   306,   931,  1271,     0,  1287,     1,   284,     4,
       4,     6,     6,   325,  1001,   127,    53,   303,   164,   364,
    1002,    17,   134,  2185,   369,    18,   216,   284,    65,   286,
     220,  1597,    26,    27,    28,    29,  1303,   364,   326,   275,
     276,   103,    52,   243,  1474,   364,   305,    41,  1473,    78,
      44,  1254,   164,   358,   352,    30,   293,   368,   351,   167,
     277,   364,   352,  1472,   172,  1047,   369,   995,   996,   997,
     998,   999,  1000,    52,  1002,   292,   313,   314,    53,   277,
     297,   364,   111,     1,  1650,  1000,     4,    20,     6,    22,
      65,   103,   104,   310,   292,   103,   104,   126,   315,   297,
    1028,  1462,   114,   287,  1385,   305,   114,   277,    26,    27,
      28,    29,   310,   364,  2276,     7,  1918,   315,   364,  1047,
     216,   357,   292,    41,   220,   364,    44,   297,    20,  1434,
      22,  1052,   114,   364,   432,  1056,  1441,  1400,   135,   364,
     310,   114,   432,   172,    57,   315,    60,    61,   365,   447,
     343,   349,  1497,   346,   284,   292,   286,   447,   131,  1433,
    1434,  1294,   352,   136,   137,   294,  1440,  1441,  1442,  1443,
     164,   168,   169,   310,   171,    45,    46,   131,   315,   216,
     317,   135,    20,   220,    22,   852,   168,   169,     7,  1953,
    1954,  1955,  1956,    43,   349,   177,   110,  1542,   991,    45,
    1142,    20,  1144,    22,  1471,     7,   120,     8,  1150,   876,
     365,    45,   126,  1827,   168,   169,   883,    45,    20,     7,
      22,  1433,  1434,  1435,  1436,  1437,  1438,  1439,  1440,  1441,
    1442,  1443,   261,   309,   852,     1,   150,   104,     4,  2154,
       6,   216,   432,   103,   104,   220,   164,   112,   113,   114,
       1,   313,   314,     4,   114,     6,   352,   447,   876,   103,
      26,    27,    28,    29,     5,  2089,  2090,     8,     1,   370,
     243,     4,    38,     6,  1407,    26,    27,    28,    29,     4,
       1,     6,     7,     4,   370,     6,   227,   349,   342,    40,
     231,   232,   233,    26,    27,    28,    29,    45,   292,     5,
    1287,   342,     8,   365,   103,    26,    27,    28,    29,    62,
      63,   359,    60,    61,    67,   352,   367,   984,   114,  2105,
     116,   117,   118,   119,   104,   350,   351,   352,   995,   996,
     997,   998,   999,  1000,  1602,  1002,   432,   177,    89,    90,
      91,    92,     1,    94,   116,     4,    97,     6,    85,   129,
     130,   447,  2116,  2117,  2118,  1716,    22,    90,    23,    25,
     132,  1028,   110,   114,    97,  1632,  1294,    26,    27,    28,
      29,   365,   120,    23,   292,    52,   277,   352,   126,  1294,
    1047,   153,  1000,   104,   105,   106,   112,   113,   160,   360,
     361,   292,    52,  1635,  1739,   432,   297,    52,  1385,   294,
     295,   134,   150,  1385,    15,    16,  1711,  1712,    52,   310,
     447,   313,   314,   164,   315,     1,    52,  1629,     4,   103,
       6,  1846,  1852,  1686,    85,  1688,   226,   227,   228,   229,
     230,  1643,  1841,   336,   337,    94,   169,  1711,  1712,   103,
      26,    27,    28,    29,    86,    87,    88,   365,    90,  1377,
    1378,  1379,  1380,  1381,   364,  1383,  1384,   432,     1,    85,
    1885,     4,   103,     6,  1392,  1393,   319,   320,   321,   322,
     323,  1880,   447,  1882,   254,   134,   103,   249,   250,  1407,
     103,  1693,   103,    26,    27,    28,    29,    85,    45,    15,
      16,  1624,  1407,    17,   274,   275,    15,    16,    41,  1711,
    1712,    44,    18,    60,    61,  1433,  1434,  1435,  1436,  1437,
    1438,  1439,  1440,  1441,  1442,  1443,   368,   103,  1785,  1761,
       4,  1458,     6,     5,     8,   103,   277,     7,   254,   255,
    1458,     7,   258,  1454,  1455,     7,  1457,  1458,  1459,  1460,
       7,   292,   103,  1458,     7,   278,   297,  1455,     5,  1457,
    1458,  1459,  1460,   110,   852,   138,   139,   140,   141,   310,
     364,  1503,   852,   120,   315,   194,   195,  1834,     7,   126,
      18,  1916,  1917,    17,  1919,   335,   336,   337,   876,  2095,
    2096,   238,   368,   349,  2275,     8,   876,   192,   193,  2155,
     247,   248,   103,   150,   251,   252,     7,     1,   349,   365,
       4,  1943,     6,   313,   314,  1946,  1552,  1553,  1550,  1551,
     103,  1944,   112,   113,   365,   128,   349,   112,   113,   278,
      38,   164,    26,    27,    28,    29,   364,  1294,   349,  2320,
     364,   144,   365,     4,     5,  1622,   364,  1624,  1625,   311,
     153,   364,  1987,   294,   365,  1975,   364,   160,   161,   103,
    1571,   112,   113,  1574,  1894,  1576,   103,   104,  1579,    95,
      96,  1643,   852,  1571,   364,  1602,  1574,     1,  1576,  1941,
       4,  1579,     6,  2003,  1602,   103,  1294,    49,    50,   103,
      52,    53,    54,  1645,    43,    44,   876,  1602,  1355,  1356,
     349,  1358,    26,    27,    28,    29,  1624,   222,   223,   224,
     225,  1629,  1000,   103,   104,  1647,   365,   103,  1645,  1624,
    1000,   104,   105,   106,   103,  1643,   364,  1645,  1728,  1729,
    1730,  1731,  1392,  1393,  1645,   104,   103,   313,   314,   103,
    1645,    45,    45,  1866,    47,   331,   332,   333,    45,    52,
    1407,    54,    97,   196,   131,    58,    60,    61,   191,   292,
     231,  1738,   235,    60,    61,   234,   852,   234,   236,   103,
      94,   235,   103,   349,   103,  1693,  1433,  1434,  1435,  1436,
    1437,  1438,  1439,  1440,  1441,  1442,  1443,   103,   343,   365,
     876,   103,   364,  1711,  1712,    98,   103,   100,  2218,  1407,
     104,  1458,  2217,   364,  1927,   364,   110,  2065,   364,   114,
     134,   116,   103,   110,     5,     8,   120,  2216,   103,   103,
    1000,    52,   126,   120,  1735,   852,   103,    22,    43,   126,
     135,  1749,   365,  1744,  1745,  1746,  1747,  1748,   143,   144,
     145,   146,  1749,   149,  1749,  2260,   150,     5,   367,   876,
    1458,    85,  1770,   150,  1765,  1766,  1767,  1768,   103,  2258,
    2259,  2212,   114,  1770,  1791,  1770,     8,    85,  1779,  1780,
    1781,    22,    22,  1791,   367,  1802,   128,   103,   364,  1790,
    1791,  1792,  1793,   135,  1802,   364,  1791,   852,   364,     7,
     371,  1802,  1790,  1791,  1792,  1793,   371,  1802,   103,   295,
     294,   153,     5,  2105,  1802,   104,   364,   312,   160,   312,
      20,   876,   316,    43,  1000,   191,  1893,  1894,   294,  1830,
    1831,  1832,   311,   316,  2344,   104,     7,    70,  2343,     5,
     292,   166,  1830,  1831,  1832,     7,   103,   299,   300,  1722,
    1723,  1724,  1725,  2342,    39,  1602,   363,   104,  1866,   103,
    1950,  1951,  1952,   315,   278,   349,  2291,   363,  2215,    45,
      99,  1866,   191,   311,   363,  1622,   104,  1624,  1625,   103,
      99,   365,  1629,  1000,    60,    61,   363,     7,   103,  2102,
     103,  1958,  2333,   365,    20,    85,  1643,   103,  1645,   103,
     103,  2326,   103,   297,  1602,   103,  1653,  2332,  1975,  1656,
    2257,  1912,  1659,  1975,     5,   190,  1294,   103,     7,  1927,
     104,     8,     8,   103,  1294,    46,  1624,    48,   104,    50,
      51,   363,  1927,   151,   110,   349,  2003,    58,    59,   103,
     103,  2003,   294,  2010,   120,  1000,  1693,  1645,   105,   103,
     126,   365,   295,   365,   364,   364,     1,   187,  1331,     4,
     364,     6,    41,   364,  1711,  1712,   112,   113,     7,   134,
     103,    92,  1973,  1974,   150,   121,   122,   123,   124,   125,
     364,    26,    27,    28,    29,    79,    80,    81,    82,    83,
      84,   364,  1993,   364,  2341,   364,   364,  2033,   104,   364,
    2001,  2002,  1749,   365,  2005,   365,   364,  2043,     1,   364,
    2011,     4,     8,     6,  2050,   364,     8,  2018,  2019,  2020,
    1893,  1894,   103,  1770,  1294,     5,   104,   404,   103,  1407,
    2018,  2019,  2020,    26,    27,    28,    29,  1407,   415,   416,
     103,   363,     8,  1020,  1791,    90,  1019,  1018,  2065,   912,
     798,  1749,    97,  1135,   797,  1802,  2123,  2065,   796,    99,
     100,   101,   102,   577,  1490,   799,   567,  1022,    20,   281,
    2065,   282,  1770,   434,  2115,  1463,  2143,  2276,  1775,  2198,
    1458,  2135,  1455,  2133,  1457,  1458,  1459,  1460,  1458,   134,
    2141,  2200,  2199,  1791,  2102,  2201,  2189,  2105,  2322,  1104,
    1724,   431,  1931,   894,  1802,   287,  1881,  2102,  2109,   904,
    1883,   104,   105,   106,  1914,   433,   967,  2188,  1294,  1866,
    2128,   297,  2263,  2124,   169,     1,  1379,  1381,     4,  2137,
       6,  2128,  1380,  2128,   987,  1398,  1622,  1407,  1405,  1624,
    2137,   877,  2137,  2144,  2145,   319,  1126,  1443,   216,  1335,
      26,    27,    28,    29,  1603,  2156,  2157,  1866,  2159,  2160,
    2161,  2162,  2163,  1641,  1641,  2166,   416,   443,  1866,     1,
    2330,   445,     4,   446,     6,   333,   444,  1294,   723,   332,
    1927,   333,   700,  1194,  2241,   667,    -1,     1,  1458,    -1,
       4,    -1,     6,    -1,    26,    27,    28,    29,  1571,    -1,
      -1,  1574,    -1,  1576,    -1,  2213,  1579,    -1,    -1,  2276,
      -1,    -1,    26,    27,    28,    29,  2213,    -1,  2213,  2220,
      -1,    -1,  2223,    -1,  1602,  2226,    -1,    41,    -1,  1927,
      44,  1407,  1602,   278,    -1,    -1,  1241,  1242,   114,  1294,
     280,    -1,    -1,    -1,    -1,    -1,  1624,    -1,  2265,   289,
     290,   291,   292,  2320,  1624,    -1,    -1,  2265,    -1,    -1,
      -1,    -1,    94,  2264,  2265,  2266,  2267,  1645,  1641,    -1,
    2265,  2272,  2273,  2274,  2275,  1645,  2264,  2265,  2266,  2267,
    2347,  2289,  1458,    -1,  2272,  2273,  2274,  2275,    -1,    -1,
    1407,  2292,  2289,  1298,  2289,    -1,    -1,    -1,    -1,  2300,
      -1,    -1,   134,    -1,   349,  2306,  2307,  2308,  2309,  2310,
    2311,  2312,  2313,  2314,  2315,  2316,  2317,    -1,  2065,    -1,
     365,    -1,    -1,  2311,  2312,  2313,    -1,    -1,    -1,    -1,
    2331,    45,  1602,    -1,    -1,    -1,    -1,  2338,  2339,  2340,
      -1,  1458,    -1,    -1,    -1,    -1,    60,    61,    -1,    -1,
     164,    -1,  1407,    -1,  1624,  2102,   349,    -1,  2105,    -1,
      -1,     1,    -1,    -1,     4,    -1,     6,  2065,    -1,    -1,
      -1,  1749,   365,    -1,    -1,  1645,    -1,    -1,    -1,  1749,
      -1,  2128,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
    2137,    -1,  1770,    -1,    -1,    -1,   110,    -1,    -1,    -1,
    1770,    -1,   278,  1458,  2102,    -1,   120,    -1,    -1,    -1,
      -1,    -1,   126,  1791,    -1,    -1,    -1,  1790,  1791,  1792,
    1793,  1791,    -1,    -1,  1802,    -1,  1602,    -1,     1,  1802,
    2128,     4,  1802,     6,    -1,     1,   150,    -1,     4,  2137,
       6,    -1,    -1,    -1,    -1,    -1,   278,    -1,  1624,    -1,
      -1,    -1,    -1,    26,    27,    28,    29,  1830,  1831,  1832,
      26,    27,    28,    29,    -1,    -1,  2213,     1,    -1,  1645,
       4,    -1,     6,   349,   114,    -1,    -1,  1482,   292,  1749,
    1485,    -1,    -1,    -1,    -1,  1602,    -1,    -1,  1866,   365,
      -1,    -1,    26,    27,    28,    29,  1866,    -1,    -1,    -1,
    1770,    -1,    -1,    -1,    -1,    -1,   883,  1624,    -1,     1,
      -1,    -1,     4,    -1,     6,  2213,    -1,   349,  2265,    -1,
      -1,  1791,    -1,    -1,    -1,    -1,    -1,    -1,  1645,    -1,
      -1,    -1,  1802,   365,    26,    27,    28,    29,    -1,    -1,
      -1,    -1,  2289,    -1,    -1,    37,    -1,  1602,    -1,  1927,
      -1,   365,    -1,    -1,   931,    47,    90,  1927,    -1,    45,
      52,  1566,    54,    97,    56,    -1,    -1,  2265,    45,  1624,
      47,    -1,    -1,  1749,    60,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    -1,    -1,    -1,    -1,    -1,
    1645,  2289,    -1,    -1,  1770,    -1,  1866,    37,    -1,    -1,
     134,  1606,    -1,  1608,  1609,    45,  1611,    47,    -1,    49,
      -1,    -1,    52,    53,    54,  1791,    56,    -1,   995,   996,
     997,   998,   999,    -1,   110,  1002,  1802,   104,    -1,    -1,
      -1,    -1,  1749,   110,   120,   169,    -1,    -1,   278,   849,
     126,   851,    -1,   120,    -1,  2018,  2019,  2020,    -1,   126,
      -1,  1028,    -1,  1770,    -1,    -1,    -1,  1927,   868,    -1,
     870,    -1,   872,    -1,   150,   114,    -1,    -1,   878,   879,
    1047,    -1,   164,   150,  1791,    -1,    -1,   126,    -1,   128,
      -1,    -1,    -1,   113,   114,  1802,   116,  2065,    10,    -1,
    1866,    45,    -1,    -1,  1749,  2065,    -1,   146,    -1,    -1,
     910,   150,    -1,   913,   153,   135,    60,    61,    -1,   349,
      -1,    -1,    -1,   143,   144,  1770,   146,    -1,    40,    41,
      42,    43,    -1,    -1,  2102,   365,   156,   157,   158,    -1,
      -1,    -1,  2102,    -1,    -1,    -1,  1791,    -1,   311,    -1,
      -1,    -1,    -1,    -1,   278,   311,    -1,  1802,   135,  1866,
    2128,  1927,    -1,    75,    -1,    -1,   110,   144,  2128,  2137,
      -1,    83,    84,    85,    86,    -1,   120,  2137,   978,    -1,
      -1,    -1,   126,   160,    -1,    -1,   349,   164,    -1,    -1,
      -1,   168,   169,   349,   106,   277,   173,   174,   175,   176,
     177,    -1,   365,   242,    -1,  2065,   150,    -1,    -1,   365,
     292,   123,    -1,    -1,    -1,    -1,   128,   129,   130,    -1,
    1927,  1866,    -1,    -1,    -1,   349,   138,    -1,   310,   141,
     297,    -1,    -1,   315,    -1,   317,   275,   276,    -1,    -1,
      -1,   365,  2102,    -1,    -1,  2213,    -1,   277,  1048,  1049,
      -1,    -1,    -1,  2213,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   292,    -1,    -1,    -1,    -1,   349,  2128,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  2137,    -1,  1874,
     310,    -1,  1927,   365,    -1,   315,    -1,   317,    -1,     1,
      -1,    -1,     4,    -1,     6,    -1,    -1,  2265,    -1,  2065,
      -1,  2264,  2265,  2266,  2267,  2265,    -1,    -1,    -1,  2272,
    2273,  2274,  2275,    -1,    26,    27,    28,    29,   357,    -1,
      -1,  2289,    -1,    -1,    -1,    -1,    -1,    -1,    40,  2289,
      -1,    -1,    -1,    -1,    -1,    -1,  2102,    -1,    -1,    -1,
      -1,    -1,     1,    -1,    -1,     4,    -1,     6,  2311,  2312,
    2313,    -1,    -1,  2213,    -1,    -1,    -1,    -1,  2065,    -1,
      -1,    -1,  2128,    -1,    -1,    -1,    -1,    26,    27,    28,
      29,  2137,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,
      92,    40,    94,    23,    -1,    97,    -1,    -1,   300,   301,
     302,    -1,    -1,    -1,    -1,  2102,    -1,    -1,    38,    -1,
      -1,    -1,   114,    -1,    -1,  2265,    -1,    -1,    -1,    -1,
    1377,  1378,  1379,  1380,  1381,    -1,  1383,  1384,    -1,    -1,
    2065,  2128,    -1,    -1,    -1,  1392,  1393,    -1,    -1,  2289,
    2137,    90,    91,    92,    -1,    94,    -1,    -1,    97,  2034,
      -1,    -1,    -1,    37,    -1,    -1,    -1,  2213,    -1,  2044,
      -1,    45,   164,    47,    -1,   114,  2051,  2102,    52,    53,
      54,    -1,    56,   103,    -1,    -1,  1433,  1434,  1435,  1436,
    1437,  1438,  1439,  1440,  1441,  1442,  1443,    -1,    -1,    -1,
      -1,  1281,    -1,  2128,    -1,  1285,    -1,    -1,    -1,    -1,
      -1,  2086,  2137,    -1,    -1,  1295,  1296,  1297,    -1,  2265,
    1300,    -1,  1302,    -1,    -1,   164,  2213,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  2289,    -1,  1325,    -1,    -1,   440,   441,
     442,    -1,    -1,    -1,    -1,   175,    -1,    -1,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,    -1,  2147,  2148,  2149,    -1,    -1,    -1,  2265,    -1,
      -1,   473,  1362,    -1,  1364,   277,    -1,    -1,  2213,   112,
     113,  1371,    -1,    -1,    -1,    -1,    -1,    -1,  2173,    -1,
     292,    -1,  2289,   126,     1,   297,  1386,     4,  1388,     6,
     502,   134,    -1,    -1,    -1,    -1,    -1,  1397,   310,  1399,
      -1,    -1,    -1,   315,    -1,    -1,    -1,   150,    -1,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,   277,    -1,
    2265,    -1,    -1,    40,    -1,    -1,    -1,    -1,   112,   113,
      -1,    -1,    -1,   292,    -1,    -1,    -1,   349,   297,    -1,
      -1,    -1,    37,   127,  2289,   129,   130,   131,    -1,    -1,
      45,   310,    47,   365,    49,    -1,   315,    52,    53,    54,
      -1,    56,  1629,    -1,    -1,    -1,    -1,    -1,   152,   153,
     154,    -1,    89,    90,    91,    92,  1643,    94,    -1,    -1,
      97,    -1,    -1,   277,    -1,    -1,    -1,    -1,  2283,    -1,
     349,    -1,    -1,    -1,    -1,    -1,    -1,   114,   292,     1,
      -1,    -1,    -1,    -1,    -1,    -1,   365,    -1,    -1,    11,
      12,    13,    14,    15,    16,    17,   310,    -1,    20,    -1,
      -1,   315,    -1,   317,    -1,    -1,  1693,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   278,   279,   280,   281,    -1,
      -1,    -1,    -1,    -1,  1711,  1712,    -1,   164,    -1,    -1,
      -1,   294,   295,   296,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  2357,  2358,  2359,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    -1,    -1,    76,    77,    78,    79,    80,    81,
      -1,    -1,  1582,  1583,    -1,    -1,    88,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    -1,    99,   100,   101,
     102,    -1,   104,    -1,    -1,    -1,   108,    -1,   110,    -1,
    1610,   113,   114,    -1,   116,    -1,    -1,   119,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,  1626,    -1,    -1,    -1,
      -1,   133,   134,   135,   136,   137,    -1,   139,   140,    -1,
      -1,   143,   144,   145,   146,    -1,    -1,   149,    -1,   151,
      -1,   112,   113,    -1,    -1,   116,    -1,    -1,    -1,    -1,
     277,    -1,  1662,    -1,    -1,    -1,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   292,    -1,    -1,    -1,  1679,
     297,  1681,   277,    -1,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   310,    -1,    70,    -1,   292,   315,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   823,    -1,    -1,    -1,   310,   177,    -1,    -1,    -1,
     315,    -1,   317,    -1,    -1,    -1,    -1,    -1,    -1,   104,
     105,   106,   349,    -1,    -1,    -1,   111,    -1,    -1,    -1,
     115,    -1,    -1,    -1,    -1,   120,    -1,    -1,   365,    -1,
      -1,   126,    -1,    -1,    -1,   130,   175,    -1,    -1,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,    -1,    -1,    -1,   150,    -1,    -1,   280,   281,
     282,   283,   284,   285,   286,   287,    -1,   289,   290,   291,
     292,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    -1,     4,    -1,     6,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    23,    -1,    -1,    26,    27,    28,    29,    30,
      31,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1859,
      -1,    62,    63,    -1,    -1,    -1,    67,    -1,    -1,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1892,    94,    -1,    -1,    -1,    98,    -1,   100,
      -1,    -1,   103,   104,   105,   106,    -1,    -1,   109,    -1,
     111,    -1,   287,   114,   115,   290,   291,    -1,    -1,   120,
      -1,  1921,   297,    -1,    -1,   126,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  2105,    -1,
      -1,   142,    -1,    -1,    -1,    -1,   147,    -1,    -1,   150,
      -1,    -1,   327,   328,   155,    -1,    -1,    -1,   159,    -1,
      -1,   162,   163,   164,    -1,    -1,   167,    -1,    -1,   170,
      -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   487,    -1,    -1,    -1,    -1,
      -1,    -1,   494,    -1,   369,    -1,   197,   198,    -1,   200,
     201,   202,   203,   505,    -1,   507,    -1,   509,    -1,   511,
      -1,    -1,    -1,    -1,   516,    -1,    -1,    -1,   520,   521,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   530,    -1,
      -1,   533,    -1,    -1,    -1,    -1,    -1,   238,   239,   240,
     241,   543,    -1,    -1,    -1,    -1,   247,   248,    -1,    -1,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,    -1,   277,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   292,    -1,    -1,    -1,    -1,   297,    -1,    -1,    -1,
    2100,    -1,    -1,   304,    -1,    -1,    -1,    -1,    -1,   310,
      -1,    -1,    -1,    -1,   315,    -1,    -1,    -1,    -1,    -1,
     321,   322,    -1,   324,    -1,    -1,   327,   328,   329,   330,
      -1,    -1,    -1,   334,    -1,    -1,    -1,   338,   339,   340,
      -1,    -1,    -1,   344,   345,    -1,    -1,    -1,   349,    -1,
      -1,    -1,   353,    -1,     1,   356,   357,     4,    -1,     6,
      -1,   362,    -1,   364,   365,   366,    -1,    -1,   369,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    62,    63,    -1,    -1,    -1,
      67,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,
      -1,    98,    -1,   100,    -1,    -1,   103,   104,   105,   106,
      -1,    -1,   109,    -1,   111,    -1,    -1,   114,   115,    -1,
      -1,     1,    -1,   120,     4,    -1,     6,    -1,    -1,   126,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   142,    26,    27,    28,    29,
     147,    -1,    -1,   150,   182,    -1,    -1,    -1,   155,    -1,
      40,    -1,   159,    -1,    -1,   162,   163,   164,    -1,    -1,
     167,    -1,    -1,   170,    -1,   172,    -1,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,    -1,    -1,    -1,    -1,    -1,    -1,
     197,   198,    -1,   200,   201,   202,   203,    -1,    -1,    -1,
      90,    91,    92,    -1,    94,    -1,    -1,    97,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   114,    -1,    -1,    -1,    -1,    -1,
      -1,   238,   239,   240,   241,    -1,    -1,    -1,    -1,    -1,
     247,   248,    -1,    -1,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,    -1,
     277,    -1,    -1,    -1,   164,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   292,    -1,    -1,    -1,    -1,
     297,    -1,    -1,    -1,    -1,    -1,    -1,   304,    -1,    -1,
      -1,    -1,    -1,   310,    -1,    -1,    -1,    -1,   315,    -1,
      -1,    -1,    -1,    -1,   321,   322,    -1,   324,    -1,    -1,
     327,   328,   329,   330,    -1,    -1,    -1,   334,    -1,    -1,
      -1,   338,   339,   340,    -1,    -1,    -1,   344,   345,    -1,
      -1,    -1,   349,    -1,    -1,    -1,   353,    -1,     1,   356,
     357,     4,    -1,     6,    -1,   362,    -1,   364,   365,   366,
      -1,    -1,   369,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    26,    27,    28,    29,    30,    31,    -1,
      -1,    -1,    35,    36,    37,    -1,    -1,   277,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      -1,    -1,   292,    -1,    -1,    -1,    -1,   297,    -1,    62,
      63,    -1,    -1,    -1,    67,    -1,    -1,    70,    -1,    -1,
     310,    -1,    -1,    -1,    -1,   315,    79,    80,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    -1,  1105,    -1,    98,    -1,   100,    -1,    -1,
     103,   104,   105,   106,    -1,    -1,   109,    -1,   111,   349,
      -1,   114,   115,    -1,    -1,     1,    -1,   120,     4,    -1,
       6,    -1,    -1,   126,    -1,   365,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,
      26,    27,    28,    29,   147,    -1,    -1,   150,    -1,    -1,
      -1,    -1,   155,    -1,    40,    -1,   159,    -1,    -1,   162,
     163,   164,    -1,    -1,   167,    -1,    -1,   170,    -1,   172,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   197,   198,    -1,   200,   201,   202,
     203,    -1,    -1,    -1,    90,    91,    92,    -1,    94,    -1,
      -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   114,    -1,
      -1,    -1,    -1,    -1,    -1,   238,   239,   240,   241,    -1,
      -1,    -1,    -1,    -1,   247,   248,    -1,    -1,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    -1,    -1,   277,    -1,    -1,    -1,   164,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,   292,
      -1,    -1,    -1,    -1,   297,    45,    -1,    47,    -1,    49,
      -1,   304,    52,    53,    54,    -1,    56,   310,    -1,    -1,
      60,    61,   315,    -1,    -1,    -1,    -1,    -1,   321,   322,
      -1,   324,    -1,    -1,   327,   328,   329,   330,    -1,    -1,
      -1,   334,    -1,    -1,    -1,   338,   339,   340,    -1,    -1,
      -1,   344,   345,    -1,    -1,    -1,   349,    -1,    -1,    -1,
     353,    -1,     1,   356,   357,     4,    -1,     6,    -1,   362,
     110,   364,   365,   366,    -1,    -1,   369,    -1,    -1,    -1,
     120,    -1,    -1,    -1,    23,    -1,   126,    26,    27,    28,
      29,    30,    31,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,   277,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,
     150,    -1,    -1,    52,    -1,    -1,   292,    -1,    -1,    -1,
      -1,   297,    -1,    62,    63,    -1,    -1,    -1,    67,    -1,
      -1,    70,    -1,    -1,   310,    -1,    -1,    -1,    -1,   315,
      79,    80,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    98,
      -1,   100,    -1,    -1,   103,   104,   105,   106,    -1,    -1,
     109,    -1,   111,   349,    -1,   114,   115,    -1,    -1,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,   126,    -1,   365,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,   147,    -1,
      -1,   150,    -1,    -1,     1,    -1,   155,     4,    -1,     6,
     159,    -1,    -1,   162,   163,   164,    -1,    -1,   167,    -1,
      -1,   170,    -1,   172,    -1,    -1,    -1,   277,    -1,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      37,    -1,   292,    -1,    -1,    -1,    -1,    -1,   197,   198,
      47,   200,   201,   202,   203,    52,    -1,    54,    -1,    56,
     310,    -1,    -1,    -1,    -1,   315,    -1,   317,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   238,
     239,   240,   241,    -1,    -1,    -1,    -1,    -1,   247,   248,
      -1,    -1,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,    -1,   277,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   292,    -1,    -1,    -1,    -1,   297,    -1,
      -1,    -1,    -1,    -1,    -1,   304,    -1,    -1,    -1,    -1,
      -1,   310,    -1,    -1,    -1,    -1,   315,   164,    -1,    -1,
      -1,    -1,   321,   322,    -1,   324,    -1,    -1,   327,   328,
     329,   330,    -1,    -1,    -1,   334,    -1,    -1,    -1,   338,
     339,   340,     4,    -1,     6,   344,   345,    -1,    -1,    -1,
     349,    -1,    -1,    -1,   353,    -1,    -1,   356,   357,    -1,
      -1,    23,    -1,   362,    -1,   364,   365,   366,    30,    31,
     369,    -1,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      62,    63,    -1,    -1,   104,    67,    -1,    -1,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     277,   131,    -1,   109,   134,    -1,    98,    -1,   100,    -1,
      -1,   103,   104,   105,   106,   292,    -1,   109,    -1,   111,
     126,   151,    -1,   115,    -1,    -1,    -1,    -1,   120,    -1,
      -1,    -1,    -1,   310,   126,    -1,   142,    -1,   315,    -1,
     317,   147,    -1,    -1,   150,    -1,    -1,    -1,    -1,   155,
     142,    -1,    -1,   159,    -1,   147,   162,   163,   150,    -1,
      -1,   167,    -1,   155,   170,    -1,   172,   159,    -1,    -1,
     162,   163,   349,    -1,    -1,   167,    -1,    -1,   170,    -1,
     172,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   365,    -1,
      -1,   197,    -1,    -1,    -1,    -1,    -1,   203,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   197,   198,    -1,   200,   201,
     202,   203,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,   238,   239,   240,   241,
      -1,    -1,    -1,    -1,    -1,   247,   248,    -1,    -1,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,    -1,    -1,   277,    -1,   317,    -1,     5,
      -1,   321,   322,    -1,   112,   113,    -1,    -1,   116,    -1,
     292,   331,    -1,    -1,    -1,   297,    -1,    23,    -1,    -1,
      -1,    -1,   304,    -1,    -1,    31,   134,    -1,   310,    -1,
      36,    -1,    -1,   315,    -1,    -1,    -1,   357,    -1,   321,
     322,    -1,   324,    -1,    -1,   327,   328,   329,   330,    -1,
      -1,    -1,   334,    -1,    -1,    -1,   338,   339,   340,    -1,
      -1,    -1,   344,   345,    70,    -1,    -1,    -1,    -1,    -1,
      76,   353,    -1,    -1,   356,   357,    -1,    -1,    -1,    85,
     362,    87,   364,   365,   366,    -1,    -1,   369,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,    -1,    -1,   109,    -1,   111,    -1,    -1,    -1,   115,
      -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,
     126,    -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,    -1,    -1,
      -1,   147,    -1,    -1,   150,    -1,   254,    -1,    -1,   155,
      -1,    -1,    -1,   159,    -1,    -1,   162,   163,    -1,    -1,
      -1,   167,    -1,    -1,   170,    -1,   172,    -1,    -1,    -1,
     278,   279,   280,   281,   282,   283,    -1,   285,   286,   287,
     288,   289,   290,   291,    -1,    -1,   294,   295,   296,    -1,
      -1,   197,   198,    90,   200,   201,   202,   203,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,   317,
      -1,    -1,    -1,    -1,    -1,   112,   113,    -1,    -1,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      -1,    -1,   238,    -1,    31,    -1,    -1,   134,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,    -1,
      -1,   277,    -1,    70,    -1,    -1,    -1,    -1,    -1,    76,
      -1,   287,    -1,    -1,   290,   291,    -1,    -1,    -1,    -1,
      -1,   297,    -1,    -1,   191,    -1,    -1,    -1,    -1,   305,
      -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,
      -1,    -1,   109,    -1,   111,   321,   322,    -1,   115,    -1,
      -1,   327,   328,   120,    -1,    -1,    -1,    -1,    -1,   126,
      -1,    -1,    -1,   130,   340,    -1,    -1,    -1,   344,   345,
      -1,    -1,    -1,    -1,    -1,   142,    -1,    -1,   354,   355,
     147,   357,    -1,   150,    -1,    -1,    -1,   254,   155,    -1,
     366,    -1,   159,   369,    -1,   162,   163,    -1,    -1,    -1,
     167,    -1,    -1,   170,    -1,   172,    -1,    -1,    -1,    -1,
      -1,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,    -1,    -1,   294,   295,   296,
     197,   198,    90,   200,   201,   202,   203,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,    -1,    -1,
     317,    -1,    -1,    -1,   112,   113,    -1,    -1,   116,    -1,
      -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,
      -1,   238,    -1,    31,    -1,    -1,   134,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,    -1,    -1,
     277,    -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,
     287,    -1,    -1,   290,   291,    -1,    -1,    -1,    -1,    -1,
     297,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,    -1,
      -1,   109,    -1,   111,   321,   322,    -1,   115,    -1,    -1,
     327,   328,   120,    -1,    -1,    -1,    -1,    -1,   126,    -1,
      -1,    -1,   130,   340,    -1,    -1,    -1,   344,   345,    -1,
      -1,    -1,    -1,    -1,   142,    -1,    -1,   354,   355,   147,
     357,    -1,   150,    -1,    -1,    -1,   254,   155,    -1,   366,
      -1,   159,   369,    -1,   162,   163,    -1,    -1,    -1,   167,
      -1,    -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,    -1,    -1,   294,   295,   296,   197,
     198,    -1,   200,   201,   202,   203,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     5,    -1,    -1,   317,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
     238,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,    -1,    -1,   277,
      -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,   287,
      -1,    -1,   290,   291,    -1,    -1,    -1,    -1,    -1,   297,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,    -1,    -1,
     109,    -1,   111,   321,   322,    -1,   115,    -1,    -1,   327,
     328,   120,    -1,    -1,   109,    -1,   111,   126,    -1,    -1,
     115,   130,   340,    -1,    -1,   120,   344,   345,    -1,    -1,
      -1,   126,    -1,   142,    -1,    -1,   354,   355,   147,   357,
      -1,   150,    -1,    -1,    -1,    -1,   155,   142,   366,    -1,
     159,   369,   147,   162,   163,   150,    -1,    -1,   167,    -1,
     155,   170,    -1,   172,   159,    -1,    -1,   162,   163,   164,
      -1,    -1,   167,    -1,    -1,   170,    -1,   172,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,   198,
      -1,   200,   201,   202,   203,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   197,    -1,   199,     5,    -1,    -1,   203,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,   238,
      -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,    -1,   277,    -1,
      70,    -1,    -1,    -1,    -1,    -1,    76,    -1,   287,    -1,
      -1,   290,   291,    -1,    -1,    -1,    -1,    -1,   297,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   103,   104,   105,   106,    -1,    -1,   109,
      -1,   111,   321,   322,    -1,   115,    -1,    -1,   327,   328,
     120,    -1,    -1,   109,    -1,   111,   126,    -1,   114,   115,
     130,   340,    -1,    -1,   120,   344,   345,    -1,    -1,    -1,
     126,    -1,   142,    -1,    -1,   354,   355,   147,   357,    -1,
     150,    -1,    -1,    -1,    -1,   155,   142,   366,    -1,   159,
     369,   147,   162,   163,   150,    -1,    -1,   167,    -1,   155,
     170,    -1,   172,   159,    -1,    -1,   162,   163,    -1,    -1,
      -1,   167,    -1,    -1,   170,    -1,   172,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,   198,    -1,
     200,   201,   202,   203,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   197,    -1,   199,    -1,    -1,    -1,   203,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,   238,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,    -1,   277,    -1,    70,
      -1,    -1,    -1,    -1,    -1,    76,    -1,   287,    -1,    -1,
     290,   291,    -1,    -1,    -1,    -1,    -1,   297,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,    -1,    -1,    -1,    -1,
     111,   321,   322,    -1,   115,    -1,    -1,   327,   328,   120,
      -1,    21,    -1,    -1,    -1,   126,    -1,    -1,    -1,   130,
     340,    -1,    -1,    -1,   344,   345,    -1,    -1,    -1,    -1,
      -1,   142,    -1,    -1,   354,   355,   147,   357,    -1,   150,
      -1,    -1,    -1,    -1,   155,    -1,   366,    -1,   159,   369,
      -1,   162,   163,    -1,    -1,    -1,   167,    -1,    -1,   170,
      70,   172,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,
      -1,    -1,    -1,   109,    -1,   111,    -1,    -1,    -1,   115,
      -1,    -1,    -1,    -1,   120,    -1,   197,    -1,    -1,   200,
     126,    -1,   203,   103,   104,   105,   106,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,   115,   142,    -1,    -1,    -1,
     120,   147,    -1,    -1,   150,    -1,   126,    -1,    -1,   155,
     130,    -1,    -1,   159,    -1,    -1,   162,   163,    -1,    -1,
      -1,   167,   142,    -1,   170,    -1,   172,   147,    -1,    -1,
     150,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,   159,
      -1,    -1,   162,   163,    -1,    -1,    -1,   167,    -1,    -1,
     170,   197,   172,    -1,    -1,    -1,    -1,   203,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   287,    -1,    -1,   290,
     291,    -1,    -1,    -1,    -1,    21,   297,   197,    -1,    -1,
     200,    -1,    -1,   203,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   327,   328,    -1,   330,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,   354,   355,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   364,    -1,   366,    -1,    -1,   369,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,    -1,    -1,    -1,    -1,   111,    -1,   287,    -1,   115,
     290,   291,    -1,    -1,   120,    -1,    -1,   297,    -1,    -1,
     126,    -1,    22,    23,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,    -1,    -1,
      -1,   147,    -1,    -1,   150,    -1,    -1,   327,   328,   155,
     330,    -1,    -1,   159,    -1,    -1,   162,   163,    -1,    -1,
      -1,   167,    -1,    -1,   170,    -1,   172,    -1,    -1,    -1,
      70,    -1,    -1,    -1,   354,   355,    76,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   364,    -1,   366,    -1,    -1,   369,
      -1,   197,    -1,    -1,   200,    -1,    -1,   203,    -1,    -1,
      -1,    -1,    -1,   103,   104,   105,   106,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,
     120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
     130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   142,    -1,    -1,    -1,    -1,   147,    -1,    -1,
     150,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,   159,
      -1,    -1,   162,   163,    -1,    -1,    -1,   167,    -1,    -1,
     170,    -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   287,    -1,    -1,   290,   291,    -1,    -1,    -1,    -1,
      -1,   297,    -1,    -1,    -1,    -1,    -1,   197,    -1,    -1,
     200,    -1,    -1,   203,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   327,   328,    -1,   330,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,   355,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
     366,    30,    31,   369,    -1,    -1,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    -1,   287,    -1,    -1,
     290,   291,    -1,    62,    63,    -1,    -1,   297,    67,    -1,
      -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      79,    80,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,   328,    98,
     330,   100,    -1,    -1,   103,   104,   105,   106,    -1,    -1,
     109,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      -1,   120,    -1,    -1,   354,   355,    -1,   126,    -1,    -1,
      -1,    -1,    -1,    -1,   364,    -1,   366,    -1,    -1,   369,
      -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,   147,    -1,
      -1,   150,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,
     159,    -1,    -1,   162,   163,    -1,    -1,    -1,   167,    -1,
      -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,   198,
      -1,   200,   201,   202,   203,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    42,    -1,    -1,    45,    46,    47,    48,    -1,    -1,
      51,    52,    -1,    54,    55,    56,    -1,    58,    -1,   238,
     239,   240,   241,    -1,    -1,    -1,    -1,    -1,   247,   248,
      -1,    -1,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,    98,   277,   100,
     101,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   292,    -1,    23,    -1,    -1,   297,    -1,
      -1,    -1,    30,    31,    -1,   304,    -1,    -1,    36,    37,
      -1,   310,    -1,    -1,    -1,    -1,   315,    -1,    -1,    -1,
      -1,    -1,   321,   322,    -1,   324,    -1,    -1,   327,   328,
     329,   330,    -1,    -1,    -1,   334,    -1,    -1,    -1,   338,
     339,   340,    -1,    -1,    -1,   344,   345,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   353,    -1,    -1,   356,   357,    87,
      -1,    -1,    -1,   362,    -1,   364,    -1,   366,    -1,    -1,
     369,    -1,    -1,    -1,    -1,   103,   104,   105,   106,    -1,
      -1,   109,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,
      -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,   126,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,   147,
      -1,    -1,   150,    -1,    -1,    -1,    -1,   155,    -1,    -1,
      -1,   159,    -1,    -1,   162,   163,    -1,    -1,    -1,   167,
      -1,    -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   277,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,
     198,   292,   200,   201,   202,   203,    -1,    -1,   299,   300,
     301,   302,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   310,
      -1,    -1,    -1,    -1,   315,    -1,   317,   318,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     238,   239,   240,   241,    -1,    -1,    -1,    -1,    -1,   247,
     248,    -1,    -1,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,    -1,    -1,   277,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,   292,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   310,    -1,    -1,    -1,    -1,   315,    -1,   317,
      -1,    -1,    -1,   321,   322,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,   340,    -1,    -1,    -1,   344,   345,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   356,   357,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,   366,    -1,
     109,   369,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,   147,    -1,
      -1,   150,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,
     159,    -1,    -1,   162,   163,    -1,    -1,    -1,   167,    -1,
      -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,   198,
      -1,   200,   201,   202,   203,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,   238,
      30,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,    -1,    -1,   277,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   287,    -1,
      -1,   290,   291,    -1,    -1,    -1,    -1,    -1,   297,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   103,   104,   105,   106,    -1,    -1,   109,
      -1,   111,   321,   322,    -1,   115,    -1,    -1,   327,   328,
     120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
      -1,   340,    -1,    -1,    -1,   344,   345,    -1,    -1,    -1,
      -1,    -1,   142,    -1,    -1,   354,   355,   147,   357,    -1,
     150,    -1,    -1,    -1,    -1,   155,    -1,   366,    -1,   159,
     369,    -1,   162,   163,    -1,    -1,    -1,   167,    -1,    -1,
     170,    -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,   198,    23,
     200,   201,   202,   203,    -1,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   238,   239,
     240,   241,    -1,    -1,    -1,    -1,    -1,   247,   248,    -1,
      -1,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,    -1,    -1,   277,    -1,   103,
     104,   105,   106,    -1,    -1,   109,    -1,   111,    -1,    -1,
      -1,   115,    -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,
      -1,   321,   322,   147,    -1,    -1,   150,    -1,    -1,    -1,
      -1,   155,    -1,    -1,    -1,   159,    -1,    -1,   162,   163,
     340,    -1,    -1,   167,   344,   345,   170,    -1,   172,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   356,   357,    -1,    -1,
      -1,    -1,    -1,    -1,   364,    -1,   366,    -1,    -1,   369,
      -1,    -1,    -1,   197,   198,    -1,   200,   201,   202,   203,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   238,   239,   240,   241,    -1,    -1,
      -1,    -1,    -1,   247,   248,    -1,    -1,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,    70,    -1,   277,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,    -1,    -1,
      -1,    -1,   111,    -1,    -1,    -1,   115,   321,   322,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,   130,    -1,    -1,    -1,    -1,   340,    -1,    -1,    -1,
     344,   345,    -1,   142,    -1,    -1,    -1,    -1,   147,    -1,
      -1,   150,   356,   357,    -1,    -1,   155,    -1,    -1,    -1,
     159,    -1,   366,   162,   163,   369,    -1,    -1,   167,    -1,
      -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,
       0,     1,    -1,    -1,     4,    -1,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,    -1,
      -1,   200,    -1,    -1,   203,    -1,    26,    27,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    47,    -1,    49,
      -1,    -1,    52,    53,    54,    -1,    56,    -1,    -1,    -1,
      60,    61,    62,    -1,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   104,    -1,    -1,    -1,   287,    -1,
     110,   290,   291,    -1,    -1,    -1,    -1,    -1,   297,    -1,
     120,    70,    -1,    -1,    -1,    -1,   126,    76,    -1,    -1,
      79,    80,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,   328,
     150,   330,    -1,    -1,   103,   104,   105,   106,    -1,    -1,
      -1,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      -1,   120,    -1,    -1,    -1,   354,   355,   126,    -1,    -1,
      -1,   130,    -1,    -1,    -1,   364,    -1,   366,    -1,    -1,
     369,    -1,    -1,   142,    -1,    -1,    -1,    -1,   147,    -1,
      -1,   150,    70,    -1,    -1,    -1,   155,    -1,    76,    -1,
     159,    -1,    -1,   162,   163,    -1,    -1,    -1,   167,    -1,
      -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,   115,   197,    -1,
      -1,   200,   120,    -1,   203,    -1,    -1,    -1,   126,    -1,
      -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   142,    -1,    -1,   277,    -1,   147,
      -1,    -1,   150,    -1,    -1,    -1,    -1,   155,    -1,    -1,
      -1,   159,   292,    -1,   162,   163,    -1,   297,    -1,   167,
      -1,    -1,   170,    -1,   172,    -1,    -1,    -1,    -1,    -1,
     310,    -1,    -1,    -1,    -1,   315,    -1,   317,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,
      -1,    -1,   200,    -1,    -1,   203,    -1,    -1,   287,    -1,
      -1,   290,   291,    -1,    -1,    -1,    -1,    -1,   297,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,   328,
      -1,   330,    -1,   104,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   354,   355,    -1,    -1,    -1,
     131,    -1,    -1,   134,    -1,    -1,    -1,   366,    -1,   287,
     369,    -1,   290,   291,   103,   104,   105,   106,    -1,   297,
     151,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,
     328,    -1,   330,   142,    -1,    -1,    -1,    -1,   147,    -1,
      -1,   150,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,
     159,    -1,    -1,   162,   163,    -1,   354,   355,   167,    -1,
      -1,   170,    -1,   172,    -1,    -1,    -1,    -1,   366,    -1,
      -1,   369,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   197,    -1,
      -1,   200,    -1,    -1,   203,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   317,    -1,    -1,    -1,
     321,   322,    -1,    -1,    -1,    -1,    -1,    -1,   287,    -1,
     331,   290,   291,    -1,    -1,    -1,    -1,    -1,   297,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   357,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,   328,
      -1,   330,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   354,   355,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   366,    -1,    -1,
     369
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   373,     0,     1,     4,     6,    26,    27,    28,    29,
      37,    45,    47,    49,    52,    53,    54,    56,    60,    61,
      62,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,   110,   120,   126,   150,   277,
     292,   310,   315,   317,   374,   430,   431,   432,   433,   514,
     515,   516,   518,   533,   374,   105,   104,   297,   510,   510,
     510,   516,   527,   516,   518,   533,   516,   521,   521,   521,
     516,   524,   433,    49,   434,    37,    45,    47,    52,    53,
      54,    56,   277,   292,   310,   315,   317,   435,    49,   436,
      37,    45,    47,    49,    52,    53,    54,    56,   277,   292,
     310,   315,   317,   441,    53,   443,    37,    42,    45,    46,
      47,    48,    51,    52,    54,    55,    56,    58,    98,   100,
     101,   102,   277,   292,   299,   300,   301,   302,   310,   315,
     317,   318,   444,    49,    50,    52,    53,    54,   292,   299,
     300,   315,   447,    45,    47,    52,    54,    58,    98,   100,
     448,    47,   449,    23,    30,    31,    36,   103,   104,   105,
     106,   109,   111,   115,   120,   126,   142,   147,   150,   155,
     159,   162,   163,   167,   170,   172,   197,   198,   200,   201,
     202,   203,   238,   239,   240,   241,   247,   248,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   277,   321,   322,   340,   344,   345,   356,   357,
     366,   369,   457,   511,   640,   641,   644,   645,   646,   650,
     714,   717,   719,   723,   728,   729,   731,   733,   743,   745,
     747,   749,   751,   753,   757,   759,   761,   763,   765,   767,
     769,   771,   773,   775,   779,   781,   783,   785,   796,   804,
     806,   808,   809,   811,   813,   815,   817,   819,   821,   823,
     825,    58,   350,   351,   352,   450,   456,    58,   451,   456,
      37,    45,    47,    49,    52,    53,    54,    56,   277,   292,
     310,   315,   317,   442,   104,   452,   453,   377,   396,   397,
      90,   284,   286,   527,   527,   527,   527,     0,   374,   510,
     510,    57,   347,   348,   530,   531,   532,    35,    37,    52,
      62,    63,    67,    70,    79,    80,    81,    82,    83,    84,
      98,   100,   253,   277,   292,   297,   304,   310,   315,   324,
     327,   328,   329,   330,   334,   338,   339,   353,   362,   364,
     537,   538,   539,   541,   542,   543,   544,   545,   546,   547,
     548,   549,   550,   551,   552,   553,   557,   558,   559,   562,
     563,   564,   571,   575,   583,   584,   587,   588,   589,   590,
     591,   612,   613,   615,   616,   618,   619,   622,   623,   624,
     634,   635,   636,   637,   638,   639,   645,   652,   653,   654,
     655,   656,   657,   661,   662,   663,   698,   712,   717,   718,
     741,   742,   743,   786,   374,   363,   363,   374,   510,   595,
     458,   463,   537,   510,   469,   472,   640,   663,   475,   510,
     483,   518,   534,   527,   516,   518,   521,   521,   521,   524,
      90,   284,   286,   527,   527,   527,   527,   533,   440,   516,
     527,   528,   437,   514,   516,   517,   438,   516,   518,   519,
     534,   439,   516,   521,   522,   521,   521,   516,   524,   525,
      90,   284,   286,   687,   440,   440,   440,   440,   521,   527,
     446,   515,   536,   516,   536,   518,   536,    45,   536,   521,
     521,   536,   524,   536,    45,    46,   521,   536,   536,    90,
     284,   303,   687,   688,   527,    45,   536,    45,   536,    45,
     536,    45,   536,   527,   527,   527,    45,   536,   403,   534,
      45,    47,   515,   516,   518,   536,   439,   521,   439,   527,
      45,   516,   536,    45,   516,   536,   527,   415,   516,   518,
     521,   521,   536,    45,   521,   518,   104,   107,   108,   109,
     746,   112,   113,   254,   255,   258,   648,   649,    32,    33,
      34,   254,   720,   133,   651,   168,   169,   807,   112,   113,
     114,   748,   114,   116,   117,   118,   119,   750,   112,   113,
     121,   122,   123,   124,   125,   752,   112,   113,   116,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   177,   754,
     114,   116,   135,   143,   144,   145,   146,   758,   114,   135,
     148,   306,   760,   112,   113,   127,   129,   130,   131,   152,
     153,   154,   762,   113,   114,   116,   135,   143,   144,   146,
     156,   157,   158,   764,   128,   144,   153,   160,   161,   766,
     144,   161,   768,   153,   164,   165,   770,   131,   135,   168,
     169,   772,   135,   168,   169,   171,   774,   135,   144,   160,
     164,   168,   169,   173,   174,   175,   176,   177,   787,   114,
     168,   169,   177,   797,   164,   199,   747,   749,   751,   753,
     757,   759,   761,   763,   765,   767,   769,   771,   773,   775,
     776,   777,   778,   780,   796,   804,   806,   127,   134,   164,
     393,   784,   393,   114,   199,   778,   782,   135,   168,   169,
     204,   237,   805,   114,   126,   128,   146,   150,   153,   242,
     275,   276,   357,   730,   732,   812,   243,   814,   243,   816,
     164,   244,   245,   246,   818,   128,   153,   810,   116,   132,
     153,   160,   249,   250,   820,   128,   153,   822,   114,   128,
     135,   153,   160,   824,   104,   131,   134,   151,   317,   331,
     357,   715,   716,   717,   112,   113,   116,   134,   254,   278,
     279,   280,   281,   282,   283,   285,   286,   287,   288,   289,
     290,   291,   294,   295,   296,   317,   734,   735,   738,   331,
     341,   722,   657,   662,   342,   238,   247,   248,   251,   252,
     826,   360,   361,   402,   725,   656,   510,   421,   456,   351,
     401,   456,   389,   440,   437,   438,   518,   534,   439,   521,
     521,   524,   525,   687,   440,   440,   440,   440,   384,   407,
      46,    48,    50,    51,    58,    59,    92,   454,   527,   527,
     527,   381,   682,   697,   684,   686,   103,   103,   103,    85,
     730,   293,   635,   172,   510,   640,   713,   713,    62,    99,
     510,   104,   715,    90,   191,   284,   734,   735,   293,   316,
     293,   311,   293,   313,   314,   572,    85,   164,    85,    85,
     730,   104,     4,   375,   664,   665,   349,   535,   548,   429,
     463,   381,   294,   295,   560,   561,   382,   427,   164,   305,
     306,   307,   308,   309,   565,   566,   414,   325,   586,   408,
       5,    70,    76,    85,    87,   111,   115,   120,   126,   130,
     150,   238,   287,   290,   291,   297,   305,   327,   328,   354,
     355,   366,   598,   599,   600,   601,   602,   603,   604,   606,
     607,   608,   609,   610,   611,   641,   644,   650,   707,   708,
     709,   714,   719,   723,   729,   730,   731,   733,   739,   740,
     743,   422,   428,    38,    39,   187,   190,   592,   593,   408,
      85,   331,   332,   333,   614,   620,   621,   408,    85,   617,
     620,   386,   392,   413,   335,   336,   337,   625,   626,   630,
     631,    23,   640,   642,   643,    15,    16,    17,    18,   368,
       8,    24,    54,     9,    10,    11,    12,    13,    14,    19,
     111,   115,   120,   126,   142,   147,   150,   155,   159,   162,
     163,   167,   170,   172,   197,   200,   203,   330,   366,   641,
     643,   644,   658,   659,   660,   663,   699,   700,   701,   702,
     703,   704,   705,   706,   708,   709,   710,   711,    52,    52,
      22,   364,   680,   699,   700,   705,   680,    38,   364,   594,
     364,   364,   364,   364,   364,   530,   537,   595,   458,   463,
     469,   472,   475,   483,   527,   527,   527,   381,   682,   697,
     684,   686,   537,   428,    57,    57,    57,   463,    57,   472,
      57,   483,   527,   381,   404,   412,   419,   472,   428,    43,
     445,   516,   521,   536,   527,    45,   381,   516,   516,   516,
     516,   404,   412,   419,   516,   516,   518,   472,   381,   516,
     516,   412,   521,   510,   424,     7,     8,   114,   258,   259,
     647,   309,   420,   104,   127,   293,   424,   423,   388,   423,
     398,   111,   126,   111,   126,   377,   138,   139,   140,   141,
     755,   396,   423,   399,   423,   400,   397,   423,   399,   376,
     387,   379,   425,   426,    23,    38,   103,   175,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   788,   789,   790,   423,   103,   383,   421,   776,   393,
     778,   182,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   798,
     803,   417,   423,   396,   397,   402,   732,   416,   416,   370,
     416,   416,   370,   416,   395,   391,   385,   423,   406,   405,
     419,   405,   419,   112,   113,   126,   134,   150,   278,   279,
     280,   736,   737,   738,   380,   342,   342,   103,   416,   395,
     391,   385,   406,   359,   724,   367,   428,   472,   483,   527,
     381,   404,   412,   419,   455,   456,   695,   695,   695,   294,
     364,   681,   311,   364,   696,   364,   572,   683,   364,   511,
     685,     5,   126,   150,   610,    85,   610,   632,   633,   657,
     177,    23,    23,    97,   364,    52,    52,    52,   103,   313,
      52,   738,    52,   610,   104,   298,   512,   610,   313,   314,
     576,   610,   103,   627,   628,   629,   640,   644,   657,   661,
     723,   729,   630,   610,   610,    85,   511,    21,   663,   668,
     669,   670,   678,   705,   706,     7,   365,   511,   364,   103,
     103,   561,    78,   111,   126,   172,   261,   568,   511,   103,
     103,   103,   511,   567,   566,   142,   155,   172,   326,   610,
     415,   384,     5,   610,    85,   388,   398,   377,   396,   397,
     382,    85,   408,   408,   603,   641,   709,    15,    16,    17,
      18,   368,    20,    22,     8,    54,     5,   620,    85,    87,
     243,   305,     7,     7,   103,   103,   593,     5,     7,     5,
     610,   628,   640,   644,   626,     7,   510,   364,   710,   710,
     701,   702,   703,   656,   364,   554,   642,   700,   396,   399,
     397,   399,   376,   387,   379,   425,   426,   421,   383,   393,
     417,   408,   705,     7,    20,    15,    16,    17,    18,   368,
       7,    20,    22,     8,   699,   700,   705,   610,   610,   103,
     365,   374,    20,   374,   103,   498,   428,   462,   464,   471,
     480,   484,   594,   364,   364,   364,   364,   364,   695,   695,
     695,   681,   696,   683,   685,   103,   103,   103,   364,   103,
     103,   364,   695,   104,   380,   516,   103,   649,   423,   390,
     103,   410,   410,   388,   396,   388,   396,   114,   131,   136,
     137,   243,   396,   756,   378,    97,   794,   191,   792,   196,
     795,   194,   195,   793,   192,   193,   791,   131,   383,   227,
     231,   232,   233,   802,   222,   223,   224,   225,   800,   226,
     227,   228,   229,   230,   801,   801,   231,   234,   234,   235,
     236,   235,   114,   131,   164,   799,   418,   416,   103,   103,
     112,   113,   112,   113,   380,   380,   103,   103,   343,   721,
     103,   161,   358,   726,   730,   364,   695,   364,   364,   364,
     103,   491,   381,   576,   496,   404,   492,   103,   412,   497,
     419,   610,     5,     5,   610,   428,   642,    90,    93,   535,
     671,   673,    38,   175,   180,   190,   789,   790,   511,   511,
     103,   657,   666,   667,   610,   610,   610,   380,   103,   610,
      52,   610,   381,   404,   103,   578,   580,   581,   412,   104,
     295,   573,    22,   413,    85,   335,    43,   610,   375,     5,
     375,   277,   292,   297,   310,   315,   675,   676,    90,    93,
     535,   674,   677,   375,   665,   465,   388,   149,   144,   149,
     569,   570,   104,   114,   585,   644,   114,   585,   421,   114,
     585,   610,     5,   610,   610,   367,   598,   598,   599,   600,
     601,   103,   603,   598,   605,   642,   663,   610,   610,    85,
       8,    85,   641,   709,   739,   739,   610,   621,   610,   620,
     631,   378,   632,   375,   555,   556,   367,   705,   699,   705,
     710,   710,   701,   702,   703,   705,   103,   699,   705,   660,
     705,    20,    20,   103,    39,   374,   365,   374,   430,   535,
     594,    37,    47,    52,    54,    56,   164,   277,   292,   310,
     315,   317,   365,   374,   430,   459,   535,    44,    94,   114,
     164,   365,   374,   430,   500,   506,   507,   535,   537,    40,
      89,    90,    91,    92,    94,    97,   114,   164,   277,   292,
     310,   315,   365,   374,   430,   470,   481,   535,   540,   547,
      40,    90,    91,    92,   114,   164,   365,   374,   430,   481,
     535,   540,    41,    44,   164,   292,   365,   374,   430,   428,
     462,   464,   471,   480,   484,   364,   364,   364,   381,   404,
     412,   419,   464,   484,   380,   380,     7,   420,   423,   396,
     790,   423,   417,   371,   371,   396,   396,   397,   397,   721,
     346,   721,   103,   394,   402,   112,   113,   727,   484,   380,
     494,   495,   493,   295,   365,   374,   430,   535,   681,   578,
     580,   365,   374,   430,   535,   696,   365,   374,   430,   535,
     683,   573,   365,   374,   430,   535,   685,   610,   610,     5,
     512,   512,   673,   421,   378,   378,   364,   548,   671,   405,
     405,   380,   380,   380,   610,   380,    20,   104,   295,   312,
     577,   312,   579,    20,   316,   574,   627,   640,   644,   629,
     628,   610,    43,    82,    83,   679,   706,   712,   191,   294,
     381,   316,   311,   572,   676,   104,   672,   512,   675,   677,
     365,   374,   537,   396,     7,   421,   585,   585,    70,   585,
     610,     5,   610,   166,   610,   620,   620,     5,   365,   540,
     668,     7,   365,   699,   699,   103,    39,   428,   510,   529,
     510,   520,   510,   523,   523,   510,   526,   104,   460,   461,
      90,   284,   286,   529,   529,   529,   529,   374,   363,    79,
      80,   508,   509,   640,   423,    99,   374,   374,   374,   374,
     374,   468,   645,   512,   512,   363,    95,    96,   482,   103,
     104,   129,   130,   254,   274,   275,   488,   489,   499,    86,
      87,    88,    90,   473,   474,   374,   374,   374,   374,   547,
     468,   512,   512,   363,   489,   473,   374,   374,   374,   104,
     363,    99,   381,   365,   365,   365,   365,   365,   494,   495,
     493,   365,   365,   103,   744,     7,   409,   103,   394,   402,
     365,    94,   134,   278,   365,   374,   430,   535,   693,    90,
      97,   134,   169,   278,   365,   374,   430,   535,   694,   114,
     278,   365,   374,   430,   535,   690,   103,   381,   577,   579,
     404,   412,   574,   419,   610,   666,   365,   380,   319,   320,
     321,   322,   323,   582,   103,   404,   103,   581,   404,   582,
     103,   412,   413,   413,   610,   375,   103,   313,   103,   298,
     512,   576,   103,   374,   570,   423,   423,   415,   423,   610,
      85,   632,     5,   365,     5,   375,   556,   190,   596,   103,
     490,   463,   469,   475,   483,     7,   529,   529,   529,   490,
     490,   490,   490,   364,   485,   640,   411,   104,     8,   374,
     374,   472,   411,     8,   423,     7,   374,     5,   374,   374,
     472,     5,   374,   151,   501,   485,   374,   365,   365,   365,
     378,   103,   721,   363,   167,   172,   689,   515,   380,   512,
     103,   689,   103,   515,   380,   105,   515,   380,   548,   294,
     104,   573,   380,   103,   295,   382,   382,   578,   580,   573,
     399,   399,   610,   365,   632,   712,   187,   597,   374,   364,
     364,   364,   364,   364,   461,   490,   490,   490,   364,   364,
     364,   364,   104,   105,   486,   487,   640,   374,    41,   645,
     488,   474,    37,    87,   277,   292,   310,   315,   317,   466,
     467,   468,   477,   478,   645,    87,   477,   479,    22,   103,
     104,   361,   502,   503,   504,   640,   374,   374,   380,   380,
     380,     7,   394,   364,   425,   421,   374,   374,   374,   374,
     374,   374,   374,   134,   374,   365,   380,   103,   577,   579,
     574,   365,   375,   596,   498,   464,   471,   480,   484,   364,
     364,   364,   491,   496,   492,   497,     7,   365,   104,   428,
     468,    90,   284,   476,   381,   404,   412,   419,   374,     8,
     374,   374,   489,   103,    22,    25,   744,   104,   691,   692,
     689,   381,   404,   404,   412,   597,   365,   365,   365,   365,
     365,   494,   495,   493,   365,   365,   365,   365,   375,   487,
      43,    44,   505,   380,   645,   423,   374,   103,   103,     5,
       7,   365,   374,   374,   374,   374,   374,   374,   365,   365,
     365,   374,   374,   374,   374,   511,   640,   363,   501,   423,
     103,   512,   513,   692,   374,   423,   428,   374,   374,   374,
     381,   404,   412,   419,   485,   411,   380,   380,   380
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   372,   373,   373,   374,   374,   375,   375,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   429,   430,   430,   430,   430,   430,   431,   431,   431,
     431,   432,   432,   432,   432,   432,   432,   432,   432,   432,
     432,   432,   432,   432,   432,   432,   432,   433,   433,   433,
     433,   433,   433,   433,   433,   433,   433,   433,   433,   433,
     433,   433,   433,   433,   433,   433,   433,   433,   433,   433,
     433,   434,   435,   435,   435,   435,   435,   435,   435,   435,
     435,   435,   435,   435,   435,   435,   435,   435,   435,   436,
     437,   437,   438,   438,   439,   439,   440,   440,   441,   441,
     441,   441,   441,   441,   441,   441,   441,   441,   441,   441,
     441,   441,   441,   441,   442,   442,   442,   442,   442,   442,
     442,   442,   442,   442,   442,   442,   442,   442,   442,   443,
     444,   444,   444,   444,   444,   444,   444,   444,   444,   444,
     444,   444,   444,   444,   444,   444,   444,   444,   444,   444,
     444,   444,   444,   444,   444,   444,   444,   444,   444,   444,
     444,   444,   444,   444,   445,   446,   446,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   448,   448,   448,   448,   448,   448,
     448,   449,   450,   450,   451,   451,   452,   453,   453,   454,
     454,   454,   454,   454,   454,   454,   454,   455,   455,   456,
     456,   456,   457,   458,   459,   459,   460,   460,   461,   462,
     462,   462,   462,   462,   462,   462,   462,   462,   462,   462,
     462,   462,   462,   462,   462,   463,   464,   464,   464,   464,
     464,   464,   464,   464,   464,   465,   465,   465,   466,   466,
     467,   467,   468,   468,   469,   470,   470,   471,   471,   471,
     471,   471,   471,   471,   471,   471,   471,   471,   471,   472,
     472,   473,   473,   474,   474,   474,   474,   475,   476,   476,
     477,   477,   477,   477,   477,   478,   478,   479,   479,   480,
     480,   480,   480,   480,   480,   480,   480,   480,   480,   480,
     480,   480,   480,   481,   481,   482,   482,   483,   484,   484,
     484,   484,   484,   484,   484,   485,   485,   486,   486,   486,
     487,   487,   487,   488,   488,   489,   489,   490,   491,   491,
     491,   491,   491,   492,   492,   492,   492,   492,   493,   493,
     493,   493,   493,   494,   494,   494,   494,   494,   495,   495,
     495,   495,   495,   496,   496,   496,   496,   496,   497,   497,
     497,   497,   497,   498,   498,   498,   498,   498,   499,   499,
     499,   499,   499,   500,   501,   502,   502,   503,   503,   503,
     503,   503,   504,   504,   505,   505,   505,   505,   506,   507,
     508,   508,   509,   509,   510,   510,   511,   511,   511,   512,
     513,   513,   514,   514,   515,   515,   515,   515,   515,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   533,   533,
     533,   534,   535,   536,   536,   537,   537,   538,   539,   539,
     540,   540,   541,   542,   543,   544,   545,   545,   546,   546,
     546,   546,   546,   547,   547,   547,   547,   547,   548,   548,
     548,   548,   548,   548,   548,   548,   548,   548,   548,   548,
     548,   548,   548,   548,   548,   548,   548,   548,   548,   548,
     548,   549,   550,   550,   551,   552,   552,   553,   554,   554,
     555,   555,   555,   556,   557,   557,   558,   558,   559,   560,
     560,   561,   561,   562,   562,   562,   563,   563,   564,   565,
     565,   566,   566,   566,   566,   566,   566,   567,   568,   568,
     568,   568,   568,   569,   569,   570,   570,   571,   571,   572,
     572,   572,   573,   573,   574,   574,   575,   576,   576,   576,
     577,   577,   578,   579,   579,   580,   580,   581,   581,   582,
     582,   582,   582,   582,   583,   584,   585,   585,   586,   586,
     586,   586,   586,   586,   586,   586,   587,   588,   588,   589,
     589,   589,   589,   589,   589,   590,   590,   591,   592,   592,
     593,   593,   593,   593,   594,   594,   595,   596,   596,   597,
     597,   598,   598,   598,   598,   598,   598,   598,   598,   598,
     598,   598,   598,   598,   599,   599,   599,   600,   600,   601,
     601,   602,   602,   603,   604,   604,   605,   605,   606,   606,
     607,   608,   609,   609,   610,   610,   610,   611,   611,   611,
     611,   611,   611,   611,   611,   611,   611,   611,   611,   611,
     611,   612,   612,   613,   614,   614,   614,   615,   615,   616,
     617,   617,   617,   617,   617,   618,   618,   619,   619,   620,
     620,   621,   621,   621,   622,   622,   622,   622,   623,   623,
     624,   625,   625,   626,   626,   627,   627,   628,   628,   628,
     629,   629,   629,   629,   630,   630,   631,   631,   632,   632,
     633,   634,   634,   634,   635,   635,   635,   636,   636,   637,
     638,   638,   639,   640,   641,   641,   642,   642,   643,   644,
     645,   645,   645,   645,   645,   645,   645,   645,   645,   645,
     645,   645,   645,   645,   645,   646,   647,   647,   647,   648,
     648,   648,   648,   648,   649,   649,   650,   650,   651,   651,
     652,   652,   652,   653,   653,   654,   654,   655,   655,   656,
     657,   657,   658,   659,   660,   660,   661,   662,   662,   662,
     663,   664,   664,   664,   665,   665,   665,   666,   666,   667,
     668,   668,   668,   669,   669,   670,   670,   671,   671,   672,
     673,   673,   673,   674,   674,   675,   675,   676,   676,   676,
     676,   676,   676,   676,   676,   676,   677,   677,   677,   678,
     679,   679,   680,   680,   680,   680,   681,   682,   683,   684,
     685,   686,   687,   687,   687,   688,   688,   688,   689,   689,
     690,   690,   691,   691,   692,   693,   693,   693,   694,   694,
     694,   694,   694,   695,   696,   696,   697,   698,   698,   698,
     698,   698,   698,   698,   698,   699,   699,   700,   700,   700,
     701,   701,   701,   702,   702,   703,   703,   704,   704,   705,
     706,   706,   706,   706,   707,   707,   708,   709,   709,   709,
     709,   709,   709,   709,   709,   709,   709,   709,   709,   709,
     709,   710,   710,   710,   710,   710,   710,   710,   710,   710,
     710,   710,   710,   710,   710,   710,   710,   710,   710,   711,
     711,   711,   711,   711,   711,   711,   712,   712,   712,   712,
     712,   712,   713,   713,   714,   714,   714,   715,   715,   716,
     716,   716,   716,   716,   717,   717,   717,   717,   717,   717,
     717,   717,   717,   717,   717,   717,   717,   717,   717,   717,
     717,   717,   717,   717,   717,   717,   717,   717,   718,   718,
     718,   718,   718,   718,   719,   719,   720,   720,   720,   721,
     721,   722,   722,   723,   724,   724,   725,   725,   726,   726,
     727,   727,   728,   728,   729,   729,   729,   730,   730,   731,
     731,   732,   732,   732,   732,   733,   733,   733,   734,   734,
     735,   735,   735,   735,   735,   735,   735,   735,   735,   735,
     735,   735,   735,   735,   735,   735,   735,   736,   736,   736,
     736,   736,   736,   736,   737,   737,   737,   737,   738,   738,
     738,   738,   739,   739,   740,   740,   741,   741,   742,   743,
     743,   743,   743,   743,   743,   743,   743,   743,   743,   743,
     743,   743,   743,   743,   743,   743,   743,   743,   743,   743,
     743,   744,   745,   746,   746,   746,   746,   747,   748,   748,
     748,   749,   750,   750,   750,   750,   750,   751,   752,   752,
     752,   752,   752,   752,   752,   752,   752,   753,   753,   753,
     754,   754,   754,   754,   754,   754,   754,   754,   754,   754,
     754,   754,   755,   755,   755,   755,   756,   756,   756,   756,
     756,   757,   758,   758,   758,   758,   758,   758,   758,   759,
     760,   760,   760,   760,   761,   762,   762,   762,   762,   762,
     762,   762,   762,   762,   763,   764,   764,   764,   764,   764,
     764,   764,   764,   764,   764,   765,   766,   766,   766,   766,
     766,   767,   768,   768,   769,   770,   770,   770,   771,   772,
     772,   772,   772,   773,   774,   774,   774,   774,   775,   775,
     775,   775,   776,   776,   776,   776,   776,   776,   776,   776,
     776,   776,   776,   776,   776,   776,   777,   777,   777,   778,
     778,   779,   779,   780,   780,   781,   781,   782,   782,   783,
     783,   784,   784,   784,   785,   786,   787,   787,   787,   787,
     787,   787,   787,   787,   787,   787,   788,   788,   788,   788,
     788,   788,   789,   789,   789,   789,   789,   790,   790,   790,
     790,   790,   790,   790,   790,   790,   790,   790,   790,   791,
     791,   792,   793,   793,   794,   795,   796,   796,   797,   797,
     797,   798,   798,   798,   798,   798,   798,   798,   798,   798,
     798,   798,   798,   798,   798,   798,   798,   798,   798,   799,
     799,   799,   800,   800,   800,   800,   801,   801,   801,   801,
     801,   802,   802,   802,   802,   803,   803,   803,   803,   803,
     803,   803,   803,   803,   803,   803,   803,   804,   804,   805,
     805,   805,   805,   806,   807,   807,   808,   808,   808,   808,
     808,   808,   808,   808,   809,   810,   810,   811,   812,   812,
     812,   812,   813,   814,   815,   816,   817,   818,   818,   818,
     818,   819,   820,   820,   820,   820,   820,   820,   821,   822,
     822,   823,   824,   824,   824,   824,   824,   825,   826,   826,
     826,   826,   826
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     5,     5,     3,     2,     1,     1,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     3,     3,
       2,     2,     3,     3,     3,     2,     3,     2,     6,     2,
       6,     3,     2,     6,     6,     3,     6,     3,     5,     7,
       5,     7,     8,     8,     8,     5,     7,     5,     7,     5,
       7,     3,     2,     6,     2,     6,     6,     6,     3,     6,
       3,     5,     5,     8,     8,     8,     5,     5,     5,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       6,     2,     2,     2,     3,     2,     2,     6,     3,     3,
       5,     3,     3,     3,     2,     2,     2,     2,     2,     3,
       2,     2,     6,     3,     3,     5,     3,     3,     3,     3,
       2,     2,     2,     2,     2,     3,     2,     2,     3,     3,
       2,     3,     3,     2,     3,     3,     2,     3,     3,     2,
       3,     3,     2,     3,     3,     2,     2,     2,     2,     2,
       2,     4,     5,     2,     2,     1,     2,     2,     2,     3,
       3,     2,     3,     2,     3,     2,     2,     3,     2,     3,
       2,     3,     2,     2,     2,     2,     2,     2,     3,     2,
       2,     3,     2,     1,     2,     1,     3,     0,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     2,     1,     0,     2,     1,     1,     3,     1,     0,
       2,     2,     3,     8,     8,     8,     8,     9,     9,    10,
      10,    10,     9,     9,     9,     0,     0,     2,     2,     3,
       3,     3,     3,     5,     3,     0,     2,     3,     1,     3,
       2,     1,     1,     3,     0,     2,     3,     0,     2,     2,
       3,     4,     4,     4,     3,     4,     2,     3,     3,     1,
       1,     3,     1,     1,     1,     1,     1,     0,     1,     1,
       2,     2,     2,     2,     2,     1,     3,     1,     0,     0,
       2,     2,     4,     4,     8,     6,     7,     6,     4,     3,
       4,     3,     3,     3,     2,     1,     1,     0,     0,     2,
       2,     5,     5,     3,     4,     3,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     3,     0,     0,     2,
       2,     2,     2,     0,     2,     2,     2,     2,     0,     2,
       2,     2,     2,     0,     2,     2,     2,     2,     0,     2,
       2,     2,     2,     0,     2,     2,     2,     2,     0,     2,
       2,     2,     2,     0,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     7,     2,     1,     1,     1,     1,     1,
       3,     3,     1,     2,     2,     2,     3,     0,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     2,     2,     1,     2,     1,     1,
       2,     3,     2,     3,     1,     2,     3,     1,     2,     3,
       1,     2,     3,     1,     2,     2,     2,     1,     2,     2,
       2,     2,     2,     0,     1,     1,     2,     1,     1,     2,
       1,     2,     4,     4,     4,     4,     5,     5,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     1,     1,
       2,     2,     2,     2,     1,     1,     2,     1,     1,     2,
       1,     3,     1,     1,     5,     1,     1,     3,     3,     1,
       1,     3,     3,     5,     4,     5,     1,     2,     1,     1,
       2,     2,     2,     1,     3,     3,     1,     2,     1,     1,
       2,     2,     2,     2,     2,     2,     2,     1,     3,     3,
       1,     2,     1,     3,     1,     1,     1,     6,     6,     1,
       1,     0,     1,     1,     0,     3,     6,     1,     1,     0,
       0,     3,     3,     0,     2,     2,     3,     2,     2,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     0,     6,
       3,     6,     3,     5,     3,     5,     2,     1,     1,     3,
       4,     4,     5,     6,     5,     1,     2,     1,     1,     2,
       2,     2,     1,     1,     6,     8,     0,     0,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     3,     1,     1,     3,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     4,     5,     2,     3,     2,     6,     4,     3,     4,
       3,     2,     1,     1,     3,     4,     1,     2,     1,     1,
       2,     3,     1,     3,     4,     3,     5,     3,     6,     1,
       3,     1,     1,     1,     2,     4,     6,     6,     1,     2,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     2,
       1,     4,     5,     6,     1,     1,     1,     7,     8,     1,
       6,     8,     1,     2,     1,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     4,     1,     1,     2,     1,
       1,     1,     1,     1,     3,     1,     4,     4,     0,     2,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       1,     3,     3,     3,     1,     1,     3,     1,     1,     1,
       3,     1,     3,     3,     3,     3,     5,     1,     2,     1,
       1,     2,     3,     1,     1,     2,     1,     1,     2,     1,
       2,     2,     1,     1,     2,     1,     2,     2,     6,     6,
       6,     4,     5,     6,     4,     4,     2,     2,     1,     1,
       1,     1,     1,     1,     2,     2,     4,     0,     4,     0,
       1,     0,     1,     1,     1,     1,     1,     1,     2,     2,
       6,     3,     1,     3,     3,     3,     7,     3,     3,     3,
       3,     3,     3,     0,     4,     4,     0,     2,     2,     4,
       4,     5,     5,     3,     3,     3,     3,     1,     1,     1,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       1,     1,     3,     3,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     5,     3,
       5,     1,     5,     5,     3,     5,     1,     1,     1,     0,
       2,     1,     1,     6,     2,     0,     1,     1,     1,     1,
       1,     1,     5,     6,     8,     6,     5,     2,     2,     3,
       4,     1,     1,     1,     2,     3,     4,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     5,     6,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     7,     1,     1,     2,     1,     3,     1,     1,
       2,     3,     1,     1,     1,     1,     2,     3,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     5,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     3,     2,     1,     1,     1,     1,     1,     1,     3,
       2,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     2,     3,
       3,     9,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     1,     2,     2,     1,     1,     3,
       3,     1,     1,     1,     3,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     4,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     5,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     3,     1,     1,
       2,     1,     3,     4,     3,     1,     3,     1,     1,     1,
       4,     3,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     3,     1,     1,     2,     1,     1,     2,     2,     2,
       2,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, nft, scanner, state, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, nft, scanner, state); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (nft);
  YY_USE (scanner);
  YY_USE (state);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, nft, scanner, state);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), nft, scanner, state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, nft, scanner, state); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (nft);
  YY_USE (scanner);
  YY_USE (state);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_STRING: /* "string"  */
#line 380 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6211 "src/parser_bison.c"
        break;

    case YYSYMBOL_QUOTED_STRING: /* "quoted string"  */
#line 380 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6217 "src/parser_bison.c"
        break;

    case YYSYMBOL_ASTERISK_STRING: /* "string with a trailing asterisk"  */
#line 380 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6223 "src/parser_bison.c"
        break;

    case YYSYMBOL_line: /* line  */
#line 704 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6229 "src/parser_bison.c"
        break;

    case YYSYMBOL_base_cmd: /* base_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6235 "src/parser_bison.c"
        break;

    case YYSYMBOL_add_cmd: /* add_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6241 "src/parser_bison.c"
        break;

    case YYSYMBOL_replace_cmd: /* replace_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6247 "src/parser_bison.c"
        break;

    case YYSYMBOL_create_cmd: /* create_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6253 "src/parser_bison.c"
        break;

    case YYSYMBOL_insert_cmd: /* insert_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6259 "src/parser_bison.c"
        break;

    case YYSYMBOL_table_or_id_spec: /* table_or_id_spec  */
#line 710 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6265 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_or_id_spec: /* chain_or_id_spec  */
#line 712 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6271 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_or_id_spec: /* set_or_id_spec  */
#line 717 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6277 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_or_id_spec: /* obj_or_id_spec  */
#line 719 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6283 "src/parser_bison.c"
        break;

    case YYSYMBOL_delete_cmd: /* delete_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6289 "src/parser_bison.c"
        break;

    case YYSYMBOL_destroy_cmd: /* destroy_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6295 "src/parser_bison.c"
        break;

    case YYSYMBOL_get_cmd: /* get_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6301 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_cmd: /* list_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6307 "src/parser_bison.c"
        break;

    case YYSYMBOL_basehook_device_name: /* basehook_device_name  */
#line 733 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6313 "src/parser_bison.c"
        break;

    case YYSYMBOL_basehook_spec: /* basehook_spec  */
#line 725 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6319 "src/parser_bison.c"
        break;

    case YYSYMBOL_reset_cmd: /* reset_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6325 "src/parser_bison.c"
        break;

    case YYSYMBOL_flush_cmd: /* flush_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6331 "src/parser_bison.c"
        break;

    case YYSYMBOL_rename_cmd: /* rename_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6337 "src/parser_bison.c"
        break;

    case YYSYMBOL_import_cmd: /* import_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6343 "src/parser_bison.c"
        break;

    case YYSYMBOL_export_cmd: /* export_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6349 "src/parser_bison.c"
        break;

    case YYSYMBOL_monitor_cmd: /* monitor_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6355 "src/parser_bison.c"
        break;

    case YYSYMBOL_monitor_event: /* monitor_event  */
#line 959 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6361 "src/parser_bison.c"
        break;

    case YYSYMBOL_describe_cmd: /* describe_cmd  */
#line 707 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6367 "src/parser_bison.c"
        break;

    case YYSYMBOL_table_block_alloc: /* table_block_alloc  */
#line 739 "src/parser_bison.y"
            { close_scope(state); table_free(((*yyvaluep).table)); }
#line 6373 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_block_alloc: /* chain_block_alloc  */
#line 741 "src/parser_bison.y"
            { close_scope(state); chain_free(((*yyvaluep).chain)); }
#line 6379 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_verdict_expr: /* typeof_verdict_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6385 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_data_expr: /* typeof_data_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6391 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_expr: /* typeof_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6397 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_block_alloc: /* set_block_alloc  */
#line 752 "src/parser_bison.y"
            { set_free(((*yyvaluep).set)); }
#line 6403 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_key_expr: /* typeof_key_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6409 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_block_expr: /* set_block_expr  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6415 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_block_alloc: /* map_block_alloc  */
#line 755 "src/parser_bison.y"
            { set_free(((*yyvaluep).set)); }
#line 6421 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_block_alloc: /* flowtable_block_alloc  */
#line 759 "src/parser_bison.y"
            { flowtable_free(((*yyvaluep).flowtable)); }
#line 6427 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_expr: /* flowtable_expr  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6433 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_list_expr: /* flowtable_list_expr  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6439 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_expr_member: /* flowtable_expr_member  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6445 "src/parser_bison.c"
        break;

    case YYSYMBOL_data_type_atom_expr: /* data_type_atom_expr  */
#line 701 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6451 "src/parser_bison.c"
        break;

    case YYSYMBOL_data_type_expr: /* data_type_expr  */
#line 701 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6457 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_block_alloc: /* obj_block_alloc  */
#line 762 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6463 "src/parser_bison.c"
        break;

    case YYSYMBOL_type_identifier: /* type_identifier  */
#line 696 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6469 "src/parser_bison.c"
        break;

    case YYSYMBOL_prio_spec: /* prio_spec  */
#line 730 "src/parser_bison.y"
            { expr_free(((*yyvaluep).prio_spec).expr); }
#line 6475 "src/parser_bison.c"
        break;

    case YYSYMBOL_extended_prio_name: /* extended_prio_name  */
#line 733 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6481 "src/parser_bison.c"
        break;

    case YYSYMBOL_extended_prio_spec: /* extended_prio_spec  */
#line 730 "src/parser_bison.y"
            { expr_free(((*yyvaluep).prio_spec).expr); }
#line 6487 "src/parser_bison.c"
        break;

    case YYSYMBOL_dev_spec: /* dev_spec  */
#line 736 "src/parser_bison.y"
            { free(((*yyvaluep).expr)); }
#line 6493 "src/parser_bison.c"
        break;

    case YYSYMBOL_policy_expr: /* policy_expr  */
#line 816 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6499 "src/parser_bison.c"
        break;

    case YYSYMBOL_identifier: /* identifier  */
#line 696 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6505 "src/parser_bison.c"
        break;

    case YYSYMBOL_string: /* string  */
#line 696 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6511 "src/parser_bison.c"
        break;

    case YYSYMBOL_table_spec: /* table_spec  */
#line 710 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6517 "src/parser_bison.c"
        break;

    case YYSYMBOL_tableid_spec: /* tableid_spec  */
#line 710 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6523 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_spec: /* chain_spec  */
#line 712 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6529 "src/parser_bison.c"
        break;

    case YYSYMBOL_chainid_spec: /* chainid_spec  */
#line 712 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6535 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_identifier: /* chain_identifier  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6541 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_spec: /* set_spec  */
#line 717 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6547 "src/parser_bison.c"
        break;

    case YYSYMBOL_setid_spec: /* setid_spec  */
#line 717 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6553 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_identifier: /* set_identifier  */
#line 722 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6559 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_spec: /* flowtable_spec  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6565 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtableid_spec: /* flowtableid_spec  */
#line 722 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6571 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_spec: /* obj_spec  */
#line 719 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6577 "src/parser_bison.c"
        break;

    case YYSYMBOL_objid_spec: /* objid_spec  */
#line 719 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6583 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_identifier: /* obj_identifier  */
#line 722 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6589 "src/parser_bison.c"
        break;

    case YYSYMBOL_handle_spec: /* handle_spec  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6595 "src/parser_bison.c"
        break;

    case YYSYMBOL_position_spec: /* position_spec  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6601 "src/parser_bison.c"
        break;

    case YYSYMBOL_index_spec: /* index_spec  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6607 "src/parser_bison.c"
        break;

    case YYSYMBOL_rule_position: /* rule_position  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6613 "src/parser_bison.c"
        break;

    case YYSYMBOL_ruleid_spec: /* ruleid_spec  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6619 "src/parser_bison.c"
        break;

    case YYSYMBOL_comment_spec: /* comment_spec  */
#line 696 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6625 "src/parser_bison.c"
        break;

    case YYSYMBOL_ruleset_spec: /* ruleset_spec  */
#line 715 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6631 "src/parser_bison.c"
        break;

    case YYSYMBOL_rule: /* rule  */
#line 743 "src/parser_bison.y"
            { rule_free(((*yyvaluep).rule)); }
#line 6637 "src/parser_bison.c"
        break;

    case YYSYMBOL_stmt_list: /* stmt_list  */
#line 765 "src/parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); free(((*yyvaluep).list)); }
#line 6643 "src/parser_bison.c"
        break;

    case YYSYMBOL_stateful_stmt_list: /* stateful_stmt_list  */
#line 765 "src/parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); free(((*yyvaluep).list)); }
#line 6649 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_counter: /* objref_stmt_counter  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6655 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_limit: /* objref_stmt_limit  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6661 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_quota: /* objref_stmt_quota  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6667 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_synproxy: /* objref_stmt_synproxy  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6673 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_ct: /* objref_stmt_ct  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6679 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt: /* objref_stmt  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6685 "src/parser_bison.c"
        break;

    case YYSYMBOL_stateful_stmt: /* stateful_stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6691 "src/parser_bison.c"
        break;

    case YYSYMBOL_stmt: /* stmt  */
#line 767 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6697 "src/parser_bison.c"
        break;

    case YYSYMBOL_xt_stmt: /* xt_stmt  */
#line 983 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6703 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_stmt: /* chain_stmt  */
#line 795 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6709 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_stmt: /* verdict_stmt  */
#line 767 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6715 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_stmt: /* verdict_map_stmt  */
#line 853 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6721 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_expr: /* verdict_map_expr  */
#line 856 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6727 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_list_expr: /* verdict_map_list_expr  */
#line 856 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6733 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_list_member_expr: /* verdict_map_list_member_expr  */
#line 856 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6739 "src/parser_bison.c"
        break;

    case YYSYMBOL_connlimit_stmt: /* connlimit_stmt  */
#line 783 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6745 "src/parser_bison.c"
        break;

    case YYSYMBOL_counter_stmt: /* counter_stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6751 "src/parser_bison.c"
        break;

    case YYSYMBOL_counter_stmt_alloc: /* counter_stmt_alloc  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6757 "src/parser_bison.c"
        break;

    case YYSYMBOL_last_stmt: /* last_stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6763 "src/parser_bison.c"
        break;

    case YYSYMBOL_log_stmt: /* log_stmt  */
#line 780 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6769 "src/parser_bison.c"
        break;

    case YYSYMBOL_log_stmt_alloc: /* log_stmt_alloc  */
#line 780 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6775 "src/parser_bison.c"
        break;

    case YYSYMBOL_limit_stmt: /* limit_stmt  */
#line 783 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6781 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_unit: /* quota_unit  */
#line 733 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6787 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_stmt: /* quota_stmt  */
#line 783 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6793 "src/parser_bison.c"
        break;

    case YYSYMBOL_reject_stmt: /* reject_stmt  */
#line 786 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6799 "src/parser_bison.c"
        break;

    case YYSYMBOL_reject_stmt_alloc: /* reject_stmt_alloc  */
#line 786 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6805 "src/parser_bison.c"
        break;

    case YYSYMBOL_reject_with_expr: /* reject_with_expr  */
#line 801 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6811 "src/parser_bison.c"
        break;

    case YYSYMBOL_nat_stmt: /* nat_stmt  */
#line 788 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6817 "src/parser_bison.c"
        break;

    case YYSYMBOL_nat_stmt_alloc: /* nat_stmt_alloc  */
#line 788 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6823 "src/parser_bison.c"
        break;

    case YYSYMBOL_tproxy_stmt: /* tproxy_stmt  */
#line 791 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6829 "src/parser_bison.c"
        break;

    case YYSYMBOL_synproxy_stmt: /* synproxy_stmt  */
#line 793 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6835 "src/parser_bison.c"
        break;

    case YYSYMBOL_synproxy_stmt_alloc: /* synproxy_stmt_alloc  */
#line 793 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6841 "src/parser_bison.c"
        break;

    case YYSYMBOL_synproxy_obj: /* synproxy_obj  */
#line 879 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6847 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_stmt_expr: /* primary_stmt_expr  */
#line 840 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6853 "src/parser_bison.c"
        break;

    case YYSYMBOL_shift_stmt_expr: /* shift_stmt_expr  */
#line 842 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6859 "src/parser_bison.c"
        break;

    case YYSYMBOL_and_stmt_expr: /* and_stmt_expr  */
#line 844 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6865 "src/parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_stmt_expr: /* exclusive_or_stmt_expr  */
#line 844 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6871 "src/parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_stmt_expr: /* inclusive_or_stmt_expr  */
#line 844 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6877 "src/parser_bison.c"
        break;

    case YYSYMBOL_basic_stmt_expr: /* basic_stmt_expr  */
#line 840 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6883 "src/parser_bison.c"
        break;

    case YYSYMBOL_concat_stmt_expr: /* concat_stmt_expr  */
#line 832 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6889 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_stmt_expr_set: /* map_stmt_expr_set  */
#line 832 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6895 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_stmt_expr: /* map_stmt_expr  */
#line 832 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6901 "src/parser_bison.c"
        break;

    case YYSYMBOL_prefix_stmt_expr: /* prefix_stmt_expr  */
#line 837 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6907 "src/parser_bison.c"
        break;

    case YYSYMBOL_range_stmt_expr: /* range_stmt_expr  */
#line 837 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6913 "src/parser_bison.c"
        break;

    case YYSYMBOL_multiton_stmt_expr: /* multiton_stmt_expr  */
#line 835 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6919 "src/parser_bison.c"
        break;

    case YYSYMBOL_stmt_expr: /* stmt_expr  */
#line 832 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6925 "src/parser_bison.c"
        break;

    case YYSYMBOL_masq_stmt: /* masq_stmt  */
#line 788 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6931 "src/parser_bison.c"
        break;

    case YYSYMBOL_masq_stmt_alloc: /* masq_stmt_alloc  */
#line 788 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6937 "src/parser_bison.c"
        break;

    case YYSYMBOL_redir_stmt: /* redir_stmt  */
#line 788 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6943 "src/parser_bison.c"
        break;

    case YYSYMBOL_redir_stmt_alloc: /* redir_stmt_alloc  */
#line 788 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6949 "src/parser_bison.c"
        break;

    case YYSYMBOL_dup_stmt: /* dup_stmt  */
#line 804 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6955 "src/parser_bison.c"
        break;

    case YYSYMBOL_fwd_stmt: /* fwd_stmt  */
#line 806 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6961 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt: /* queue_stmt  */
#line 799 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6967 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_compat: /* queue_stmt_compat  */
#line 799 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6973 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_alloc: /* queue_stmt_alloc  */
#line 799 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6979 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_expr: /* queue_expr  */
#line 801 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6985 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_expr_simple: /* queue_stmt_expr_simple  */
#line 801 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6991 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_expr: /* queue_stmt_expr  */
#line 801 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6997 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_stmt: /* set_elem_expr_stmt  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7003 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_stmt_alloc: /* set_elem_expr_stmt_alloc  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7009 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_stmt: /* set_stmt  */
#line 808 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7015 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_stmt: /* map_stmt  */
#line 811 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7021 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_stmt: /* meter_stmt  */
#line 813 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7027 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_stmt_alloc: /* meter_stmt_alloc  */
#line 813 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7033 "src/parser_bison.c"
        break;

    case YYSYMBOL_match_stmt: /* match_stmt  */
#line 767 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7039 "src/parser_bison.c"
        break;

    case YYSYMBOL_variable_expr: /* variable_expr  */
#line 816 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7045 "src/parser_bison.c"
        break;

    case YYSYMBOL_symbol_expr: /* symbol_expr  */
#line 816 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7051 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_ref_expr: /* set_ref_expr  */
#line 824 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7057 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_ref_symbol_expr: /* set_ref_symbol_expr  */
#line 824 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7063 "src/parser_bison.c"
        break;

    case YYSYMBOL_integer_expr: /* integer_expr  */
#line 816 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7069 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_expr: /* primary_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7075 "src/parser_bison.c"
        break;

    case YYSYMBOL_fib_expr: /* fib_expr  */
#line 950 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7081 "src/parser_bison.c"
        break;

    case YYSYMBOL_osf_expr: /* osf_expr  */
#line 955 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7087 "src/parser_bison.c"
        break;

    case YYSYMBOL_shift_expr: /* shift_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7093 "src/parser_bison.c"
        break;

    case YYSYMBOL_and_expr: /* and_expr  */
#line 818 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7099 "src/parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_expr: /* exclusive_or_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7105 "src/parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_expr: /* inclusive_or_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7111 "src/parser_bison.c"
        break;

    case YYSYMBOL_basic_expr: /* basic_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7117 "src/parser_bison.c"
        break;

    case YYSYMBOL_concat_expr: /* concat_expr  */
#line 847 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7123 "src/parser_bison.c"
        break;

    case YYSYMBOL_prefix_rhs_expr: /* prefix_rhs_expr  */
#line 829 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7129 "src/parser_bison.c"
        break;

    case YYSYMBOL_range_rhs_expr: /* range_rhs_expr  */
#line 829 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7135 "src/parser_bison.c"
        break;

    case YYSYMBOL_multiton_rhs_expr: /* multiton_rhs_expr  */
#line 827 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7141 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_expr: /* map_expr  */
#line 850 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7147 "src/parser_bison.c"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 869 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7153 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_expr: /* set_expr  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7159 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_list_expr: /* set_list_expr  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7165 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_list_member_expr: /* set_list_member_expr  */
#line 859 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7171 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_key_expr: /* meter_key_expr  */
#line 866 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7177 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_key_expr_alloc: /* meter_key_expr_alloc  */
#line 866 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7183 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr: /* set_elem_expr  */
#line 861 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7189 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_key_expr: /* set_elem_key_expr  */
#line 1006 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7195 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_alloc: /* set_elem_expr_alloc  */
#line 861 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7201 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_stmt_list: /* set_elem_stmt_list  */
#line 765 "src/parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); free(((*yyvaluep).list)); }
#line 7207 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_stmt: /* set_elem_stmt  */
#line 767 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7213 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_lhs_expr: /* set_lhs_expr  */
#line 861 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7219 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_rhs_expr: /* set_rhs_expr  */
#line 861 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7225 "src/parser_bison.c"
        break;

    case YYSYMBOL_initializer_expr: /* initializer_expr  */
#line 869 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7231 "src/parser_bison.c"
        break;

    case YYSYMBOL_counter_obj: /* counter_obj  */
#line 879 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7237 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_obj: /* quota_obj  */
#line 879 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7243 "src/parser_bison.c"
        break;

    case YYSYMBOL_secmark_obj: /* secmark_obj  */
#line 879 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7249 "src/parser_bison.c"
        break;

    case YYSYMBOL_timeout_states: /* timeout_states  */
#line 999 "src/parser_bison.y"
            { timeout_states_free(((*yyvaluep).list)); }
#line 7255 "src/parser_bison.c"
        break;

    case YYSYMBOL_timeout_state: /* timeout_state  */
#line 996 "src/parser_bison.y"
            { timeout_state_free(((*yyvaluep).timeout_state)); }
#line 7261 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_obj_alloc: /* ct_obj_alloc  */
#line 879 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7267 "src/parser_bison.c"
        break;

    case YYSYMBOL_limit_obj: /* limit_obj  */
#line 879 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7273 "src/parser_bison.c"
        break;

    case YYSYMBOL_relational_expr: /* relational_expr  */
#line 882 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7279 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_rhs_expr: /* list_rhs_expr  */
#line 874 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7285 "src/parser_bison.c"
        break;

    case YYSYMBOL_rhs_expr: /* rhs_expr  */
#line 872 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7291 "src/parser_bison.c"
        break;

    case YYSYMBOL_shift_rhs_expr: /* shift_rhs_expr  */
#line 874 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7297 "src/parser_bison.c"
        break;

    case YYSYMBOL_and_rhs_expr: /* and_rhs_expr  */
#line 876 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7303 "src/parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_rhs_expr: /* exclusive_or_rhs_expr  */
#line 876 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7309 "src/parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_rhs_expr: /* inclusive_or_rhs_expr  */
#line 876 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7315 "src/parser_bison.c"
        break;

    case YYSYMBOL_basic_rhs_expr: /* basic_rhs_expr  */
#line 872 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7321 "src/parser_bison.c"
        break;

    case YYSYMBOL_concat_rhs_expr: /* concat_rhs_expr  */
#line 872 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7327 "src/parser_bison.c"
        break;

    case YYSYMBOL_boolean_expr: /* boolean_expr  */
#line 986 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7333 "src/parser_bison.c"
        break;

    case YYSYMBOL_keyword_expr: /* keyword_expr  */
#line 869 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7339 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_rhs_expr: /* primary_rhs_expr  */
#line 874 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7345 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_expr: /* verdict_expr  */
#line 816 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7351 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_expr: /* chain_expr  */
#line 816 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7357 "src/parser_bison.c"
        break;

    case YYSYMBOL_meta_expr: /* meta_expr  */
#line 932 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7363 "src/parser_bison.c"
        break;

    case YYSYMBOL_meta_stmt: /* meta_stmt  */
#line 778 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7369 "src/parser_bison.c"
        break;

    case YYSYMBOL_socket_expr: /* socket_expr  */
#line 936 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7375 "src/parser_bison.c"
        break;

    case YYSYMBOL_numgen_expr: /* numgen_expr  */
#line 897 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7381 "src/parser_bison.c"
        break;

    case YYSYMBOL_xfrm_expr: /* xfrm_expr  */
#line 1003 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7387 "src/parser_bison.c"
        break;

    case YYSYMBOL_hash_expr: /* hash_expr  */
#line 897 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7393 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt_expr: /* rt_expr  */
#line 942 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7399 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_expr: /* ct_expr  */
#line 946 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7405 "src/parser_bison.c"
        break;

    case YYSYMBOL_symbol_stmt_expr: /* symbol_stmt_expr  */
#line 874 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7411 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_stmt_expr: /* list_stmt_expr  */
#line 842 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7417 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_stmt: /* ct_stmt  */
#line 776 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7423 "src/parser_bison.c"
        break;

    case YYSYMBOL_payload_stmt: /* payload_stmt  */
#line 774 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7429 "src/parser_bison.c"
        break;

    case YYSYMBOL_payload_expr: /* payload_expr  */
#line 886 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7435 "src/parser_bison.c"
        break;

    case YYSYMBOL_payload_raw_expr: /* payload_raw_expr  */
#line 886 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7441 "src/parser_bison.c"
        break;

    case YYSYMBOL_eth_hdr_expr: /* eth_hdr_expr  */
#line 891 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7447 "src/parser_bison.c"
        break;

    case YYSYMBOL_vlan_hdr_expr: /* vlan_hdr_expr  */
#line 891 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7453 "src/parser_bison.c"
        break;

    case YYSYMBOL_arp_hdr_expr: /* arp_hdr_expr  */
#line 894 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7459 "src/parser_bison.c"
        break;

    case YYSYMBOL_ip_hdr_expr: /* ip_hdr_expr  */
#line 897 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7465 "src/parser_bison.c"
        break;

    case YYSYMBOL_icmp_hdr_expr: /* icmp_hdr_expr  */
#line 897 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7471 "src/parser_bison.c"
        break;

    case YYSYMBOL_igmp_hdr_expr: /* igmp_hdr_expr  */
#line 897 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7477 "src/parser_bison.c"
        break;

    case YYSYMBOL_ip6_hdr_expr: /* ip6_hdr_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7483 "src/parser_bison.c"
        break;

    case YYSYMBOL_icmp6_hdr_expr: /* icmp6_hdr_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7489 "src/parser_bison.c"
        break;

    case YYSYMBOL_auth_hdr_expr: /* auth_hdr_expr  */
#line 904 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7495 "src/parser_bison.c"
        break;

    case YYSYMBOL_esp_hdr_expr: /* esp_hdr_expr  */
#line 904 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7501 "src/parser_bison.c"
        break;

    case YYSYMBOL_comp_hdr_expr: /* comp_hdr_expr  */
#line 904 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7507 "src/parser_bison.c"
        break;

    case YYSYMBOL_udp_hdr_expr: /* udp_hdr_expr  */
#line 907 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7513 "src/parser_bison.c"
        break;

    case YYSYMBOL_udplite_hdr_expr: /* udplite_hdr_expr  */
#line 907 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7519 "src/parser_bison.c"
        break;

    case YYSYMBOL_tcp_hdr_expr: /* tcp_hdr_expr  */
#line 965 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7525 "src/parser_bison.c"
        break;

    case YYSYMBOL_inner_inet_expr: /* inner_inet_expr  */
#line 973 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7531 "src/parser_bison.c"
        break;

    case YYSYMBOL_inner_eth_expr: /* inner_eth_expr  */
#line 973 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7537 "src/parser_bison.c"
        break;

    case YYSYMBOL_inner_expr: /* inner_expr  */
#line 973 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7543 "src/parser_bison.c"
        break;

    case YYSYMBOL_vxlan_hdr_expr: /* vxlan_hdr_expr  */
#line 976 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7549 "src/parser_bison.c"
        break;

    case YYSYMBOL_geneve_hdr_expr: /* geneve_hdr_expr  */
#line 976 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7555 "src/parser_bison.c"
        break;

    case YYSYMBOL_gre_hdr_expr: /* gre_hdr_expr  */
#line 976 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7561 "src/parser_bison.c"
        break;

    case YYSYMBOL_gretap_hdr_expr: /* gretap_hdr_expr  */
#line 976 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7567 "src/parser_bison.c"
        break;

    case YYSYMBOL_optstrip_stmt: /* optstrip_stmt  */
#line 980 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7573 "src/parser_bison.c"
        break;

    case YYSYMBOL_dccp_hdr_expr: /* dccp_hdr_expr  */
#line 910 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7579 "src/parser_bison.c"
        break;

    case YYSYMBOL_sctp_chunk_alloc: /* sctp_chunk_alloc  */
#line 910 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7585 "src/parser_bison.c"
        break;

    case YYSYMBOL_sctp_hdr_expr: /* sctp_hdr_expr  */
#line 910 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7591 "src/parser_bison.c"
        break;

    case YYSYMBOL_th_hdr_expr: /* th_hdr_expr  */
#line 916 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7597 "src/parser_bison.c"
        break;

    case YYSYMBOL_exthdr_expr: /* exthdr_expr  */
#line 920 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7603 "src/parser_bison.c"
        break;

    case YYSYMBOL_hbh_hdr_expr: /* hbh_hdr_expr  */
#line 922 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7609 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt_hdr_expr: /* rt_hdr_expr  */
#line 925 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7615 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt0_hdr_expr: /* rt0_hdr_expr  */
#line 925 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7621 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt2_hdr_expr: /* rt2_hdr_expr  */
#line 925 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7627 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt4_hdr_expr: /* rt4_hdr_expr  */
#line 925 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7633 "src/parser_bison.c"
        break;

    case YYSYMBOL_frag_hdr_expr: /* frag_hdr_expr  */
#line 922 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7639 "src/parser_bison.c"
        break;

    case YYSYMBOL_dst_hdr_expr: /* dst_hdr_expr  */
#line 922 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7645 "src/parser_bison.c"
        break;

    case YYSYMBOL_mh_hdr_expr: /* mh_hdr_expr  */
#line 928 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7651 "src/parser_bison.c"
        break;

    case YYSYMBOL_exthdr_exists_expr: /* exthdr_exists_expr  */
#line 990 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7657 "src/parser_bison.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */


/* User initialization code.  */
#line 216 "src/parser_bison.y"
{
	location_init(scanner, state, &yylloc);
	if (nft->debug_mask & NFT_DEBUG_SCANNER)
		nft_set_debug(1, scanner);
	if (nft->debug_mask & NFT_DEBUG_PARSER)
		yydebug = 1;
}

#line 7763 "src/parser_bison.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= TOKEN_EOF)
    {
      yychar = TOKEN_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* input: input line  */
#line 1012 "src/parser_bison.y"
                        {
				if ((yyvsp[0].cmd) != NULL) {
					(yyvsp[0].cmd)->location = (yylsp[0]);
					list_add_tail(&(yyvsp[0].cmd)->list, state->cmds);
				}
			}
#line 7981 "src/parser_bison.c"
    break;

  case 8: /* close_scope_ah: %empty  */
#line 1028 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_AH); }
#line 7987 "src/parser_bison.c"
    break;

  case 9: /* close_scope_arp: %empty  */
#line 1029 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ARP); }
#line 7993 "src/parser_bison.c"
    break;

  case 10: /* close_scope_at: %empty  */
#line 1030 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_AT); }
#line 7999 "src/parser_bison.c"
    break;

  case 11: /* close_scope_comp: %empty  */
#line 1031 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_COMP); }
#line 8005 "src/parser_bison.c"
    break;

  case 12: /* close_scope_ct: %empty  */
#line 1032 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CT); }
#line 8011 "src/parser_bison.c"
    break;

  case 13: /* close_scope_counter: %empty  */
#line 1033 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_COUNTER); }
#line 8017 "src/parser_bison.c"
    break;

  case 14: /* close_scope_last: %empty  */
#line 1034 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_LAST); }
#line 8023 "src/parser_bison.c"
    break;

  case 15: /* close_scope_dccp: %empty  */
#line 1035 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_DCCP); }
#line 8029 "src/parser_bison.c"
    break;

  case 16: /* close_scope_destroy: %empty  */
#line 1036 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_DESTROY); }
#line 8035 "src/parser_bison.c"
    break;

  case 17: /* close_scope_dst: %empty  */
#line 1037 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_DST); }
#line 8041 "src/parser_bison.c"
    break;

  case 18: /* close_scope_dup: %empty  */
#line 1038 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_DUP); }
#line 8047 "src/parser_bison.c"
    break;

  case 19: /* close_scope_esp: %empty  */
#line 1039 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_ESP); }
#line 8053 "src/parser_bison.c"
    break;

  case 20: /* close_scope_eth: %empty  */
#line 1040 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ETH); }
#line 8059 "src/parser_bison.c"
    break;

  case 21: /* close_scope_export: %empty  */
#line 1041 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_EXPORT); }
#line 8065 "src/parser_bison.c"
    break;

  case 22: /* close_scope_fib: %empty  */
#line 1042 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_FIB); }
#line 8071 "src/parser_bison.c"
    break;

  case 23: /* close_scope_frag: %empty  */
#line 1043 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_FRAG); }
#line 8077 "src/parser_bison.c"
    break;

  case 24: /* close_scope_fwd: %empty  */
#line 1044 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_FWD); }
#line 8083 "src/parser_bison.c"
    break;

  case 25: /* close_scope_gre: %empty  */
#line 1045 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_GRE); }
#line 8089 "src/parser_bison.c"
    break;

  case 26: /* close_scope_hash: %empty  */
#line 1046 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_HASH); }
#line 8095 "src/parser_bison.c"
    break;

  case 27: /* close_scope_hbh: %empty  */
#line 1047 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_HBH); }
#line 8101 "src/parser_bison.c"
    break;

  case 28: /* close_scope_ip: %empty  */
#line 1048 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IP); }
#line 8107 "src/parser_bison.c"
    break;

  case 29: /* close_scope_ip6: %empty  */
#line 1049 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IP6); }
#line 8113 "src/parser_bison.c"
    break;

  case 30: /* close_scope_vlan: %empty  */
#line 1050 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_VLAN); }
#line 8119 "src/parser_bison.c"
    break;

  case 31: /* close_scope_icmp: %empty  */
#line 1051 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ICMP); }
#line 8125 "src/parser_bison.c"
    break;

  case 32: /* close_scope_igmp: %empty  */
#line 1052 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IGMP); }
#line 8131 "src/parser_bison.c"
    break;

  case 33: /* close_scope_import: %empty  */
#line 1053 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_IMPORT); }
#line 8137 "src/parser_bison.c"
    break;

  case 34: /* close_scope_ipsec: %empty  */
#line 1054 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_IPSEC); }
#line 8143 "src/parser_bison.c"
    break;

  case 35: /* close_scope_list: %empty  */
#line 1055 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_LIST); }
#line 8149 "src/parser_bison.c"
    break;

  case 36: /* close_scope_limit: %empty  */
#line 1056 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_LIMIT); }
#line 8155 "src/parser_bison.c"
    break;

  case 37: /* close_scope_meta: %empty  */
#line 1057 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_META); }
#line 8161 "src/parser_bison.c"
    break;

  case 38: /* close_scope_mh: %empty  */
#line 1058 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_MH); }
#line 8167 "src/parser_bison.c"
    break;

  case 39: /* close_scope_monitor: %empty  */
#line 1059 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_MONITOR); }
#line 8173 "src/parser_bison.c"
    break;

  case 40: /* close_scope_nat: %empty  */
#line 1060 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_NAT); }
#line 8179 "src/parser_bison.c"
    break;

  case 41: /* close_scope_numgen: %empty  */
#line 1061 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_NUMGEN); }
#line 8185 "src/parser_bison.c"
    break;

  case 42: /* close_scope_osf: %empty  */
#line 1062 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_OSF); }
#line 8191 "src/parser_bison.c"
    break;

  case 43: /* close_scope_policy: %empty  */
#line 1063 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_POLICY); }
#line 8197 "src/parser_bison.c"
    break;

  case 44: /* close_scope_quota: %empty  */
#line 1064 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_QUOTA); }
#line 8203 "src/parser_bison.c"
    break;

  case 45: /* close_scope_queue: %empty  */
#line 1065 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_QUEUE); }
#line 8209 "src/parser_bison.c"
    break;

  case 46: /* close_scope_reject: %empty  */
#line 1066 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_REJECT); }
#line 8215 "src/parser_bison.c"
    break;

  case 47: /* close_scope_reset: %empty  */
#line 1067 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_RESET); }
#line 8221 "src/parser_bison.c"
    break;

  case 48: /* close_scope_rt: %empty  */
#line 1068 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_RT); }
#line 8227 "src/parser_bison.c"
    break;

  case 49: /* close_scope_sctp: %empty  */
#line 1069 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_SCTP); }
#line 8233 "src/parser_bison.c"
    break;

  case 50: /* close_scope_sctp_chunk: %empty  */
#line 1070 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_SCTP_CHUNK); }
#line 8239 "src/parser_bison.c"
    break;

  case 51: /* close_scope_secmark: %empty  */
#line 1071 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_SECMARK); }
#line 8245 "src/parser_bison.c"
    break;

  case 52: /* close_scope_socket: %empty  */
#line 1072 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_SOCKET); }
#line 8251 "src/parser_bison.c"
    break;

  case 53: /* close_scope_tcp: %empty  */
#line 1073 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_TCP); }
#line 8257 "src/parser_bison.c"
    break;

  case 54: /* close_scope_tproxy: %empty  */
#line 1074 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_TPROXY); }
#line 8263 "src/parser_bison.c"
    break;

  case 55: /* close_scope_type: %empty  */
#line 1075 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_TYPE); }
#line 8269 "src/parser_bison.c"
    break;

  case 56: /* close_scope_th: %empty  */
#line 1076 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_TH); }
#line 8275 "src/parser_bison.c"
    break;

  case 57: /* close_scope_udp: %empty  */
#line 1077 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_UDP); }
#line 8281 "src/parser_bison.c"
    break;

  case 58: /* close_scope_udplite: %empty  */
#line 1078 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_UDPLITE); }
#line 8287 "src/parser_bison.c"
    break;

  case 59: /* close_scope_log: %empty  */
#line 1080 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_LOG); }
#line 8293 "src/parser_bison.c"
    break;

  case 60: /* close_scope_synproxy: %empty  */
#line 1081 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_SYNPROXY); }
#line 8299 "src/parser_bison.c"
    break;

  case 61: /* close_scope_xt: %empty  */
#line 1082 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_XT); }
#line 8305 "src/parser_bison.c"
    break;

  case 62: /* common_block: "include" "quoted string" stmt_separator  */
#line 1085 "src/parser_bison.y"
                        {
				if (scanner_include_file(nft, scanner, (yyvsp[-1].string), &(yyloc)) < 0) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				free_const((yyvsp[-1].string));
			}
#line 8317 "src/parser_bison.c"
    break;

  case 63: /* common_block: "define" identifier '=' initializer_expr stmt_separator  */
#line 1093 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				if (symbol_lookup(scope, (yyvsp[-3].string)) != NULL) {
					erec_queue(error(&(yylsp[-3]), "redefinition of symbol '%s'", (yyvsp[-3].string)),
						   state->msgs);
					expr_free((yyvsp[-1].expr));
					free_const((yyvsp[-3].string));
					YYERROR;
				}

				symbol_bind(scope, (yyvsp[-3].string), (yyvsp[-1].expr));
				free_const((yyvsp[-3].string));
			}
#line 8336 "src/parser_bison.c"
    break;

  case 64: /* common_block: "redefine" identifier '=' initializer_expr stmt_separator  */
#line 1108 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				symbol_bind(scope, (yyvsp[-3].string), (yyvsp[-1].expr));
				free_const((yyvsp[-3].string));
			}
#line 8347 "src/parser_bison.c"
    break;

  case 65: /* common_block: "undefine" identifier stmt_separator  */
#line 1115 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				if (symbol_unbind(scope, (yyvsp[-1].string)) < 0) {
					erec_queue(error(&(yylsp[-1]), "undefined symbol '%s'", (yyvsp[-1].string)),
						   state->msgs);
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				free_const((yyvsp[-1].string));
			}
#line 8363 "src/parser_bison.c"
    break;

  case 66: /* common_block: error stmt_separator  */
#line 1127 "src/parser_bison.y"
                        {
				if (++state->nerrs == nft->parser_max_errors)
					YYABORT;
				yyerrok;
			}
#line 8373 "src/parser_bison.c"
    break;

  case 67: /* line: common_block  */
#line 1134 "src/parser_bison.y"
                                                                { (yyval.cmd) = NULL; }
#line 8379 "src/parser_bison.c"
    break;

  case 68: /* line: stmt_separator  */
#line 1135 "src/parser_bison.y"
                                                                { (yyval.cmd) = NULL; }
#line 8385 "src/parser_bison.c"
    break;

  case 69: /* line: base_cmd stmt_separator  */
#line 1136 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8391 "src/parser_bison.c"
    break;

  case 70: /* line: base_cmd "end of file"  */
#line 1138 "src/parser_bison.y"
                        {
				/*
				 * Very hackish workaround for bison >= 2.4: previous versions
				 * terminated parsing after EOF, 2.4+ tries to get further input
				 * in 'input' and calls the scanner again, causing a crash when
				 * the final input buffer has been popped. Terminate manually to
				 * avoid this. The correct fix should be to adjust the grammar
				 * to accept EOF in input, but for unknown reasons it does not
				 * work.
				 */
				if ((yyvsp[-1].cmd) != NULL) {
					(yyvsp[-1].cmd)->location = (yylsp[-1]);
					list_add_tail(&(yyvsp[-1].cmd)->list, state->cmds);
				}
				(yyval.cmd) = NULL;
				YYACCEPT;
			}
#line 8413 "src/parser_bison.c"
    break;

  case 71: /* base_cmd: add_cmd  */
#line 1157 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8419 "src/parser_bison.c"
    break;

  case 72: /* base_cmd: "add" add_cmd  */
#line 1158 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8425 "src/parser_bison.c"
    break;

  case 73: /* base_cmd: "replace" replace_cmd  */
#line 1159 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8431 "src/parser_bison.c"
    break;

  case 74: /* base_cmd: "create" create_cmd  */
#line 1160 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8437 "src/parser_bison.c"
    break;

  case 75: /* base_cmd: "insert" insert_cmd  */
#line 1161 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8443 "src/parser_bison.c"
    break;

  case 76: /* base_cmd: "delete" delete_cmd  */
#line 1162 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8449 "src/parser_bison.c"
    break;

  case 77: /* base_cmd: "get" get_cmd  */
#line 1163 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8455 "src/parser_bison.c"
    break;

  case 78: /* base_cmd: "list" list_cmd close_scope_list  */
#line 1164 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8461 "src/parser_bison.c"
    break;

  case 79: /* base_cmd: "reset" reset_cmd close_scope_reset  */
#line 1165 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8467 "src/parser_bison.c"
    break;

  case 80: /* base_cmd: "flush" flush_cmd  */
#line 1166 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8473 "src/parser_bison.c"
    break;

  case 81: /* base_cmd: "rename" rename_cmd  */
#line 1167 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8479 "src/parser_bison.c"
    break;

  case 82: /* base_cmd: "import" import_cmd close_scope_import  */
#line 1168 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8485 "src/parser_bison.c"
    break;

  case 83: /* base_cmd: "export" export_cmd close_scope_export  */
#line 1169 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8491 "src/parser_bison.c"
    break;

  case 84: /* base_cmd: "monitor" monitor_cmd close_scope_monitor  */
#line 1170 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8497 "src/parser_bison.c"
    break;

  case 85: /* base_cmd: "describe" describe_cmd  */
#line 1171 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8503 "src/parser_bison.c"
    break;

  case 86: /* base_cmd: "destroy" destroy_cmd close_scope_destroy  */
#line 1172 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8509 "src/parser_bison.c"
    break;

  case 87: /* add_cmd: "table" table_spec  */
#line 1176 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8517 "src/parser_bison.c"
    break;

  case 88: /* add_cmd: "table" table_spec table_block_alloc '{' table_block '}'  */
#line 1181 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-3].table)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_TABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].table));
			}
#line 8527 "src/parser_bison.c"
    break;

  case 89: /* add_cmd: "chain" chain_spec  */
#line 1187 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8535 "src/parser_bison.c"
    break;

  case 90: /* add_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1192 "src/parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8546 "src/parser_bison.c"
    break;

  case 91: /* add_cmd: "rule" rule_position rule  */
#line 1199 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8554 "src/parser_bison.c"
    break;

  case 92: /* add_cmd: rule_position rule  */
#line 1203 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8562 "src/parser_bison.c"
    break;

  case 93: /* add_cmd: "set" set_spec set_block_alloc '{' set_block '}'  */
#line 1208 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8572 "src/parser_bison.c"
    break;

  case 94: /* add_cmd: "map" set_spec map_block_alloc '{' map_block '}'  */
#line 1215 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8582 "src/parser_bison.c"
    break;

  case 95: /* add_cmd: "element" set_spec set_block_expr  */
#line 1221 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8590 "src/parser_bison.c"
    break;

  case 96: /* add_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1226 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8600 "src/parser_bison.c"
    break;

  case 97: /* add_cmd: "counter" obj_spec close_scope_counter  */
#line 1232 "src/parser_bison.y"
                        {
				struct obj *obj;

				obj = obj_alloc(&(yyloc));
				obj->type = NFT_OBJECT_COUNTER;
				handle_merge(&obj->handle, &(yyvsp[-1].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), obj);
			}
#line 8613 "src/parser_bison.c"
    break;

  case 98: /* add_cmd: "counter" obj_spec counter_obj counter_config close_scope_counter  */
#line 1241 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8621 "src/parser_bison.c"
    break;

  case 99: /* add_cmd: "counter" obj_spec counter_obj '{' counter_block '}' close_scope_counter  */
#line 1245 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8629 "src/parser_bison.c"
    break;

  case 100: /* add_cmd: "quota" obj_spec quota_obj quota_config close_scope_quota  */
#line 1249 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_QUOTA, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8637 "src/parser_bison.c"
    break;

  case 101: /* add_cmd: "quota" obj_spec quota_obj '{' quota_block '}' close_scope_quota  */
#line 1253 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_QUOTA, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8645 "src/parser_bison.c"
    break;

  case 102: /* add_cmd: "ct" "helper" obj_spec ct_obj_alloc '{' ct_helper_block '}' close_scope_ct  */
#line 1257 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_HELPER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8653 "src/parser_bison.c"
    break;

  case 103: /* add_cmd: "ct" "timeout" obj_spec ct_obj_alloc '{' ct_timeout_block '}' close_scope_ct  */
#line 1261 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_TIMEOUT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8661 "src/parser_bison.c"
    break;

  case 104: /* add_cmd: "ct" "expectation" obj_spec ct_obj_alloc '{' ct_expect_block '}' close_scope_ct  */
#line 1265 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_EXPECT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8669 "src/parser_bison.c"
    break;

  case 105: /* add_cmd: "limit" obj_spec limit_obj limit_config close_scope_limit  */
#line 1269 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_LIMIT, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8677 "src/parser_bison.c"
    break;

  case 106: /* add_cmd: "limit" obj_spec limit_obj '{' limit_block '}' close_scope_limit  */
#line 1273 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_LIMIT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8685 "src/parser_bison.c"
    break;

  case 107: /* add_cmd: "secmark" obj_spec secmark_obj secmark_config close_scope_secmark  */
#line 1277 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SECMARK, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8693 "src/parser_bison.c"
    break;

  case 108: /* add_cmd: "secmark" obj_spec secmark_obj '{' secmark_block '}' close_scope_secmark  */
#line 1281 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SECMARK, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8701 "src/parser_bison.c"
    break;

  case 109: /* add_cmd: "synproxy" obj_spec synproxy_obj synproxy_config close_scope_synproxy  */
#line 1285 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SYNPROXY, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8709 "src/parser_bison.c"
    break;

  case 110: /* add_cmd: "synproxy" obj_spec synproxy_obj '{' synproxy_block '}' close_scope_synproxy  */
#line 1289 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SYNPROXY, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8717 "src/parser_bison.c"
    break;

  case 111: /* replace_cmd: "rule" ruleid_spec rule  */
#line 1295 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_REPLACE, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8725 "src/parser_bison.c"
    break;

  case 112: /* create_cmd: "table" table_spec  */
#line 1301 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8733 "src/parser_bison.c"
    break;

  case 113: /* create_cmd: "table" table_spec table_block_alloc '{' table_block '}'  */
#line 1306 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-3].table)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_TABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].table));
			}
#line 8743 "src/parser_bison.c"
    break;

  case 114: /* create_cmd: "chain" chain_spec  */
#line 1312 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8751 "src/parser_bison.c"
    break;

  case 115: /* create_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1317 "src/parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8762 "src/parser_bison.c"
    break;

  case 116: /* create_cmd: "set" set_spec set_block_alloc '{' set_block '}'  */
#line 1325 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8772 "src/parser_bison.c"
    break;

  case 117: /* create_cmd: "map" set_spec map_block_alloc '{' map_block '}'  */
#line 1332 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8782 "src/parser_bison.c"
    break;

  case 118: /* create_cmd: "element" set_spec set_block_expr  */
#line 1338 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8790 "src/parser_bison.c"
    break;

  case 119: /* create_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1343 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8800 "src/parser_bison.c"
    break;

  case 120: /* create_cmd: "counter" obj_spec close_scope_counter  */
#line 1349 "src/parser_bison.y"
                        {
				struct obj *obj;

				obj = obj_alloc(&(yyloc));
				obj->type = NFT_OBJECT_COUNTER;
				handle_merge(&obj->handle, &(yyvsp[-1].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), obj);
			}
#line 8813 "src/parser_bison.c"
    break;

  case 121: /* create_cmd: "counter" obj_spec counter_obj counter_config close_scope_counter  */
#line 1358 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_COUNTER, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8821 "src/parser_bison.c"
    break;

  case 122: /* create_cmd: "quota" obj_spec quota_obj quota_config close_scope_quota  */
#line 1362 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_QUOTA, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8829 "src/parser_bison.c"
    break;

  case 123: /* create_cmd: "ct" "helper" obj_spec ct_obj_alloc '{' ct_helper_block '}' close_scope_ct  */
#line 1366 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_HELPER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8837 "src/parser_bison.c"
    break;

  case 124: /* create_cmd: "ct" "timeout" obj_spec ct_obj_alloc '{' ct_timeout_block '}' close_scope_ct  */
#line 1370 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_TIMEOUT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8845 "src/parser_bison.c"
    break;

  case 125: /* create_cmd: "ct" "expectation" obj_spec ct_obj_alloc '{' ct_expect_block '}' close_scope_ct  */
#line 1374 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_EXPECT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8853 "src/parser_bison.c"
    break;

  case 126: /* create_cmd: "limit" obj_spec limit_obj limit_config close_scope_limit  */
#line 1378 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_LIMIT, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8861 "src/parser_bison.c"
    break;

  case 127: /* create_cmd: "secmark" obj_spec secmark_obj secmark_config close_scope_secmark  */
#line 1382 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SECMARK, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8869 "src/parser_bison.c"
    break;

  case 128: /* create_cmd: "synproxy" obj_spec synproxy_obj synproxy_config close_scope_synproxy  */
#line 1386 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SYNPROXY, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8877 "src/parser_bison.c"
    break;

  case 129: /* insert_cmd: "rule" rule_position rule  */
#line 1392 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_INSERT, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8885 "src/parser_bison.c"
    break;

  case 138: /* delete_cmd: "table" table_or_id_spec  */
#line 1414 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8893 "src/parser_bison.c"
    break;

  case 139: /* delete_cmd: "chain" chain_or_id_spec  */
#line 1418 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8901 "src/parser_bison.c"
    break;

  case 140: /* delete_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1423 "src/parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8912 "src/parser_bison.c"
    break;

  case 141: /* delete_cmd: "rule" ruleid_spec  */
#line 1430 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8920 "src/parser_bison.c"
    break;

  case 142: /* delete_cmd: "set" set_or_id_spec  */
#line 1434 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8928 "src/parser_bison.c"
    break;

  case 143: /* delete_cmd: "map" set_spec  */
#line 1438 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8936 "src/parser_bison.c"
    break;

  case 144: /* delete_cmd: "element" set_spec set_block_expr  */
#line 1442 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8944 "src/parser_bison.c"
    break;

  case 145: /* delete_cmd: "flowtable" flowtable_spec  */
#line 1446 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8952 "src/parser_bison.c"
    break;

  case 146: /* delete_cmd: "flowtable" flowtableid_spec  */
#line 1450 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8960 "src/parser_bison.c"
    break;

  case 147: /* delete_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1455 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8970 "src/parser_bison.c"
    break;

  case 148: /* delete_cmd: "counter" obj_or_id_spec close_scope_counter  */
#line 1461 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8978 "src/parser_bison.c"
    break;

  case 149: /* delete_cmd: "quota" obj_or_id_spec close_scope_quota  */
#line 1465 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8986 "src/parser_bison.c"
    break;

  case 150: /* delete_cmd: "ct" ct_obj_type obj_spec ct_obj_alloc close_scope_ct  */
#line 1469 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_DELETE, (yyvsp[-3].val), &(yyvsp[-2].handle), &(yyloc), (yyvsp[-1].obj));
				if ((yyvsp[-3].val) == NFT_OBJECT_CT_TIMEOUT)
					init_list_head(&(yyvsp[-1].obj)->ct_timeout.timeout_list);
			}
#line 8996 "src/parser_bison.c"
    break;

  case 151: /* delete_cmd: "limit" obj_or_id_spec close_scope_limit  */
#line 1475 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9004 "src/parser_bison.c"
    break;

  case 152: /* delete_cmd: "secmark" obj_or_id_spec close_scope_secmark  */
#line 1479 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9012 "src/parser_bison.c"
    break;

  case 153: /* delete_cmd: "synproxy" obj_or_id_spec close_scope_synproxy  */
#line 1483 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9020 "src/parser_bison.c"
    break;

  case 154: /* destroy_cmd: "table" table_or_id_spec  */
#line 1489 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9028 "src/parser_bison.c"
    break;

  case 155: /* destroy_cmd: "chain" chain_or_id_spec  */
#line 1493 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9036 "src/parser_bison.c"
    break;

  case 156: /* destroy_cmd: "rule" ruleid_spec  */
#line 1497 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9044 "src/parser_bison.c"
    break;

  case 157: /* destroy_cmd: "set" set_or_id_spec  */
#line 1501 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9052 "src/parser_bison.c"
    break;

  case 158: /* destroy_cmd: "map" set_spec  */
#line 1505 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9060 "src/parser_bison.c"
    break;

  case 159: /* destroy_cmd: "element" set_spec set_block_expr  */
#line 1509 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9068 "src/parser_bison.c"
    break;

  case 160: /* destroy_cmd: "flowtable" flowtable_spec  */
#line 1513 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9076 "src/parser_bison.c"
    break;

  case 161: /* destroy_cmd: "flowtable" flowtableid_spec  */
#line 1517 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9084 "src/parser_bison.c"
    break;

  case 162: /* destroy_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1522 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 9094 "src/parser_bison.c"
    break;

  case 163: /* destroy_cmd: "counter" obj_or_id_spec close_scope_counter  */
#line 1528 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9102 "src/parser_bison.c"
    break;

  case 164: /* destroy_cmd: "quota" obj_or_id_spec close_scope_quota  */
#line 1532 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9110 "src/parser_bison.c"
    break;

  case 165: /* destroy_cmd: "ct" ct_obj_type obj_spec ct_obj_alloc close_scope_ct  */
#line 1536 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_DESTROY, (yyvsp[-3].val), &(yyvsp[-2].handle), &(yyloc), (yyvsp[-1].obj));
				if ((yyvsp[-3].val) == NFT_OBJECT_CT_TIMEOUT)
					init_list_head(&(yyvsp[-1].obj)->ct_timeout.timeout_list);
			}
#line 9120 "src/parser_bison.c"
    break;

  case 166: /* destroy_cmd: "limit" obj_or_id_spec close_scope_limit  */
#line 1542 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9128 "src/parser_bison.c"
    break;

  case 167: /* destroy_cmd: "secmark" obj_or_id_spec close_scope_secmark  */
#line 1546 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9136 "src/parser_bison.c"
    break;

  case 168: /* destroy_cmd: "synproxy" obj_or_id_spec close_scope_synproxy  */
#line 1550 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9144 "src/parser_bison.c"
    break;

  case 169: /* get_cmd: "element" set_spec set_block_expr  */
#line 1557 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_GET, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9152 "src/parser_bison.c"
    break;

  case 170: /* list_cmd: "table" table_spec  */
#line 1563 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9160 "src/parser_bison.c"
    break;

  case 171: /* list_cmd: "tables" ruleset_spec  */
#line 1567 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9168 "src/parser_bison.c"
    break;

  case 172: /* list_cmd: "chain" chain_spec  */
#line 1571 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9176 "src/parser_bison.c"
    break;

  case 173: /* list_cmd: "chains" ruleset_spec  */
#line 1575 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_CHAINS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9184 "src/parser_bison.c"
    break;

  case 174: /* list_cmd: "sets" ruleset_spec  */
#line 1579 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SETS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9192 "src/parser_bison.c"
    break;

  case 175: /* list_cmd: "sets" "table" table_spec  */
#line 1583 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SETS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9200 "src/parser_bison.c"
    break;

  case 176: /* list_cmd: "set" set_spec  */
#line 1587 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9208 "src/parser_bison.c"
    break;

  case 177: /* list_cmd: "counters" ruleset_spec  */
#line 1591 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9216 "src/parser_bison.c"
    break;

  case 178: /* list_cmd: "counters" "table" table_spec  */
#line 1595 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9224 "src/parser_bison.c"
    break;

  case 179: /* list_cmd: "counter" obj_spec close_scope_counter  */
#line 1599 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9232 "src/parser_bison.c"
    break;

  case 180: /* list_cmd: "quotas" ruleset_spec  */
#line 1603 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9240 "src/parser_bison.c"
    break;

  case 181: /* list_cmd: "quotas" "table" table_spec  */
#line 1607 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9248 "src/parser_bison.c"
    break;

  case 182: /* list_cmd: "quota" obj_spec close_scope_quota  */
#line 1611 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9256 "src/parser_bison.c"
    break;

  case 183: /* list_cmd: "limits" ruleset_spec  */
#line 1615 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMITS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9264 "src/parser_bison.c"
    break;

  case 184: /* list_cmd: "limits" "table" table_spec  */
#line 1619 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMITS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9272 "src/parser_bison.c"
    break;

  case 185: /* list_cmd: "limit" obj_spec close_scope_limit  */
#line 1623 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9280 "src/parser_bison.c"
    break;

  case 186: /* list_cmd: "secmarks" ruleset_spec  */
#line 1627 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9288 "src/parser_bison.c"
    break;

  case 187: /* list_cmd: "secmarks" "table" table_spec  */
#line 1631 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9296 "src/parser_bison.c"
    break;

  case 188: /* list_cmd: "secmark" obj_spec close_scope_secmark  */
#line 1635 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9304 "src/parser_bison.c"
    break;

  case 189: /* list_cmd: "synproxys" ruleset_spec  */
#line 1639 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXYS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9312 "src/parser_bison.c"
    break;

  case 190: /* list_cmd: "synproxys" "table" table_spec  */
#line 1643 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXYS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9320 "src/parser_bison.c"
    break;

  case 191: /* list_cmd: "synproxy" obj_spec close_scope_synproxy  */
#line 1647 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9328 "src/parser_bison.c"
    break;

  case 192: /* list_cmd: "ruleset" ruleset_spec  */
#line 1651 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_RULESET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9336 "src/parser_bison.c"
    break;

  case 193: /* list_cmd: "flow" "tables" ruleset_spec  */
#line 1655 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9344 "src/parser_bison.c"
    break;

  case 194: /* list_cmd: "flow" "table" set_spec  */
#line 1659 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9352 "src/parser_bison.c"
    break;

  case 195: /* list_cmd: "meters" ruleset_spec  */
#line 1663 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9360 "src/parser_bison.c"
    break;

  case 196: /* list_cmd: "meter" set_spec  */
#line 1667 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9368 "src/parser_bison.c"
    break;

  case 197: /* list_cmd: "flowtables" ruleset_spec  */
#line 1671 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_FLOWTABLES, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9376 "src/parser_bison.c"
    break;

  case 198: /* list_cmd: "flowtable" flowtable_spec  */
#line 1675 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9384 "src/parser_bison.c"
    break;

  case 199: /* list_cmd: "maps" ruleset_spec  */
#line 1679 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_MAPS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9392 "src/parser_bison.c"
    break;

  case 200: /* list_cmd: "map" set_spec  */
#line 1683 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9400 "src/parser_bison.c"
    break;

  case 201: /* list_cmd: "ct" ct_obj_type obj_spec close_scope_ct  */
#line 1687 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_LIST, (yyvsp[-2].val), &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9408 "src/parser_bison.c"
    break;

  case 202: /* list_cmd: "ct" ct_cmd_type "table" table_spec close_scope_ct  */
#line 1691 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, (yyvsp[-3].val), &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9416 "src/parser_bison.c"
    break;

  case 203: /* list_cmd: "hooks" basehook_spec  */
#line 1695 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_HOOKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9424 "src/parser_bison.c"
    break;

  case 204: /* basehook_device_name: "device" "string"  */
#line 1701 "src/parser_bison.y"
                        {
				(yyval.string) = (yyvsp[0].string);
			}
#line 9432 "src/parser_bison.c"
    break;

  case 205: /* basehook_spec: ruleset_spec  */
#line 1707 "src/parser_bison.y"
                        {
				(yyval.handle) = (yyvsp[0].handle);
			}
#line 9440 "src/parser_bison.c"
    break;

  case 206: /* basehook_spec: ruleset_spec basehook_device_name  */
#line 1711 "src/parser_bison.y"
                        {
				if ((yyvsp[0].string)) {
					(yyvsp[-1].handle).obj.name = (yyvsp[0].string);
					(yyvsp[-1].handle).obj.location = (yylsp[0]);
				}
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 9452 "src/parser_bison.c"
    break;

  case 207: /* reset_cmd: "counters" ruleset_spec  */
#line 1721 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9460 "src/parser_bison.c"
    break;

  case 208: /* reset_cmd: "counters" table_spec  */
#line 1725 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9468 "src/parser_bison.c"
    break;

  case 209: /* reset_cmd: "counters" "table" table_spec  */
#line 1729 "src/parser_bison.y"
                        {
				/* alias of previous rule. */
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9477 "src/parser_bison.c"
    break;

  case 210: /* reset_cmd: "counter" obj_spec close_scope_counter  */
#line 1734 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTER, &(yyvsp[-1].handle),&(yyloc), NULL);
			}
#line 9485 "src/parser_bison.c"
    break;

  case 211: /* reset_cmd: "quotas" ruleset_spec  */
#line 1738 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9493 "src/parser_bison.c"
    break;

  case 212: /* reset_cmd: "quotas" "table" table_spec  */
#line 1742 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9501 "src/parser_bison.c"
    break;

  case 213: /* reset_cmd: "quotas" table_spec  */
#line 1746 "src/parser_bison.y"
                        {
				/* alias of previous rule. */
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9510 "src/parser_bison.c"
    break;

  case 214: /* reset_cmd: "quota" obj_spec close_scope_quota  */
#line 1751 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9518 "src/parser_bison.c"
    break;

  case 215: /* reset_cmd: "rules" ruleset_spec  */
#line 1755 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_RULES, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9526 "src/parser_bison.c"
    break;

  case 216: /* reset_cmd: "rules" table_spec  */
#line 1759 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9534 "src/parser_bison.c"
    break;

  case 217: /* reset_cmd: "rules" "table" table_spec  */
#line 1763 "src/parser_bison.y"
                        {
				/* alias of previous rule. */
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9543 "src/parser_bison.c"
    break;

  case 218: /* reset_cmd: "rules" chain_spec  */
#line 1768 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9551 "src/parser_bison.c"
    break;

  case 219: /* reset_cmd: "rules" "chain" chain_spec  */
#line 1772 "src/parser_bison.y"
                        {
				/* alias of previous rule. */
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9560 "src/parser_bison.c"
    break;

  case 220: /* reset_cmd: "rule" ruleid_spec  */
#line 1777 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9568 "src/parser_bison.c"
    break;

  case 221: /* reset_cmd: "element" set_spec set_block_expr  */
#line 1781 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9576 "src/parser_bison.c"
    break;

  case 222: /* reset_cmd: "set" set_or_id_spec  */
#line 1785 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9584 "src/parser_bison.c"
    break;

  case 223: /* reset_cmd: "map" set_or_id_spec  */
#line 1789 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9592 "src/parser_bison.c"
    break;

  case 224: /* flush_cmd: "table" table_spec  */
#line 1795 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9600 "src/parser_bison.c"
    break;

  case 225: /* flush_cmd: "chain" chain_spec  */
#line 1799 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9608 "src/parser_bison.c"
    break;

  case 226: /* flush_cmd: "set" set_spec  */
#line 1803 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9616 "src/parser_bison.c"
    break;

  case 227: /* flush_cmd: "map" set_spec  */
#line 1807 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9624 "src/parser_bison.c"
    break;

  case 228: /* flush_cmd: "flow" "table" set_spec  */
#line 1811 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9632 "src/parser_bison.c"
    break;

  case 229: /* flush_cmd: "meter" set_spec  */
#line 1815 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9640 "src/parser_bison.c"
    break;

  case 230: /* flush_cmd: "ruleset" ruleset_spec  */
#line 1819 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_RULESET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9648 "src/parser_bison.c"
    break;

  case 231: /* rename_cmd: "chain" chain_spec identifier  */
#line 1825 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RENAME, CMD_OBJ_CHAIN, &(yyvsp[-1].handle), &(yyloc), NULL);
				(yyval.cmd)->arg = (yyvsp[0].string);
			}
#line 9657 "src/parser_bison.c"
    break;

  case 232: /* import_cmd: "ruleset" markup_format  */
#line 1832 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_IMPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9667 "src/parser_bison.c"
    break;

  case 233: /* import_cmd: markup_format  */
#line 1838 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_IMPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9677 "src/parser_bison.c"
    break;

  case 234: /* export_cmd: "ruleset" markup_format  */
#line 1846 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_EXPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9687 "src/parser_bison.c"
    break;

  case 235: /* export_cmd: markup_format  */
#line 1852 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_EXPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9697 "src/parser_bison.c"
    break;

  case 236: /* monitor_cmd: monitor_event monitor_object monitor_format  */
#line 1860 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct monitor *m = monitor_alloc((yyvsp[0].val), (yyvsp[-1].val), (yyvsp[-2].string));
				m->location = (yylsp[-2]);
				(yyval.cmd) = cmd_alloc(CMD_MONITOR, CMD_OBJ_MONITOR, &h, &(yyloc), m);
			}
#line 9708 "src/parser_bison.c"
    break;

  case 237: /* monitor_event: %empty  */
#line 1868 "src/parser_bison.y"
                                                { (yyval.string) = NULL; }
#line 9714 "src/parser_bison.c"
    break;

  case 238: /* monitor_event: "string"  */
#line 1869 "src/parser_bison.y"
                                                { (yyval.string) = (yyvsp[0].string); }
#line 9720 "src/parser_bison.c"
    break;

  case 239: /* monitor_object: %empty  */
#line 1872 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_ANY; }
#line 9726 "src/parser_bison.c"
    break;

  case 240: /* monitor_object: "tables"  */
#line 1873 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_TABLES; }
#line 9732 "src/parser_bison.c"
    break;

  case 241: /* monitor_object: "chains"  */
#line 1874 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_CHAINS; }
#line 9738 "src/parser_bison.c"
    break;

  case 242: /* monitor_object: "sets"  */
#line 1875 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_SETS; }
#line 9744 "src/parser_bison.c"
    break;

  case 243: /* monitor_object: "rules"  */
#line 1876 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_RULES; }
#line 9750 "src/parser_bison.c"
    break;

  case 244: /* monitor_object: "elements"  */
#line 1877 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_ELEMS; }
#line 9756 "src/parser_bison.c"
    break;

  case 245: /* monitor_object: "ruleset"  */
#line 1878 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_RULESET; }
#line 9762 "src/parser_bison.c"
    break;

  case 246: /* monitor_object: "trace"  */
#line 1879 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_TRACE; }
#line 9768 "src/parser_bison.c"
    break;

  case 247: /* monitor_format: %empty  */
#line 1882 "src/parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_DEFAULT; }
#line 9774 "src/parser_bison.c"
    break;

  case 249: /* markup_format: "xml"  */
#line 1886 "src/parser_bison.y"
                                                { (yyval.val) = __NFT_OUTPUT_NOTSUPP; }
#line 9780 "src/parser_bison.c"
    break;

  case 250: /* markup_format: "json"  */
#line 1887 "src/parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_JSON; }
#line 9786 "src/parser_bison.c"
    break;

  case 251: /* markup_format: "vm" "json"  */
#line 1888 "src/parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_JSON; }
#line 9792 "src/parser_bison.c"
    break;

  case 252: /* describe_cmd: primary_expr  */
#line 1892 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				(yyval.cmd) = cmd_alloc(CMD_DESCRIBE, CMD_OBJ_EXPR, &h, &(yyloc), NULL);
				(yyval.cmd)->expr = (yyvsp[0].expr);
			}
#line 9802 "src/parser_bison.c"
    break;

  case 253: /* table_block_alloc: %empty  */
#line 1900 "src/parser_bison.y"
                        {
				(yyval.table) = table_alloc();
				if (open_scope(state, &(yyval.table)->scope) < 0) {
					erec_queue(error(&(yyloc), "too many levels of nesting"),
						   state->msgs);
					state->nerrs++;
				}
			}
#line 9815 "src/parser_bison.c"
    break;

  case 254: /* table_options: "flags" table_flags  */
#line 1911 "src/parser_bison.y"
                        {
				(yyvsp[-2].table)->flags |= (yyvsp[0].val);
			}
#line 9823 "src/parser_bison.c"
    break;

  case 255: /* table_options: comment_spec  */
#line 1915 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].table)->comment, &(yyloc), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].table)->comment = (yyvsp[0].string);
			}
#line 9835 "src/parser_bison.c"
    break;

  case 257: /* table_flags: table_flags "comma" table_flag  */
#line 1926 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 9843 "src/parser_bison.c"
    break;

  case 258: /* table_flag: "string"  */
#line 1931 "src/parser_bison.y"
                        {
				(yyval.val) = parse_table_flag((yyvsp[0].string));
				free_const((yyvsp[0].string));
				if ((yyval.val) == 0) {
					erec_queue(error(&(yylsp[0]), "unknown table option %s", (yyvsp[0].string)),
						   state->msgs);
					YYERROR;
				}
			}
#line 9857 "src/parser_bison.c"
    break;

  case 259: /* table_block: %empty  */
#line 1942 "src/parser_bison.y"
                                                { (yyval.table) = (yyvsp[(-1) - (0)].table); }
#line 9863 "src/parser_bison.c"
    break;

  case 263: /* table_block: table_block "chain" chain_identifier chain_block_alloc '{' chain_block '}' stmt_separator  */
#line 1949 "src/parser_bison.y"
                        {
				(yyvsp[-4].chain)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].chain)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				close_scope(state);
				list_add_tail(&(yyvsp[-4].chain)->list, &(yyvsp[-7].table)->chains);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9876 "src/parser_bison.c"
    break;

  case 264: /* table_block: table_block "set" set_identifier set_block_alloc '{' set_block '}' stmt_separator  */
#line 1960 "src/parser_bison.y"
                        {
				(yyvsp[-4].set)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].set)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].set)->list, &(yyvsp[-7].table)->sets);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9888 "src/parser_bison.c"
    break;

  case 265: /* table_block: table_block "map" set_identifier map_block_alloc '{' map_block '}' stmt_separator  */
#line 1970 "src/parser_bison.y"
                        {
				(yyvsp[-4].set)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].set)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].set)->list, &(yyvsp[-7].table)->sets);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9900 "src/parser_bison.c"
    break;

  case 266: /* table_block: table_block "flowtable" flowtable_identifier flowtable_block_alloc '{' flowtable_block '}' stmt_separator  */
#line 1981 "src/parser_bison.y"
                        {
				(yyvsp[-4].flowtable)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].flowtable)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].flowtable)->list, &(yyvsp[-7].table)->flowtables);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9912 "src/parser_bison.c"
    break;

  case 267: /* table_block: table_block "counter" obj_identifier obj_block_alloc '{' counter_block '}' stmt_separator close_scope_counter  */
#line 1991 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_COUNTER;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9925 "src/parser_bison.c"
    break;

  case 268: /* table_block: table_block "quota" obj_identifier obj_block_alloc '{' quota_block '}' stmt_separator close_scope_quota  */
#line 2002 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_QUOTA;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9938 "src/parser_bison.c"
    break;

  case 269: /* table_block: table_block "ct" "helper" obj_identifier obj_block_alloc '{' ct_helper_block '}' stmt_separator close_scope_ct  */
#line 2011 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_HELPER;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9951 "src/parser_bison.c"
    break;

  case 270: /* table_block: table_block "ct" "timeout" obj_identifier obj_block_alloc '{' ct_timeout_block '}' stmt_separator close_scope_ct  */
#line 2020 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_TIMEOUT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9964 "src/parser_bison.c"
    break;

  case 271: /* table_block: table_block "ct" "expectation" obj_identifier obj_block_alloc '{' ct_expect_block '}' stmt_separator close_scope_ct  */
#line 2029 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_EXPECT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9977 "src/parser_bison.c"
    break;

  case 272: /* table_block: table_block "limit" obj_identifier obj_block_alloc '{' limit_block '}' stmt_separator close_scope_limit  */
#line 2040 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_LIMIT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9990 "src/parser_bison.c"
    break;

  case 273: /* table_block: table_block "secmark" obj_identifier obj_block_alloc '{' secmark_block '}' stmt_separator close_scope_secmark  */
#line 2051 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_SECMARK;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 10003 "src/parser_bison.c"
    break;

  case 274: /* table_block: table_block "synproxy" obj_identifier obj_block_alloc '{' synproxy_block '}' stmt_separator close_scope_synproxy  */
#line 2062 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_SYNPROXY;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 10016 "src/parser_bison.c"
    break;

  case 275: /* chain_block_alloc: %empty  */
#line 2073 "src/parser_bison.y"
                        {
				(yyval.chain) = chain_alloc();
				if (open_scope(state, &(yyval.chain)->scope) < 0) {
					erec_queue(error(&(yyloc), "too many levels of nesting"),
						   state->msgs);
					state->nerrs++;
				}
			}
#line 10029 "src/parser_bison.c"
    break;

  case 276: /* chain_block: %empty  */
#line 2083 "src/parser_bison.y"
                                                { (yyval.chain) = (yyvsp[(-1) - (0)].chain); }
#line 10035 "src/parser_bison.c"
    break;

  case 282: /* chain_block: chain_block rule stmt_separator  */
#line 2090 "src/parser_bison.y"
                        {
				list_add_tail(&(yyvsp[-1].rule)->list, &(yyvsp[-2].chain)->rules);
				(yyval.chain) = (yyvsp[-2].chain);
			}
#line 10044 "src/parser_bison.c"
    break;

  case 283: /* chain_block: chain_block "devices" '=' flowtable_expr stmt_separator  */
#line 2095 "src/parser_bison.y"
                        {
				if ((yyval.chain)->dev_expr) {
					list_splice_init(&(yyvsp[-1].expr)->expressions, &(yyval.chain)->dev_expr->expressions);
					expr_free((yyvsp[-1].expr));
					break;
				}
				(yyval.chain)->dev_expr = (yyvsp[-1].expr);
			}
#line 10057 "src/parser_bison.c"
    break;

  case 284: /* chain_block: chain_block comment_spec stmt_separator  */
#line 2104 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].chain)->comment, &(yylsp[-1]), state)) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].chain)->comment = (yyvsp[-1].string);
			}
#line 10069 "src/parser_bison.c"
    break;

  case 285: /* subchain_block: %empty  */
#line 2113 "src/parser_bison.y"
                                                { (yyval.chain) = (yyvsp[(-1) - (0)].chain); }
#line 10075 "src/parser_bison.c"
    break;

  case 287: /* subchain_block: subchain_block rule stmt_separator  */
#line 2116 "src/parser_bison.y"
                        {
				list_add_tail(&(yyvsp[-1].rule)->list, &(yyvsp[-2].chain)->rules);
				(yyval.chain) = (yyvsp[-2].chain);
			}
#line 10084 "src/parser_bison.c"
    break;

  case 288: /* typeof_verdict_expr: primary_expr  */
#line 2123 "src/parser_bison.y"
                        {
				struct expr *e = (yyvsp[0].expr);

				if (e->etype == EXPR_SYMBOL &&
				    strcmp("verdict", e->identifier) == 0) {
					struct expr *v = verdict_expr_alloc(&(yylsp[0]), NF_ACCEPT, NULL);

					expr_free(e);
					v->flags &= ~EXPR_F_CONSTANT;
					e = v;
				}

				if (expr_ops(e)->build_udata == NULL) {
					erec_queue(error(&(yylsp[0]), "map data type '%s' lacks typeof serialization", expr_ops(e)->name),
						   state->msgs);
					expr_free(e);
					YYERROR;
				}
				(yyval.expr) = e;
			}
#line 10109 "src/parser_bison.c"
    break;

  case 289: /* typeof_verdict_expr: typeof_expr "." primary_expr  */
#line 2144 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 10122 "src/parser_bison.c"
    break;

  case 290: /* typeof_data_expr: "interval" typeof_expr  */
#line 2155 "src/parser_bison.y"
                        {
				(yyvsp[0].expr)->flags |= EXPR_F_INTERVAL;
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10131 "src/parser_bison.c"
    break;

  case 291: /* typeof_data_expr: typeof_verdict_expr  */
#line 2160 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10139 "src/parser_bison.c"
    break;

  case 292: /* typeof_expr: primary_expr  */
#line 2166 "src/parser_bison.y"
                        {
				if (expr_ops((yyvsp[0].expr))->build_udata == NULL) {
					erec_queue(error(&(yylsp[0]), "primary expression type '%s' lacks typeof serialization", expr_ops((yyvsp[0].expr))->name),
						   state->msgs);
					expr_free((yyvsp[0].expr));
					YYERROR;
				}

				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10154 "src/parser_bison.c"
    break;

  case 293: /* typeof_expr: typeof_expr "." primary_expr  */
#line 2177 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 10167 "src/parser_bison.c"
    break;

  case 294: /* set_block_alloc: %empty  */
#line 2189 "src/parser_bison.y"
                        {
				(yyval.set) = set_alloc(&internal_location);
			}
#line 10175 "src/parser_bison.c"
    break;

  case 295: /* typeof_key_expr: "typeof" typeof_expr  */
#line 2194 "src/parser_bison.y"
                                                    { (yyval.expr) = (yyvsp[0].expr); }
#line 10181 "src/parser_bison.c"
    break;

  case 296: /* typeof_key_expr: "type" data_type_expr close_scope_type  */
#line 2195 "src/parser_bison.y"
                                                                        { (yyval.expr) = (yyvsp[-1].expr); }
#line 10187 "src/parser_bison.c"
    break;

  case 297: /* set_block: %empty  */
#line 2198 "src/parser_bison.y"
                                                { (yyval.set) = (yyvsp[(-1) - (0)].set); }
#line 10193 "src/parser_bison.c"
    break;

  case 300: /* set_block: set_block typeof_key_expr stmt_separator  */
#line 2202 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->key, &(yylsp[-1]), state)) {
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}

				(yyvsp[-2].set)->key = (yyvsp[-1].expr);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 10207 "src/parser_bison.c"
    break;

  case 301: /* set_block: set_block "flags" set_flag_list stmt_separator  */
#line 2212 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->flags = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10216 "src/parser_bison.c"
    break;

  case 302: /* set_block: set_block "timeout" time_spec stmt_separator  */
#line 2217 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->timeout = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10225 "src/parser_bison.c"
    break;

  case 303: /* set_block: set_block "gc-interval" time_spec stmt_separator  */
#line 2222 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->gc_int = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10234 "src/parser_bison.c"
    break;

  case 304: /* set_block: set_block stateful_stmt_list stmt_separator  */
#line 2227 "src/parser_bison.y"
                        {
				list_splice_tail((yyvsp[-1].list), &(yyvsp[-2].set)->stmt_list);
				(yyval.set) = (yyvsp[-2].set);
				free((yyvsp[-1].list));
			}
#line 10244 "src/parser_bison.c"
    break;

  case 305: /* set_block: set_block "elements" '=' set_block_expr  */
#line 2233 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-3].set)->init, &(yylsp[-2]), state)) {
					expr_free((yyvsp[0].expr));
					YYERROR;
				}
				(yyvsp[-3].set)->init = (yyvsp[0].expr);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10257 "src/parser_bison.c"
    break;

  case 306: /* set_block: set_block "auto-merge"  */
#line 2242 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->automerge = true;
				(yyval.set) = (yyvsp[-1].set);
			}
#line 10266 "src/parser_bison.c"
    break;

  case 308: /* set_block: set_block comment_spec stmt_separator  */
#line 2248 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->comment, &(yylsp[-1]), state)) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].set)->comment = (yyvsp[-1].string);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 10279 "src/parser_bison.c"
    break;

  case 311: /* set_flag_list: set_flag_list "comma" set_flag  */
#line 2263 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 10287 "src/parser_bison.c"
    break;

  case 313: /* set_flag: "constant"  */
#line 2269 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_CONSTANT; }
#line 10293 "src/parser_bison.c"
    break;

  case 314: /* set_flag: "interval"  */
#line 2270 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_INTERVAL; }
#line 10299 "src/parser_bison.c"
    break;

  case 315: /* set_flag: "timeout"  */
#line 2271 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_TIMEOUT; }
#line 10305 "src/parser_bison.c"
    break;

  case 316: /* set_flag: "dynamic"  */
#line 2272 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_EVAL; }
#line 10311 "src/parser_bison.c"
    break;

  case 317: /* map_block_alloc: %empty  */
#line 2276 "src/parser_bison.y"
                        {
				(yyval.set) = set_alloc(&internal_location);
			}
#line 10319 "src/parser_bison.c"
    break;

  case 318: /* ct_obj_type_map: "timeout"  */
#line 2281 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_TIMEOUT; }
#line 10325 "src/parser_bison.c"
    break;

  case 319: /* ct_obj_type_map: "expectation"  */
#line 2282 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_EXPECT; }
#line 10331 "src/parser_bison.c"
    break;

  case 320: /* map_block_obj_type: "counter" close_scope_counter  */
#line 2285 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_OBJECT_COUNTER; }
#line 10337 "src/parser_bison.c"
    break;

  case 321: /* map_block_obj_type: "quota" close_scope_quota  */
#line 2286 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_OBJECT_QUOTA; }
#line 10343 "src/parser_bison.c"
    break;

  case 322: /* map_block_obj_type: "limit" close_scope_limit  */
#line 2287 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_OBJECT_LIMIT; }
#line 10349 "src/parser_bison.c"
    break;

  case 323: /* map_block_obj_type: "secmark" close_scope_secmark  */
#line 2288 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_OBJECT_SECMARK; }
#line 10355 "src/parser_bison.c"
    break;

  case 324: /* map_block_obj_type: "synproxy" close_scope_synproxy  */
#line 2289 "src/parser_bison.y"
                                                              { (yyval.val) = NFT_OBJECT_SYNPROXY; }
#line 10361 "src/parser_bison.c"
    break;

  case 326: /* map_block_obj_typeof: "ct" ct_obj_type_map close_scope_ct  */
#line 2293 "src/parser_bison.y"
                                                                        { (yyval.val) = (yyvsp[-1].val); }
#line 10367 "src/parser_bison.c"
    break;

  case 327: /* map_block_data_interval: "interval"  */
#line 2296 "src/parser_bison.y"
                                         { (yyval.val) = EXPR_F_INTERVAL; }
#line 10373 "src/parser_bison.c"
    break;

  case 328: /* map_block_data_interval: %empty  */
#line 2297 "src/parser_bison.y"
                                { (yyval.val) = 0; }
#line 10379 "src/parser_bison.c"
    break;

  case 329: /* map_block: %empty  */
#line 2300 "src/parser_bison.y"
                                                { (yyval.set) = (yyvsp[(-1) - (0)].set); }
#line 10385 "src/parser_bison.c"
    break;

  case 332: /* map_block: map_block "timeout" time_spec stmt_separator  */
#line 2304 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->timeout = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10394 "src/parser_bison.c"
    break;

  case 333: /* map_block: map_block "gc-interval" time_spec stmt_separator  */
#line 2309 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->gc_int = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10403 "src/parser_bison.c"
    break;

  case 334: /* map_block: map_block "type" data_type_expr "colon" map_block_data_interval data_type_expr stmt_separator close_scope_type  */
#line 2316 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-7].set)->key, &(yylsp[-6]), state)) {
					expr_free((yyvsp[-5].expr));
					expr_free((yyvsp[-2].expr));
					YYERROR;
				}

				(yyvsp[-7].set)->key = (yyvsp[-5].expr);
				(yyvsp[-7].set)->data = (yyvsp[-2].expr);
				(yyvsp[-7].set)->data->flags |= (yyvsp[-3].val);

				(yyvsp[-7].set)->flags |= NFT_SET_MAP;
				(yyval.set) = (yyvsp[-7].set);
			}
#line 10422 "src/parser_bison.c"
    break;

  case 335: /* map_block: map_block "typeof" typeof_expr "colon" typeof_data_expr stmt_separator  */
#line 2333 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-5].set)->key, &(yylsp[-4]), state)) {
					expr_free((yyvsp[-3].expr));
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}

				(yyvsp[-5].set)->key = (yyvsp[-3].expr);

				if ((yyvsp[-1].expr)->etype == EXPR_CT && (yyvsp[-1].expr)->ct.key == NFT_CT_HELPER) {
					(yyvsp[-5].set)->objtype = NFT_OBJECT_CT_HELPER;
					(yyvsp[-5].set)->flags  |= NFT_SET_OBJECT;
					expr_free((yyvsp[-1].expr));
				} else {
					(yyvsp[-5].set)->data = (yyvsp[-1].expr);
					(yyvsp[-5].set)->flags |= NFT_SET_MAP;
				}

				(yyval.set) = (yyvsp[-5].set);
			}
#line 10447 "src/parser_bison.c"
    break;

  case 336: /* map_block: map_block "type" data_type_expr "colon" map_block_obj_type stmt_separator close_scope_type  */
#line 2356 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-6].set)->key, &(yylsp[-5]), state)) {
					expr_free((yyvsp[-4].expr));
					YYERROR;
				}

				(yyvsp[-6].set)->key = (yyvsp[-4].expr);
				(yyvsp[-6].set)->objtype = (yyvsp[-2].val);
				(yyvsp[-6].set)->flags  |= NFT_SET_OBJECT;
				(yyval.set) = (yyvsp[-6].set);
			}
#line 10463 "src/parser_bison.c"
    break;

  case 337: /* map_block: map_block "typeof" typeof_expr "colon" map_block_obj_typeof stmt_separator  */
#line 2370 "src/parser_bison.y"
                        {
				(yyvsp[-5].set)->key = (yyvsp[-3].expr);
				(yyvsp[-5].set)->objtype = (yyvsp[-1].val);
				(yyvsp[-5].set)->flags  |= NFT_SET_OBJECT;
				(yyval.set) = (yyvsp[-5].set);
			}
#line 10474 "src/parser_bison.c"
    break;

  case 338: /* map_block: map_block "flags" set_flag_list stmt_separator  */
#line 2377 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->flags |= (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10483 "src/parser_bison.c"
    break;

  case 339: /* map_block: map_block stateful_stmt_list stmt_separator  */
#line 2382 "src/parser_bison.y"
                        {
				list_splice_tail((yyvsp[-1].list), &(yyvsp[-2].set)->stmt_list);
				(yyval.set) = (yyvsp[-2].set);
				free((yyvsp[-1].list));
			}
#line 10493 "src/parser_bison.c"
    break;

  case 340: /* map_block: map_block "elements" '=' set_block_expr  */
#line 2388 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->init = (yyvsp[0].expr);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10502 "src/parser_bison.c"
    break;

  case 341: /* map_block: map_block comment_spec stmt_separator  */
#line 2393 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->comment, &(yylsp[-1]), state)) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].set)->comment = (yyvsp[-1].string);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 10515 "src/parser_bison.c"
    break;

  case 343: /* set_mechanism: "policy" set_policy_spec close_scope_policy  */
#line 2405 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->policy = (yyvsp[-1].val);
			}
#line 10523 "src/parser_bison.c"
    break;

  case 344: /* set_mechanism: "size" "number"  */
#line 2409 "src/parser_bison.y"
                        {
				(yyvsp[-2].set)->desc.size = (yyvsp[0].val);
			}
#line 10531 "src/parser_bison.c"
    break;

  case 345: /* set_policy_spec: "performance"  */
#line 2414 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_POL_PERFORMANCE; }
#line 10537 "src/parser_bison.c"
    break;

  case 346: /* set_policy_spec: "memory"  */
#line 2415 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_POL_MEMORY; }
#line 10543 "src/parser_bison.c"
    break;

  case 347: /* flowtable_block_alloc: %empty  */
#line 2419 "src/parser_bison.y"
                        {
				(yyval.flowtable) = flowtable_alloc(&internal_location);
			}
#line 10551 "src/parser_bison.c"
    break;

  case 348: /* flowtable_block: %empty  */
#line 2424 "src/parser_bison.y"
                                                { (yyval.flowtable) = (yyvsp[(-1) - (0)].flowtable); }
#line 10557 "src/parser_bison.c"
    break;

  case 351: /* flowtable_block: flowtable_block "hook" "string" prio_spec stmt_separator  */
#line 2428 "src/parser_bison.y"
                        {
				(yyval.flowtable)->hook.loc = (yylsp[-2]);
				(yyval.flowtable)->hook.name = chain_hookname_lookup((yyvsp[-2].string));
				if ((yyval.flowtable)->hook.name == NULL) {
					erec_queue(error(&(yylsp[-2]), "unknown chain hook"),
						   state->msgs);
					free_const((yyvsp[-2].string));
					YYERROR;
				}
				free_const((yyvsp[-2].string));

				(yyval.flowtable)->priority = (yyvsp[-1].prio_spec);
			}
#line 10575 "src/parser_bison.c"
    break;

  case 352: /* flowtable_block: flowtable_block "devices" '=' flowtable_expr stmt_separator  */
#line 2442 "src/parser_bison.y"
                        {
				(yyval.flowtable)->dev_expr = (yyvsp[-1].expr);
			}
#line 10583 "src/parser_bison.c"
    break;

  case 353: /* flowtable_block: flowtable_block "counter" close_scope_counter  */
#line 2446 "src/parser_bison.y"
                        {
				(yyval.flowtable)->flags |= NFT_FLOWTABLE_COUNTER;
			}
#line 10591 "src/parser_bison.c"
    break;

  case 354: /* flowtable_block: flowtable_block "flags" "offload" stmt_separator  */
#line 2450 "src/parser_bison.y"
                        {
				(yyval.flowtable)->flags |= FLOWTABLE_F_HW_OFFLOAD;
			}
#line 10599 "src/parser_bison.c"
    break;

  case 355: /* flowtable_expr: '{' flowtable_list_expr '}'  */
#line 2456 "src/parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 10608 "src/parser_bison.c"
    break;

  case 356: /* flowtable_expr: variable_expr  */
#line 2461 "src/parser_bison.y"
                        {
				(yyvsp[0].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10617 "src/parser_bison.c"
    break;

  case 357: /* flowtable_list_expr: flowtable_expr_member  */
#line 2468 "src/parser_bison.y"
                        {
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 10626 "src/parser_bison.c"
    break;

  case 358: /* flowtable_list_expr: flowtable_list_expr "comma" flowtable_expr_member  */
#line 2473 "src/parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 10635 "src/parser_bison.c"
    break;

  case 360: /* flowtable_expr_member: "quoted string"  */
#line 2481 "src/parser_bison.y"
                        {
				struct expr *expr = ifname_expr_alloc(&(yyloc), state->msgs, (yyvsp[0].string));

				if (!expr)
					YYERROR;

				(yyval.expr) = expr;
			}
#line 10648 "src/parser_bison.c"
    break;

  case 361: /* flowtable_expr_member: "string"  */
#line 2490 "src/parser_bison.y"
                        {
				struct expr *expr = ifname_expr_alloc(&(yyloc), state->msgs, (yyvsp[0].string));

				if (!expr)
					YYERROR;

				(yyval.expr) = expr;
			}
#line 10661 "src/parser_bison.c"
    break;

  case 362: /* flowtable_expr_member: variable_expr  */
#line 2499 "src/parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &ifname_type);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10670 "src/parser_bison.c"
    break;

  case 363: /* data_type_atom_expr: type_identifier  */
#line 2506 "src/parser_bison.y"
                        {
				const struct datatype *dtype = datatype_lookup_byname((yyvsp[0].string));
				if (dtype == NULL) {
					erec_queue(error(&(yylsp[0]), "unknown datatype %s", (yyvsp[0].string)),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyval.expr) = constant_expr_alloc(&(yylsp[0]), dtype, dtype->byteorder,
							 dtype->size, NULL);
				free_const((yyvsp[0].string));
			}
#line 10687 "src/parser_bison.c"
    break;

  case 364: /* data_type_atom_expr: "time"  */
#line 2519 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yylsp[0]), &time_type, time_type.byteorder,
							 time_type.size, NULL);
			}
#line 10696 "src/parser_bison.c"
    break;

  case 366: /* data_type_expr: data_type_expr "." data_type_atom_expr  */
#line 2527 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 10709 "src/parser_bison.c"
    break;

  case 367: /* obj_block_alloc: %empty  */
#line 2538 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&internal_location);
			}
#line 10717 "src/parser_bison.c"
    break;

  case 368: /* counter_block: %empty  */
#line 2543 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10723 "src/parser_bison.c"
    break;

  case 371: /* counter_block: counter_block counter_config  */
#line 2547 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10731 "src/parser_bison.c"
    break;

  case 372: /* counter_block: counter_block comment_spec  */
#line 2551 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10743 "src/parser_bison.c"
    break;

  case 373: /* quota_block: %empty  */
#line 2560 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10749 "src/parser_bison.c"
    break;

  case 376: /* quota_block: quota_block quota_config  */
#line 2564 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10757 "src/parser_bison.c"
    break;

  case 377: /* quota_block: quota_block comment_spec  */
#line 2568 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10769 "src/parser_bison.c"
    break;

  case 378: /* ct_helper_block: %empty  */
#line 2577 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10775 "src/parser_bison.c"
    break;

  case 381: /* ct_helper_block: ct_helper_block ct_helper_config  */
#line 2581 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10783 "src/parser_bison.c"
    break;

  case 382: /* ct_helper_block: ct_helper_block comment_spec  */
#line 2585 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10795 "src/parser_bison.c"
    break;

  case 383: /* ct_timeout_block: %empty  */
#line 2595 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[(-1) - (0)].obj);
				init_list_head(&(yyval.obj)->ct_timeout.timeout_list);
				(yyval.obj)->type = NFT_OBJECT_CT_TIMEOUT;
			}
#line 10805 "src/parser_bison.c"
    break;

  case 386: /* ct_timeout_block: ct_timeout_block ct_timeout_config  */
#line 2603 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10813 "src/parser_bison.c"
    break;

  case 387: /* ct_timeout_block: ct_timeout_block comment_spec  */
#line 2607 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10825 "src/parser_bison.c"
    break;

  case 388: /* ct_expect_block: %empty  */
#line 2616 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10831 "src/parser_bison.c"
    break;

  case 391: /* ct_expect_block: ct_expect_block ct_expect_config  */
#line 2620 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10839 "src/parser_bison.c"
    break;

  case 392: /* ct_expect_block: ct_expect_block comment_spec  */
#line 2624 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10851 "src/parser_bison.c"
    break;

  case 393: /* limit_block: %empty  */
#line 2633 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10857 "src/parser_bison.c"
    break;

  case 396: /* limit_block: limit_block limit_config  */
#line 2637 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10865 "src/parser_bison.c"
    break;

  case 397: /* limit_block: limit_block comment_spec  */
#line 2641 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10877 "src/parser_bison.c"
    break;

  case 398: /* secmark_block: %empty  */
#line 2650 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10883 "src/parser_bison.c"
    break;

  case 401: /* secmark_block: secmark_block secmark_config  */
#line 2654 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10891 "src/parser_bison.c"
    break;

  case 402: /* secmark_block: secmark_block comment_spec  */
#line 2658 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10903 "src/parser_bison.c"
    break;

  case 403: /* synproxy_block: %empty  */
#line 2667 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10909 "src/parser_bison.c"
    break;

  case 406: /* synproxy_block: synproxy_block synproxy_config  */
#line 2671 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10917 "src/parser_bison.c"
    break;

  case 407: /* synproxy_block: synproxy_block comment_spec  */
#line 2675 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10929 "src/parser_bison.c"
    break;

  case 408: /* type_identifier: "string"  */
#line 2684 "src/parser_bison.y"
                                        { (yyval.string) = (yyvsp[0].string); }
#line 10935 "src/parser_bison.c"
    break;

  case 409: /* type_identifier: "mark"  */
#line 2685 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("mark"); }
#line 10941 "src/parser_bison.c"
    break;

  case 410: /* type_identifier: "dscp"  */
#line 2686 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("dscp"); }
#line 10947 "src/parser_bison.c"
    break;

  case 411: /* type_identifier: "ecn"  */
#line 2687 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("ecn"); }
#line 10953 "src/parser_bison.c"
    break;

  case 412: /* type_identifier: "classid"  */
#line 2688 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("classid"); }
#line 10959 "src/parser_bison.c"
    break;

  case 413: /* hook_spec: "type" close_scope_type "string" "hook" "string" dev_spec prio_spec  */
#line 2692 "src/parser_bison.y"
                        {
				const char *chain_type = chain_type_name_lookup((yyvsp[-4].string));

				if (chain_type == NULL) {
					erec_queue(error(&(yylsp[-4]), "unknown chain type"),
						   state->msgs);
					free_const((yyvsp[-4].string));
					free_const((yyvsp[-2].string));
					expr_free((yyvsp[-1].expr));
					expr_free((yyvsp[0].prio_spec).expr);
					YYERROR;
				}
				(yyvsp[-7].chain)->type.loc = (yylsp[-4]);
				(yyvsp[-7].chain)->type.str = xstrdup(chain_type);
				free_const((yyvsp[-4].string));

				(yyvsp[-7].chain)->loc = (yyloc);
				(yyvsp[-7].chain)->hook.loc = (yylsp[-2]);
				(yyvsp[-7].chain)->hook.name = chain_hookname_lookup((yyvsp[-2].string));
				if ((yyvsp[-7].chain)->hook.name == NULL) {
					erec_queue(error(&(yylsp[-2]), "unknown chain hook"),
						   state->msgs);
					free_const((yyvsp[-2].string));
					expr_free((yyvsp[-1].expr));
					expr_free((yyvsp[0].prio_spec).expr);
					YYERROR;
				}
				free_const((yyvsp[-2].string));

				(yyvsp[-7].chain)->dev_expr	= (yyvsp[-1].expr);
				(yyvsp[-7].chain)->priority	= (yyvsp[0].prio_spec);
				(yyvsp[-7].chain)->flags	|= CHAIN_F_BASECHAIN;
			}
#line 10997 "src/parser_bison.c"
    break;

  case 414: /* prio_spec: "priority" extended_prio_spec  */
#line 2728 "src/parser_bison.y"
                        {
				(yyval.prio_spec) = (yyvsp[0].prio_spec);
				(yyval.prio_spec).loc = (yyloc);
			}
#line 11006 "src/parser_bison.c"
    break;

  case 415: /* extended_prio_name: "out"  */
#line 2735 "src/parser_bison.y"
                        {
				(yyval.string) = strdup("out");
			}
#line 11014 "src/parser_bison.c"
    break;

  case 417: /* extended_prio_spec: int_num  */
#line 2742 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = constant_expr_alloc(&(yyloc), &integer_type,
								BYTEORDER_HOST_ENDIAN,
								sizeof(int) *
								BITS_PER_BYTE, &(yyvsp[0].val32));
				(yyval.prio_spec) = spec;
			}
#line 11028 "src/parser_bison.c"
    break;

  case 418: /* extended_prio_spec: variable_expr  */
#line 2752 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = (yyvsp[0].expr);
				(yyval.prio_spec) = spec;
			}
#line 11039 "src/parser_bison.c"
    break;

  case 419: /* extended_prio_spec: extended_prio_name  */
#line 2759 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen((yyvsp[0].string)) * BITS_PER_BYTE,
								(yyvsp[0].string));
				free_const((yyvsp[0].string));
				(yyval.prio_spec) = spec;
			}
#line 11054 "src/parser_bison.c"
    break;

  case 420: /* extended_prio_spec: extended_prio_name "+" "number"  */
#line 2770 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				char str[NFT_NAME_MAXLEN];
				snprintf(str, sizeof(str), "%s + %" PRIu64, (yyvsp[-2].string), (yyvsp[0].val));
				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen(str) * BITS_PER_BYTE,
								str);
				free_const((yyvsp[-2].string));
				(yyval.prio_spec) = spec;
			}
#line 11071 "src/parser_bison.c"
    break;

  case 421: /* extended_prio_spec: extended_prio_name "-" "number"  */
#line 2783 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};
				char str[NFT_NAME_MAXLEN];

				snprintf(str, sizeof(str), "%s - %" PRIu64, (yyvsp[-2].string), (yyvsp[0].val));
				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen(str) * BITS_PER_BYTE,
								str);
				free_const((yyvsp[-2].string));
				(yyval.prio_spec) = spec;
			}
#line 11088 "src/parser_bison.c"
    break;

  case 422: /* int_num: "number"  */
#line 2797 "src/parser_bison.y"
                                                        { (yyval.val32) = (yyvsp[0].val); }
#line 11094 "src/parser_bison.c"
    break;

  case 423: /* int_num: "-" "number"  */
#line 2798 "src/parser_bison.y"
                                                        { (yyval.val32) = -(yyvsp[0].val); }
#line 11100 "src/parser_bison.c"
    break;

  case 424: /* dev_spec: "device" string  */
#line 2802 "src/parser_bison.y"
                        {
				struct expr *expr = ifname_expr_alloc(&(yyloc), state->msgs, (yyvsp[0].string));

				if (!expr)
					YYERROR;

				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), expr);

			}
#line 11115 "src/parser_bison.c"
    break;

  case 425: /* dev_spec: "device" variable_expr  */
#line 2813 "src/parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &ifname_type);
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 11125 "src/parser_bison.c"
    break;

  case 426: /* dev_spec: "devices" '=' flowtable_expr  */
#line 2819 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 11133 "src/parser_bison.c"
    break;

  case 427: /* dev_spec: %empty  */
#line 2822 "src/parser_bison.y"
                                                        { (yyval.expr) = NULL; }
#line 11139 "src/parser_bison.c"
    break;

  case 428: /* flags_spec: "flags" "offload"  */
#line 2826 "src/parser_bison.y"
                        {
				(yyvsp[-2].chain)->flags |= CHAIN_F_HW_OFFLOAD;
			}
#line 11147 "src/parser_bison.c"
    break;

  case 429: /* policy_spec: "policy" policy_expr close_scope_policy  */
#line 2832 "src/parser_bison.y"
                        {
				if ((yyvsp[-3].chain)->policy) {
					erec_queue(error(&(yyloc), "you cannot set chain policy twice"),
						   state->msgs);
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}
				(yyvsp[-3].chain)->policy		= (yyvsp[-1].expr);
				(yyvsp[-3].chain)->policy->location	= (yyloc);
			}
#line 11162 "src/parser_bison.c"
    break;

  case 430: /* policy_expr: variable_expr  */
#line 2845 "src/parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &policy_type);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 11171 "src/parser_bison.c"
    break;

  case 431: /* policy_expr: chain_policy  */
#line 2850 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &integer_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(int) *
							 BITS_PER_BYTE, &(yyvsp[0].val32));
			}
#line 11182 "src/parser_bison.c"
    break;

  case 432: /* chain_policy: "accept"  */
#line 2858 "src/parser_bison.y"
                                                { (yyval.val32) = NF_ACCEPT; }
#line 11188 "src/parser_bison.c"
    break;

  case 433: /* chain_policy: "drop"  */
#line 2859 "src/parser_bison.y"
                                                { (yyval.val32) = NF_DROP;   }
#line 11194 "src/parser_bison.c"
    break;

  case 435: /* identifier: "last"  */
#line 2863 "src/parser_bison.y"
                                                { (yyval.string) = xstrdup("last"); }
#line 11200 "src/parser_bison.c"
    break;

  case 439: /* time_spec: "string"  */
#line 2872 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t res;

				erec = time_parse(&(yylsp[0]), (yyvsp[0].string), &res);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = res;
			}
#line 11217 "src/parser_bison.c"
    break;

  case 441: /* time_spec_or_num_s: time_spec  */
#line 2888 "src/parser_bison.y"
                                          { (yyval.val) = (yyvsp[0].val) / 1000u; }
#line 11223 "src/parser_bison.c"
    break;

  case 442: /* family_spec: %empty  */
#line 2891 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV4; }
#line 11229 "src/parser_bison.c"
    break;

  case 444: /* family_spec_explicit: "ip" close_scope_ip  */
#line 2895 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV4; }
#line 11235 "src/parser_bison.c"
    break;

  case 445: /* family_spec_explicit: "ip6" close_scope_ip6  */
#line 2896 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV6; }
#line 11241 "src/parser_bison.c"
    break;

  case 446: /* family_spec_explicit: "inet"  */
#line 2897 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_INET; }
#line 11247 "src/parser_bison.c"
    break;

  case 447: /* family_spec_explicit: "arp" close_scope_arp  */
#line 2898 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_ARP; }
#line 11253 "src/parser_bison.c"
    break;

  case 448: /* family_spec_explicit: "bridge"  */
#line 2899 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_BRIDGE; }
#line 11259 "src/parser_bison.c"
    break;

  case 449: /* family_spec_explicit: "netdev"  */
#line 2900 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_NETDEV; }
#line 11265 "src/parser_bison.c"
    break;

  case 450: /* table_spec: family_spec identifier  */
#line 2904 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= (yyvsp[-1].val);
				(yyval.handle).table.location = (yylsp[0]);
				(yyval.handle).table.name	= (yyvsp[0].string);
			}
#line 11276 "src/parser_bison.c"
    break;

  case 451: /* tableid_spec: family_spec "handle" "number"  */
#line 2913 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family 		= (yyvsp[-2].val);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
				(yyval.handle).handle.location	= (yylsp[0]);
			}
#line 11287 "src/parser_bison.c"
    break;

  case 452: /* chain_spec: table_spec identifier  */
#line 2922 "src/parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).chain.name	= (yyvsp[0].string);
				(yyval.handle).chain.location = (yylsp[0]);
			}
#line 11297 "src/parser_bison.c"
    break;

  case 453: /* chainid_spec: table_spec "handle" "number"  */
#line 2930 "src/parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location 	= (yylsp[0]);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
			}
#line 11307 "src/parser_bison.c"
    break;

  case 454: /* chain_identifier: identifier  */
#line 2938 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).chain.name		= (yyvsp[0].string);
				(yyval.handle).chain.location	= (yylsp[0]);
			}
#line 11317 "src/parser_bison.c"
    break;

  case 455: /* set_spec: table_spec identifier  */
#line 2946 "src/parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).set.name	= (yyvsp[0].string);
				(yyval.handle).set.location	= (yylsp[0]);
			}
#line 11327 "src/parser_bison.c"
    break;

  case 456: /* setid_spec: table_spec "handle" "number"  */
#line 2954 "src/parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location 	= (yylsp[0]);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
			}
#line 11337 "src/parser_bison.c"
    break;

  case 457: /* set_identifier: identifier  */
#line 2962 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).set.name	= (yyvsp[0].string);
				(yyval.handle).set.location	= (yylsp[0]);
			}
#line 11347 "src/parser_bison.c"
    break;

  case 458: /* flowtable_spec: table_spec identifier  */
#line 2970 "src/parser_bison.y"
                        {
				(yyval.handle)			= (yyvsp[-1].handle);
				(yyval.handle).flowtable.name	= (yyvsp[0].string);
				(yyval.handle).flowtable.location	= (yylsp[0]);
			}
#line 11357 "src/parser_bison.c"
    break;

  case 459: /* flowtableid_spec: table_spec "handle" "number"  */
#line 2978 "src/parser_bison.y"
                        {
				(yyval.handle)			= (yyvsp[-2].handle);
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 11367 "src/parser_bison.c"
    break;

  case 460: /* flowtable_identifier: identifier  */
#line 2986 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).flowtable.name	= (yyvsp[0].string);
				(yyval.handle).flowtable.location	= (yylsp[0]);
			}
#line 11377 "src/parser_bison.c"
    break;

  case 461: /* obj_spec: table_spec identifier  */
#line 2994 "src/parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).obj.name	= (yyvsp[0].string);
				(yyval.handle).obj.location	= (yylsp[0]);
			}
#line 11387 "src/parser_bison.c"
    break;

  case 462: /* objid_spec: table_spec "handle" "number"  */
#line 3002 "src/parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 11397 "src/parser_bison.c"
    break;

  case 463: /* obj_identifier: identifier  */
#line 3010 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).obj.name		= (yyvsp[0].string);
				(yyval.handle).obj.location		= (yylsp[0]);
			}
#line 11407 "src/parser_bison.c"
    break;

  case 464: /* handle_spec: "handle" "number"  */
#line 3018 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 11417 "src/parser_bison.c"
    break;

  case 465: /* position_spec: "position" "number"  */
#line 3026 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).position.location	= (yyloc);
				(yyval.handle).position.id		= (yyvsp[0].val);
			}
#line 11427 "src/parser_bison.c"
    break;

  case 466: /* index_spec: "index" "number"  */
#line 3034 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).index.location	= (yyloc);
				(yyval.handle).index.id		= (yyvsp[0].val) + 1;
			}
#line 11437 "src/parser_bison.c"
    break;

  case 467: /* rule_position: chain_spec  */
#line 3042 "src/parser_bison.y"
                        {
				(yyval.handle) = (yyvsp[0].handle);
			}
#line 11445 "src/parser_bison.c"
    break;

  case 468: /* rule_position: chain_spec position_spec  */
#line 3046 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11454 "src/parser_bison.c"
    break;

  case 469: /* rule_position: chain_spec handle_spec  */
#line 3051 "src/parser_bison.y"
                        {
				(yyvsp[0].handle).position.location = (yyvsp[0].handle).handle.location;
				(yyvsp[0].handle).position.id = (yyvsp[0].handle).handle.id;
				(yyvsp[0].handle).handle.id = 0;
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11466 "src/parser_bison.c"
    break;

  case 470: /* rule_position: chain_spec index_spec  */
#line 3059 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11475 "src/parser_bison.c"
    break;

  case 471: /* ruleid_spec: chain_spec handle_spec  */
#line 3066 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11484 "src/parser_bison.c"
    break;

  case 472: /* comment_spec: "comment" string  */
#line 3073 "src/parser_bison.y"
                        {
				if (strlen((yyvsp[0].string)) > NFTNL_UDATA_COMMENT_MAXLEN) {
					erec_queue(error(&(yylsp[0]), "comment too long, %d characters maximum allowed",
							 NFTNL_UDATA_COMMENT_MAXLEN),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyval.string) = (yyvsp[0].string);
			}
#line 11499 "src/parser_bison.c"
    break;

  case 473: /* ruleset_spec: %empty  */
#line 3086 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= NFPROTO_UNSPEC;
			}
#line 11508 "src/parser_bison.c"
    break;

  case 474: /* ruleset_spec: family_spec_explicit  */
#line 3091 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= (yyvsp[0].val);
			}
#line 11517 "src/parser_bison.c"
    break;

  case 475: /* rule: rule_alloc  */
#line 3098 "src/parser_bison.y"
                        {
				(yyval.rule)->comment = NULL;
			}
#line 11525 "src/parser_bison.c"
    break;

  case 476: /* rule: rule_alloc comment_spec  */
#line 3102 "src/parser_bison.y"
                        {
				(yyval.rule)->comment = (yyvsp[0].string);
			}
#line 11533 "src/parser_bison.c"
    break;

  case 477: /* rule_alloc: stmt_list  */
#line 3108 "src/parser_bison.y"
                        {
				struct stmt *i;

				(yyval.rule) = rule_alloc(&(yyloc), NULL);
				list_for_each_entry(i, (yyvsp[0].list), list)
					(yyval.rule)->num_stmts++;
				list_splice_tail((yyvsp[0].list), &(yyval.rule)->stmts);
				free((yyvsp[0].list));
			}
#line 11547 "src/parser_bison.c"
    break;

  case 478: /* stmt_list: stmt  */
#line 3120 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 11557 "src/parser_bison.c"
    break;

  case 479: /* stmt_list: stmt_list stmt  */
#line 3126 "src/parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 11566 "src/parser_bison.c"
    break;

  case 480: /* stateful_stmt_list: stateful_stmt  */
#line 3133 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 11576 "src/parser_bison.c"
    break;

  case 481: /* stateful_stmt_list: stateful_stmt_list stateful_stmt  */
#line 3139 "src/parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 11585 "src/parser_bison.c"
    break;

  case 482: /* objref_stmt_counter: "counter" "name" stmt_expr close_scope_counter  */
#line 3146 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_COUNTER;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11595 "src/parser_bison.c"
    break;

  case 483: /* objref_stmt_limit: "limit" "name" stmt_expr close_scope_limit  */
#line 3154 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_LIMIT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11605 "src/parser_bison.c"
    break;

  case 484: /* objref_stmt_quota: "quota" "name" stmt_expr close_scope_quota  */
#line 3162 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_QUOTA;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11615 "src/parser_bison.c"
    break;

  case 485: /* objref_stmt_synproxy: "synproxy" "name" stmt_expr close_scope_synproxy  */
#line 3170 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_SYNPROXY;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11625 "src/parser_bison.c"
    break;

  case 486: /* objref_stmt_ct: "ct" "timeout" "set" stmt_expr close_scope_ct  */
#line 3178 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_CT_TIMEOUT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);

			}
#line 11636 "src/parser_bison.c"
    break;

  case 487: /* objref_stmt_ct: "ct" "expectation" "set" stmt_expr close_scope_ct  */
#line 3185 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_CT_EXPECT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11646 "src/parser_bison.c"
    break;

  case 521: /* xt_stmt: "xt" "string" string  */
#line 3232 "src/parser_bison.y"
                        {
				(yyval.stmt) = NULL;
				free_const((yyvsp[-1].string));
				free_const((yyvsp[0].string));
				erec_queue(error(&(yyloc), "unsupported xtables compat expression, use iptables-nft with this ruleset"),
					   state->msgs);
				YYERROR;
			}
#line 11659 "src/parser_bison.c"
    break;

  case 522: /* chain_stmt_type: "jump"  */
#line 3242 "src/parser_bison.y"
                                        { (yyval.val) = NFT_JUMP; }
#line 11665 "src/parser_bison.c"
    break;

  case 523: /* chain_stmt_type: "goto"  */
#line 3243 "src/parser_bison.y"
                                        { (yyval.val) = NFT_GOTO; }
#line 11671 "src/parser_bison.c"
    break;

  case 524: /* chain_stmt: chain_stmt_type chain_block_alloc '{' subchain_block '}'  */
#line 3247 "src/parser_bison.y"
                        {
				(yyvsp[-3].chain)->location = (yylsp[-3]);
				close_scope(state);
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				(yyval.stmt) = chain_stmt_alloc(&(yyloc), (yyvsp[-1].chain), (yyvsp[-4].val));
			}
#line 11682 "src/parser_bison.c"
    break;

  case 525: /* verdict_stmt: verdict_expr  */
#line 3256 "src/parser_bison.y"
                        {
				(yyval.stmt) = verdict_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 11690 "src/parser_bison.c"
    break;

  case 526: /* verdict_stmt: verdict_map_stmt  */
#line 3260 "src/parser_bison.y"
                        {
				(yyval.stmt) = verdict_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 11698 "src/parser_bison.c"
    break;

  case 527: /* verdict_map_stmt: concat_expr "vmap" verdict_map_expr  */
#line 3266 "src/parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11706 "src/parser_bison.c"
    break;

  case 528: /* verdict_map_expr: '{' verdict_map_list_expr '}'  */
#line 3272 "src/parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 11715 "src/parser_bison.c"
    break;

  case 530: /* verdict_map_list_expr: verdict_map_list_member_expr  */
#line 3280 "src/parser_bison.y"
                        {
				(yyval.expr) = set_expr_alloc(&(yyloc), NULL);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 11724 "src/parser_bison.c"
    break;

  case 531: /* verdict_map_list_expr: verdict_map_list_expr "comma" verdict_map_list_member_expr  */
#line 3285 "src/parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 11733 "src/parser_bison.c"
    break;

  case 533: /* verdict_map_list_member_expr: opt_newline set_elem_expr "colon" verdict_expr opt_newline  */
#line 3293 "src/parser_bison.y"
                        {
				(yyval.expr) = mapping_expr_alloc(&(yylsp[-3]), (yyvsp[-3].expr), (yyvsp[-1].expr));
			}
#line 11741 "src/parser_bison.c"
    break;

  case 534: /* connlimit_stmt: "ct" "count" "number" close_scope_ct  */
#line 3299 "src/parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count	= (yyvsp[-1].val);
			}
#line 11750 "src/parser_bison.c"
    break;

  case 535: /* connlimit_stmt: "ct" "count" "over" "number" close_scope_ct  */
#line 3304 "src/parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count = (yyvsp[-1].val);
				(yyval.stmt)->connlimit.flags = NFT_CONNLIMIT_F_INV;
			}
#line 11760 "src/parser_bison.c"
    break;

  case 538: /* counter_stmt_alloc: "counter"  */
#line 3315 "src/parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
			}
#line 11768 "src/parser_bison.c"
    break;

  case 539: /* counter_args: counter_arg  */
#line 3321 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 11776 "src/parser_bison.c"
    break;

  case 541: /* counter_arg: "packets" "number"  */
#line 3328 "src/parser_bison.y"
                        {
				assert((yyvsp[-2].stmt)->ops->type == STMT_COUNTER);
				(yyvsp[-2].stmt)->counter.packets = (yyvsp[0].val);
			}
#line 11785 "src/parser_bison.c"
    break;

  case 542: /* counter_arg: "bytes" "number"  */
#line 3333 "src/parser_bison.y"
                        {
				assert((yyvsp[-2].stmt)->ops->type == STMT_COUNTER);
				(yyvsp[-2].stmt)->counter.bytes	 = (yyvsp[0].val);
			}
#line 11794 "src/parser_bison.c"
    break;

  case 543: /* last_stmt: "last"  */
#line 3340 "src/parser_bison.y"
                        {
				(yyval.stmt) = last_stmt_alloc(&(yyloc));
			}
#line 11802 "src/parser_bison.c"
    break;

  case 544: /* last_stmt: "last" "used" "never"  */
#line 3344 "src/parser_bison.y"
                        {
				(yyval.stmt) = last_stmt_alloc(&(yyloc));
			}
#line 11810 "src/parser_bison.c"
    break;

  case 545: /* last_stmt: "last" "used" time_spec  */
#line 3348 "src/parser_bison.y"
                        {
				(yyval.stmt) = last_stmt_alloc(&(yyloc));
				(yyval.stmt)->last.used = (yyvsp[0].val);
				(yyval.stmt)->last.set = true;
			}
#line 11820 "src/parser_bison.c"
    break;

  case 548: /* log_stmt_alloc: "log"  */
#line 3360 "src/parser_bison.y"
                        {
				(yyval.stmt) = log_stmt_alloc(&(yyloc));
			}
#line 11828 "src/parser_bison.c"
    break;

  case 549: /* log_args: log_arg  */
#line 3366 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 11836 "src/parser_bison.c"
    break;

  case 551: /* log_arg: "prefix" string  */
#line 3373 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);
				struct error_record *erec;
				const char *prefix;

				prefix = str_preprocess(state, &(yylsp[0]), scope, (yyvsp[0].string), &erec);
				if (!prefix) {
					erec_queue(erec, state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}

				free_const((yyvsp[0].string));
				(yyvsp[-2].stmt)->log.prefix = prefix;
				(yyvsp[-2].stmt)->log.flags |= STMT_LOG_PREFIX;
			}
#line 11857 "src/parser_bison.c"
    break;

  case 552: /* log_arg: "group" "number"  */
#line 3390 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.group	 = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_GROUP;
			}
#line 11866 "src/parser_bison.c"
    break;

  case 553: /* log_arg: "snaplen" "number"  */
#line 3395 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.snaplen	 = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_SNAPLEN;
			}
#line 11875 "src/parser_bison.c"
    break;

  case 554: /* log_arg: "queue-threshold" "number"  */
#line 3400 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.qthreshold = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_QTHRESHOLD;
			}
#line 11884 "src/parser_bison.c"
    break;

  case 555: /* log_arg: "level" level_type  */
#line 3405 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.level	= (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_LEVEL;
			}
#line 11893 "src/parser_bison.c"
    break;

  case 556: /* log_arg: "flags" log_flags  */
#line 3410 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.logflags	|= (yyvsp[0].val);
			}
#line 11901 "src/parser_bison.c"
    break;

  case 557: /* level_type: string  */
#line 3416 "src/parser_bison.y"
                        {
				if (!strcmp("emerg", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_EMERG;
				else if (!strcmp("alert", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_ALERT;
				else if (!strcmp("crit", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_CRIT;
				else if (!strcmp("err", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_ERR;
				else if (!strcmp("warn", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_WARNING;
				else if (!strcmp("notice", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_NOTICE;
				else if (!strcmp("info", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_INFO;
				else if (!strcmp("debug", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_DEBUG;
				else if (!strcmp("audit", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_AUDIT;
				else {
					erec_queue(error(&(yylsp[0]), "invalid log level"),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 11933 "src/parser_bison.c"
    break;

  case 558: /* log_flags: "tcp" log_flags_tcp close_scope_tcp  */
#line 3446 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-1].val);
			}
#line 11941 "src/parser_bison.c"
    break;

  case 559: /* log_flags: "ip" "options" close_scope_ip  */
#line 3450 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_IPOPT;
			}
#line 11949 "src/parser_bison.c"
    break;

  case 560: /* log_flags: "skuid"  */
#line 3454 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_UID;
			}
#line 11957 "src/parser_bison.c"
    break;

  case 561: /* log_flags: "ether" close_scope_eth  */
#line 3458 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_MACDECODE;
			}
#line 11965 "src/parser_bison.c"
    break;

  case 562: /* log_flags: "all"  */
#line 3462 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_MASK;
			}
#line 11973 "src/parser_bison.c"
    break;

  case 563: /* log_flags_tcp: log_flags_tcp "comma" log_flag_tcp  */
#line 3468 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 11981 "src/parser_bison.c"
    break;

  case 565: /* log_flag_tcp: "seq"  */
#line 3475 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_TCPSEQ;
			}
#line 11989 "src/parser_bison.c"
    break;

  case 566: /* log_flag_tcp: "options"  */
#line 3479 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_TCPOPT;
			}
#line 11997 "src/parser_bison.c"
    break;

  case 567: /* limit_stmt: "limit" "rate" limit_mode limit_rate_pkts limit_burst_pkts close_scope_limit  */
#line 3485 "src/parser_bison.y"
                        {
				if ((yyvsp[-1].val) == 0) {
					erec_queue(error(&(yylsp[-1]), "packet limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate	= (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit	= (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst	= (yyvsp[-1].val);
				(yyval.stmt)->limit.type	= NFT_LIMIT_PKTS;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 12015 "src/parser_bison.c"
    break;

  case 568: /* limit_stmt: "limit" "rate" limit_mode limit_rate_bytes limit_burst_bytes close_scope_limit  */
#line 3499 "src/parser_bison.y"
                        {
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate	= (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit	= (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst	= (yyvsp[-1].val);
				(yyval.stmt)->limit.type	= NFT_LIMIT_PKT_BYTES;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 12028 "src/parser_bison.c"
    break;

  case 569: /* quota_mode: "over"  */
#line 3509 "src/parser_bison.y"
                                                { (yyval.val) = NFT_QUOTA_F_INV; }
#line 12034 "src/parser_bison.c"
    break;

  case 570: /* quota_mode: "until"  */
#line 3510 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12040 "src/parser_bison.c"
    break;

  case 571: /* quota_mode: %empty  */
#line 3511 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12046 "src/parser_bison.c"
    break;

  case 572: /* quota_unit: "bytes"  */
#line 3514 "src/parser_bison.y"
                                                { (yyval.string) = xstrdup("bytes"); }
#line 12052 "src/parser_bison.c"
    break;

  case 573: /* quota_unit: "string"  */
#line 3515 "src/parser_bison.y"
                                                { (yyval.string) = (yyvsp[0].string); }
#line 12058 "src/parser_bison.c"
    break;

  case 574: /* quota_used: %empty  */
#line 3518 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12064 "src/parser_bison.c"
    break;

  case 575: /* quota_used: "used" "number" quota_unit  */
#line 3520 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[0].string), &rate);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[-1].val) * rate;
			}
#line 12081 "src/parser_bison.c"
    break;

  case 576: /* quota_stmt: "quota" quota_mode "number" quota_unit quota_used close_scope_quota  */
#line 3535 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[-2].string), &rate);
				free_const((yyvsp[-2].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.stmt) = quota_stmt_alloc(&(yyloc));
				(yyval.stmt)->quota.bytes	= (yyvsp[-3].val) * rate;
				(yyval.stmt)->quota.used = (yyvsp[-1].val);
				(yyval.stmt)->quota.flags	= (yyvsp[-4].val);
			}
#line 12101 "src/parser_bison.c"
    break;

  case 577: /* limit_mode: "over"  */
#line 3552 "src/parser_bison.y"
                                                                { (yyval.val) = NFT_LIMIT_F_INV; }
#line 12107 "src/parser_bison.c"
    break;

  case 578: /* limit_mode: "until"  */
#line 3553 "src/parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 12113 "src/parser_bison.c"
    break;

  case 579: /* limit_mode: %empty  */
#line 3554 "src/parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 12119 "src/parser_bison.c"
    break;

  case 580: /* limit_burst_pkts: %empty  */
#line 3557 "src/parser_bison.y"
                                                                { (yyval.val) = 5; }
#line 12125 "src/parser_bison.c"
    break;

  case 581: /* limit_burst_pkts: "burst" "number" "packets"  */
#line 3558 "src/parser_bison.y"
                                                                { (yyval.val) = (yyvsp[-1].val); }
#line 12131 "src/parser_bison.c"
    break;

  case 582: /* limit_rate_pkts: "number" "/" time_unit  */
#line 3562 "src/parser_bison.y"
                        {
				(yyval.limit_rate).rate = (yyvsp[-2].val);
				(yyval.limit_rate).unit = (yyvsp[0].val);
			}
#line 12140 "src/parser_bison.c"
    break;

  case 583: /* limit_burst_bytes: %empty  */
#line 3568 "src/parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 12146 "src/parser_bison.c"
    break;

  case 584: /* limit_burst_bytes: "burst" limit_bytes  */
#line 3569 "src/parser_bison.y"
                                                                { (yyval.val) = (yyvsp[0].val); }
#line 12152 "src/parser_bison.c"
    break;

  case 585: /* limit_rate_bytes: "number" "string"  */
#line 3573 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate, unit;

				erec = rate_parse(&(yyloc), (yyvsp[0].string), &rate, &unit);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.limit_rate).rate = rate * (yyvsp[-1].val);
				(yyval.limit_rate).unit = unit;
			}
#line 12170 "src/parser_bison.c"
    break;

  case 586: /* limit_rate_bytes: limit_bytes "/" time_unit  */
#line 3587 "src/parser_bison.y"
                        {
				(yyval.limit_rate).rate = (yyvsp[-2].val);
				(yyval.limit_rate).unit = (yyvsp[0].val);
			}
#line 12179 "src/parser_bison.c"
    break;

  case 587: /* limit_bytes: "number" "bytes"  */
#line 3593 "src/parser_bison.y"
                                                        { (yyval.val) = (yyvsp[-1].val); }
#line 12185 "src/parser_bison.c"
    break;

  case 588: /* limit_bytes: "number" "string"  */
#line 3595 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[0].string), &rate);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[-1].val) * rate;
			}
#line 12202 "src/parser_bison.c"
    break;

  case 589: /* time_unit: "second"  */
#line 3609 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL; }
#line 12208 "src/parser_bison.c"
    break;

  case 590: /* time_unit: "minute"  */
#line 3610 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60; }
#line 12214 "src/parser_bison.c"
    break;

  case 591: /* time_unit: "hour"  */
#line 3611 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60; }
#line 12220 "src/parser_bison.c"
    break;

  case 592: /* time_unit: "day"  */
#line 3612 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60 * 24; }
#line 12226 "src/parser_bison.c"
    break;

  case 593: /* time_unit: "week"  */
#line 3613 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60 * 24 * 7; }
#line 12232 "src/parser_bison.c"
    break;

  case 595: /* reject_stmt_alloc: "reject"  */
#line 3620 "src/parser_bison.y"
                        {
				(yyval.stmt) = reject_stmt_alloc(&(yyloc));
			}
#line 12240 "src/parser_bison.c"
    break;

  case 596: /* reject_with_expr: "string"  */
#line 3626 "src/parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state), (yyvsp[0].string));
				free_const((yyvsp[0].string));
			}
#line 12250 "src/parser_bison.c"
    break;

  case 597: /* reject_with_expr: integer_expr  */
#line 3631 "src/parser_bison.y"
                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12256 "src/parser_bison.c"
    break;

  case 598: /* reject_opts: %empty  */
#line 3635 "src/parser_bison.y"
                        {
				(yyvsp[0].stmt)->reject.type = -1;
				(yyvsp[0].stmt)->reject.icmp_code = -1;
			}
#line 12265 "src/parser_bison.c"
    break;

  case 599: /* reject_opts: "with" "icmp" "type" reject_with_expr close_scope_type close_scope_icmp  */
#line 3640 "src/parser_bison.y"
                        {
				(yyvsp[-6].stmt)->reject.family = NFPROTO_IPV4;
				(yyvsp[-6].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-6].stmt)->reject.expr = (yyvsp[-2].expr);
				datatype_set((yyvsp[-6].stmt)->reject.expr, &reject_icmp_code_type);
			}
#line 12276 "src/parser_bison.c"
    break;

  case 600: /* reject_opts: "with" "icmp" reject_with_expr  */
#line 3647 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.family = NFPROTO_IPV4;
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &reject_icmp_code_type);
			}
#line 12287 "src/parser_bison.c"
    break;

  case 601: /* reject_opts: "with" "icmpv6" "type" reject_with_expr close_scope_type close_scope_icmp  */
#line 3654 "src/parser_bison.y"
                        {
				(yyvsp[-6].stmt)->reject.family = NFPROTO_IPV6;
				(yyvsp[-6].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-6].stmt)->reject.expr = (yyvsp[-2].expr);
				datatype_set((yyvsp[-6].stmt)->reject.expr, &reject_icmpv6_code_type);
			}
#line 12298 "src/parser_bison.c"
    break;

  case 602: /* reject_opts: "with" "icmpv6" reject_with_expr  */
#line 3661 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.family = NFPROTO_IPV6;
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &reject_icmpv6_code_type);
			}
#line 12309 "src/parser_bison.c"
    break;

  case 603: /* reject_opts: "with" "icmpx" "type" reject_with_expr close_scope_type  */
#line 3668 "src/parser_bison.y"
                        {
				(yyvsp[-5].stmt)->reject.type = NFT_REJECT_ICMPX_UNREACH;
				(yyvsp[-5].stmt)->reject.expr = (yyvsp[-1].expr);
				datatype_set((yyvsp[-5].stmt)->reject.expr, &reject_icmpx_code_type);
			}
#line 12319 "src/parser_bison.c"
    break;

  case 604: /* reject_opts: "with" "icmpx" reject_with_expr  */
#line 3674 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMPX_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &reject_icmpx_code_type);
			}
#line 12329 "src/parser_bison.c"
    break;

  case 605: /* reject_opts: "with" "tcp" close_scope_tcp "reset" close_scope_reset  */
#line 3680 "src/parser_bison.y"
                        {
				(yyvsp[-5].stmt)->reject.type = NFT_REJECT_TCP_RST;
			}
#line 12337 "src/parser_bison.c"
    break;

  case 607: /* nat_stmt_alloc: "snat"  */
#line 3688 "src/parser_bison.y"
                                        { (yyval.stmt) = nat_stmt_alloc(&(yyloc), __NFT_NAT_SNAT); }
#line 12343 "src/parser_bison.c"
    break;

  case 608: /* nat_stmt_alloc: "dnat"  */
#line 3689 "src/parser_bison.y"
                                        { (yyval.stmt) = nat_stmt_alloc(&(yyloc), __NFT_NAT_DNAT); }
#line 12349 "src/parser_bison.c"
    break;

  case 609: /* tproxy_stmt: "tproxy" "to" stmt_expr  */
#line 3693 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.addr = (yyvsp[0].expr);
			}
#line 12359 "src/parser_bison.c"
    break;

  case 610: /* tproxy_stmt: "tproxy" nf_key_proto "to" stmt_expr  */
#line 3699 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-2].val);
				(yyval.stmt)->tproxy.addr = (yyvsp[0].expr);
			}
#line 12369 "src/parser_bison.c"
    break;

  case 611: /* tproxy_stmt: "tproxy" "to" "colon" stmt_expr  */
#line 3705 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12379 "src/parser_bison.c"
    break;

  case 612: /* tproxy_stmt: "tproxy" "to" stmt_expr "colon" stmt_expr  */
#line 3711 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.addr = (yyvsp[-2].expr);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12390 "src/parser_bison.c"
    break;

  case 613: /* tproxy_stmt: "tproxy" nf_key_proto "to" stmt_expr "colon" stmt_expr  */
#line 3718 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-4].val);
				(yyval.stmt)->tproxy.addr = (yyvsp[-2].expr);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12401 "src/parser_bison.c"
    break;

  case 614: /* tproxy_stmt: "tproxy" nf_key_proto "to" "colon" stmt_expr  */
#line 3725 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-3].val);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12411 "src/parser_bison.c"
    break;

  case 617: /* synproxy_stmt_alloc: "synproxy"  */
#line 3737 "src/parser_bison.y"
                        {
				(yyval.stmt) = synproxy_stmt_alloc(&(yyloc));
			}
#line 12419 "src/parser_bison.c"
    break;

  case 618: /* synproxy_args: synproxy_arg  */
#line 3743 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 12427 "src/parser_bison.c"
    break;

  case 620: /* synproxy_arg: "mss" "number"  */
#line 3750 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->synproxy.mss = (yyvsp[0].val);
				(yyvsp[-2].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_MSS;
			}
#line 12436 "src/parser_bison.c"
    break;

  case 621: /* synproxy_arg: "wscale" "number"  */
#line 3755 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->synproxy.wscale = (yyvsp[0].val);
				(yyvsp[-2].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_WSCALE;
			}
#line 12445 "src/parser_bison.c"
    break;

  case 622: /* synproxy_arg: "timestamp"  */
#line 3760 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_TIMESTAMP;
			}
#line 12453 "src/parser_bison.c"
    break;

  case 623: /* synproxy_arg: "sack-permitted"  */
#line 3764 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_SACK_PERM;
			}
#line 12461 "src/parser_bison.c"
    break;

  case 624: /* synproxy_config: "mss" "number" "wscale" "number" synproxy_ts synproxy_sack  */
#line 3770 "src/parser_bison.y"
                        {
				struct synproxy *synproxy;
				uint32_t flags = 0;

				synproxy = &(yyvsp[-6].obj)->synproxy;
				synproxy->mss = (yyvsp[-4].val);
				flags |= NF_SYNPROXY_OPT_MSS;
				synproxy->wscale = (yyvsp[-2].val);
				flags |= NF_SYNPROXY_OPT_WSCALE;
				if ((yyvsp[-1].val))
					flags |= (yyvsp[-1].val);
				if ((yyvsp[0].val))
					flags |= (yyvsp[0].val);
				synproxy->flags = flags;
			}
#line 12481 "src/parser_bison.c"
    break;

  case 625: /* synproxy_config: "mss" "number" stmt_separator "wscale" "number" stmt_separator synproxy_ts synproxy_sack  */
#line 3786 "src/parser_bison.y"
                        {
				struct synproxy *synproxy;
				uint32_t flags = 0;

				synproxy = &(yyvsp[-8].obj)->synproxy;
				synproxy->mss = (yyvsp[-6].val);
				flags |= NF_SYNPROXY_OPT_MSS;
				synproxy->wscale = (yyvsp[-3].val);
				flags |= NF_SYNPROXY_OPT_WSCALE;
				if ((yyvsp[-1].val))
					flags |= (yyvsp[-1].val);
				if ((yyvsp[0].val))
					flags |= (yyvsp[0].val);
				synproxy->flags = flags;
			}
#line 12501 "src/parser_bison.c"
    break;

  case 626: /* synproxy_obj: %empty  */
#line 3804 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_SYNPROXY;
			}
#line 12510 "src/parser_bison.c"
    break;

  case 627: /* synproxy_ts: %empty  */
#line 3810 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12516 "src/parser_bison.c"
    break;

  case 628: /* synproxy_ts: "timestamp"  */
#line 3812 "src/parser_bison.y"
                        {
				(yyval.val) = NF_SYNPROXY_OPT_TIMESTAMP;
			}
#line 12524 "src/parser_bison.c"
    break;

  case 629: /* synproxy_sack: %empty  */
#line 3817 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12530 "src/parser_bison.c"
    break;

  case 630: /* synproxy_sack: "sack-permitted"  */
#line 3819 "src/parser_bison.y"
                        {
				(yyval.val) = NF_SYNPROXY_OPT_SACK_PERM;
			}
#line 12538 "src/parser_bison.c"
    break;

  case 631: /* primary_stmt_expr: symbol_expr  */
#line 3824 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12544 "src/parser_bison.c"
    break;

  case 632: /* primary_stmt_expr: integer_expr  */
#line 3825 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12550 "src/parser_bison.c"
    break;

  case 633: /* primary_stmt_expr: boolean_expr  */
#line 3826 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12556 "src/parser_bison.c"
    break;

  case 634: /* primary_stmt_expr: meta_expr  */
#line 3827 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12562 "src/parser_bison.c"
    break;

  case 635: /* primary_stmt_expr: rt_expr  */
#line 3828 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12568 "src/parser_bison.c"
    break;

  case 636: /* primary_stmt_expr: ct_expr  */
#line 3829 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12574 "src/parser_bison.c"
    break;

  case 637: /* primary_stmt_expr: numgen_expr  */
#line 3830 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12580 "src/parser_bison.c"
    break;

  case 638: /* primary_stmt_expr: hash_expr  */
#line 3831 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12586 "src/parser_bison.c"
    break;

  case 639: /* primary_stmt_expr: payload_expr  */
#line 3832 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12592 "src/parser_bison.c"
    break;

  case 640: /* primary_stmt_expr: keyword_expr  */
#line 3833 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12598 "src/parser_bison.c"
    break;

  case 641: /* primary_stmt_expr: socket_expr  */
#line 3834 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12604 "src/parser_bison.c"
    break;

  case 642: /* primary_stmt_expr: osf_expr  */
#line 3835 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12610 "src/parser_bison.c"
    break;

  case 643: /* primary_stmt_expr: '(' basic_stmt_expr ')'  */
#line 3836 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 12616 "src/parser_bison.c"
    break;

  case 645: /* shift_stmt_expr: shift_stmt_expr "<<" primary_stmt_expr  */
#line 3841 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12624 "src/parser_bison.c"
    break;

  case 646: /* shift_stmt_expr: shift_stmt_expr ">>" primary_stmt_expr  */
#line 3845 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12632 "src/parser_bison.c"
    break;

  case 648: /* and_stmt_expr: and_stmt_expr "&" shift_stmt_expr  */
#line 3852 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12640 "src/parser_bison.c"
    break;

  case 650: /* exclusive_or_stmt_expr: exclusive_or_stmt_expr "^" and_stmt_expr  */
#line 3859 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12648 "src/parser_bison.c"
    break;

  case 652: /* inclusive_or_stmt_expr: inclusive_or_stmt_expr '|' exclusive_or_stmt_expr  */
#line 3866 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12656 "src/parser_bison.c"
    break;

  case 655: /* concat_stmt_expr: concat_stmt_expr "." primary_stmt_expr  */
#line 3876 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 12669 "src/parser_bison.c"
    break;

  case 658: /* map_stmt_expr: concat_stmt_expr "map" map_stmt_expr_set  */
#line 3891 "src/parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12677 "src/parser_bison.c"
    break;

  case 659: /* map_stmt_expr: concat_stmt_expr  */
#line 3894 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 12683 "src/parser_bison.c"
    break;

  case 660: /* prefix_stmt_expr: basic_stmt_expr "/" "number"  */
#line 3898 "src/parser_bison.y"
                        {
				(yyval.expr) = prefix_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].val));
			}
#line 12691 "src/parser_bison.c"
    break;

  case 661: /* range_stmt_expr: basic_stmt_expr "-" basic_stmt_expr  */
#line 3904 "src/parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12699 "src/parser_bison.c"
    break;

  case 667: /* nat_stmt_args: stmt_expr  */
#line 3919 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12707 "src/parser_bison.c"
    break;

  case 668: /* nat_stmt_args: "to" stmt_expr  */
#line 3923 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12715 "src/parser_bison.c"
    break;

  case 669: /* nat_stmt_args: nf_key_proto "to" stmt_expr  */
#line 3927 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.family = (yyvsp[-2].val);
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12724 "src/parser_bison.c"
    break;

  case 670: /* nat_stmt_args: stmt_expr "colon" stmt_expr  */
#line 3932 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12733 "src/parser_bison.c"
    break;

  case 671: /* nat_stmt_args: "to" stmt_expr "colon" stmt_expr  */
#line 3937 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12742 "src/parser_bison.c"
    break;

  case 672: /* nat_stmt_args: nf_key_proto "to" stmt_expr "colon" stmt_expr  */
#line 3942 "src/parser_bison.y"
                        {
				(yyvsp[-5].stmt)->nat.family = (yyvsp[-4].val);
				(yyvsp[-5].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-5].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12752 "src/parser_bison.c"
    break;

  case 673: /* nat_stmt_args: "colon" stmt_expr  */
#line 3948 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12760 "src/parser_bison.c"
    break;

  case 674: /* nat_stmt_args: "to" "colon" stmt_expr  */
#line 3952 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12768 "src/parser_bison.c"
    break;

  case 675: /* nat_stmt_args: nat_stmt_args nf_nat_flags  */
#line 3956 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12776 "src/parser_bison.c"
    break;

  case 676: /* nat_stmt_args: nf_key_proto "addr" "." "port" "to" stmt_expr  */
#line 3960 "src/parser_bison.y"
                        {
				(yyvsp[-6].stmt)->nat.family = (yyvsp[-5].val);
				(yyvsp[-6].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-6].stmt)->nat.type_flags = STMT_NAT_F_CONCAT;
			}
#line 12786 "src/parser_bison.c"
    break;

  case 677: /* nat_stmt_args: nf_key_proto "interval" "to" stmt_expr  */
#line 3966 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.family = (yyvsp[-3].val);
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12795 "src/parser_bison.c"
    break;

  case 678: /* nat_stmt_args: "interval" "to" stmt_expr  */
#line 3971 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12803 "src/parser_bison.c"
    break;

  case 679: /* nat_stmt_args: nf_key_proto "prefix" "to" stmt_expr  */
#line 3975 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.family = (yyvsp[-3].val);
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-4].stmt)->nat.type_flags =
						STMT_NAT_F_PREFIX;
				(yyvsp[-4].stmt)->nat.flags |= NF_NAT_RANGE_NETMAP;
			}
#line 12815 "src/parser_bison.c"
    break;

  case 680: /* nat_stmt_args: "prefix" "to" stmt_expr  */
#line 3983 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-3].stmt)->nat.type_flags =
						STMT_NAT_F_PREFIX;
				(yyvsp[-3].stmt)->nat.flags |= NF_NAT_RANGE_NETMAP;
			}
#line 12826 "src/parser_bison.c"
    break;

  case 683: /* masq_stmt_alloc: "masquerade"  */
#line 3995 "src/parser_bison.y"
                                                { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_MASQ); }
#line 12832 "src/parser_bison.c"
    break;

  case 684: /* masq_stmt_args: "to" "colon" stmt_expr  */
#line 3999 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12840 "src/parser_bison.c"
    break;

  case 685: /* masq_stmt_args: "to" "colon" stmt_expr nf_nat_flags  */
#line 4003 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-4].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12849 "src/parser_bison.c"
    break;

  case 686: /* masq_stmt_args: nf_nat_flags  */
#line 4008 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12857 "src/parser_bison.c"
    break;

  case 689: /* redir_stmt_alloc: "redirect"  */
#line 4017 "src/parser_bison.y"
                                                { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_REDIR); }
#line 12863 "src/parser_bison.c"
    break;

  case 690: /* redir_stmt_arg: "to" stmt_expr  */
#line 4021 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12871 "src/parser_bison.c"
    break;

  case 691: /* redir_stmt_arg: "to" "colon" stmt_expr  */
#line 4025 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12879 "src/parser_bison.c"
    break;

  case 692: /* redir_stmt_arg: nf_nat_flags  */
#line 4029 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12887 "src/parser_bison.c"
    break;

  case 693: /* redir_stmt_arg: "to" stmt_expr nf_nat_flags  */
#line 4033 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-3].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12896 "src/parser_bison.c"
    break;

  case 694: /* redir_stmt_arg: "to" "colon" stmt_expr nf_nat_flags  */
#line 4038 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-4].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12905 "src/parser_bison.c"
    break;

  case 695: /* dup_stmt: "dup" "to" stmt_expr  */
#line 4045 "src/parser_bison.y"
                        {
				(yyval.stmt) = dup_stmt_alloc(&(yyloc));
				(yyval.stmt)->dup.to = (yyvsp[0].expr);
			}
#line 12914 "src/parser_bison.c"
    break;

  case 696: /* dup_stmt: "dup" "to" stmt_expr "device" stmt_expr  */
#line 4050 "src/parser_bison.y"
                        {
				(yyval.stmt) = dup_stmt_alloc(&(yyloc));
				(yyval.stmt)->dup.to = (yyvsp[-2].expr);
				(yyval.stmt)->dup.dev = (yyvsp[0].expr);
			}
#line 12924 "src/parser_bison.c"
    break;

  case 697: /* fwd_stmt: "fwd" "to" stmt_expr  */
#line 4058 "src/parser_bison.y"
                        {
				(yyval.stmt) = fwd_stmt_alloc(&(yyloc));
				(yyval.stmt)->fwd.dev = (yyvsp[0].expr);
			}
#line 12933 "src/parser_bison.c"
    break;

  case 698: /* fwd_stmt: "fwd" nf_key_proto "to" stmt_expr "device" stmt_expr  */
#line 4063 "src/parser_bison.y"
                        {
				(yyval.stmt) = fwd_stmt_alloc(&(yyloc));
				(yyval.stmt)->fwd.family = (yyvsp[-4].val);
				(yyval.stmt)->fwd.addr = (yyvsp[-2].expr);
				(yyval.stmt)->fwd.dev = (yyvsp[0].expr);
			}
#line 12944 "src/parser_bison.c"
    break;

  case 700: /* nf_nat_flags: nf_nat_flags "comma" nf_nat_flag  */
#line 4073 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 12952 "src/parser_bison.c"
    break;

  case 701: /* nf_nat_flag: "random"  */
#line 4078 "src/parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PROTO_RANDOM; }
#line 12958 "src/parser_bison.c"
    break;

  case 702: /* nf_nat_flag: "fully-random"  */
#line 4079 "src/parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PROTO_RANDOM_FULLY; }
#line 12964 "src/parser_bison.c"
    break;

  case 703: /* nf_nat_flag: "persistent"  */
#line 4080 "src/parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PERSISTENT; }
#line 12970 "src/parser_bison.c"
    break;

  case 705: /* queue_stmt: "queue" "to" queue_stmt_expr close_scope_queue  */
#line 4085 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), 0);
			}
#line 12978 "src/parser_bison.c"
    break;

  case 706: /* queue_stmt: "queue" "flags" queue_stmt_flags "to" queue_stmt_expr close_scope_queue  */
#line 4089 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), (yyvsp[-3].val));
			}
#line 12986 "src/parser_bison.c"
    break;

  case 707: /* queue_stmt: "queue" "flags" queue_stmt_flags "num" queue_stmt_expr_simple close_scope_queue  */
#line 4093 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), (yyvsp[-3].val));
			}
#line 12994 "src/parser_bison.c"
    break;

  case 710: /* queue_stmt_alloc: "queue"  */
#line 4103 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), NULL, 0);
			}
#line 13002 "src/parser_bison.c"
    break;

  case 711: /* queue_stmt_args: queue_stmt_arg  */
#line 4109 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 13010 "src/parser_bison.c"
    break;

  case 713: /* queue_stmt_arg: "num" queue_stmt_expr_simple  */
#line 4116 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->queue.queue = (yyvsp[0].expr);
				(yyvsp[-2].stmt)->queue.queue->location = (yyloc);
			}
#line 13019 "src/parser_bison.c"
    break;

  case 714: /* queue_stmt_arg: queue_stmt_flags  */
#line 4121 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->queue.flags |= (yyvsp[0].val);
			}
#line 13027 "src/parser_bison.c"
    break;

  case 719: /* queue_stmt_expr_simple: queue_expr "-" queue_expr  */
#line 4133 "src/parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13035 "src/parser_bison.c"
    break;

  case 725: /* queue_stmt_flags: queue_stmt_flags "comma" queue_stmt_flag  */
#line 4146 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 13043 "src/parser_bison.c"
    break;

  case 726: /* queue_stmt_flag: "bypass"  */
#line 4151 "src/parser_bison.y"
                                        { (yyval.val) = NFT_QUEUE_FLAG_BYPASS; }
#line 13049 "src/parser_bison.c"
    break;

  case 727: /* queue_stmt_flag: "fanout"  */
#line 4152 "src/parser_bison.y"
                                        { (yyval.val) = NFT_QUEUE_FLAG_CPU_FANOUT; }
#line 13055 "src/parser_bison.c"
    break;

  case 730: /* set_elem_expr_stmt_alloc: concat_expr  */
#line 4160 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 13063 "src/parser_bison.c"
    break;

  case 731: /* set_stmt: "set" set_stmt_op set_elem_expr_stmt set_ref_expr  */
#line 4166 "src/parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-2].val);
				(yyval.stmt)->set.key = (yyvsp[-1].expr);
				(yyval.stmt)->set.set = (yyvsp[0].expr);
			}
#line 13074 "src/parser_bison.c"
    break;

  case 732: /* set_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt '}'  */
#line 4173 "src/parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-4].val);
				(yyval.stmt)->set.key = (yyvsp[-1].expr);
				(yyval.stmt)->set.set = (yyvsp[-3].expr);
			}
#line 13085 "src/parser_bison.c"
    break;

  case 733: /* set_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt stateful_stmt_list '}'  */
#line 4180 "src/parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-5].val);
				(yyval.stmt)->set.key = (yyvsp[-2].expr);
				(yyval.stmt)->set.set = (yyvsp[-4].expr);
				list_splice_tail((yyvsp[-1].list), &(yyval.stmt)->set.stmt_list);
				free((yyvsp[-1].list));
			}
#line 13098 "src/parser_bison.c"
    break;

  case 734: /* set_stmt_op: "add"  */
#line 4190 "src/parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_ADD; }
#line 13104 "src/parser_bison.c"
    break;

  case 735: /* set_stmt_op: "update"  */
#line 4191 "src/parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_UPDATE; }
#line 13110 "src/parser_bison.c"
    break;

  case 736: /* set_stmt_op: "delete"  */
#line 4192 "src/parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_DELETE; }
#line 13116 "src/parser_bison.c"
    break;

  case 737: /* map_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt "colon" set_elem_expr_stmt '}'  */
#line 4196 "src/parser_bison.y"
                        {
				(yyval.stmt) = map_stmt_alloc(&(yyloc));
				(yyval.stmt)->map.op  = (yyvsp[-6].val);
				(yyval.stmt)->map.key = (yyvsp[-3].expr);
				(yyval.stmt)->map.data = (yyvsp[-1].expr);
				(yyval.stmt)->map.set = (yyvsp[-5].expr);
			}
#line 13128 "src/parser_bison.c"
    break;

  case 738: /* map_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt stateful_stmt_list "colon" set_elem_expr_stmt '}'  */
#line 4204 "src/parser_bison.y"
                        {
				(yyval.stmt) = map_stmt_alloc(&(yyloc));
				(yyval.stmt)->map.op  = (yyvsp[-7].val);
				(yyval.stmt)->map.key = (yyvsp[-4].expr);
				(yyval.stmt)->map.data = (yyvsp[-1].expr);
				(yyval.stmt)->map.set = (yyvsp[-6].expr);
				list_splice_tail((yyvsp[-3].list), &(yyval.stmt)->map.stmt_list);
				free((yyvsp[-3].list));
			}
#line 13142 "src/parser_bison.c"
    break;

  case 739: /* meter_stmt: meter_stmt_alloc  */
#line 4215 "src/parser_bison.y"
                                                                { (yyval.stmt) = (yyvsp[0].stmt); }
#line 13148 "src/parser_bison.c"
    break;

  case 740: /* meter_stmt_alloc: "meter" identifier '{' meter_key_expr stmt '}'  */
#line 4219 "src/parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
				(yyval.stmt)->meter.name = (yyvsp[-4].string);
				(yyval.stmt)->meter.size = 0;
				(yyval.stmt)->meter.key  = (yyvsp[-2].expr);
				(yyval.stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
			}
#line 13161 "src/parser_bison.c"
    break;

  case 741: /* meter_stmt_alloc: "meter" identifier "size" "number" '{' meter_key_expr stmt '}'  */
#line 4228 "src/parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
				(yyval.stmt)->meter.name = (yyvsp[-6].string);
				(yyval.stmt)->meter.size = (yyvsp[-4].val);
				(yyval.stmt)->meter.key  = (yyvsp[-2].expr);
				(yyval.stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
			}
#line 13174 "src/parser_bison.c"
    break;

  case 742: /* match_stmt: relational_expr  */
#line 4239 "src/parser_bison.y"
                        {
				(yyval.stmt) = expr_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 13182 "src/parser_bison.c"
    break;

  case 743: /* variable_expr: '$' identifier  */
#line 4245 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);
				struct symbol *sym;

				sym = symbol_get(scope, (yyvsp[0].string));
				if (!sym) {
					sym = symbol_lookup_fuzzy(scope, (yyvsp[0].string));
					if (sym) {
						erec_queue(error(&(yylsp[0]), "unknown identifier '%s'; "
								      "did you mean identifier '%s’?",
								      (yyvsp[0].string), sym->identifier),
							   state->msgs);
					} else {
						erec_queue(error(&(yylsp[0]), "unknown identifier '%s'", (yyvsp[0].string)),
							   state->msgs);
					}
					free_const((yyvsp[0].string));
					YYERROR;
				}

				(yyval.expr) = variable_expr_alloc(&(yyloc), scope, sym);
				free_const((yyvsp[0].string));
			}
#line 13210 "src/parser_bison.c"
    break;

  case 745: /* symbol_expr: string  */
#line 4272 "src/parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state),
						       (yyvsp[0].string));
				free_const((yyvsp[0].string));
			}
#line 13221 "src/parser_bison.c"
    break;

  case 748: /* set_ref_symbol_expr: "@" identifier close_scope_at  */
#line 4285 "src/parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_SET,
						       current_scope(state),
						       (yyvsp[-1].string));
				free_const((yyvsp[-1].string));
			}
#line 13232 "src/parser_bison.c"
    break;

  case 749: /* integer_expr: "number"  */
#line 4294 "src/parser_bison.y"
                        {
				char str[64];

				snprintf(str, sizeof(str), "%" PRIu64, (yyvsp[0].val));
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state),
						       str);
			}
#line 13245 "src/parser_bison.c"
    break;

  case 750: /* primary_expr: symbol_expr  */
#line 4304 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13251 "src/parser_bison.c"
    break;

  case 751: /* primary_expr: integer_expr  */
#line 4305 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13257 "src/parser_bison.c"
    break;

  case 752: /* primary_expr: payload_expr  */
#line 4306 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13263 "src/parser_bison.c"
    break;

  case 753: /* primary_expr: exthdr_expr  */
#line 4307 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13269 "src/parser_bison.c"
    break;

  case 754: /* primary_expr: exthdr_exists_expr  */
#line 4308 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13275 "src/parser_bison.c"
    break;

  case 755: /* primary_expr: meta_expr  */
#line 4309 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13281 "src/parser_bison.c"
    break;

  case 756: /* primary_expr: socket_expr  */
#line 4310 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13287 "src/parser_bison.c"
    break;

  case 757: /* primary_expr: rt_expr  */
#line 4311 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13293 "src/parser_bison.c"
    break;

  case 758: /* primary_expr: ct_expr  */
#line 4312 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13299 "src/parser_bison.c"
    break;

  case 759: /* primary_expr: numgen_expr  */
#line 4313 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13305 "src/parser_bison.c"
    break;

  case 760: /* primary_expr: hash_expr  */
#line 4314 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13311 "src/parser_bison.c"
    break;

  case 761: /* primary_expr: fib_expr  */
#line 4315 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13317 "src/parser_bison.c"
    break;

  case 762: /* primary_expr: osf_expr  */
#line 4316 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13323 "src/parser_bison.c"
    break;

  case 763: /* primary_expr: xfrm_expr  */
#line 4317 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13329 "src/parser_bison.c"
    break;

  case 764: /* primary_expr: '(' basic_expr ')'  */
#line 4318 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 13335 "src/parser_bison.c"
    break;

  case 765: /* fib_expr: "fib" fib_tuple fib_result close_scope_fib  */
#line 4322 "src/parser_bison.y"
                        {
				if (((yyvsp[-2].val) & (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) == 0) {
					erec_queue(error(&(yylsp[-2]), "fib: need either saddr or daddr"), state->msgs);
					YYERROR;
				}

				if (((yyvsp[-2].val) & (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) ==
					  (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) {
					erec_queue(error(&(yylsp[-2]), "fib: saddr and daddr are mutually exclusive"), state->msgs);
					YYERROR;
				}

				if (((yyvsp[-2].val) & (NFTA_FIB_F_IIF|NFTA_FIB_F_OIF)) ==
					  (NFTA_FIB_F_IIF|NFTA_FIB_F_OIF)) {
					erec_queue(error(&(yylsp[-2]), "fib: iif and oif are mutually exclusive"), state->msgs);
					YYERROR;
				}

				(yyval.expr) = fib_expr_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 13360 "src/parser_bison.c"
    break;

  case 766: /* fib_result: "oif"  */
#line 4344 "src/parser_bison.y"
                                        { (yyval.val) =NFT_FIB_RESULT_OIF; }
#line 13366 "src/parser_bison.c"
    break;

  case 767: /* fib_result: "oifname"  */
#line 4345 "src/parser_bison.y"
                                        { (yyval.val) =NFT_FIB_RESULT_OIFNAME; }
#line 13372 "src/parser_bison.c"
    break;

  case 768: /* fib_result: "type" close_scope_type  */
#line 4346 "src/parser_bison.y"
                                                                { (yyval.val) =NFT_FIB_RESULT_ADDRTYPE; }
#line 13378 "src/parser_bison.c"
    break;

  case 769: /* fib_flag: "saddr"  */
#line 4349 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_SADDR; }
#line 13384 "src/parser_bison.c"
    break;

  case 770: /* fib_flag: "daddr"  */
#line 4350 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_DADDR; }
#line 13390 "src/parser_bison.c"
    break;

  case 771: /* fib_flag: "mark"  */
#line 4351 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_MARK; }
#line 13396 "src/parser_bison.c"
    break;

  case 772: /* fib_flag: "iif"  */
#line 4352 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_IIF; }
#line 13402 "src/parser_bison.c"
    break;

  case 773: /* fib_flag: "oif"  */
#line 4353 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_OIF; }
#line 13408 "src/parser_bison.c"
    break;

  case 774: /* fib_tuple: fib_flag "." fib_tuple  */
#line 4357 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 13416 "src/parser_bison.c"
    break;

  case 776: /* osf_expr: "osf" osf_ttl "version" close_scope_osf  */
#line 4364 "src/parser_bison.y"
                        {
				(yyval.expr) = osf_expr_alloc(&(yyloc), (yyvsp[-2].val), NFT_OSF_F_VERSION);
			}
#line 13424 "src/parser_bison.c"
    break;

  case 777: /* osf_expr: "osf" osf_ttl "name" close_scope_osf  */
#line 4368 "src/parser_bison.y"
                        {
				(yyval.expr) = osf_expr_alloc(&(yyloc), (yyvsp[-2].val), 0);
			}
#line 13432 "src/parser_bison.c"
    break;

  case 778: /* osf_ttl: %empty  */
#line 4374 "src/parser_bison.y"
                        {
				(yyval.val) = NF_OSF_TTL_TRUE;
			}
#line 13440 "src/parser_bison.c"
    break;

  case 779: /* osf_ttl: "ttl" "string"  */
#line 4378 "src/parser_bison.y"
                        {
				if (!strcmp((yyvsp[0].string), "loose"))
					(yyval.val) = NF_OSF_TTL_LESS;
				else if (!strcmp((yyvsp[0].string), "skip"))
					(yyval.val) = NF_OSF_TTL_NOCHECK;
				else {
					erec_queue(error(&(yylsp[0]), "invalid ttl option"),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 13458 "src/parser_bison.c"
    break;

  case 781: /* shift_expr: shift_expr "<<" primary_rhs_expr  */
#line 4395 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13466 "src/parser_bison.c"
    break;

  case 782: /* shift_expr: shift_expr ">>" primary_rhs_expr  */
#line 4399 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13474 "src/parser_bison.c"
    break;

  case 784: /* and_expr: and_expr "&" shift_rhs_expr  */
#line 4406 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13482 "src/parser_bison.c"
    break;

  case 786: /* exclusive_or_expr: exclusive_or_expr "^" and_rhs_expr  */
#line 4413 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13490 "src/parser_bison.c"
    break;

  case 788: /* inclusive_or_expr: inclusive_or_expr '|' exclusive_or_rhs_expr  */
#line 4420 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13498 "src/parser_bison.c"
    break;

  case 791: /* concat_expr: concat_expr "." basic_expr  */
#line 4430 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 13511 "src/parser_bison.c"
    break;

  case 792: /* prefix_rhs_expr: basic_rhs_expr "/" "number"  */
#line 4441 "src/parser_bison.y"
                        {
				(yyval.expr) = prefix_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].val));
			}
#line 13519 "src/parser_bison.c"
    break;

  case 793: /* range_rhs_expr: basic_rhs_expr "-" basic_rhs_expr  */
#line 4447 "src/parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13527 "src/parser_bison.c"
    break;

  case 796: /* map_expr: concat_expr "map" rhs_expr  */
#line 4457 "src/parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13535 "src/parser_bison.c"
    break;

  case 800: /* set_expr: '{' set_list_expr '}'  */
#line 4468 "src/parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13544 "src/parser_bison.c"
    break;

  case 801: /* set_list_expr: set_list_member_expr  */
#line 4475 "src/parser_bison.y"
                        {
				(yyval.expr) = set_expr_alloc(&(yyloc), NULL);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 13553 "src/parser_bison.c"
    break;

  case 802: /* set_list_expr: set_list_expr "comma" set_list_member_expr  */
#line 4480 "src/parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 13562 "src/parser_bison.c"
    break;

  case 804: /* set_list_member_expr: opt_newline set_expr opt_newline  */
#line 4488 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13570 "src/parser_bison.c"
    break;

  case 805: /* set_list_member_expr: opt_newline set_elem_expr opt_newline  */
#line 4492 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13578 "src/parser_bison.c"
    break;

  case 806: /* set_list_member_expr: opt_newline set_elem_expr "colon" set_rhs_expr opt_newline  */
#line 4496 "src/parser_bison.y"
                        {
				(yyval.expr) = mapping_expr_alloc(&(yylsp[-3]), (yyvsp[-3].expr), (yyvsp[-1].expr));
			}
#line 13586 "src/parser_bison.c"
    break;

  case 808: /* meter_key_expr: meter_key_expr_alloc set_elem_options  */
#line 4503 "src/parser_bison.y"
                        {
				(yyval.expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13595 "src/parser_bison.c"
    break;

  case 809: /* meter_key_expr_alloc: concat_expr  */
#line 4510 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 13603 "src/parser_bison.c"
    break;

  case 812: /* set_elem_expr: set_elem_expr_alloc set_elem_expr_options set_elem_stmt_list  */
#line 4518 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-2].expr);
				list_splice_tail((yyvsp[0].list), &(yyval.expr)->stmt_list);
				free((yyvsp[0].list));
			}
#line 13613 "src/parser_bison.c"
    break;

  case 813: /* set_elem_key_expr: set_lhs_expr  */
#line 4525 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13619 "src/parser_bison.c"
    break;

  case 814: /* set_elem_key_expr: "*"  */
#line 4526 "src/parser_bison.y"
                                                        { (yyval.expr) = set_elem_catchall_expr_alloc(&(yylsp[0])); }
#line 13625 "src/parser_bison.c"
    break;

  case 815: /* set_elem_expr_alloc: set_elem_key_expr set_elem_stmt_list  */
#line 4530 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[-1]), (yyvsp[-1].expr));
				list_splice_tail((yyvsp[0].list), &(yyval.expr)->stmt_list);
				free((yyvsp[0].list));
			}
#line 13635 "src/parser_bison.c"
    break;

  case 816: /* set_elem_expr_alloc: set_elem_key_expr  */
#line 4536 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 13643 "src/parser_bison.c"
    break;

  case 817: /* set_elem_options: set_elem_option  */
#line 4542 "src/parser_bison.y"
                        {
				(yyval.expr)	= (yyvsp[-1].expr);
			}
#line 13651 "src/parser_bison.c"
    break;

  case 819: /* set_elem_time_spec: "string"  */
#line 4549 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t res;

				if (!strcmp("never", (yyvsp[0].string))) {
					free_const((yyvsp[0].string));
					(yyval.val) = NFT_NEVER_TIMEOUT;
					break;
				}

				erec = time_parse(&(yylsp[0]), (yyvsp[0].string), &res);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = res;
			}
#line 13674 "src/parser_bison.c"
    break;

  case 820: /* set_elem_option: "timeout" time_spec  */
#line 4570 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->timeout = (yyvsp[0].val);
			}
#line 13682 "src/parser_bison.c"
    break;

  case 821: /* set_elem_option: "expires" time_spec  */
#line 4574 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->expiration = (yyvsp[0].val);
			}
#line 13690 "src/parser_bison.c"
    break;

  case 822: /* set_elem_option: comment_spec  */
#line 4578 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].expr)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].expr)->comment = (yyvsp[0].string);
			}
#line 13702 "src/parser_bison.c"
    break;

  case 823: /* set_elem_expr_options: set_elem_expr_option  */
#line 4588 "src/parser_bison.y"
                        {
				(yyval.expr)	= (yyvsp[-1].expr);
			}
#line 13710 "src/parser_bison.c"
    break;

  case 825: /* set_elem_stmt_list: set_elem_stmt  */
#line 4595 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 13720 "src/parser_bison.c"
    break;

  case 826: /* set_elem_stmt_list: set_elem_stmt_list set_elem_stmt  */
#line 4601 "src/parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 13729 "src/parser_bison.c"
    break;

  case 827: /* set_elem_stmt: "counter" close_scope_counter  */
#line 4608 "src/parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
			}
#line 13737 "src/parser_bison.c"
    break;

  case 828: /* set_elem_stmt: "counter" "packets" "number" "bytes" "number" close_scope_counter  */
#line 4612 "src/parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
				(yyval.stmt)->counter.packets = (yyvsp[-3].val);
				(yyval.stmt)->counter.bytes = (yyvsp[-1].val);
			}
#line 13747 "src/parser_bison.c"
    break;

  case 829: /* set_elem_stmt: "limit" "rate" limit_mode limit_rate_pkts limit_burst_pkts close_scope_limit  */
#line 4618 "src/parser_bison.y"
                        {
				if ((yyvsp[-1].val) == 0) {
					erec_queue(error(&(yylsp[-1]), "limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate  = (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit  = (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst = (yyvsp[-1].val);
				(yyval.stmt)->limit.type  = NFT_LIMIT_PKTS;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 13765 "src/parser_bison.c"
    break;

  case 830: /* set_elem_stmt: "limit" "rate" limit_mode limit_rate_bytes limit_burst_bytes close_scope_limit  */
#line 4632 "src/parser_bison.y"
                        {
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate  = (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit  = (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst = (yyvsp[-1].val);
				(yyval.stmt)->limit.type  = NFT_LIMIT_PKT_BYTES;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 13778 "src/parser_bison.c"
    break;

  case 831: /* set_elem_stmt: "ct" "count" "number" close_scope_ct  */
#line 4641 "src/parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count	= (yyvsp[-1].val);
			}
#line 13787 "src/parser_bison.c"
    break;

  case 832: /* set_elem_stmt: "ct" "count" "over" "number" close_scope_ct  */
#line 4646 "src/parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count = (yyvsp[-1].val);
				(yyval.stmt)->connlimit.flags = NFT_CONNLIMIT_F_INV;
			}
#line 13797 "src/parser_bison.c"
    break;

  case 833: /* set_elem_stmt: "quota" quota_mode "number" quota_unit quota_used close_scope_quota  */
#line 4652 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[-2].string), &rate);
				free_const((yyvsp[-2].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.stmt) = quota_stmt_alloc(&(yyloc));
				(yyval.stmt)->quota.bytes	= (yyvsp[-3].val) * rate;
				(yyval.stmt)->quota.used = (yyvsp[-1].val);
				(yyval.stmt)->quota.flags	= (yyvsp[-4].val);
			}
#line 13817 "src/parser_bison.c"
    break;

  case 834: /* set_elem_stmt: "last" "used" "never" close_scope_last  */
#line 4668 "src/parser_bison.y"
                        {
				(yyval.stmt) = last_stmt_alloc(&(yyloc));
			}
#line 13825 "src/parser_bison.c"
    break;

  case 835: /* set_elem_stmt: "last" "used" time_spec close_scope_last  */
#line 4672 "src/parser_bison.y"
                        {
				(yyval.stmt) = last_stmt_alloc(&(yyloc));
				(yyval.stmt)->last.used = (yyvsp[-1].val);
				(yyval.stmt)->last.set = true;
			}
#line 13835 "src/parser_bison.c"
    break;

  case 836: /* set_elem_expr_option: "timeout" set_elem_time_spec  */
#line 4680 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->timeout = (yyvsp[0].val);
			}
#line 13843 "src/parser_bison.c"
    break;

  case 837: /* set_elem_expr_option: "expires" time_spec  */
#line 4684 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->expiration = (yyvsp[0].val);
			}
#line 13851 "src/parser_bison.c"
    break;

  case 838: /* set_elem_expr_option: comment_spec  */
#line 4688 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].expr)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].expr)->comment = (yyvsp[0].string);
			}
#line 13863 "src/parser_bison.c"
    break;

  case 844: /* initializer_expr: '{' '}'  */
#line 4706 "src/parser_bison.y"
                                                { (yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_SET); }
#line 13869 "src/parser_bison.c"
    break;

  case 845: /* initializer_expr: "-" "number"  */
#line 4708 "src/parser_bison.y"
                        {
				int32_t num = -(yyvsp[0].val);

				(yyval.expr) = constant_expr_alloc(&(yyloc), &integer_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(num) * BITS_PER_BYTE,
							 &num);
			}
#line 13882 "src/parser_bison.c"
    break;

  case 846: /* counter_config: "packets" "number" "bytes" "number"  */
#line 4719 "src/parser_bison.y"
                        {
				struct counter *counter;

				counter = &(yyvsp[-4].obj)->counter;
				counter->packets = (yyvsp[-2].val);
				counter->bytes = (yyvsp[0].val);
			}
#line 13894 "src/parser_bison.c"
    break;

  case 847: /* counter_obj: %empty  */
#line 4729 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_COUNTER;
			}
#line 13903 "src/parser_bison.c"
    break;

  case 848: /* quota_config: quota_mode "number" quota_unit quota_used  */
#line 4736 "src/parser_bison.y"
                        {
				struct error_record *erec;
				struct quota *quota;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[-1].string), &rate);
				free_const((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				quota = &(yyvsp[-4].obj)->quota;
				quota->bytes	= (yyvsp[-2].val) * rate;
				quota->used	= (yyvsp[0].val);
				quota->flags	= (yyvsp[-3].val);
			}
#line 13925 "src/parser_bison.c"
    break;

  case 849: /* quota_obj: %empty  */
#line 4756 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_QUOTA;
			}
#line 13934 "src/parser_bison.c"
    break;

  case 850: /* secmark_config: string  */
#line 4763 "src/parser_bison.y"
                        {
				int ret;
				struct secmark *secmark;

				secmark = &(yyvsp[-1].obj)->secmark;
				ret = snprintf(secmark->ctx, sizeof(secmark->ctx), "%s", (yyvsp[0].string));
				if (ret <= 0 || ret >= (int)sizeof(secmark->ctx)) {
					erec_queue(error(&(yylsp[0]), "invalid context '%s', max length is %u\n", (yyvsp[0].string), (int)sizeof(secmark->ctx)), state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 13952 "src/parser_bison.c"
    break;

  case 851: /* secmark_obj: %empty  */
#line 4779 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_SECMARK;
			}
#line 13961 "src/parser_bison.c"
    break;

  case 852: /* ct_obj_type: "helper"  */
#line 4785 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_HELPER; }
#line 13967 "src/parser_bison.c"
    break;

  case 853: /* ct_obj_type: "timeout"  */
#line 4786 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_TIMEOUT; }
#line 13973 "src/parser_bison.c"
    break;

  case 854: /* ct_obj_type: "expectation"  */
#line 4787 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_EXPECT; }
#line 13979 "src/parser_bison.c"
    break;

  case 855: /* ct_cmd_type: "helpers"  */
#line 4790 "src/parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_HELPERS; }
#line 13985 "src/parser_bison.c"
    break;

  case 856: /* ct_cmd_type: "timeout"  */
#line 4791 "src/parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_TIMEOUTS; }
#line 13991 "src/parser_bison.c"
    break;

  case 857: /* ct_cmd_type: "expectation"  */
#line 4792 "src/parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_EXPECTATIONS; }
#line 13997 "src/parser_bison.c"
    break;

  case 858: /* ct_l4protoname: "tcp" close_scope_tcp  */
#line 4795 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_TCP; }
#line 14003 "src/parser_bison.c"
    break;

  case 859: /* ct_l4protoname: "udp" close_scope_udp  */
#line 4796 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_UDP; }
#line 14009 "src/parser_bison.c"
    break;

  case 860: /* ct_helper_config: "type" "quoted string" "protocol" ct_l4protoname stmt_separator close_scope_type  */
#line 4800 "src/parser_bison.y"
                        {
				struct ct_helper *ct;
				int ret;

				ct = &(yyvsp[-6].obj)->ct_helper;

				if (ct->l4proto) {
					erec_queue(error(&(yylsp[-4]), "You can only specify this once. This statement is already set for %s.", ct->name), state->msgs);
					free_const((yyvsp[-4].string));
					YYERROR;
				}

				ret = snprintf(ct->name, sizeof(ct->name), "%s", (yyvsp[-4].string));
				if (ret <= 0 || ret >= (int)sizeof(ct->name)) {
					erec_queue(error(&(yylsp[-4]), "invalid name '%s', max length is %u\n", (yyvsp[-4].string), (int)sizeof(ct->name)), state->msgs);
					free_const((yyvsp[-4].string));
					YYERROR;
				}
				free_const((yyvsp[-4].string));

				ct->l4proto = (yyvsp[-2].val);
			}
#line 14036 "src/parser_bison.c"
    break;

  case 861: /* ct_helper_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4823 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_helper.l3proto = (yyvsp[-1].val);
			}
#line 14044 "src/parser_bison.c"
    break;

  case 862: /* timeout_states: timeout_state  */
#line 4829 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].timeout_state)->head, (yyval.list));
			}
#line 14054 "src/parser_bison.c"
    break;

  case 863: /* timeout_states: timeout_states "comma" timeout_state  */
#line 4835 "src/parser_bison.y"
                        {
				list_add_tail(&(yyvsp[0].timeout_state)->head, (yyvsp[-2].list));
				(yyval.list) = (yyvsp[-2].list);
			}
#line 14063 "src/parser_bison.c"
    break;

  case 864: /* timeout_state: "string" "colon" time_spec_or_num_s  */
#line 4842 "src/parser_bison.y"
                        {
				struct timeout_state *ts;

				ts = xzalloc(sizeof(*ts));
				ts->timeout_str = (yyvsp[-2].string);
				ts->timeout_value = (yyvsp[0].val);
				ts->location = (yylsp[-2]);
				init_list_head(&ts->head);
				(yyval.timeout_state) = ts;
			}
#line 14078 "src/parser_bison.c"
    break;

  case 865: /* ct_timeout_config: "protocol" ct_l4protoname stmt_separator  */
#line 4855 "src/parser_bison.y"
                        {
				struct ct_timeout *ct;
				int l4proto = (yyvsp[-1].val);

				ct = &(yyvsp[-3].obj)->ct_timeout;
				ct->l4proto = l4proto;
			}
#line 14090 "src/parser_bison.c"
    break;

  case 866: /* ct_timeout_config: "policy" '=' '{' timeout_states '}' stmt_separator close_scope_policy  */
#line 4863 "src/parser_bison.y"
                        {
				struct ct_timeout *ct;

				ct = &(yyvsp[-7].obj)->ct_timeout;
				list_splice_tail((yyvsp[-3].list), &ct->timeout_list);
				free((yyvsp[-3].list));
			}
#line 14102 "src/parser_bison.c"
    break;

  case 867: /* ct_timeout_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4871 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_timeout.l3proto = (yyvsp[-1].val);
			}
#line 14110 "src/parser_bison.c"
    break;

  case 868: /* ct_expect_config: "protocol" ct_l4protoname stmt_separator  */
#line 4877 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.l4proto = (yyvsp[-1].val);
			}
#line 14118 "src/parser_bison.c"
    break;

  case 869: /* ct_expect_config: "dport" "number" stmt_separator  */
#line 4881 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.dport = (yyvsp[-1].val);
			}
#line 14126 "src/parser_bison.c"
    break;

  case 870: /* ct_expect_config: "timeout" time_spec stmt_separator  */
#line 4885 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.timeout = (yyvsp[-1].val);
			}
#line 14134 "src/parser_bison.c"
    break;

  case 871: /* ct_expect_config: "size" "number" stmt_separator  */
#line 4889 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.size = (yyvsp[-1].val);
			}
#line 14142 "src/parser_bison.c"
    break;

  case 872: /* ct_expect_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4893 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.l3proto = (yyvsp[-1].val);
			}
#line 14150 "src/parser_bison.c"
    break;

  case 873: /* ct_obj_alloc: %empty  */
#line 4899 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
			}
#line 14158 "src/parser_bison.c"
    break;

  case 874: /* limit_config: "rate" limit_mode limit_rate_pkts limit_burst_pkts  */
#line 4905 "src/parser_bison.y"
                        {
				struct limit *limit;

				limit = &(yyvsp[-4].obj)->limit;
				limit->rate	= (yyvsp[-1].limit_rate).rate;
				limit->unit	= (yyvsp[-1].limit_rate).unit;
				limit->burst	= (yyvsp[0].val);
				limit->type	= NFT_LIMIT_PKTS;
				limit->flags	= (yyvsp[-2].val);
			}
#line 14173 "src/parser_bison.c"
    break;

  case 875: /* limit_config: "rate" limit_mode limit_rate_bytes limit_burst_bytes  */
#line 4916 "src/parser_bison.y"
                        {
				struct limit *limit;

				limit = &(yyvsp[-4].obj)->limit;
				limit->rate	= (yyvsp[-1].limit_rate).rate;
				limit->unit	= (yyvsp[-1].limit_rate).unit;
				limit->burst	= (yyvsp[0].val);
				limit->type	= NFT_LIMIT_PKT_BYTES;
				limit->flags	= (yyvsp[-2].val);
			}
#line 14188 "src/parser_bison.c"
    break;

  case 876: /* limit_obj: %empty  */
#line 4929 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_LIMIT;
			}
#line 14197 "src/parser_bison.c"
    break;

  case 877: /* relational_expr: expr rhs_expr  */
#line 4936 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, (yyvsp[-1].expr), (yyvsp[0].expr));
			}
#line 14205 "src/parser_bison.c"
    break;

  case 878: /* relational_expr: expr list_rhs_expr  */
#line 4940 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, (yyvsp[-1].expr), (yyvsp[0].expr));
			}
#line 14213 "src/parser_bison.c"
    break;

  case 879: /* relational_expr: expr basic_rhs_expr "/" list_rhs_expr  */
#line 4944 "src/parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), OP_EQ, (yyvsp[-3].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 14221 "src/parser_bison.c"
    break;

  case 880: /* relational_expr: expr list_rhs_expr "/" list_rhs_expr  */
#line 4948 "src/parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), OP_EQ, (yyvsp[-3].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 14229 "src/parser_bison.c"
    break;

  case 881: /* relational_expr: expr relational_op basic_rhs_expr "/" list_rhs_expr  */
#line 4952 "src/parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 14237 "src/parser_bison.c"
    break;

  case 882: /* relational_expr: expr relational_op list_rhs_expr "/" list_rhs_expr  */
#line 4956 "src/parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 14245 "src/parser_bison.c"
    break;

  case 883: /* relational_expr: expr relational_op rhs_expr  */
#line 4960 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yylsp[-1]), (yyvsp[-1].val), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14253 "src/parser_bison.c"
    break;

  case 884: /* relational_expr: expr relational_op list_rhs_expr  */
#line 4964 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yylsp[-1]), (yyvsp[-1].val), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14261 "src/parser_bison.c"
    break;

  case 885: /* list_rhs_expr: basic_rhs_expr "comma" basic_rhs_expr  */
#line 4970 "src/parser_bison.y"
                        {
				(yyval.expr) = list_expr_alloc(&(yyloc));
				compound_expr_add((yyval.expr), (yyvsp[-2].expr));
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 14271 "src/parser_bison.c"
    break;

  case 886: /* list_rhs_expr: list_rhs_expr "comma" basic_rhs_expr  */
#line 4976 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->location = (yyloc);
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 14281 "src/parser_bison.c"
    break;

  case 887: /* rhs_expr: concat_rhs_expr  */
#line 4983 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14287 "src/parser_bison.c"
    break;

  case 888: /* rhs_expr: set_expr  */
#line 4984 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14293 "src/parser_bison.c"
    break;

  case 889: /* rhs_expr: set_ref_symbol_expr  */
#line 4985 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14299 "src/parser_bison.c"
    break;

  case 891: /* shift_rhs_expr: shift_rhs_expr "<<" primary_rhs_expr  */
#line 4990 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14307 "src/parser_bison.c"
    break;

  case 892: /* shift_rhs_expr: shift_rhs_expr ">>" primary_rhs_expr  */
#line 4994 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14315 "src/parser_bison.c"
    break;

  case 894: /* and_rhs_expr: and_rhs_expr "&" shift_rhs_expr  */
#line 5001 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14323 "src/parser_bison.c"
    break;

  case 896: /* exclusive_or_rhs_expr: exclusive_or_rhs_expr "^" and_rhs_expr  */
#line 5008 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14331 "src/parser_bison.c"
    break;

  case 898: /* inclusive_or_rhs_expr: inclusive_or_rhs_expr '|' exclusive_or_rhs_expr  */
#line 5015 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14339 "src/parser_bison.c"
    break;

  case 902: /* concat_rhs_expr: concat_rhs_expr "." multiton_rhs_expr  */
#line 5026 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 14352 "src/parser_bison.c"
    break;

  case 903: /* concat_rhs_expr: concat_rhs_expr "." basic_rhs_expr  */
#line 5035 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 14365 "src/parser_bison.c"
    break;

  case 904: /* boolean_keys: "exists"  */
#line 5045 "src/parser_bison.y"
                                                { (yyval.val8) = true; }
#line 14371 "src/parser_bison.c"
    break;

  case 905: /* boolean_keys: "missing"  */
#line 5046 "src/parser_bison.y"
                                                { (yyval.val8) = false; }
#line 14377 "src/parser_bison.c"
    break;

  case 906: /* boolean_expr: boolean_keys  */
#line 5050 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &boolean_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof((yyvsp[0].val8)) * BITS_PER_BYTE, &(yyvsp[0].val8));
			}
#line 14387 "src/parser_bison.c"
    break;

  case 907: /* keyword_expr: "ether" close_scope_eth  */
#line 5057 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ether"); }
#line 14393 "src/parser_bison.c"
    break;

  case 908: /* keyword_expr: "ip" close_scope_ip  */
#line 5058 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ip"); }
#line 14399 "src/parser_bison.c"
    break;

  case 909: /* keyword_expr: "ip6" close_scope_ip6  */
#line 5059 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ip6"); }
#line 14405 "src/parser_bison.c"
    break;

  case 910: /* keyword_expr: "vlan" close_scope_vlan  */
#line 5060 "src/parser_bison.y"
                                                         { (yyval.expr) = symbol_value(&(yyloc), "vlan"); }
#line 14411 "src/parser_bison.c"
    break;

  case 911: /* keyword_expr: "arp" close_scope_arp  */
#line 5061 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "arp"); }
#line 14417 "src/parser_bison.c"
    break;

  case 912: /* keyword_expr: "dnat" close_scope_nat  */
#line 5062 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "dnat"); }
#line 14423 "src/parser_bison.c"
    break;

  case 913: /* keyword_expr: "snat" close_scope_nat  */
#line 5063 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "snat"); }
#line 14429 "src/parser_bison.c"
    break;

  case 914: /* keyword_expr: "ecn"  */
#line 5064 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ecn"); }
#line 14435 "src/parser_bison.c"
    break;

  case 915: /* keyword_expr: "reset" close_scope_reset  */
#line 5065 "src/parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "reset"); }
#line 14441 "src/parser_bison.c"
    break;

  case 916: /* keyword_expr: "destroy" close_scope_destroy  */
#line 5066 "src/parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "destroy"); }
#line 14447 "src/parser_bison.c"
    break;

  case 917: /* keyword_expr: "original"  */
#line 5067 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "original"); }
#line 14453 "src/parser_bison.c"
    break;

  case 918: /* keyword_expr: "reply"  */
#line 5068 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "reply"); }
#line 14459 "src/parser_bison.c"
    break;

  case 919: /* keyword_expr: "label"  */
#line 5069 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "label"); }
#line 14465 "src/parser_bison.c"
    break;

  case 920: /* keyword_expr: "last" close_scope_last  */
#line 5070 "src/parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "last"); }
#line 14471 "src/parser_bison.c"
    break;

  case 921: /* primary_rhs_expr: symbol_expr  */
#line 5073 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14477 "src/parser_bison.c"
    break;

  case 922: /* primary_rhs_expr: integer_expr  */
#line 5074 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14483 "src/parser_bison.c"
    break;

  case 923: /* primary_rhs_expr: boolean_expr  */
#line 5075 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14489 "src/parser_bison.c"
    break;

  case 924: /* primary_rhs_expr: keyword_expr  */
#line 5076 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14495 "src/parser_bison.c"
    break;

  case 925: /* primary_rhs_expr: "tcp" close_scope_tcp  */
#line 5078 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_TCP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14506 "src/parser_bison.c"
    break;

  case 926: /* primary_rhs_expr: "udp" close_scope_udp  */
#line 5085 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_UDP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14517 "src/parser_bison.c"
    break;

  case 927: /* primary_rhs_expr: "udplite" close_scope_udplite  */
#line 5092 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_UDPLITE;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14528 "src/parser_bison.c"
    break;

  case 928: /* primary_rhs_expr: "esp" close_scope_esp  */
#line 5099 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_ESP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14539 "src/parser_bison.c"
    break;

  case 929: /* primary_rhs_expr: "ah" close_scope_ah  */
#line 5106 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_AH;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14550 "src/parser_bison.c"
    break;

  case 930: /* primary_rhs_expr: "icmp" close_scope_icmp  */
#line 5113 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_ICMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14561 "src/parser_bison.c"
    break;

  case 931: /* primary_rhs_expr: "igmp"  */
#line 5120 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_IGMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14572 "src/parser_bison.c"
    break;

  case 932: /* primary_rhs_expr: "icmpv6" close_scope_icmp  */
#line 5127 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_ICMPV6;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14583 "src/parser_bison.c"
    break;

  case 933: /* primary_rhs_expr: "gre" close_scope_gre  */
#line 5134 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_GRE;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14594 "src/parser_bison.c"
    break;

  case 934: /* primary_rhs_expr: "comp" close_scope_comp  */
#line 5141 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_COMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14605 "src/parser_bison.c"
    break;

  case 935: /* primary_rhs_expr: "dccp" close_scope_dccp  */
#line 5148 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_DCCP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14616 "src/parser_bison.c"
    break;

  case 936: /* primary_rhs_expr: "sctp" close_scope_sctp  */
#line 5155 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_SCTP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14627 "src/parser_bison.c"
    break;

  case 937: /* primary_rhs_expr: "redirect" close_scope_nat  */
#line 5162 "src/parser_bison.y"
                        {
				uint8_t data = ICMP_REDIRECT;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &icmp_type_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14638 "src/parser_bison.c"
    break;

  case 938: /* primary_rhs_expr: '(' basic_rhs_expr ')'  */
#line 5168 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 14644 "src/parser_bison.c"
    break;

  case 939: /* relational_op: "=="  */
#line 5171 "src/parser_bison.y"
                                                { (yyval.val) = OP_EQ; }
#line 14650 "src/parser_bison.c"
    break;

  case 940: /* relational_op: "!="  */
#line 5172 "src/parser_bison.y"
                                                { (yyval.val) = OP_NEQ; }
#line 14656 "src/parser_bison.c"
    break;

  case 941: /* relational_op: "<"  */
#line 5173 "src/parser_bison.y"
                                                { (yyval.val) = OP_LT; }
#line 14662 "src/parser_bison.c"
    break;

  case 942: /* relational_op: ">"  */
#line 5174 "src/parser_bison.y"
                                                { (yyval.val) = OP_GT; }
#line 14668 "src/parser_bison.c"
    break;

  case 943: /* relational_op: ">="  */
#line 5175 "src/parser_bison.y"
                                                { (yyval.val) = OP_GTE; }
#line 14674 "src/parser_bison.c"
    break;

  case 944: /* relational_op: "<="  */
#line 5176 "src/parser_bison.y"
                                                { (yyval.val) = OP_LTE; }
#line 14680 "src/parser_bison.c"
    break;

  case 945: /* relational_op: "!"  */
#line 5177 "src/parser_bison.y"
                                                { (yyval.val) = OP_NEG; }
#line 14686 "src/parser_bison.c"
    break;

  case 946: /* verdict_expr: "accept"  */
#line 5181 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NF_ACCEPT, NULL);
			}
#line 14694 "src/parser_bison.c"
    break;

  case 947: /* verdict_expr: "drop"  */
#line 5185 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NF_DROP, NULL);
			}
#line 14702 "src/parser_bison.c"
    break;

  case 948: /* verdict_expr: "continue"  */
#line 5189 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_CONTINUE, NULL);
			}
#line 14710 "src/parser_bison.c"
    break;

  case 949: /* verdict_expr: "jump" chain_expr  */
#line 5193 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_JUMP, (yyvsp[0].expr));
			}
#line 14718 "src/parser_bison.c"
    break;

  case 950: /* verdict_expr: "goto" chain_expr  */
#line 5197 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_GOTO, (yyvsp[0].expr));
			}
#line 14726 "src/parser_bison.c"
    break;

  case 951: /* verdict_expr: "return"  */
#line 5201 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_RETURN, NULL);
			}
#line 14734 "src/parser_bison.c"
    break;

  case 953: /* chain_expr: identifier  */
#line 5208 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &string_type,
							 BYTEORDER_HOST_ENDIAN,
							 strlen((yyvsp[0].string)) * BITS_PER_BYTE,
							 (yyvsp[0].string));
				free_const((yyvsp[0].string));
			}
#line 14746 "src/parser_bison.c"
    break;

  case 954: /* meta_expr: "meta" meta_key close_scope_meta  */
#line 5218 "src/parser_bison.y"
                        {
				(yyval.expr) = meta_expr_alloc(&(yyloc), (yyvsp[-1].val));
			}
#line 14754 "src/parser_bison.c"
    break;

  case 955: /* meta_expr: meta_key_unqualified  */
#line 5222 "src/parser_bison.y"
                        {
				(yyval.expr) = meta_expr_alloc(&(yyloc), (yyvsp[0].val));
			}
#line 14762 "src/parser_bison.c"
    break;

  case 956: /* meta_expr: "meta" "string" close_scope_meta  */
#line 5226 "src/parser_bison.y"
                        {
				struct error_record *erec;
				unsigned int key;

				erec = meta_key_parse(&(yyloc), (yyvsp[-1].string), &key);
				free_const((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				(yyval.expr) = meta_expr_alloc(&(yyloc), key);
			}
#line 14780 "src/parser_bison.c"
    break;

  case 959: /* meta_key_qualified: "length"  */
#line 5245 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_LEN; }
#line 14786 "src/parser_bison.c"
    break;

  case 960: /* meta_key_qualified: "protocol"  */
#line 5246 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PROTOCOL; }
#line 14792 "src/parser_bison.c"
    break;

  case 961: /* meta_key_qualified: "priority"  */
#line 5247 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PRIORITY; }
#line 14798 "src/parser_bison.c"
    break;

  case 962: /* meta_key_qualified: "random"  */
#line 5248 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PRANDOM; }
#line 14804 "src/parser_bison.c"
    break;

  case 963: /* meta_key_qualified: "secmark" close_scope_secmark  */
#line 5249 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_META_SECMARK; }
#line 14810 "src/parser_bison.c"
    break;

  case 964: /* meta_key_unqualified: "mark"  */
#line 5252 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_MARK; }
#line 14816 "src/parser_bison.c"
    break;

  case 965: /* meta_key_unqualified: "iif"  */
#line 5253 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIF; }
#line 14822 "src/parser_bison.c"
    break;

  case 966: /* meta_key_unqualified: "iifname"  */
#line 5254 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFNAME; }
#line 14828 "src/parser_bison.c"
    break;

  case 967: /* meta_key_unqualified: "iiftype"  */
#line 5255 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFTYPE; }
#line 14834 "src/parser_bison.c"
    break;

  case 968: /* meta_key_unqualified: "oif"  */
#line 5256 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIF; }
#line 14840 "src/parser_bison.c"
    break;

  case 969: /* meta_key_unqualified: "oifname"  */
#line 5257 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFNAME; }
#line 14846 "src/parser_bison.c"
    break;

  case 970: /* meta_key_unqualified: "oiftype"  */
#line 5258 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFTYPE; }
#line 14852 "src/parser_bison.c"
    break;

  case 971: /* meta_key_unqualified: "skuid"  */
#line 5259 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_SKUID; }
#line 14858 "src/parser_bison.c"
    break;

  case 972: /* meta_key_unqualified: "skgid"  */
#line 5260 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_SKGID; }
#line 14864 "src/parser_bison.c"
    break;

  case 973: /* meta_key_unqualified: "nftrace"  */
#line 5261 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_NFTRACE; }
#line 14870 "src/parser_bison.c"
    break;

  case 974: /* meta_key_unqualified: "rtclassid"  */
#line 5262 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_RTCLASSID; }
#line 14876 "src/parser_bison.c"
    break;

  case 975: /* meta_key_unqualified: "ibriport"  */
#line 5263 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_IIFNAME; }
#line 14882 "src/parser_bison.c"
    break;

  case 976: /* meta_key_unqualified: "obriport"  */
#line 5264 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_OIFNAME; }
#line 14888 "src/parser_bison.c"
    break;

  case 977: /* meta_key_unqualified: "ibrname"  */
#line 5265 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_IIFNAME; }
#line 14894 "src/parser_bison.c"
    break;

  case 978: /* meta_key_unqualified: "obrname"  */
#line 5266 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_OIFNAME; }
#line 14900 "src/parser_bison.c"
    break;

  case 979: /* meta_key_unqualified: "pkttype"  */
#line 5267 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PKTTYPE; }
#line 14906 "src/parser_bison.c"
    break;

  case 980: /* meta_key_unqualified: "cpu"  */
#line 5268 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_CPU; }
#line 14912 "src/parser_bison.c"
    break;

  case 981: /* meta_key_unqualified: "iifgroup"  */
#line 5269 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFGROUP; }
#line 14918 "src/parser_bison.c"
    break;

  case 982: /* meta_key_unqualified: "oifgroup"  */
#line 5270 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFGROUP; }
#line 14924 "src/parser_bison.c"
    break;

  case 983: /* meta_key_unqualified: "cgroup"  */
#line 5271 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_CGROUP; }
#line 14930 "src/parser_bison.c"
    break;

  case 984: /* meta_key_unqualified: "ipsec" close_scope_ipsec  */
#line 5272 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_META_SECPATH; }
#line 14936 "src/parser_bison.c"
    break;

  case 985: /* meta_key_unqualified: "time"  */
#line 5273 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_NS; }
#line 14942 "src/parser_bison.c"
    break;

  case 986: /* meta_key_unqualified: "day"  */
#line 5274 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_DAY; }
#line 14948 "src/parser_bison.c"
    break;

  case 987: /* meta_key_unqualified: "hour"  */
#line 5275 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_HOUR; }
#line 14954 "src/parser_bison.c"
    break;

  case 988: /* meta_stmt: "meta" meta_key "set" stmt_expr close_scope_meta  */
#line 5279 "src/parser_bison.y"
                        {
				switch ((yyvsp[-3].val)) {
				case NFT_META_SECMARK:
					switch ((yyvsp[-1].expr)->etype) {
					case EXPR_CT:
						(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-1].expr));
						break;
					default:
						(yyval.stmt) = objref_stmt_alloc(&(yyloc));
						(yyval.stmt)->objref.type = NFT_OBJECT_SECMARK;
						(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
						break;
					}
					break;
				default:
					(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-1].expr));
					break;
				}
			}
#line 14978 "src/parser_bison.c"
    break;

  case 989: /* meta_stmt: meta_key_unqualified "set" stmt_expr  */
#line 5299 "src/parser_bison.y"
                        {
				(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[0].expr));
			}
#line 14986 "src/parser_bison.c"
    break;

  case 990: /* meta_stmt: "meta" "string" "set" stmt_expr close_scope_meta  */
#line 5303 "src/parser_bison.y"
                        {
				struct error_record *erec;
				unsigned int key;

				erec = meta_key_parse(&(yyloc), (yyvsp[-3].string), &key);
				free_const((yyvsp[-3].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}

				(yyval.stmt) = meta_stmt_alloc(&(yyloc), key, (yyvsp[-1].expr));
			}
#line 15005 "src/parser_bison.c"
    break;

  case 991: /* meta_stmt: "notrack"  */
#line 5318 "src/parser_bison.y"
                        {
				(yyval.stmt) = notrack_stmt_alloc(&(yyloc));
			}
#line 15013 "src/parser_bison.c"
    break;

  case 992: /* meta_stmt: "flow" "offload" "@" string close_scope_at  */
#line 5322 "src/parser_bison.y"
                        {
				(yyval.stmt) = flow_offload_stmt_alloc(&(yyloc), (yyvsp[-1].string));
			}
#line 15021 "src/parser_bison.c"
    break;

  case 993: /* meta_stmt: "flow" "add" "@" string close_scope_at  */
#line 5326 "src/parser_bison.y"
                        {
				(yyval.stmt) = flow_offload_stmt_alloc(&(yyloc), (yyvsp[-1].string));
			}
#line 15029 "src/parser_bison.c"
    break;

  case 994: /* socket_expr: "socket" socket_key close_scope_socket  */
#line 5332 "src/parser_bison.y"
                        {
				(yyval.expr) = socket_expr_alloc(&(yyloc), (yyvsp[-1].val), 0);
			}
#line 15037 "src/parser_bison.c"
    break;

  case 995: /* socket_expr: "socket" "cgroupv2" "level" "number" close_scope_socket  */
#line 5336 "src/parser_bison.y"
                        {
				(yyval.expr) = socket_expr_alloc(&(yyloc), NFT_SOCKET_CGROUPV2, (yyvsp[-1].val));
			}
#line 15045 "src/parser_bison.c"
    break;

  case 996: /* socket_key: "transparent"  */
#line 5341 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_TRANSPARENT; }
#line 15051 "src/parser_bison.c"
    break;

  case 997: /* socket_key: "mark"  */
#line 5342 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_MARK; }
#line 15057 "src/parser_bison.c"
    break;

  case 998: /* socket_key: "wildcard"  */
#line 5343 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_WILDCARD; }
#line 15063 "src/parser_bison.c"
    break;

  case 999: /* offset_opt: %empty  */
#line 5346 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 15069 "src/parser_bison.c"
    break;

  case 1000: /* offset_opt: "offset" "number"  */
#line 5347 "src/parser_bison.y"
                                                { (yyval.val) = (yyvsp[0].val); }
#line 15075 "src/parser_bison.c"
    break;

  case 1001: /* numgen_type: "inc"  */
#line 5350 "src/parser_bison.y"
                                                { (yyval.val) = NFT_NG_INCREMENTAL; }
#line 15081 "src/parser_bison.c"
    break;

  case 1002: /* numgen_type: "random"  */
#line 5351 "src/parser_bison.y"
                                                { (yyval.val) = NFT_NG_RANDOM; }
#line 15087 "src/parser_bison.c"
    break;

  case 1003: /* numgen_expr: "numgen" numgen_type "mod" "number" offset_opt close_scope_numgen  */
#line 5355 "src/parser_bison.y"
                        {
				(yyval.expr) = numgen_expr_alloc(&(yyloc), (yyvsp[-4].val), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 15095 "src/parser_bison.c"
    break;

  case 1004: /* xfrm_spnum: "spnum" "number"  */
#line 5360 "src/parser_bison.y"
                                            { (yyval.val) = (yyvsp[0].val); }
#line 15101 "src/parser_bison.c"
    break;

  case 1005: /* xfrm_spnum: %empty  */
#line 5361 "src/parser_bison.y"
                                            { (yyval.val) = 0; }
#line 15107 "src/parser_bison.c"
    break;

  case 1006: /* xfrm_dir: "in"  */
#line 5364 "src/parser_bison.y"
                                        { (yyval.val) = XFRM_POLICY_IN; }
#line 15113 "src/parser_bison.c"
    break;

  case 1007: /* xfrm_dir: "out"  */
#line 5365 "src/parser_bison.y"
                                        { (yyval.val) = XFRM_POLICY_OUT; }
#line 15119 "src/parser_bison.c"
    break;

  case 1008: /* xfrm_state_key: "spi"  */
#line 5368 "src/parser_bison.y"
                                    { (yyval.val) = NFT_XFRM_KEY_SPI; }
#line 15125 "src/parser_bison.c"
    break;

  case 1009: /* xfrm_state_key: "reqid"  */
#line 5369 "src/parser_bison.y"
                                      { (yyval.val) = NFT_XFRM_KEY_REQID; }
#line 15131 "src/parser_bison.c"
    break;

  case 1010: /* xfrm_state_proto_key: "daddr"  */
#line 5372 "src/parser_bison.y"
                                                { (yyval.val) = NFT_XFRM_KEY_DADDR_IP4; }
#line 15137 "src/parser_bison.c"
    break;

  case 1011: /* xfrm_state_proto_key: "saddr"  */
#line 5373 "src/parser_bison.y"
                                                { (yyval.val) = NFT_XFRM_KEY_SADDR_IP4; }
#line 15143 "src/parser_bison.c"
    break;

  case 1012: /* xfrm_expr: "ipsec" xfrm_dir xfrm_spnum xfrm_state_key close_scope_ipsec  */
#line 5377 "src/parser_bison.y"
                        {
				if ((yyvsp[-2].val) > 255) {
					erec_queue(error(&(yylsp[-2]), "value too large"), state->msgs);
					YYERROR;
				}
				(yyval.expr) = xfrm_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 15155 "src/parser_bison.c"
    break;

  case 1013: /* xfrm_expr: "ipsec" xfrm_dir xfrm_spnum nf_key_proto xfrm_state_proto_key close_scope_ipsec  */
#line 5385 "src/parser_bison.y"
                        {
				enum nft_xfrm_keys xfrmk = (yyvsp[-1].val);

				switch ((yyvsp[-2].val)) {
				case NFPROTO_IPV4:
					break;
				case NFPROTO_IPV6:
					if ((yyvsp[-1].val) == NFT_XFRM_KEY_SADDR_IP4)
						xfrmk = NFT_XFRM_KEY_SADDR_IP6;
					else if ((yyvsp[-1].val) == NFT_XFRM_KEY_DADDR_IP4)
						xfrmk = NFT_XFRM_KEY_DADDR_IP6;
					break;
				default:
					YYERROR;
					break;
				}

				if ((yyvsp[-3].val) > 255) {
					erec_queue(error(&(yylsp[-3]), "value too large"), state->msgs);
					YYERROR;
				}

				(yyval.expr) = xfrm_expr_alloc(&(yyloc), (yyvsp[-4].val), (yyvsp[-3].val), xfrmk);
			}
#line 15184 "src/parser_bison.c"
    break;

  case 1014: /* hash_expr: "jhash" expr "mod" "number" "seed" "number" offset_opt close_scope_hash  */
#line 5412 "src/parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-4].val), true, (yyvsp[-2].val), (yyvsp[-1].val), NFT_HASH_JENKINS);
				(yyval.expr)->hash.expr = (yyvsp[-6].expr);
			}
#line 15193 "src/parser_bison.c"
    break;

  case 1015: /* hash_expr: "jhash" expr "mod" "number" offset_opt close_scope_hash  */
#line 5417 "src/parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-2].val), false, 0, (yyvsp[-1].val), NFT_HASH_JENKINS);
				(yyval.expr)->hash.expr = (yyvsp[-4].expr);
			}
#line 15202 "src/parser_bison.c"
    break;

  case 1016: /* hash_expr: "symhash" "mod" "number" offset_opt close_scope_hash  */
#line 5422 "src/parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-2].val), false, 0, (yyvsp[-1].val), NFT_HASH_SYM);
			}
#line 15210 "src/parser_bison.c"
    break;

  case 1017: /* nf_key_proto: "ip" close_scope_ip  */
#line 5427 "src/parser_bison.y"
                                                       { (yyval.val) = NFPROTO_IPV4; }
#line 15216 "src/parser_bison.c"
    break;

  case 1018: /* nf_key_proto: "ip6" close_scope_ip6  */
#line 5428 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV6; }
#line 15222 "src/parser_bison.c"
    break;

  case 1019: /* rt_expr: "rt" rt_key close_scope_rt  */
#line 5432 "src/parser_bison.y"
                        {
				(yyval.expr) = rt_expr_alloc(&(yyloc), (yyvsp[-1].val), true);
			}
#line 15230 "src/parser_bison.c"
    break;

  case 1020: /* rt_expr: "rt" nf_key_proto rt_key close_scope_rt  */
#line 5436 "src/parser_bison.y"
                        {
				enum nft_rt_keys rtk = (yyvsp[-1].val);

				switch ((yyvsp[-2].val)) {
				case NFPROTO_IPV4:
					break;
				case NFPROTO_IPV6:
					if ((yyvsp[-1].val) == NFT_RT_NEXTHOP4)
						rtk = NFT_RT_NEXTHOP6;
					break;
				default:
					YYERROR;
					break;
				}

				(yyval.expr) = rt_expr_alloc(&(yyloc), rtk, false);
			}
#line 15252 "src/parser_bison.c"
    break;

  case 1021: /* rt_key: "classid"  */
#line 5455 "src/parser_bison.y"
                                                { (yyval.val) = NFT_RT_CLASSID; }
#line 15258 "src/parser_bison.c"
    break;

  case 1022: /* rt_key: "nexthop"  */
#line 5456 "src/parser_bison.y"
                                                { (yyval.val) = NFT_RT_NEXTHOP4; }
#line 15264 "src/parser_bison.c"
    break;

  case 1023: /* rt_key: "mtu"  */
#line 5457 "src/parser_bison.y"
                                                { (yyval.val) = NFT_RT_TCPMSS; }
#line 15270 "src/parser_bison.c"
    break;

  case 1024: /* rt_key: "ipsec" close_scope_ipsec  */
#line 5458 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_RT_XFRM; }
#line 15276 "src/parser_bison.c"
    break;

  case 1025: /* ct_expr: "ct" ct_key close_scope_ct  */
#line 5462 "src/parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), -1);
			}
#line 15284 "src/parser_bison.c"
    break;

  case 1026: /* ct_expr: "ct" ct_dir ct_key_dir close_scope_ct  */
#line 5466 "src/parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[-2].val));
			}
#line 15292 "src/parser_bison.c"
    break;

  case 1027: /* ct_expr: "ct" ct_dir ct_key_proto_field close_scope_ct  */
#line 5470 "src/parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[-2].val));
			}
#line 15300 "src/parser_bison.c"
    break;

  case 1028: /* ct_dir: "original"  */
#line 5475 "src/parser_bison.y"
                                                { (yyval.val) = IP_CT_DIR_ORIGINAL; }
#line 15306 "src/parser_bison.c"
    break;

  case 1029: /* ct_dir: "reply"  */
#line 5476 "src/parser_bison.y"
                                                { (yyval.val) = IP_CT_DIR_REPLY; }
#line 15312 "src/parser_bison.c"
    break;

  case 1030: /* ct_key: "l3proto"  */
#line 5479 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_L3PROTOCOL; }
#line 15318 "src/parser_bison.c"
    break;

  case 1031: /* ct_key: "protocol"  */
#line 5480 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTOCOL; }
#line 15324 "src/parser_bison.c"
    break;

  case 1032: /* ct_key: "mark"  */
#line 5481 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_MARK; }
#line 15330 "src/parser_bison.c"
    break;

  case 1033: /* ct_key: "state"  */
#line 5482 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_STATE; }
#line 15336 "src/parser_bison.c"
    break;

  case 1034: /* ct_key: "direction"  */
#line 5483 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_DIRECTION; }
#line 15342 "src/parser_bison.c"
    break;

  case 1035: /* ct_key: "status"  */
#line 5484 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_STATUS; }
#line 15348 "src/parser_bison.c"
    break;

  case 1036: /* ct_key: "expiration"  */
#line 5485 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_EXPIRATION; }
#line 15354 "src/parser_bison.c"
    break;

  case 1037: /* ct_key: "helper"  */
#line 5486 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_HELPER; }
#line 15360 "src/parser_bison.c"
    break;

  case 1038: /* ct_key: "saddr"  */
#line 5487 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_SRC; }
#line 15366 "src/parser_bison.c"
    break;

  case 1039: /* ct_key: "daddr"  */
#line 5488 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_DST; }
#line 15372 "src/parser_bison.c"
    break;

  case 1040: /* ct_key: "proto-src"  */
#line 5489 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_SRC; }
#line 15378 "src/parser_bison.c"
    break;

  case 1041: /* ct_key: "proto-dst"  */
#line 5490 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_DST; }
#line 15384 "src/parser_bison.c"
    break;

  case 1042: /* ct_key: "label"  */
#line 5491 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_LABELS; }
#line 15390 "src/parser_bison.c"
    break;

  case 1043: /* ct_key: "event"  */
#line 5492 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_EVENTMASK; }
#line 15396 "src/parser_bison.c"
    break;

  case 1044: /* ct_key: "secmark" close_scope_secmark  */
#line 5493 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_CT_SECMARK; }
#line 15402 "src/parser_bison.c"
    break;

  case 1045: /* ct_key: "id"  */
#line 5494 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_ID; }
#line 15408 "src/parser_bison.c"
    break;

  case 1047: /* ct_key_dir: "saddr"  */
#line 5498 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_SRC; }
#line 15414 "src/parser_bison.c"
    break;

  case 1048: /* ct_key_dir: "daddr"  */
#line 5499 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_DST; }
#line 15420 "src/parser_bison.c"
    break;

  case 1049: /* ct_key_dir: "l3proto"  */
#line 5500 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_L3PROTOCOL; }
#line 15426 "src/parser_bison.c"
    break;

  case 1050: /* ct_key_dir: "protocol"  */
#line 5501 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTOCOL; }
#line 15432 "src/parser_bison.c"
    break;

  case 1051: /* ct_key_dir: "proto-src"  */
#line 5502 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_SRC; }
#line 15438 "src/parser_bison.c"
    break;

  case 1052: /* ct_key_dir: "proto-dst"  */
#line 5503 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_DST; }
#line 15444 "src/parser_bison.c"
    break;

  case 1054: /* ct_key_proto_field: "ip" "saddr" close_scope_ip  */
#line 5507 "src/parser_bison.y"
                                                               { (yyval.val) = NFT_CT_SRC_IP; }
#line 15450 "src/parser_bison.c"
    break;

  case 1055: /* ct_key_proto_field: "ip" "daddr" close_scope_ip  */
#line 5508 "src/parser_bison.y"
                                                               { (yyval.val) = NFT_CT_DST_IP; }
#line 15456 "src/parser_bison.c"
    break;

  case 1056: /* ct_key_proto_field: "ip6" "saddr" close_scope_ip6  */
#line 5509 "src/parser_bison.y"
                                                                { (yyval.val) = NFT_CT_SRC_IP6; }
#line 15462 "src/parser_bison.c"
    break;

  case 1057: /* ct_key_proto_field: "ip6" "daddr" close_scope_ip6  */
#line 5510 "src/parser_bison.y"
                                                                { (yyval.val) = NFT_CT_DST_IP6; }
#line 15468 "src/parser_bison.c"
    break;

  case 1058: /* ct_key_dir_optional: "bytes"  */
#line 5513 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_BYTES; }
#line 15474 "src/parser_bison.c"
    break;

  case 1059: /* ct_key_dir_optional: "packets"  */
#line 5514 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PKTS; }
#line 15480 "src/parser_bison.c"
    break;

  case 1060: /* ct_key_dir_optional: "avgpkt"  */
#line 5515 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_AVGPKT; }
#line 15486 "src/parser_bison.c"
    break;

  case 1061: /* ct_key_dir_optional: "zone"  */
#line 5516 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_ZONE; }
#line 15492 "src/parser_bison.c"
    break;

  case 1064: /* list_stmt_expr: symbol_stmt_expr "comma" symbol_stmt_expr  */
#line 5524 "src/parser_bison.y"
                        {
				(yyval.expr) = list_expr_alloc(&(yyloc));
				compound_expr_add((yyval.expr), (yyvsp[-2].expr));
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 15502 "src/parser_bison.c"
    break;

  case 1065: /* list_stmt_expr: list_stmt_expr "comma" symbol_stmt_expr  */
#line 5530 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->location = (yyloc);
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 15512 "src/parser_bison.c"
    break;

  case 1066: /* ct_stmt: "ct" ct_key "set" stmt_expr close_scope_ct  */
#line 5538 "src/parser_bison.y"
                        {
				switch ((yyvsp[-3].val)) {
				case NFT_CT_HELPER:
					(yyval.stmt) = objref_stmt_alloc(&(yyloc));
					(yyval.stmt)->objref.type = NFT_OBJECT_CT_HELPER;
					(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
					break;
				default:
					(yyval.stmt) = ct_stmt_alloc(&(yyloc), (yyvsp[-3].val), -1, (yyvsp[-1].expr));
					break;
				}
			}
#line 15529 "src/parser_bison.c"
    break;

  case 1067: /* ct_stmt: "ct" ct_dir ct_key_dir_optional "set" stmt_expr close_scope_ct  */
#line 5551 "src/parser_bison.y"
                        {
				(yyval.stmt) = ct_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].val), (yyvsp[-1].expr));
			}
#line 15537 "src/parser_bison.c"
    break;

  case 1068: /* payload_stmt: payload_expr "set" stmt_expr  */
#line 5557 "src/parser_bison.y"
                        {
				if ((yyvsp[-2].expr)->etype == EXPR_EXTHDR)
					(yyval.stmt) = exthdr_stmt_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
				else
					(yyval.stmt) = payload_stmt_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 15548 "src/parser_bison.c"
    break;

  case 1091: /* payload_raw_len: "number"  */
#line 5590 "src/parser_bison.y"
                        {
				if ((yyvsp[0].val) > NFT_MAX_EXPR_LEN_BITS) {
					erec_queue(error(&(yylsp[0]), "raw payload length %u exceeds upper limit of %u",
							 (yyvsp[0].val), NFT_MAX_EXPR_LEN_BITS),
						 state->msgs);
					YYERROR;
				}

				if ((yyvsp[0].val) == 0) {
					erec_queue(error(&(yylsp[0]), "raw payload length cannot be 0"), state->msgs);
					YYERROR;
				}

				(yyval.val) = (yyvsp[0].val);
			}
#line 15568 "src/parser_bison.c"
    break;

  case 1092: /* payload_raw_expr: "@" payload_base_spec "comma" "number" "comma" payload_raw_len close_scope_at  */
#line 5608 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), NULL, 0);
				payload_init_raw((yyval.expr), (yyvsp[-5].val), (yyvsp[-3].val), (yyvsp[-1].val));
				(yyval.expr)->byteorder		= BYTEORDER_BIG_ENDIAN;
				(yyval.expr)->payload.is_raw	= true;
			}
#line 15579 "src/parser_bison.c"
    break;

  case 1093: /* payload_base_spec: "ll"  */
#line 5616 "src/parser_bison.y"
                                                { (yyval.val) = PROTO_BASE_LL_HDR; }
#line 15585 "src/parser_bison.c"
    break;

  case 1094: /* payload_base_spec: "nh"  */
#line 5617 "src/parser_bison.y"
                                                { (yyval.val) = PROTO_BASE_NETWORK_HDR; }
#line 15591 "src/parser_bison.c"
    break;

  case 1095: /* payload_base_spec: "th" close_scope_th  */
#line 5618 "src/parser_bison.y"
                                                                { (yyval.val) = PROTO_BASE_TRANSPORT_HDR; }
#line 15597 "src/parser_bison.c"
    break;

  case 1096: /* payload_base_spec: "string"  */
#line 5620 "src/parser_bison.y"
                        {
				if (!strcmp((yyvsp[0].string), "ih")) {
					(yyval.val) = PROTO_BASE_INNER_HDR;
				} else {
					erec_queue(error(&(yylsp[0]), "unknown raw payload base"), state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 15612 "src/parser_bison.c"
    break;

  case 1097: /* eth_hdr_expr: "ether" eth_hdr_field close_scope_eth  */
#line 5633 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_eth, (yyvsp[-1].val));
			}
#line 15620 "src/parser_bison.c"
    break;

  case 1098: /* eth_hdr_field: "saddr"  */
#line 5638 "src/parser_bison.y"
                                                { (yyval.val) = ETHHDR_SADDR; }
#line 15626 "src/parser_bison.c"
    break;

  case 1099: /* eth_hdr_field: "daddr"  */
#line 5639 "src/parser_bison.y"
                                                { (yyval.val) = ETHHDR_DADDR; }
#line 15632 "src/parser_bison.c"
    break;

  case 1100: /* eth_hdr_field: "type" close_scope_type  */
#line 5640 "src/parser_bison.y"
                                                                        { (yyval.val) = ETHHDR_TYPE; }
#line 15638 "src/parser_bison.c"
    break;

  case 1101: /* vlan_hdr_expr: "vlan" vlan_hdr_field close_scope_vlan  */
#line 5644 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_vlan, (yyvsp[-1].val));
			}
#line 15646 "src/parser_bison.c"
    break;

  case 1102: /* vlan_hdr_field: "id"  */
#line 5649 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_VID; }
#line 15652 "src/parser_bison.c"
    break;

  case 1103: /* vlan_hdr_field: "cfi"  */
#line 5650 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_CFI; }
#line 15658 "src/parser_bison.c"
    break;

  case 1104: /* vlan_hdr_field: "dei"  */
#line 5651 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_DEI; }
#line 15664 "src/parser_bison.c"
    break;

  case 1105: /* vlan_hdr_field: "pcp"  */
#line 5652 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_PCP; }
#line 15670 "src/parser_bison.c"
    break;

  case 1106: /* vlan_hdr_field: "type" close_scope_type  */
#line 5653 "src/parser_bison.y"
                                                                        { (yyval.val) = VLANHDR_TYPE; }
#line 15676 "src/parser_bison.c"
    break;

  case 1107: /* arp_hdr_expr: "arp" arp_hdr_field close_scope_arp  */
#line 5657 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_arp, (yyvsp[-1].val));
			}
#line 15684 "src/parser_bison.c"
    break;

  case 1108: /* arp_hdr_field: "htype"  */
#line 5662 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_HRD; }
#line 15690 "src/parser_bison.c"
    break;

  case 1109: /* arp_hdr_field: "ptype"  */
#line 5663 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_PRO; }
#line 15696 "src/parser_bison.c"
    break;

  case 1110: /* arp_hdr_field: "hlen"  */
#line 5664 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_HLN; }
#line 15702 "src/parser_bison.c"
    break;

  case 1111: /* arp_hdr_field: "plen"  */
#line 5665 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_PLN; }
#line 15708 "src/parser_bison.c"
    break;

  case 1112: /* arp_hdr_field: "operation"  */
#line 5666 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_OP; }
#line 15714 "src/parser_bison.c"
    break;

  case 1113: /* arp_hdr_field: "saddr" "ether" close_scope_eth  */
#line 5667 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_SADDR_ETHER; }
#line 15720 "src/parser_bison.c"
    break;

  case 1114: /* arp_hdr_field: "daddr" "ether" close_scope_eth  */
#line 5668 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_DADDR_ETHER; }
#line 15726 "src/parser_bison.c"
    break;

  case 1115: /* arp_hdr_field: "saddr" "ip" close_scope_ip  */
#line 5669 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_SADDR_IP; }
#line 15732 "src/parser_bison.c"
    break;

  case 1116: /* arp_hdr_field: "daddr" "ip" close_scope_ip  */
#line 5670 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_DADDR_IP; }
#line 15738 "src/parser_bison.c"
    break;

  case 1117: /* ip_hdr_expr: "ip" ip_hdr_field close_scope_ip  */
#line 5674 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ip, (yyvsp[-1].val));
			}
#line 15746 "src/parser_bison.c"
    break;

  case 1118: /* ip_hdr_expr: "ip" "option" ip_option_type ip_option_field close_scope_ip  */
#line 5678 "src/parser_bison.y"
                        {
				(yyval.expr) = ipopt_expr_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[-1].val));
				if (!(yyval.expr)) {
					erec_queue(error(&(yylsp[-4]), "unknown ip option type/field"), state->msgs);
					YYERROR;
				}
			}
#line 15758 "src/parser_bison.c"
    break;

  case 1119: /* ip_hdr_expr: "ip" "option" ip_option_type close_scope_ip  */
#line 5686 "src/parser_bison.y"
                        {
				(yyval.expr) = ipopt_expr_alloc(&(yyloc), (yyvsp[-1].val), IPOPT_FIELD_TYPE);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 15767 "src/parser_bison.c"
    break;

  case 1120: /* ip_hdr_field: "version"  */
#line 5692 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_VERSION; }
#line 15773 "src/parser_bison.c"
    break;

  case 1121: /* ip_hdr_field: "hdrlength"  */
#line 5693 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_HDRLENGTH; }
#line 15779 "src/parser_bison.c"
    break;

  case 1122: /* ip_hdr_field: "dscp"  */
#line 5694 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_DSCP; }
#line 15785 "src/parser_bison.c"
    break;

  case 1123: /* ip_hdr_field: "ecn"  */
#line 5695 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_ECN; }
#line 15791 "src/parser_bison.c"
    break;

  case 1124: /* ip_hdr_field: "length"  */
#line 5696 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_LENGTH; }
#line 15797 "src/parser_bison.c"
    break;

  case 1125: /* ip_hdr_field: "id"  */
#line 5697 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_ID; }
#line 15803 "src/parser_bison.c"
    break;

  case 1126: /* ip_hdr_field: "frag-off"  */
#line 5698 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_FRAG_OFF; }
#line 15809 "src/parser_bison.c"
    break;

  case 1127: /* ip_hdr_field: "ttl"  */
#line 5699 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_TTL; }
#line 15815 "src/parser_bison.c"
    break;

  case 1128: /* ip_hdr_field: "protocol"  */
#line 5700 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_PROTOCOL; }
#line 15821 "src/parser_bison.c"
    break;

  case 1129: /* ip_hdr_field: "checksum"  */
#line 5701 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_CHECKSUM; }
#line 15827 "src/parser_bison.c"
    break;

  case 1130: /* ip_hdr_field: "saddr"  */
#line 5702 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_SADDR; }
#line 15833 "src/parser_bison.c"
    break;

  case 1131: /* ip_hdr_field: "daddr"  */
#line 5703 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_DADDR; }
#line 15839 "src/parser_bison.c"
    break;

  case 1132: /* ip_option_type: "lsrr"  */
#line 5706 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_LSRR; }
#line 15845 "src/parser_bison.c"
    break;

  case 1133: /* ip_option_type: "rr"  */
#line 5707 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_RR; }
#line 15851 "src/parser_bison.c"
    break;

  case 1134: /* ip_option_type: "ssrr"  */
#line 5708 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_SSRR; }
#line 15857 "src/parser_bison.c"
    break;

  case 1135: /* ip_option_type: "ra"  */
#line 5709 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_RA; }
#line 15863 "src/parser_bison.c"
    break;

  case 1136: /* ip_option_field: "type" close_scope_type  */
#line 5712 "src/parser_bison.y"
                                                                        { (yyval.val) = IPOPT_FIELD_TYPE; }
#line 15869 "src/parser_bison.c"
    break;

  case 1137: /* ip_option_field: "length"  */
#line 5713 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_LENGTH; }
#line 15875 "src/parser_bison.c"
    break;

  case 1138: /* ip_option_field: "value"  */
#line 5714 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_VALUE; }
#line 15881 "src/parser_bison.c"
    break;

  case 1139: /* ip_option_field: "ptr"  */
#line 5715 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_PTR; }
#line 15887 "src/parser_bison.c"
    break;

  case 1140: /* ip_option_field: "addr"  */
#line 5716 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_ADDR_0; }
#line 15893 "src/parser_bison.c"
    break;

  case 1141: /* icmp_hdr_expr: "icmp" icmp_hdr_field close_scope_icmp  */
#line 5720 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_icmp, (yyvsp[-1].val));
			}
#line 15901 "src/parser_bison.c"
    break;

  case 1142: /* icmp_hdr_field: "type" close_scope_type  */
#line 5725 "src/parser_bison.y"
                                                                        { (yyval.val) = ICMPHDR_TYPE; }
#line 15907 "src/parser_bison.c"
    break;

  case 1143: /* icmp_hdr_field: "code"  */
#line 5726 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_CODE; }
#line 15913 "src/parser_bison.c"
    break;

  case 1144: /* icmp_hdr_field: "checksum"  */
#line 5727 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_CHECKSUM; }
#line 15919 "src/parser_bison.c"
    break;

  case 1145: /* icmp_hdr_field: "id"  */
#line 5728 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_ID; }
#line 15925 "src/parser_bison.c"
    break;

  case 1146: /* icmp_hdr_field: "seq"  */
#line 5729 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_SEQ; }
#line 15931 "src/parser_bison.c"
    break;

  case 1147: /* icmp_hdr_field: "gateway"  */
#line 5730 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_GATEWAY; }
#line 15937 "src/parser_bison.c"
    break;

  case 1148: /* icmp_hdr_field: "mtu"  */
#line 5731 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_MTU; }
#line 15943 "src/parser_bison.c"
    break;

  case 1149: /* igmp_hdr_expr: "igmp" igmp_hdr_field close_scope_igmp  */
#line 5735 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_igmp, (yyvsp[-1].val));
			}
#line 15951 "src/parser_bison.c"
    break;

  case 1150: /* igmp_hdr_field: "type" close_scope_type  */
#line 5740 "src/parser_bison.y"
                                                                        { (yyval.val) = IGMPHDR_TYPE; }
#line 15957 "src/parser_bison.c"
    break;

  case 1151: /* igmp_hdr_field: "checksum"  */
#line 5741 "src/parser_bison.y"
                                                { (yyval.val) = IGMPHDR_CHECKSUM; }
#line 15963 "src/parser_bison.c"
    break;

  case 1152: /* igmp_hdr_field: "mrt"  */
#line 5742 "src/parser_bison.y"
                                                { (yyval.val) = IGMPHDR_MRT; }
#line 15969 "src/parser_bison.c"
    break;

  case 1153: /* igmp_hdr_field: "group"  */
#line 5743 "src/parser_bison.y"
                                                { (yyval.val) = IGMPHDR_GROUP; }
#line 15975 "src/parser_bison.c"
    break;

  case 1154: /* ip6_hdr_expr: "ip6" ip6_hdr_field close_scope_ip6  */
#line 5747 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ip6, (yyvsp[-1].val));
			}
#line 15983 "src/parser_bison.c"
    break;

  case 1155: /* ip6_hdr_field: "version"  */
#line 5752 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_VERSION; }
#line 15989 "src/parser_bison.c"
    break;

  case 1156: /* ip6_hdr_field: "dscp"  */
#line 5753 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_DSCP; }
#line 15995 "src/parser_bison.c"
    break;

  case 1157: /* ip6_hdr_field: "ecn"  */
#line 5754 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_ECN; }
#line 16001 "src/parser_bison.c"
    break;

  case 1158: /* ip6_hdr_field: "flowlabel"  */
#line 5755 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_FLOWLABEL; }
#line 16007 "src/parser_bison.c"
    break;

  case 1159: /* ip6_hdr_field: "length"  */
#line 5756 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_LENGTH; }
#line 16013 "src/parser_bison.c"
    break;

  case 1160: /* ip6_hdr_field: "nexthdr"  */
#line 5757 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_NEXTHDR; }
#line 16019 "src/parser_bison.c"
    break;

  case 1161: /* ip6_hdr_field: "hoplimit"  */
#line 5758 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_HOPLIMIT; }
#line 16025 "src/parser_bison.c"
    break;

  case 1162: /* ip6_hdr_field: "saddr"  */
#line 5759 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_SADDR; }
#line 16031 "src/parser_bison.c"
    break;

  case 1163: /* ip6_hdr_field: "daddr"  */
#line 5760 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_DADDR; }
#line 16037 "src/parser_bison.c"
    break;

  case 1164: /* icmp6_hdr_expr: "icmpv6" icmp6_hdr_field close_scope_icmp  */
#line 5763 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_icmp6, (yyvsp[-1].val));
			}
#line 16045 "src/parser_bison.c"
    break;

  case 1165: /* icmp6_hdr_field: "type" close_scope_type  */
#line 5768 "src/parser_bison.y"
                                                                        { (yyval.val) = ICMP6HDR_TYPE; }
#line 16051 "src/parser_bison.c"
    break;

  case 1166: /* icmp6_hdr_field: "code"  */
#line 5769 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_CODE; }
#line 16057 "src/parser_bison.c"
    break;

  case 1167: /* icmp6_hdr_field: "checksum"  */
#line 5770 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_CHECKSUM; }
#line 16063 "src/parser_bison.c"
    break;

  case 1168: /* icmp6_hdr_field: "param-problem"  */
#line 5771 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_PPTR; }
#line 16069 "src/parser_bison.c"
    break;

  case 1169: /* icmp6_hdr_field: "mtu"  */
#line 5772 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_MTU; }
#line 16075 "src/parser_bison.c"
    break;

  case 1170: /* icmp6_hdr_field: "id"  */
#line 5773 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_ID; }
#line 16081 "src/parser_bison.c"
    break;

  case 1171: /* icmp6_hdr_field: "seq"  */
#line 5774 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_SEQ; }
#line 16087 "src/parser_bison.c"
    break;

  case 1172: /* icmp6_hdr_field: "max-delay"  */
#line 5775 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_MAXDELAY; }
#line 16093 "src/parser_bison.c"
    break;

  case 1173: /* icmp6_hdr_field: "taddr"  */
#line 5776 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_TADDR; }
#line 16099 "src/parser_bison.c"
    break;

  case 1174: /* icmp6_hdr_field: "daddr"  */
#line 5777 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_DADDR; }
#line 16105 "src/parser_bison.c"
    break;

  case 1175: /* auth_hdr_expr: "ah" auth_hdr_field close_scope_ah  */
#line 5781 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ah, (yyvsp[-1].val));
			}
#line 16113 "src/parser_bison.c"
    break;

  case 1176: /* auth_hdr_field: "nexthdr"  */
#line 5786 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_NEXTHDR; }
#line 16119 "src/parser_bison.c"
    break;

  case 1177: /* auth_hdr_field: "hdrlength"  */
#line 5787 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_HDRLENGTH; }
#line 16125 "src/parser_bison.c"
    break;

  case 1178: /* auth_hdr_field: "reserved"  */
#line 5788 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_RESERVED; }
#line 16131 "src/parser_bison.c"
    break;

  case 1179: /* auth_hdr_field: "spi"  */
#line 5789 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_SPI; }
#line 16137 "src/parser_bison.c"
    break;

  case 1180: /* auth_hdr_field: "seq"  */
#line 5790 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_SEQUENCE; }
#line 16143 "src/parser_bison.c"
    break;

  case 1181: /* esp_hdr_expr: "esp" esp_hdr_field close_scope_esp  */
#line 5794 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_esp, (yyvsp[-1].val));
			}
#line 16151 "src/parser_bison.c"
    break;

  case 1182: /* esp_hdr_field: "spi"  */
#line 5799 "src/parser_bison.y"
                                                { (yyval.val) = ESPHDR_SPI; }
#line 16157 "src/parser_bison.c"
    break;

  case 1183: /* esp_hdr_field: "seq"  */
#line 5800 "src/parser_bison.y"
                                                { (yyval.val) = ESPHDR_SEQUENCE; }
#line 16163 "src/parser_bison.c"
    break;

  case 1184: /* comp_hdr_expr: "comp" comp_hdr_field close_scope_comp  */
#line 5804 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_comp, (yyvsp[-1].val));
			}
#line 16171 "src/parser_bison.c"
    break;

  case 1185: /* comp_hdr_field: "nexthdr"  */
#line 5809 "src/parser_bison.y"
                                                { (yyval.val) = COMPHDR_NEXTHDR; }
#line 16177 "src/parser_bison.c"
    break;

  case 1186: /* comp_hdr_field: "flags"  */
#line 5810 "src/parser_bison.y"
                                                { (yyval.val) = COMPHDR_FLAGS; }
#line 16183 "src/parser_bison.c"
    break;

  case 1187: /* comp_hdr_field: "cpi"  */
#line 5811 "src/parser_bison.y"
                                                { (yyval.val) = COMPHDR_CPI; }
#line 16189 "src/parser_bison.c"
    break;

  case 1188: /* udp_hdr_expr: "udp" udp_hdr_field close_scope_udp  */
#line 5815 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_udp, (yyvsp[-1].val));
			}
#line 16197 "src/parser_bison.c"
    break;

  case 1189: /* udp_hdr_field: "sport"  */
#line 5820 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_SPORT; }
#line 16203 "src/parser_bison.c"
    break;

  case 1190: /* udp_hdr_field: "dport"  */
#line 5821 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_DPORT; }
#line 16209 "src/parser_bison.c"
    break;

  case 1191: /* udp_hdr_field: "length"  */
#line 5822 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_LENGTH; }
#line 16215 "src/parser_bison.c"
    break;

  case 1192: /* udp_hdr_field: "checksum"  */
#line 5823 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_CHECKSUM; }
#line 16221 "src/parser_bison.c"
    break;

  case 1193: /* udplite_hdr_expr: "udplite" udplite_hdr_field close_scope_udplite  */
#line 5827 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_udplite, (yyvsp[-1].val));
			}
#line 16229 "src/parser_bison.c"
    break;

  case 1194: /* udplite_hdr_field: "sport"  */
#line 5832 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_SPORT; }
#line 16235 "src/parser_bison.c"
    break;

  case 1195: /* udplite_hdr_field: "dport"  */
#line 5833 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_DPORT; }
#line 16241 "src/parser_bison.c"
    break;

  case 1196: /* udplite_hdr_field: "csumcov"  */
#line 5834 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_LENGTH; }
#line 16247 "src/parser_bison.c"
    break;

  case 1197: /* udplite_hdr_field: "checksum"  */
#line 5835 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_CHECKSUM; }
#line 16253 "src/parser_bison.c"
    break;

  case 1198: /* tcp_hdr_expr: "tcp" tcp_hdr_field  */
#line 5839 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_tcp, (yyvsp[0].val));
			}
#line 16261 "src/parser_bison.c"
    break;

  case 1199: /* tcp_hdr_expr: "tcp" "option" tcp_hdr_option_type  */
#line 5843 "src/parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[0].val), TCPOPT_COMMON_KIND);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 16270 "src/parser_bison.c"
    break;

  case 1200: /* tcp_hdr_expr: "tcp" "option" tcp_hdr_option_kind_and_field  */
#line 5848 "src/parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[0].tcp_kind_field).kind, (yyvsp[0].tcp_kind_field).field);
				if ((yyval.expr) == NULL) {
					erec_queue(error(&(yylsp[-2]), "Could not find a tcp option template"), state->msgs);
					YYERROR;
				}
			}
#line 16282 "src/parser_bison.c"
    break;

  case 1201: /* tcp_hdr_expr: "tcp" "option" "@" close_scope_at tcp_hdr_option_type "comma" "number" "comma" payload_raw_len  */
#line 5856 "src/parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[-4].val), 0);
				tcpopt_init_raw((yyval.expr), (yyvsp[-4].val), (yyvsp[-2].val), (yyvsp[0].val), 0);
			}
#line 16291 "src/parser_bison.c"
    break;

  case 1221: /* vxlan_hdr_expr: "vxlan" vxlan_hdr_field  */
#line 5888 "src/parser_bison.y"
                        {
				struct expr *expr;

				expr = payload_expr_alloc(&(yyloc), &proto_vxlan, (yyvsp[0].val));
				expr->payload.inner_desc = &proto_vxlan;
				(yyval.expr) = expr;
			}
#line 16303 "src/parser_bison.c"
    break;

  case 1222: /* vxlan_hdr_expr: "vxlan" inner_expr  */
#line 5896 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->location = (yyloc);
				(yyval.expr)->payload.inner_desc = &proto_vxlan;
			}
#line 16313 "src/parser_bison.c"
    break;

  case 1223: /* vxlan_hdr_field: "vni"  */
#line 5903 "src/parser_bison.y"
                                                        { (yyval.val) = VXLANHDR_VNI; }
#line 16319 "src/parser_bison.c"
    break;

  case 1224: /* vxlan_hdr_field: "flags"  */
#line 5904 "src/parser_bison.y"
                                                        { (yyval.val) = VXLANHDR_FLAGS; }
#line 16325 "src/parser_bison.c"
    break;

  case 1225: /* geneve_hdr_expr: "geneve" geneve_hdr_field  */
#line 5908 "src/parser_bison.y"
                        {
				struct expr *expr;

				expr = payload_expr_alloc(&(yyloc), &proto_geneve, (yyvsp[0].val));
				expr->payload.inner_desc = &proto_geneve;
				(yyval.expr) = expr;
			}
#line 16337 "src/parser_bison.c"
    break;

  case 1226: /* geneve_hdr_expr: "geneve" inner_expr  */
#line 5916 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->location = (yyloc);
				(yyval.expr)->payload.inner_desc = &proto_geneve;
			}
#line 16347 "src/parser_bison.c"
    break;

  case 1227: /* geneve_hdr_field: "vni"  */
#line 5923 "src/parser_bison.y"
                                                        { (yyval.val) = GNVHDR_VNI; }
#line 16353 "src/parser_bison.c"
    break;

  case 1228: /* geneve_hdr_field: "type"  */
#line 5924 "src/parser_bison.y"
                                                        { (yyval.val) = GNVHDR_TYPE; }
#line 16359 "src/parser_bison.c"
    break;

  case 1229: /* gre_hdr_expr: "gre" gre_hdr_field close_scope_gre  */
#line 5928 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_gre, (yyvsp[-1].val));
			}
#line 16367 "src/parser_bison.c"
    break;

  case 1230: /* gre_hdr_expr: "gre" close_scope_gre inner_inet_expr  */
#line 5932 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->payload.inner_desc = &proto_gre;
			}
#line 16376 "src/parser_bison.c"
    break;

  case 1231: /* gre_hdr_field: "version"  */
#line 5938 "src/parser_bison.y"
                                                        { (yyval.val) = GREHDR_VERSION;	}
#line 16382 "src/parser_bison.c"
    break;

  case 1232: /* gre_hdr_field: "flags"  */
#line 5939 "src/parser_bison.y"
                                                        { (yyval.val) = GREHDR_FLAGS; }
#line 16388 "src/parser_bison.c"
    break;

  case 1233: /* gre_hdr_field: "protocol"  */
#line 5940 "src/parser_bison.y"
                                                        { (yyval.val) = GREHDR_PROTOCOL; }
#line 16394 "src/parser_bison.c"
    break;

  case 1234: /* gretap_hdr_expr: "gretap" close_scope_gre inner_expr  */
#line 5944 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->payload.inner_desc = &proto_gretap;
			}
#line 16403 "src/parser_bison.c"
    break;

  case 1235: /* optstrip_stmt: "reset" "tcp" "option" tcp_hdr_option_type close_scope_tcp  */
#line 5951 "src/parser_bison.y"
                        {
				(yyval.stmt) = optstrip_stmt_alloc(&(yyloc), tcpopt_expr_alloc(&(yyloc),
										(yyvsp[-1].val), TCPOPT_COMMON_KIND));
			}
#line 16412 "src/parser_bison.c"
    break;

  case 1236: /* tcp_hdr_field: "sport"  */
#line 5957 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_SPORT; }
#line 16418 "src/parser_bison.c"
    break;

  case 1237: /* tcp_hdr_field: "dport"  */
#line 5958 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_DPORT; }
#line 16424 "src/parser_bison.c"
    break;

  case 1238: /* tcp_hdr_field: "seq"  */
#line 5959 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_SEQ; }
#line 16430 "src/parser_bison.c"
    break;

  case 1239: /* tcp_hdr_field: "ackseq"  */
#line 5960 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_ACKSEQ; }
#line 16436 "src/parser_bison.c"
    break;

  case 1240: /* tcp_hdr_field: "doff"  */
#line 5961 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_DOFF; }
#line 16442 "src/parser_bison.c"
    break;

  case 1241: /* tcp_hdr_field: "reserved"  */
#line 5962 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_RESERVED; }
#line 16448 "src/parser_bison.c"
    break;

  case 1242: /* tcp_hdr_field: "flags"  */
#line 5963 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_FLAGS; }
#line 16454 "src/parser_bison.c"
    break;

  case 1243: /* tcp_hdr_field: "window"  */
#line 5964 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_WINDOW; }
#line 16460 "src/parser_bison.c"
    break;

  case 1244: /* tcp_hdr_field: "checksum"  */
#line 5965 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_CHECKSUM; }
#line 16466 "src/parser_bison.c"
    break;

  case 1245: /* tcp_hdr_field: "urgptr"  */
#line 5966 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_URGPTR; }
#line 16472 "src/parser_bison.c"
    break;

  case 1246: /* tcp_hdr_option_kind_and_field: "mss" tcpopt_field_maxseg  */
#line 5970 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_MAXSEG, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16481 "src/parser_bison.c"
    break;

  case 1247: /* tcp_hdr_option_kind_and_field: tcp_hdr_option_sack tcpopt_field_sack  */
#line 5975 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = (yyvsp[-1].val), .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16490 "src/parser_bison.c"
    break;

  case 1248: /* tcp_hdr_option_kind_and_field: "window" tcpopt_field_window  */
#line 5980 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_WINDOW, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16499 "src/parser_bison.c"
    break;

  case 1249: /* tcp_hdr_option_kind_and_field: "timestamp" tcpopt_field_tsopt  */
#line 5985 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_TIMESTAMP, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16508 "src/parser_bison.c"
    break;

  case 1250: /* tcp_hdr_option_kind_and_field: tcp_hdr_option_type "length"  */
#line 5990 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = (yyvsp[-1].val), .field = TCPOPT_COMMON_LENGTH };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16517 "src/parser_bison.c"
    break;

  case 1251: /* tcp_hdr_option_kind_and_field: "mptcp" tcpopt_field_mptcp  */
#line 5995 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_MPTCP, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16526 "src/parser_bison.c"
    break;

  case 1252: /* tcp_hdr_option_sack: "sack"  */
#line 6001 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK; }
#line 16532 "src/parser_bison.c"
    break;

  case 1253: /* tcp_hdr_option_sack: "sack0"  */
#line 6002 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK; }
#line 16538 "src/parser_bison.c"
    break;

  case 1254: /* tcp_hdr_option_sack: "sack1"  */
#line 6003 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK1; }
#line 16544 "src/parser_bison.c"
    break;

  case 1255: /* tcp_hdr_option_sack: "sack2"  */
#line 6004 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK2; }
#line 16550 "src/parser_bison.c"
    break;

  case 1256: /* tcp_hdr_option_sack: "sack3"  */
#line 6005 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK3; }
#line 16556 "src/parser_bison.c"
    break;

  case 1257: /* tcp_hdr_option_type: "echo"  */
#line 6008 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_ECHO; }
#line 16562 "src/parser_bison.c"
    break;

  case 1258: /* tcp_hdr_option_type: "eol"  */
#line 6009 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_EOL; }
#line 16568 "src/parser_bison.c"
    break;

  case 1259: /* tcp_hdr_option_type: "fastopen"  */
#line 6010 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_FASTOPEN; }
#line 16574 "src/parser_bison.c"
    break;

  case 1260: /* tcp_hdr_option_type: "md5sig"  */
#line 6011 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MD5SIG; }
#line 16580 "src/parser_bison.c"
    break;

  case 1261: /* tcp_hdr_option_type: "mptcp"  */
#line 6012 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MPTCP; }
#line 16586 "src/parser_bison.c"
    break;

  case 1262: /* tcp_hdr_option_type: "mss"  */
#line 6013 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MAXSEG; }
#line 16592 "src/parser_bison.c"
    break;

  case 1263: /* tcp_hdr_option_type: "nop"  */
#line 6014 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_NOP; }
#line 16598 "src/parser_bison.c"
    break;

  case 1264: /* tcp_hdr_option_type: "sack-permitted"  */
#line 6015 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_SACK_PERMITTED; }
#line 16604 "src/parser_bison.c"
    break;

  case 1265: /* tcp_hdr_option_type: "timestamp"  */
#line 6016 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_TIMESTAMP; }
#line 16610 "src/parser_bison.c"
    break;

  case 1266: /* tcp_hdr_option_type: "window"  */
#line 6017 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_WINDOW; }
#line 16616 "src/parser_bison.c"
    break;

  case 1267: /* tcp_hdr_option_type: tcp_hdr_option_sack  */
#line 6018 "src/parser_bison.y"
                                                        { (yyval.val) = (yyvsp[0].val); }
#line 16622 "src/parser_bison.c"
    break;

  case 1268: /* tcp_hdr_option_type: "number"  */
#line 6019 "src/parser_bison.y"
                                                        {
				if ((yyvsp[0].val) > 255) {
					erec_queue(error(&(yylsp[0]), "value too large"), state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[0].val);
			}
#line 16634 "src/parser_bison.c"
    break;

  case 1269: /* tcpopt_field_sack: "left"  */
#line 6028 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_SACK_LEFT; }
#line 16640 "src/parser_bison.c"
    break;

  case 1270: /* tcpopt_field_sack: "right"  */
#line 6029 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_SACK_RIGHT; }
#line 16646 "src/parser_bison.c"
    break;

  case 1271: /* tcpopt_field_window: "count"  */
#line 6032 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_WINDOW_COUNT; }
#line 16652 "src/parser_bison.c"
    break;

  case 1272: /* tcpopt_field_tsopt: "tsval"  */
#line 6035 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_TS_TSVAL; }
#line 16658 "src/parser_bison.c"
    break;

  case 1273: /* tcpopt_field_tsopt: "tsecr"  */
#line 6036 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_TS_TSECR; }
#line 16664 "src/parser_bison.c"
    break;

  case 1274: /* tcpopt_field_maxseg: "size"  */
#line 6039 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_MAXSEG_SIZE; }
#line 16670 "src/parser_bison.c"
    break;

  case 1275: /* tcpopt_field_mptcp: "subtype"  */
#line 6042 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_MPTCP_SUBTYPE; }
#line 16676 "src/parser_bison.c"
    break;

  case 1276: /* dccp_hdr_expr: "dccp" dccp_hdr_field close_scope_dccp  */
#line 6046 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_dccp, (yyvsp[-1].val));
			}
#line 16684 "src/parser_bison.c"
    break;

  case 1277: /* dccp_hdr_expr: "dccp" "option" "number" close_scope_dccp  */
#line 6050 "src/parser_bison.y"
                        {
				if ((yyvsp[-1].val) > DCCPOPT_TYPE_MAX) {
					erec_queue(error(&(yylsp[-3]), "value too large"),
						   state->msgs);
					YYERROR;
				}
				(yyval.expr) = dccpopt_expr_alloc(&(yyloc), (yyvsp[-1].val));
			}
#line 16697 "src/parser_bison.c"
    break;

  case 1278: /* dccp_hdr_field: "sport"  */
#line 6060 "src/parser_bison.y"
                                                { (yyval.val) = DCCPHDR_SPORT; }
#line 16703 "src/parser_bison.c"
    break;

  case 1279: /* dccp_hdr_field: "dport"  */
#line 6061 "src/parser_bison.y"
                                                { (yyval.val) = DCCPHDR_DPORT; }
#line 16709 "src/parser_bison.c"
    break;

  case 1280: /* dccp_hdr_field: "type" close_scope_type  */
#line 6062 "src/parser_bison.y"
                                                                        { (yyval.val) = DCCPHDR_TYPE; }
#line 16715 "src/parser_bison.c"
    break;

  case 1281: /* sctp_chunk_type: "data"  */
#line 6065 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_DATA; }
#line 16721 "src/parser_bison.c"
    break;

  case 1282: /* sctp_chunk_type: "init"  */
#line 6066 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_INIT; }
#line 16727 "src/parser_bison.c"
    break;

  case 1283: /* sctp_chunk_type: "init-ack"  */
#line 6067 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_INIT_ACK; }
#line 16733 "src/parser_bison.c"
    break;

  case 1284: /* sctp_chunk_type: "sack"  */
#line 6068 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SACK; }
#line 16739 "src/parser_bison.c"
    break;

  case 1285: /* sctp_chunk_type: "heartbeat"  */
#line 6069 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_HEARTBEAT; }
#line 16745 "src/parser_bison.c"
    break;

  case 1286: /* sctp_chunk_type: "heartbeat-ack"  */
#line 6070 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_HEARTBEAT_ACK; }
#line 16751 "src/parser_bison.c"
    break;

  case 1287: /* sctp_chunk_type: "abort"  */
#line 6071 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ABORT; }
#line 16757 "src/parser_bison.c"
    break;

  case 1288: /* sctp_chunk_type: "shutdown"  */
#line 6072 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN; }
#line 16763 "src/parser_bison.c"
    break;

  case 1289: /* sctp_chunk_type: "shutdown-ack"  */
#line 6073 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN_ACK; }
#line 16769 "src/parser_bison.c"
    break;

  case 1290: /* sctp_chunk_type: "error"  */
#line 6074 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ERROR; }
#line 16775 "src/parser_bison.c"
    break;

  case 1291: /* sctp_chunk_type: "cookie-echo"  */
#line 6075 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_COOKIE_ECHO; }
#line 16781 "src/parser_bison.c"
    break;

  case 1292: /* sctp_chunk_type: "cookie-ack"  */
#line 6076 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_COOKIE_ACK; }
#line 16787 "src/parser_bison.c"
    break;

  case 1293: /* sctp_chunk_type: "ecne"  */
#line 6077 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ECNE; }
#line 16793 "src/parser_bison.c"
    break;

  case 1294: /* sctp_chunk_type: "cwr"  */
#line 6078 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_CWR; }
#line 16799 "src/parser_bison.c"
    break;

  case 1295: /* sctp_chunk_type: "shutdown-complete"  */
#line 6079 "src/parser_bison.y"
                                                  { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN_COMPLETE; }
#line 16805 "src/parser_bison.c"
    break;

  case 1296: /* sctp_chunk_type: "asconf-ack"  */
#line 6080 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ASCONF_ACK; }
#line 16811 "src/parser_bison.c"
    break;

  case 1297: /* sctp_chunk_type: "forward-tsn"  */
#line 6081 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_FORWARD_TSN; }
#line 16817 "src/parser_bison.c"
    break;

  case 1298: /* sctp_chunk_type: "asconf"  */
#line 6082 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ASCONF; }
#line 16823 "src/parser_bison.c"
    break;

  case 1299: /* sctp_chunk_common_field: "type" close_scope_type  */
#line 6085 "src/parser_bison.y"
                                                                { (yyval.val) = SCTP_CHUNK_COMMON_TYPE; }
#line 16829 "src/parser_bison.c"
    break;

  case 1300: /* sctp_chunk_common_field: "flags"  */
#line 6086 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_COMMON_FLAGS; }
#line 16835 "src/parser_bison.c"
    break;

  case 1301: /* sctp_chunk_common_field: "length"  */
#line 6087 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_COMMON_LENGTH; }
#line 16841 "src/parser_bison.c"
    break;

  case 1302: /* sctp_chunk_data_field: "tsn"  */
#line 6090 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_TSN; }
#line 16847 "src/parser_bison.c"
    break;

  case 1303: /* sctp_chunk_data_field: "stream"  */
#line 6091 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_STREAM; }
#line 16853 "src/parser_bison.c"
    break;

  case 1304: /* sctp_chunk_data_field: "ssn"  */
#line 6092 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_SSN; }
#line 16859 "src/parser_bison.c"
    break;

  case 1305: /* sctp_chunk_data_field: "ppid"  */
#line 6093 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_PPID; }
#line 16865 "src/parser_bison.c"
    break;

  case 1306: /* sctp_chunk_init_field: "init-tag"  */
#line 6096 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_TAG; }
#line 16871 "src/parser_bison.c"
    break;

  case 1307: /* sctp_chunk_init_field: "a-rwnd"  */
#line 6097 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_RWND; }
#line 16877 "src/parser_bison.c"
    break;

  case 1308: /* sctp_chunk_init_field: "num-outbound-streams"  */
#line 6098 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_OSTREAMS; }
#line 16883 "src/parser_bison.c"
    break;

  case 1309: /* sctp_chunk_init_field: "num-inbound-streams"  */
#line 6099 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_ISTREAMS; }
#line 16889 "src/parser_bison.c"
    break;

  case 1310: /* sctp_chunk_init_field: "initial-tsn"  */
#line 6100 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_TSN; }
#line 16895 "src/parser_bison.c"
    break;

  case 1311: /* sctp_chunk_sack_field: "cum-tsn-ack"  */
#line 6103 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_CTSN_ACK; }
#line 16901 "src/parser_bison.c"
    break;

  case 1312: /* sctp_chunk_sack_field: "a-rwnd"  */
#line 6104 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_RWND; }
#line 16907 "src/parser_bison.c"
    break;

  case 1313: /* sctp_chunk_sack_field: "num-gap-ack-blocks"  */
#line 6105 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_GACK_BLOCKS; }
#line 16913 "src/parser_bison.c"
    break;

  case 1314: /* sctp_chunk_sack_field: "num-dup-tsns"  */
#line 6106 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_DUP_TSNS; }
#line 16919 "src/parser_bison.c"
    break;

  case 1315: /* sctp_chunk_alloc: sctp_chunk_type  */
#line 6110 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), (yyvsp[0].val), SCTP_CHUNK_COMMON_TYPE);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 16928 "src/parser_bison.c"
    break;

  case 1316: /* sctp_chunk_alloc: sctp_chunk_type sctp_chunk_common_field  */
#line 6115 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[0].val));
			}
#line 16936 "src/parser_bison.c"
    break;

  case 1317: /* sctp_chunk_alloc: "data" sctp_chunk_data_field  */
#line 6119 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_DATA, (yyvsp[0].val));
			}
#line 16944 "src/parser_bison.c"
    break;

  case 1318: /* sctp_chunk_alloc: "init" sctp_chunk_init_field  */
#line 6123 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_INIT, (yyvsp[0].val));
			}
#line 16952 "src/parser_bison.c"
    break;

  case 1319: /* sctp_chunk_alloc: "init-ack" sctp_chunk_init_field  */
#line 6127 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_INIT_ACK, (yyvsp[0].val));
			}
#line 16960 "src/parser_bison.c"
    break;

  case 1320: /* sctp_chunk_alloc: "sack" sctp_chunk_sack_field  */
#line 6131 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_SACK, (yyvsp[0].val));
			}
#line 16968 "src/parser_bison.c"
    break;

  case 1321: /* sctp_chunk_alloc: "shutdown" "cum-tsn-ack"  */
#line 6135 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_SHUTDOWN,
							   SCTP_CHUNK_SHUTDOWN_CTSN_ACK);
			}
#line 16977 "src/parser_bison.c"
    break;

  case 1322: /* sctp_chunk_alloc: "ecne" "lowest-tsn"  */
#line 6140 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ECNE,
							   SCTP_CHUNK_ECNE_CWR_MIN_TSN);
			}
#line 16986 "src/parser_bison.c"
    break;

  case 1323: /* sctp_chunk_alloc: "cwr" "lowest-tsn"  */
#line 6145 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_CWR,
							   SCTP_CHUNK_ECNE_CWR_MIN_TSN);
			}
#line 16995 "src/parser_bison.c"
    break;

  case 1324: /* sctp_chunk_alloc: "asconf-ack" "seqno"  */
#line 6150 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ASCONF_ACK,
							   SCTP_CHUNK_ASCONF_SEQNO);
			}
#line 17004 "src/parser_bison.c"
    break;

  case 1325: /* sctp_chunk_alloc: "forward-tsn" "new-cum-tsn"  */
#line 6155 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_FORWARD_TSN,
							   SCTP_CHUNK_FORWARD_TSN_NCTSN);
			}
#line 17013 "src/parser_bison.c"
    break;

  case 1326: /* sctp_chunk_alloc: "asconf" "seqno"  */
#line 6160 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ASCONF,
							   SCTP_CHUNK_ASCONF_SEQNO);
			}
#line 17022 "src/parser_bison.c"
    break;

  case 1327: /* sctp_hdr_expr: "sctp" sctp_hdr_field close_scope_sctp  */
#line 6167 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_sctp, (yyvsp[-1].val));
			}
#line 17030 "src/parser_bison.c"
    break;

  case 1328: /* sctp_hdr_expr: "sctp" "chunk" sctp_chunk_alloc close_scope_sctp_chunk close_scope_sctp  */
#line 6171 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 17038 "src/parser_bison.c"
    break;

  case 1329: /* sctp_hdr_field: "sport"  */
#line 6176 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_SPORT; }
#line 17044 "src/parser_bison.c"
    break;

  case 1330: /* sctp_hdr_field: "dport"  */
#line 6177 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_DPORT; }
#line 17050 "src/parser_bison.c"
    break;

  case 1331: /* sctp_hdr_field: "vtag"  */
#line 6178 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_VTAG; }
#line 17056 "src/parser_bison.c"
    break;

  case 1332: /* sctp_hdr_field: "checksum"  */
#line 6179 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_CHECKSUM; }
#line 17062 "src/parser_bison.c"
    break;

  case 1333: /* th_hdr_expr: "th" th_hdr_field close_scope_th  */
#line 6183 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_th, (yyvsp[-1].val));
				if ((yyval.expr))
					(yyval.expr)->payload.is_raw = true;
			}
#line 17072 "src/parser_bison.c"
    break;

  case 1334: /* th_hdr_field: "sport"  */
#line 6190 "src/parser_bison.y"
                                                { (yyval.val) = THDR_SPORT; }
#line 17078 "src/parser_bison.c"
    break;

  case 1335: /* th_hdr_field: "dport"  */
#line 6191 "src/parser_bison.y"
                                                { (yyval.val) = THDR_DPORT; }
#line 17084 "src/parser_bison.c"
    break;

  case 1344: /* hbh_hdr_expr: "hbh" hbh_hdr_field close_scope_hbh  */
#line 6205 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_hbh, (yyvsp[-1].val));
			}
#line 17092 "src/parser_bison.c"
    break;

  case 1345: /* hbh_hdr_field: "nexthdr"  */
#line 6210 "src/parser_bison.y"
                                                { (yyval.val) = HBHHDR_NEXTHDR; }
#line 17098 "src/parser_bison.c"
    break;

  case 1346: /* hbh_hdr_field: "hdrlength"  */
#line 6211 "src/parser_bison.y"
                                                { (yyval.val) = HBHHDR_HDRLENGTH; }
#line 17104 "src/parser_bison.c"
    break;

  case 1347: /* rt_hdr_expr: "rt" rt_hdr_field close_scope_rt  */
#line 6215 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt, (yyvsp[-1].val));
			}
#line 17112 "src/parser_bison.c"
    break;

  case 1348: /* rt_hdr_field: "nexthdr"  */
#line 6220 "src/parser_bison.y"
                                                { (yyval.val) = RTHDR_NEXTHDR; }
#line 17118 "src/parser_bison.c"
    break;

  case 1349: /* rt_hdr_field: "hdrlength"  */
#line 6221 "src/parser_bison.y"
                                                { (yyval.val) = RTHDR_HDRLENGTH; }
#line 17124 "src/parser_bison.c"
    break;

  case 1350: /* rt_hdr_field: "type" close_scope_type  */
#line 6222 "src/parser_bison.y"
                                                                        { (yyval.val) = RTHDR_TYPE; }
#line 17130 "src/parser_bison.c"
    break;

  case 1351: /* rt_hdr_field: "seg-left"  */
#line 6223 "src/parser_bison.y"
                                                { (yyval.val) = RTHDR_SEG_LEFT; }
#line 17136 "src/parser_bison.c"
    break;

  case 1352: /* rt0_hdr_expr: "rt0" rt0_hdr_field close_scope_rt  */
#line 6227 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt0, (yyvsp[-1].val));
			}
#line 17144 "src/parser_bison.c"
    break;

  case 1353: /* rt0_hdr_field: "addr" '[' "number" ']'  */
#line 6233 "src/parser_bison.y"
                        {
				(yyval.val) = RT0HDR_ADDR_1 + (yyvsp[-1].val) - 1;
			}
#line 17152 "src/parser_bison.c"
    break;

  case 1354: /* rt2_hdr_expr: "rt2" rt2_hdr_field close_scope_rt  */
#line 6239 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt2, (yyvsp[-1].val));
			}
#line 17160 "src/parser_bison.c"
    break;

  case 1355: /* rt2_hdr_field: "addr"  */
#line 6244 "src/parser_bison.y"
                                                { (yyval.val) = RT2HDR_ADDR; }
#line 17166 "src/parser_bison.c"
    break;

  case 1356: /* rt4_hdr_expr: "srh" rt4_hdr_field close_scope_rt  */
#line 6248 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt4, (yyvsp[-1].val));
			}
#line 17174 "src/parser_bison.c"
    break;

  case 1357: /* rt4_hdr_field: "last-entry"  */
#line 6253 "src/parser_bison.y"
                                                { (yyval.val) = RT4HDR_LASTENT; }
#line 17180 "src/parser_bison.c"
    break;

  case 1358: /* rt4_hdr_field: "flags"  */
#line 6254 "src/parser_bison.y"
                                                { (yyval.val) = RT4HDR_FLAGS; }
#line 17186 "src/parser_bison.c"
    break;

  case 1359: /* rt4_hdr_field: "tag"  */
#line 6255 "src/parser_bison.y"
                                                { (yyval.val) = RT4HDR_TAG; }
#line 17192 "src/parser_bison.c"
    break;

  case 1360: /* rt4_hdr_field: "sid" '[' "number" ']'  */
#line 6257 "src/parser_bison.y"
                        {
				(yyval.val) = RT4HDR_SID_1 + (yyvsp[-1].val) - 1;
			}
#line 17200 "src/parser_bison.c"
    break;

  case 1361: /* frag_hdr_expr: "frag" frag_hdr_field close_scope_frag  */
#line 6263 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_frag, (yyvsp[-1].val));
			}
#line 17208 "src/parser_bison.c"
    break;

  case 1362: /* frag_hdr_field: "nexthdr"  */
#line 6268 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_NEXTHDR; }
#line 17214 "src/parser_bison.c"
    break;

  case 1363: /* frag_hdr_field: "reserved"  */
#line 6269 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_RESERVED; }
#line 17220 "src/parser_bison.c"
    break;

  case 1364: /* frag_hdr_field: "frag-off"  */
#line 6270 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_FRAG_OFF; }
#line 17226 "src/parser_bison.c"
    break;

  case 1365: /* frag_hdr_field: "reserved2"  */
#line 6271 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_RESERVED2; }
#line 17232 "src/parser_bison.c"
    break;

  case 1366: /* frag_hdr_field: "more-fragments"  */
#line 6272 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_MFRAGS; }
#line 17238 "src/parser_bison.c"
    break;

  case 1367: /* frag_hdr_field: "id"  */
#line 6273 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_ID; }
#line 17244 "src/parser_bison.c"
    break;

  case 1368: /* dst_hdr_expr: "dst" dst_hdr_field close_scope_dst  */
#line 6277 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_dst, (yyvsp[-1].val));
			}
#line 17252 "src/parser_bison.c"
    break;

  case 1369: /* dst_hdr_field: "nexthdr"  */
#line 6282 "src/parser_bison.y"
                                                { (yyval.val) = DSTHDR_NEXTHDR; }
#line 17258 "src/parser_bison.c"
    break;

  case 1370: /* dst_hdr_field: "hdrlength"  */
#line 6283 "src/parser_bison.y"
                                                { (yyval.val) = DSTHDR_HDRLENGTH; }
#line 17264 "src/parser_bison.c"
    break;

  case 1371: /* mh_hdr_expr: "mh" mh_hdr_field close_scope_mh  */
#line 6287 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_mh, (yyvsp[-1].val));
			}
#line 17272 "src/parser_bison.c"
    break;

  case 1372: /* mh_hdr_field: "nexthdr"  */
#line 6292 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_NEXTHDR; }
#line 17278 "src/parser_bison.c"
    break;

  case 1373: /* mh_hdr_field: "hdrlength"  */
#line 6293 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_HDRLENGTH; }
#line 17284 "src/parser_bison.c"
    break;

  case 1374: /* mh_hdr_field: "type" close_scope_type  */
#line 6294 "src/parser_bison.y"
                                                                        { (yyval.val) = MHHDR_TYPE; }
#line 17290 "src/parser_bison.c"
    break;

  case 1375: /* mh_hdr_field: "reserved"  */
#line 6295 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_RESERVED; }
#line 17296 "src/parser_bison.c"
    break;

  case 1376: /* mh_hdr_field: "checksum"  */
#line 6296 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_CHECKSUM; }
#line 17302 "src/parser_bison.c"
    break;

  case 1377: /* exthdr_exists_expr: "exthdr" exthdr_key  */
#line 6300 "src/parser_bison.y"
                        {
				const struct exthdr_desc *desc;

				desc = exthdr_find_proto((yyvsp[0].val));

				/* Assume that NEXTHDR template is always
				 * the first one in list of templates.
				 */
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), desc, 1);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 17318 "src/parser_bison.c"
    break;

  case 1378: /* exthdr_key: "hbh" close_scope_hbh  */
#line 6313 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_HOPOPTS; }
#line 17324 "src/parser_bison.c"
    break;

  case 1379: /* exthdr_key: "rt" close_scope_rt  */
#line 6314 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_ROUTING; }
#line 17330 "src/parser_bison.c"
    break;

  case 1380: /* exthdr_key: "frag" close_scope_frag  */
#line 6315 "src/parser_bison.y"
                                                                { (yyval.val) = IPPROTO_FRAGMENT; }
#line 17336 "src/parser_bison.c"
    break;

  case 1381: /* exthdr_key: "dst" close_scope_dst  */
#line 6316 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_DSTOPTS; }
#line 17342 "src/parser_bison.c"
    break;

  case 1382: /* exthdr_key: "mh" close_scope_mh  */
#line 6317 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_MH; }
#line 17348 "src/parser_bison.c"
    break;


#line 17352 "src/parser_bison.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, nft, scanner, state, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= TOKEN_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == TOKEN_EOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, nft, scanner, state);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, nft, scanner, state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, nft, scanner, state, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, nft, scanner, state);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, nft, scanner, state);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 6320 "src/parser_bison.y"

