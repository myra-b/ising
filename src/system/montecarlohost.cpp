#include "montecarlohost.hpp"



void MonteCarloHost::run(const unsigned long& steps, const bool EQUILMODE)
{
    qDebug() << __PRETTY_FUNCTION__;        // diese beiden Zeilen bitte 
    Q_CHECK_PTR(parameters);                // einfach stehen lassen und ignorieren


     /* Aufgabe 1.7:
     *
     * input:    steps: Anzahl MC Steps die durchgeführt werden sollen, 
     *           EQUILMODE: Modus des Runs
     * return:   /
     * Funktion: Durchführung der MC Schritte, Akzeptanz/Rückweisung der 
     *           Schritte nach dem Metropoliskriterium.
     *           Bei EQUILMODE = false: Speichern der aktuellen Werte von 
     *           Hamiltonian und Magnetisierung nach Durchführung der Schritte 
     *           durch anhängen an die Membervariablen "energies" und "magnetisations".
     */

    double energy_old;
    double energy_new;
    
    for(unsigned int t=0; t<steps; ++t)   
    {
        // flip spin:
        energy_old = spinsystem.getHamiltonian();
        spinsystem.flip();
        energy_new = spinsystem.getHamiltonian();
    
        // check metropolis criterion:
        if( ! acceptance(energy_old, energy_new, getTemperature()) )
        {
            spinsystem.flip_back(); 
            isingDEBUG("mc: " << "move rejected, new H would have been: " << energy_new)
        }
        else
        {
            isingDEBUG("mc: " << "move accepted, new H: " << energy_new)
            isingDEBUG(spinsystem.getStringOfSystem())
        }
    }
    
    if( !EQUILMODE )
    {
        energies.push_back(spinsystem.getHamiltonian());
        magnetisations.push_back(spinsystem.getMagnetisation());
    }

}



bool MonteCarloHost::acceptance(const double Eold, const double Enew, const double temperature)
{
    
    #ifndef NDEBUG
        double random = enhance::randomDouble(0.0, 1.0);
        double condition = std::exp(-(Enew-Eold)/temperature);
        isingDEBUG("mc: " << "random = " << random << ", exp(-(energy_new-energy_old)/temperature) = " << condition)
        return random < condition ? true : false;
    #endif

    return enhance::randomDouble(0.0, 1.0) < std::exp(-(Enew-Eold)/temperature) ? true : false;
}




/*
 * DER HIER FOLGENDE TEIL DER KLASSE IST NICHT RELEVANT FUER 
 * DIE IMPLEMENTIERUNGSAUFGABEN UND KANN IGNORIERT WERDEN 
 */

double MonteCarloHost::getTemperature() const 
{ 
    Q_CHECK_PTR(parameters); 
    return parameters->getTemperature(); 
}


MonteCarloHost::MonteCarloHost()
{
    qDebug() << __PRETTY_FUNCTION__;
}


MonteCarloHost::~MonteCarloHost()
{
    qDebug() << __PRETTY_FUNCTION__;
}


const Spinsystem& MonteCarloHost::getSpinsystem() const
{
    qDebug() << __PRETTY_FUNCTION__;

    return spinsystem;
}


void MonteCarloHost::setParameters(BaseParametersWidget* prms)
{
    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(prms);
    parameters = prms;
    Q_CHECK_PTR(parameters);
}


void MonteCarloHost::setup()
{
    qDebug() << __PRETTY_FUNCTION__;
    
    Q_CHECK_PTR(parameters);
    
    spinsystem.setParameters(parameters);
    spinsystem.setup();
    
    clearRecords();
}


void MonteCarloHost::resetSpins()
{
    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(parameters);
    
    if( parameters->getWavelengthPattern() )
    {
        spinsystem.resetSpinsCosinus(parameters->getWavelength());
    }
    else
    {
        spinsystem.resetSpins();
    }
}


void MonteCarloHost::clearRecords()
{
    qDebug() << __PRETTY_FUNCTION__;

    energies.clear();
    magnetisations.clear();

    spinsystem.resetParameters();
    
}


