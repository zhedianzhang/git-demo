#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>

std::string encode_frame(const std::string& payload);
bool decode_frame(const char* buf, size_t len, uint32_t* out_len);
bool decode_into_buffer(const char* buf, size_t len, char* out, size_t out_cap);

static int g_fail = 0;

void expect_true(const char* name, bool v) {
    if (v) {
        std::cout << "[PASS] " << name << "\n";
    } else {
        std::cout << "[FAIL] " << name << "\n";
        ++g_fail;
    }
}

void expect_u32(const char* name, uint32_t got, uint32_t want) {
    if (got == want) {
        std::cout << "[PASS] " << name << " = " << got << "\n";
    } else {
        std::cout << "[FAIL] " << name << " got " << got << " want " << want << "\n";
        ++g_fail;
    }
}

int main() {
    // 1) 短 payload 的完整往返
    {
        std::string payload = "HELLO";
        std::string wire = encode_frame(payload);
        uint32_t n = 0;
        expect_true("decode_frame_ok", decode_frame(wire.data(), wire.size(), &n));
        expect_u32("decoded_length", n, 5u);

        char out[512] = {0};
        bool ok = decode_into_buffer(wire.data(), wire.size(), out, sizeof(out));
        expect_true("decode_into_buffer_ok", ok);
        expect_true("payload_matches", std::string(out, n) == payload);
    }

    // 2) 截断的报头必须被拒绝
    {
        char bad[3] = {0, 0, 0};
        uint32_t n = 0;
        expect_true("truncated_header_rejected", !decode_frame(bad, sizeof(bad), &n));
    }

    // 3) 长 payload：长度前缀占满 4 字节
    {
        std::string payload(200, 'A');
        std::string wire = encode_frame(payload);
        uint32_t n = 0;
        expect_true("long_message_decode_frame_ok", decode_frame(wire.data(), wire.size(), &n));
        expect_u32("long_message_decoded_length", n, 200u);

        char out[512] = {0};
        bool ok = decode_into_buffer(wire.data(), wire.size(), out, sizeof(out));
        expect_true("long_message_decode_into_buffer_ok", ok);
        expect_true("long_message_payload_matches", std::string(out, n) == payload);
    }

    // 4) 目标缓冲区不足以容纳该帧：必须被拒绝
    {
        std::string payload(200, 'B');
        std::string wire = encode_frame(payload);

        uint32_t n = 0;
        expect_true("small_target_decode_frame_ok", decode_frame(wire.data(), wire.size(), &n));
        expect_u32("small_target_decoded_length", n, 200u);

        char out[64] = {0};
        bool ok = decode_into_buffer(wire.data(), wire.size(), out, sizeof(out));
        expect_true("small_target_rejected", !ok);
    }

    std::cout << (g_fail == 0 ? "ALL PASS" : "SOME FAILED") << "\n";
    return g_fail == 0 ? 0 : 1;
}
