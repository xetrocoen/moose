/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/
#ifndef COMPUTINGINITIALTEST_H
#define COMPUTINGINITIALTEST_H

#include "Material.h"


//Forward Declarations
class ComputingInitialTest;

template<>
InputParameters validParams<ComputingInitialTest>();

/**
 * Empty material for use in simple applications that don't need material properties.
 */
class ComputingInitialTest : public Material
{
public:
  ComputingInitialTest(const std::string & name,
                InputParameters parameters);

protected:
  virtual void initQpStatefulProperties();
  virtual void computeQpProperties();

  MaterialProperty<Real> & _thermal_conductivity;
  MaterialProperty<Real> & _thermal_conductivity_old;
};

#endif //COMPUTINGINITIALTEST_H