void MonteCarloHost::print_data() const
{
    // save to file:  step  J  T  B  H  M  

    qDebug() << __PRETTY_FUNCTION__;
    isingDEBUG("mc: " << "saving data ...")
    
    Q_CHECK_PTR(parameters);
    std::string filekeystring = parameters->getFileKey();
    std::string filekey = filekeystring.substr( 0, filekeystring.find_first_of(" ") );
    filekey.append(".data");

    std::ofstream FILE;
    FILE.open(filekey);
    
    // print header line
    FILE << std::setw(14) << "# step"
    << std::setw(8) << "J"
    << std::setw(8) << "T"
    << std::setw(8) << "B"
    << std::setw(14) << "H"
    << std::setw(14) << "M"
    << '\n';
    
    assert(energies.size() == magnetisations.size());
    for(unsigned int i=0; i<energies.size(); ++i)
    {
        FILE << std::setw(14) << std::fixed << std::setprecision(0)<< (i+1)*parameters->getPrintFreq()
             << std::setw(8) << std::fixed << std::setprecision(2)<< parameters->getInteraction()
             << std::setw(8) << std::fixed << std::setprecision(2)<< parameters->getTemperature()
             << std::setw(8) << std::fixed << std::setprecision(2)<< parameters->getMagnetic()
             << std::setw(14) << std::fixed << std::setprecision(2) << energies[i]
             << std::setw(14) << std::fixed << std::setprecision(6) << magnetisations[i];
        FILE << '\n';
    }
    
    FILE.close();
}


void MonteCarloHost::print_averages() const
{
    // compute averages and save to file: <energy>  <magnetisation>  <susceptibility>  <heat capacity>

    qDebug() << __PRETTY_FUNCTION__;
    isingDEBUG("mc: " << "saving averaged data ...")

    Q_CHECK_PTR(parameters);
    std::string filekeystring = parameters->getFileKey();
    std::string filekey = filekeystring.substr( 0, filekeystring.find_first_of(" ") );
    filekey.append(".averaged_data");

    std::ofstream FILE;
    if( ! enhance::fileExists(filekey) )
    {
        // print header line
        FILE.open(filekey);
        FILE << std::setw(8) << "J"
             << std::setw(8) << "T"
             << std::setw(8) << "B"
             << std::setw(14) << "<H>"
             << std::setw(14) << "<M>"
             << std::setw(18) << "<chi>"
             << std::setw(18) << "<Cv>"
             << std::setw(14) << "# of samples"
             << '\n';
    }
    else
    {
        FILE.open(filekey, std::ios::app);
    }
    double averageEnergies = std::accumulate(std::begin(energies), std::end(energies), 0.0) / energies.size();
    double averageEnergiesSquared = std::accumulate(std::begin(energies), std::end(energies), 0.0, [](auto lhs, auto rhs){ return lhs + rhs*rhs; }) / energies.size();
    double averageMagnetisations = std::accumulate(std::begin(magnetisations), std::end(magnetisations), 0.0) / magnetisations.size();
    double averageMagnetisationsSquared = std::accumulate(std::begin(magnetisations), std::end(magnetisations), 0.0, [](auto lhs, auto rhs){ return lhs + rhs*rhs; }) / magnetisations.size();
    double denominator = std::pow(parameters->getTemperature(),2) * std::pow(parameters->getWidth()*parameters->getHeight(),2);
    
    FILE << std::setw(8) << std::fixed << std::setprecision(2) << parameters->getInteraction()
         << std::setw(8) << std::fixed << std::setprecision(2) << parameters->getTemperature()
         << std::setw(8) << std::fixed << std::setprecision(2) << parameters->getMagnetic()
         << std::setw(14) << std::fixed << std::setprecision(2) << averageEnergies
         << std::setw(14) << std::fixed << std::setprecision(6) << averageMagnetisations
         << std::setw(18) << std::fixed << std::setprecision(10) << (averageMagnetisationsSquared - averageMagnetisations*averageMagnetisations) / parameters->getTemperature()
         << std::setw(18) << std::fixed << std::setprecision(10) << (averageEnergiesSquared - averageEnergies*averageEnergies) / denominator
         << std::setw(14) << energies.size() 
         << '\n';
    
    FILE.close();
}


void MonteCarloHost::print_correlation(Histogram<double>& correlation) const
{
    // save correlation of current state in file  

    qDebug() << __PRETTY_FUNCTION__;
    isingDEBUG("mc: " << "saving correlation function G(r) ...")

    Q_CHECK_PTR(parameters);
    std::string filekeystring = parameters->getFileKey();
    std::string filekey = filekeystring.substr( 0, filekeystring.find_first_of(" ") );
    filekey.append(".correlation");

    std::ofstream FILE(filekey);
    FILE << "# correlation G(r) = <S(0) S(r)> - <S>^2\n";
    FILE << correlation.formatted_string();
    FILE.close();
}


void MonteCarloHost::print_structureFunction(Histogram<double>& structureFunction) const
{
    // save structure Function of current state in file

    qDebug() << __PRETTY_FUNCTION__;
    isingDEBUG("mc: " << "saving structure function S(k) ...")

    Q_CHECK_PTR(parameters);
    std::string filekeystring = parameters->getFileKey();
    std::string filekey = filekeystring.substr( 0, filekeystring.find_first_of(" ") );
    filekey.append(".structureFunction");

    std::ofstream FILE(filekey);
    FILE << "# structure function S(k) = FT( G(r) )\n";
    FILE << structureFunction.formatted_string();
    FILE.close();
}

