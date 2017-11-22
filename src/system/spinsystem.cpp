#include "spinsystem.hpp"


void Spinsystem::setParameters(BaseParametersWidget* prms)
{
    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(prms);
    parameters = prms;
    Q_CHECK_PTR(parameters);
}



void Spinsystem::resetParameters()
{
    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(parameters);

    computeHamiltonian();
    Logger::getInstance().debug_new_line("[spinsystem]", "resetting parameters ... new initial H = ", Hamiltonian);
}



void Spinsystem::setup()
{
    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(parameters);

    spins.clear();
    lastFlipped.clear();

    // some safety checks:
    if( parameters->getConstrained() && (getWidth()*getHeight()) % 2 != 0 )
    {
        throw std::logic_error("[spinsystem] system size must be an even number if system is constrained");
    }
    if( parameters->getConstrained() && parameters->getMagnetic() != 0 )
    {
        throw std::logic_error("[spinsystem] constrained system cannot have a magnetic field");
    }

    auto width  = getWidth();
    auto height = getHeight();
    auto totalnumber = width * height;

    // create spinarray:
    for(unsigned int i=0; i<totalnumber; ++i)
        spins.emplace_back(i, +1);

    // set neighbours:
    for(auto& s: spins)
    {
        std::vector<std::reference_wrapper<Spin> > Nrefs;
        unsigned int Nid;
        const unsigned int id = s.getID();

        {
            // up
            Nid = ((long)id - static_cast<long>(width)) < 0 ? id - width + totalnumber : id - width;
            assert( Nid < spins.size() );
            if( Nid != id )
                Nrefs.push_back( std::ref(spins[Nid]) );
        }

        {
            // right
            Nid = (id + 1) % width == 0  ? id + 1 - width : id + 1;
            assert( Nid < spins.size() );
            if( Nid != id )
                Nrefs.push_back( std::ref(spins[Nid]) );
        }

        {
            // below
            Nid = id + width >= totalnumber ? id + width - totalnumber : id + width;
            assert( Nid < spins.size() );
            if( Nid != id )
                 Nrefs.push_back( std::ref(spins[Nid]) );
        }

        {
            // left
            Nid = id % width == 0  ? id - 1 + width : id - 1;
            assert( Nid < spins.size() );
            if( Nid != id )
                 Nrefs.push_back( std::ref(spins[Nid]) );
        }

        s.setNeighbours(Nrefs);
    }
    // debugging:
    for(auto& s: spins)
    {
        Logger::getInstance().debug_new_line("[spinsystem]",  "spin", s.getID(), "has neighbours :");
        std::for_each( std::begin(s.getNeighbours()), std::end(s.getNeighbours()), [](auto& N){ Logger::getInstance().debug( N.get().getID()," "); } );
    }
    
    // set spins:
    if(parameters->getWavelengthPattern())
    {
        resetSpinsCosinus(parameters->getWavelength());
    }
    else
    {
        resetSpins();
    }

}



void Spinsystem::resetSpins() 
{
    // randomly set types of all spins new

    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(parameters);

    int random;
    if( ! parameters->getConstrained() ) // initialise spins randomly
    {
        for( auto& s: spins )
        {
            random = enhance::random_int(0,1);
            s.setType( random == 1 ? +1 : -1 );
        }
    }      
    else  // constrained to specific up-spin to down-spin ratio
    {
        Logger::getInstance().debug_new_line("[spinsystem]", "ratio =", parameters->getRatio(), ", results in", static_cast<unsigned int>(parameters->getRatio() * spins.size()), " down spins.");
        for( auto& s: spins ) 
            s.setType( +1 );
        for(unsigned int i=0; i<static_cast<unsigned int>(parameters->getRatio() * spins.size()); ++i)
        {
            do
            {
                random = enhance::random_int(0, spins.size()-1);
            }
            while( spins[random].getType() == -1 );
            spins[random].setType(-1);
        }
    }

    // clear / reset all vectors: 
    lastFlipped.clear();
    
    // calculate initial Hamiltonian:
    computeHamiltonian();
    Logger::getInstance().debug_new_line("[spinsystem]", " resetting spins randomly... new initial H =", Hamiltonian);
    Logger::getInstance().debug_new_line(str());

}



