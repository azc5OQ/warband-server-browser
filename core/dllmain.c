#include <stdio.h>
#include <stdbool.h>
#include "veci.h"
#include <WinSock2.h>

#include "cJSON.h"

unsigned int bitmask_table[33];



#define RECEIVED_DATA_BUFFER_SIZE 1600
#define DATA_TO_SEND_BUFFER_SIZE 1600

bool is_bitsmask_table_initialized = false;
bool is_console_initialized = false;



typedef struct thread_context
{
    SOCKET client_socket;
    SOCKADDR_IN* server_address;
    SOCKADDR_IN* client_address;
    unsigned char* received_data_buffer;
    unsigned char* data_to_send_buffer;
    char* testarry;
    char return_buffer[1000];
    cJSON* json_message_object1;
    int server_address_size;
    _Bool job_finished;
    _Bool did_receive_response_yet;
    unsigned long long timestamp_connected;
} thread_context_t;

thread_context_t thread_contexts[20];



void watch(LPARAM thread_id)
{
    while (1)
    {
        unsigned long long timestamp_now = timeGetTime();

        if (thread_contexts[thread_id].did_receive_response_yet)
        {
#ifdef DEBUG_ACTIVE
            printf("%s %d %s", "thread id: ", thread_id, "did_receive_response_yet true \n");
#endif

            break;
        }
        if ((thread_contexts[thread_id].timestamp_connected + 3500 < timestamp_now) && !thread_contexts[thread_id].did_receive_response_yet)
        {
            //time to skip
#ifdef DEBUG_ACTIVE
            printf("%s %d %s", "SKIPPING SERVER thread id: ", thread_id, " SKIPPING SERVER \n");
#endif

            thread_contexts[thread_id].job_finished = true;
            break;
        }
        Sleep(1);
    }
}

typedef struct network_buffer
{
    int u1;
    int total_size_in_bits;
    unsigned char data[1500];
    unsigned char* external_buffer;
    unsigned char* buffer;
    int size_in_bits;
    int cursor_in_bits;
    double receive_time;
} network_buffer_t;

typedef struct rgl_string
{
    int cursor;
    char* buffer;
    int buffer_length;
    int str_length;
    char static_buffer[128];
} rgl_string_t;


void init_bitmask_table(void)
{
    bitmask_table[0] = 0;

    bitmask_table[1] = 0x1;
    bitmask_table[2] = 0x3;
    bitmask_table[3] = 0x7;
    bitmask_table[4] = 0xf;

    bitmask_table[5] = 0x1f;
    bitmask_table[6] = 0x3f;
    bitmask_table[7] = 0x7f;
    bitmask_table[8] = 0xff;

    bitmask_table[9] = 0x1ff;
    bitmask_table[10] = 0x3ff;
    bitmask_table[11] = 0x7ff;
    bitmask_table[12] = 0xfff;

    bitmask_table[13] = 0x1fff;
    bitmask_table[14] = 0x3fff;
    bitmask_table[15] = 0x7fff;
    bitmask_table[16] = 0xffff;

    bitmask_table[17] = 0x1ffff;
    bitmask_table[18] = 0x3ffff;
    bitmask_table[19] = 0x7ffff;
    bitmask_table[20] = 0xfffff;

    bitmask_table[21] = 0x1fffff;
    bitmask_table[22] = 0x3fffff;
    bitmask_table[23] = 0x7fffff;
    bitmask_table[24] = 0xffffff;

    bitmask_table[25] = 0x1ffffff;
    bitmask_table[26] = 0x3ffffff;
    bitmask_table[27] = 0x7ffffff;
    bitmask_table[28] = 0xfffffff;

    bitmask_table[29] = 0x1fffffff;
    bitmask_table[30] = 0x3fffffff;
    bitmask_table[31] = 0x7fffffff;
    bitmask_table[32] = 0xffffffff;
}

