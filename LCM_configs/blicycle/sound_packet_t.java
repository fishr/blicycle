/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package blicycle;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class sound_packet_t implements lcm.lcm.LCMEncodable
{
    public short sound_id;
    public boolean newSound;
    public boolean solo;
    public byte fade;
 
    public sound_packet_t()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0xcdfad2258b15661eL;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(blicycle.sound_packet_t.class))
            return 0L;
 
        classes.add(blicycle.sound_packet_t.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeShort(this.sound_id); 
 
        outs.writeByte( this.newSound ? 1 : 0); 
 
        outs.writeByte( this.solo ? 1 : 0); 
 
        outs.writeByte(this.fade); 
 
    }
 
    public sound_packet_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public sound_packet_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static blicycle.sound_packet_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        blicycle.sound_packet_t o = new blicycle.sound_packet_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.sound_id = ins.readShort();
 
        this.newSound = ins.readByte()!=0;
 
        this.solo = ins.readByte()!=0;
 
        this.fade = ins.readByte();
 
    }
 
    public blicycle.sound_packet_t copy()
    {
        blicycle.sound_packet_t outobj = new blicycle.sound_packet_t();
        outobj.sound_id = this.sound_id;
 
        outobj.newSound = this.newSound;
 
        outobj.solo = this.solo;
 
        outobj.fade = this.fade;
 
        return outobj;
    }
 
}

