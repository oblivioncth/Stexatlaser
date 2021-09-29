#ifndef CPACK_H
#define CPACK_H

#include "../command.h"
#include "../klei/k-tex.h"

class CPack : public Command
{
//-Inner Classes--------------------------------------------------------------------------------------------------------
private:
    class ErrorCodes
    {
    //-Class Variables--------------------------------------------------------------------------------------------------
    public:
        static const ErrorCode NO_INPUT = 101;
        static const ErrorCode NO_OUTPUT = 102;
        static const ErrorCode INVALID_INPUT = 103;
        static const ErrorCode INVALID_OUTPUT = 104;
        static const ErrorCode INVALID_FORMAT = 105;
        static const ErrorCode NO_IMAGES = 106;
        static const ErrorCode CANT_READ_IMAGE = 107;
        static const ErrorCode CANT_WRITE_ATLAS = 108;
        static const ErrorCode CANT_WRITE_KEY = 109;
    };

//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Processing
    static inline const QStringList IMAGE_FORMATS = {".png"};
    static inline const QMap<QString, KTex::Header::PixelFormat> PIXEL_FORMAT_MAP = {
        {"dxt1", KTex::Header::PixelFormat::DXT1},
        {"dxt3", KTex::Header::PixelFormat::DXT3},
        {"dxt5", KTex::Header::PixelFormat::DXT5},
        {"rgb", KTex::Header::PixelFormat::RGB},
        {"rgba", KTex::Header::PixelFormat::RGBA},
    };

    // Messages
    static inline const QString MSG_INPUT_VALIDATION = "Validating input...";
    static inline const QString MSG_READ_IMAGES = "Reading input images...";
    static inline const QString MSG_CREATE_ATLAS = "Creating atlas...";
    static inline const QString MSG_CREATE_KEY = "Creating atlas key...";
    static inline const QString MSG_CREATE_TEX = "Creating TEX...";
    static inline const QString MSG_WRITE_TEX = "Writing TEX...";
    static inline const QString MSG_WRITE_KEY = "Writing atlas key...";

    // Error Messages
    static inline const QString ERR_NO_INPUT = "No input directory was provided.";
    static inline const QString ERR_NO_OUTPUT = "No output directory was provided.";
    static inline const QString ERR_INVALID_INPUT = "The provided input directory was invalid.";
    static inline const QString ERR_INVALID_OUTPUT = "The provided output directory was invalid.";
    static inline const QString ERR_INVALID_FORMAT = "The provided output pixel format was invalid.";
    static inline const QString ERR_NO_IMAGES = "The provided input directory contained no images.";
    static inline const QString ERR_CANT_READ_IMAGE = "Failed to read image %1";
    static inline const QString ERR_CANT_WRITE_ATLAS = "Failed to write output atlas %1";
    static inline const QString ERR_CANT_WRITE_KEY = "Failed to write output atlas key %1";

    // Command line option strings
    static inline const QString CL_OPT_STRAIGHT_S_NAME = "s";
    static inline const QString CL_OPT_STRAIGHT_L_NAME = "straight";
    static inline const QString CL_OPT_STRAIGHT_DESC = "Keep straight alpha channel, do not premultiply.";

    static inline const QString CL_OPT_UNOPT_S_NAME = "u";
    static inline const QString CL_OPT_UNOPT_L_NAME = "unoptimized";
    static inline const QString CL_OPT_UNOPT_DESC = "Do not generate smoothed mip-maps.";

    static inline const QString CL_OPT_FORMAT_S_NAME = "f";
    static inline const QString CL_OPT_FORMAT_L_NAME = "format";
    static inline const QString CL_OPT_FORMAT_DESC = "Pixel format to use when encoding to TEX. <" +
                                                     PIXEL_FORMAT_MAP.keys().join(" | ") + ">." +
                                                     "Defaults to DXT5.";

    static inline const QString CL_OPT_INPUT_S_NAME = "i";
    static inline const QString CL_OPT_INPUT_L_NAME = "input";
    static inline const QString CL_OPT_INPUT_DESC = "Directory containing images to pack.";

    static inline const QString CL_OPT_OUTPUT_S_NAME = "o";
    static inline const QString CL_OPT_OUTPUT_L_NAME = "output";
    static inline const QString CL_OPT_OUTPUT_DESC = "Directory in which to place the resultant atlas and key.";

    // Command line options
    static inline const QCommandLineOption CL_OPTION_STRAIGHT{{CL_OPT_STRAIGHT_S_NAME, CL_OPT_STRAIGHT_L_NAME}, CL_OPT_STRAIGHT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_UNOPT{{CL_OPT_UNOPT_S_NAME, CL_OPT_UNOPT_L_NAME}, CL_OPT_UNOPT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_FORMAT{{CL_OPT_FORMAT_S_NAME, CL_OPT_FORMAT_L_NAME}, CL_OPT_FORMAT_DESC, "format"}; // Takes value
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, "input"}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, "output"}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_STRAIGHT, &CL_OPTION_UNOPT, &CL_OPTION_FORMAT,
                                                                             &CL_OPTION_INPUT, &CL_OPTION_OUTPUT};

public:
    // Meta
    static inline const QString NAME = "pack";
    static inline const QString DESCRIPTION = "Pack a folder of images (PNG) into a TEX atlas. The input directory will be used as the name for the atlas/key, "
                                              "while the image names will be used as the element names";

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CPack(Stex& coreRef);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    const QList<const QCommandLineOption*> options();
    const QString name();

public:
    ErrorCode process(const QStringList& commandLine);
};
REGISTER_COMMAND(CPack::NAME, CPack, CPack::DESCRIPTION);

#endif // CPACK_H