void print_bits_of_byte(unsigned char value_to_print)
{
    unsigned char help_byte = 1;
    for (char i = 7; i >= 0; i--)
    {
        unsigned char lol = (help_byte << i) & value_to_print;
        if (lol)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
}

void take_data(unsigned char* srcBuffer, int* srcCursor, void* dstBuffer, int dstSize, int offsetSize)
{
    int totalSize = 0;
    int sizeIndex = 10000;

    while (totalSize < dstSize)
    {
        int blockSize = dstSize - totalSize;
        int cursorShift = *srcCursor & 0x7;
        int sizeShift = totalSize & 0x7;

        sizeIndex = totalSize / 8;

        if (blockSize >= 8 - cursorShift)
            blockSize = 8 - cursorShift;

        if (blockSize >= 8 - sizeShift)
            blockSize = 8 - sizeShift;

        ((unsigned char*)dstBuffer)[sizeIndex] = (((bitmask_table[blockSize] << cursorShift) & srcBuffer[*srcCursor / 8]) >> cursorShift << sizeShift) | ((unsigned char*)dstBuffer)[sizeIndex] & ~((unsigned char)bitmask_table[blockSize] << sizeShift);

        *srcCursor += blockSize;
        totalSize += blockSize;
    }

    if (offsetSize > totalSize && ((unsigned char*)dstBuffer)[sizeIndex] >> ((totalSize - 1) & 0x7))
    {
        while (totalSize < offsetSize)
        {
            int blockSize = offsetSize - totalSize;
            int sizeShift = totalSize & 0x7;

            if (blockSize >= 8 - sizeShift)
                blockSize = 8 - sizeShift;

            ((unsigned char*)dstBuffer)[totalSize / 8] |= (unsigned char)bitmask_table[blockSize] << sizeShift;
            totalSize += blockSize;
        }
    }
}

void print_bits_of_memory_block(unsigned char* start_byte, int offset, char* comment)
{
    for (int i = 0; i < offset; i++)
    {
        unsigned char read_byte = *(unsigned char*)(start_byte + i);
        print_bits_of_byte(read_byte);
        printf(" ");
    }

    printf("%s %s %s", " | ", comment, "\n");
}

unsigned short network_buffer__extract_uint16(network_buffer_t* this, int size, int offset)
{
    unsigned short value = 0;

    if (this->cursor_in_bits + size <= this->size_in_bits)
    {
        take_value(this->buffer, &this->cursor_in_bits, &value, size, 0);
        value += offset;
    }

    return value;
}

void extract_c_str(char* packed_data_buffer, char* c_string_result, const int len, int* cursor_in_bits, int size_in_bits)
{
    if (*cursor_in_bits + 16 <= size_in_bits)
    {
        unsigned short length = extract_uint16(16, cursor_in_bits, size_in_bits, packed_data_buffer);

        if (length < len && *cursor_in_bits + length * 8 <= size_in_bits)
        {
            take_data(packed_data_buffer, cursor_in_bits, c_string_result, length * 8, 0);
            c_string_result[length] = '\0'; //null terminator
        }
    }
}

void network_buffer__extract_c_str(network_buffer_t* this, char* c_string_result, const int len)
{
    //printf("void network_buffer__extract_c_str(network_buffer_t* this, char *c_string_result, const int len) \n");
    if (this->cursor_in_bits + 16 <= this->size_in_bits)
    {
        int length = network_buffer__extract_uint16(this, 16, 0);

        //printf("%s %d %s", "network_buffer__extract_uint16 length of string is -> ", length, "\n");

        if (length < len && this->cursor_in_bits + length * 8 <= this->size_in_bits)
        {
            //printf("WORKS? \n");

            take_data(this->buffer, &this->cursor_in_bits, c_string_result, length * 8, 0);
            //printf("network_buffer__extract_c_str result \n");
            c_string_result[length] = '\0';
        }
        else
        {
            //printf("condition not met \n");

        }
    }
    else
    {
        //printf("this->size_in_bits man \n");
    }
}

short network_buffer__extract_int16(network_buffer_t* this, int size, int offset)
{
    short value = 0;

    if (this->cursor_in_bits + size <= this->size_in_bits)
    {
        if (offset == SHRT_MIN)
        {
            take_value(this->buffer, &this->cursor_in_bits, &value, size, 16);
        }
        else
        {
            take_value(this->buffer, &this->cursor_in_bits, &value, size, 0);
        }
    }

    return value;
}

unsigned int network_buffer__extract_uint32(network_buffer_t* this, int size, int offset)
{
    unsigned int value = 0;

    if (this->cursor_in_bits + size <= this->size_in_bits)
    {
        take_value(this->buffer, &this->cursor_in_bits, &value, size, 0);

        if (offset != INT_MAX)
        {
            value += offset;
        }
    }

    return value;
}

unsigned short extract_uint16(int size, int* cursor_in_bits, int size_in_bits, char* buffer)
{
    unsigned short value = 0;

    if (*cursor_in_bits + size <= size_in_bits)
    {
        take_value(buffer, cursor_in_bits, &value, size, 0);
    }

    return value;
}

void take_value(const unsigned char* srcBuffer, int* p_srcCursor, void* dstValue, int dstSize, int offsetSize)
{
    unsigned int* intBuffer = (unsigned int*)srcBuffer;
    int cursorShift = *p_srcCursor & 0x1F; //11
    int cursorIndex = *p_srcCursor / 32; //2

    //printf("%s %i %s", "srcCursor ", *p_srcCursor, "\n");

    //printf("%s %i %s", "cursorShift ", cursorShift, "\n");
    //printf("%s %i %s", "cursorIndex ", cursorIndex, "\n");

    int blockSize = dstSize;

    //printf("%s %i %s", "blockSize ", blockSize, "\n");


    if (dstSize >= 32 - cursorShift)
    {
        blockSize = 32 - cursorShift;
        //printf("%s %i %s", "if (dstSize >= 32 - cursorShift) blockSize ", blockSize, "\n");
    }

    //print_bits_of_memory_block(&intBuffer[cursorIndex], 4, "intBuffer[cursorIndex]");
    //print_bits_of_memory_block(&bitmask_table[blockSize], 4, "bitmask_table[blockSize]");
    //print_bits_of_memory_block(&cursorShift, 4, "cursorShift");

    unsigned int lul = bitmask_table[blockSize] << cursorShift;

    //print_bits_of_memory_block(&lul, 4, "bitmask_table[blockSize] << cursorShift");

    unsigned int lul1 = (intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift));

    //print_bits_of_memory_block(&lul1, 4, "(intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift)");

    unsigned int lul2 = ((intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift)) >> cursorShift);

    //print_bits_of_memory_block(&lul2, 4, "((intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift)) >> cursorShift)");

    unsigned int lul43 = *(unsigned int*)dstValue;

    //print_bits_of_memory_block(&lul43, 4, "*(unsigned int *)dstValue");

    unsigned int lul413 = ~bitmask_table[blockSize];

    //print_bits_of_memory_block(&lul413, 4, "~bitmask_table[blockSize];");


    unsigned int lul3 = *(unsigned int*)dstValue & ~bitmask_table[blockSize];

    //print_bits_of_memory_block(&lul3, 4, "*(unsigned int *)dstValue & ~bitmask_table[blockSize];");

    unsigned int lul4 = ((intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift)) >> cursorShift) | *(unsigned int*)dstValue & ~bitmask_table[blockSize];

    //print_bits_of_memory_block(&lul4, 4, "((intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift)) >> cursorShift) | *(unsigned int *)dstValue & ~bitmask_table[blockSize];");


    *(unsigned int*)dstValue = ((intBuffer[cursorIndex] & (bitmask_table[blockSize] << cursorShift)) >> cursorShift) | *(unsigned int*)dstValue & ~bitmask_table[blockSize];

    if (blockSize < dstSize)
    {
        *(unsigned int*)dstValue = ((bitmask_table[dstSize - blockSize] & intBuffer[cursorIndex + 1]) << blockSize) | *(unsigned int*)dstValue & ~(bitmask_table[dstSize - blockSize] << blockSize);
    }
    if (offsetSize > dstSize && *(unsigned int*)dstValue >> (dstSize - 1))
    {
        *(unsigned int*)dstValue |= bitmask_table[offsetSize - dstSize] << dstSize;
    }

    *p_srcCursor += dstSize;
}

