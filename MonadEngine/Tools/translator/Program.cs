// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;

Console.WriteLine(args.Length > 1 ? "Indexing art's colors for: " + args[1] : "Indexing art's colors");

try
{
    if (args.Length != 2)
        throw new Exception("Incorrect arguments. Usage: translator <input.png> <out.idx>");
    var header = DdsReader.ReadHeaders();
    using (var writer = new BinaryWriter(File.Open(args[1], FileMode.Create)))
    {
        if (!File.Exists(args[0]))
            throw new Exception("Input file does not exist.");

        using (var loaded = new Bitmap(args[0]))
        using (var source = loaded.Clone(new Rectangle(0, 0, loaded.Width, loaded.Height), System.Drawing.Imaging.PixelFormat.Format32bppArgb))
        {
            var colorDictionary = new Dictionary<Color, int>()
                        {
                            {
                                Color.FromArgb(255, 0, 0, 0), 0
                            }
                        };

            header.dwWidth = (uint)source.Width;
            header.dwHeight = (uint)source.Height;

            DdsWriter.WriteHeaders(writer, header);

            var rect = new Rectangle(0, 0, source.Width, source.Height);
            var data = source.LockBits(
                rect,
                System.Drawing.Imaging.ImageLockMode.ReadOnly,
                System.Drawing.Imaging.PixelFormat.Format32bppArgb
                );
            try
            {
                int width = source.Width;
                int height = source.Height;
                int stride = Math.Abs(data.Stride);
                int bytes = stride * height;
                int bitmapSize = height * width;
                var buffer = new byte[bytes];
                Debug.Assert(width * 4 == stride, "Inconsistent color's data.");
                Marshal.Copy(data.Scan0, buffer, 0, bytes);

                for (int y = 0; y < bitmapSize; ++y)
                {
                    int i = y * 4; // B,G,R,A
                    byte b = buffer[i];
                    byte g = buffer[i + 1];
                    byte r = buffer[i + 2];
                    byte a = buffer[i + 3];

                    var tmpColor = Color.FromArgb(a, r, g, b);

                    if (colorDictionary.TryGetValue(tmpColor, out int val))
                        writer.Write((UInt16)val);
                    else
                    {
                        UInt16 Cnt = (UInt16)(colorDictionary.Count << 6);
                        colorDictionary.Add(tmpColor, Cnt);
                        writer.Write(Cnt);
                    }
                }
            }
            finally
            {
                source.UnlockBits(data);
            }

            Console.WriteLine("Art's indices: " + colorDictionary.Count.ToString());
        }
    }
}
catch (Exception exc)
{
    Console.Beep();
    Console.WriteLine(exc.Message + " - " + Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "Translator.exe - " + (args.Length > 0 ? args[0] : string.Empty));
    Environment.ExitCode = 1;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct DDS_PIXELFORMAT
{
    public uint dwSize;        // must be 32
    public uint dwFlags;       // DDPF_* flags
    public uint dwFourCC;      // 0x30315844 = "DX10"
    public uint dwRGBBitCount;
    public uint dwRBitMask;
    public uint dwGBitMask;
    public uint dwBBitMask;
    public uint dwABitMask;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct DDS_HEADER
{
    public uint dwSize;              // must be 124
    public uint dwFlags;             // DDSD_* flags
    public uint dwHeight;
    public uint dwWidth;
    public uint dwPitchOrLinearSize;
    public uint dwDepth;             // for volume textures
    public uint dwMipMapCount;
    public fixed uint dwReserved1[11];     // unused
    public DDS_PIXELFORMAT ddspf;
    public uint dwCaps;
    public uint dwCaps2;
    public uint dwCaps3;
    public uint dwCaps4;
    public uint dwReserved2;
    public DDS_HEADER_DX10 dx10;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct DDS_HEADER_DX10
{
    public uint dxgiFormat;
    public D3D10_RESOURCE_DIMENSION resourceDimension;
    public uint miscFlag;    // DDS_RESOURCE_MISC_TEXTURECUBE = 0x4
    public uint arraySize;
    public uint miscFlags2;  // alpha mode
}

// Useful enums
public enum D3D10_RESOURCE_DIMENSION : uint
{
    Unknown = 0,
    Buffer = 1,
    Texture1D = 2,
    Texture2D = 3,
    Texture3D = 4,
}

// DDS_HEADER flags
public static class DdsReader
{
    private const uint MagicNumber = 0x20534444; // "DDS "
    private const uint FourCC_DX10 = 0x30315844; // "DX10"

    public static DDS_HEADER ReadHeaders()
    {
        using var fs = File.OpenRead(Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "\\..\\sea.dds-template");
        using var br = new BinaryReader(fs);

        uint magic = br.ReadUInt32();
        if (magic != MagicNumber)
            throw new InvalidDataException("Not a DDS file.");

        var header = ReadStruct<DDS_HEADER>(br);

        return header;
    }

    private static unsafe T ReadStruct<T>(BinaryReader br) where T : unmanaged
    {
        var bytes = br.ReadBytes(sizeof(T));
        fixed (byte* ptr = bytes)
            return *(T*)ptr;
    }
}

public static class DdsWriter
{
    public static void WriteHeaders(BinaryWriter bw,
                                           in DDS_HEADER header)
    {
        bw.Write(0x20534444u);
        WriteStruct(bw, header);
    }

    private static unsafe void WriteStruct<T>(BinaryWriter bw, in T value)
    where T : unmanaged
    {
        fixed (T* ptr = &value)
        {
            var span = new ReadOnlySpan<byte>(ptr, sizeof(T));
            bw.Write(span);
        }
    }
}