#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>  // 包含htonl和ntohl函数

// 使用2的幂次方作为缓冲区大小，这样可以用位运算代替取模运算
#define RING_BUFFER_SIZE 1024  // 2^10
#define RING_BUFFER_MASK (RING_BUFFER_SIZE - 1)
#define MAX_PACKET_SIZE 512
#define LENGTH_FIELD_SIZE 4  // 使用4字节表示长度

// 环形缓冲区方法声明
typedef struct RingBuffer_t RingBuffer;
typedef size_t (*RingBufferWrite)(RingBuffer* rb, const uint8_t* data, size_t len);
typedef size_t (*RingBufferPeek)(const RingBuffer* rb, uint8_t* data, size_t len);
typedef void (*RingBufferConsume)(RingBuffer* rb, size_t len);
typedef size_t (*RingBufferFreeSpace)(const RingBuffer* rb);
typedef size_t (*RingBufferDataSize)(const RingBuffer* rb);
// LV包解析器方法声明
typedef struct LVParser_t LVParser;
typedef void (*LVParserTryParse)(LVParser* parser);
typedef void (*LVParserProcessPacket)(const uint8_t* data, size_t len);

// 环形缓冲区结构
struct RingBuffer_t {
    uint8_t buffer[RING_BUFFER_SIZE];
    size_t read_pos;   // 读指针位置
    size_t write_pos;  // 写指针位置
    size_t size;       // 当前存储的数据量

    RingBufferWrite write;           // 写入数据的函数指针
    RingBufferPeek peek;             // 查看数据但不移除的函数指针
    RingBufferConsume consume;       // 消费(移除)数据的函数指针
    RingBufferFreeSpace free_space;  // 获取可用空间的函数指针
    RingBufferDataSize data_size;    // 获取已用数据量的函数指针
};
// LV包解析器状态
struct LVParser_t {
    RingBuffer* rb;
    int state;           // 解析状态: 0表示等待长度字段, 1表示等待数据
    uint32_t remaining;  // 当前数据包剩余需要读取的字节数

    LVParserTryParse try_parse;            // 尝试解析数据包的函数指针
    LVParserProcessPacket process_packet;  // 处理完整数据包的函数指针
};

// 初始化环形缓冲区
static void ring_buffer_init(RingBuffer* rb)
{
    rb->read_pos = 0;
    rb->write_pos = 0;
    rb->size = 0;
}

// 获取环形缓冲区中可用的空间
static size_t ring_buffer_free_space(const RingBuffer* rb)
{
    return RING_BUFFER_SIZE - rb->size;
}

// 获取环形缓冲区中数据量
static size_t ring_buffer_data_size(const RingBuffer* rb)
{
    return rb->size;
}

// 写入数据到环形缓冲区
static size_t ring_buffer_write(RingBuffer* rb, const uint8_t* data, size_t len)
{
    if (len > ring_buffer_free_space(rb)) {
        len = ring_buffer_free_space(rb);  // 防止溢出
    }

    // 计算到缓冲区末尾的连续空间
    size_t continuous_space = RING_BUFFER_SIZE - (rb->write_pos & RING_BUFFER_MASK);

    if (len <= continuous_space) {
        // 数据可以一次性写入
        memcpy(rb->buffer + rb->write_pos, data, len);
    } else {
        // 需要分两次写入
        memcpy(&(rb->buffer[rb->write_pos]), data, continuous_space);
        memcpy(&rb->buffer[0], &data[continuous_space], len - continuous_space);
    }

    rb->write_pos = (rb->write_pos + len) & RING_BUFFER_MASK;
    rb->size += len;
    return len;
}

// 从环形缓冲区读取数据但不移动读指针
size_t ring_buffer_peek(const RingBuffer* rb, uint8_t* data, size_t len)
{
    if (len > rb->size) {
        len = rb->size;
    }

    size_t continuous_data = RING_BUFFER_SIZE - (rb->read_pos & RING_BUFFER_MASK);
    if (len <= continuous_data) {
        // 数据是连续的
        memcpy(data, rb->buffer + rb->read_pos, len);
    } else {
        // 数据需要分两次读取
        memcpy(data, rb->buffer + rb->read_pos, continuous_data);
        memcpy(data + continuous_data, rb->buffer, len - continuous_data);
    }

    return len;
}