int extract_int32(int size, int offset, int* cursor_in_bits, int size_in_bits, char* buffer)
{
    int value = 0;

    if (*cursor_in_bits + size <= size_in_bits)
    {
        if (offset == INT_MIN)
        {
            take_value(buffer, cursor_in_bits, &value, size, 32);
        }
        else
        {
            take_value(buffer, cursor_in_bits, &value, size, 0);
            value += offset;
        }
    }

    return value;
}

int network_buffer__extract_int32(network_buffer_t* this, int size, int offset)
{
    int value = 0;

    if (this->cursor_in_bits + size <= this->size_in_bits)
    {
        if (offset == INT_MIN)
        {
            take_value(this->buffer, &this->cursor_in_bits, &value, size, 32);
        }
        else
        {
            take_value(this->buffer, &this->cursor_in_bits, &value, size, 0);
            value += offset;
        }
    }

    return value;
}

void receive_data_thread(LPVOID param)
{
    int thread_id = (int)param;
    printf("%s %d %s", "receive_data_thread", thread_id, "\n");

    while (!thread_contexts[thread_id].job_finished)
    {
        memset(thread_contexts[thread_id].received_data_buffer, RECEIVED_DATA_BUFFER_SIZE, 0);

        int received_bytes_count = recvfrom(thread_contexts[thread_id].client_socket, thread_contexts[thread_id].received_data_buffer, RECEIVED_DATA_BUFFER_SIZE, 0, thread_contexts[thread_id].server_address, &thread_contexts[thread_id].server_address_size);

        if (received_bytes_count < 0)
        {
            printf("failed to receive data");
            thread_contexts[thread_id].job_finished = true;
            return;
        }
        else
        {
            if (received_bytes_count == 6)
            {
                //printf("%s %d %s", "%received_bytes_count", received_bytes_count, "\n");
                memset(thread_contexts[thread_id].received_data_buffer, RECEIVED_DATA_BUFFER_SIZE, 0);

                char peer0_1[] = { /* Packet 2 */
                0x1f, 0xd0, 0xda, 0x76, 0x33, 0x40, 0x00, 0x24,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

                int data_to_send_size = sizeof(peer0_1);

                memset(thread_contexts[thread_id].data_to_send_buffer, DATA_TO_SEND_BUFFER_SIZE, 0);

                memcpy(thread_contexts[thread_id].data_to_send_buffer, peer0_1, data_to_send_size);

                int send_bytes_count = sendto(thread_contexts[thread_id].client_socket, thread_contexts[thread_id].data_to_send_buffer, data_to_send_size, 0, thread_contexts[thread_id].server_address, thread_contexts[thread_id].server_address_size);
            }
            if (received_bytes_count > 100)
            {
                thread_contexts[thread_id].did_receive_response_yet = true; //information for watch thread

                network_buffer_t network_buffer1 = { 0 };
                memset(&network_buffer1, sizeof(network_buffer_t), 0);

                network_buffer1.total_size_in_bits = 12000;
                network_buffer1.size_in_bits = received_bytes_count * 8;
                network_buffer1.cursor_in_bits = 0;
                memcpy(&network_buffer1.data[0], thread_contexts[thread_id].received_data_buffer, received_bytes_count);
                network_buffer1.buffer = &network_buffer1.data[0];

                //
                //set initial cursor position first
                //if cursor_in_bits is not right server name simply wont be able to be read/extracted successfully from network buffer
                //the needed cursor position (position of a bit) for reading the server name, from data contained in network buffer, can be different each time, usually cursor position is 75 or 83,84
                //this makes reading data hard
                //

                //
                //update. now that initial position of network buffer is known. reading details of any server should not be a problem
                //

                network_buffer1.cursor_in_bits = 11;
        

                unsigned int unknown1287541287 = network_buffer__extract_uint32(&network_buffer1, 1, 0);
           
                //this was find out in IDA
                if (unknown1287541287 != 0)
                {
                    network_buffer1.cursor_in_bits = 32;
                }
                else
                {
                    network_buffer1.cursor_in_bits = 24;
                }

                unsigned short unknown128712875 = network_buffer__extract_uint16(&network_buffer1, 16, 0);
                //printf("%s %d %s", "unknown128712875 -> ", unknown128712875, "\n");

               // printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                unsigned short maybe_packet_id = network_buffer__extract_uint16(&network_buffer1, 16, 0);
               // printf("%s %d %s", "maybe_packet_id -> ", maybe_packet_id, "\n");


               // printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");


                short unknown_987 = network_buffer__extract_int16(&network_buffer1, 8, -1);
       
                bool unknown111 = ((network_buffer1.cursor_in_bits + 4) <= network_buffer1.size_in_bits);

                unsigned int unknown_read_value = 0;

                if (unknown111)
                {
                    take_value(network_buffer1.buffer, &network_buffer1.cursor_in_bits, &unknown_read_value, 4, 0);
                    //printf("%s %d %s", "unknown_read_value -> ", unknown_read_value, "\n");
                    //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");
                }


                //network_buffer1.cursor_in_bits = 68;

                unsigned short client_event_type = network_buffer__extract_uint16(&network_buffer1, 6, 0);
             //   printf("%s %d %s", "client_event_type -> ", client_event_type, "\n");
             //   printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                unsigned int unknown = network_buffer__extract_uint32(&network_buffer1, 1, 0);
           //     printf("%s %d %s", "unknown ", unknown, "\n");
            //    printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");
               

                char server_name[50] = { 0 };
                network_buffer__extract_c_str(&network_buffer1, &server_name, 50);
#ifdef DEBUG_ACTIVE
                printf("%s %s %s", "server name -> ", server_name, "\n");
#endif
                cJSON_AddStringToObject(thread_contexts[thread_id].json_message_object1, "server_name", server_name);

                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                unsigned int compatible_game_version = network_buffer__extract_uint32(&network_buffer1, 14, 0);
                //printf("%s %d %s", "compatible_game_version ", compatible_game_version, "\n");
                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "compatible_game_version", compatible_game_version);



                unsigned int compatible_module_version = network_buffer__extract_uint32(&network_buffer1, 14, 0);
                //printf("%s %d %s", "compatible_module_version ", compatible_module_version, "\n");
                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "compatible_module_version", compatible_module_version);

                char module_name[50] = { 0 };
                network_buffer__extract_c_str(&network_buffer1, &module_name, 50);
                //printf("%s %s %s", "module name -> ", module_name, "\n");
                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");
                cJSON_AddStringToObject(thread_contexts[thread_id].json_message_object1, "module_name", module_name);




                unsigned short site_no = network_buffer__extract_uint16(&network_buffer1, 16, 0);
                //printf("%s %d %s", "site_no -> ", site_no, "\n");
                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "site_no", site_no);



                char game_server_map[50] = { 0 };
                network_buffer__extract_c_str(&network_buffer1, &game_server_map, 50);
                //printf("%s %s %s", "game server map -> ", game_server_map, "\n");
                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                cJSON_AddStringToObject(thread_contexts[thread_id].json_message_object1, "game_server_map", game_server_map);



                unsigned short mission_template_no = network_buffer__extract_uint16(&network_buffer1, 16, 0);
                //printf("%s %d %s", "mission_template_no -> ", mission_template_no, "\n");
                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "mission_template_no", mission_template_no);

                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");


                char game_server_mode[50] = { 0 };
                network_buffer__extract_c_str(&network_buffer1, &game_server_mode, 50);
                //printf("%s %s %s", "game_server_mode -> ", game_server_mode, "\n");
                cJSON_AddStringToObject(thread_contexts[thread_id].json_message_object1, "game_server_mode", game_server_mode);



                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");


                int current_players = network_buffer__extract_int32(&network_buffer1, 8, -1);
                //printf("%s %d %s", "current_players -> ", current_players, "\n");

                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "current_players", current_players);


                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");


                int max_players = network_buffer__extract_int32(&network_buffer1, 8, -1);
                //printf("%s %d %s", "max_players -> ", max_players, "\n");
                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "max_players", max_players);



                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");


                unsigned int password_protected = network_buffer__extract_uint32(&network_buffer1, 1, 0);
                //printf("%s %d %s", "password_protected ", password_protected, "\n");

                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "password_protected", password_protected);


                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                unsigned int dedicated = network_buffer__extract_uint32(&network_buffer1, 1, 0);
                //printf("%s %d %s", "dedicated ", dedicated, "\n");

                cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "dedicated", dedicated);


                //printf("%s %d %s", "cursor in bits ", network_buffer1.cursor_in_bits, "\n");

                //cJSON_AddItemToObject(json_root_object1, "message", json_message_object1);

                char* json_root_object1_string = cJSON_PrintUnformatted(thread_contexts[thread_id].json_message_object1);

                //printf("%s %s %s", "json_root_object1_string ", json_root_object1_string, "\n");


                memcpy(thread_contexts[thread_id].return_buffer, json_root_object1_string, strlen(json_root_object1_string));

                thread_contexts[thread_id].job_finished = true;
            }
        }
    }
}

