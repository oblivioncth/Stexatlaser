#ifndef UNTEX_COMMAND_H
#define UNTEX_COMMAND_H

// Qx Includes
#include <qx/io/qx-ioopreport.h>

// Project Includes
#include "command.h"

class QX_ERROR_TYPE(UntexCommandError, "UntexCommandError", 1212)
{
    friend class UntexCommand;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        CantWriteImage
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {CantWriteImage, u"Failed to write output image."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    UntexCommandError(Type t = NoError, const QString& s = {}, const QString& d = {});

//-Instance Functions-------------------------------------------------------------
public:
    bool isValid() const;
    Type type() const;
    QString specific() const;

private:
    Qx::Severity deriveSeverity() const override;
    quint32 deriveValue() const override;
    QString derivePrimary() const override;
    QString deriveSecondary() const override;
    QString deriveDetails() const override;
};

class KTex;

class UntexCommand : public Command
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_READ_TEX = u"Reading TEX..."_s;
    static inline const QString MSG_TEX_INFO =  u"TEX Info:\n%1"_s;
    static inline const QString MSG_EXTRACT_IMAGE = u"Extracting primary TEX image..."_s;

    // Command line option strings
    static inline const QString CL_OPT_STRAIGHT_S_NAME = u"s"_s;
    static inline const QString CL_OPT_STRAIGHT_L_NAME = u"straight"_s;
    static inline const QString CL_OPT_STRAIGHT_DESC = u"Specify that the alpha information within the input TEX is straight, do not de-multiply."_s;

protected:
    // Messages
    static inline const QString MSG_INPUT_VALIDATION = u"Validating input..."_s;

    // Output
    static inline const QString STD_OUTPUT_EXT = u"png"_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_STRAIGHT{{CL_OPT_STRAIGHT_S_NAME, CL_OPT_STRAIGHT_L_NAME}, CL_OPT_STRAIGHT_DESC}; // Boolean option

    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_STRAIGHT};

public:
    // Meta
    static inline const QString NAME = u"untex-command"_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    UntexCommand(Stex& coreRef);

//-Destructor----------------------------------------------------------------------------------------------------------
public:
    virtual ~UntexCommand() = default;

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    virtual QList<const QCommandLineOption*> options() const override;
    Qx::IoOpReport readTex(KTex& tex, const QString& path) const;
    QImage extractImage(const KTex& tex, bool forceStraight = false) const;
    UntexCommandError writeImage(const QImage& image, const QString& path) const;
};

#endif // UNTEX_COMMAND_H
