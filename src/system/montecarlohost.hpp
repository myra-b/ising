#pragma once

#ifdef QT_NO_DEBUG
    #ifndef QT_NO_DEBUG_OUTPUT
        #define QT_NO_DEBUG_OUTPUT
    #endif
#endif


#include "gui/default_parameters_widget.hpp"
#include "spinsystem.hpp"
#include "histogram.hpp"
#include "acceptance.hpp"
#include "lib/enhance.hpp"
#include "utility/logger.hpp"
#include <QDebug>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <fstream>



class MonteCarloHost
{
    BaseParametersWidget*  parameters = Q_NULLPTR;
    AcceptanceAdaptor* acceptance = nullptr;
    Spinsystem         spinsystem {};

    std::vector<double>  energies {};
    std::vector<double>  energiesSquared {};
    std::vector<double>  magnetisations {};
    std::vector<double>  magnetisationsSquared {};
    std::vector<std::vector<double>> amplitudes {};
    
protected:
    
public:
    MonteCarloHost();
    MonteCarloHost(const MonteCarloHost&) = delete;
    void operator=(const MonteCarloHost&) = delete;
    ~MonteCarloHost();
    
    void setup();
    void setParameters(BaseParametersWidget*);
    void setAcceptance(AcceptanceAdaptor*);
    void resetSpins();
    void clearRecords();
    
    const Spinsystem& getSpinsystem() const;
    
    void run(const unsigned long&, const bool EQUILMODE = false);
    
    void print_correlation(Histogram<double>&) const;
    void print_data() const;
    void print_averages() const;
    void print_amplitudes() const;
};