void Spinsystem::resetSpinsCosinus(const double k) 
{
    // set types of all spins new according to c(x) =  cos(kx) 

    qDebug() << __PRETTY_FUNCTION__;

    Q_CHECK_PTR(parameters);

    int random;
    if( ! parameters->getConstrained() ) 
        throw std::logic_error("[spinsystem] resetting spins according to cos not implemented for !CONSTRAINED");
     
    {
        for( auto& s: spins ) 
            s.setType( +1 );
        for(unsigned int i = 0; i<parameters->getWidth(); ++i)
        {
            double ratio = (std::cos(k*(2*M_PI/parameters->getWidth())*static_cast<double>(i+0.5)) + 1) / 2;
            unsigned int nrDownSpins = std::round(ratio*parameters->getWidth());
            for(unsigned int j=0; j<nrDownSpins; ++j)
            {
                do
                {
                    random = enhance::random_int(i*parameters->getWidth(), (i+1)*parameters->getWidth() - 1);
                }
                while( spins[random].getType() == -1 );
                spins[random].setType(-1);
            }
        }
    }

    // clear / reset all vectors: 
    lastFlipped.clear();
    
    // calculate initial Hamiltonian:
    computeHamiltonian();
    Logger::getInstance().debug_new_line("[spinsystem]", "resetting spins with cos pattern ... new initial H =", Hamiltonian);
    Logger::getInstance().debug_new_line(str());

}



double Spinsystem::local_energy_interaction(const Spin& _spin) const
{
    // calculate interaction contribution to local energy for given spin
    
    // double energy = - parameters->getInteraction() * _spin.num_signed<SPINTYPE::UP>()
                    // - parameters->getInteraction() * _spin.num_signed<SPINTYPE::DOWN>();

    return - parameters->getInteraction() * _spin.sumNeighbours();
}



double Spinsystem::local_energy_magnetic(const Spin& _spin) const
{
    // calculate magnetic contribution to local energy for given spin
    
    // return - parameters->getMagnetic() * (_spin.getType() == SPINTYPE::UP ? 1 : -1);
    return - parameters->getMagnetic() * _spin.getType();
}



void Spinsystem::computeHamiltonian()
{
    // calculate Hamiltonian of the system

    Hamiltonian = std::accumulate(std::cbegin(spins), std::cend(spins), static_cast<double>(0), [&](double i, const Spin& S)
    {
        return i + local_energy_interaction(S) / 2 + local_energy_magnetic(S);
    });
}



void Spinsystem::flip()
{
    lastFlipped.clear();
    double localEnergy_before = 0.f;
    double localEnergy_after = 0.f;

    if( ! parameters->getConstrained() )
    {
        // find random spin
        auto randomspin = enhance::random_iterator(spins);
        lastFlipped.emplace_back( std::ref(*randomspin) );
        // flip spin
        localEnergy_before = local_energy_interaction( *randomspin ) + local_energy_magnetic( *randomspin );
        randomspin->flip();
        localEnergy_after = local_energy_interaction( *randomspin ) + local_energy_magnetic( *randomspin );
        // update Hamiltonian:
        Hamiltonian += localEnergy_after - localEnergy_before;
    }
    else
    {
        // find random spin
        auto randomspin = enhance::random_iterator(spins);
        do
        {
            randomspin = enhance::random_iterator(spins);
        } while( randomspin->sumOppositeNeighbours() == 0 );
        
        // find random neighbour
        auto randomneighbour = enhance::random_iterator(randomspin->getNeighbours());
        do
        {
            randomneighbour = enhance::random_iterator(randomspin->getNeighbours());
        } while( randomneighbour->get().getType() == randomspin->getType() );
        
        // flip spins
        lastFlipped.emplace_back(*randomspin);
        lastFlipped.emplace_back(randomneighbour->get());
        localEnergy_before = local_energy_interaction(*randomspin) + local_energy_interaction(randomneighbour->get());
        randomspin->flip();
        randomneighbour->get().flip();
        localEnergy_after = local_energy_interaction(*randomspin) + local_energy_interaction(randomneighbour->get());

        // update Hamiltonian
        Hamiltonian += localEnergy_after - localEnergy_before;
        
    }

#ifndef NDEBUG
    Logger::getInstance().debug_new_line("[spinsystem]",  "flipping spin: ");
    for(const auto& s: lastFlipped) Logger::getInstance().debug( " ", s.get().getID());
#endif
}



