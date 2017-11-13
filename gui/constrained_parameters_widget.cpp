#include "constrained_parameters_widget.hpp"

#define CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL \
    Q_CHECK_PTR(interactionSpinBox); \
    Q_CHECK_PTR(temperatureSpinBox); \
    Q_CHECK_PTR(heightSpinBox);      \
    Q_CHECK_PTR(widthSpinBox);       \
    Q_CHECK_PTR(stepsEquilSpinBox);  \
    Q_CHECK_PTR(stepsProdSpinBox);   \
    Q_CHECK_PTR(printFreqSpinBox);   \
    Q_CHECK_PTR(randomiseBtn);       \
    Q_CHECK_PTR(filenameLineEdit);   \
    Q_CHECK_PTR(startValueSpinBox);  \
    Q_CHECK_PTR(stepValueSpinBox);   \
    Q_CHECK_PTR(stopValueSpinBox);   \
    Q_CHECK_PTR(ratioSpinBox);    



ConstrainedParametersWidget::ConstrainedParametersWidget(QWidget* parent)
 : BaseParametersWidget(parent)
{
    qDebug() << __PRETTY_FUNCTION__;

    ratioSpinBox = new QDoubleSpinBox(this);

    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL
    setup();

}



ConstrainedParametersWidget::~ConstrainedParametersWidget()
{
    qDebug() << __PRETTY_FUNCTION__;
}


