#ifndef CUNPACK_H
#define CUNPACK_H

#include "../command.h"
#include "../klei/k-tex.h"

class CUnpack : public Command
{
//-Inner Classes--------------------------------------------------------------------------------------------------------
private:
    class ErrorCodes
    {
    //-Class Variables--------------------------------------------------------------------------------------------------
    public:
        static const ErrorCode NO_INPUT = 201;
        static const ErrorCode NO_OUTPUT = 202;
        static const ErrorCode INVALID_INPUT = 203;
        static const ErrorCode INVALID_OUTPUT = 204;
        static const ErrorCode CANT_READ_KEY = 205;
        static const ErrorCode ATLAS_DOESNT_EXIST = 206;
        static const ErrorCode CANT_READ_ATALAS = 207;
        static const ErrorCode ATLAS_UNSUPPORTED = 208;
        static const ErrorCode CANT_CREATE_DIR = 209;
        static const ErrorCode CANT_WRITE_IMAGE = 210;
    };

//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_INPUT_VALIDATION = "Validating input...";
    static inline const QString MSG_READ_KEY = "Reading input atlas key...";
    static inline const QString MSG_READ_TEX = "Reading TEX...";
    static inline const QString MSG_EXTRACT_IMAGE = "Extracing primary TEX image...";
    static inline const QString MSG_FORM_ATLAS = "Forming atlas...";
    static inline const QString MSG_DEATLAS = "Deatlasing...";
    static inline const QString MSG_WRITE_IMAGES = "Writing output images...";
    static inline const QString MSG_SUCCESS = "Successfully unpacked %1 images";

    // Error Messages
    static inline const QString ERR_NO_INPUT = "No input directory was provided.";
    static inline const QString ERR_NO_OUTPUT = "No output directory was provided.";
    static inline const QString ERR_INVALID_INPUT = "The provided input atlas key was invalid.";
    static inline const QString ERR_INVALID_OUTPUT = "The provided output directory was invalid.";
    static inline const QString ERR_CANT_READ_KEY = "Failed to read atlas key %1";
    static inline const QString ERR_ATLAS_DNE = "The atlas specified by the provided atlas key does not exist (%1)";
    static inline const QString ERR_CANT_READ_ATLAS = "Failed to read atlas %1";
    static inline const QString ERR_ATLAS_NOT_SUPPORTED = "The input atlas is not supported.";
    static inline const QString ERR_CANT_CREATE_DIR = "Failed to create unpack folder %1";
    static inline const QString ERR_CANT_WRITE_IMAGE = "Failed to write output image %1";

    // Command line option strings
    static inline const QString CL_OPT_STRAIGHT_S_NAME = "s";
    static inline const QString CL_OPT_STRAIGHT_L_NAME = "straight";
    static inline const QString CL_OPT_STRAIGHT_DESC = "Specify that the alpha information within the input TEX is straight, do not de-multiply.";

    static inline const QString CL_OPT_INPUT_S_NAME = "i";
    static inline const QString CL_OPT_INPUT_L_NAME = "input";
    static inline const QString CL_OPT_INPUT_DESC = "Key of the atlas to unpack. Must be in the same directory as its atlas.";

    static inline const QString CL_OPT_OUTPUT_S_NAME = "o";
    static inline const QString CL_OPT_OUTPUT_L_NAME = "output";
    static inline const QString CL_OPT_OUTPUT_DESC = "Directory in which to place the resultant folder of unpacked images.";

    // Command line options
    static inline const QCommandLineOption CL_OPTION_STRAIGHT{{CL_OPT_STRAIGHT_S_NAME, CL_OPT_STRAIGHT_L_NAME}, CL_OPT_STRAIGHT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, "input"}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, "output"}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT, &CL_OPTION_STRAIGHT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};

public:
    // Meta
    static inline const QString NAME = "unpack";
    static inline const QString DESCRIPTION = "Unpack a TEX atlas into its component images";

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CUnpack(Stex& coreRef);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    const QList<const QCommandLineOption*> options();
    const QSet<const QCommandLineOption*> requiredOptions();
    const QString name();

public:
    ErrorCode process(const QStringList& commandLine);
};
REGISTER_COMMAND(CUnpack::NAME, CUnpack, CUnpack::DESCRIPTION);

#endif // CUNPACK_H
