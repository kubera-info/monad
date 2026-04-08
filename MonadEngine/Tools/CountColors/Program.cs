// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

using System;
using System.Drawing;

// var clArgs = Environment.GetCommandLineArgs();
Console.WriteLine("Filling gaps in a new coloring page");
Console.WriteLine("Copyright (C) 2016-2026 by Włodzimierz O. Kubera");

try
{
    if (args.Length != 3)
    {
        Console.WriteLine("Usage: CountColors <input.png> <output.png>");
        throw new System.Exception("Incorrect arguments.");
    }

    int globalIdx = 1;
    using var loaded = new Bitmap(args[1]);
    // Convert to 32bpp ARGB for efficient pixel access
    using var source = loaded.Clone(new System.Drawing.Rectangle(0, 0, loaded.Width, loaded.Height), System.Drawing.Imaging.PixelFormat.Format32bppArgb);

    for (int idxY = 0; idxY < source.Height; ++idxY)
        for (int idxX = 0; idxX < source.Width; ++idxX)
            if (IsWhite(source, idxX, idxY))
            {
                var fillColor = Color.FromArgb(0xFF, (globalIdx >> 8) & 0xFF, globalIdx & 0xFF, 0xFF);
                FloodFill(source, idxX, idxY, fillColor);
                ++globalIdx;
            }

    source.Save(args[2]);
}
catch (Exception exc)
{
    Console.Beep();
    Console.WriteLine(exc.Message + " - " + args[1]);
    Environment.ExitCode = 1;
}

static bool IsWhite(in Bitmap bmp, in int x, in int y)
{
    var c = bmp.GetPixel(x, y);
    return (c.ToArgb() & 0xFFFFFF) == (Color.White.ToArgb() & 0xFFFFFF);
}

static void FloodFill(Bitmap bmp, in int startX, in int startY, in Color newColor)
{
    // Iterative flood fill using a pixel buffer to avoid deep recursion and many GetPixel/SetPixel calls.
    var rect = new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height);
    var data = bmp.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
    try
    {
        int width = bmp.Width;
        int height = bmp.Height;
        int stride = Math.Abs(data.Stride);
        int bytes = stride * height;
        var buffer = new byte[bytes];
        System.Runtime.InteropServices.Marshal.Copy(data.Scan0, buffer, 0, bytes);

        int idx(int x, int y) => y * stride + x * 4;

        // Check starting pixel is white (compare RGB only)
        int si = idx(startX, startY);
        if (startX < 0 || startX >= width || startY < 0 || startY >= height)
            return;
        if (!(buffer[si] == 0xFF && buffer[si + 1] == 0xFF && buffer[si + 2] == 0xFF))
            return;

        byte nb = newColor.B;
        byte ng = newColor.G;
        byte nr = newColor.R;
        byte na = newColor.A;

        var stack = new System.Collections.Generic.Stack<(int x, int y)>();
        stack.Push((startX, startY));

        while (stack.Count > 0)
        {
            var (x, y) = stack.Pop();
            if (x < 0 || x >= width || y < 0 || y >= height) continue;
            int i = idx(x, y);
            // If not white (RGB), skip
            if (!(buffer[i] == 0xFF && buffer[i + 1] == 0xFF && buffer[i + 2] == 0xFF))
                continue;

            // Set new color (B,G,R,A)
            buffer[i] = nb;
            buffer[i + 1] = ng;
            buffer[i + 2] = nr;
            buffer[i + 3] = na;

            stack.Push((x - 1, y));
            stack.Push((x + 1, y));
            stack.Push((x, y - 1));
            stack.Push((x, y + 1));
        }

        System.Runtime.InteropServices.Marshal.Copy(buffer, 0, data.Scan0, bytes);
    }
    finally
    {
        bmp.UnlockBits(data);
    }
}
