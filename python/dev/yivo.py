#!/usr/bin/env python3
import struct
from typing import Optional, Tuple, Union

# Yivo protocol constants
YIVO_HEADER_0 = ord('$')
YIVO_HEADER_1 = ord('K')
YIVO_OVERHEAD = 6  # Header ($K), size (2 bytes), ID, checksum
YIVO_PKT_NULL = -2
YIVO_SRC_NULL = -3
YIVO_SIZE_ERROR = -4
YIVO_PKT_HEADER_ERR = -5
YIVO_PKT_ID_ERR = -6
YIVO_PKT_CS_ERR = -7

class YivoError(Exception):
    """Exception raised for Yivo protocol errors."""
    def __init__(self, code: int, message: str):
        self.code = code
        super().__init__(message)

class YivoPacket:
    crc8_table = [
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
    ]
    """A Yivo packet for serialization."""
    def __init__(self, size: int):
        """
        Initialize a Yivo packet with a given payload size.

        Args:
            size: Size of the payload (excluding header and checksum).

        Raises:
            ValueError: If size is negative or too large.
        """
        if size < 0 or size > 0xFFFF:
            raise ValueError("Invalid payload size")
        self.size = size + YIVO_OVERHEAD
        self.data = bytearray(self.size)

    def pack(self, msgid: int, payload: bytes) -> None:
        """
        Pack a payload into the packet with headers and CRC-8 checksum.

        Args:
            msgid: Message ID (1-255).
            payload: Payload data as bytes.

        Raises:
            YivoError: If msgid is invalid, payload size mismatches, or inputs are None.
        """
        if msgid < 1 or msgid > 255:
            raise YivoError(YIVO_PKT_ID_ERR, "Message ID must be 1-255")
        if payload is None:
            raise YivoError(YIVO_SRC_NULL, "Payload cannot be None")
        if len(payload) + YIVO_OVERHEAD != self.size:
            raise YivoError(YIVO_SIZE_ERROR, f"Payload size {len(payload)} does not match expected {self.size - YIVO_OVERHEAD}")

        self.data[0] = YIVO_HEADER_0
        self.data[1] = YIVO_HEADER_1
        self.data[2] = len(payload) & 0xFF  # Low byte
        self.data[3] = len(payload) >> 8    # High byte
        self.data[4] = msgid
        self.data[5] = 0  # Checksum placeholder
        self.data[6:] = payload
        self.data[5] = self._calc_checksum()

    def _calc_checksum(self) -> int:
        """Calculate CRC-8-ATM checksum over size, ID, and payload."""
        crc = 0x00
        for i in (2, 3, 4):  # Size (LN, HN), ID
            crc = self.crc8_table[crc ^ self.data[i]]
        for b in self.data[6:]:  # Payload
            crc = self.crc8_table[crc ^ b]
        return crc

    def validate(self) -> None:
        """
        Validate the packet's headers, size, and checksum.

        Raises:
            YivoError: If headers, message ID, size, or checksum are invalid.
        """
        if len(self.data) < YIVO_OVERHEAD:
            raise YivoError(YIVO_SIZE_ERROR, "Packet too small")
        if self.data[0] != YIVO_HEADER_0 or self.data[1] != YIVO_HEADER_1:
            raise YivoError(YIVO_PKT_HEADER_ERR, "Invalid header")
        if self.data[4] == 0:
            raise YivoError(YIVO_PKT_ID_ERR, "Invalid message ID 0")
        payload_size = (self.data[3] << 8) | self.data[2]
        if payload_size + YIVO_OVERHEAD != len(self.data):
            raise YivoError(YIVO_SIZE_ERROR, "Size mismatch")
        if self.data[5] != self._calc_checksum():
            raise YivoError(YIVO_PKT_CS_ERR, "Checksum mismatch")

