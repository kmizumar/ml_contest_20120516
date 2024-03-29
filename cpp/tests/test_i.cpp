#include "stdafx.h"
#include <math.h>
#include "alglibmisc.h"
#include "alglibinternal.h"
#include "linalg.h"
#include "statistics.h"
#include "dataanalysis.h"
#include "specialfunctions.h"
#include "solvers.h"
#include "optimization.h"
#include "diffequations.h"
#include "fasttransforms.h"
#include "integration.h"
#include "interpolation.h"

using namespace alglib;

bool doc_test_bool(bool v, bool t)
{ return (v && t) || (!v && !t); }

bool doc_test_int(ae_int_t v, ae_int_t t)
{ return v==t; }

bool doc_test_real(double v, double t, double _threshold)
{
    double s = _threshold>=0 ? 1.0 : fabs(t);
    double threshold = fabs(_threshold);
    return fabs(v-t)/s<=threshold;
}

bool doc_test_complex(alglib::complex v, alglib::complex t, double _threshold)
{
    double s = _threshold>=0 ? 1.0 : alglib::abscomplex(t);
    double threshold = fabs(_threshold);
    return abscomplex(v-t)/s<=threshold;
}


bool doc_test_bool_vector(const boolean_1d_array &v, const boolean_1d_array &t)
{
    ae_int_t i;
    if( v.length()!=t.length() )
        return false;
    for(i=0; i<v.length(); i++)
        if( v(i)!=t(i) )
            return false;
    return true;
}

bool doc_test_bool_matrix(const boolean_2d_array &v, const boolean_2d_array &t)
{
    ae_int_t i, j;
    if( v.rows()!=t.rows() )
        return false;
    if( v.cols()!=t.cols() )
        return false;
    for(i=0; i<v.rows(); i++)
        for(j=0; j<v.cols(); j++)
            if( v(i,j)!=t(i,j) )
                return false;
    return true;
}

bool doc_test_int_vector(const integer_1d_array &v, const integer_1d_array &t)
{
    ae_int_t i;
    if( v.length()!=t.length() )
        return false;
    for(i=0; i<v.length(); i++)
        if( v(i)!=t(i) )
            return false;
    return true;
}

bool doc_test_int_matrix(const integer_2d_array &v, const integer_2d_array &t)
{
    ae_int_t i, j;
    if( v.rows()!=t.rows() )
        return false;
    if( v.cols()!=t.cols() )
        return false;
    for(i=0; i<v.rows(); i++)
        for(j=0; j<v.cols(); j++)
            if( v(i,j)!=t(i,j) )
                return false;
    return true;
}

bool doc_test_real_vector(const real_1d_array &v, const real_1d_array &t, double _threshold)
{
    ae_int_t i;
    if( v.length()!=t.length() )
        return false;
    for(i=0; i<v.length(); i++)
    {
        double s = _threshold>=0 ? 1.0 : fabs(t(i));
        double threshold = fabs(_threshold);
        if( fabs(v(i)-t(i))/s>threshold )
            return false;
    }
    return true;
}

bool doc_test_real_matrix(const real_2d_array &v, const real_2d_array &t, double _threshold)
{
    ae_int_t i, j;
    if( v.rows()!=t.rows() )
        return false;
    if( v.cols()!=t.cols() )
        return false;
    for(i=0; i<v.rows(); i++)
        for(j=0; j<v.cols(); j++)
        {
            double s = _threshold>=0 ? 1.0 : fabs(t(i,j));
            double threshold = fabs(_threshold);
            if( fabs(v(i,j)-t(i,j))/s>threshold )
                return false;
        }
    return true;
}

bool doc_test_complex_vector(const complex_1d_array &v, const complex_1d_array &t, double _threshold)
{
    ae_int_t i;
    if( v.length()!=t.length() )
        return false;
    for(i=0; i<v.length(); i++)
    {
        double s = _threshold>=0 ? 1.0 : alglib::abscomplex(t(i));
        double threshold = fabs(_threshold);
        if( abscomplex(v(i)-t(i))/s>threshold )
            return false;
    }
    return true;
}

bool doc_test_complex_matrix(const complex_2d_array &v, const complex_2d_array &t, double _threshold)
{
    ae_int_t i, j;
    if( v.rows()!=t.rows() )
        return false;
    if( v.cols()!=t.cols() )
        return false;
    for(i=0; i<v.rows(); i++)
        for(j=0; j<v.cols(); j++)
        {
            double s = _threshold>=0 ? 1.0 : alglib::abscomplex(t(i,j));
            double threshold = fabs(_threshold);
            if( abscomplex(v(i,j)-t(i,j))/s>threshold )
                return false;
        }
    return true;
}

template<class T>
void spoil_vector_by_adding_element(T &x)
{
    ae_int_t i;
    T y = x;
    x.setlength(y.length()+1);
    for(i=0; i<y.length(); i++)
        x(i) = y(i);
    x(y.length()) = 0;
}

template<class T>
void spoil_vector_by_deleting_element(T &x)
{
    ae_int_t i;
    T y = x;
    x.setlength(y.length()-1);
    for(i=0; i<y.length()-1; i++)
        x(i) = y(i);
}

template<class T>
void spoil_matrix_by_adding_row(T &x)
{
    ae_int_t i, j;
    T y = x;
    x.setlength(y.rows()+1, y.cols());
    for(i=0; i<y.rows(); i++)
        for(j=0; j<y.cols(); j++)
            x(i,j) = y(i,j);
    for(j=0; j<y.cols(); j++)
        x(y.rows(),j) = 0;
}

template<class T>
void spoil_matrix_by_deleting_row(T &x)
{
    ae_int_t i, j;
    T y = x;
    x.setlength(y.rows()-1, y.cols());
    for(i=0; i<y.rows()-1; i++)
        for(j=0; j<y.cols(); j++)
            x(i,j) = y(i,j);
}

template<class T>
void spoil_matrix_by_adding_col(T &x)
{
    ae_int_t i, j;
    T y = x;
    x.setlength(y.rows(), y.cols()+1);
    for(i=0; i<y.rows(); i++)
        for(j=0; j<y.cols(); j++)
            x(i,j) = y(i,j);
    for(i=0; i<y.rows(); i++)
        x(i,y.cols()) = 0;
}

template<class T>
void spoil_matrix_by_deleting_col(T &x)
{
    ae_int_t i, j;
    T y = x;
    x.setlength(y.rows(), y.cols()-1);
    for(i=0; i<y.rows(); i++)
        for(j=0; j<y.cols()-1; j++)
            x(i,j) = y(i,j);
}

template<class T>
void spoil_vector_by_nan(T &x)
{
    if( x.length()!=0 )
        x(randominteger(x.length())) = fp_nan;
}

template<class T>
void spoil_vector_by_posinf(T &x)
{
    if( x.length()!=0 )
        x(randominteger(x.length())) = fp_posinf;
}

template<class T>
void spoil_vector_by_neginf(T &x)
{
    if( x.length()!=0 )
        x(randominteger(x.length())) = fp_neginf;
}



template<class T>
void spoil_matrix_by_nan(T &x)
{
    if( x.rows()!=0 && x.cols()!=0 )
        x(randominteger(x.rows()),randominteger(x.cols())) = fp_nan;
}

template<class T>
void spoil_matrix_by_posinf(T &x)
{
    if( x.rows()!=0 && x.cols()!=0 )
        x(randominteger(x.rows()),randominteger(x.cols())) = fp_posinf;
}

template<class T>
void spoil_matrix_by_neginf(T &x)
{
    if( x.rows()!=0 && x.cols()!=0 )
        x(randominteger(x.rows()),randominteger(x.cols())) = fp_neginf;
}

void function1_func(const real_1d_array &x, double &func, void *ptr)
{
    //
    // this callback calculates f(x0,x1) = 100*(x0+3)^4 + (x1-3)^4
    //
    func = 100*pow(x[0]+3,4) + pow(x[1]-3,4);
}
void function1_grad(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr) 
{
    //
    // this callback calculates f(x0,x1) = 100*(x0+3)^4 + (x1-3)^4
    // and its derivatives df/d0 and df/dx1
    //
    func = 100*pow(x[0]+3,4) + pow(x[1]-3,4);
    grad[0] = 400*pow(x[0]+3,3);
    grad[1] = 4*pow(x[1]-3,3);
}
void function1_hess(const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr)
{
    //
    // this callback calculates f(x0,x1) = 100*(x0+3)^4 + (x1-3)^4
    // its derivatives df/d0 and df/dx1
    // and its Hessian.
    //
    func = 100*pow(x[0]+3,4) + pow(x[1]-3,4);
    grad[0] = 400*pow(x[0]+3,3);
    grad[1] = 4*pow(x[1]-3,3);
    hess[0][0] = 1200*pow(x[0]+3,2);
    hess[0][1] = 0;
    hess[1][0] = 0;
    hess[1][1] = 12*pow(x[1]-3,2);
}
void  function1_fvec(const real_1d_array &x, real_1d_array &fi, void *ptr)
{
    //
    // this callback calculates
    // f0(x0,x1) = 100*(x0+3)^4,
    // f1(x0,x1) = (x1-3)^4
    //
    fi[0] = 10*pow(x[0]+3,2);
    fi[1] = pow(x[1]-3,2);
}
void  function1_jac(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr)
{
    //
    // this callback calculates
    // f0(x0,x1) = 100*(x0+3)^4,
    // f1(x0,x1) = (x1-3)^4
    // and Jacobian matrix J = [dfi/dxj]
    //
    fi[0] = 10*pow(x[0]+3,2);
    fi[1] = pow(x[1]-3,2);
    jac[0][0] = 20*(x[0]+3);
    jac[0][1] = 0;
    jac[1][0] = 0;
    jac[1][1] = 2*(x[1]-3);
}
void function2_func(const real_1d_array &x, double &func, void *ptr)
{
    //
    // this callback calculates f(x0,x1) = (x0^2+1)^2 + (x1-1)^2
    //
    func = pow(x[0]*x[0]+1,2) + pow(x[1]-1,2);
}
void function2_grad(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr) 
{
    //
    // this callback calculates f(x0,x1) = (x0^2+1)^2 + (x1-1)^2
    // and its derivatives df/d0 and df/dx1
    //
    func = pow(x[0]*x[0]+1,2) + pow(x[1]-1,2);
    grad[0] = 4*(x[0]*x[0]+1)*x[0];
    grad[1] = 2*(x[1]-1);
}
void function2_hess(const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr)
{
    //
    // this callback calculates f(x0,x1) = (x0^2+1)^2 + (x1-1)^2
    // its gradient and Hessian
    //
    func = pow(x[0]*x[0]+1,2) + pow(x[1]-1,2);
    grad[0] = 4*(x[0]*x[0]+1)*x[0];
    grad[1] = 2*(x[1]-1);
    hess[0][0] = 12*x[0]*x[0]+4;
    hess[0][1] = 0;
    hess[1][0] = 0;
    hess[1][1] = 2;
}
void  function2_fvec(const real_1d_array &x, real_1d_array &fi, void *ptr)
{
    //
    // this callback calculates
    // f0(x0,x1) = x0^2+1
    // f1(x0,x1) = x1-1
    //
    fi[0] = x[0]*x[0]+1;
    fi[1] = x[1]-1;
}
void  function2_jac(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr)
{
    //
    // this callback calculates
    // f0(x0,x1) = x0^2+1
    // f1(x0,x1) = x1-1
    // and Jacobian matrix J = [dfi/dxj]
    //
    fi[0] = x[0]*x[0]+1;
    fi[1] = x[1]-1;
    jac[0][0] = 2*x[0];
    jac[0][1] = 0;
    jac[1][0] = 0;
    jac[1][1] = 1;
}
void bad_func(const real_1d_array &x, double &func, void *ptr)
{
    //
    // this callback calculates 'bad' function,
    // i.e. function with incorrectly calculated derivatives
    //
    func = 100*pow(x[0]+3,4) + pow(x[1]-3,4);
}
void bad_grad(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr) 
{
    //
    // this callback calculates 'bad' function,
    // i.e. function with incorrectly calculated derivatives
    //
    func = 100*pow(x[0]+3,4) + pow(x[1]-3,4);
    grad[0] = 40*pow(x[0]+3,3);
    grad[1] = 40*pow(x[1]-3,3);
}
void bad_hess(const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr)
{
    //
    // this callback calculates 'bad' function,
    // i.e. function with incorrectly calculated derivatives
    //
    func = 100*pow(x[0]+3,4) + pow(x[1]-3,4);
    grad[0] = 40*pow(x[0]+3,3);
    grad[1] = 40*pow(x[1]-3,3);
    hess[0][0] = 120*pow(x[0]+3,2);
    hess[0][1] = 0;
    hess[1][0] = 0;
    hess[1][1] = 120*pow(x[1]-3,2);
}
void  bad_fvec(const real_1d_array &x, real_1d_array &fi, void *ptr)
{
    //
    // this callback calculates 'bad' function,
    // i.e. function with incorrectly calculated derivatives
    //
    fi[0] = 10*pow(x[0]+3,2);
    fi[1] = pow(x[1]-3,2);
}
void  bad_jac(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr)
{
    //
    // this callback calculates 'bad' function,
    // i.e. function with incorrectly calculated derivatives
    //
    fi[0] = 10*pow(x[0]+3,2);
    fi[1] = pow(x[1]-3,2);
    jac[0][0] = 2*(x[0]+3);
    jac[0][1] = 1;
    jac[1][0] = 0;
    jac[1][1] = 20*(x[1]-3);
}
void function_cx_1_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0))
    // where x is a position on X-axis and c is adjustable parameter
    func = exp(-c[0]*pow(x[0],2));
}
void function_cx_1_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr) 
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0)) and gradient G={df/dc[i]}
    // where x is a position on X-axis and c is adjustable parameter.
    // IMPORTANT: gradient is calculated with respect to C, not to X
    func = exp(-c[0]*pow(x[0],2));
    grad[0] = -pow(x[0],2)*func;
}
void function_cx_1_hess(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr) 
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0)), gradient G={df/dc[i]} and Hessian H={d2f/(dc[i]*dc[j])}
    // where x is a position on X-axis and c is adjustable parameter.
    // IMPORTANT: gradient/Hessian are calculated with respect to C, not to X
    func = exp(-c[0]*pow(x[0],2));
    grad[0] = -pow(x[0],2)*func;
    hess[0][0] = pow(x[0],4)*func;
}
void ode_function_1_diff(const real_1d_array &y, double x, real_1d_array &dy, void *ptr) 
{
    // this callback calculates f(y[],x)=-y[0]
    dy[0] = -y[0];
}
void int_function_1_func(double x, double xminusa, double bminusx, double &y, void *ptr) 
{
    // this callback calculates f(x)=exp(x)
    y = exp(x);
}
void function_debt_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    //
    // this callback calculates f(c,x)=c[0]*(1+c[1]*(pow(x[0]-1999,c[2])-1))
    //
    func = c[0]*(1+c[1]*(pow(x[0]-1999,c[2])-1));
}
void s1_grad(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    //
    // this callback calculates f(x) = (1+x)^(-0.2) + (1-x)^(-0.3) + 1000*x and its gradient.
    //
    // function is trimmed when we calculate it near the singular points or outside of the [-1,+1].
    // Note that we do NOT calculate gradient in this case.
    //
    if( (x[0]<=-0.999999999999) || (x[0]>=+0.999999999999) )
    {
        func = 1.0E+300;
        return;
    }
    func = pow(1+x[0],-0.2) + pow(1-x[0],-0.3) + 1000*x[0];
    grad[0] = -0.2*pow(1+x[0],-1.2) +0.3*pow(1-x[0],-1.3) + 1000;
}