void Spinsystem::flip_back()
{
    if( lastFlipped.size() == 0 )
        throw std::logic_error("[spinsystem] Cannot flip back, since nothing has flipped yet");
    else
    {
        double localEnergy_before = 0;
        double localEnergy_after = 0;

        for( const auto& s: lastFlipped ) 
        {
            localEnergy_before += local_energy_interaction( s.get() ) + local_energy_magnetic( s.get() );
        }
        for( const auto& s: lastFlipped ) 
        {
            s.get().flip();
        }
        for( const auto& s: lastFlipped ) 
        {
            localEnergy_after += local_energy_interaction( s.get() ) + local_energy_magnetic( s.get() );
        }

        // update Hamiltonian
        Hamiltonian += localEnergy_after - localEnergy_before;
    }

    Logger::getInstance().debug_new_line("[spinsystem]", "flipping back: ");
    for(const auto& s: lastFlipped) Logger::getInstance().debug("  ", s.get().getID());

}



void Spinsystem::print(std::ostream & stream) const
{
    // print spinarray to stream
    for(const auto& s: spins)
    {
        stream << ( s.getType() == -1 ? "-" : "+" )
        << ( (static_cast<unsigned int>(s.getID() + 1)) % parameters->getWidth() == 0 ? '\n' : ' ');
    }
}



std::string Spinsystem::str() const
{
    std::stringstream sstream;
    sstream << *this;
    
    return sstream.str(); 
}



double Spinsystem::distance(const Spin& _spin1, const Spin& _spin2) const
{
    // compute distance between spins _spin1 and _spin2
    int a, b, c, d, x, y;

    a = _spin1.getID() % getWidth();
    b = _spin1.getID() / getWidth();
    c = _spin2.getID() % getWidth();
    d = _spin2.getID() / getWidth();


    x = (std::abs(c - a) <= static_cast<int>(getWidth()/2) ? std::abs(c - a) : std::abs(c - a) - getWidth());
    y = (std::abs(d - b) <= static_cast<int>(getHeight()/2) ? std::abs(d - b) : std::abs(d - b) - getHeight());

    return  std::sqrt( x*x + y*y );
}



Histogram<double> Spinsystem::getCorrelation() const
{
    // compute correlations between spins: G(r) = <S(0)S(r)> - <S>^2

    Histogram<double> correlation {0.01};
    Logger::getInstance().debug_new_line("[spinsystem]", "computing correlation <Si Sj>:");

    // first: <S(0)S(r)>:
    auto counter = correlation;
    for( auto s1 = std::begin(spins); s1 != std::end(spins); s1 += 1)
    {
        for( auto s2 = s1 + 1; s2 != std::end(spins); s2 += 1)
        {
            double dist = distance(*s1, *s2);
            correlation.add_data( dist, s1->getType() == s2->getType() ? 1 : -1);
            counter.add_data( dist );
            Logger::getInstance().debug_new_line("[spinsystem]", "correlating ", s1->getID(), " with ", s2->getID()," : ", s1->getType() == s2->getType() ? 1 : -1);
            Logger::getInstance().debug("   distance ", dist);
        }
    }
    // normalisation:
    std::for_each(std::begin(correlation), std::end(correlation), [&](auto& B)
    { 
        if(B.counter != 0)
            B.counter /= counter.get_data(B.position()); 
    });

    // then:  - <S>:
    correlation.shift( getMagnetisation() );

    correlation.sort_bins();
    return correlation;
}



