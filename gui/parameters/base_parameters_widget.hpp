#pragma once


#ifdef QT_NO_DEBUG
    #ifndef QT_NO_DEBUG_OUTPUT
        #define QT_NO_DEBUG_OUTPUT
    #endif
#endif

// #include "long_qspinbox.hpp"
#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QtDebug>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QSize>
#include <limits>
#include <cmath>



class BaseParametersWidget : public QWidget
{
    Q_OBJECT
    
public:
    ~BaseParametersWidget();
    
    unsigned int getHeight() const;
    unsigned int getWidth() const;
    void         setHeight(const unsigned int);
    void         setWidth(const unsigned int);
    double getInteraction() const;
    double getTemperature() const;
    unsigned long getStepsEquil() const;
    unsigned long getStepsProd() const;
    unsigned int  getPrintFreq() const;
    std::string getFileKey() const;

    virtual double getMagnetic() const = 0;
    virtual double getRatio() const = 0;
    virtual bool   getWavelengthPattern() const = 0;
    virtual int    getWavelength() const = 0;
    virtual bool   getConstrained() const = 0;
    virtual double getStartValue() const = 0;
    virtual double getStopValue() const = 0;
    virtual double getStepValue() const = 0;
    virtual bool   getAdvancedRandomise() const = 0;
    
    virtual void setAdvancedValue(const double) = 0;
    
public slots:
    virtual void setReadOnly(bool) = 0;
    virtual void setDefault() = 0;
    
signals:
    void valueChanged(); 
    void criticalValueChanged();
    void randomise();
    
protected:
    explicit BaseParametersWidget(QWidget* parent = Q_NULLPTR);
    BaseParametersWidget(const BaseParametersWidget&) = delete;
    void operator=(const BaseParametersWidget&) = delete;

    void randomiseSystem();
    QGroupBox* createOutputBox();
    virtual QGroupBox* createSystemBox() = 0;
    virtual QGroupBox* createEquilBox() = 0;
    virtual QGroupBox* createProdBox() = 0;
    virtual QGroupBox* createAdvancedOptionsBox() = 0;
    
    // Line edits
    QSpinBox* heightSpinBox = new QSpinBox(this);
    QSpinBox* widthSpinBox = new QSpinBox(this);
    QDoubleSpinBox* interactionSpinBox = new QDoubleSpinBox(this);
    QDoubleSpinBox* temperatureSpinBox = new QDoubleSpinBox(this);
    QSpinBox* stepsEquilSpinBox = new QSpinBox(this);
    QSpinBox* stepsEquilExponentSpinBox = new QSpinBox(this);
    QSpinBox* stepsProdSpinBox = new QSpinBox(this);
    QSpinBox* stepsProdExponentSpinBox = new QSpinBox(this);
    QSpinBox* printFreqSpinBox = new QSpinBox(this);

    QLineEdit* filenameLineEdit = new QLineEdit(this);
    
    // Buttons
    QPushButton* randomiseBtn = new QPushButton("Randomise spins", this);
};