unsigned __int64 network_buffer__extract_uint64(network_buffer_t* this, int size)
{
    unsigned __int64 value = 0;

    if (this->cursor_in_bits + size <= this->size_in_bits)
    {
        take_data(this->buffer, &this->cursor_in_bits, &value, size, 0);
    }

    return value;
}

void init_socket(char* ip_address, unsigned short port, int thread_id)
{
    WSADATA wsa;

    struct sockaddr_in si_other;
    thread_contexts[thread_id].server_address_size = sizeof(si_other);

    //Initialise winsock

    int status = WSAStartup(0x202, &wsa);
    if (status == SOCKET_ERROR)
    {
#ifdef DEBUG_ACTIVE

        printf("WSAStartup == SOCKET_ERROR\n");
#endif
        WSACleanup();
    }
    else
    {
#ifdef DEBUG_ACTIVE
        printf("%s %d %s", "socket initialized thread_id: ", thread_id, "\n");
#endif
    }


    thread_contexts[thread_id].server_address->sin_family = AF_INET;
    thread_contexts[thread_id].server_address->sin_addr.s_addr = inet_addr(ip_address);
    thread_contexts[thread_id].server_address->sin_port = htons(port);

    thread_contexts[thread_id].client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (thread_contexts[thread_id].client_socket == INVALID_SOCKET)
    {
        printf("socket creation failed");
        WSACleanup();
        return;
    }

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)receive_data_thread, thread_id, 0, 0); //crashes without creating new thread

    //char peer0_0[] = { /* Packet 427 */
    //0x06, 0xf0, 0x56, 0x00, 0x00, 0x00 };

    char peer0_0[] = { /* Packet 1 */
    0x06, 0xd0, 0xda, 0x00, 0x00, 0x01 };

    int data_to_send_size = sizeof(peer0_0);

    memcpy(thread_contexts[thread_id].data_to_send_buffer, peer0_0, 6);

    int send_bytes_count = sendto(
        thread_contexts[thread_id].client_socket,
        thread_contexts[thread_id].data_to_send_buffer,
        data_to_send_size,
        0,
        thread_contexts[thread_id].server_address,
        thread_contexts[thread_id].server_address_size);
    
    if (send_bytes_count == SOCKET_ERROR)
    {
        printf("sendto failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
}



__declspec(dllexport) char* __cdecl get_server_details(char* ip_address, unsigned short port, int thread_id)
{
    thread_contexts[thread_id].did_receive_response_yet = true;
    Sleep(20);
    thread_contexts[thread_id].did_receive_response_yet = false;

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)watch, thread_id, 0, 0);

