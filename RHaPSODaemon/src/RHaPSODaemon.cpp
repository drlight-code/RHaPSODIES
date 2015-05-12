#include <iostream>

#include <GL/freeglut.h>

#include <VistaBase/VistaTimeUtils.h>

#include <RHaPSODIES.hpp>
#include <HandTracker.hpp>
#include <DebugViewConsole.hpp>

#include "RHaPSODaemon.hpp"

namespace rhapsodies {
	RHaPSODaemon::RHaPSODaemon() :
		m_pTracker(NULL) {

		m_pFakeColorBuffer = new unsigned char[320*240*3];
		m_pFakeDepthBuffer = new unsigned short[320*240];
		m_pFakeUVMapBuffer = new float[320*240*2];
	}

	RHaPSODaemon::~RHaPSODaemon() {
		delete [] m_pFakeUVMapBuffer;
		delete [] m_pFakeDepthBuffer;
		delete [] m_pFakeColorBuffer;
		
		delete m_pDebugView;
		delete m_pTracker;
	}
	
	bool RHaPSODaemon::Initialize() {
		bool success = true;

		success &= InitGlut();
		success &= InitDeviceDriver();
		success &= InitHandTracker();

		return success;
	}
	
	bool RHaPSODaemon::Run() {
		const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();
		VistaType::microtime tStart = oTimer.GetMicroTime();

		m_pTracker->StartTracking();
		
		while(oTimer.GetMicroTime() - tStart < 2) {
			m_pTracker->FrameUpdate(
				m_pFakeColorBuffer,
				m_pFakeDepthBuffer,
				m_pFakeUVMapBuffer);

			glFinish();
		
			glutPostRedisplay();
			glutSwapBuffers();
		}
	}

	bool RHaPSODaemon::InitGlut() {
		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_CORE_PROFILE);

		int iDisplayMode =
			GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE |
			GLUT_BORDERLESS | GLUT_CAPTIONLESS;
		glutInitDisplayMode(iDisplayMode);

		glutInitWindowPosition(0, 0);
		glutInitWindowSize(1024, 768);
		int iWindowID = glutCreateWindow("RHaPSODaemon");
		glutSetWindow(iWindowID);

		return true;
	}
	
	bool RHaPSODaemon::InitDeviceDriver() {

		return true;
	}

	bool RHaPSODaemon::InitHandTracker() {
		RHaPSODIES::Initialize();

		m_pTracker = new HandTracker();
		m_pDebugView = new DebugViewConsole();

		m_pTracker->SetDebugView(m_pDebugView);
		
		return m_pTracker->Initialize();
	}

}
