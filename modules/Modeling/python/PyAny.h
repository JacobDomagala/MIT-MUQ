#ifndef PYANY_H_
#define PYANY_H_

#include <boost/any.hpp>

#include <typeindex>

#include "pybind11/pybind11.h"
#include "pybind11/eigen.h"

namespace pybind11 { namespace detail {

    template <> struct type_caster<boost::any> {
    public:
        
        PYBIND11_TYPE_CASTER(boost::any, _("boost::any"));

        bool load(handle src, bool) {

            PyObject *source = src.ptr();

            char thisStr[] = "this";

            // If the pyobject does not have a "this" attribute, then we should try to convert it to a native c++ type (e.g., double, int, string, etc...)
            if (!PyObject_HasAttrString(source, thisStr))
            {
                std::string typeStr =  source->ob_type->tp_name;
                auto it = fromPythonMap.find(typeStr);
                if( it != fromPythonMap.end()){
                    value = it->second(source);
                    return true;
                }else{
                    std::cerr << "ERROR: Did not know how to handle type " << typeStr << std::endl;
                    return false;
                }
            }

            // In this case, the python object is a class
            PyObject* thisAttr = PyObject_GetAttrString(source, thisStr);
            if (thisAttr == NULL)
            {
                std::cerr << "\nERROR: Could not get the this attr.\n\n";
            }

            std::cerr << "ERROR: Don't have a good way to deal with classes yet...." << std::endl;
            assert(false);
            
            Py_DECREF(thisAttr);
            return false;
        }

        static handle cast(boost::any src, return_value_policy /* policy */, handle /* parent */) {

            auto it = toPythonMap.find(src.type());
            if(it != toPythonMap.end()){
                return it->second(src);
            }else{
            
                std::cerr << "ERROR: haven't implemented the boost::any->python conversion for type " << src.type().name() << std::endl;
                assert(false);
                return PyLong_FromLong(1);
            }
        }

    private:

        using EigenRowType = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
        using EigenColType = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;
              
        // A map of functions to convert a boost::any to a corresponding PyObject
        static std::map<std::type_index, std::function<handle(boost::any const&)>> toPythonMap;
        static std::map<std::string, std::function<boost::any(PyObject*)>> fromPythonMap;
        
        static std::map<std::type_index, std::function<handle(boost::any const&)>> createToPythonMap()
        {
          std::map<std::type_index, std::function<handle(boost::any const&)>> m;
          m[typeid(long)]          = [](boost::any const& x) { return PyLong_FromLong(boost::any_cast<long>(x));};
          m[typeid(int)]           = [](boost::any const& x) { return PyLong_FromLong(boost::any_cast<int>(x));};
          m[typeid(unsigned)]      = [](boost::any const& x) { return PyLong_FromLong(boost::any_cast<unsigned>(x));};
          m[typeid(unsigned long)] = [](boost::any const& x) { return PyLong_FromLong(boost::any_cast<unsigned long>(x));};
          m[typeid(double)]        = [](boost::any const& x) { return PyFloat_FromDouble(boost::any_cast<double>(x));};
          m[typeid(float)]         = [](boost::any const& x) { return PyFloat_FromDouble(static_cast<double>(boost::any_cast<float>(x)));};
          m[typeid(std::string)]   = [](boost::any const& x) { return PyBytes_FromString(boost::any_cast<std::string>(x).c_str());};
          
          m[typeid(Eigen::Ref<EigenRowType>)] = [](boost::any const& x){
              return eigen_array_cast<EigenProps<Eigen::Ref<EigenRowType>>>(boost::any_cast<Eigen::Ref<EigenRowType>>(x));
          };

          m[typeid(Eigen::Ref<EigenColType>)] = [](boost::any const& x){
              return eigen_array_cast<EigenProps<Eigen::Ref<EigenColType>>>(boost::any_cast<Eigen::Ref<EigenColType>>(x));
          };

          return m;
        }

        static std::map<std::string, std::function<boost::any(PyObject*)>> createFromPythonMap()
        {
          std::map<std::string, std::function<boost::any(PyObject*)>> m;
          m["float"] = [](PyObject* obj) { return boost::any(PyFloat_AsDouble(obj));};
          m["int"] = [](PyObject* obj) { return boost::any(PyLong_AsLong(obj));};
          m["str"] = [](PyObject* obj){
              PyObject* str_exc_type = PyObject_Repr(obj);
                
              PyObject* pyStr = PyUnicode_AsEncodedString(str_exc_type, "utf-8", "Error ~");
              const char *strExcType =  PyBytes_AS_STRING(pyStr);
              
              boost::any output = std::string(strExcType);
              
              Py_XDECREF(str_exc_type);
              Py_XDECREF(pyStr);
              
              return output;
          };
          
          m["numpy.ndarray"] = [](PyObject* obj){
              
              type_caster<Eigen::Ref<EigenRowType>> rowCaster;
              bool res = rowCaster.load(obj, false);
              if(res){
                  Eigen::Ref<EigenRowType> *ref = rowCaster;
                  return boost::any(*ref);
              }else{
                  type_caster<Eigen::Ref<EigenColType>> colCaster;
                  bool res = colCaster.load(obj, false);

                  if(!res){
                      std::cerr << "ERROR: Could not convert numpy array to Eigen::Ref.  Current support is only for row-major and col-major arrays of doubles.  Is the numpy array full of doubles?";
                      assert(res);
                  }

                  Eigen::Ref<EigenColType> *ref = colCaster;
                  return boost::any(*ref);
              }
          };
          
          return m;
        }
    };

    // Fill in the map for functions that transform c++ types to python types
    std::map<std::type_index, std::function<handle(boost::any const&)>>
    type_caster<boost::any>::toPythonMap = type_caster<boost::any>::createToPythonMap();

    // Fill in the map for functions that transform python types to c++ types
    std::map<std::string, std::function<boost::any(PyObject*)>>
    type_caster<boost::any>::fromPythonMap = type_caster<boost::any>::createFromPythonMap();

}} // namespace pybind11::detail

#endif