std::vector<double> Spinsystem::computeStructureFunction(const Histogram<double> correlation) const
{
    // compute Fourier transformation A(k) = sum( cos(2PI/width*k*r) * corr(r) ), wobei r = vector(x,y) mit x,y: Abstände zwischen Spins in x/y Richtung

    if( ! parameters->getConstrained() ) 
        throw std::logic_error("[spinsystem] computation of Amplitudes not implemented for !CONSTRAINED");

    // std::vector<double> amplitudes;

    Logger::getInstance().debug_new_line("[spinsystem]", "computing amplitudes");
    Logger::getInstance().debug_new_line( str() );

    // std::vector<int> rowSummedSpins;
    // for(unsigned int i= 0; i<parameters->getWidth(); ++i)
    // {
    //     rowSummedSpins.push_back(0);
    //     for(unsigned int j = i*parameters->getWidth(); j < (i+1)*parameters->getWidth(); ++j)
    //     {
    //         rowSummedSpins.back() += ( spins[j].get_type() == SPINTYPE::UP ? 1 : -1 );
    //         Logger::getInstance().debug("[spinsystem], spin ", spins[j].get_ID(), " ", spins[j].get_type() == UP ? 1 : -1);
    //     }
    //     Logger::getInstance().debug("[spinsystem]", "row ", i , " sum ", rowSummedSpins.back());
    // }

    // for( unsigned int k = 1; k<= parameters->getWidth()/2-1; ++k )
    // {
    //     amplitudes.push_back(0);
    //     Logger::getInstance().debug("[spinsystem]", "k = ", k);
    //     double cosSum = 0;
    //     double sinSum = 0;
    //     for(unsigned int i = 0; i<parameters->getWidth(); ++i)
    //     {
    //         double cos = std::cos(k*(2*M_PI/parameters->getWidth())*static_cast<double>(i+0.5));
    //         double sin = std::sin(k*(2*M_PI/parameters->getWidth())*static_cast<double>(i+0.5));
    //         cosSum += cos*rowSummedSpins[i];
    //         sinSum += sin*rowSummedSpins[i];
    //         Logger::getInstance().debug("[spinsystem]", "cos(ki) =", cos, "sin(ki) =", sin);
    //     }
    //     amplitudes.back() += std::sqrt( cosSum*cosSum + sinSum*sinSum );
    //     Logger::getInstance().debug(" amplitude: ", amplitudes.back());
    // }

    std::vector<double> structureFunction;

    std::string filekeystring = parameters->getFileKey();
    std::string filekey = filekeystring.substr( 0, filekeystring.find_first_of(" ") );
    filekey.append(".structureFunktion");
    std::ofstream FILE(filekey);
    FILE << "#k    S(k)\n";

    for(double k=0; k< 1; k+=0.02)
    {
        // Logger::getInstance().write_new_line("k =", k);
        structureFunction.push_back(0);
        for(auto& B: correlation)
        {
            structureFunction.back() += std::cos( k*B.position()*2*M_PI/parameters->getWidth() ) * B.counter;
            // structureFunction.back() += std::cos( static_cast<double>(k)*(2*M_PI/parameters->getWidth())*B.position() ) * B.counter;
        }
        // Logger::getInstance().write(" S(k) = ", structureFunction.back());
        FILE << k << "   " << structureFunction.back() << "\n";
    }
    FILE.close();

    return structureFunction;
}



double Spinsystem::getMagnetisation() const
{

    int numUp = std::accumulate( std::begin(spins), std::end(spins), 0, [](int i, const Spin& S)
    {
        return S.getType() == +1 ? i+1 : i;
    });
    int numDown = std::accumulate( std::begin(spins), std::end(spins), 0, [](int i, const Spin& S)
    {
        return S.getType() == -1 ? i+1 : i;
    });

    return ((double) numUp - numDown ) / spins.size(); 
}


