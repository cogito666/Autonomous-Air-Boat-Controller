// PID.h

#ifndef _PID_h
#define _PID_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "MathUtils.h"
class PID{
public:
	PID(const String& name, const double& P, const double& D, const double& I=0.0, const double& iMax=1.0)
	:name(name){
		setP(P);
		setI(I);
		setIMax(iMax);
		setD(D);
		resetAll();
	}

	void setP(const double& P){
		m_P = P;
	}

	void setD(const double& D){
		m_D = D;
	}

	void setI(const double& I){
		m_I = I;
	}

	void setIMax(const double& iMax){
		m_imax = iMax;
	}
	
	const String& getName()const{
		return name;
	}

	const double& getP() const{
		return m_P;
	}

	const double& getD() const{
		return m_D;
	}

	const double& getI() const{
		return m_I;
	}

	const double& getIMax() const{
		return m_imax;
	}

	double update(const double& err, double dt){
		if (first_run){
			m_err = err;
			m_ierr += err;
			first_run = false;
		}
		else{
			if (fabs(dt) < 0.001) {
				dt = 0.0009999;
			}
			m_derr = (err - m_err) / dt;
			m_ierr += err;
			m_err = err;
		}
		if (fabs(m_ierr) > m_imax){
			m_ierr = sign(m_ierr) * m_imax;
		}
		return m_P * m_err + m_I * m_ierr + m_D * m_derr;
	}

	void resetITerm(const double& val){
		m_ierr = val;
	}

	void resetAll(){
		m_err = 0;
		m_derr = 0;
		m_ierr = 0;
		first_run = true;
	}
protected:
	double m_P, m_I, m_D;
	double m_err, m_derr, m_ierr;
	double m_imax;
	String name;
	bool first_run;
};

#endif

