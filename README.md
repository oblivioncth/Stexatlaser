# Stexatlaser (Stex)
<img align="left" src="https://i.imgur.com/LCBz647.png" width=25%>

Stexatlaser (*stex-atlaser*, a play on 'spectacular'), or simply Stex, is a simple tool for generating a (Klei) TEX format atlas and its key from a simple folder structure with no external dependencies required.

It uses an implementation of the MaxRects algorithm to efficiently pack each input element image into a larger atlas image, with as little wasted space as possible. Some empty space is inevitable given that atlases must have power-of-two dimensions.

It can also reverse the process and extract element images from an atlas using its key.


## Compatibility
While this tool was written with Don't Starve Together in mind, it should work with any Klei TEX file as long as the format is not significantly different. There are still a few unknowns concerning format interpretation with even DST, but they do not impact the performance of this tool and will be addressed in the event they're determined to be relevant.


## Usage
Stex uses the following syntax scheme:

    stex <global options> [command] <command options>
The order of switches within each options section does not matter.


### Basic Usage
*NOTE:* For technical reasons, Stex currently only supports PNG for input/output images

**Creating an Atlas**
To create an atlas/key, start by setting up an input folder with the following structure:

    .atlas_name
    |-- element1_name.png
    |-- element2_name.png
    |-- element3_name.png
    |-- ...
As implied by the above, the folder name will be used as the atlas/key name, and the file names (without extension) of each image will be used as their respective element names. All sub-folders and other files will be ignored.

Then run Stex with the **pack** command:

    stex pack -i "X:\Path\To\Input\Directory" -o "X:\Path\To\Output\Directory"

When finished, the resultant atlas.tex and key.xml files will be placed in the specified output directory. You can then refer to each element by its name in LUA without concerning yourself where they actually were placed within the atlas.

If you want to see the element arrangement, try viewing the output with a [TEX viewer](https://github.com/oblivioncth/dont-starve-tools/releases/).

**Extracting an Atlas**
To extract an atlas, run Stex with the **unpack** command:
 
    stex unpack -i "X:\Path\To\Input\key.xml" -o "X:\Path\To\Output\Directory"

The key's corresponding atlas, which must be located alongside it, will be read automatically using the name specified within the key.

When finished, a subfolder with the name of the atlas will be created within the specified output directory that contains each individual element as a separate image. This results in the same structure used as input when packing an atlas.


### Advance Usage
- If the alpha channel was pre-multiplied when a given TEX atlas was created, the **-s** switch must be passed to **unpack** for the images to be recovered correctly. This is handled automatically for atlas/key pairs that were generated with Stex's pack command, as detailed in the "Additional Features" section 
- See the following section for more detailed options/modes.


## All Commands/Options
The recommended way to use all switches is to use their short form when the value for the switch has no spaces:

    -i C:/Users/Name/Desktop/input

and the long form when the value does have spaces

    --input="C:/Textures/Don't Starve Together/input.xml"
though this isn't required as long as quotation and space use is carefully employed.


### Global Options:
 -  **-h | --help | -?:** Prints usage information
 -  **-v | --version:** Prints the current version of the tool
 
 
### Commands:
**pack** - Pack  a  folder  of  images  (PNG)  into  a  TEX  atlas.  The  input  directory  will  be  used  as  the  name  for  the  atlas/key, while  the  image  names  will  be  used  as  the  element  names

Options:
 -  **-i | --input:** Directory  containing  images  to  pack
 -  **-o | --output:** Directory  in  which  to  place  the  resultant  atlas  and  key
 -  **-f | --format:** Pixel  format  to  use  when  encoding  to  TEX.  The valid options are <dxt1 | dxt3 | dxt5 | rgb | rgba>. Defaults  to  DXT5
 -  **-u | --unoptimized:** Do  not  generate  smoothed  mipmaps
 -  **-s | --straight:** Keep  straight  alpha  channel,  do  not  pre-multiply

Requires:
**-i** and **-o** 

--------------------------------------------------------------------------------

**unpack** - Unpack  a  TEX  atlas  into  its  component  images

Options:
 -  **-i | --input:** Key  of  the  atlas  to  unpack.  Must  be  in  the  same  directory  as  its  atlas
 -  **-o | --output:** Directory  in  which  to  place  the  resultant  folder  of  unpacked  images
 -  **-s | --straight:** Specify  that  the  alpha  information  within  the  input  TEX  is  straight,  do  not  de-multiply

Requires:
**-i** and **-o** 

Notes: 
Because Klei TEX atlas keys use relative coordinates and converting to/from them incurs floating-point inaccuracies, there are some edge cases where the dimensions of unpacked images may differ very slightly from the originals used to create the TEX; however, this is generally not the case.

Still, for this reason it is recommended to keep original copies of your textures and not rely on the TEX version as your only copy.

--------------------------------------------------------------------------------
 

## Exit Codes
Once stex has finished executing an exit code is reported that indicates the "error status" of the program, which can be useful for recording/determining issues. The exit code can be obtained by running the application in the following manner:

    start /wait CLIFp.exe [parameters]
    echo %errorlevel%

| Value | Code               | Description                                                              |
|-------|--------------------|--------------------------------------------------------------------------|
| 0     | NO_ERR             | The application completed successfully                                   |
| 1     | INVALID_ARGS       | The arguments provided were not recognized or were formatted incorrectly |
| 2     | NO_INPUT           | The required input directory was not provided                            |
| 3     | NO_OUTPUT          | The required output directory was not provided                           |
| 4     | INVALID_INPUT      | The provided input directory was invalid                                 |
| 5     | INVALID_OUTPUT     | The provided output directory was invalid                                |
| 101   | INVALID_FORMAT     | The provided pixel format was invalid                                    |
| 102   | NO_IMAGES          | The provided input directory contains no images                          |
| 103   | CANT_READ_IMAGE    | Failed to read an input image                                            |
| 104   | CANT_WRITE_ATLAS   | Failed to write the output atlas                                         |
| 105   | CANT_WRITE_KEY     | Failed to write the output atlas key                                     |
| 201   | CANT_READ_KEY      | Failed to read the input atlas key                                       |
| 202   | ATLAS_DOESNT_EXIST | The TEX file specified within the input atlas key could not be found     |
| 203   | CANT_READ_ATALAS   | Failed to read the input atlas                                           |
| 204   | ATLAS_UNSUPPORTED  | The provided input atlas uses an unknown format                          |
| 205   | CANT_CREATE_DIR    | Failed to create the element output directory                            |
| 206   | CANT_WRITE_IMAGE   | Failed to write an output image                                          |


## Additional Information
**Automatic Pre-multiplied Alpha Handling**
A small shortcoming of the TEX format is that it doesn't store whether or not its image data is using pre-multiplied alpha (unless that's the purpose of one of the two unknown flags), and so one needs to somehow otherwise know if this is the case and manually specify that the alpha needs to be de-multiplied when using tools that handle TEX files (they often just assume they need to). To circumvent this, any TEX atlases created with Stex's **pack** command will have an extra entry in their key that records this property. This value is then subsequently read and utilized when extracting that same TEX using Stex's **unpack** command. If the value isn't present within a key then an input atlas is assumed to be using pre-multiplied alpha unless the **-s** switch is used with the unpack command/

