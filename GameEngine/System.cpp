#include "System.h"

SystemData::SystemData() 
:systemType(SystemType::Sys_Invalid) {

}

SystemData::SystemData(const SystemType& type) 
:systemType(type){

}

System::System(const SystemData& data)
:UObject()
, m_SystemType(data.systemType){
	setCanDraw(false);
}

System::~System()
{
}
