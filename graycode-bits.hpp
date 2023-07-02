static auto graycode_bits(unsigned m) {
    assert(m < sizeof(1u)*8);
    vector<uint8_t> gray;
    gray.reserve(1u<<m);
    // generate graycode sequence
    constexpr uint8_t neg = 1u<<7;
    // low 7 bits is changed bit number, 8th bit set if change is clear this bit
    for (unsigned bit = 0; bit < m; bit++) {
        unsigned k = gray.size();
        gray.push_back(bit);
        while(k--)
            gray.push_back(gray[k] ^ neg);
    }
    return gray;
}