int main()
{
    bool _TotalResult = true;
    bool _TestResult;
    int _spoil_scenario;
    printf("C++ tests. Please wait...\n");
#ifdef AE_USE_ALLOC_COUNTER
    if( alglib_impl::_alloc_counter!=0 )
    {
        _TotalResult = false;
        printf("FAILURE: alloc_counter is non-zero on start!\n");
    }
#endif
    try
    {
        //
        // TEST nneighbor_d_1
        //      Nearest neighbor search, KNN queries
        //
        printf("0/104\n");
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                real_2d_array a = "[[0,0],[0,1],[1,0],[1,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                ae_int_t nx = 2;
                ae_int_t ny = 0;
                ae_int_t normtype = 2;
                kdtree kdt;
                real_1d_array x;
                real_2d_array r = "[[]]";
                ae_int_t k;
                kdtreebuild(a, nx, ny, normtype, kdt);
                x = "[-1,0]";
                k = kdtreequeryknn(kdt, x, 1);
                _TestResult = _TestResult && doc_test_int(k, 1);
                kdtreequeryresultsx(kdt, r);
                _TestResult = _TestResult && doc_test_real_matrix(r, "[[0,0]]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "nneighbor_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST nneighbor_t_2
        //      Subsequent queries; buffered functions must use previously allocated storage (if large enough), so buffer may contain some info from previous call
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                real_2d_array a = "[[0,0],[0,1],[1,0],[1,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                ae_int_t nx = 2;
                ae_int_t ny = 0;
                ae_int_t normtype = 2;
                kdtree kdt;
                real_1d_array x;
                real_2d_array rx = "[[]]";
                ae_int_t k;
                kdtreebuild(a, nx, ny, normtype, kdt);
                x = "[+2,0]";
                k = kdtreequeryknn(kdt, x, 2, true);
                _TestResult = _TestResult && doc_test_int(k, 2);
                kdtreequeryresultsx(kdt, rx);
                _TestResult = _TestResult && doc_test_real_matrix(rx, "[[1,0],[1,1]]", 0.05);
                x = "[-2,0]";
                k = kdtreequeryknn(kdt, x, 1, true);
                _TestResult = _TestResult && doc_test_int(k, 1);
                kdtreequeryresultsx(kdt, rx);
                _TestResult = _TestResult && doc_test_real_matrix(rx, "[[0,0],[1,1]]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "nneighbor_t_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST nneighbor_d_2
        //      Serialization of KD-trees
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                real_2d_array a = "[[0,0],[0,1],[1,0],[1,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                ae_int_t nx = 2;
                ae_int_t ny = 0;
                ae_int_t normtype = 2;
                kdtree kdt0;
                kdtree kdt1;
                std::string s;
                real_1d_array x;
                real_2d_array r0 = "[[]]";
                real_2d_array r1 = "[[]]";

                //
                // Build tree and serialize it
                //
                kdtreebuild(a, nx, ny, normtype, kdt0);
                alglib::kdtreeserialize(kdt0, s);
                alglib::kdtreeunserialize(s, kdt1);

                //
                // Compare results from KNN queries
                //
                x = "[-1,0]";
                kdtreequeryknn(kdt0, x, 1);
                kdtreequeryresultsx(kdt0, r0);
                kdtreequeryknn(kdt1, x, 1);
                kdtreequeryresultsx(kdt1, r1);
                _TestResult = _TestResult && doc_test_real_matrix(r0, "[[0,0]]", 0.05);
                _TestResult = _TestResult && doc_test_real_matrix(r1, "[[0,0]]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "nneighbor_d_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST basestat_d_base
        //      Basic functionality (moments, adev, median, percentile)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double mean;
                double variance;
                double skewness;
                double kurtosis;
                double adev;
                double p;
                double v;

                //
                // Here we demonstrate calculation of sample moments
                // (mean, variance, skewness, kurtosis)
                //
                samplemoments(x, mean, variance, skewness, kurtosis);
                _TestResult = _TestResult && doc_test_real(mean, 28.5, 0.01);
                _TestResult = _TestResult && doc_test_real(variance, 801.1667, 0.01);
                _TestResult = _TestResult && doc_test_real(skewness, 0.5751, 0.01);
                _TestResult = _TestResult && doc_test_real(kurtosis, -1.2666, 0.01);

                //
                // Average deviation
                //
                sampleadev(x, adev);
                _TestResult = _TestResult && doc_test_real(adev, 23.2, 0.01);

                //
                // Median and percentile
                //
                samplemedian(x, v);
                _TestResult = _TestResult && doc_test_real(v, 20.5, 0.01);
                p = 0.5;
                if( _spoil_scenario==3 )
                    p = fp_nan;
                if( _spoil_scenario==4 )
                    p = fp_posinf;
                if( _spoil_scenario==5 )
                    p = fp_neginf;
                samplepercentile(x, p, v);
                _TestResult = _TestResult && doc_test_real(v, 20.5, 0.01);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "basestat_d_base");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST basestat_d_c2
        //      Correlation (covariance) between two random variables
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<10; _spoil_scenario++)
        {
            try
            {
                //
                // We have two samples - x and y, and want to measure dependency between them
                //
                real_1d_array x = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                double v;

                //
                // Three dependency measures are calculated:
                // * covariation
                // * Pearson correlation
                // * Spearman rank correlation
                //
                v = cov2(x, y);
                _TestResult = _TestResult && doc_test_real(v, 82.5, 0.001);
                v = pearsoncorr2(x, y);
                _TestResult = _TestResult && doc_test_real(v, 0.9627, 0.001);
                v = spearmancorr2(x, y);
                _TestResult = _TestResult && doc_test_real(v, 1.000, 0.001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "basestat_d_c2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST basestat_d_cm
        //      Correlation (covariance) between components of random vector
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // X is a sample matrix:
                // * I-th row corresponds to I-th observation
                // * J-th column corresponds to J-th variable
                //
                real_2d_array x = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                real_2d_array c;

                //
                // Three dependency measures are calculated:
                // * covariation
                // * Pearson correlation
                // * Spearman rank correlation
                //
                // Result is stored into C, with C[i,j] equal to correlation
                // (covariance) between I-th and J-th variables of X.
                //
                covm(x, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[1.80,0.60,-1.40],[0.60,0.70,-0.80],[-1.40,-0.80,14.70]]", 0.01);
                pearsoncorrm(x, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[1.000,0.535,-0.272],[0.535,1.000,-0.249],[-0.272,-0.249,1.000]]", 0.01);
                spearmancorrm(x, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[1.000,0.556,-0.306],[0.556,1.000,-0.750],[-0.306,-0.750,1.000]]", 0.01);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "basestat_d_cm");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST basestat_d_cm2
        //      Correlation (covariance) between two random vectors
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                //
                // X and Y are sample matrices:
                // * I-th row corresponds to I-th observation
                // * J-th column corresponds to J-th variable
                //
                real_2d_array x = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                real_2d_array y = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==3 )
                    spoil_matrix_by_nan(y);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_posinf(y);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_neginf(y);
                real_2d_array c;

                //
                // Three dependency measures are calculated:
                // * covariation
                // * Pearson correlation
                // * Spearman rank correlation
                //
                // Result is stored into C, with C[i,j] equal to correlation
                // (covariance) between I-th variable of X and J-th variable of Y.
                //
                covm2(x, y, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[4.100,-3.250],[2.450,-1.500],[13.450,-5.750]]", 0.01);
                pearsoncorrm2(x, y, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[0.519,-0.699],[0.497,-0.518],[0.596,-0.433]]", 0.01);
                spearmancorrm2(x, y, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[0.541,-0.649],[0.216,-0.433],[0.433,-0.135]]", 0.01);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "basestat_d_cm2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST basestat_t_base
        //      Tests ability to detect errors in inputs
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<34; _spoil_scenario++)
        {
            try
            {
                double mean;
                double variance;
                double skewness;
                double kurtosis;
                double adev;
                double p;
                double v;

                //
                // first, we test short form of functions
                //
                real_1d_array x1 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x1);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x1);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x1);
                samplemoments(x1, mean, variance, skewness, kurtosis);
                real_1d_array x2 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==3 )
                    spoil_vector_by_nan(x2);
                if( _spoil_scenario==4 )
                    spoil_vector_by_posinf(x2);
                if( _spoil_scenario==5 )
                    spoil_vector_by_neginf(x2);
                sampleadev(x2, adev);
                real_1d_array x3 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==6 )
                    spoil_vector_by_nan(x3);
                if( _spoil_scenario==7 )
                    spoil_vector_by_posinf(x3);
                if( _spoil_scenario==8 )
                    spoil_vector_by_neginf(x3);
                samplemedian(x3, v);
                real_1d_array x4 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==9 )
                    spoil_vector_by_nan(x4);
                if( _spoil_scenario==10 )
                    spoil_vector_by_posinf(x4);
                if( _spoil_scenario==11 )
                    spoil_vector_by_neginf(x4);
                p = 0.5;
                if( _spoil_scenario==12 )
                    p = fp_nan;
                if( _spoil_scenario==13 )
                    p = fp_posinf;
                if( _spoil_scenario==14 )
                    p = fp_neginf;
                samplepercentile(x4, p, v);

                //
                // and then we test full form
                //
                real_1d_array x5 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(x5);
                if( _spoil_scenario==16 )
                    spoil_vector_by_posinf(x5);
                if( _spoil_scenario==17 )
                    spoil_vector_by_neginf(x5);
                if( _spoil_scenario==18 )
                    spoil_vector_by_deleting_element(x5);
                samplemoments(x5, 10, mean, variance, skewness, kurtosis);
                real_1d_array x6 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==19 )
                    spoil_vector_by_nan(x6);
                if( _spoil_scenario==20 )
                    spoil_vector_by_posinf(x6);
                if( _spoil_scenario==21 )
                    spoil_vector_by_neginf(x6);
                if( _spoil_scenario==22 )
                    spoil_vector_by_deleting_element(x6);
                sampleadev(x6, 10, adev);
                real_1d_array x7 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==23 )
                    spoil_vector_by_nan(x7);
                if( _spoil_scenario==24 )
                    spoil_vector_by_posinf(x7);
                if( _spoil_scenario==25 )
                    spoil_vector_by_neginf(x7);
                if( _spoil_scenario==26 )
                    spoil_vector_by_deleting_element(x7);
                samplemedian(x7, 10, v);
                real_1d_array x8 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==27 )
                    spoil_vector_by_nan(x8);
                if( _spoil_scenario==28 )
                    spoil_vector_by_posinf(x8);
                if( _spoil_scenario==29 )
                    spoil_vector_by_neginf(x8);
                if( _spoil_scenario==30 )
                    spoil_vector_by_deleting_element(x8);
                p = 0.5;
                if( _spoil_scenario==31 )
                    p = fp_nan;
                if( _spoil_scenario==32 )
                    p = fp_posinf;
                if( _spoil_scenario==33 )
                    p = fp_neginf;
                samplepercentile(x8, 10, p, v);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "basestat_t_base");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST basestat_t_covcorr
        //      Tests ability to detect errors in inputs
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<126; _spoil_scenario++)
        {
            try
            {
                double v;
                real_2d_array c;

                //
                // 2-sample short-form cov/corr are tested
                //
                real_1d_array x1 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x1);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x1);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x1);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x1);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x1);
                real_1d_array y1 = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y1);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y1);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y1);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y1);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y1);
                v = cov2(x1, y1);
                real_1d_array x2 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(x2);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(x2);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(x2);
                if( _spoil_scenario==13 )
                    spoil_vector_by_adding_element(x2);
                if( _spoil_scenario==14 )
                    spoil_vector_by_deleting_element(x2);
                real_1d_array y2 = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(y2);
                if( _spoil_scenario==16 )
                    spoil_vector_by_posinf(y2);
                if( _spoil_scenario==17 )
                    spoil_vector_by_neginf(y2);
                if( _spoil_scenario==18 )
                    spoil_vector_by_adding_element(y2);
                if( _spoil_scenario==19 )
                    spoil_vector_by_deleting_element(y2);
                v = pearsoncorr2(x2, y2);
                real_1d_array x3 = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==20 )
                    spoil_vector_by_nan(x3);
                if( _spoil_scenario==21 )
                    spoil_vector_by_posinf(x3);
                if( _spoil_scenario==22 )
                    spoil_vector_by_neginf(x3);
                if( _spoil_scenario==23 )
                    spoil_vector_by_adding_element(x3);
                if( _spoil_scenario==24 )
                    spoil_vector_by_deleting_element(x3);
                real_1d_array y3 = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==25 )
                    spoil_vector_by_nan(y3);
                if( _spoil_scenario==26 )
                    spoil_vector_by_posinf(y3);
                if( _spoil_scenario==27 )
                    spoil_vector_by_neginf(y3);
                if( _spoil_scenario==28 )
                    spoil_vector_by_adding_element(y3);
                if( _spoil_scenario==29 )
                    spoil_vector_by_deleting_element(y3);
                v = spearmancorr2(x3, y3);

                //
                // 2-sample full-form cov/corr are tested
                //
                real_1d_array x1a = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==30 )
                    spoil_vector_by_nan(x1a);
                if( _spoil_scenario==31 )
                    spoil_vector_by_posinf(x1a);
                if( _spoil_scenario==32 )
                    spoil_vector_by_neginf(x1a);
                if( _spoil_scenario==33 )
                    spoil_vector_by_deleting_element(x1a);
                real_1d_array y1a = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==34 )
                    spoil_vector_by_nan(y1a);
                if( _spoil_scenario==35 )
                    spoil_vector_by_posinf(y1a);
                if( _spoil_scenario==36 )
                    spoil_vector_by_neginf(y1a);
                if( _spoil_scenario==37 )
                    spoil_vector_by_deleting_element(y1a);
                v = cov2(x1a, y1a, 10);
                real_1d_array x2a = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==38 )
                    spoil_vector_by_nan(x2a);
                if( _spoil_scenario==39 )
                    spoil_vector_by_posinf(x2a);
                if( _spoil_scenario==40 )
                    spoil_vector_by_neginf(x2a);
                if( _spoil_scenario==41 )
                    spoil_vector_by_deleting_element(x2a);
                real_1d_array y2a = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==42 )
                    spoil_vector_by_nan(y2a);
                if( _spoil_scenario==43 )
                    spoil_vector_by_posinf(y2a);
                if( _spoil_scenario==44 )
                    spoil_vector_by_neginf(y2a);
                if( _spoil_scenario==45 )
                    spoil_vector_by_deleting_element(y2a);
                v = pearsoncorr2(x2a, y2a, 10);
                real_1d_array x3a = "[0,1,4,9,16,25,36,49,64,81]";
                if( _spoil_scenario==46 )
                    spoil_vector_by_nan(x3a);
                if( _spoil_scenario==47 )
                    spoil_vector_by_posinf(x3a);
                if( _spoil_scenario==48 )
                    spoil_vector_by_neginf(x3a);
                if( _spoil_scenario==49 )
                    spoil_vector_by_deleting_element(x3a);
                real_1d_array y3a = "[0,1,2,3,4,5,6,7,8,9]";
                if( _spoil_scenario==50 )
                    spoil_vector_by_nan(y3a);
                if( _spoil_scenario==51 )
                    spoil_vector_by_posinf(y3a);
                if( _spoil_scenario==52 )
                    spoil_vector_by_neginf(y3a);
                if( _spoil_scenario==53 )
                    spoil_vector_by_deleting_element(y3a);
                v = spearmancorr2(x3a, y3a, 10);

                //
                // vector short-form cov/corr are tested.
                //
                real_2d_array x4 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==54 )
                    spoil_matrix_by_nan(x4);
                if( _spoil_scenario==55 )
                    spoil_matrix_by_posinf(x4);
                if( _spoil_scenario==56 )
                    spoil_matrix_by_neginf(x4);
                covm(x4, c);
                real_2d_array x5 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==57 )
                    spoil_matrix_by_nan(x5);
                if( _spoil_scenario==58 )
                    spoil_matrix_by_posinf(x5);
                if( _spoil_scenario==59 )
                    spoil_matrix_by_neginf(x5);
                pearsoncorrm(x5, c);
                real_2d_array x6 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==60 )
                    spoil_matrix_by_nan(x6);
                if( _spoil_scenario==61 )
                    spoil_matrix_by_posinf(x6);
                if( _spoil_scenario==62 )
                    spoil_matrix_by_neginf(x6);
                spearmancorrm(x6, c);

                //
                // vector full-form cov/corr are tested.
                //
                real_2d_array x7 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==63 )
                    spoil_matrix_by_nan(x7);
                if( _spoil_scenario==64 )
                    spoil_matrix_by_posinf(x7);
                if( _spoil_scenario==65 )
                    spoil_matrix_by_neginf(x7);
                if( _spoil_scenario==66 )
                    spoil_matrix_by_deleting_row(x7);
                if( _spoil_scenario==67 )
                    spoil_matrix_by_deleting_col(x7);
                covm(x7, 5, 3, c);
                real_2d_array x8 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==68 )
                    spoil_matrix_by_nan(x8);
                if( _spoil_scenario==69 )
                    spoil_matrix_by_posinf(x8);
                if( _spoil_scenario==70 )
                    spoil_matrix_by_neginf(x8);
                if( _spoil_scenario==71 )
                    spoil_matrix_by_deleting_row(x8);
                if( _spoil_scenario==72 )
                    spoil_matrix_by_deleting_col(x8);
                pearsoncorrm(x8, 5, 3, c);
                real_2d_array x9 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==73 )
                    spoil_matrix_by_nan(x9);
                if( _spoil_scenario==74 )
                    spoil_matrix_by_posinf(x9);
                if( _spoil_scenario==75 )
                    spoil_matrix_by_neginf(x9);
                if( _spoil_scenario==76 )
                    spoil_matrix_by_deleting_row(x9);
                if( _spoil_scenario==77 )
                    spoil_matrix_by_deleting_col(x9);
                spearmancorrm(x9, 5, 3, c);

                //
                // cross-vector short-form cov/corr are tested.
                //
                real_2d_array x10 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==78 )
                    spoil_matrix_by_nan(x10);
                if( _spoil_scenario==79 )
                    spoil_matrix_by_posinf(x10);
                if( _spoil_scenario==80 )
                    spoil_matrix_by_neginf(x10);
                real_2d_array y10 = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==81 )
                    spoil_matrix_by_nan(y10);
                if( _spoil_scenario==82 )
                    spoil_matrix_by_posinf(y10);
                if( _spoil_scenario==83 )
                    spoil_matrix_by_neginf(y10);
                covm2(x10, y10, c);
                real_2d_array x11 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==84 )
                    spoil_matrix_by_nan(x11);
                if( _spoil_scenario==85 )
                    spoil_matrix_by_posinf(x11);
                if( _spoil_scenario==86 )
                    spoil_matrix_by_neginf(x11);
                real_2d_array y11 = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==87 )
                    spoil_matrix_by_nan(y11);
                if( _spoil_scenario==88 )
                    spoil_matrix_by_posinf(y11);
                if( _spoil_scenario==89 )
                    spoil_matrix_by_neginf(y11);
                pearsoncorrm2(x11, y11, c);
                real_2d_array x12 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==90 )
                    spoil_matrix_by_nan(x12);
                if( _spoil_scenario==91 )
                    spoil_matrix_by_posinf(x12);
                if( _spoil_scenario==92 )
                    spoil_matrix_by_neginf(x12);
                real_2d_array y12 = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==93 )
                    spoil_matrix_by_nan(y12);
                if( _spoil_scenario==94 )
                    spoil_matrix_by_posinf(y12);
                if( _spoil_scenario==95 )
                    spoil_matrix_by_neginf(y12);
                spearmancorrm2(x12, y12, c);

                //
                // cross-vector full-form cov/corr are tested.
                //
                real_2d_array x13 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==96 )
                    spoil_matrix_by_nan(x13);
                if( _spoil_scenario==97 )
                    spoil_matrix_by_posinf(x13);
                if( _spoil_scenario==98 )
                    spoil_matrix_by_neginf(x13);
                if( _spoil_scenario==99 )
                    spoil_matrix_by_deleting_row(x13);
                if( _spoil_scenario==100 )
                    spoil_matrix_by_deleting_col(x13);
                real_2d_array y13 = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==101 )
                    spoil_matrix_by_nan(y13);
                if( _spoil_scenario==102 )
                    spoil_matrix_by_posinf(y13);
                if( _spoil_scenario==103 )
                    spoil_matrix_by_neginf(y13);
                if( _spoil_scenario==104 )
                    spoil_matrix_by_deleting_row(y13);
                if( _spoil_scenario==105 )
                    spoil_matrix_by_deleting_col(y13);
                covm2(x13, y13, 5, 3, 2, c);
                real_2d_array x14 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==106 )
                    spoil_matrix_by_nan(x14);
                if( _spoil_scenario==107 )
                    spoil_matrix_by_posinf(x14);
                if( _spoil_scenario==108 )
                    spoil_matrix_by_neginf(x14);
                if( _spoil_scenario==109 )
                    spoil_matrix_by_deleting_row(x14);
                if( _spoil_scenario==110 )
                    spoil_matrix_by_deleting_col(x14);
                real_2d_array y14 = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==111 )
                    spoil_matrix_by_nan(y14);
                if( _spoil_scenario==112 )
                    spoil_matrix_by_posinf(y14);
                if( _spoil_scenario==113 )
                    spoil_matrix_by_neginf(y14);
                if( _spoil_scenario==114 )
                    spoil_matrix_by_deleting_row(y14);
                if( _spoil_scenario==115 )
                    spoil_matrix_by_deleting_col(y14);
                pearsoncorrm2(x14, y14, 5, 3, 2, c);
                real_2d_array x15 = "[[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]";
                if( _spoil_scenario==116 )
                    spoil_matrix_by_nan(x15);
                if( _spoil_scenario==117 )
                    spoil_matrix_by_posinf(x15);
                if( _spoil_scenario==118 )
                    spoil_matrix_by_neginf(x15);
                if( _spoil_scenario==119 )
                    spoil_matrix_by_deleting_row(x15);
                if( _spoil_scenario==120 )
                    spoil_matrix_by_deleting_col(x15);
                real_2d_array y15 = "[[2,3],[2,1],[-1,6],[-9,9],[7,1]]";
                if( _spoil_scenario==121 )
                    spoil_matrix_by_nan(y15);
                if( _spoil_scenario==122 )
                    spoil_matrix_by_posinf(y15);
                if( _spoil_scenario==123 )
                    spoil_matrix_by_neginf(y15);
                if( _spoil_scenario==124 )
                    spoil_matrix_by_deleting_row(y15);
                if( _spoil_scenario==125 )
                    spoil_matrix_by_deleting_col(y15);
                spearmancorrm2(x15, y15, 5, 3, 2, c);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "basestat_t_covcorr");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST filters_d_sma
        //      SMA(k) filter
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // Here we demonstrate SMA(k) filtering for time series.
                //
                real_1d_array x = "[5,6,7,8]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);

                //
                // Apply filter.
                // We should get [5, 5.5, 6.5, 7.5] as result
                //
                filtersma(x, 2);
                _TestResult = _TestResult && doc_test_real_vector(x, "[5,5.5,6.5,7.5]", 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "filters_d_sma");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST filters_d_ema
        //      EMA(alpha) filter
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // Here we demonstrate EMA(0.5) filtering for time series.
                //
                real_1d_array x = "[5,6,7,8]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);

                //
                // Apply filter.
                // We should get [5, 5.5, 6.25, 7.125] as result
                //
                filterema(x, 0.5);
                _TestResult = _TestResult && doc_test_real_vector(x, "[5,5.5,6.25,7.125]", 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "filters_d_ema");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST filters_d_lrma
        //      LRMA(k) filter
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // Here we demonstrate LRMA(3) filtering for time series.
                //
                real_1d_array x = "[7,8,8,9,12,12]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);

                //
                // Apply filter.
                // We should get [7.0000, 8.0000, 8.1667, 8.8333, 11.6667, 12.5000] as result
                //    
                filterlrma(x, 3);
                _TestResult = _TestResult && doc_test_real_vector(x, "[7.0000,8.0000,8.1667,8.8333,11.6667,12.5000]", 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "filters_d_lrma");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_d_r1
        //      Real matrix inverse
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                real_2d_array a = "[[1,-1],[1,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(a);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(a);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(a);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(a);
                ae_int_t info;
                matinvreport rep;
                rmatrixinverse(a, info, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_real_matrix(a, "[[0.5,0.5],[-0.5,0.5]]", 0.00005);
                _TestResult = _TestResult && doc_test_real(rep.r1, 0.5, 0.00005);
                _TestResult = _TestResult && doc_test_real(rep.rinf, 0.5, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_d_r1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_d_c1
        //      Complex matrix inverse
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                complex_2d_array a = "[[1i,-1],[1i,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(a);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(a);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(a);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(a);
                ae_int_t info;
                matinvreport rep;
                cmatrixinverse(a, info, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_complex_matrix(a, "[[-0.5i,-0.5i],[-0.5,0.5]]", 0.00005);
                _TestResult = _TestResult && doc_test_real(rep.r1, 0.5, 0.00005);
                _TestResult = _TestResult && doc_test_real(rep.rinf, 0.5, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_d_c1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_d_spd1
        //      SPD matrix inverse
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                real_2d_array a = "[[2,1],[1,2]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(a);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(a);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(a);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(a);
                ae_int_t info;
                matinvreport rep;
                spdmatrixinverse(a, info, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_real_matrix(a, "[[0.666666,-0.333333],[-0.333333,0.666666]]", 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_d_spd1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_d_hpd1
        //      HPD matrix inverse
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                complex_2d_array a = "[[2,1],[1,2]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(a);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(a);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(a);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(a);
                ae_int_t info;
                matinvreport rep;
                hpdmatrixinverse(a, info, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_complex_matrix(a, "[[0.666666,-0.333333],[-0.333333,0.666666]]", 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_d_hpd1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_t_r1
        //      Real matrix inverse: singular matrix
        //
        _TestResult = true;
        try
        {
            real_2d_array a = "[[1,-1],[-2,2]]";
            ae_int_t info;
            matinvreport rep;
            rmatrixinverse(a, info, rep);
            _TestResult = _TestResult && doc_test_int(info, -3);
            _TestResult = _TestResult && doc_test_real(rep.r1, 0.0, 0.00005);
            _TestResult = _TestResult && doc_test_real(rep.rinf, 0.0, 0.00005);
        }
        catch(ap_error e)
        { _TestResult = false; }
        catch(...)
        { throw; }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_t_r1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_t_c1
        //      Complex matrix inverse: singular matrix
        //
        _TestResult = true;
        try
        {
            complex_2d_array a = "[[1i,-1i],[-2,2]]";
            ae_int_t info;
            matinvreport rep;
            cmatrixinverse(a, info, rep);
            _TestResult = _TestResult && doc_test_int(info, -3);
            _TestResult = _TestResult && doc_test_real(rep.r1, 0.0, 0.00005);
            _TestResult = _TestResult && doc_test_real(rep.rinf, 0.0, 0.00005);
        }
        catch(ap_error e)
        { _TestResult = false; }
        catch(...)
        { throw; }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_t_c1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_e_spd1
        //      Attempt to use SPD function on nonsymmetrix matrix
        //
        _TestResult = true;
        try
        {
            real_2d_array a = "[[1,0],[1,1]]";
            ae_int_t info;
            matinvreport rep;
            spdmatrixinverse(a, info, rep);
            _TestResult = false;
        }
        catch(ap_error e)
        {}
        catch(...)
        { throw; }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_e_spd1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matinv_e_hpd1
        //      Attempt to use SPD function on nonsymmetrix matrix
        //
        _TestResult = true;
        try
        {
            complex_2d_array a = "[[1,0],[1,1]]";
            ae_int_t info;
            matinvreport rep;
            hpdmatrixinverse(a, info, rep);
            _TestResult = false;
        }
        catch(ap_error e)
        {}
        catch(...)
        { throw; }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matinv_e_hpd1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST mincg_d_1
        //      Nonlinear optimization by CG
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // with nonlinear conjugate gradient method.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                mincgstate state;
                mincgreport rep;

                mincgcreate(x, state);
                mincgsetcond(state, epsg, epsf, epsx, maxits);
                alglib::mincgoptimize(state, function1_grad);
                mincgresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "mincg_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST mincg_d_2
        //      Nonlinear optimization with additional settings and restarts
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<18; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // with nonlinear conjugate gradient method.
                //
                // Several advanced techniques are demonstrated:
                // * upper limit on step size
                // * restart from new point
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                double stpmax = 0.1;
                if( _spoil_scenario==12 )
                    stpmax = fp_nan;
                if( _spoil_scenario==13 )
                    stpmax = fp_posinf;
                if( _spoil_scenario==14 )
                    stpmax = fp_neginf;
                ae_int_t maxits = 0;
                mincgstate state;
                mincgreport rep;

                // first run
                mincgcreate(x, state);
                mincgsetcond(state, epsg, epsf, epsx, maxits);
                mincgsetstpmax(state, stpmax);
                alglib::mincgoptimize(state, function1_grad);
                mincgresults(state, x, rep);

                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);

                // second run - algorithm is restarted with mincgrestartfrom()
                x = "[10,10]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==16 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==17 )
                    spoil_vector_by_neginf(x);
                mincgrestartfrom(state, x);
                alglib::mincgoptimize(state, function1_grad);
                mincgresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "mincg_d_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST mincg_numdiff
        //      Nonlinear optimization by CG with numerical differentiation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<15; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // using numerical differentiation to calculate gradient.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                double diffstep = 1.0e-6;
                if( _spoil_scenario==12 )
                    diffstep = fp_nan;
                if( _spoil_scenario==13 )
                    diffstep = fp_posinf;
                if( _spoil_scenario==14 )
                    diffstep = fp_neginf;
                ae_int_t maxits = 0;
                mincgstate state;
                mincgreport rep;

                mincgcreatef(x, diffstep, state);
                mincgsetcond(state, epsg, epsf, epsx, maxits);
                alglib::mincgoptimize(state, function1_func);
                mincgresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "mincg_numdiff");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST mincg_ftrim
        //      Nonlinear optimization by CG, function with singularities
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x) = (1+x)^(-0.2) + (1-x)^(-0.3) + 1000*x.
                // This function has singularities at the boundary of the [-1,+1], but technique called
                // "function trimming" allows us to solve this optimization problem.
                //
                // See http://www.alglib.net/optimization/tipsandtricks.php#ftrimming for more information
                // on this subject.
                //
                real_1d_array x = "[0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 1.0e-6;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                mincgstate state;
                mincgreport rep;

                mincgcreate(x, state);
                mincgsetcond(state, epsg, epsf, epsx, maxits);
                alglib::mincgoptimize(state, s1_grad);
                mincgresults(state, x, rep);

                _TestResult = _TestResult && doc_test_real_vector(x, "[-0.99917305]", 0.000005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "mincg_ftrim");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minbleic_d_1
        //      Nonlinear optimization with bound constraints
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<22; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // subject to bound constraints -1<=x<=+1, -1<=y<=+1, using BLEIC optimizer.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                real_1d_array bndl = "[-1,-1]";
                if( _spoil_scenario==3 )
                    spoil_vector_by_nan(bndl);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(bndl);
                real_1d_array bndu = "[+1,+1]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(bndu);
                if( _spoil_scenario==6 )
                    spoil_vector_by_deleting_element(bndu);
                minbleicstate state;
                minbleicreport rep;

                //
                // These variables define stopping conditions for the underlying CG algorithm.
                // They should be stringent enough in order to guarantee overall stability
                // of the outer iterations.
                //
                // We use very simple condition - |g|<=epsg
                //
                double epsg = 0.000001;
                if( _spoil_scenario==7 )
                    epsg = fp_nan;
                if( _spoil_scenario==8 )
                    epsg = fp_posinf;
                if( _spoil_scenario==9 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==10 )
                    epsf = fp_nan;
                if( _spoil_scenario==11 )
                    epsf = fp_posinf;
                if( _spoil_scenario==12 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==13 )
                    epsx = fp_nan;
                if( _spoil_scenario==14 )
                    epsx = fp_posinf;
                if( _spoil_scenario==15 )
                    epsx = fp_neginf;

                //
                // These variables define stopping conditions for the outer iterations:
                // * epso controls convergence of outer iterations; algorithm will stop
                //   when difference between solutions of subsequent unconstrained problems
                //   will be less than 0.0001
                // * epsi controls amount of infeasibility allowed in the final solution
                //
                double epso = 0.00001;
                if( _spoil_scenario==16 )
                    epso = fp_nan;
                if( _spoil_scenario==17 )
                    epso = fp_posinf;
                if( _spoil_scenario==18 )
                    epso = fp_neginf;
                double epsi = 0.00001;
                if( _spoil_scenario==19 )
                    epsi = fp_nan;
                if( _spoil_scenario==20 )
                    epsi = fp_posinf;
                if( _spoil_scenario==21 )
                    epsi = fp_neginf;

                //
                // Now we are ready to actually optimize something:
                // * first we create optimizer
                // * we add boundary constraints
                // * we tune stopping conditions
                // * and, finally, optimize and obtain results...
                //
                minbleiccreate(x, state);
                minbleicsetbc(state, bndl, bndu);
                minbleicsetinnercond(state, epsg, epsf, epsx);
                minbleicsetoutercond(state, epso, epsi);
                alglib::minbleicoptimize(state, function1_grad);
                minbleicresults(state, x, rep);

                //
                // ...and evaluate these results
                //
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-1,1]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minbleic_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minbleic_d_2
        //      Nonlinear optimization with linear inequality constraints
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<24; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // subject to inequality constraints:
                // * x>=2 (posed as general linear constraint),
                // * x+y>=6
                // using BLEIC optimizer.
                //
                real_1d_array x = "[5,5]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                real_2d_array c = "[[1,0,2],[1,1,6]]";
                if( _spoil_scenario==3 )
                    spoil_matrix_by_nan(c);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_posinf(c);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_neginf(c);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_row(c);
                if( _spoil_scenario==7 )
                    spoil_matrix_by_deleting_col(c);
                integer_1d_array ct = "[1,1]";
                if( _spoil_scenario==8 )
                    spoil_vector_by_deleting_element(ct);
                minbleicstate state;
                minbleicreport rep;

                //
                // These variables define stopping conditions for the underlying CG algorithm.
                // They should be stringent enough in order to guarantee overall stability
                // of the outer iterations.
                //
                // We use very simple condition - |g|<=epsg
                //
                double epsg = 0.000001;
                if( _spoil_scenario==9 )
                    epsg = fp_nan;
                if( _spoil_scenario==10 )
                    epsg = fp_posinf;
                if( _spoil_scenario==11 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==12 )
                    epsf = fp_nan;
                if( _spoil_scenario==13 )
                    epsf = fp_posinf;
                if( _spoil_scenario==14 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==15 )
                    epsx = fp_nan;
                if( _spoil_scenario==16 )
                    epsx = fp_posinf;
                if( _spoil_scenario==17 )
                    epsx = fp_neginf;

                //
                // These variables define stopping conditions for the outer iterations:
                // * epso controls convergence of outer iterations; algorithm will stop
                //   when difference between solutions of subsequent unconstrained problems
                //   will be less than 0.0001
                // * epsi controls amount of infeasibility allowed in the final solution
                //
                double epso = 0.00001;
                if( _spoil_scenario==18 )
                    epso = fp_nan;
                if( _spoil_scenario==19 )
                    epso = fp_posinf;
                if( _spoil_scenario==20 )
                    epso = fp_neginf;
                double epsi = 0.00001;
                if( _spoil_scenario==21 )
                    epsi = fp_nan;
                if( _spoil_scenario==22 )
                    epsi = fp_posinf;
                if( _spoil_scenario==23 )
                    epsi = fp_neginf;

                //
                // Now we are ready to actually optimize something:
                // * first we create optimizer
                // * we add linear constraints
                // * we tune stopping conditions
                // * and, finally, optimize and obtain results...
                //
                minbleiccreate(x, state);
                minbleicsetlc(state, c, ct);
                minbleicsetinnercond(state, epsg, epsf, epsx);
                minbleicsetoutercond(state, epso, epsi);
                alglib::minbleicoptimize(state, function1_grad);
                minbleicresults(state, x, rep);

                //
                // ...and evaluate these results
                //
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[2,4]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minbleic_d_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minbleic_numdiff
        //      Nonlinear optimization with bound constraints and numerical differentiation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<25; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // subject to bound constraints -1<=x<=+1, -1<=y<=+1, using BLEIC optimizer.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                real_1d_array bndl = "[-1,-1]";
                if( _spoil_scenario==3 )
                    spoil_vector_by_nan(bndl);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(bndl);
                real_1d_array bndu = "[+1,+1]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(bndu);
                if( _spoil_scenario==6 )
                    spoil_vector_by_deleting_element(bndu);
                minbleicstate state;
                minbleicreport rep;

                //
                // These variables define stopping conditions for the underlying CG algorithm.
                // They should be stringent enough in order to guarantee overall stability
                // of the outer iterations.
                //
                // We use very simple condition - |g|<=epsg
                //
                double epsg = 0.000001;
                if( _spoil_scenario==7 )
                    epsg = fp_nan;
                if( _spoil_scenario==8 )
                    epsg = fp_posinf;
                if( _spoil_scenario==9 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==10 )
                    epsf = fp_nan;
                if( _spoil_scenario==11 )
                    epsf = fp_posinf;
                if( _spoil_scenario==12 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==13 )
                    epsx = fp_nan;
                if( _spoil_scenario==14 )
                    epsx = fp_posinf;
                if( _spoil_scenario==15 )
                    epsx = fp_neginf;

                //
                // These variables define stopping conditions for the outer iterations:
                // * epso controls convergence of outer iterations; algorithm will stop
                //   when difference between solutions of subsequent unconstrained problems
                //   will be less than 0.0001
                // * epsi controls amount of infeasibility allowed in the final solution
                //
                double epso = 0.00001;
                if( _spoil_scenario==16 )
                    epso = fp_nan;
                if( _spoil_scenario==17 )
                    epso = fp_posinf;
                if( _spoil_scenario==18 )
                    epso = fp_neginf;
                double epsi = 0.00001;
                if( _spoil_scenario==19 )
                    epsi = fp_nan;
                if( _spoil_scenario==20 )
                    epsi = fp_posinf;
                if( _spoil_scenario==21 )
                    epsi = fp_neginf;

                //
                // This variable contains differentiation step
                //
                double diffstep = 1.0e-6;
                if( _spoil_scenario==22 )
                    diffstep = fp_nan;
                if( _spoil_scenario==23 )
                    diffstep = fp_posinf;
                if( _spoil_scenario==24 )
                    diffstep = fp_neginf;

                //
                // Now we are ready to actually optimize something:
                // * first we create optimizer
                // * we add boundary constraints
                // * we tune stopping conditions
                // * and, finally, optimize and obtain results...
                //
                minbleiccreatef(x, diffstep, state);
                minbleicsetbc(state, bndl, bndu);
                minbleicsetinnercond(state, epsg, epsf, epsx);
                minbleicsetoutercond(state, epso, epsi);
                alglib::minbleicoptimize(state, function1_func);
                minbleicresults(state, x, rep);

                //
                // ...and evaluate these results
                //
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-1,1]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minbleic_numdiff");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minbleic_ftrim
        //      Nonlinear optimization by BLEIC, function with singularities
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<18; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x) = (1+x)^(-0.2) + (1-x)^(-0.3) + 1000*x.
                //
                // This function is undefined outside of (-1,+1) and has singularities at x=-1 and x=+1.
                // Special technique called "function trimming" allows us to solve this optimization problem 
                // - without using boundary constraints!
                //
                // See http://www.alglib.net/optimization/tipsandtricks.php#ftrimming for more information
                // on this subject.
                //
                real_1d_array x = "[0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 1.0e-6;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                double epso = 1.0e-6;
                if( _spoil_scenario==12 )
                    epso = fp_nan;
                if( _spoil_scenario==13 )
                    epso = fp_posinf;
                if( _spoil_scenario==14 )
                    epso = fp_neginf;
                double epsi = 1.0e-6;
                if( _spoil_scenario==15 )
                    epsi = fp_nan;
                if( _spoil_scenario==16 )
                    epsi = fp_posinf;
                if( _spoil_scenario==17 )
                    epsi = fp_neginf;
                minbleicstate state;
                minbleicreport rep;

                minbleiccreate(x, state);
                minbleicsetinnercond(state, epsg, epsf, epsx);
                minbleicsetoutercond(state, epso, epsi);
                alglib::minbleicoptimize(state, s1_grad);
                minbleicresults(state, x, rep);

                _TestResult = _TestResult && doc_test_real_vector(x, "[-0.99917305]", 0.000005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minbleic_ftrim");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST mcpd_simple1
        //      Simple unconstrained MCPD model (no entry/exit states)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                //
                // The very simple MCPD example
                //
                // We have a loan portfolio. Our loans can be in one of two states:
                // * normal loans ("good" ones)
                // * past due loans ("bad" ones)
                //
                // We assume that:
                // * loans can transition from any state to any other state. In 
                //   particular, past due loan can become "good" one at any moment 
                //   with same (fixed) probability. Not realistic, but it is toy example :)
                // * portfolio size does not change over time
                //
                // Thus, we have following model
                //     state_new = P*state_old
                // where
                //         ( p00  p01 )
                //     P = (          )
                //         ( p10  p11 )
                //
                // We want to model transitions between these two states using MCPD
                // approach (Markov Chains for Proportional/Population Data), i.e.
                // to restore hidden transition matrix P using actual portfolio data.
                // We have:
                // * poportional data, i.e. proportion of loans in the normal and past 
                //   due states (not portfolio size measured in some currency, although 
                //   it is possible to work with population data too)
                // * two tracks, i.e. two sequences which describe portfolio
                //   evolution from two different starting states: [1,0] (all loans 
                //   are "good") and [0.8,0.2] (only 80% of portfolio is in the "good"
                //   state)
                //
                mcpdstate s;
                mcpdreport rep;
                real_2d_array p;
                real_2d_array track0 = "[[1.00000,0.00000],[0.95000,0.05000],[0.92750,0.07250],[0.91738,0.08263],[0.91282,0.08718]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(track0);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(track0);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(track0);
                real_2d_array track1 = "[[0.80000,0.20000],[0.86000,0.14000],[0.88700,0.11300],[0.89915,0.10085]]";
                if( _spoil_scenario==3 )
                    spoil_matrix_by_nan(track1);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_posinf(track1);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_neginf(track1);

                mcpdcreate(2, s);
                mcpdaddtrack(s, track0);
                mcpdaddtrack(s, track1);
                mcpdsolve(s);
                mcpdresults(s, p, rep);

                //
                // Hidden matrix P is equal to
                //         ( 0.95  0.50 )
                //         (            )
                //         ( 0.05  0.50 )
                // which means that "good" loans can become "bad" with 5% probability, 
                // while "bad" loans will return to good state with 50% probability.
                //
                _TestResult = _TestResult && doc_test_real_matrix(p, "[[0.95,0.50],[0.05,0.50]]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "mcpd_simple1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST mcpd_simple2
        //      Simple MCPD model (no entry/exit states) with equality constraints
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                //
                // Simple MCPD example
                //
                // We have a loan portfolio. Our loans can be in one of three states:
                // * normal loans
                // * past due loans
                // * charged off loans
                //
                // We assume that:
                // * normal loan can stay normal or become past due (but not charged off)
                // * past due loan can stay past due, become normal or charged off
                // * charged off loan will stay charged off for the rest of eternity
                // * portfolio size does not change over time
                // Not realistic, but it is toy example :)
                //
                // Thus, we have following model
                //     state_new = P*state_old
                // where
                //         ( p00  p01    )
                //     P = ( p10  p11    )
                //         (      p21  1 )
                // i.e. four elements of P are known a priori.
                //
                // Although it is possible (given enough data) to In order to enforce 
                // this property we set equality constraints on these elements.
                //
                // We want to model transitions between these two states using MCPD
                // approach (Markov Chains for Proportional/Population Data), i.e.
                // to restore hidden transition matrix P using actual portfolio data.
                // We have:
                // * poportional data, i.e. proportion of loans in the current and past 
                //   due states (not portfolio size measured in some currency, although 
                //   it is possible to work with population data too)
                // * two tracks, i.e. two sequences which describe portfolio
                //   evolution from two different starting states: [1,0,0] (all loans 
                //   are "good") and [0.8,0.2,0.0] (only 80% of portfolio is in the "good"
                //   state)
                //
                mcpdstate s;
                mcpdreport rep;
                real_2d_array p;
                real_2d_array track0 = "[[1.000000,0.000000,0.000000],[0.950000,0.050000,0.000000],[0.927500,0.060000,0.012500],[0.911125,0.061375,0.027500],[0.896256,0.060900,0.042844]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(track0);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(track0);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(track0);
                real_2d_array track1 = "[[0.800000,0.200000,0.000000],[0.860000,0.090000,0.050000],[0.862000,0.065500,0.072500],[0.851650,0.059475,0.088875],[0.838805,0.057451,0.103744]]";
                if( _spoil_scenario==3 )
                    spoil_matrix_by_nan(track1);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_posinf(track1);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_neginf(track1);

                mcpdcreate(3, s);
                mcpdaddtrack(s, track0);
                mcpdaddtrack(s, track1);
                mcpdaddec(s, 0, 2, 0.0);
                mcpdaddec(s, 1, 2, 0.0);
                mcpdaddec(s, 2, 2, 1.0);
                mcpdaddec(s, 2, 0, 0.0);
                mcpdsolve(s);
                mcpdresults(s, p, rep);

                //
                // Hidden matrix P is equal to
                //         ( 0.95 0.50      )
                //         ( 0.05 0.25      )
                //         (      0.25 1.00 ) 
                // which means that "good" loans can become past due with 5% probability, 
                // while past due loans will become charged off with 25% probability or
                // return back to normal state with 50% probability.
                //
                _TestResult = _TestResult && doc_test_real_matrix(p, "[[0.95,0.50,0.00],[0.05,0.25,0.00],[0.00,0.25,1.00]]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "mcpd_simple2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlbfgs_d_1
        //      Nonlinear optimization by L-BFGS
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // using LBFGS method.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlbfgsstate state;
                minlbfgsreport rep;

                minlbfgscreate(1, x, state);
                minlbfgssetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlbfgsoptimize(state, function1_grad);
                minlbfgsresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlbfgs_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlbfgs_d_2
        //      Nonlinear optimization with additional settings and restarts
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<18; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // using LBFGS method.
                //
                // Several advanced techniques are demonstrated:
                // * upper limit on step size
                // * restart from new point
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                double stpmax = 0.1;
                if( _spoil_scenario==12 )
                    stpmax = fp_nan;
                if( _spoil_scenario==13 )
                    stpmax = fp_posinf;
                if( _spoil_scenario==14 )
                    stpmax = fp_neginf;
                ae_int_t maxits = 0;
                minlbfgsstate state;
                minlbfgsreport rep;

                // first run
                minlbfgscreate(1, x, state);
                minlbfgssetcond(state, epsg, epsf, epsx, maxits);
                minlbfgssetstpmax(state, stpmax);
                alglib::minlbfgsoptimize(state, function1_grad);
                minlbfgsresults(state, x, rep);

                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);

                // second run - algorithm is restarted
                x = "[10,10]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==16 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==17 )
                    spoil_vector_by_neginf(x);
                minlbfgsrestartfrom(state, x);
                alglib::minlbfgsoptimize(state, function1_grad);
                minlbfgsresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlbfgs_d_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlbfgs_numdiff
        //      Nonlinear optimization by L-BFGS with numerical differentiation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<15; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
                // using numerical differentiation to calculate gradient.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                double diffstep = 1.0e-6;
                if( _spoil_scenario==12 )
                    diffstep = fp_nan;
                if( _spoil_scenario==13 )
                    diffstep = fp_posinf;
                if( _spoil_scenario==14 )
                    diffstep = fp_neginf;
                ae_int_t maxits = 0;
                minlbfgsstate state;
                minlbfgsreport rep;

                minlbfgscreatef(1, x, diffstep, state);
                minlbfgssetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlbfgsoptimize(state, function1_func);
                minlbfgsresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlbfgs_numdiff");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlbfgs_ftrim
        //      Nonlinear optimization by LBFGS, function with singularities
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of f(x) = (1+x)^(-0.2) + (1-x)^(-0.3) + 1000*x.
                // This function has singularities at the boundary of the [-1,+1], but technique called
                // "function trimming" allows us to solve this optimization problem.
                //
                // See http://www.alglib.net/optimization/tipsandtricks.php#ftrimming for more information
                // on this subject.
                //
                real_1d_array x = "[0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 1.0e-6;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlbfgsstate state;
                minlbfgsreport rep;

                minlbfgscreate(1, x, state);
                minlbfgssetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlbfgsoptimize(state, s1_grad);
                minlbfgsresults(state, x, rep);

                _TestResult = _TestResult && doc_test_real_vector(x, "[-0.99917305]", 0.000005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlbfgs_ftrim");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST odesolver_d1
        //      Solving y'=-y with ODE solver
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<13; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[1]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array x = "[0, 1, 2, 3]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(x);
                double eps = 0.00001;
                if( _spoil_scenario==7 )
                    eps = fp_nan;
                if( _spoil_scenario==8 )
                    eps = fp_posinf;
                if( _spoil_scenario==9 )
                    eps = fp_neginf;
                double h = 0;
                if( _spoil_scenario==10 )
                    h = fp_nan;
                if( _spoil_scenario==11 )
                    h = fp_posinf;
                if( _spoil_scenario==12 )
                    h = fp_neginf;
                odesolverstate s;
                ae_int_t m;
                real_1d_array xtbl;
                real_2d_array ytbl;
                odesolverreport rep;
                odesolverrkck(y, x, eps, h, s);
                alglib::odesolversolve(s, ode_function_1_diff);
                odesolverresults(s, m, xtbl, ytbl, rep);
                _TestResult = _TestResult && doc_test_int(m, 4);
                _TestResult = _TestResult && doc_test_real_vector(xtbl, "[0, 1, 2, 3]", 0.005);
                _TestResult = _TestResult && doc_test_real_matrix(ytbl, "[[1], [0.367], [0.135], [0.050]]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "odesolver_d1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST fft_complex_d1
        //      Complex FFT: simple example
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // first we demonstrate forward FFT:
                // [1i,1i,1i,1i] is converted to [4i, 0, 0, 0]
                //
                complex_1d_array z = "[1i,1i,1i,1i]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(z);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(z);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(z);
                fftc1d(z);
                _TestResult = _TestResult && doc_test_complex_vector(z, "[4i,0,0,0]", 0.0001);

                //
                // now we convert [4i, 0, 0, 0] back to [1i,1i,1i,1i]
                // with backward FFT
                //
                fftc1dinv(z);
                _TestResult = _TestResult && doc_test_complex_vector(z, "[1i,1i,1i,1i]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "fft_complex_d1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST fft_complex_d2
        //      Complex FFT: advanced example
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // first we demonstrate forward FFT:
                // [0,1,0,1i] is converted to [1+1i, -1-1i, -1-1i, 1+1i]
                //
                complex_1d_array z = "[0,1,0,1i]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(z);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(z);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(z);
                fftc1d(z);
                _TestResult = _TestResult && doc_test_complex_vector(z, "[1+1i, -1-1i, -1-1i, 1+1i]", 0.0001);

                //
                // now we convert result back with backward FFT
                //
                fftc1dinv(z);
                _TestResult = _TestResult && doc_test_complex_vector(z, "[0,1,0,1i]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "fft_complex_d2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST fft_real_d1
        //      Real FFT: simple example
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // first we demonstrate forward FFT:
                // [1,1,1,1] is converted to [4, 0, 0, 0]
                //
                real_1d_array x = "[1,1,1,1]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                complex_1d_array f;
                real_1d_array x2;
                fftr1d(x, f);
                _TestResult = _TestResult && doc_test_complex_vector(f, "[4,0,0,0]", 0.0001);

                //
                // now we convert [4, 0, 0, 0] back to [1,1,1,1]
                // with backward FFT
                //
                fftr1dinv(f, x2);
                _TestResult = _TestResult && doc_test_real_vector(x2, "[1,1,1,1]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "fft_real_d1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST fft_real_d2
        //      Real FFT: advanced example
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // first we demonstrate forward FFT:
                // [1,2,3,4] is converted to [10, -2+2i, -2, -2-2i]
                //
                // note that output array is self-adjoint:
                // * f[0] = conj(f[0])
                // * f[1] = conj(f[3])
                // * f[2] = conj(f[2])
                //
                real_1d_array x = "[1,2,3,4]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                complex_1d_array f;
                real_1d_array x2;
                fftr1d(x, f);
                _TestResult = _TestResult && doc_test_complex_vector(f, "[10, -2+2i, -2, -2-2i]", 0.0001);

                //
                // now we convert [10, -2+2i, -2, -2-2i] back to [1,2,3,4]
                //
                fftr1dinv(f, x2);
                _TestResult = _TestResult && doc_test_real_vector(x2, "[1,2,3,4]", 0.0001);

                //
                // remember that F is self-adjoint? It means that we can pass just half
                // (slightly larger than half) of F to inverse real FFT and still get our result.
                //
                // I.e. instead [10, -2+2i, -2, -2-2i] we pass just [10, -2+2i, -2] and everything works!
                //
                // NOTE: in this case we should explicitly pass array length (which is 4) to ALGLIB;
                // if not, it will automatically use array length to determine FFT size and
                // will erroneously make half-length FFT.
                //
                f = "[10, -2+2i, -2]";
                fftr1dinv(f, 4, x2);
                _TestResult = _TestResult && doc_test_real_vector(x2, "[1,2,3,4]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "fft_real_d2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST fft_complex_e1
        //      error detection in backward FFT
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                complex_1d_array z = "[0,2,0,-2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(z);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(z);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(z);
                fftc1dinv(z);
                _TestResult = _TestResult && doc_test_complex_vector(z, "[0,1i,0,-1i]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "fft_complex_e1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST autogk_d1
        //      Integrating f=exp(x) by adaptive integrator
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates integration of f=exp(x) on [0,1]:
                // * first, autogkstate is initialized
                // * then we call integration function
                // * and finally we obtain results with autogkresults() call
                //
                double a = 0;
                if( _spoil_scenario==0 )
                    a = fp_nan;
                if( _spoil_scenario==1 )
                    a = fp_posinf;
                if( _spoil_scenario==2 )
                    a = fp_neginf;
                double b = 1;
                if( _spoil_scenario==3 )
                    b = fp_nan;
                if( _spoil_scenario==4 )
                    b = fp_posinf;
                if( _spoil_scenario==5 )
                    b = fp_neginf;
                autogkstate s;
                double v;
                autogkreport rep;

                autogksmooth(a, b, s);
                alglib::autogkintegrate(s, int_function_1_func);
                autogkresults(s, v, rep);

                _TestResult = _TestResult && doc_test_real(v, 1.7182, 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "autogk_d1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_d_calcdiff
        //      Interpolation and differentiation using barycentric representation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // Here we demonstrate polynomial interpolation and differentiation
                // of y=x^2-x sampled at [0,1,2]. Barycentric representation of polynomial is used.
                //
                real_1d_array x = "[0,1,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                double t = -1;
                if( _spoil_scenario==10 )
                    t = fp_posinf;
                if( _spoil_scenario==11 )
                    t = fp_neginf;
                double v;
                double dv;
                double d2v;
                barycentricinterpolant p;

                // barycentric model is created
                polynomialbuild(x, y, p);

                // barycentric interpolation is demonstrated
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);

                // barycentric differentation is demonstrated
                barycentricdiff1(p, t, v, dv);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && doc_test_real(dv, -3.0, 0.00005);

                // second derivatives with barycentric representation
                barycentricdiff1(p, t, v, dv);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && doc_test_real(dv, -3.0, 0.00005);
                barycentricdiff2(p, t, v, dv, d2v);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && doc_test_real(dv, -3.0, 0.00005);
                _TestResult = _TestResult && doc_test_real(d2v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_d_calcdiff");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_d_conv
        //      Conversion between power basis and barycentric representation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                //
                // Here we demonstrate conversion of y=x^2-x
                // between power basis and barycentric representation.
                //
                real_1d_array a = "[0,-1,+1]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(a);
                double t = 2;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                real_1d_array a2;
                double v;
                barycentricinterpolant p;

                //
                // a=[0,-1,+1] is decomposition of y=x^2-x in the power basis:
                //
                //     y = 0 - 1*x + 1*x^2
                //
                // We convert it to the barycentric form.
                //
                polynomialpow2bar(a, p);

                // now we have barycentric interpolation; we can use it for interpolation
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.005);

                // we can also convert back from barycentric representation to power basis
                polynomialbar2pow(p, a2);
                _TestResult = _TestResult && doc_test_real_vector(a2, "[0,-1,+1]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_d_conv");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_d_spec
        //      Polynomial interpolation on special grids (equidistant, Chebyshev I/II)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<11; _spoil_scenario++)
        {
            try
            {
                //
                // Temporaries:
                // * values of y=x^2-x sampled at three special grids:
                //   * equdistant grid spanning [0,2],     x[i] = 2*i/(N-1), i=0..N-1
                //   * Chebyshev-I grid spanning [-1,+1],  x[i] = 1 + Cos(PI*(2*i+1)/(2*n)), i=0..N-1
                //   * Chebyshev-II grid spanning [-1,+1], x[i] = 1 + Cos(PI*i/(n-1)), i=0..N-1
                // * barycentric interpolants for these three grids
                // * vectors to store coefficients of quadratic representation
                //
                real_1d_array y_eqdist = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y_eqdist);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y_eqdist);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y_eqdist);
                real_1d_array y_cheb1 = "[-0.116025,0.000000,1.616025]";
                if( _spoil_scenario==3 )
                    spoil_vector_by_nan(y_cheb1);
                if( _spoil_scenario==4 )
                    spoil_vector_by_posinf(y_cheb1);
                if( _spoil_scenario==5 )
                    spoil_vector_by_neginf(y_cheb1);
                real_1d_array y_cheb2 = "[0,0,2]";
                if( _spoil_scenario==6 )
                    spoil_vector_by_nan(y_cheb2);
                if( _spoil_scenario==7 )
                    spoil_vector_by_posinf(y_cheb2);
                if( _spoil_scenario==8 )
                    spoil_vector_by_neginf(y_cheb2);
                barycentricinterpolant p_eqdist;
                barycentricinterpolant p_cheb1;
                barycentricinterpolant p_cheb2;
                real_1d_array a_eqdist;
                real_1d_array a_cheb1;
                real_1d_array a_cheb2;

                //
                // First, we demonstrate construction of barycentric interpolants on
                // special grids. We unpack power representation to ensure that
                // interpolant was built correctly.
                //
                // In all three cases we should get same quadratic function.
                //
                polynomialbuildeqdist(0.0, 2.0, y_eqdist, p_eqdist);
                polynomialbar2pow(p_eqdist, a_eqdist);
                _TestResult = _TestResult && doc_test_real_vector(a_eqdist, "[0,-1,+1]", 0.00005);

                polynomialbuildcheb1(-1, +1, y_cheb1, p_cheb1);
                polynomialbar2pow(p_cheb1, a_cheb1);
                _TestResult = _TestResult && doc_test_real_vector(a_cheb1, "[0,-1,+1]", 0.00005);

                polynomialbuildcheb2(-1, +1, y_cheb2, p_cheb2);
                polynomialbar2pow(p_cheb2, a_cheb2);
                _TestResult = _TestResult && doc_test_real_vector(a_cheb2, "[0,-1,+1]", 0.00005);

                //
                // Now we demonstrate polynomial interpolation without construction 
                // of the barycentricinterpolant structure.
                //
                // We calculate interpolant value at x=-2.
                // In all three cases we should get same f=6
                //
                double t = -2;
                if( _spoil_scenario==9 )
                    t = fp_posinf;
                if( _spoil_scenario==10 )
                    t = fp_neginf;
                double v;
                v = polynomialcalceqdist(0.0, 2.0, y_eqdist, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);

                v = polynomialcalccheb1(-1, +1, y_cheb1, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);

                v = polynomialcalccheb2(-1, +1, y_cheb2, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_d_spec");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_1
        //      Polynomial interpolation, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<10; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[0,1,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y);
                double t = -1;
                if( _spoil_scenario==8 )
                    t = fp_posinf;
                if( _spoil_scenario==9 )
                    t = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuild(x, y, 3, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_2
        //      Polynomial interpolation, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                double t = -1;
                if( _spoil_scenario==4 )
                    t = fp_posinf;
                if( _spoil_scenario==5 )
                    t = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuildeqdist(0.0, 2.0, y, 3, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_3
        //      Polynomial interpolation, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[-0.116025,0.000000,1.616025]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                double t = -1;
                if( _spoil_scenario==4 )
                    t = fp_posinf;
                if( _spoil_scenario==5 )
                    t = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuildcheb1(-1.0, +1.0, y, 3, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_3");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_4
        //      Polynomial interpolation, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                double t = -2;
                if( _spoil_scenario==4 )
                    t = fp_posinf;
                if( _spoil_scenario==5 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==6 )
                    a = fp_nan;
                if( _spoil_scenario==7 )
                    a = fp_posinf;
                if( _spoil_scenario==8 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==9 )
                    b = fp_nan;
                if( _spoil_scenario==10 )
                    b = fp_posinf;
                if( _spoil_scenario==11 )
                    b = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuildcheb2(a, b, y, 3, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_4");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_5
        //      Polynomial interpolation, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                double t = -1;
                if( _spoil_scenario==4 )
                    t = fp_posinf;
                if( _spoil_scenario==5 )
                    t = fp_neginf;
                double v;
                v = polynomialcalceqdist(0.0, 2.0, y, 3, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_5");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_6
        //      Polynomial interpolation, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[-0.116025,0.000000,1.616025]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                double t = -1;
                if( _spoil_scenario==4 )
                    t = fp_posinf;
                if( _spoil_scenario==5 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==6 )
                    a = fp_nan;
                if( _spoil_scenario==7 )
                    a = fp_posinf;
                if( _spoil_scenario==8 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==9 )
                    b = fp_nan;
                if( _spoil_scenario==10 )
                    b = fp_posinf;
                if( _spoil_scenario==11 )
                    b = fp_neginf;
                double v;
                v = polynomialcalccheb1(a, b, y, 3, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_6");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_7
        //      Polynomial interpolation, full list of parameters.
        //
        printf("50/104\n");
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(y);
                double t = -2;
                if( _spoil_scenario==4 )
                    t = fp_posinf;
                if( _spoil_scenario==5 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==6 )
                    a = fp_nan;
                if( _spoil_scenario==7 )
                    a = fp_posinf;
                if( _spoil_scenario==8 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==9 )
                    b = fp_nan;
                if( _spoil_scenario==10 )
                    b = fp_posinf;
                if( _spoil_scenario==11 )
                    b = fp_neginf;
                double v;
                v = polynomialcalccheb2(a, b, y, 3, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_7");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_8
        //      Polynomial interpolation: y=x^2-x, equidistant grid, barycentric form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                double t = -1;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuildeqdist(0.0, 2.0, y, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_8");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_9
        //      Polynomial interpolation: y=x^2-x, Chebyshev grid (first kind), barycentric form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<11; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[-0.116025,0.000000,1.616025]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                double t = -1;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==5 )
                    a = fp_nan;
                if( _spoil_scenario==6 )
                    a = fp_posinf;
                if( _spoil_scenario==7 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==8 )
                    b = fp_nan;
                if( _spoil_scenario==9 )
                    b = fp_posinf;
                if( _spoil_scenario==10 )
                    b = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuildcheb1(a, b, y, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_9");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_10
        //      Polynomial interpolation: y=x^2-x, Chebyshev grid (second kind), barycentric form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<11; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                double t = -2;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==5 )
                    a = fp_nan;
                if( _spoil_scenario==6 )
                    a = fp_posinf;
                if( _spoil_scenario==7 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==8 )
                    b = fp_nan;
                if( _spoil_scenario==9 )
                    b = fp_posinf;
                if( _spoil_scenario==10 )
                    b = fp_neginf;
                barycentricinterpolant p;
                double v;
                polynomialbuildcheb2(a, b, y, p);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_10");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_11
        //      Polynomial interpolation: y=x^2-x, equidistant grid
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                double t = -1;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                double v;
                v = polynomialcalceqdist(0.0, 2.0, y, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_11");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_12
        //      Polynomial interpolation: y=x^2-x, Chebyshev grid (first kind)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<11; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[-0.116025,0.000000,1.616025]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                double t = -1;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==5 )
                    a = fp_nan;
                if( _spoil_scenario==6 )
                    a = fp_posinf;
                if( _spoil_scenario==7 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==8 )
                    b = fp_nan;
                if( _spoil_scenario==9 )
                    b = fp_posinf;
                if( _spoil_scenario==10 )
                    b = fp_neginf;
                double v;
                v = polynomialcalccheb1(a, b, y, t);
                _TestResult = _TestResult && doc_test_real(v, 2.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_12");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST polint_t_13
        //      Polynomial interpolation: y=x^2-x, Chebyshev grid (second kind)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<11; _spoil_scenario++)
        {
            try
            {
                real_1d_array y = "[0,0,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                double t = -2;
                if( _spoil_scenario==3 )
                    t = fp_posinf;
                if( _spoil_scenario==4 )
                    t = fp_neginf;
                double a = -1;
                if( _spoil_scenario==5 )
                    a = fp_nan;
                if( _spoil_scenario==6 )
                    a = fp_posinf;
                if( _spoil_scenario==7 )
                    a = fp_neginf;
                double b = +1;
                if( _spoil_scenario==8 )
                    b = fp_nan;
                if( _spoil_scenario==9 )
                    b = fp_posinf;
                if( _spoil_scenario==10 )
                    b = fp_neginf;
                double v;
                v = polynomialcalccheb2(a, b, y, t);
                _TestResult = _TestResult && doc_test_real(v, 6.0, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "polint_t_13");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST spline1d_d_linear
        //      Piecewise linear spline interpolation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // We use piecewise linear spline to interpolate f(x)=x^2 sampled 
                // at 5 equidistant nodes on [-1,+1].
                //
                real_1d_array x = "[-1.0,-0.5,0.0,+0.5,+1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[+1.0,0.25,0.0,0.25,+1.0]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                double t = 0.25;
                if( _spoil_scenario==10 )
                    t = fp_posinf;
                if( _spoil_scenario==11 )
                    t = fp_neginf;
                double v;
                spline1dinterpolant s;

                // build spline
                spline1dbuildlinear(x, y, s);

                // calculate S(0.25) - it is quite different from 0.25^2=0.0625
                v = spline1dcalc(s, t);
                _TestResult = _TestResult && doc_test_real(v, 0.125, 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "spline1d_d_linear");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST spline1d_d_cubic
        //      Cubic spline interpolation
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<10; _spoil_scenario++)
        {
            try
            {
                //
                // We use cubic spline to interpolate f(x)=x^2 sampled 
                // at 5 equidistant nodes on [-1,+1].
                //
                // First, we use default boundary conditions ("parabolically terminated
                // spline") because cubic spline built with such boundary conditions 
                // will exactly reproduce any quadratic f(x).
                //
                // Then we try to use natural boundary conditions
                //     d2S(-1)/dx^2 = 0.0
                //     d2S(+1)/dx^2 = 0.0
                // and see that such spline interpolated f(x) with small error.
                //
                real_1d_array x = "[-1.0,-0.5,0.0,+0.5,+1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[+1.0,0.25,0.0,0.25,+1.0]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y);
                double t = 0.25;
                if( _spoil_scenario==8 )
                    t = fp_posinf;
                if( _spoil_scenario==9 )
                    t = fp_neginf;
                double v;
                spline1dinterpolant s;
                ae_int_t natural_bound_type = 2;
                //
                // Test exact boundary conditions: build S(x), calculare S(0.25)
                // (almost same as original function)
                //
                spline1dbuildcubic(x, y, s);
                v = spline1dcalc(s, t);
                _TestResult = _TestResult && doc_test_real(v, 0.0625, 0.00001);

                //
                // Test natural boundary conditions: build S(x), calculare S(0.25)
                // (small interpolation error)
                //
                spline1dbuildcubic(x, y, 5, natural_bound_type, 0.0, natural_bound_type, 0.0, s);
                v = spline1dcalc(s, t);
                _TestResult = _TestResult && doc_test_real(v, 0.0580, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "spline1d_d_cubic");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST spline1d_d_griddiff
        //      Differentiation on the grid using cubic splines
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<10; _spoil_scenario++)
        {
            try
            {
                //
                // We use cubic spline to do grid differentiation, i.e. having
                // values of f(x)=x^2 sampled at 5 equidistant nodes on [-1,+1]
                // we calculate derivatives of cubic spline at nodes WITHOUT
                // CONSTRUCTION OF SPLINE OBJECT.
                //
                // There are efficient functions spline1dgriddiffcubic() and
                // spline1dgriddiff2cubic() for such calculations.
                //
                // We use default boundary conditions ("parabolically terminated
                // spline") because cubic spline built with such boundary conditions 
                // will exactly reproduce any quadratic f(x).
                //
                // Actually, we could use natural conditions, but we feel that 
                // spline which exactly reproduces f() will show us more 
                // understandable results.
                //
                real_1d_array x = "[-1.0,-0.5,0.0,+0.5,+1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[+1.0,0.25,0.0,0.25,+1.0]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array d1;
                real_1d_array d2;

                //
                // We calculate first derivatives: they must be equal to 2*x
                //
                spline1dgriddiffcubic(x, y, d1);
                _TestResult = _TestResult && doc_test_real_vector(d1, "[-2.0, -1.0, 0.0, +1.0, +2.0]", 0.0001);

                //
                // Now test griddiff2, which returns first AND second derivatives.
                // First derivative is 2*x, second is equal to 2.0
                //
                spline1dgriddiff2cubic(x, y, d1, d2);
                _TestResult = _TestResult && doc_test_real_vector(d1, "[-2.0, -1.0, 0.0, +1.0, +2.0]", 0.0001);
                _TestResult = _TestResult && doc_test_real_vector(d2, "[ 2.0,  2.0, 2.0,  2.0,  2.0]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "spline1d_d_griddiff");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST spline1d_d_convdiff
        //      Resampling using cubic splines
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<11; _spoil_scenario++)
        {
            try
            {
                //
                // We use cubic spline to do resampling, i.e. having
                // values of f(x)=x^2 sampled at 5 equidistant nodes on [-1,+1]
                // we calculate values/derivatives of cubic spline on 
                // another grid (equidistant with 9 nodes on [-1,+1])
                // WITHOUT CONSTRUCTION OF SPLINE OBJECT.
                //
                // There are efficient functions spline1dconvcubic(),
                // spline1dconvdiffcubic() and spline1dconvdiff2cubic() 
                // for such calculations.
                //
                // We use default boundary conditions ("parabolically terminated
                // spline") because cubic spline built with such boundary conditions 
                // will exactly reproduce any quadratic f(x).
                //
                // Actually, we could use natural conditions, but we feel that 
                // spline which exactly reproduces f() will show us more 
                // understandable results.
                //
                real_1d_array x_old = "[-1.0,-0.5,0.0,+0.5,+1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x_old);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x_old);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x_old);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(x_old);
                real_1d_array y_old = "[+1.0,0.25,0.0,0.25,+1.0]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(y_old);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(y_old);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(y_old);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y_old);
                real_1d_array x_new = "[-1.00,-0.75,-0.50,-0.25,0.00,+0.25,+0.50,+0.75,+1.00]";
                if( _spoil_scenario==8 )
                    spoil_vector_by_nan(x_new);
                if( _spoil_scenario==9 )
                    spoil_vector_by_posinf(x_new);
                if( _spoil_scenario==10 )
                    spoil_vector_by_neginf(x_new);
                real_1d_array y_new;
                real_1d_array d1_new;
                real_1d_array d2_new;

                //
                // First, conversion without differentiation.
                //
                //
                spline1dconvcubic(x_old, y_old, x_new, y_new);
                _TestResult = _TestResult && doc_test_real_vector(y_new, "[1.0000, 0.5625, 0.2500, 0.0625, 0.0000, 0.0625, 0.2500, 0.5625, 1.0000]", 0.0001);

                //
                // Then, conversion with differentiation (first derivatives only)
                //
                //
                spline1dconvdiffcubic(x_old, y_old, x_new, y_new, d1_new);
                _TestResult = _TestResult && doc_test_real_vector(y_new, "[1.0000, 0.5625, 0.2500, 0.0625, 0.0000, 0.0625, 0.2500, 0.5625, 1.0000]", 0.0001);
                _TestResult = _TestResult && doc_test_real_vector(d1_new, "[-2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0]", 0.0001);

                //
                // Finally, conversion with first and second derivatives
                //
                //
                spline1dconvdiff2cubic(x_old, y_old, x_new, y_new, d1_new, d2_new);
                _TestResult = _TestResult && doc_test_real_vector(y_new, "[1.0000, 0.5625, 0.2500, 0.0625, 0.0000, 0.0625, 0.2500, 0.5625, 1.0000]", 0.0001);
                _TestResult = _TestResult && doc_test_real_vector(d1_new, "[-2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0]", 0.0001);
                _TestResult = _TestResult && doc_test_real_vector(d2_new, "[2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]", 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "spline1d_d_convdiff");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minqp_d_u1
        //      Unconstrained dense quadratic programming
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<13; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = x0^2 + x1^2 -6*x0 - 4*x1
                //
                // Exact solution is [x0,x1] = [3,2]
                //
                // We provide algorithm with starting point, although in this case
                // (dense matrix, no constraints) it can work without such information.
                //
                // IMPORTANT: this solver minimizes  following  function:
                //     f(x) = 0.5*x'*A*x + b'*x.
                // Note that quadratic term has 0.5 before it. So if you want to minimize
                // quadratic function, you should rewrite it in such way that quadratic term
                // is multiplied by 0.5 too.
                // For example, our function is f(x)=x0^2+x1^2+..., but we rewrite it as 
                //     f(x) = 0.5*(2*x0^2+2*x1^2) + ....
                // and pass diag(2,2) as quadratic term - NOT diag(1,1)!
                //
                real_2d_array a = "[[2,0],[0,2]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(a);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(a);
                real_1d_array b = "[-6,-4]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(b);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(b);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(b);
                if( _spoil_scenario==8 )
                    spoil_vector_by_deleting_element(b);
                real_1d_array x0 = "[0,1]";
                if( _spoil_scenario==9 )
                    spoil_vector_by_nan(x0);
                if( _spoil_scenario==10 )
                    spoil_vector_by_posinf(x0);
                if( _spoil_scenario==11 )
                    spoil_vector_by_neginf(x0);
                if( _spoil_scenario==12 )
                    spoil_vector_by_deleting_element(x0);
                real_1d_array x;
                minqpstate state;
                minqpreport rep;

                minqpcreate(2, state);
                minqpsetquadraticterm(state, a);
                minqpsetlinearterm(state, b);
                minqpsetstartingpoint(state, x0);
                minqpoptimize(state);
                minqpresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[3,2]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minqp_d_u1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minqp_d_bc1
        //      Constrained dense quadratic programming
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<17; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = x0^2 + x1^2 -6*x0 - 4*x1
                // subject to bound constraints 0<=x0<=2.5, 0<=x1<=2.5
                //
                // Exact solution is [x0,x1] = [2.5,2]
                //
                // We provide algorithm with starting point. With such small problem good starting
                // point is not really necessary, but with high-dimensional problem it can save us
                // a lot of time.
                //
                // IMPORTANT: this solver minimizes  following  function:
                //     f(x) = 0.5*x'*A*x + b'*x.
                // Note that quadratic term has 0.5 before it. So if you want to minimize
                // quadratic function, you should rewrite it in such way that quadratic term
                // is multiplied by 0.5 too.
                // For example, our function is f(x)=x0^2+x1^2+..., but we rewrite it as 
                //     f(x) = 0.5*(2*x0^2+2*x1^2) + ....
                // and pass diag(2,2) as quadratic term - NOT diag(1,1)!
                //
                real_2d_array a = "[[2,0],[0,2]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(a);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(a);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(a);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(a);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(a);
                real_1d_array b = "[-6,-4]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(b);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(b);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(b);
                if( _spoil_scenario==8 )
                    spoil_vector_by_deleting_element(b);
                real_1d_array x0 = "[0,1]";
                if( _spoil_scenario==9 )
                    spoil_vector_by_nan(x0);
                if( _spoil_scenario==10 )
                    spoil_vector_by_posinf(x0);
                if( _spoil_scenario==11 )
                    spoil_vector_by_neginf(x0);
                if( _spoil_scenario==12 )
                    spoil_vector_by_deleting_element(x0);
                real_1d_array bndl = "[0.0,0.0]";
                if( _spoil_scenario==13 )
                    spoil_vector_by_nan(bndl);
                if( _spoil_scenario==14 )
                    spoil_vector_by_deleting_element(bndl);
                real_1d_array bndu = "[2.5,2.5]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(bndu);
                if( _spoil_scenario==16 )
                    spoil_vector_by_deleting_element(bndu);
                real_1d_array x;
                minqpstate state;
                minqpreport rep;

                minqpcreate(2, state);
                minqpsetquadraticterm(state, a);
                minqpsetlinearterm(state, b);
                minqpsetstartingpoint(state, x0);
                minqpsetbc(state, bndl, bndu);
                minqpoptimize(state);
                minqpresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[2.5,2]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minqp_d_bc1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_d_v
        //      Nonlinear least squares optimization using function vector only
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
                //
                //     f0(x0,x1) = 10*(x0+3)^2
                //     f1(x0,x1) = (x1-3)^2
                //
                // using "V" mode of the Levenberg-Marquardt optimizer.
                //
                // Optimization algorithm uses:
                // * function vector f[] = {f1,f2}
                //
                // No other information (Jacobian, gradient, etc.) is needed.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;

                minlmcreatev(2, x, 0.0001, state);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_fvec);
                minlmresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,+3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_d_v");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_d_vj
        //      Nonlinear least squares optimization using function vector and Jacobian
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
                //
                //     f0(x0,x1) = 10*(x0+3)^2
                //     f1(x0,x1) = (x1-3)^2
                //
                // using "VJ" mode of the Levenberg-Marquardt optimizer.
                //
                // Optimization algorithm uses:
                // * function vector f[] = {f1,f2}
                // * Jacobian matrix J = {dfi/dxj}.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;

                minlmcreatevj(2, x, state);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_fvec, function1_jac);
                minlmresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,+3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_d_vj");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_d_fgh
        //      Nonlinear Hessian-based optimization for general functions
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = 100*(x0+3)^4+(x1-3)^4
                // using "FGH" mode of the Levenberg-Marquardt optimizer.
                //
                // F is treated like a monolitic function without internal structure,
                // i.e. we do NOT represent it as a sum of squares.
                //
                // Optimization algorithm uses:
                // * function value F(x0,x1)
                // * gradient G={dF/dxi}
                // * Hessian H={d2F/(dxi*dxj)}
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;

                minlmcreatefgh(x, state);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_func, function1_grad, function1_hess);
                minlmresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,+3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_d_fgh");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_d_vb
        //      Bound constrained nonlinear least squares optimization
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<16; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
                //
                //     f0(x0,x1) = 10*(x0+3)^2
                //     f1(x0,x1) = (x1-3)^2
                //
                // with boundary constraints
                //
                //     -1 <= x0 <= +1
                //     -1 <= x1 <= +1
                //
                // using "V" mode of the Levenberg-Marquardt optimizer.
                //
                // Optimization algorithm uses:
                // * function vector f[] = {f1,f2}
                //
                // No other information (Jacobian, gradient, etc.) is needed.
                //
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                real_1d_array bndl = "[-1,-1]";
                if( _spoil_scenario==3 )
                    spoil_vector_by_nan(bndl);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(bndl);
                real_1d_array bndu = "[+1,+1]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(bndu);
                if( _spoil_scenario==6 )
                    spoil_vector_by_deleting_element(bndu);
                double epsg = 0.0000000001;
                if( _spoil_scenario==7 )
                    epsg = fp_nan;
                if( _spoil_scenario==8 )
                    epsg = fp_posinf;
                if( _spoil_scenario==9 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==10 )
                    epsf = fp_nan;
                if( _spoil_scenario==11 )
                    epsf = fp_posinf;
                if( _spoil_scenario==12 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==13 )
                    epsx = fp_nan;
                if( _spoil_scenario==14 )
                    epsx = fp_posinf;
                if( _spoil_scenario==15 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;

                minlmcreatev(2, x, 0.0001, state);
                minlmsetbc(state, bndl, bndu);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_fvec);
                minlmresults(state, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-1,+1]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_d_vb");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_d_restarts
        //      Efficient restarts of LM optimizer
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<15; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
                //
                //     f0(x0,x1) = 10*(x0+3)^2
                //     f1(x0,x1) = (x1-3)^2
                //
                // using several starting points and efficient restarts.
                //
                real_1d_array x;
                double epsg = 0.0000000001;
                if( _spoil_scenario==0 )
                    epsg = fp_nan;
                if( _spoil_scenario==1 )
                    epsg = fp_posinf;
                if( _spoil_scenario==2 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==3 )
                    epsf = fp_nan;
                if( _spoil_scenario==4 )
                    epsf = fp_posinf;
                if( _spoil_scenario==5 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==6 )
                    epsx = fp_nan;
                if( _spoil_scenario==7 )
                    epsx = fp_posinf;
                if( _spoil_scenario==8 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;

                //
                // create optimizer using minlmcreatev()
                //
                x = "[10,10]";
                if( _spoil_scenario==9 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==10 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==11 )
                    spoil_vector_by_neginf(x);
                minlmcreatev(2, x, 0.0001, state);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_fvec);
                minlmresults(state, x, rep);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,+3]", 0.005);

                //
                // restart optimizer using minlmrestartfrom()
                //
                // we can use different starting point, different function,
                // different stopping conditions, but problem size
                // must remain unchanged.
                //
                x = "[4,4]";
                if( _spoil_scenario==12 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==13 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==14 )
                    spoil_vector_by_neginf(x);
                minlmrestartfrom(state, x);
                alglib::minlmoptimize(state, function2_fvec);
                minlmresults(state, x, rep);
                _TestResult = _TestResult && doc_test_real_vector(x, "[0,1]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_d_restarts");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_t_1
        //      Nonlinear least squares optimization, FJ scheme (obsolete, but supported)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;
                minlmcreatefj(2, x, state);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_func, function1_jac);
                minlmresults(state, x, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,+3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_t_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST minlm_t_2
        //      Nonlinear least squares optimization, FGJ scheme (obsolete, but supported)
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<12; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[0,0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                double epsg = 0.0000000001;
                if( _spoil_scenario==3 )
                    epsg = fp_nan;
                if( _spoil_scenario==4 )
                    epsg = fp_posinf;
                if( _spoil_scenario==5 )
                    epsg = fp_neginf;
                double epsf = 0;
                if( _spoil_scenario==6 )
                    epsf = fp_nan;
                if( _spoil_scenario==7 )
                    epsf = fp_posinf;
                if( _spoil_scenario==8 )
                    epsf = fp_neginf;
                double epsx = 0;
                if( _spoil_scenario==9 )
                    epsx = fp_nan;
                if( _spoil_scenario==10 )
                    epsx = fp_posinf;
                if( _spoil_scenario==11 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                minlmstate state;
                minlmreport rep;
                minlmcreatefgj(2, x, state);
                minlmsetcond(state, epsg, epsf, epsx, maxits);
                alglib::minlmoptimize(state, function1_func, function1_grad, function1_jac);
                minlmresults(state, x, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[-3,+3]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "minlm_t_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_nlf
        //      Nonlinear fitting using function value only
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<27; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate exponential fitting
                // by f(x) = exp(-c*x^2)
                // using function value only.
                //
                // Gradient is estimated using combination of numerical differences
                // and secant updates. diffstep variable stores differentiation step 
                // (we have to tell algorithm what step to use).
                //
                real_2d_array x = "[[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(x);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(x);
                real_1d_array y = "[0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array c = "[0.3]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(c);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(c);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(c);
                double epsf = 0;
                if( _spoil_scenario==13 )
                    epsf = fp_nan;
                if( _spoil_scenario==14 )
                    epsf = fp_posinf;
                if( _spoil_scenario==15 )
                    epsf = fp_neginf;
                double epsx = 0.000001;
                if( _spoil_scenario==16 )
                    epsx = fp_nan;
                if( _spoil_scenario==17 )
                    epsx = fp_posinf;
                if( _spoil_scenario==18 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                ae_int_t info;
                lsfitstate state;
                lsfitreport rep;
                double diffstep = 0.0001;
                if( _spoil_scenario==19 )
                    diffstep = fp_nan;
                if( _spoil_scenario==20 )
                    diffstep = fp_posinf;
                if( _spoil_scenario==21 )
                    diffstep = fp_neginf;

                //
                // Fitting without weights
                //
                lsfitcreatef(x, y, c, diffstep, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.5]", 0.05);

                //
                // Fitting with weights
                // (you can change weights and see how it changes result)
                //
                real_1d_array w = "[1,1,1,1,1,1,1,1,1,1,1]";
                if( _spoil_scenario==22 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==23 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==24 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==25 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==26 )
                    spoil_vector_by_deleting_element(w);
                lsfitcreatewf(x, y, w, c, diffstep, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.5]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_nlf");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_nlfg
        //      Nonlinear fitting using gradient
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<24; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate exponential fitting
                // by f(x) = exp(-c*x^2)
                // using function value and gradient (with respect to c).
                //
                real_2d_array x = "[[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(x);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(x);
                real_1d_array y = "[0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array c = "[0.3]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(c);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(c);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(c);
                double epsf = 0;
                if( _spoil_scenario==13 )
                    epsf = fp_nan;
                if( _spoil_scenario==14 )
                    epsf = fp_posinf;
                if( _spoil_scenario==15 )
                    epsf = fp_neginf;
                double epsx = 0.000001;
                if( _spoil_scenario==16 )
                    epsx = fp_nan;
                if( _spoil_scenario==17 )
                    epsx = fp_posinf;
                if( _spoil_scenario==18 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                ae_int_t info;
                lsfitstate state;
                lsfitreport rep;

                //
                // Fitting without weights
                //
                lsfitcreatefg(x, y, c, true, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func, function_cx_1_grad);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.5]", 0.05);

                //
                // Fitting with weights
                // (you can change weights and see how it changes result)
                //
                real_1d_array w = "[1,1,1,1,1,1,1,1,1,1,1]";
                if( _spoil_scenario==19 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==20 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==21 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==22 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==23 )
                    spoil_vector_by_deleting_element(w);
                lsfitcreatewfg(x, y, w, c, true, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func, function_cx_1_grad);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.5]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_nlfg");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_nlfgh
        //      Nonlinear fitting using gradient and Hessian
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<24; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate exponential fitting
                // by f(x) = exp(-c*x^2)
                // using function value, gradient and Hessian (with respect to c)
                //
                real_2d_array x = "[[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(x);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(x);
                real_1d_array y = "[0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array c = "[0.3]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(c);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(c);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(c);
                double epsf = 0;
                if( _spoil_scenario==13 )
                    epsf = fp_nan;
                if( _spoil_scenario==14 )
                    epsf = fp_posinf;
                if( _spoil_scenario==15 )
                    epsf = fp_neginf;
                double epsx = 0.000001;
                if( _spoil_scenario==16 )
                    epsx = fp_nan;
                if( _spoil_scenario==17 )
                    epsx = fp_posinf;
                if( _spoil_scenario==18 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                ae_int_t info;
                lsfitstate state;
                lsfitreport rep;

                //
                // Fitting without weights
                //
                lsfitcreatefgh(x, y, c, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func, function_cx_1_grad, function_cx_1_hess);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.5]", 0.05);

                //
                // Fitting with weights
                // (you can change weights and see how it changes result)
                //
                real_1d_array w = "[1,1,1,1,1,1,1,1,1,1,1]";
                if( _spoil_scenario==19 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==20 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==21 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==22 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==23 )
                    spoil_vector_by_deleting_element(w);
                lsfitcreatewfgh(x, y, w, c, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func, function_cx_1_grad, function_cx_1_hess);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.5]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_nlfgh");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_nlfb
        //      Bound contstrained nonlinear fitting using function value only
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<26; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate exponential fitting by
                //     f(x) = exp(-c*x^2)
                // subject to bound constraints
                //     0.0 <= c <= 1.0
                // using function value only.
                //
                // Gradient is estimated using combination of numerical differences
                // and secant updates. diffstep variable stores differentiation step 
                // (we have to tell algorithm what step to use).
                //
                // Unconstrained solution is c=1.5, but because of constraints we should
                // get c=1.0 (at the boundary).
                //
                real_2d_array x = "[[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(x);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(x);
                real_1d_array y = "[0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array c = "[0.3]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(c);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(c);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(c);
                real_1d_array bndl = "[0.0]";
                if( _spoil_scenario==13 )
                    spoil_vector_by_nan(bndl);
                if( _spoil_scenario==14 )
                    spoil_vector_by_deleting_element(bndl);
                real_1d_array bndu = "[1.0]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(bndu);
                if( _spoil_scenario==16 )
                    spoil_vector_by_deleting_element(bndu);
                double epsf = 0;
                if( _spoil_scenario==17 )
                    epsf = fp_nan;
                if( _spoil_scenario==18 )
                    epsf = fp_posinf;
                if( _spoil_scenario==19 )
                    epsf = fp_neginf;
                double epsx = 0.000001;
                if( _spoil_scenario==20 )
                    epsx = fp_nan;
                if( _spoil_scenario==21 )
                    epsx = fp_posinf;
                if( _spoil_scenario==22 )
                    epsx = fp_neginf;
                ae_int_t maxits = 0;
                ae_int_t info;
                lsfitstate state;
                lsfitreport rep;
                double diffstep = 0.0001;
                if( _spoil_scenario==23 )
                    diffstep = fp_nan;
                if( _spoil_scenario==24 )
                    diffstep = fp_posinf;
                if( _spoil_scenario==25 )
                    diffstep = fp_neginf;

                lsfitcreatef(x, y, c, diffstep, state);
                lsfitsetbc(state, bndl, bndu);
                lsfitsetcond(state, epsf, epsx, maxits);
                alglib::lsfitfit(state, function_cx_1_func);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.0]", 0.05);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_nlfb");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_nlscale
        //      Nonlinear fitting with custom scaling and bound constraints
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<30; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate fitting by
                //     f(x) = c[0]*(1+c[1]*((x-1999)^c[2]-1))
                // subject to bound constraints
                //     -INF  < c[0] < +INF
                //      -10 <= c[1] <= +10
                //      0.1 <= c[2] <= 2.0
                // Data we want to fit are time series of Japan national debt
                // collected from 2000 to 2008 measured in USD (dollars, not
                // millions of dollars).
                //
                // Our variables are:
                //     c[0] - debt value at initial moment (2000),
                //     c[1] - direction coefficient (growth or decrease),
                //     c[2] - curvature coefficient.
                // You may see that our variables are badly scaled - first one 
                // is order of 10^12, and next two are somewhere about 1 in 
                // magnitude. Such problem is difficult to solve without some
                // kind of scaling.
                // That is exactly where lsfitsetscale() function can be used.
                // We set scale of our variables to [1.0E12, 1, 1], which allows
                // us to easily solve this problem.
                //
                // You can try commenting out lsfitsetscale() call - and you will 
                // see that algorithm will fail to converge.
                //
                real_2d_array x = "[[2000],[2001],[2002],[2003],[2004],[2005],[2006],[2007],[2008]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(x);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(x);
                real_1d_array y = "[4323239600000.0, 4560913100000.0, 5564091500000.0, 6743189300000.0, 7284064600000.0, 7050129600000.0, 7092221500000.0, 8483907600000.0, 8625804400000.0]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array c = "[1.0e+13, 1, 1]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(c);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(c);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(c);
                double epsf = 0;
                if( _spoil_scenario==13 )
                    epsf = fp_nan;
                if( _spoil_scenario==14 )
                    epsf = fp_posinf;
                if( _spoil_scenario==15 )
                    epsf = fp_neginf;
                double epsx = 1.0e-5;
                if( _spoil_scenario==16 )
                    epsx = fp_nan;
                if( _spoil_scenario==17 )
                    epsx = fp_posinf;
                if( _spoil_scenario==18 )
                    epsx = fp_neginf;
                real_1d_array bndl = "[-inf, -10, 0.1]";
                if( _spoil_scenario==19 )
                    spoil_vector_by_nan(bndl);
                if( _spoil_scenario==20 )
                    spoil_vector_by_deleting_element(bndl);
                real_1d_array bndu = "[+inf, +10, 2.0]";
                if( _spoil_scenario==21 )
                    spoil_vector_by_nan(bndu);
                if( _spoil_scenario==22 )
                    spoil_vector_by_deleting_element(bndu);
                real_1d_array s = "[1.0e+12, 1, 1]";
                if( _spoil_scenario==23 )
                    spoil_vector_by_nan(s);
                if( _spoil_scenario==24 )
                    spoil_vector_by_posinf(s);
                if( _spoil_scenario==25 )
                    spoil_vector_by_neginf(s);
                if( _spoil_scenario==26 )
                    spoil_vector_by_deleting_element(s);
                ae_int_t maxits = 0;
                ae_int_t info;
                lsfitstate state;
                lsfitreport rep;
                double diffstep = 1.0e-5;
                if( _spoil_scenario==27 )
                    diffstep = fp_nan;
                if( _spoil_scenario==28 )
                    diffstep = fp_posinf;
                if( _spoil_scenario==29 )
                    diffstep = fp_neginf;

                lsfitcreatef(x, y, c, diffstep, state);
                lsfitsetcond(state, epsf, epsx, maxits);
                lsfitsetbc(state, bndl, bndu);
                lsfitsetscale(state, s);
                alglib::lsfitfit(state, function_debt_func);
                lsfitresults(state, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 2);
                _TestResult = _TestResult && doc_test_real_vector(c, "[4.142560e+12, 0.434240, 0.565376]", -0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_nlscale");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_lin
        //      Unconstrained (general) linear least squares fitting with and without weights
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<13; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate linear fitting by f(x|a) = a*exp(0.5*x).
                //
                // We have:
                // * y - vector of experimental data
                // * fmatrix -  matrix of basis functions calculated at sample points
                //              Actually, we have only one basis function F0 = exp(0.5*x).
                //
                real_2d_array fmatrix = "[[0.606531],[0.670320],[0.740818],[0.818731],[0.904837],[1.000000],[1.105171],[1.221403],[1.349859],[1.491825],[1.648721]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(fmatrix);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(fmatrix);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(fmatrix);
                real_1d_array y = "[1.133719, 1.306522, 1.504604, 1.554663, 1.884638, 2.072436, 2.257285, 2.534068, 2.622017, 2.897713, 3.219371]";
                if( _spoil_scenario==3 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==4 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==5 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y);
                ae_int_t info;
                real_1d_array c;
                lsfitreport rep;

                //
                // Linear fitting without weights
                //
                lsfitlinear(y, fmatrix, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.98650]", 0.00005);

                //
                // Linear fitting with individual weights.
                // Slightly different result is returned.
                //
                real_1d_array w = "[1.414213, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]";
                if( _spoil_scenario==8 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==9 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==10 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==11 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==12 )
                    spoil_vector_by_deleting_element(w);
                lsfitlinearw(y, w, fmatrix, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_real_vector(c, "[1.983354]", 0.00005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_lin");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_linc
        //      Constrained (general) linear least squares fitting with and without weights
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<20; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate linear fitting by f(x|a,b) = a*x+b
                // with simple constraint f(0)=0.
                //
                // We have:
                // * y - vector of experimental data
                // * fmatrix -  matrix of basis functions sampled at [0,1] with step 0.2:
                //                  [ 1.0   0.0 ]
                //                  [ 1.0   0.2 ]
                //                  [ 1.0   0.4 ]
                //                  [ 1.0   0.6 ]
                //                  [ 1.0   0.8 ]
                //                  [ 1.0   1.0 ]
                //              first column contains value of first basis function (constant term)
                //              second column contains second basis function (linear term)
                // * cmatrix -  matrix of linear constraints:
                //                  [ 1.0  0.0  0.0 ]
                //              first two columns contain coefficients before basis functions,
                //              last column contains desired value of their sum.
                //              So [1,0,0] means "1*constant_term + 0*linear_term = 0" 
                //
                real_1d_array y = "[0.072436,0.246944,0.491263,0.522300,0.714064,0.921929]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(y);
                real_2d_array fmatrix = "[[1,0.0],[1,0.2],[1,0.4],[1,0.6],[1,0.8],[1,1.0]]";
                if( _spoil_scenario==5 )
                    spoil_matrix_by_nan(fmatrix);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_posinf(fmatrix);
                if( _spoil_scenario==7 )
                    spoil_matrix_by_neginf(fmatrix);
                if( _spoil_scenario==8 )
                    spoil_matrix_by_adding_row(fmatrix);
                if( _spoil_scenario==9 )
                    spoil_matrix_by_adding_col(fmatrix);
                if( _spoil_scenario==10 )
                    spoil_matrix_by_deleting_row(fmatrix);
                if( _spoil_scenario==11 )
                    spoil_matrix_by_deleting_col(fmatrix);
                real_2d_array cmatrix = "[[1,0,0]]";
                if( _spoil_scenario==12 )
                    spoil_matrix_by_nan(cmatrix);
                if( _spoil_scenario==13 )
                    spoil_matrix_by_posinf(cmatrix);
                if( _spoil_scenario==14 )
                    spoil_matrix_by_neginf(cmatrix);
                ae_int_t info;
                real_1d_array c;
                lsfitreport rep;

                //
                // Constrained fitting without weights
                //
                lsfitlinearc(y, fmatrix, cmatrix, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_real_vector(c, "[0,0.932933]", 0.0005);

                //
                // Constrained fitting with individual weights
                //
                real_1d_array w = "[1, 1.414213, 1, 1, 1, 1]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==16 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==17 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==18 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==19 )
                    spoil_vector_by_deleting_element(w);
                lsfitlinearwc(y, w, fmatrix, cmatrix, info, c, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && doc_test_real_vector(c, "[0,0.938322]", 0.0005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_linc");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_pol
        //      Unconstrained polynomial fitting
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<20; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates polynomial fitting.
                //
                // Fitting is done by two (M=2) functions from polynomial basis:
                //     f0 = 1
                //     f1 = x
                // Basically, it just a linear fit; more complex polynomials may be used
                // (e.g. parabolas with M=3, cubic with M=4), but even such simple fit allows
                // us to demonstrate polynomialfit() function in action.
                //
                // We have:
                // * x      set of abscissas
                // * y      experimental data
                //
                // Additionally we demonstrate weighted fitting, where second point has
                // more weight than other ones.
                //
                real_1d_array x = "[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                ae_int_t m = 2;
                double t = 2;
                if( _spoil_scenario==10 )
                    t = fp_posinf;
                if( _spoil_scenario==11 )
                    t = fp_neginf;
                ae_int_t info;
                barycentricinterpolant p;
                polynomialfitreport rep;
                double v;

                //
                // Fitting without individual weights
                //
                // NOTE: result is returned as barycentricinterpolant structure.
                //       if you want to get representation in the power basis,
                //       you can use barycentricbar2pow() function to convert
                //       from barycentric to power representation (see docs for 
                //       POLINT subpackage for more info).
                //
                polynomialfit(x, y, m, info, p, rep);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.011, 0.002);

                //
                // Fitting with individual weights
                //
                // NOTE: slightly different result is returned
                //
                real_1d_array w = "[1,1.414213562,1,1,1,1,1,1,1,1,1]";
                if( _spoil_scenario==12 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==13 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==14 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==15 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==16 )
                    spoil_vector_by_deleting_element(w);
                real_1d_array xc = "[]";
                if( _spoil_scenario==17 )
                    spoil_vector_by_adding_element(xc);
                real_1d_array yc = "[]";
                if( _spoil_scenario==18 )
                    spoil_vector_by_adding_element(yc);
                integer_1d_array dc = "[]";
                if( _spoil_scenario==19 )
                    spoil_vector_by_adding_element(dc);
                polynomialfitwc(x, y, w, xc, yc, dc, m, info, p, rep);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.023, 0.002);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_pol");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_polc
        //      Constrained polynomial fitting
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<29; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates polynomial fitting.
                //
                // Fitting is done by two (M=2) functions from polynomial basis:
                //     f0 = 1
                //     f1 = x
                // with simple constraint on function value
                //     f(0) = 0
                // Basically, it just a linear fit; more complex polynomials may be used
                // (e.g. parabolas with M=3, cubic with M=4), but even such simple fit allows
                // us to demonstrate polynomialfit() function in action.
                //
                // We have:
                // * x      set of abscissas
                // * y      experimental data
                // * xc     points where constraints are placed
                // * yc     constraints on derivatives
                // * dc     derivative indices
                //          (0 means function itself, 1 means first derivative)
                //
                real_1d_array x = "[1.0,1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0.9,1.1]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array w = "[1,1]";
                if( _spoil_scenario==10 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==11 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==12 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==13 )
                    spoil_vector_by_adding_element(w);
                if( _spoil_scenario==14 )
                    spoil_vector_by_deleting_element(w);
                real_1d_array xc = "[0]";
                if( _spoil_scenario==15 )
                    spoil_vector_by_nan(xc);
                if( _spoil_scenario==16 )
                    spoil_vector_by_posinf(xc);
                if( _spoil_scenario==17 )
                    spoil_vector_by_neginf(xc);
                if( _spoil_scenario==18 )
                    spoil_vector_by_adding_element(xc);
                if( _spoil_scenario==19 )
                    spoil_vector_by_deleting_element(xc);
                real_1d_array yc = "[0]";
                if( _spoil_scenario==20 )
                    spoil_vector_by_nan(yc);
                if( _spoil_scenario==21 )
                    spoil_vector_by_posinf(yc);
                if( _spoil_scenario==22 )
                    spoil_vector_by_neginf(yc);
                if( _spoil_scenario==23 )
                    spoil_vector_by_adding_element(yc);
                if( _spoil_scenario==24 )
                    spoil_vector_by_deleting_element(yc);
                integer_1d_array dc = "[0]";
                if( _spoil_scenario==25 )
                    spoil_vector_by_adding_element(dc);
                if( _spoil_scenario==26 )
                    spoil_vector_by_deleting_element(dc);
                double t = 2;
                if( _spoil_scenario==27 )
                    t = fp_posinf;
                if( _spoil_scenario==28 )
                    t = fp_neginf;
                ae_int_t m = 2;
                ae_int_t info;
                barycentricinterpolant p;
                polynomialfitreport rep;
                double v;

                polynomialfitwc(x, y, w, xc, yc, dc, m, info, p, rep);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.000, 0.001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_polc");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_d_spline
        //      Unconstrained fitting by penalized regression spline
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<19; _spoil_scenario++)
        {
            try
            {
                //
                // In this example we demonstrate penalized spline fitting of noisy data
                //
                // We have:
                // * x - abscissas
                // * y - vector of experimental data, straight line with small noise
                //
                real_1d_array x = "[0.00,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.80,0.90]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_adding_element(x);
                if( _spoil_scenario==4 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0.10,0.00,0.30,0.40,0.30,0.40,0.62,0.68,0.75,0.95]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==8 )
                    spoil_vector_by_adding_element(y);
                if( _spoil_scenario==9 )
                    spoil_vector_by_deleting_element(y);
                ae_int_t info;
                double v;
                spline1dinterpolant s;
                spline1dfitreport rep;
                double rho;

                //
                // Fit with VERY small amount of smoothing (rho = -5.0)
                // and large number of basis functions (M=50).
                //
                // With such small regularization penalized spline almost fully reproduces function values
                //
                rho = -5.0;
                if( _spoil_scenario==10 )
                    rho = fp_nan;
                if( _spoil_scenario==11 )
                    rho = fp_posinf;
                if( _spoil_scenario==12 )
                    rho = fp_neginf;
                spline1dfitpenalized(x, y, 50, rho, info, s, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                v = spline1dcalc(s, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.10, 0.01);

                //
                // Fit with VERY large amount of smoothing (rho = 10.0)
                // and large number of basis functions (M=50).
                //
                // With such regularization our spline should become close to the straight line fit.
                // We will compare its value in x=1.0 with results obtained from such fit.
                //
                rho = +10.0;
                if( _spoil_scenario==13 )
                    rho = fp_nan;
                if( _spoil_scenario==14 )
                    rho = fp_posinf;
                if( _spoil_scenario==15 )
                    rho = fp_neginf;
                spline1dfitpenalized(x, y, 50, rho, info, s, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                v = spline1dcalc(s, 1.0);
                _TestResult = _TestResult && doc_test_real(v, 0.969, 0.001);

                //
                // In real life applications you may need some moderate degree of fitting,
                // so we try to fit once more with rho=3.0.
                //
                rho = +3.0;
                if( _spoil_scenario==16 )
                    rho = fp_nan;
                if( _spoil_scenario==17 )
                    rho = fp_posinf;
                if( _spoil_scenario==18 )
                    rho = fp_neginf;
                spline1dfitpenalized(x, y, 50, rho, info, s, rep);
                _TestResult = _TestResult && doc_test_int(info, 1);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_d_spline");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_t_polfit_1
        //      Polynomial fitting, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<10; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y);
                ae_int_t m = 2;
                double t = 2;
                if( _spoil_scenario==8 )
                    t = fp_posinf;
                if( _spoil_scenario==9 )
                    t = fp_neginf;
                ae_int_t info;
                barycentricinterpolant p;
                polynomialfitreport rep;
                double v;
                polynomialfit(x, y, 11, m, info, p, rep);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.011, 0.002);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_t_polfit_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_t_polfit_2
        //      Polynomial fitting, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<14; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array w = "[1,1.414213562,1,1,1,1,1,1,1,1,1]";
                if( _spoil_scenario==8 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==9 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==10 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==11 )
                    spoil_vector_by_deleting_element(w);
                real_1d_array xc = "[]";
                real_1d_array yc = "[]";
                integer_1d_array dc = "[]";
                ae_int_t m = 2;
                double t = 2;
                if( _spoil_scenario==12 )
                    t = fp_posinf;
                if( _spoil_scenario==13 )
                    t = fp_neginf;
                ae_int_t info;
                barycentricinterpolant p;
                polynomialfitreport rep;
                double v;
                polynomialfitwc(x, y, w, 11, xc, yc, dc, 0, m, info, p, rep);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.023, 0.002);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_t_polfit_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lsfit_t_polfit_3
        //      Polynomial fitting, full list of parameters.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<23; _spoil_scenario++)
        {
            try
            {
                real_1d_array x = "[1.0,1.0]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[0.9,1.1]";
                if( _spoil_scenario==4 )
                    spoil_vector_by_nan(y);
                if( _spoil_scenario==5 )
                    spoil_vector_by_posinf(y);
                if( _spoil_scenario==6 )
                    spoil_vector_by_neginf(y);
                if( _spoil_scenario==7 )
                    spoil_vector_by_deleting_element(y);
                real_1d_array w = "[1,1]";
                if( _spoil_scenario==8 )
                    spoil_vector_by_nan(w);
                if( _spoil_scenario==9 )
                    spoil_vector_by_posinf(w);
                if( _spoil_scenario==10 )
                    spoil_vector_by_neginf(w);
                if( _spoil_scenario==11 )
                    spoil_vector_by_deleting_element(w);
                real_1d_array xc = "[0]";
                if( _spoil_scenario==12 )
                    spoil_vector_by_nan(xc);
                if( _spoil_scenario==13 )
                    spoil_vector_by_posinf(xc);
                if( _spoil_scenario==14 )
                    spoil_vector_by_neginf(xc);
                if( _spoil_scenario==15 )
                    spoil_vector_by_deleting_element(xc);
                real_1d_array yc = "[0]";
                if( _spoil_scenario==16 )
                    spoil_vector_by_nan(yc);
                if( _spoil_scenario==17 )
                    spoil_vector_by_posinf(yc);
                if( _spoil_scenario==18 )
                    spoil_vector_by_neginf(yc);
                if( _spoil_scenario==19 )
                    spoil_vector_by_deleting_element(yc);
                integer_1d_array dc = "[0]";
                if( _spoil_scenario==20 )
                    spoil_vector_by_deleting_element(dc);
                ae_int_t m = 2;
                double t = 2;
                if( _spoil_scenario==21 )
                    t = fp_posinf;
                if( _spoil_scenario==22 )
                    t = fp_neginf;
                ae_int_t info;
                barycentricinterpolant p;
                polynomialfitreport rep;
                double v;
                polynomialfitwc(x, y, w, 2, xc, yc, dc, 1, m, info, p, rep);
                v = barycentriccalc(p, t);
                _TestResult = _TestResult && doc_test_real(v, 2.000, 0.001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lsfit_t_polfit_3");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST sparse_d_1
        //      Basic operations with sparse matrices
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<1; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates creation/initialization of the sparse matrix
                // and matrix-vector multiplication.
                //
                // First, we have to create matrix and initialize it. Matrix is initially created
                // in the Hash-Table format, which allows convenient initialization. We can modify
                // Hash-Table matrix with sparseset() and sparseadd() functions.
                //
                // NOTE: Unlike CRS format, Hash-Table representation allows you to initialize
                // elements in the arbitrary order. You may see that we initialize a[0][0] first,
                // then move to the second row, and then move back to the first row.
                //
                sparsematrix s;
                sparsecreate(2, 2, s);
                sparseset(s, 0, 0, 2.0);
                sparseset(s, 1, 1, 1.0);
                sparseset(s, 0, 1, 1.0);

                sparseadd(s, 1, 1, 4.0);

                //
                // Now S is equal to
                //   [ 2 1 ]
                //   [   5 ]
                // Lets check it by reading matrix contents with sparseget().
                // You may see that with sparseget() you may read both non-zero
                // and zero elements.
                //
                double v;
                v = sparseget(s, 0, 0);
                _TestResult = _TestResult && doc_test_real(v, 2.0000, 0.005);
                v = sparseget(s, 0, 1);
                _TestResult = _TestResult && doc_test_real(v, 1.0000, 0.005);
                v = sparseget(s, 1, 0);
                _TestResult = _TestResult && doc_test_real(v, 0.0000, 0.005);
                v = sparseget(s, 1, 1);
                _TestResult = _TestResult && doc_test_real(v, 5.0000, 0.005);

                //
                // After successful creation we can use our matrix for linear operations.
                //
                // However, there is one more thing we MUST do before using S in linear
                // operations: we have to convert it from HashTable representation (used for
                // initialization and dynamic operations) to CRS format with sparseconverttocrs()
                // call. If you omit this call, ALGLIB will generate exception on the first
                // attempt to use S in linear operations. 
                //
                sparseconverttocrs(s);

                //
                // Now S is in the CRS format and we are ready to do linear operations.
                // Lets calculate A*x for some x.
                //
                real_1d_array x = "[1,-1]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[]";
                sparsemv(s, x, y);
                _TestResult = _TestResult && doc_test_real_vector(y, "[1.000,-5.000]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "sparse_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST sparse_d_2
        //      Advanced topic: creation in the CRS format.
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<2; _spoil_scenario++)
        {
            try
            {
                //
                // This example demonstrates creation/initialization of the sparse matrix in the
                // CRS format.
                //
                // Hash-Table format used by default is very convenient (it allows easy
                // insertion of elements, automatic memory reallocation), but has
                // significant memory and performance overhead. Insertion of one element 
                // costs hundreds of CPU cycles, and memory consumption is several times
                // higher than that of CRS.
                //
                // When you work with really large matrices and when you can tell in 
                // advance how many elements EXACTLY you need, it can be beneficial to 
                // create matrix in the CRS format from the very beginning.
                //
                // If you want to create matrix in the CRS format, you should:
                // * use sparsecreatecrs() function
                // * know row sizes in advance (number of non-zero entries in the each row)
                // * initialize matrix with sparseset() - another function, sparseadd(), is not allowed
                // * initialize elements from left to right, from top to bottom, each
                //   element is initialized only once.
                //
                sparsematrix s;
                integer_1d_array row_sizes = "[2,2,2,1]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_deleting_element(row_sizes);
                sparsecreatecrs(4, 4, row_sizes, s);
                sparseset(s, 0, 0, 2.0);
                sparseset(s, 0, 1, 1.0);
                sparseset(s, 1, 1, 4.0);
                sparseset(s, 1, 2, 2.0);
                sparseset(s, 2, 2, 3.0);
                sparseset(s, 2, 3, 1.0);
                sparseset(s, 3, 3, 9.0);

                //
                // Now S is equal to
                //   [ 2 1     ]
                //   [   4 2   ]
                //   [     3 1 ]
                //   [       9 ]
                //
                // We should point that we have initialized S elements from left to right,
                // from top to bottom. CRS representation does NOT allow you to do so in
                // the different order. Try to change order of the sparseset() calls above,
                // and you will see that your program generates exception.
                //
                // We can check it by reading matrix contents with sparseget().
                // However, you should remember that sparseget() is inefficient on
                // CRS matrices (it may have to pass through all elements of the row 
                // until it finds element you need).
                //
                double v;
                v = sparseget(s, 0, 0);
                _TestResult = _TestResult && doc_test_real(v, 2.0000, 0.005);
                v = sparseget(s, 2, 3);
                _TestResult = _TestResult && doc_test_real(v, 1.0000, 0.005);

                // you may see that you can read zero elements (which are not stored) with sparseget()
                v = sparseget(s, 3, 2);
                _TestResult = _TestResult && doc_test_real(v, 0.0000, 0.005);

                //
                // After successful creation we can use our matrix for linear operations.
                // Lets calculate A*x for some x.
                //
                real_1d_array x = "[1,-1,1,-1]";
                if( _spoil_scenario==1 )
                    spoil_vector_by_deleting_element(x);
                real_1d_array y = "[]";
                sparsemv(s, x, y);
                _TestResult = _TestResult && doc_test_real_vector(y, "[1.000,-2.000,2.000,-9]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "sparse_d_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST linlsqr_d_1
        //      Solution of sparse linear systems with CG
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<4; _spoil_scenario++)
        {
            try
            {
                //
                // This example illustrates solution of sparse linear least squares problem
                // with LSQR algorithm.
                // 
                // Suppose that we have least squares problem min|A*x-b| with sparse A
                // represented by sparsematrix object
                //         [ 1 1 ]
                //         [ 1 1 ]
                //     A = [ 2 1 ]
                //         [ 1   ]
                //         [   1 ]
                // and right part b
                //     [ 4 ]
                //     [ 2 ]
                // b = [ 4 ]
                //     [ 1 ]
                //     [ 2 ]
                // and we want to solve this system in the least squares sense using
                // LSQR algorithm. In order to do so, we have to create left part
                // (sparsematrix object) and right part (dense array).
                //
                // Initially, sparse matrix is created in the Hash-Table format,
                // which allows easy initialization, but do not allow matrix to be
                // used in the linear solvers. So after construction you should convert
                // sparse matrix to CRS format (one suited for linear operations).
                //
                sparsematrix a;
                sparsecreate(5, 2, a);
                sparseset(a, 0, 0, 1.0);
                sparseset(a, 0, 1, 1.0);
                sparseset(a, 1, 0, 1.0);
                sparseset(a, 1, 1, 1.0);
                sparseset(a, 2, 0, 2.0);
                sparseset(a, 2, 1, 1.0);
                sparseset(a, 3, 0, 1.0);
                sparseset(a, 4, 1, 1.0);

                //
                // Now our matrix is fully initialized, but we have to do one more
                // step - convert it from Hash-Table format to CRS format (see
                // documentation on sparse matrices for more information about these
                // formats).
                //
                // If you omit this call, ALGLIB will generate exception on the first
                // attempt to use A in linear operations. 
                //
                sparseconverttocrs(a);

                //
                // Initialization of the right part
                //
                real_1d_array b = "[4,2,4,1,2]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(b);

                //
                // Now we have to create linear solver object and to use it for the
                // solution of the linear system.
                //
                linlsqrstate s;
                linlsqrreport rep;
                real_1d_array x;
                linlsqrcreate(5, 2, s);
                linlsqrsolvesparse(s, a, b);
                linlsqrresults(s, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 4);
                _TestResult = _TestResult && doc_test_real_vector(x, "[1.000,2.000]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "linlsqr_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST rbf_d_qnn
        //      Simple model built with RBF-QNN algorithm
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // This example illustrates basic concepts of the RBF models: creation, modification,
                // evaluation.
                // 
                // Suppose that we have set of 2-dimensional points with associated
                // scalar function values, and we want to build a RBF model using
                // our data.
                // 
                // NOTE: we can work with 3D models too :)
                // 
                // Typical sequence of steps is given below:
                // 1. we create RBF model object
                // 2. we attach our dataset to the RBF model and tune algorithm settings
                // 3. we rebuild RBF model using QNN algorithm on new data
                // 4. we use RBF model (evaluate, serialize, etc.)
                //
                double v;

                //
                // Step 1: RBF model creation.
                //
                // We have to specify dimensionality of the space (2 or 3) and
                // dimensionality of the function (scalar or vector).
                //
                rbfmodel model;
                rbfcreate(2, 1, model);

                // New model is empty - it can be evaluated,
                // but we just get zero value at any point.
                v = rbfcalc2(model, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.000, 0.005);

                //
                // Step 2: we add dataset.
                //
                // XY arrays containt two points - x0=(-1,0) and x1=(+1,0) -
                // and two function values f(x0)=2, f(x1)=3.
                //
                real_2d_array xy = "[[-1,0,2],[+1,0,3]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(xy);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(xy);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(xy);
                rbfsetpoints(model, xy);

                // We added points, but model was not rebuild yet.
                // If we call rbfcalc2(), we still will get 0.0 as result.
                v = rbfcalc2(model, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.000, 0.005);

                //
                // Step 3: rebuild model
                //
                // After we've configured model, we should rebuild it -
                // it will change coefficients stored internally in the
                // rbfmodel structure.
                //
                // By default, RBF uses QNN algorithm, which works well with
                // relatively uniform datasets (all points are well separated,
                // average distance is approximately same for all points).
                // This default algorithm is perfectly suited for our simple
                // made up data.
                //
                // NOTE: we recommend you to take a look at example of RBF-ML,
                // multilayer RBF algorithm, which sometimes is a better
                // option than QNN.
                //
                rbfreport rep;
                rbfsetalgoqnn(model);
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);

                //
                // Step 4: model was built
                //
                // After call of rbfbuildmodel(), rbfcalc2() will return
                // value of the new model.
                //
                v = rbfcalc2(model, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 2.500, 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "rbf_d_qnn");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST rbf_d_vector
        //      Working with vector functions
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                //
                // Suppose that we have set of 2-dimensional points with associated VECTOR
                // function values, and we want to build a RBF model using our data.
                // 
                // Typical sequence of steps is given below:
                // 1. we create RBF model object
                // 2. we attach our dataset to the RBF model and tune algorithm settings
                // 3. we rebuild RBF model using new data
                // 4. we use RBF model (evaluate, serialize, etc.)
                //
                real_1d_array x;
                real_1d_array y;

                //
                // Step 1: RBF model creation.
                //
                // We have to specify dimensionality of the space (equal to 2) and
                // dimensionality of the function (2-dimensional vector function).
                //
                rbfmodel model;
                rbfcreate(2, 2, model);

                // New model is empty - it can be evaluated,
                // but we just get zero value at any point.
                x = "[+1,+1]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(x);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(x);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(x);
                rbfcalc(model, x, y);
                _TestResult = _TestResult && doc_test_real_vector(y, "[0.000,0.000]", 0.005);

                //
                // Step 2: we add dataset.
                //
                // XY arrays containt four points:
                // * (x0,y0) = (+1,+1), f(x0,y0)=(0,-1)
                // * (x1,y1) = (+1,-1), f(x1,y1)=(-1,0)
                // * (x2,y2) = (-1,-1), f(x2,y2)=(0,+1)
                // * (x3,y3) = (-1,+1), f(x3,y3)=(+1,0)
                //
                // By default, RBF uses QNN algorithm, which works well with
                // relatively uniform datasets (all points are well separated,
                // average distance is approximately same for all points).
                //
                // This default algorithm is perfectly suited for our simple
                // made up data.
                //
                real_2d_array xy = "[[+1,+1,0,-1],[+1,-1,-1,0],[-1,-1,0,+1],[-1,+1,+1,0]]";
                if( _spoil_scenario==3 )
                    spoil_matrix_by_nan(xy);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_posinf(xy);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_neginf(xy);
                rbfsetpoints(model, xy);

                // We added points, but model was not rebuild yet.
                // If we call rbfcalc(), we still will get 0.0 as result.
                rbfcalc(model, x, y);
                _TestResult = _TestResult && doc_test_real_vector(y, "[0.000,0.000]", 0.005);

                //
                // Step 3: rebuild model
                //
                // After we've configured model, we should rebuild it -
                // it will change coefficients stored internally in the
                // rbfmodel structure.
                //
                rbfreport rep;
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);

                //
                // Step 4: model was built
                //
                // After call of rbfbuildmodel(), rbfcalc() will return
                // value of the new model.
                //
                rbfcalc(model, x, y);
                _TestResult = _TestResult && doc_test_real_vector(y, "[0.000,-1.000]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "rbf_d_vector");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST rbf_d_polterm
        //      RBF models - working with polynomial term
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // This example show how to work with polynomial term
                // 
                // Suppose that we have set of 2-dimensional points with associated
                // scalar function values, and we want to build a RBF model using
                // our data.
                //
                double v;
                rbfmodel model;
                real_2d_array xy = "[[-1,0,2],[+1,0,3]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(xy);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(xy);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(xy);
                rbfreport rep;

                rbfcreate(2, 1, model);
                rbfsetpoints(model, xy);
                rbfsetalgoqnn(model);

                //
                // By default, RBF model uses linear term. It means that model
                // looks like
                //     f(x,y) = SUM(RBF[i]) + a*x + b*y + c
                // where RBF[i] is I-th radial basis function and a*x+by+c is a
                // linear term. Having linear terms in a model gives us:
                // (1) improved extrapolation properties
                // (2) linearity of the model when data can be perfectly fitted
                //     by the linear function
                // (3) linear asymptotic behavior
                //
                // Our simple dataset can be modelled by the linear function
                //     f(x,y) = 0.5*x + 2.5
                // and rbfbuildmodel() with default settings should preserve this
                // linearity.
                //
                ae_int_t nx;
                ae_int_t ny;
                ae_int_t nc;
                real_2d_array xwr;
                real_2d_array c;
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);
                rbfunpack(model, nx, ny, xwr, nc, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[0.500,0.000,2.500]]", 0.005);

                // asymptotic behavior of our function is linear
                v = rbfcalc2(model, 1000.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 502.50, 0.05);

                //
                // Instead of linear term we can use constant term. In this case
                // we will get model which has form
                //     f(x,y) = SUM(RBF[i]) + c
                // where RBF[i] is I-th radial basis function and c is a constant,
                // which is equal to the average function value on the dataset.
                //
                // Because we've already attached dataset to the model the only
                // thing we have to do is to call rbfsetconstterm() and then
                // rebuild model with rbfbuildmodel().
                //
                rbfsetconstterm(model);
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);
                rbfunpack(model, nx, ny, xwr, nc, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[0.000,0.000,2.500]]", 0.005);

                // asymptotic behavior of our function is constant
                v = rbfcalc2(model, 1000.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 2.500, 0.005);

                //
                // Finally, we can use zero term. Just plain RBF without polynomial
                // part:
                //     f(x,y) = SUM(RBF[i])
                // where RBF[i] is I-th radial basis function.
                //
                rbfsetzeroterm(model);
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);
                rbfunpack(model, nx, ny, xwr, nc, c);
                _TestResult = _TestResult && doc_test_real_matrix(c, "[[0.000,0.000,0.000]]", 0.005);

                // asymptotic behavior of our function is just zero constant
                v = rbfcalc2(model, 1000.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.000, 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "rbf_d_polterm");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST rbf_d_serialize
        //      Serialization/unserialization
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // This example show how to serialize and unserialize RBF model
                // 
                // Suppose that we have set of 2-dimensional points with associated
                // scalar function values, and we want to build a RBF model using
                // our data. Then we want to serialize it to string and to unserialize
                // from string, loading to another instance of RBF model.
                //
                // Here we assume that you already know how to create RBF models.
                //
                std::string s;
                double v;
                rbfmodel model0;
                rbfmodel model1;
                real_2d_array xy = "[[-1,0,2],[+1,0,3]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(xy);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(xy);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(xy);
                rbfreport rep;

                // model initialization
                rbfcreate(2, 1, model0);
                rbfsetpoints(model0, xy);
                rbfsetalgoqnn(model0);
                rbfbuildmodel(model0, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);

                //
                // Serialization - it looks easy,
                // but you should carefully read next section.
                //
                alglib::rbfserialize(model0, s);
                alglib::rbfunserialize(s, model1);

                // both models return same value
                v = rbfcalc2(model0, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 2.500, 0.005);
                v = rbfcalc2(model1, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 2.500, 0.005);

                //
                // Previous section shows that model state is saved/restored during
                // serialization. However, some properties are NOT serialized.
                //
                // Serialization saves/restores RBF model, but it does NOT saves/restores
                // settings which were used to build current model. In particular, dataset
                // which were used to build model, is not preserved.
                //
                // What does it mean in for us?
                //
                // Do you remember this sequence: rbfcreate-rbfsetpoints-rbfbuildmodel?
                // First step creates model, second step adds dataset and tunes model
                // settings, third step builds model using current dataset and model
                // construction settings.
                //
                // If you call rbfbuildmodel() without calling rbfsetpoints() first, you
                // will get empty (zero) RBF model. In our example, model0 contains
                // dataset which was added by rbfsetpoints() call. However, model1 does
                // NOT contain dataset - because dataset is NOT serialized.
                //
                // This, if we call rbfbuildmodel(model0,rep), we will get same model,
                // which returns 2.5 at (x,y)=(0,0). However, after same call model1 will
                // return zero - because it contains RBF model (coefficients), but does NOT
                // contain dataset which was used to build this model.
                //
                // Basically, it means that:
                // * serialization of the RBF model preserves anything related to the model
                //   EVALUATION
                // * but it does NOT creates perfect copy of the original object.
                //
                rbfbuildmodel(model0, rep);
                v = rbfcalc2(model0, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 2.500, 0.005);

                rbfbuildmodel(model1, rep);
                v = rbfcalc2(model1, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.000, 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "rbf_d_serialize");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST rbf_d_ml_simple
        //      Simple model built with RBF-ML algorithm
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // This example shows how to build models with RBF-ML algorithm. Below
                // we assume that you already know basic concepts shown in the example
                // on RBF-QNN algorithm.
                //
                // RBF-ML is a multilayer RBF algorithm, which fits a sequence of models
                // with decreasing radii. Each model is fitted with fixed number of
                // iterations of linear solver. First layers give only inexact approximation
                // of the target function, because RBF problems with large radii are
                // ill-conditioned. However, as we add more and more layers with smaller
                // and smaller radii, we get better conditioned systems - and more precise models.
                //
                rbfmodel model;
                rbfreport rep;
                double v;

                //
                // We have 2-dimensional space and very simple interpolation problem - all
                // points are distinct and located at straight line. We want to solve it
                // with RBF-ML algorithm. This problem is very simple, and RBF-QNN will
                // solve it too, but we want to evaluate RBF-ML and to start from the simple
                // problem.
                //     X        Y
                //     -2       1
                //     -1       0
                //      0       1
                //     +1      -1
                //     +2       1
                //
                rbfcreate(2, 1, model);
                real_2d_array xy0 = "[[-2,0,1],[-1,0,0],[0,0,1],[+1,0,-1],[+2,0,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(xy0);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(xy0);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(xy0);
                rbfsetpoints(model, xy0);

                // First, we try to use R=5.0 with single layer (NLayers=1) and moderate amount
                // of regularization.... but results are disappointing: Model(x=0,y=0)=-0.02,
                // and we need 1.0 at (x,y)=(0,0). Why?
                //
                // Because first layer gives very smooth and imprecise approximation of the
                // function. Average distance between points is 1.0, and R=5.0 is too large
                // to give us flexible model. It can give smoothness, but can't give precision.
                // So we need more layers with smaller radii.
                rbfsetalgomultilayer(model, 5.0, 1, 1.0e-3);
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);
                v = rbfcalc2(model, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, -0.021690, 0.002);

                // Now we know that single layer is not enough. We still want to start with
                // R=5.0 because it has good smoothness properties, but we will add more layers,
                // each with R[i+1]=R[i]/2. We think that 4 layers is enough, because last layer
                // will have R = 5.0/2^3 = 5/8 ~ 0.63, which is smaller than the average distance
                // between points. And it works!
                rbfsetalgomultilayer(model, 5.0, 4, 1.0e-3);
                rbfbuildmodel(model, rep);
                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);
                v = rbfcalc2(model, 0.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.000000, 0.002);

                // BTW, if you look at v, you will see that it is equal to 0.9999999997, not to 1.
                // This small error can be fixed by adding one more layer.
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "rbf_d_ml_simple");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST rbf_d_ml_ls
        //      Least squares problem solved with RBF-ML algorithm
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<3; _spoil_scenario++)
        {
            try
            {
                //
                // This example shows how to solve least squares problems with RBF-ML algorithm.
                // Below we assume that you already know basic concepts shown in the RBF_D_QNN and
                // RBF_D_ML_SIMPLE examples.
                //
                rbfmodel model;
                rbfreport rep;
                double v;

                //
                // We have 2-dimensional space and very simple fitting problem - all points
                // except for two are well separated and located at straight line. Two
                // "exceptional" points are very close, with distance between them as small
                // as 0.01. RBF-QNN algorithm will have many difficulties with such distribution
                // of points:
                //     X        Y
                //     -2       1
                //     -1       0
                //     -0.005   1
                //     +0.005   2
                //     +1      -1
                //     +2       1
                // How will RBF-ML handle such problem?
                //
                rbfcreate(2, 1, model);
                real_2d_array xy0 = "[[-2,0,1],[-1,0,0],[-0.005,0,1],[+0.005,0,2],[+1,0,-1],[+2,0,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(xy0);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(xy0);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(xy0);
                rbfsetpoints(model, xy0);

                // First, we try to use R=5.0 with single layer (NLayers=1) and moderate amount
                // of regularization. Well, we already expected that results will be bad:
                //     Model(x=-2,y=0)=0.7635    (instead of 1.0)
                //     Model(x=0.005,y=0)=0.6577 (instead of 2.0)
                // We need more layers to show better results.
                rbfsetalgomultilayer(model, 5.0, 1, 1.0e-3);
                rbfbuildmodel(model, rep);
                v = rbfcalc2(model, -2.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.7635098170, 0.002);
                v = rbfcalc2(model, 0.005, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.6576751127, 0.002);

                // With 4 layers we got better result at x=-2 (point which is well separated
                // from its neighbors). Model is now many times closer to the original data
                //     Model(x=-2,y=0)=0.9986    (instead of 1.0)
                //     Model(x=0.005,y=0)=1.5996 (instead of 2.0)
                // We may see that at x=0.005 result is a bit closer to 2.0, but does not
                // reproduce function value precisely because of close neighbor located at
                // at x=-0.005. Let's add two layers...
                rbfsetalgomultilayer(model, 5.0, 4, 1.0e-3);
                rbfbuildmodel(model, rep);
                v = rbfcalc2(model, -2.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 0.9985538830, 0.002);
                v = rbfcalc2(model, 0.005, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.5996044344, 0.002);

                // With 6 layers we got almost perfect fit:
                //     Model(x=-2,y=0)=1.000    (perfect fit)
                //     Model(x=0.005,y=0)=1.999 (instead of 2.0)
                // Of course, we can reduce error at x=0.005 down to zero by adding more
                // layers. But do we really need it?
                rbfsetalgomultilayer(model, 5.0, 6, 1.0e-3);
                rbfbuildmodel(model, rep);
                v = rbfcalc2(model, -2.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.0000000000, 0.002);
                v = rbfcalc2(model, 0.005, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.9990824777, 0.002);

                // Do we really need zero error? We have f(+0.005)=2 and f(-0.005)=1.
                // Two points are very close, and in real life situations it often means
                // that difference in function values can be explained by noise in the
                // data. Thus, true value of the underlying function should be close to
                // 1.5 (halfway between 1.0 and 2.0).
                //
                // How can we get such result with RBF-ML? Well, we can:
                // a) reduce number of layers (make model less flexible)
                // b) increase regularization coefficient (another way of reducing flexibility)
                //
                // Having NLayers=5 and LambdaV=0.1 gives us good least squares fit to the data:
                //     Model(x=-2,y=0)=1.000
                //     Model(x=-0.005,y=0)=1.504
                //     Model(x=+0.005,y=0)=1.496
                rbfsetalgomultilayer(model, 5.0, 5, 1.0e-1);
                rbfbuildmodel(model, rep);
                v = rbfcalc2(model, -2.0, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.0000001620, 0.002);
                v = rbfcalc2(model, -0.005, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.5040840708, 0.002);
                v = rbfcalc2(model, 0.005, 0.0);
                _TestResult = _TestResult && doc_test_real(v, 1.4959158517, 0.002);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "rbf_d_ml_ls");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_d_1
        //      Determinant calculation, real matrix, short form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                real_2d_array b = "[[1,2],[2,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(b);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(b);
                double a;
                a = rmatrixdet(b);
                _TestResult = _TestResult && doc_test_real(a, -3, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_d_2
        //      Determinant calculation, real matrix, full form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                real_2d_array b = "[[5,4],[4,5]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(b);
                double a;
                a = rmatrixdet(b, 2);
                _TestResult = _TestResult && doc_test_real(a, 9, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_d_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_d_3
        //      Determinant calculation, complex matrix, short form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                complex_2d_array b = "[[1+1i,2],[2,1-1i]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(b);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(b);
                alglib::complex a;
                a = cmatrixdet(b);
                _TestResult = _TestResult && doc_test_complex(a, -2, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_d_3");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_d_4
        //      Determinant calculation, complex matrix, full form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                alglib::complex a;
                complex_2d_array b = "[[5i,4],[4i,5]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(b);
                a = cmatrixdet(b, 2);
                _TestResult = _TestResult && doc_test_complex(a, alglib::complex(0,9), 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_d_4");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_d_5
        //      Determinant calculation, complex matrix with zero imaginary part, short form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<7; _spoil_scenario++)
        {
            try
            {
                alglib::complex a;
                complex_2d_array b = "[[9,1],[2,1]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(b);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(b);
                a = cmatrixdet(b);
                _TestResult = _TestResult && doc_test_complex(a, 7, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_d_5");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_t_0
        //      Determinant calculation, real matrix, full form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                double a;
                real_2d_array b = "[[3,4],[-4,3]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(b);
                a = rmatrixdet(b, 2);
                _TestResult = _TestResult && doc_test_real(a, 25, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_t_0");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_t_1
        //      Determinant calculation, real matrix, LU, short form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<9; _spoil_scenario++)
        {
            try
            {
                double a;
                real_2d_array b = "[[1,2],[2,5]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(b);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(b);
                integer_1d_array p = "[1,1]";
                if( _spoil_scenario==7 )
                    spoil_vector_by_adding_element(p);
                if( _spoil_scenario==8 )
                    spoil_vector_by_deleting_element(p);
                a = rmatrixludet(b, p);
                _TestResult = _TestResult && doc_test_real(a, -5, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_t_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_t_2
        //      Determinant calculation, real matrix, LU, full form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                double a;
                real_2d_array b = "[[5,4],[4,5]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(b);
                integer_1d_array p = "[0,1]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_deleting_element(p);
                a = rmatrixludet(b, p, 2);
                _TestResult = _TestResult && doc_test_real(a, 25, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_t_2");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_t_3
        //      Determinant calculation, complex matrix, full form
        //
        printf("100/104\n");
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<5; _spoil_scenario++)
        {
            try
            {
                alglib::complex a;
                complex_2d_array b = "[[5i,4],[-4,5i]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(b);
                a = cmatrixdet(b, 2);
                _TestResult = _TestResult && doc_test_complex(a, -9, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_t_3");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_t_4
        //      Determinant calculation, complex matrix, LU, short form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<9; _spoil_scenario++)
        {
            try
            {
                alglib::complex a;
                complex_2d_array b = "[[1,2],[2,5i]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_adding_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_adding_col(b);
                if( _spoil_scenario==5 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==6 )
                    spoil_matrix_by_deleting_col(b);
                integer_1d_array p = "[1,1]";
                if( _spoil_scenario==7 )
                    spoil_vector_by_adding_element(p);
                if( _spoil_scenario==8 )
                    spoil_vector_by_deleting_element(p);
                a = cmatrixludet(b, p);
                _TestResult = _TestResult && doc_test_complex(a, alglib::complex(0,-5), 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_t_4");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST matdet_t_5
        //      Determinant calculation, complex matrix, LU, full form
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<6; _spoil_scenario++)
        {
            try
            {
                alglib::complex a;
                complex_2d_array b = "[[5,4i],[4,5]]";
                if( _spoil_scenario==0 )
                    spoil_matrix_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_matrix_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_matrix_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_matrix_by_deleting_row(b);
                if( _spoil_scenario==4 )
                    spoil_matrix_by_deleting_col(b);
                integer_1d_array p = "[0,1]";
                if( _spoil_scenario==5 )
                    spoil_vector_by_deleting_element(p);
                a = cmatrixludet(b, p, 2);
                _TestResult = _TestResult && doc_test_complex(a, 25, 0.0001);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "matdet_t_5");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        //
        // TEST lincg_d_1
        //      Solution of sparse linear systems with CG
        //
        _TestResult = true;
        for(_spoil_scenario=-1; _spoil_scenario<4; _spoil_scenario++)
        {
            try
            {
                //
                // This example illustrates solution of sparse linear systems with
                // conjugate gradient method.
                // 
                // Suppose that we have linear system A*x=b with sparse symmetric
                // positive definite A (represented by sparsematrix object)
                //         [ 5 1       ]
                //         [ 1 7 2     ]
                //     A = [   2 8 1   ]
                //         [     1 4 1 ]
                //         [       1 4 ]
                // and right part b
                //     [  7 ]
                //     [ 17 ]
                // b = [ 14 ]
                //     [ 10 ]
                //     [  6 ]
                // and we want to solve this system using sparse linear CG. In order
                // to do so, we have to create left part (sparsematrix object) and
                // right part (dense array).
                //
                // Initially, sparse matrix is created in the Hash-Table format,
                // which allows easy initialization, but do not allow matrix to be
                // used in the linear solvers. So after construction you should convert
                // sparse matrix to CRS format (one suited for linear operations).
                //
                // It is important to note that in our example we initialize full
                // matrix A, both lower and upper triangles. However, it is symmetric
                // and sparse solver needs just one half of the matrix. So you may
                // save about half of the space by filling only one of the triangles.
                //
                sparsematrix a;
                sparsecreate(5, 5, a);
                sparseset(a, 0, 0, 5.0);
                sparseset(a, 0, 1, 1.0);
                sparseset(a, 1, 0, 1.0);
                sparseset(a, 1, 1, 7.0);
                sparseset(a, 1, 2, 2.0);
                sparseset(a, 2, 1, 2.0);
                sparseset(a, 2, 2, 8.0);
                sparseset(a, 2, 3, 1.0);
                sparseset(a, 3, 2, 1.0);
                sparseset(a, 3, 3, 4.0);
                sparseset(a, 3, 4, 1.0);
                sparseset(a, 4, 3, 1.0);
                sparseset(a, 4, 4, 4.0);

                //
                // Now our matrix is fully initialized, but we have to do one more
                // step - convert it from Hash-Table format to CRS format (see
                // documentation on sparse matrices for more information about these
                // formats).
                //
                // If you omit this call, ALGLIB will generate exception on the first
                // attempt to use A in linear operations. 
                //
                sparseconverttocrs(a);

                //
                // Initialization of the right part
                //
                real_1d_array b = "[7,17,14,10,6]";
                if( _spoil_scenario==0 )
                    spoil_vector_by_nan(b);
                if( _spoil_scenario==1 )
                    spoil_vector_by_posinf(b);
                if( _spoil_scenario==2 )
                    spoil_vector_by_neginf(b);
                if( _spoil_scenario==3 )
                    spoil_vector_by_deleting_element(b);

                //
                // Now we have to create linear solver object and to use it for the
                // solution of the linear system.
                //
                // NOTE: lincgsolvesparse() accepts additional parameter which tells
                //       what triangle of the symmetric matrix should be used - upper
                //       or lower. Because we've filled both parts of the matrix, we
                //       can use any part - upper or lower.
                //
                lincgstate s;
                lincgreport rep;
                real_1d_array x;
                lincgcreate(5, s);
                lincgsolvesparse(s, a, true, b);
                lincgresults(s, x, rep);

                _TestResult = _TestResult && doc_test_int(rep.terminationtype, 1);
                _TestResult = _TestResult && doc_test_real_vector(x, "[1.000,2.000,1.000,2.000,1.000]", 0.005);
                _TestResult = _TestResult && (_spoil_scenario==-1);
            }
            catch(ap_error e)
            { _TestResult = _TestResult && (_spoil_scenario!=-1); }
            catch(...)
            { throw; }
        }
        if( !_TestResult)
        {
            printf("%-32s FAILED\n", "lincg_d_1");
            fflush(stdout);
        }
        _TotalResult = _TotalResult && _TestResult;


        printf("104/104\n");
    }
    catch(...)
    {
        printf("Unhandled exception was raised!\n");
        return 1;
    }
#ifdef AE_USE_ALLOC_COUNTER
    if( alglib_impl::_alloc_counter!=0 )
    {
        _TotalResult = false;
        printf("FAILURE: alloc_counter is non-zero on end!\n");
    }
#endif
    return _TotalResult ? 0 : 1;
}
