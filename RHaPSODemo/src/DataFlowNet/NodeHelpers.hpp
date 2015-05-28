#ifndef _RHAPSODIES_NODEHELPERS
#define _RHAPSODIES_NODEHELPERS

namespace rhapsodies {
	template<class PortT> class sPortHlp {
	public:
		sPortHlp() : m_iUpdateCount(0) {}
			
		TVdfnPort<PortT> *m_pPort;
		unsigned int m_iUpdateCount;

		bool HasNewData() {
			if(!m_pPort)
				return false;
				
			if(m_pPort->GetUpdateCounter() > m_iUpdateCount) {
				m_iUpdateCount = m_pPort->GetUpdateCounter();
				return true;
			}
				
			return false;
		}
	};
}

#endif // _RHAPSODIES_NODEHELPERS