Simply put, if you always pack and unpack your multi-image atlases with Stex you will never have to worry about this.

Although this breaks the "standard" for atlas keys, since they are just XML files the games parser will simply ignore this extra element and it therefore causes no issues and maintains compatibility.

**Atlas Key Element Extensions**
Although in a practical sense they shouldn't be needed, some atlas elements require the extension ".tex" to function properly due to the exact implementation of some Klei scripts. The elements themselves don't actually refer to files and instead are just labels for the images within a TEX file, which makes this requirement a bit award and sometimes confusing, but nonetheless Stex ensures compliance with this annoyance. Any input images that don't already end with ".tex" (before their actual extension) will have the extension appended to the element name that their filename becomes.

*Example:*
| Image Filename    | Resultant Element Name |
|-------------------|------------------------|
| texture01.tex.png | texture01.tex          |
| texture02.png     | texture02.tex          |

This extension will be removed during filename assignment when unpacking an atlas.


## Source
This tool was written in C++ 17 targeting Windows and has the following dependencies:

 - Qt 5.15.2
 - Windows 10 SDK 10.0.19041.0 or later
 - MSVC2019 v142 or later
 - Qxtended (my own personal Qt based library, see below)
 - libsquish 1.15

Because of the allowances by various licenses and overall small footprints, this repository comes with everything needed to build for the target platform, other than a build of Qt itself. This includes pre-built static libs of Qxtended (source code available upon request), and libquish for Debug/Release and x86/x64.

This project is configured for the qmake build system and can be built using it directly or through Qt Creator via the included .pro file.

Other platforms are possible to compile for with some modifications.


## Klei TEX Format
When creating this tool I couldn't find any documentation on the Klei TEX format and had to use other existing tools' code as reference. I have provided my interpretation here for convivence:

    KTEX FORMAT
    ============
    0x00 - HEADER (pre or post caves update)
    0x08 - MIMAP_METADATA[Mipmap_Count]
    0x^^ - MIMAP_DATA[Mipmap_Count]
    
    HEADER (pre-caves update)
    ------
    0x00 - uint8[4]: Magic Number "KTEX"
    0x04 - uint32: Specifications
    >> Specifications
       --------------
       b0:2 - uint3: Platform
       b3:5 - uint3: Pixel Format
       b6:8 - uint3: Texture Type
       b9:12 - uint4: Mipmap Count
       b13 - uint1: Flags (Unknown)
       b14:31 - uint18: *padding (all high)*
       
    HEADER (post-caves update)
    ------
    0x00 - uint8[4]: Magic Number "KTEX"
    0x04 - uint32: Specifications
    >> Specifications
       --------------
       b0:3 - uint4: Platform
       b4:8 - uint5: Pixel Format
       b9:12 - uint4: Texture Type
       b13:17 - uint5: Mipmap Count
       b18:19 - uint2: Flags (Unknown)
       b20:31 - uint12: *padding (all high)*
       
    MIPMAP_METADATA
    ---------------
    0x00 - uint16: Width
    0x02 - uint16: Height
    0x04 - uint16: Pitch
    0x06 - uint32: Data Size
    
    MIMAP_DATA
    ----------
    0x00 - uint8[Data_Size]: Image Data

This tools defaults both flags in the newer header spec (the only one used when writing) to high. If anyone knows the purpose of these flags I'd be grateful if you could share it with me.