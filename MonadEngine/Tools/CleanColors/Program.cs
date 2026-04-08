// ##########################################################################
// ### Copyright © 2016-2025 by Włodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

using System;
using System.Drawing;

Console.WriteLine("Cleaning gaps in a new coloring page");
Console.WriteLine("Copyright (C) 2016-2026 by Włodzimierz O. Kubera");

try
{
    if (args.Length != 2)
    {
        Console.WriteLine("Usage: CleanColors <input.png> <output.png>");
        throw new System.Exception("Incorrect arguments.");
    }

    using var loaded = new Bitmap(args[0]);
    using var source = loaded.Clone(new System.Drawing.Rectangle(0, 0, loaded.Width, loaded.Height), System.Drawing.Imaging.PixelFormat.Format32bppArgb);

    var rect = new System.Drawing.Rectangle(0, 0, source.Width, source.Height);
    var data = source.LockBits(rect, System.Drawing.Imaging.ImageLockMode.ReadWrite, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
    try
    {
        int width = source.Width;
        int height = source.Height;
        int stride = Math.Abs(data.Stride);
        int bytes = stride * height;
        var buffer = new byte[bytes];
        System.Runtime.InteropServices.Marshal.Copy(data.Scan0, buffer, 0, bytes);

        for (int y = 0; y < height; ++y)
        {
            int row = y * stride;
            for (int x = 0; x < width; ++x)
            {
                int i = row + x * 4; // B,G,R,A
                bool isWhite = buffer[i] == 255 && buffer[i + 1] == 255 && buffer[i + 2] == 255;
                bool isBlack = buffer[i] == 0 && buffer[i + 1] == 0 && buffer[i + 2] == 0;
                if (!isWhite && !isBlack)
                {
                    buffer[i] = 255;
                    buffer[i + 1] = 255;
                    buffer[i + 2] = 255;
                    buffer[i + 3] = 255;
                }
            }
        }

        System.Runtime.InteropServices.Marshal.Copy(buffer, 0, data.Scan0, bytes);
    }
    finally
    {
        source.UnlockBits(data);
    }

    source.Save(args[1]);
}
catch (Exception exc)
{
    Console.Beep();
    Console.WriteLine(exc.Message + " - " + (args.Length > 1 ? args[1] : string.Empty));
    Environment.ExitCode = 1;
}