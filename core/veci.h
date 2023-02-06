
#define MB_LEN_MAX    5
#define SHRT_MIN    (-32768)
#define SHRT_MAX      32767
#define USHRT_MAX     0xffff
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX       2147483647
#define UINT_MAX      0xffffffff
#define LONG_MIN    (-2147483647L - 1)
#define LONG_MAX      2147483647L
#define ULONG_MAX     0xffffffffUL
#define LLONG_MAX     9223372036854775807i64
#define LLONG_MIN   (-9223372036854775807i64 - 1)
#define ULLONG_MAX    0xffffffffffffffffui64

#define _I8_MIN     (-127i8 - 1)
#define _I8_MAX       127i8
#define _UI8_MAX      0xffui8

#define _I16_MIN    (-32767i16 - 1)
#define _I16_MAX      32767i16
#define _UI16_MAX     0xffffui16

#define _I32_MIN    (-2147483647i32 - 1)
#define _I32_MAX      2147483647i32
#define _UI32_MAX     0xffffffffui32

#define _I64_MIN    (-9223372036854775807i64 - 1)
#define _I64_MAX      9223372036854775807i64
#define _UI64_MAX     0xffffffffffffffffui64


unsigned short extract_uint16(int size, int* cursor_in_bits, int size_in_bits, char* buffer);
void take_data(unsigned char* srcBuffer, int* srcCursor, void* dstBuffer, int dstSize, int offsetSize);
void extract_c_str(char* packed_data_buffer, char* c_string_result, const int len, int* cursor_in_bits, int size_in_bits);
void take_value(const unsigned char* srcBuffer, int* p_srcCursor, void* dstValue, int dstSize, int offsetSize);
int extract_int32(int size, int offset, int* cursor_in_bits, int size_in_bits, char* buffer);
void print_bits_of_byte(unsigned char value_to_print);
void init_bitmask_table(void);
void print_bits_of_memory_block(void* start_byte, int offset, char* comment);
__declspec(dllexport) char* __cdecl get_server_details(char* ip_address, unsigned short port, int thread_id);


void init_socket(char* ip_address, unsigned short port, int thread_id);
