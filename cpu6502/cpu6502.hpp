#pragma once
#include "type_aliases.hpp"

#include <concepts>
#include <functional>
#include <vector>

template <typename T>
concept ConstMapable = requires(T object, u16 address) {
    { object.read(address) } -> std::same_as<u8>;
};
template <typename T>
concept Mapable = ConstMapable<T> && requires(T object, u16 address, u8 data) {
    { object.write(address, data) } -> std::same_as<void>;
};

struct AddressRange
{
    u16 start;
    u16 end;

    bool contains(u16 address, u16& offset) const
    {
        offset = address - start;
        return (address >= start) && (address <= end);
    }
};

class CPU6502
{
public:
    static constexpr u8 BITMASK_C = 1 << 0;
    static constexpr u8 BITMASK_Z = 1 << 1;
    static constexpr u8 BITMASK_I = 1 << 2;
    static constexpr u8 BITMASK_D = 1 << 3;
    static constexpr u8 BITMASK_B = 1 << 4;
    static constexpr u8 BITMASK_U = 1 << 5;
    static constexpr u8 BITMASK_V = 1 << 6;
    static constexpr u8 BITMASK_N = 1 << 7;

    union Flags
    {
        struct {
            u8 C : 1; // 0
            u8 Z : 1; // 1
            u8 I : 1; // 2
            u8 D : 1; // 3
            u8 B : 1; // 4
            u8 U : 1; // 5
            u8 V : 1; // 6
            u8 N : 1; // 7
        } bits;
        u8 byte;
    };

    template <Mapable Device>
    void map(Device& device, AddressRange range);
    template <ConstMapable ConstDevice>
    void map(const ConstDevice& device, AddressRange range);

    void reset();
    void clock();
    void setIRQ(bool state) { m_irq = state; }
    void setNMI(bool state) { m_nmi = state; }

    u16 getPC() const { return PC; }
    Flags getFlags() const { return F; }

    CPU6502() = default;
    CPU6502(const CPU6502&) = delete;
    CPU6502& operator=(const CPU6502&) = delete;
private:
    void IRQ();
    void NMI();

    // Memory Access:
    u8 load8(u16 address);
    u16 load16(u16 address);
    void store8(u16 address, u8 data);
    void store16(u16 address, u16 data);
    void push8(u8 data);
    void push16(u16 data);
    u8 pop8();
    u16 pop16();

    // Addressing Modes:
    void am_ACC(); void am_IMM();
    void am_ZPG(); void am_ZPX(); void am_ZPY();
    void am_ABS(); void am_ABX(); void am_ABY();
    void am_IND(); void am_INX(); void am_INY();

    void op_NOP();

    // Branch Instructions:
    void op_JMP();
    void op_BPL(); void op_BMI();
    void op_BEQ(); void op_BNE();
    void op_BCS(); void op_BCC();
    void op_BVS(); void op_BVC();
    void op_JSR(); void op_RTS();
    void op_BRK(); void op_RTI();
    
    // Load/Store Instructions:
    void op_LDA(); void op_LDX(); void op_LDY();
    void op_STA(); void op_STX(); void op_STY();

    // Transfer Instructions:
    void op_TXA(); void op_TAX();
    void op_TYA(); void op_TAY();
    void op_TSX(); void op_TXS();

    // Stack Instructions:
    void op_PHA(); void op_PLA();
    void op_PHP(); void op_PLP();

    // ALU Instructions:
    void op_ADC(); void op_SBC();
    void op_ORA(); void op_AND(); void op_EOR();
    void op_LSR(); void op_ASL();
    void op_ROL(); void op_ROR();
    void op_BIT();
    void op_INC(); void op_INX(); void op_INY();
    void op_DEC(); void op_DEX(); void op_DEY();
    void op_CMP(); void op_CPX(); void op_CPY();

    // Flags Instructions:
    void op_SEC(); void op_CLC();
    void op_SEI(); void op_CLI();
    void op_SED(); void op_CLD();
    void op_CLV();

    // CPU State:
    u16 PC;
    u8 ACC;
    Flags F;
    u8 X;
    u8 Y;
    u8 SP;

    // Helpers:
    u16 m_cyclesLeft;
    u16 m_absoluteAddress;
    bool m_isACCAddressing = false;
    bool m_irq = false;
    bool m_nmi = false;
    bool m_isDuringNMI = false;

    struct ReadMapEntry {
        using ReadFunc = std::function<u8(u16)>;
        
        AddressRange range;
        ReadFunc read;

        ReadMapEntry(AddressRange inRange, ReadFunc inRead) :
            range{ inRange }, read{ inRead } {}
    };
    std::vector<ReadMapEntry> m_readMap;

    struct WriteMapEntry {
        using WriteFunc = std::function<void(u16, u8)>;

        AddressRange range;
        WriteFunc write;

        WriteMapEntry(AddressRange inRange, WriteFunc inWrite) :
            range{ inRange }, write{ inWrite } {}
    };
    std::vector<WriteMapEntry> m_writeMap;
};

template<Mapable Device>
inline void CPU6502::map(Device& device, AddressRange range)
{
    m_readMap.emplace_back(
        ReadMapEntry{
            range,
            [&device](u16 address) { return device.read(address); }
        }
    );
    m_writeMap.emplace_back(
        WriteMapEntry{
            range,
            [&device](u16 address, u8 data) { return device.write(address, data); }
        }
    );
}

template<ConstMapable ConstDevice>
inline void CPU6502::map(const ConstDevice& device, AddressRange range)
{
    m_readMap.emplace_back(
        ReadMapEntry{
            range,
            [&device](u16 address) { return device.read(address); }
        }
    );
}