// 从环形缓冲区中消费数据
void ring_buffer_consume(RingBuffer* rb, size_t len)
{
    if (len > rb->size) {
        len = rb->size;
    }
    rb->read_pos = (rb->read_pos + len) & RING_BUFFER_MASK;
    rb->size -= len;
}



// 初始化LV解析器
static void lv_parser_init(LVParser* parser, RingBuffer* rb)
{
    parser->rb = rb;
    parser->state = 0;
    parser->remaining = LENGTH_FIELD_SIZE;
}

// 处理一个完整的LV包
void process_lv_packet(const uint8_t* data, size_t len)
{
    printf("Received packet (length=%zu): ", len);
    for (size_t i = 0; i < len && i < 16; i++) {  // 只打印前16个字节
        printf("%02x ", data[i]);
    }
    if (len > 16)
        printf("...");
    printf("\n");
}

// 从大端序转换为主机字节序
static inline uint32_t ntohl_fast(const uint8_t* buf)
{
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

// 尝试解析LV包（优化版本）
void try_parse_lv_packets(LVParser* parser)
{
    uint8_t temp[MAX_PACKET_SIZE];

    while (1) {
        if (parser->state == 0) {  // 等待长度字段
            if (parser->rb->data_size(parser->rb) < LENGTH_FIELD_SIZE) {
                break;  // 数据不足
            }

            // 读取长度字段
            parser->rb->peek(parser->rb, temp, LENGTH_FIELD_SIZE);
            uint32_t length = ntohl_fast(temp);  // 使用优化的字节序转换

            if (length > MAX_PACKET_SIZE - LENGTH_FIELD_SIZE) {
                printf("Error: packet too large (%u bytes)\n", length);
                parser->rb->consume(parser->rb, LENGTH_FIELD_SIZE);
                continue;
            }

            parser->remaining = length;
            parser->state = 1;
            parser->rb->consume(parser->rb, LENGTH_FIELD_SIZE);
        }

        if (parser->state == 1) {  // 等待数据
            if (parser->rb->data_size(parser->rb) < parser->remaining) {
                break;  // 数据不足
            }

            // 读取完整数据包
            parser->rb->peek(parser->rb, temp, parser->remaining);
            parser->process_packet(temp, parser->remaining);
            parser->rb->consume(parser->rb, parser->remaining);

            // 重置状态，准备解析下一个包
            parser->state = 0;
            parser->remaining = LENGTH_FIELD_SIZE;
        }
    }
}

// 模拟数据生成函数
void generate_test_packet(uint8_t* buffer, size_t* len)
{
    static uint32_t counter = 0;
    counter++;

    // 生成一个测试包
    uint32_t data_len = 8;  // 固定8字节数据长度

    // 写入长度字段（网络字节序，大端）
    uint32_t net_len = htonl(data_len);
    memcpy(buffer, &net_len, LENGTH_FIELD_SIZE);

    // 写入数据
    for (uint32_t i = 0; i < data_len; i++) {
        buffer[LENGTH_FIELD_SIZE + i] = (counter + i) & 0xFF;
    }

    *len = LENGTH_FIELD_SIZE + data_len;
}

int main()
{
    RingBuffer rb;
    LVParser parser;
    uint8_t test_data[MAX_PACKET_SIZE];
    size_t test_data_len;

    // 初始化环形缓冲区并注册方法
    ring_buffer_init(&rb);
    rb.write = ring_buffer_write;
    rb.peek = ring_buffer_peek;
    rb.consume = ring_buffer_consume;
    rb.free_space = ring_buffer_free_space;
    rb.data_size = ring_buffer_data_size;

    // 初始化解析器并注册方法
    lv_parser_init(&parser, &rb);
    parser.try_parse = try_parse_lv_packets;
    parser.process_packet = process_lv_packet;

    // 模拟接收5个数据包
    printf("Simulating stream reception with 5 packets...\n\n");
    for (int i = 0; i < 5; i++) {
        // 生成测试数据
        generate_test_packet(test_data, &test_data_len);

        // 模拟分段接收数据
        size_t first_part = test_data_len / 2;
        size_t second_part = test_data_len - first_part;

        printf("Receiving packet %d in two parts (%zu + %zu bytes)...\n", i + 1, first_part, second_part);

        // 写入第一部分
        rb.write(&rb, test_data, first_part);
        parser.try_parse(&parser);

        // 写入第二部分
        rb.write(&rb, test_data + first_part, second_part);
        parser.try_parse(&parser);

        printf("\n");
        usleep(100000);  // 模拟网络延迟
    }

    return 0;
}
