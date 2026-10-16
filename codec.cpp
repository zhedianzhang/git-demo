#include <cstdint>
#include <cstring>
#include <string>
#include <cstdio>

// 把 payload 编码成 4 字节长度前缀 + 数据
std::string encode_frame(const std::string& payload) {
    std::string out;
    uint32_t n = static_cast<uint32_t>(payload.size());
    char hdr[4];
    hdr[0] = static_cast<char>((n >> 24) & 0xFF);
    hdr[1] = static_cast<char>((n >> 16) & 0xFF);
    hdr[2] = static_cast<char>((n >> 8) & 0xFF);
    hdr[3] = static_cast<char>(n & 0xFF);
    out.append(hdr, 4);
    out += payload;
    return out;
}

// 从缓冲区首部读出 4 字节长度前缀（大端/网络字节序）
bool decode_frame(const char* buf, size_t len, uint32_t* out_len) {
    if (len < 4) {
        return false;
    }
    uint32_t n = 0;
    n |= static_cast<uint32_t>(static_cast<unsigned char>(buf[0])) << 24;
    n |= static_cast<uint32_t>(static_cast<unsigned char>(buf[1])) << 16;
    n |= static_cast<uint32_t>(static_cast<unsigned char>(buf[2])) << 8;
    n |= static_cast<uint32_t>(static_cast<unsigned char>(buf[3]));
    *out_len = n;
    return true;
}

// 解析出一帧，把 payload 拷进调用方提供的缓冲区
bool decode_into_buffer(const char* buf, size_t len, char* out, size_t out_cap) {
    uint32_t n = 0;
    if (!decode_frame(buf, len, &n)) {
        return false;
    }
    if (n > out_cap) {
        return false;
    }
    if (len < 4 + n) {
        return false;
    }
    memcpy(out, buf + 4, n);
    return true;
}