void ConstrainedParametersWidget::setup()
{
    qDebug() << __PRETTY_FUNCTION__;
    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL

    setMinimumWidth(300);
    
    // layout of this widget
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignVCenter);

    
    // add Box with Line Edits
    mainLayout->addWidget(createSystemBox());
    mainLayout->addWidget(randomiseBtn);
    mainLayout->addWidget(createEquilBox());
    mainLayout->addWidget(createProdBox());
    mainLayout->addWidget(createOutputBox());
    mainLayout->addWidget(createAdvancedOptionsBox());
    setDefault();
    
    // add randomiseBtn
    randomiseBtn->setFocusPolicy(Qt::NoFocus);
    connect(randomiseBtn, &QPushButton::clicked, this, &ConstrainedParametersWidget::randomiseSystem);
    mainLayout->addWidget(randomiseBtn);
    
    // https://stackoverflow.com/a/16795664
    connect( interactionSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    connect( temperatureSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    connect( stepsEquilSpinBox , static_cast<void (QtLongLongSpinBox::*)(qlonglong)>(&QtLongLongSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    connect( stepsProdSpinBox  , static_cast<void (QtLongLongSpinBox::*)(qlonglong)>(&QtLongLongSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    connect( printFreqSpinBox  , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    
    connect( ratioSpinBox      , static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConstrainedParametersWidget::criticalValueChanged );

    connect( heightSpinBox     , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), widthSpinBox, &QSpinBox::setValue );
    connect( widthSpinBox      , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), heightSpinBox, &QSpinBox::setValue );
    connect( heightSpinBox     , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    connect( widthSpinBox      , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ConstrainedParametersWidget::valueChanged );
    connect( heightSpinBox     , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ConstrainedParametersWidget::criticalValueChanged );
    connect( widthSpinBox      , static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ConstrainedParametersWidget::criticalValueChanged );
    
    adjustSize();
    setLayout(mainLayout);

}




QGroupBox* ConstrainedParametersWidget::createSystemBox()
{
    qDebug() << __PRETTY_FUNCTION__;
    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL

    // the group
    QGroupBox* labelBox = new QGroupBox("System parameters");

    // default texts for LineEdits
    interactionSpinBox->setMinimum(-3);
    interactionSpinBox->setMaximum(3);
    interactionSpinBox->setDecimals(1);
    interactionSpinBox->setSingleStep(0.5);
    interactionSpinBox->setMinimumWidth(40);
    interactionSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ratioSpinBox->setMinimum(0.1);
    ratioSpinBox->setMaximum(0.9);
    ratioSpinBox->setSingleStep(0.1);
    ratioSpinBox->setDecimals(1);
    ratioSpinBox->setMinimumWidth(40);
    ratioSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    temperatureSpinBox->setMinimum(0.1);
    temperatureSpinBox->setMaximum(10);
    temperatureSpinBox->setDecimals(1);
    temperatureSpinBox->setSingleStep(0.1);
    temperatureSpinBox->setMinimumWidth(40);
    temperatureSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    heightSpinBox->setMinimum(2);
    heightSpinBox->setMaximum(300);
    heightSpinBox->setSingleStep(2);
    heightSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    widthSpinBox->setMinimum(2);
    widthSpinBox->setMaximum(300);
    widthSpinBox->setSingleStep(2);
    widthSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    // the layout
    QFormLayout* formLayout = new QFormLayout();
    // align content
    formLayout->setLabelAlignment(Qt::AlignLeft);
    // add Line Edits
    formLayout->addRow("width",widthSpinBox);
    formLayout->addRow("height",heightSpinBox);
    formLayout->addRow("interaction strength J",interactionSpinBox);
    formLayout->addRow("temperature T",temperatureSpinBox);
    formLayout->addRow("ratio B to A [0.1:0.9]",ratioSpinBox);

    // set group layout
    labelBox->setLayout(formLayout);

    return labelBox;
}


QGroupBox* ConstrainedParametersWidget::createEquilBox()
{
    qDebug() << __PRETTY_FUNCTION__;
    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL

    // the group
    QGroupBox* labelBox = new QGroupBox("Equilibration parameters");
    
    // default texts for LineEdits
    stepsEquilSpinBox->setMinimum(0);
    stepsEquilSpinBox->setMaximum(std::numeric_limits<qlonglong>::max());
    stepsEquilSpinBox->setSingleStep(1000);
    stepsEquilSpinBox->setMinimumWidth(100);
    stepsEquilSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // the layout
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignVCenter);
    formLayout->addRow("equilibration steps",stepsEquilSpinBox);

    // set group layout
    labelBox->setLayout(formLayout);

    return labelBox;
}



QGroupBox* ConstrainedParametersWidget::createProdBox()
{
    qDebug() << __PRETTY_FUNCTION__;
    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL

    // the group
    QGroupBox* labelBox = new QGroupBox("Production parameters");

    // default texts for LineEdits
    printFreqSpinBox->setMinimum(0);
    printFreqSpinBox->setMaximum(1000000);
    printFreqSpinBox->setSingleStep(10);
    printFreqSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    stepsProdSpinBox->setMinimum(0);
    stepsProdSpinBox->setMaximum(std::numeric_limits<qlonglong>::max());
    stepsProdSpinBox->setSingleStep(1000);
    stepsProdSpinBox->setMinimumWidth(100);
    stepsProdSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // the layout
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignHCenter);
    formLayout->addRow("production steps",stepsProdSpinBox);
    formLayout->addRow("sample every ...", printFreqSpinBox); 


    // set group layout
    labelBox->setLayout(formLayout);

    return labelBox;
}



void ConstrainedParametersWidget::setReadOnly(bool flag)
{
    qDebug() << __PRETTY_FUNCTION__;
    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL

    interactionSpinBox->setReadOnly(flag);
    temperatureSpinBox->setReadOnly(flag);
    heightSpinBox->setReadOnly(flag);
    widthSpinBox->setReadOnly(flag);
    stepsEquilSpinBox->setReadOnly(flag);
    stepsProdSpinBox->setReadOnly(flag);
    printFreqSpinBox->setReadOnly(flag);
    randomiseBtn->setEnabled(!flag);
    filenameLineEdit->setReadOnly(flag);
    advancedComboBox->setEditable(!flag);
    startValueSpinBox->setReadOnly(flag);
    stepValueSpinBox->setReadOnly(flag);
    stopValueSpinBox->setReadOnly(flag);
    ratioSpinBox->setReadOnly(flag);
}




void ConstrainedParametersWidget::setDefault()
{
    qDebug() << __PRETTY_FUNCTION__;
    CONSTRAINED_PARAMETERS_WIDGET_ASSERT_ALL

    heightSpinBox->setValue(4);
    widthSpinBox->setValue(4);
    interactionSpinBox->setValue(1.0);
    temperatureSpinBox->setValue(2.0);
    stepsEquilSpinBox->setValue(10000);
    stepsProdSpinBox->setValue(10000);
    printFreqSpinBox->setValue(10);
    filenameLineEdit->setText("ising");
    advancedComboBox->setCurrentIndex(0);
    startValueSpinBox->setValue(0);
    stepValueSpinBox->setValue(0.1);
    stopValueSpinBox->setValue(0);
    ratioSpinBox->setValue(0.5);
}




double ConstrainedParametersWidget::getMagnetic() const
{
    return 0;
}

                
double ConstrainedParametersWidget::getRatio() const
{
    Q_CHECK_PTR(ratioSpinBox);
    return ratioSpinBox->value();
}


bool ConstrainedParametersWidget::getConstrained() const
{
    return true;
}

void ConstrainedParametersWidget::setAdvancedValue(const double& value)
{
    // add code
}
                
                