class YivoParser:
    """Streaming parser for Yivo packets with a fixed-size buffer."""
    _H0_STATE = 0
    _H1_STATE = 1
    _SZ0_STATE = 2
    _SZ1_STATE = 3
    _ID_STATE = 4
    _CS_STATE = 5
    _DATA_STATE = 6
    crc8_table = [
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
    ]

    def __init__(self, buffer: bytearray, max_size: int):
        """
        Initialize parser with a user-supplied fixed-size buffer.

        Args:
            buffer: Bytearray for storing payload.
            max_size: Maximum payload size the buffer can hold.

        Raises:
            ValueError: If buffer is None or max_size is invalid.
        """
        if buffer is None or max_size <= 0 or len(buffer) < max_size:
            raise ValueError("Invalid buffer or max_size")
        self.buffer = buffer
        self.max_size = max_size
        self.reset()

    def reset(self) -> None:
        """Reset parser state."""
        self.state = self._H0_STATE
        self.payload_size = 0
        self.msgid = 0
        self.index = 0
        self.cs = 0
        self.computed_crc = 0
        self.ln = 0
        self.hn = 0
        self.id = 0

    def parse(self, byte: int) -> Optional[int]:
        """
        Parse a single byte, returning message ID if a valid message is complete.

        Args:
            byte: The byte to parse (0-255).

        Returns:
            Message ID (1-255) if a valid message is parsed, None otherwise.

        Raises:
            ValueError: If byte is out of range.
        """
        if not 0 <= byte <= 255:
            raise ValueError("Byte out of range")

        if self.state == self._H0_STATE:
            if byte == YIVO_HEADER_0:
                self.reset()
                self.state = self._H1_STATE
            return None
        elif self.state == self._H1_STATE:
            if byte == YIVO_HEADER_1:
                self.state = self._SZ0_STATE
            else:
                self.state = self._H0_STATE
            return None
        elif self.state == self._SZ0_STATE:
            self.ln = byte
            self.payload_size = byte
            self.state = self._SZ1_STATE
            return None
        elif self.state == self._SZ1_STATE:
            self.hn = byte
            self.payload_size |= byte << 8
            if self.payload_size > self.max_size:
                self.reset()
                return None
            self.state = self._ID_STATE
            return None
        elif self.state == self._ID_STATE:
            if byte == 0:
                self.reset()
                return None
            self.id = byte
            self.msgid = byte
            self.state = self._CS_STATE
            return None
        elif self.state == self._CS_STATE:
            self.cs = byte
            self.index = 0
            self.computed_crc = 0x00
            self.computed_crc = self.crc8_table[self.computed_crc ^ self.ln]
            self.computed_crc = self.crc8_table[self.computed_crc ^ self.hn]
            self.computed_crc = self.crc8_table[self.computed_crc ^ self.id]
            self.state = self._DATA_STATE
            return None
        elif self.state == self._DATA_STATE:
            self.buffer[self.index] = byte
            self.computed_crc = self.crc8_table[self.computed_crc ^ byte]
            self.index += 1
            if self.index == self.payload_size:
                if self.computed_crc != self.cs:
                    self.reset()
                    return None
                self.state = self._H0_STATE
                return self.msgid
            return None
        return None

    def get_payload(self, expected_size: int, fmt: str) -> Tuple[int, Union[tuple, None]]:
        """
        Extract and unpack the parsed payload using a struct format.

        Args:
            expected_size: Expected payload size.
            fmt: Struct format string for unpacking (e.g., '<ii' for test_t).

        Returns:
            Tuple of (message ID, unpacked data as tuple) if valid, (0, None) on error.

        Raises:
            YivoError: If payload size mismatches or unpacking fails.
        """
        if self.buffer is None:
            raise YivoError(YIVO_PKT_NULL, "No payload available")
        if expected_size != self.payload_size:
            raise YivoError(YIVO_SIZE_ERROR, f"Expected size {expected_size}, got {self.payload_size}")
        try:
            data = struct.unpack(fmt, self.buffer[:self.payload_size])
            return (self.msgid, data)
        except struct.error as e:
            raise YivoError(YIVO_SIZE_ERROR, f"Unpacking failed: {e}")

def pack_message(msgid: int, data: tuple, fmt: str) -> bytes:
    """
    Pack a Python tuple into a Yivo packet.

    Args:
        msgid: Message ID (1-255).
        data: Tuple of values to pack (e.g., (-10, 300) for test_t).
        fmt: Struct format string (e.g., '<ii' for test_t).

    Returns:
        Serialized Yivo packet as bytes.

    Raises:
        YivoError: If packing fails or inputs are invalid.
    """
    try:
        # print(type(fmt), type(data))
        payload = struct.pack(fmt, *data)
        # print(payload)
    except struct.error as e:
        raise YivoError(YIVO_SRC_NULL, f"Packing failed: {e}")
    pkt = YivoPacket(len(payload))
    pkt.pack(msgid, payload)
    # return bytes(pkt.data)
    # print(f"{pkt.data == bytes(pkt.data)}")
    return pkt.data

def example_usage():
    """Example usage replicating multi.c functionality."""
    # Create packets
    test_t = (-10, 300)
    test2_t = (999, -3.14, b'a')
    pkt1 = pack_message(10, test_t, '<ii')  # test_t: int a, b
    pkt1 = pack_message(10, test_t, '<ii')  # test_t: int a, b
    pkt2 = pack_message(20, test2_t, '<ifc')  # test2_t: int i, float f, char c

    # Prepare buffer and parser
    buffer = bytearray(128)
    parser = YivoParser(buffer, 128)

    # Simulate multi.c buffer
    stream = bytearray(128)
    stream[0] = 34
    stream[1] = 200
    offset = 2
    for data in [pkt1, pkt2, pkt1, pkt1, pkt2]:
        stream[offset:offset+len(data)] = data
        offset += len(data)

    # Parse stream
    for b in stream:
        msgid = parser.parse(b)
        if msgid == 10:
            _, data = parser.get_payload(8, '<ii')
            print(f"test_t[{msgid}]: {data[0]} {data[1]}")
        elif msgid == 20:
            _, data = parser.get_payload(9, '<ifc')
            print(f"test2_t[{msgid}]: {data[0]} {data[1]} {data[2].decode()}")

if __name__ == "__main__":
    example_usage()