#ifdef DEBUG_ACTIVE
    if (!is_console_initialized)
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        is_console_initialized = true;
    }
#endif

    thread_contexts[thread_id].timestamp_connected = timeGetTime();
    thread_contexts[thread_id].did_receive_response_yet = false;
    thread_contexts[thread_id].client_socket = (SOCKET)calloc(sizeof(SOCKET), 1);
    thread_contexts[thread_id].server_address = (SOCKADDR_IN*)calloc(sizeof(SOCKADDR_IN), 1);
    thread_contexts[thread_id].client_address = (SOCKADDR_IN*)calloc(sizeof(SOCKADDR_IN), 1);
    thread_contexts[thread_id].received_data_buffer = (unsigned char*)calloc(RECEIVED_DATA_BUFFER_SIZE, 1);
    thread_contexts[thread_id].data_to_send_buffer = (unsigned char*)calloc(DATA_TO_SEND_BUFFER_SIZE, 1);
    memset(thread_contexts[thread_id].return_buffer, 0, 1000);
    thread_contexts[thread_id].testarry = (char*)calloc(450, 1);
    thread_contexts[thread_id].json_message_object1 = cJSON_CreateObject();

    cJSON_AddStringToObject(thread_contexts[thread_id].json_message_object1, "ip_address", ip_address);
    cJSON_AddNumberToObject(thread_contexts[thread_id].json_message_object1, "port", port);


    if (thread_contexts[thread_id].client_socket == 0 || 
        thread_contexts[thread_id].server_address == 0 || 
        thread_contexts[thread_id].client_address == 0 || 
        thread_contexts[thread_id].received_data_buffer == 0 || 
        thread_contexts[thread_id].data_to_send_buffer == 0 || 
        thread_contexts[thread_id].testarry == 0)
    {
        return 0;
    }

    thread_contexts[thread_id].job_finished = false;

    if (!is_bitsmask_table_initialized)
    {
        is_bitsmask_table_initialized = true;
        init_bitmask_table();
    }

    init_socket(ip_address, port, thread_id);

    while (!thread_contexts[thread_id].job_finished)
    {
        Sleep(10);
    }


    memset(thread_contexts[thread_id].received_data_buffer, 0, RECEIVED_DATA_BUFFER_SIZE);
    memset(thread_contexts[thread_id].data_to_send_buffer, 0, DATA_TO_SEND_BUFFER_SIZE);
    memset(thread_contexts[thread_id].testarry, 0, 450);

    free(thread_contexts[thread_id].received_data_buffer);
    free(thread_contexts[thread_id].data_to_send_buffer);
    free(thread_contexts[thread_id].testarry);

    thread_contexts[thread_id].received_data_buffer = 0;
    thread_contexts[thread_id].data_to_send_buffer = 0;
    thread_contexts[thread_id].testarry = 0;

    cJSON_Delete(thread_contexts[thread_id].json_message_object1);

    closesocket(thread_contexts[thread_id].client_socket);

    thread_contexts[thread_id].client_socket = 0;
    thread_contexts[thread_id].server_address = 0;
    thread_contexts[thread_id].client_address = 0;


    return &thread_contexts[thread_id].return_buffer;
}
