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

#ifndef MATERIALPROPERTY_H
#define MATERIALPROPERTY_H

#include <vector>

#include "MooseArray.h"
#include "ColumnMajorMatrix.h"
#include "MaterialPropertyIO.h"

#include "libmesh_common.h"
#include "tensor_value.h"
#include "vector_value.h"

class PropertyValue;

/**
 * Scalar Init helper routine so that specialization isn't needed for basic scalar MaterialProperty types
 */
template<typename P>
PropertyValue *_init_helper(int size, PropertyValue *prop, const P* the_type);

/**
 * Vector Init helper routine so that specialization isn't needed for basic vector MaterialProperty types
 */
template<typename P>
PropertyValue *_init_helper(int size, PropertyValue *prop, const std::vector<P>* the_type);

/**
 * Abstract definition of a property value.
 */
class PropertyValue
{
public:
  /**
   * Destructor.
   */
  virtual ~PropertyValue() {};

  /**
   * String identifying the type of parameter stored.
   * Must be reimplemented in derived classes.
   */
  virtual std::string type () = 0;

  /**
   * Clone this value.  Useful in copy-construction.
   * Must be reimplemented in derived classes.
   */
  virtual PropertyValue *init (int size) = 0;

  virtual int size () = 0;

  /**
   * Resizes the property to the size n
   * Must be reimplemented in derived classes.
   */
  virtual void resize (int n) = 0;

  virtual void shallowCopy (PropertyValue *rhs) = 0;

  // save/restore in a file
  virtual void store(std::ofstream & stream) = 0;
  virtual void load(std::ifstream & stream) = 0;
};

/**
 * Concrete definition of a parameter value
 * for a specified type.
 */
template <typename T>
class MaterialProperty : public PropertyValue
{
public:
  virtual ~MaterialProperty()
  {
    _value.release();
  }

  /**
   * @returns a read-only reference to the parameter value.
   */
  MooseArray<T> & get () { return _value; }

  /**
   * @returns a writable reference to the parameter value.
   */
  MooseArray<T> & set () { return _value; }

  /**
   * String identifying the type of parameter stored.
   */
  virtual std::string type ();

  /**
   * Clone this value.  Useful in copy-construction.
   */
  virtual PropertyValue *init (int size);

  /**
   * Resizes the property to the size n
   */
  virtual void resize (int n);

  /**
   * Get element i out of the array.
   */
  T & operator[](const unsigned int i) { return _value[i]; }

  int size() { return _value.size(); }

  /**
   * Get element i out of the array.
   */
  const T & operator[](const unsigned int i) const { return _value[i]; }

  /**
   *
   */
  virtual void shallowCopy (PropertyValue *rhs);

  /**
   * Store the property into a binary stream
   */
  virtual void store(std::ofstream & stream);

  /**
   * Load the property from a binary stream
   */
  virtual void load(std::ifstream & stream);

  /**
   * Friend helper function to handle scalar material property initializations
   * @param size - the size corresponding to the quadrature rule
   * @param prop - The Material property that we will resize since this is not a member
   * @param the_type - This is just a template paramter used to identify the
   *                   difference between the scalar and vector template functions
   */
  template<typename P>
  friend
  PropertyValue *_init_helper(int size, PropertyValue *prop, const P* the_type);

  /**
   * Friend helper function to handle vector material property initializations
   * This function is an overload for the vector version
   */
  template<typename P>
  friend
  PropertyValue *_init_helper(int size, PropertyValue *prop, const std::vector<P>* the_type);

private:

  /// Stored parameter value.
  MooseArray<T> _value;
};


// ------------------------------------------------------------
// Material::Property<> class inline methods
template <typename T>
inline std::string
MaterialProperty<T>::type ()
{
  return typeid(T).name();
}

template <typename T>
inline PropertyValue *
MaterialProperty<T>::init (int size)
{
  return _init_helper(size, this, static_cast<T *>(0));
}

template <typename T>
inline void
MaterialProperty<T>::resize (int n)
{
  _value.resize(n);
}

template <typename T>
inline void
MaterialProperty<T>::shallowCopy (PropertyValue *rhs)
{
  mooseAssert(rhs != NULL, "Assigning NULL?");
  _value.shallowCopy(libmesh_cast_ptr<const MaterialProperty<T>*>(rhs)->_value);
}

template<typename T>
inline void
MaterialProperty<T>::store(std::ofstream & stream)
{
  for (unsigned int i = 0; i < _value.size(); i++)
    materialPropertyStore<T>(stream, _value[i]);
}

template<typename T>
inline void
MaterialProperty<T>::load(std::ifstream & stream)
{
  for (unsigned int i = 0; i < _value.size(); i++)
    materialPropertyLoad<T>(stream, _value[i]);
}


/**
 * Container for storing material properties
 */
class MaterialProperties : public std::vector<PropertyValue *>
{
public:
  MaterialProperties() { }

  /**
   * Parameter map iterator.
   */
  typedef std::vector<PropertyValue *>::iterator iterator;

  /**
   * Constant parameter map iterator.
   */
  typedef std::vector<PropertyValue *>::const_iterator const_iterator;

  /**
   * Deallocates the memory
   */
  void destroy()
  {
    for (iterator k = begin(); k != end(); ++k)
      delete *k;
  }
};

// Scalar Init Helper Function
template<typename P>
PropertyValue *_init_helper(int size, PropertyValue *prop, const P*)
{
  MaterialProperty<P> *copy = new MaterialProperty<P>;
  copy->_value.resize(size);
  return copy;
}

// Vector Init Helper Function
template<typename P>
PropertyValue *_init_helper(int size, PropertyValue *prop, const std::vector<P>*)
{
  typedef MaterialProperty<std::vector<P> > PropType;
  PropType *copy = new PropType;
  copy->_value.resize(size);

  // We don't know the size of the underlying vector at each
  // quadrature point, the user will be responsible for resizing it
  // and filling in the entries...

  // Return the copy we allocated
  return copy;
}


#endif
