"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

import cStringIO as StringIO
import struct

class blicycle_packet_t(object):
    __slots__ = ["lock", "delta", "lane", "totalLanes", "phi"]

    def __init__(self):
        self.lock = 0
        self.delta = 0.0
        self.lane = 0
        self.totalLanes = 0
        self.phi = 0.0

    def encode(self):
        buf = StringIO.StringIO()
        buf.write(blicycle_packet_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">idbbd", self.lock, self.delta, self.lane, self.totalLanes, self.phi))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = StringIO.StringIO(data)
        if buf.read(8) != blicycle_packet_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return blicycle_packet_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = blicycle_packet_t()
        self.lock, self.delta, self.lane, self.totalLanes, self.phi = struct.unpack(">idbbd", buf.read(22))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if blicycle_packet_t in parents: return 0
        tmphash = (0xeb17c27efafcc53d) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if blicycle_packet_t._packed_fingerprint is None:
            blicycle_packet_t._packed_fingerprint = struct.pack(">Q", blicycle_packet_t._get_hash_recursive([]))
        return blicycle_packet_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

