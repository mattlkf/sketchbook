// This file should be common to the sender and receiver

struct tinkl_packet{
    uint16_t temp;
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t turbidity;
    uint32_t node_id;
    uint32_t packet_id;
    uint16_t battery_voltage;
    uint16_t checksum;
}__attribute__((packed));

// The "packed" attribute tells GCC to omit padding between fields
// We don't want to waste space in the radio packets we send by adding padding
