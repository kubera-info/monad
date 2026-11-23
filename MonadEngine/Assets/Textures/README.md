# Textures - Objects
### The Monad(a) Engine's assets
This project converts common **images** to **textures** in **DDS** formats.
The powershell script is used to process folders of images into DDS textures.
Each output texture in one folder has the same format.
If there are required **different types** of **DDS**, they should be in **different folders**.
**Textures** are only build if they are not created yet or the source image is newer than the texture
(simple like in Makefiles).